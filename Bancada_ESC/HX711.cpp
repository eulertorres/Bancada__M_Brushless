// --- Configuração de pinos e parâmetros ---
#include "HX711.hpp"

HX711::HX711(const uint8_t PIN_DOUT, const uint8_t PIN_SCK) :
	_PIN_DOUT(PIN_DOUT),
	_PIN_SCK(PIN_SCK),
	offset(1974),
	scale(114.6f)
{
	pinMode(_PIN_DOUT, INPUT);
	pinMode(_PIN_SCK, OUTPUT);
	digitalWrite(_PIN_SCK, LOW);	
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
	// converte de 24 bits two’s-complement para signed long
	if (value & 0x800000) {
		value |= 0xFF000000;
	}
	return (int32_t)value;
}

// --- Lê várias vezes e retorna a média ---
int16_t HX711::read() {
	sum = 0;
	for (uint8_t i = 0; i < N_amostras; i++) {
		sum += readRaw();
	}
	return (sum / N_amostras)/scale - offset;
}

long HX711::calibra(){
	offset = read();
	return offset;
}
