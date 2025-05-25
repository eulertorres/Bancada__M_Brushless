#pragma once

#include <Arduino.h>

class Relay {

public:
    Relay(const uint8_t _pin, bool _reversed = false) : pin(_pin), reversed(_reversed)  {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, reversed);
    }

    ~Relay() {};

    void turnOn() {
        state = !reversed;
        digitalWrite(pin, state);
    }

    void turnOff() {
        state = reversed;
        digitalWrite(pin, state);
    }

    void reverseState() {
        state = !state;
        digitalWrite(pin, state);
    }


private:
    const uint8_t pin;
    bool state;
    bool reversed;
};