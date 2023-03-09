#include <Arduino.h>
#include <conexao.h>
#include <SPI.h>
#include <SD.h>
#include <server.h>

unsigned long millisAtual;

void setup() {
    Serial.begin(115200);
    // wait for serial port to connect. Needed for native USB port only
    while (!Serial) { ; }
    delay(100);

    millisAtual = millis();
    iniciaWifi();
    iniciaServidor();
}

void loop() {
    millisAtual = millis();
    handleServer();
    verificaConexao();
}



