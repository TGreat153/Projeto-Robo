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