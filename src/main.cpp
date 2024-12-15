#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf_driver;

// Definição dos pinos
#define MOTOR_PIN_ENA 5
#define MOTOR_PIN_1 7
#define MOTOR_PIN_2 8

int counter = 0;            // Variável que varia de 0 a 255
const int step = 15;        // Incremento/decremento do COUNTER
const int maxSpeed = 150;   // Velocidade máxima (PWM)
unsigned long lastTime = 0; // Controle de tempo
const int interval = 500;   // Intervalo de meio segundo
bool accelerating = false;  // Estado de aceleração
bool braking = false;       // Estado de frenagem

struct Command {
  uint8_t button;
  uint8_t action;
};

void setup() {
  Serial.begin(9600);
  
  rf_driver.init();
  Serial.println("Receptor iniciado...");
  
  pinMode(MOTOR_PIN_ENA, OUTPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);

  // Inicializa o motor como desligado
  digitalWrite(MOTOR_PIN_1, LOW); // motor parado
  digitalWrite(MOTOR_PIN_2, LOW); // motor parado
  analogWrite(MOTOR_PIN_ENA, 0);  // velocidade 0
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    Command cmd;
    memcpy(&cmd, buf, sizeof(cmd));

    // Processa os comandos recebidos
    if (cmd.action == 1) {
      if (cmd.button == 1) {
        accelerating = true;
        braking = false;
      } else if (cmd.button == 2) {
        braking = true;
      }
    } else {
      accelerating = false;
    }
  }

  if (millis() - lastTime >= interval) {
    lastTime = millis();
    
    if (accelerating) {
      if (counter < maxSpeed) {
        counter += step; // Incrementa a velocidade
      }
    } else {
      if (counter > 0) {
        counter -= step; // Decrementa a velocidade
      }
    }

    if (braking) {
      counter = 0;
    }

    if (counter != 0) {
      digitalWrite(MOTOR_PIN_1, HIGH);
      digitalWrite(MOTOR_PIN_2, LOW); // Verificar necessidade
      Serial.print("Velocidade: ");
      Serial.println(counter);
    }

    analogWrite(MOTOR_PIN_ENA, counter); // Atualiza a velocidade do motor
  }
}
