#include "Arduino.h"
#include "ACS712.hpp"

ACS712::ACS712(const uint8_t _pin, const String _label) : Sensor(_pin, _label) {
    pinMode(pin, INPUT);
}

void ACS712::sample() {

    if ((micros() - lastSampleTime) < sampleRate) {
        return;
    }

    float readingVolts = (float)analogRead(pin) * 5.0 / 1023.0;
	float readingAmps = (readingVolts - 2.5) / shunt;

    squareSum += readingAmps * readingAmps;
    sum += readingAmps;
    amountOfSamples++;
}

float ACS712::read() {
    
    float rms = (float)sqrt((double)squareSum / amountOfSamples);
    float mean = (float)((double)sum / amountOfSamples);

    squareSum = 0;
    sum = 0;
    amountOfSamples = 0;

    return rms;
}