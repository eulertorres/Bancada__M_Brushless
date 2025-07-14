#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.print("Inicializando SD...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Falha!");
    return;
  }

  Serial.println("Cartão iniciado com sucesso.");
}

void loop() {
}
