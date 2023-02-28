#include <WiFi.h>
#include <comum.h>
#include <conexao.h>

#define CHECK_WIFI_INTERVALO 5000 // 5s
#define NET_TIMEOUT 30000 // 30s

char ssid[] = "NET_2G4B5A13";
char password[] = "B34B5A13";

unsigned long ultimoCheckWifi;
unsigned long ultimoWifiOk;

inline void netReset() {
    WiFi.disconnect(true);
    WiFi.begin(ssid, password);
}

void iniciaWifi() {
    // Seleciona modo do Wifi
    WiFi.mode(WIFI_STA);

    netReset();
}

void verificaConexao() {
    // Verificamos ocasionalmente se a conexão WiFi está funcionando
    if (millisAtual - ultimoCheckWifi <= CHECK_WIFI_INTERVALO) return;
    Serial.println(WiFi.localIP().toString().c_str());
    ultimoCheckWifi = millisAtual;

    if (WiFi.status() == WL_CONNECTED) {
        ultimoWifiOk = millisAtual;
    } else if (millisAtual - ultimoWifiOk > NET_TIMEOUT) {
        netReset();
        ultimoWifiOk = millisAtual;
    }
}

bool conectado() {
    return WiFi.status() == WL_CONNECTED; 
}