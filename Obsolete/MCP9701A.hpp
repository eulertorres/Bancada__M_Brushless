#pragma once

#include "Sensor.hpp"
#include <Arduino.h>

class MCP9701A : public Sensor {
public:
    MCP9701A(const uint8_t _pin, const String _label);
    ~MCP9701A();
    float read() override;
};
