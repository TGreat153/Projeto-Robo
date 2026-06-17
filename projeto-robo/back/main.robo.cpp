#include <Arduino.h>

// ==========================================
// parte 1: DECLARAÇÃO DOS PINOS (PONTE H)
// ==========================================

// --- Motores do Lado Esquerdo ---
// O pino 2 vai no ENA da Ponte H para controlar a velocidade da esquerda
#define PINO_VEL_ESQ 2 
// Os pinos 13 e 14 decidem se a roda esquerda gira para frente ou para trás
#define PINO_ESQ_FRENTE 13    
#define PINO_ESQ_TRAS 14      

// --- Motores do Lado Direito ---
// O pino 12 vai no ENB da Ponte H para controlar a velocidade da direita
#define PINO_VEL_DIR 12
// Os pinos 15 e 33 decidem se a roda direita gira para frente ou para trás
#define PINO_DIR_FRENTE 15
#define PINO_DIR_TRAS 33
// ==========================================
// parte 2: CONFIGURAÇÕES DE VELOCIDADE (PWM)
// ==========================================

// 1. Frequência do PWM (Velocidade do pulso)
// 1000 Hz significa que a placa "pisca" a energia 1000 vezes por segundo.
// Usamos esse valor padrão porque faz o motor rodar macio e sem fazer zumbido.
#define FREQUENCIA_PWM 1000

// 2. Resolução (O limite do acelerador)
// Usando 8 bits, a nossa velocidade vai ser um número de 0 a 255.
// Sendo 0 o robô totalmente parado, e 255 a velocidade máxima.
#define RESOLUCAO_PWM 8

// 3. Canais do ESP32
// O ESP32 precisa de canais separados para enviar o sinal de velocidade.
// Deixamos o canal 0 para a esquerda e o canal 1 para a direita.
#define CANAL_PWM_ESQ 0
#define CANAL_PWM_DIR 1
//parte 3
// ==========================================
// ZONA 3: AS FUNÇÕES DE MOVIMENTO (OS MÚSCULOS)
// ==========================================

// Função para parar totalmente o robô (Freio)
void parar_motores() {
  // 1. Tira o pé do acelerador (Velocidade zero)
  ledcWrite(CANAL_PWM_ESQ, 0);
  ledcWrite(CANAL_PWM_DIR, 0);

  // 2. Corta a energia da direção da roda esquerda (LOW = Desligado)
  digitalWrite(PINO_ESQ_FRENTE, LOW);
  digitalWrite(PINO_ESQ_TRAS, LOW);

  // 3. Corta a energia da direção da roda direita (LOW = Desligado)
  digitalWrite(PINO_DIR_FRENTE, LOW);
  digitalWrite(PINO_DIR_TRAS, LOW);
}
// Função para o robô andar para frente com velocidade controlada
// A "velocidade" deve ser um número entre 0 e 255
void andar_frente(int velocidade) {
  // 1. Configura as rodas esquerdas para girar para frente
  digitalWrite(PINO_ESQ_FRENTE, HIGH);
  digitalWrite(PINO_ESQ_TRAS, LOW);

  // 2. Configura as rodas direitas para girar para frente
  digitalWrite(PINO_DIR_FRENTE, HIGH);
  digitalWrite(PINO_DIR_TRAS, LOW);

  // 3. Aplica a potência do motor (aciona o PWM)
  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}
// Função para o robô dar ré
void andar_tras(int velocidade) {
  // Inverte a polaridade: Pinos de trás ligados, pinos da frente desligados
  digitalWrite(PINO_ESQ_FRENTE, LOW);
  digitalWrite(PINO_ESQ_TRAS, HIGH);
  
  digitalWrite(PINO_DIR_FRENTE, LOW);
  digitalWrite(PINO_DIR_TRAS, HIGH);

  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}

// Função para virar à esquerda (Giro no próprio eixo)
void girar_esquerda(int velocidade) {
  // Roda esquerda vai para trás, roda direita vai para frente
  digitalWrite(PINO_ESQ_FRENTE, LOW);
  digitalWrite(PINO_ESQ_TRAS, HIGH);
  
  digitalWrite(PINO_DIR_FRENTE, HIGH);
  digitalWrite(PINO_DIR_TRAS, LOW);

  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}

// Função para virar à direita (Giro no próprio eixo)
void girar_direita(int velocidade) {
  // Roda esquerda vai para frente, roda direita vai para trás
  digitalWrite(PINO_ESQ_FRENTE, HIGH);
  digitalWrite(PINO_ESQ_TRAS, LOW);
  
  digitalWrite(PINO_DIR_FRENTE, LOW);
  digitalWrite(PINO_DIR_TRAS, HIGH);

  ledcWrite(CANAL_PWM_ESQ, velocidade);
  ledcWrite(CANAL_PWM_DIR, velocidade);
}
