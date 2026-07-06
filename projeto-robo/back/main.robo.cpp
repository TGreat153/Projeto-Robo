#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "soc/soc.h"             // Pra mexer nas configs da placa
#include "soc/rtc_cntl_reg.h"    // Pra conseguir desligar o aviso de energia fraca

// ---------------------------------------------------------
// Configuração do Wi-Fi do Robô
// A gente mudou pro robô criar o próprio Wi-Fi (Modo AP)
// O IP fixo pra gente conectar no Python vai ser sempre 192.168.4.1
// ---------------------------------------------------------
const char* ssid = "Robo_Joao";
const char* password = "123465789"; 

// ---------------------------------------------------------
// Pinos da Ponte H (Ligados nos motores)
// ---------------------------------------------------------
#define PINO_VEL_ESQ 2        // Pino PWM pra controlar a velocidade da roda esquerda
#define PINO_ESQ_FRENTE 13    // Gira esquerda pra frente
#define PINO_ESQ_TRAS 14      // Gira esquerda pra trás

#define PINO_VEL_DIR 12       // Pino PWM pra controlar a velocidade da roda direita
#define PINO_DIR_FRENTE 15    // Gira direita pra frente
#define PINO_DIR_TRAS 33      // Gira direita pra trás

// ---------------------------------------------------------
// Config do PWM (Pra velocidade não ficar só no máximo/mínimo)
// ---------------------------------------------------------
#define FREQUENCIA_PWM 1000   
#define RESOLUCAO_PWM 8       // Velocidade vai de 0 até 255
#define CANAL_PWM_ESQ 0
#define CANAL_PWM_DIR 1

// ---------------------------------------------------------
// Pinos da Câmera
// ---------------------------------------------------------
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

httpd_handle_t servidor = NULL; 

// ---------------------------------------------------------
// FUNÇÕES DE MOVIMENTO
// ---------------------------------------------------------

void parar_motores() {
  ledcWrite(CANAL_PWM_ESQ, 0); // Velocidade zero
  ledcWrite(CANAL_PWM_DIR, 0);
  digitalWrite(PINO_ESQ_FRENTE, LOW); 
  digitalWrite(PINO_ESQ_TRAS, LOW);
  digitalWrite(PINO_DIR_FRENTE, LOW);
  digitalWrite(PINO_DIR_TRAS, LOW);
}

void andar_frente(int velocidad) {
  digitalWrite(PINO_ESQ_FRENTE, HIGH);
  digitalWrite(PINO_ESQ_TRAS, LOW);
  digitalWrite(PINO_DIR_FRENTE, HIGH);
  digitalWrite(PINO_DIR_TRAS, LOW);
  
  // O motor esquerdo tava mais rápido e o robô entortava
  // Então multipliquei por 0.85 pra calibrar e ele andar reto
  int velocidade_calibrada_esq = velocidad * 0.85; 
  ledcWrite(CANAL_PWM_ESQ, velocidade_calibrada_esq);
  ledcWrite(CANAL_PWM_DIR, velocidad);
}

void andar_tras(int velocidade) {
  digitalWrite(PINO_ESQ_FRENTE, LOW);
  digitalWrite(PINO_ESQ_TRAS, HIGH);
  digitalWrite(PINO_DIR_FRENTE, LOW);
  digitalWrite(PINO_DIR_TRAS, HIGH);
  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}

void girar_esquerda(int velocidade) {
  // Gira uma roda pra frente e outra pra trás pro robô girar no próprio eixo
  digitalWrite(PINO_ESQ_FRENTE, LOW);
  digitalWrite(PINO_ESQ_TRAS, HIGH);
  digitalWrite(PINO_DIR_FRENTE, HIGH);
  digitalWrite(PINO_DIR_TRAS, LOW);
  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}

void girar_direita(int velocidade) {
  digitalWrite(PINO_ESQ_FRENTE, HIGH);
  digitalWrite(PINO_ESQ_TRAS, LOW);
  digitalWrite(PINO_DIR_FRENTE, LOW);
  digitalWrite(PINO_DIR_TRAS, HIGH);
  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}

// ---------------------------------------------------------
// COMANDOS RECEBIDOS PELO SITE/PYTHON
// ---------------------------------------------------------

esp_err_t controle_handler(httpd_req_t *req) {
  char* rota = (char*)req->uri;
  
  // Lê qual link foi acessado (ex: /frente) e chama a função certa
  if (strstr(rota, "/frente")) andar_frente(140);
  else if (strstr(rota, "/tras")) andar_tras(140);
  else if (strstr(rota, "/esquerda")) girar_esquerda(130);
  else if (strstr(rota, "/direita")) girar_direita(130);
  else if (strstr(rota, "/parar")) parar_motores();
  
  // Responde "OK" pro navegador não ficar carregando infinitamente
  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, "OK", 2);
}

// Códigos obrigatórios pro formato de vídeo MJPEG
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// Puxa as imagens da câmera e joga na rede (Streaming)
esp_err_t video_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK) return res;

  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Erro ao puxar a imagem");
      res = ESP_FAIL;
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }
    
    // Manda os dados da foto pro navegador
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    
    // Limpa a memória da placa pra não travar depois de alguns minutos
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK) break;
  }
  return res;
}

// ---------------------------------------------------------
// INICIALIZAÇÃO
// ---------------------------------------------------------
void setup() {
  // A placa tava reiniciando sozinha quando o Wi-Fi ligava (pico de energia)
  // Esse comando desliga o aviso de Brownout pra forçar ela a ligar mesmo assim
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  
  Serial.begin(115200);

  // 1. Config dos motores
  pinMode(PINO_ESQ_FRENTE, OUTPUT);
  pinMode(PINO_ESQ_TRAS, OUTPUT);
  pinMode(PINO_DIR_FRENTE, OUTPUT);
  pinMode(PINO_DIR_TRAS, OUTPUT);

  // Liga os pinos no PWM do ESP32
  ledcSetup(CANAL_PWM_ESQ, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_ESQ, CANAL_PWM_ESQ);
  ledcSetup(CANAL_PWM_DIR, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_DIR, CANAL_PWM_DIR);
  parar_motores();

  // 2. Config da Câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_2;
  config.ledc_timer = LEDC_TIMER_1;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Resolução 320x240 (QVGA) pra imagem ficar leve e o vídeo não travar
  config.frame_size = FRAMESIZE_QVGA; 
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if(esp_camera_init(&config) != ESP_OK){
    Serial.println("Aviso: Falha na câmera.");
    // Eu comentei o 'return;' aqui porque quando dava erro na câmera 
    // ele parava o código inteiro e nem criava o Wi-Fi pra gente testar.
  }

  sensor_t * s = esp_camera_sensor_get();
  // Se a câmera ligou certinho, ajusta pra imagem não ficar invertida
  if (s != NULL) { 
    s->set_vflip(s, 0);   
    s->set_hmirror(s, 0); 
  }

  // 3. Ligando a Rede Wi-Fi
  Serial.println("\nCriando o Wi-Fi do robô...");
  WiFi.softAP(ssid, password); 
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Wi-Fi pronto! Conecte na rede: ");
  Serial.println(ssid);
  Serial.print("IP Fixo pra colocar no site/Python: ");
  Serial.println(IP);

  // 4. Configurando os links do servidor web (Porta 80)
  httpd_config_t config_server = HTTPD_DEFAULT_CONFIG();
  config_server.server_port = 80;

  // Associa cada link a uma função do robô
  httpd_uri_t url_video = { .uri = "/stream", .method = HTTP_GET, .handler = video_handler, .user_ctx = NULL };
  httpd_uri_t url_frente = { .uri = "/frente", .method = HTTP_GET, .handler = controle_handler, .user_ctx = NULL };
  httpd_uri_t url_tras = { .uri = "/tras", .method = HTTP_GET, .handler = controle_handler, .user_ctx = NULL };
  httpd_uri_t url_esq = { .uri = "/esquerda", .method = HTTP_GET, .handler = controle_handler, .user_ctx = NULL };
  httpd_uri_t url_dir = { .uri = "/direita", .method = HTTP_GET, .handler = controle_handler, .user_ctx = NULL };
  httpd_uri_t url_parar = { .uri = "/parar", .method = HTTP_GET, .handler = controle_handler, .user_ctx = NULL };

  if (httpd_start(&servidor, &config_server) == ESP_OK) {
    httpd_register_uri_handler(servidor, &url_video);
    httpd_register_uri_handler(servidor, &url_frente);
    httpd_register_uri_handler(servidor, &url_tras);
    httpd_register_uri_handler(servidor, &url_esq);
    httpd_register_uri_handler(servidor, &url_dir);
    httpd_register_uri_handler(servidor, &url_parar);
  }
}

void loop() {
  // Fica parado aqui. A câmera e a rede funcionam sozinhas em segundo plano.
  delay(10000); 
}
