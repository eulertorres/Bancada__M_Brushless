#include "SdLogger.hpp"

SdLogger::SdLogger(const uint8_t _csPin) : csPin(_csPin) {

    pinMode(csPin, OUTPUT);

    // This begins use of the SPI bus (digital pins 11, 12, and 13 on most Arduino boards; 50, 51, and 52 on the Mega) 
    if (!SD.begin(csPin)) {
        Serial.println("Card Mount Failed");
    }
    else {
        Serial.println("SD Card Initialized");
    }
}

SdLogger::~SdLogger() {

}



bool SdLogger::createNewCsvFile(const String dirName) {

    if (!SD.exists(dirName)) {
        Serial.print("Creating directory '");
        Serial.print(dirName);
        Serial.println("'...");
        bool success = SD.mkdir(dirName);
        Serial.println(success ? "Directory created." : "Directory creation failed!");
        if (!success) {
            while(true) { /* halt */ }
        }
    } 
    else {
        Serial.print("Directory '");
        Serial.print(dirName);
        Serial.println("' already exists.");
    }

    uint16_t index = 0;
    String filePath;
    do {
        filePath = String(dirName) + "/LOG" + index + ".CSV";
        index++;
    } while (SD.exists(filePath));

    Serial.print("Next available file: ");
    Serial.println(filePath);

    file = SD.open(filePath, FILE_WRITE);
    if (!file) {
        currentFilePath = filePath;
        Serial.print("Failed to open '");
        Serial.print(filePath);
        Serial.println("' for writing. :(");
    } 
    else {
        Serial.print("Opened '");
        Serial.print(filePath);
        Serial.println("' successfully!");
    }

    hasFileOpen = true;
    return true;
}

bool SdLogger::append(const String dataline, const Relay& errorLed) {
    if (!hasFileOpen || !file) {
        return false;
    }

    file.print(dataline);
    batchSize++;

    if (batchSize >= batchMaxSize) {
        batchSize = 0;

        file.flush();
    }

    Serial.print(batchSize);
    Serial.print(" ");
    Serial.print(dataline);

    return true;
}

void SdLogger::closeCsvFile() {
    file.close();
    hasFileOpen = false;
}

