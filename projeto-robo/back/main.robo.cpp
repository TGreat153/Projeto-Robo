#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "soc/soc.h"             
#include "soc/rtc_cntl_reg.h"    

// ---------------------------------------------------------
// Configuração de Rede (Modo AP)
// ---------------------------------------------------------
const char* ssid = "Robo_Joao";
const char* password = "123465789"; 

// ---------------------------------------------------------
// Mapeamento de Pinos - Ponte H
// ---------------------------------------------------------
#define PINO_VEL_ESQ 2        
#define PINO_ESQ_FRENTE 13    
#define PINO_ESQ_TRAS 14      

#define PINO_VEL_DIR 12       
#define PINO_DIR_FRENTE 15    
#define PINO_DIR_TRAS 33      

// ---------------------------------------------------------
// Configuração do PWM dos Motores
// ---------------------------------------------------------
#define FREQUENCIA_PWM 1000   
#define RESOLUCAO_PWM 8       
#define CANAL_PWM_ESQ 0
#define CANAL_PWM_DIR 1

// ---------------------------------------------------------
// Mapeamento de Pinos - Câmera 
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
// Lógica de Movimentação do Chassi
// ---------------------------------------------------------

void parar_motores() {
  ledcWrite(CANAL_PWM_ESQ, 0); 
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
  
  // Multiplicador de 0.85 para compensar o desvio mecânico do motor esquerdo
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
  // Giro no próprio eixo (estilo tanque)
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
// Rotas do Servidor HTTP (Comandos de Movimento)
// ---------------------------------------------------------

esp_err_t controle_handler(httpd_req_t *req) {
  char* rota = (char*)req->uri;
  
  // Velocidade fixada em 150 para manter estabilidade na pista
  if (strstr(rota, "/frente")) andar_frente(150);
  else if (strstr(rota, "/tras")) andar_tras(150);
  else if (strstr(rota, "/esquerda")) girar_esquerda(150);
  else if (strstr(rota, "/direita")) girar_direita(150);
  else if (strstr(rota, "/parar")) parar_motores();
  
  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, "OK", 2);
}

// ---------------------------------------------------------
// Servidor de Streaming de Vídeo (MJPEG)
// ---------------------------------------------------------

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

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
      Serial.println("Erro ao capturar frame");
      res = ESP_FAIL;
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }
    
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
    
    // Liberação do buffer para evitar memory leak
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
// INICIALIZAÇÃO (Setup)
// ---------------------------------------------------------
void setup() {
  // Desativa o Brownout detector para evitar reboot no pico de energia do Wi-Fi
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  
  Serial.begin(115200);

  // 1. Configuração dos Pinos da Ponte H
  pinMode(PINO_ESQ_FRENTE, OUTPUT);
  pinMode(PINO_ESQ_TRAS, OUTPUT);
  pinMode(PINO_DIR_FRENTE, OUTPUT);
  pinMode(PINO_DIR_TRAS, OUTPUT);

  ledcSetup(CANAL_PWM_ESQ, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_ESQ, CANAL_PWM_ESQ);
  ledcSetup(CANAL_PWM_DIR, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_DIR, CANAL_PWM_DIR);
  parar_motores();

  // 2. Configuração da Câmera
  camera_config_t config;
  
  // Usa o Canal 2 para a câmera não conflitar com o PWM do motor (Canal 0)
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // Utiliza PSRAM se disponível para suportar o buffer do streaming
  if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  if(esp_camera_init(&config) != ESP_OK){
    Serial.println("Erro: Falha na inicialização da câmera.");
  }

  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) { 
    // Resolução travada em QVGA para o OpenCV processar rápido
    s->set_framesize(s, FRAMESIZE_QVGA);
    
    // Ajuste de orientação do sensor físico
    s->set_vflip(s, 1);   
    s->set_hmirror(s, 1); 
  }

  // 3. Inicialização do Wi-Fi (Modo AP)
  Serial.println("\nIniciando AP...");
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password); 
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(IP);

  // 4. Mapeamento de Endpoints do Servidor HTTP (Porta 80)
  httpd_config_t config_server = HTTPD_DEFAULT_CONFIG();
  config_server.server_port = 80;

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
  // Tarefas rodam de forma assíncrona
  delay(10000); 
}