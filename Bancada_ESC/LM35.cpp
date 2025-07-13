#include "LM35.hpp"

LM35::LM35(const uint8_t PIN_An, const String Label) : 
	Sensor(Label),
	_PIN_An(PIN_An),
	_Label(Label),
	scale(100.0f),	// 10mV/C°  2 até 150°C
	voltDrop(0.0f)
{
	pinMode(_PIN_An, INPUT);
}

float LM35::read(){
    analogRead(_PIN_An);
    delay(2);	
    uint16_t reading = analogRead(_PIN_An);
    readingVolts = (float)reading * 5.0 / 1023.0;
    temperatureCelsius = (readingVolts - voltDrop) * scale;

    return temperatureCelsius;
}