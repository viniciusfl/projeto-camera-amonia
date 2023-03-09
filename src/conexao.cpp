#include <WiFi.h>
#include <comum.h>
#include <conexao.h>
#include "time.h"

#define CHECK_WIFI_INTERVALO 5000 // 5s
#define NET_TIMEOUT 30000 // 30s

char ssid[] = "RESIDENCIA-IME/POLI";
char password[] = "siricuri123";

const char* ntpServer = "a.st1.ntp.br";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = -3600*3;

unsigned long ultimoCheckWifi;
unsigned long ultimoWifiOk;

inline void comecaHoras(WiFiEvent_t event, WiFiEventInfo_t info) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

inline void resetaNet() {
    WiFi.disconnect(true);
    WiFi.begin(ssid, password);
    WiFi.onEvent(comecaHoras, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
}

void iniciaWifi() {
    // Seleciona modo do Wifi
    WiFi.mode(WIFI_STA);

    resetaNet();
}

void verificaConexao() {
    // Verificamos ocasionalmente se a conexão WiFi está funcionando
    if (millisAtual - ultimoCheckWifi <= CHECK_WIFI_INTERVALO) return;
    Serial.println(WiFi.localIP().toString().c_str());
    ultimoCheckWifi = millisAtual;

    if (WiFi.status() == WL_CONNECTED) {
        ultimoWifiOk = millisAtual;
    } else if (millisAtual - ultimoWifiOk > NET_TIMEOUT) {
        resetaNet();
        ultimoWifiOk = millisAtual;
    }
}

bool conectado() {
    return WiFi.status() == WL_CONNECTED; 
}