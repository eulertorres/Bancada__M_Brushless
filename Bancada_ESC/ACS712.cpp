#include "ACS712.hpp"
#include <Math.h>

#define NAmostras 5

ACS712::ACS712(const uint8_t AnPin, const String Label) :
	Sensor(Label),
	_AnPin(AnPin),
	_Label(Label),
	scale(0.066f),	// V/A (Shunt)
	offset(0.0f)
{
	pinMode(_AnPin, INPUT);
}

float ACS712::read() {
    float localSquareSum = 0;
    float localSum = 0;
    int localAmountOfSamples = 0;

    const unsigned long sampleDelayMicros = 100; // Pequeno atraso entre amostras

    for (int i = 0; i < NAmostras; i++) {
        // Lógica de amostragem
        float readingVolts = (float)analogRead(_AnPin) * 4.83 / 1023.0; 
        float readingAmps = (readingVolts - 2.5) / scale;

        localSquareSum += readingAmps * readingAmps;
        localSum += readingAmps;
        localAmountOfSamples++;

        delayMicroseconds(sampleDelayMicros);
    }

    float rms = 0;
    if (localAmountOfSamples > 0) { // Evita divisão por zero
        rms = (float)sqrt((double)localSquareSum / localAmountOfSamples);
    }

    return rms;
}