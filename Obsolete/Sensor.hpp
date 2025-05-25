#pragma once

#include <Arduino.h>

class Sensor {
public:
    
    Sensor(const uint8_t pin, const String label) : pin(pin), label(label) {};
    virtual ~Sensor() {};
    virtual float read() = 0;

    virtual String getLabel() const {
        return label;
    };

protected:
    uint8_t pin;
    String label;
};
