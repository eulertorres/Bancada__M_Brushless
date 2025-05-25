#include "SdLogger.hpp"
#include "Sensor.hpp"
#include "MCP9701A.hpp"
#include "DS18B20.hpp"
#include "ACS712.hpp"
#include "Relay.hpp"

const uint8_t sdCsPin {4};
const uint8_t buttonPin {2};
const uint8_t ledPin {3};
const uint8_t pumpPin {A2};
const uint8_t oneWireBus {A5};

volatile bool buttonPressed {false};
uint32_t logTimeRef;
uint32_t minutesCounter;
uint32_t milliSecondsCounter;
const uint32_t logDurationMin = 8 * 60;

Relay pump(pumpPin, false);
Relay led(ledPin, false);

SdLogger logger(sdCsPin);

OneWire oneWire(oneWireBus);
DallasTemperature dallas(&oneWire);

MCP9701A pumpTemperatureSensor(A4, "pump[°C]");
MCP9701A roomTemperatureSensor(A3, "room[°C]");
DS18B20  fluidTemperartureSensor(dallas, "fluid[°C]");
ACS712   currentSensor(A7, "current[A]");

Sensor* sensors[] = {
    &pumpTemperatureSensor, 
    &roomTemperatureSensor,
    &fluidTemperartureSensor,
    &currentSensor,
};

void buttonInterruptHandler() {
    buttonPressed = true;
}

void openNewLog() {
    if (logger.hasFileOpen) {
        return;
    }

    bool success = logger.createNewCsvFile("PUMP");

    if (!success) {
        return;
    }

    led.turnOn();
    logTimeRef = millis();

    String header = "time[ms]";
    for (auto sensor : sensors) {
        header += ";" + sensor->getLabel();
    }
    header += "\n";
    logger.append(header, led);
    pump.turnOn();

    milliSecondsCounter = 0;
    minutesCounter = 0;
}

void closeCurrentLog() {

    if (!logger.hasFileOpen) {
        return;
    }

    logger.closeCsvFile();
    led.turnOff();
    pump.turnOff();
}

void handleButtonPress() {

    led.reverseState();
    delay(250);
    led.reverseState();
    delay(250);

    if (!logger.hasFileOpen) {
        openNewLog();
    }
    else {
        closeCurrentLog();
    }
}

void setup() {

    dallas.begin();

    Serial.begin(9600);
    delay(1000);
    Serial.println("Startup");

    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterruptHandler, FALLING);
}

void loop() {
    static const uint32_t buttonDebounceTime {500};
    static const uint32_t logRate = 1000;
    static uint32_t lastLog = millis();

    if (buttonPressed) {
        static uint32_t lastTrigger = 0;
        uint32_t now = millis();

        if ((now - lastTrigger) > buttonDebounceTime) {
            handleButtonPress();
        }
        buttonPressed = false;
    }

    uint32_t now = millis();
    if (((now - lastLog) > logRate) && logger.hasFileOpen) {
        lastLog = now;

        milliSecondsCounter += logRate;
        if (milliSecondsCounter >= 60000) {
            milliSecondsCounter -= 60000;
            minutesCounter += 1;
        }

        String logLine = String(now - logTimeRef);
        for (auto sensor : sensors) {
            logLine += ";" + String(sensor->read());
        }
        logLine += "\n";
        logger.append(logLine, led);
    }

    if (minutesCounter >= logDurationMin) {
        closeCurrentLog();
    }

    if (pumpTemperatureSensor.read() > 100.0f) {
        closeCurrentLog();
    }

    currentSensor.sample();
}

