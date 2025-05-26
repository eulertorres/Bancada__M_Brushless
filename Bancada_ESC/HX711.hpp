#pragma once
#include <stdint.h>

#include "Sensor.hpp"
#include "Arduino.h"

#define GAIN_PULSES     1   // 1 pulso = canal A ganho 128
#define N_amostras      10  // #Amostras para média

class HX711 : public Sensor{
	
public:
	HX711(const uint8_t PIN_DOUT, const uint8_t PIN_CLK);	//Dout , SCK 
	~HX711() override = default;
	
	int16_t read() override;
	long	calibra() override;
private:
	uint8_t	_PIN_DOUT;
	uint8_t _PIN_SCK;
	
	unsigned long	value = 0;	// Valor raw unitario
	int32_t			sum	= 0;			// Soma de amostras
	long 			offset;      		// “tare” zero
	float		 	scale;   		// fator de calibração (raw/gramas)
	
	int32_t tare();
	int32_t readRaw();
};