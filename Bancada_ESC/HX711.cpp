/*
 * CÓDIGO PARA BANCADA DE CARACTERIZAÇÃO DE GRUPO MOTOPROPULSOR DE DRONES
 * Versão: 1.0
 * Data: 07/2025
 * Autor: Euler Torres
 * * Código para leitura de força em g.f usando uma classe genérica de Sensor.
 *
 */
#include "HX711.hpp"

HX711::HX711(const uint8_t PIN_DOUT, const uint8_t PIN_SCK, const String Label) :
	Sensor(Label),
	_PIN_DOUT(PIN_DOUT),
	_PIN_SCK(PIN_SCK),
	_Label(Label),
	offset(1974.0),
	scale(114.6f)
{
	pinMode(_PIN_DOUT, INPUT);
	pinMode(_PIN_SCK, OUTPUT);
	digitalWrite(_PIN_SCK, HIGH);	//Modo Power-Down do HX711
}

// --- Lê uma única conversão de 24 bits e aplica seleção de ganho ---
int32_t HX711::readRaw() {
	// aguarda DOUT ficar LOW (dado pronto)
	while (digitalRead(_PIN_DOUT) == HIGH);
	
	value = 0;
	// lê 24 bits
	for (uint8_t i = 0; i < 24; i++) {
		digitalWrite(_PIN_SCK, HIGH);
		value <<= 1;
		if (digitalRead(_PIN_DOUT)) {
			value |= 1;
		}
		digitalWrite(_PIN_SCK, LOW);
	}
	// pulsos adicionais para ganho
	for (uint8_t i = 0; i < GAIN_PULSES; i++) {
		digitalWrite(_PIN_SCK, HIGH);
		digitalWrite(_PIN_SCK, LOW);
	}
	//Serial.println(value);
	// converte de 24 bits two’s-complement para signed long
	if (value & 0x800000) {
		value |= 0xFF000000;
	}
	return (int32_t)value;
}

// --- Lê várias vezes e retorna a média ---
float HX711::read() {
	//digitalWrite(4, HIGH);
	sum = 0;
	for (uint8_t i = 0; i < N_amostras; i++) {
		sum += readRaw();
	}
	return ((float)sum / N_amostras)/scale - offset;
}

float HX711::calibra(){
	digitalWrite(_PIN_SCK, LOW); //Modo Wake-up do HX711
	for (uint8_t i = 0; i < 20; i++) {
		sum += readRaw();
	}
	digitalWrite(_PIN_SCK, HIGH); //Modo Power-Down do HX711
	offset = ((float)sum / 20)/scale;
	//Serial.print("Novo offset: "); Serial.println(offset); 
}
