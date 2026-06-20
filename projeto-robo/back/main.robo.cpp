#include <Arduino.h>
#include <WiFi.h> // lib do wifi

// --- Config rede ---
const char* WiFi_SSID = "robo FP";
const char* WiFi_PASS = "robo123456";

// Server na porta 80
WiFiServer servidor(80);

// --- Pinos Ponte H ---
// Esquerda
#define PINO_VEL_ESQ 2 
#define PINO_ESQ_FRENTE 13    
#define PINO_ESQ_TRAS 14      

// Direita
#define PINO_VEL_DIR 12
#define PINO_DIR_FRENTE 15
#define PINO_DIR_TRAS 33

// --- Config PWM ---
#define FREQUENCIA_PWM 1000
#define RESOLUCAO_PWM 8
#define CANAL_PWM_ESQ 0
#define CANAL_PWM_DIR 1


// --- Funcoes de Movimento ---
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

  // calibracao: reduz motor esquerdo em ~15% pra corrigir desvio do chassi
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


// --- Setup ---
void setup() {
  // serial pra pegar o ip no monitor
  Serial.begin(115200);

  pinMode(PINO_ESQ_FRENTE, OUTPUT);
  pinMode(PINO_ESQ_TRAS, OUTPUT);
  pinMode(PINO_DIR_FRENTE, OUTPUT);
  pinMode(PINO_DIR_TRAS, OUTPUT);

  ledcSetup(CANAL_PWM_ESQ, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_ESQ, CANAL_PWM_ESQ);

  ledcSetup(CANAL_PWM_DIR, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_DIR, CANAL_PWM_DIR);

  parar_motores();

  // conecta wifi
  Serial.print("Conectando em: ");
  Serial.println(WiFi_SSID);
  WiFi.begin(WiFi_SSID, WiFi_PASS);

  // trava aqui ate conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // mostra ip pra equipe
  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  servidor.begin();
}


// --- Loop Principal ---
void loop() {
  // checa se tem cliente mandando requisicao
  WiFiClient cliente = servidor.available();

  if (cliente) {
    String linha_requisicao = "";

    while (cliente.connected()) {
      if (cliente.available()) {
        char c = cliente.read();
        linha_requisicao += c;

        // quebra de linha indica fim do request
        if (c == '\n') {
          if (linha_requisicao.length() == 1) {
            // header http padrao pro python nao dar timeout
            cliente.println("HTTP/1.1 200 OK");
            cliente.println("Content-Type: text/plain");
            cliente.println("Connection: close");
            cliente.println();
            cliente.println("OK");
            break;
          }

          // rotas de controle (forca no maximo)
          if (linha_requisicao.indexOf("GET /frente") >= 0) {
            andar_frente(255);
          } 
          else if (linha_requisicao.indexOf("GET /tras") >= 0) {
            andar_tras(255);
          } 
          else if (linha_requisicao.indexOf("GET /esquerda") >= 0) {
            girar_esquerda(255);
          } 
          else if (linha_requisicao.indexOf("GET /direita") >= 0) {
            girar_direita(255);
          } 
          else if (linha_requisicao.indexOf("GET /parar") >= 0) {
            parar_motores();
          }

          linha_requisicao = "";
        }
      }
    }
    // derruba a conexao pra liberar o loop pro proximo comando
    cliente.stop();
  }
}