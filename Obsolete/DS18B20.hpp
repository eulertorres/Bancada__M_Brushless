#pragma once

#include "Sensor.hpp"
#include <DallasTemperature.h> // no need to include OneWire here

class DS18B20 : public Sensor {
public:
    DS18B20(DallasTemperature& _dallas, const String& _label);
    ~DS18B20() = default; // No dynamic memory -> default is fine

    float read() override;

private:
    DallasTemperature& dallas;  // reference to an existing DallasTemperature object
};
