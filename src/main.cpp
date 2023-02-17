#include <Arduino.h>
#include <SPI.h>
#include <SD.h>


void setup() {
    Serial.begin(115200);
    // wait for serial port to connect. Needed for native USB port only
    while (!Serial) { ; }
    delay(100);
}

void loop() {
    Serial.println("teste");
    delay(5000);
}



