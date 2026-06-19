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

// ==========================================
// Parte 3: AS FUNÇÕES DE MOVIMENTO 
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
// ==========================================
// parte 4: A FUNÇÃO SETUP 
// ==========================================

void setup() {
  // 1. Configurando os pinos de direção como SAÍDA de energia
  pinMode(PINO_ESQ_FRENTE, OUTPUT);
  pinMode(PINO_ESQ_TRAS, OUTPUT);
  pinMode(PINO_DIR_FRENTE, OUTPUT);
  pinMode(PINO_DIR_TRAS, OUTPUT);

  // 2. Ligando o motor do PWM Esquerdo
  // Primeiro, configuramos o canal com as nossas regras matemáticas
  ledcSetup(CANAL_PWM_ESQ, FREQUENCIA_PWM, RESOLUCAO_PWM);
  // Depois, "plugamos" esse canal no pino físico do ESP32
  ledcAttachPin(PINO_VEL_ESQ, CANAL_PWM_ESQ);

  // 3. Ligando o motor do PWM Direito
  ledcSetup(CANAL_PWM_DIR, FREQUENCIA_PWM, RESOLUCAO_PWM);
  ledcAttachPin(PINO_VEL_DIR, CANAL_PWM_DIR);

  // 4. Medida de segurança: garante que o robô ligue totalmente parado
  parar_motores();
}
// ==========================================
// Parte  5: O CICLO PRINCIPAL 
// ==========================================

void loop() {

  // 1. Acelera para frente com velocidade média (150) por 2 segundos
  andar_frente(150);
  delay(2000); // delay em C++ é medido em milissegundos (2000 = 2 segundos)

  // 2. Freia e respira por 1 segundo
  parar_motores();
  delay(1000);

  // 3. Gira no próprio eixo para a direita (velocidade 120) por 1 segundo
  girar_direita(120);
  delay(1000);

  // 4. Freia por 2 segundos antes de recomeçar o ciclo
  parar_motores();
  delay(2000);
}