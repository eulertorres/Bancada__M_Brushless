#pragma once
#include <stdint.h>

#include "Sensor.hpp"
#include "Arduino.h"

class ACS712 : public Sensor {

public:
    ACS712(const uint8_t, const String);
    ~ACS712() = default;
    float read() override;
    void sample();

private: 
    float squareSum = 0;
    float sum = 0;
    uint32_t amountOfSamples = 0;

    float shunt = 0.066; // V/A
    
    const uint32_t sampleRate = 2000;
    uint32_t lastSampleTime;
};
