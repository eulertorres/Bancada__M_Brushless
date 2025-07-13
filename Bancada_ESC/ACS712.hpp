#pragma once

#include<stdint.h>
#include<Arduino.h>

#include "sensor.hpp"

class ACS712 : public Sensor{
public:
	ACS712(const uint8_t AnPin, const String Label);
	~ACS712() override = default;
	
	float read() override;
	
private:	
	uint8_t _AnPin;
	String _Label;
	
	uint16_t raw = 0;
	float value=0;
	float scale;
	float offset;
};