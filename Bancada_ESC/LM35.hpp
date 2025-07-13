#pragma once
#include <stdint.h>

#include "Sensor.hpp"
#include <Arduino.h>

class LM35 : public Sensor{
	
public:
	LM35(const uint8_t PIN_An, const String Label);
	~LM35() override = default;

	float	read()		override;
private:
	uint8_t _PIN_An;
	String _Label;

	float scale;
	float readingVolts;
	float temperatureCelsius = 0;
	float voltDrop;
};