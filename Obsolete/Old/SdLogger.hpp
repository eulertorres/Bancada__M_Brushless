#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "Arduino.h"
#include "HardwareSerial.h"
#include "SD.h"

#include "Relay.hpp"

class SdLogger {

public:
    SdLogger(const uint8_t);
    ~SdLogger();

    bool createNewCsvFile(const String);
    bool append(const String, const Relay&);
    void closeCsvFile();
    
    bool hasFileOpen = false;

private:
    File file;
    const uint8_t batchMaxSize = 30;
    uint8_t batchSize = 0;
    String currentFilePath;
    uint8_t csPin;
};