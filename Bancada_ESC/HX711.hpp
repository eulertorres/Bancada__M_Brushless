#pragma once
#include <stdint.h>

#include "Sensor.hpp"
#include "Arduino.h"

#define HX711_DOUT_PIN  3   // conecta ao DOUT do módulo
#define HX711_SCK_PIN   2   // conecta ao SCK do módulo
#define GAIN_PULSES     1   // 1 pulso = canal A ganho 128

class HX711 : public Sensor{
	
public:
	HX711(const uint8_t, const uint8_t);	//Dout , SCK 
	~HX711() = default;
	int32_t read() override;
	void sample();

private:
	long offset = 0;      // “tare” zero
	float scale  = 1.0;   // fator de calibração (raw/gramas)
};