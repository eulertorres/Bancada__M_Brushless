#include "MCP9701A.hpp"

MCP9701A::MCP9701A(const uint8_t _pin, const String _label) : Sensor(_pin, _label) {
    pinMode(pin, INPUT);
}

float MCP9701A::read() {
    uint16_t reading = analogRead(pin);
    float readingVolts = (float)reading * 4.4 / 1023.0;
    float temperatureCelsius = (readingVolts - 0.4) / 0.0195;

    return temperatureCelsius;
}

MCP9701A::~MCP9701A() {
}