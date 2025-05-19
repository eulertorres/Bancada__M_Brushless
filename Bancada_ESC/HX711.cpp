// --- Configuração de pinos e parâmetros ---
#include "HX711.hpp"

#define HX711_DOUT_PIN  3   // conecta ao DOUT do módulo
#define HX711_SCK_PIN   2   // conecta ao SCK do módulo
#define GAIN_PULSES     1   // 1 pulso = canal A ganho 128

long offset = 0;      // “tare” zero
float scale  = 1.0;   // fator de calibração (raw/gramas)

// --- Função de inicialização ---
void setup() {
  Serial.begin(9600);
  pinMode(HX711_SCK_PIN, OUTPUT);
  pinMode(HX711_DOUT_PIN, INPUT);
  digitalWrite(HX711_SCK_PIN, LOW);

  // calibra offset com célula vazia
  Serial.println("Tare: calibrando zero sem carga...");
  offset = readRawAverage(10);
  Serial.print("Offset ajustado para: ");
  Serial.println(offset);
  Serial.println("Pronto para medições.");
}

// --- Loop principal ---
void loop() {
  // lê valor bruto médio e converte em peso
  long raw  = readRawAverage(10);
  float w   = (raw - offset) / scale;

  Serial.print("Bruto: ");
  Serial.print(raw);
  Serial.print("    Peso: ");
  Serial.print(w, 2);
  Serial.println(" unidades");
  delay(500);
}

// --- Lê uma única conversão de 24 bits e aplica seleção de ganho ---
long readRaw() {
  // aguarda DOUT ficar LOW (dado pronto)
  while (digitalRead(HX711_DOUT_PIN) == HIGH);

  unsigned long value = 0;
  // lê 24 bits
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(HX711_SCK_PIN, HIGH);
    value <<= 1;
    if (digitalRead(HX711_DOUT_PIN)) {
      value |= 1;
    }
    digitalWrite(HX711_SCK_PIN, LOW);
  }
  // pulsos adicionais para ganho
  for (uint8_t i = 0; i < GAIN_PULSES; i++) {
    digitalWrite(HX711_SCK_PIN, HIGH);
    digitalWrite(HX711_SCK_PIN, LOW);
  }
  // converte de 24 bits two’s-complement para signed long
  if (value & 0x800000) {
    value |= 0xFF000000;
  }
  return (long)value;
}

// --- Lê várias vezes e retorna a média ---
long readRawAverage(uint8_t times) {
  long sum = 0;
  for (uint8_t i = 0; i < times; i++) {
    sum += readRaw();
  }
  return sum / times;
}
