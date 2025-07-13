#pragma once
#include <stdint.h>
#include <Arduino.h>

#include "Sensor.hpp"

class VoltDiv : public Sensor{
public:
	VoltDiv(const uint8_t AnPin, const String Label);
	~VoltDiv() override = default;
	
	float read() override;	
private:
	uint8_t _AnPin;
	String _Label;
	
	uint16_t raw = 0;
	float value = 0;
	float scale;
	//float offset;
};