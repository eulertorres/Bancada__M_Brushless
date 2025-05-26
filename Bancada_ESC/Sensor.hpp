#pragma once
#include <stdint.h>

class Sensor{
public:
	virtual ~Sensor() = default;
	
	virtual long calibra() = 0;
	virtual int16_t read() = 0;
};