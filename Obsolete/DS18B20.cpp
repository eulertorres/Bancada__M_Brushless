#include "DS18B20.hpp"
#include <Arduino.h>

DS18B20::DS18B20(DallasTemperature& _dallas, const String& _label)
    : Sensor(255, _label),
      dallas(_dallas) // store reference
{
    // no "begin" call here, because we do that externally in setup().
    // If needed, you can still store or check anything else.
}

float DS18B20::read() {
    // We assume dallas has been initialized externally
    dallas.requestTemperatures();
    float tempC = dallas.getTempCByIndex(0);
    return tempC;
}
