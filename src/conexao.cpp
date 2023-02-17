#include <WiFi.h>
#include <comum.h>
#include <conexao.h>

#define CHECK_WIFI_INTERVALO 5000 // 5s
#define NET_TIMEOUT 30000 // 30s

char ssid[] = "";
char password[] = "";

unsigned long ultimoCheckWifi;
unsigned long ultimoWifiOk;

inline void netReset() {
    WiFi.disconnect(true);
    WiFi.begin(ssid, password);
}

inline void iniciaWifi() {
    // Seleciona modo do Wifi
    WiFi.mode(WIFI_STA);

    netReset();
}

inline void verificaConexao() {
    // Verificamos ocasionalmente se a conexão WiFi está funcionando
    if (millisAtual - ultimoCheckWifi <= CHECK_WIFI_INTERVALO) return;

    ultimoCheckWifi = millisAtual;

    if (WiFi.status() == WL_CONNECTED) {
        ultimoWifiOk = millisAtual;
    } else if (millisAtual - ultimoWifiOk > NET_TIMEOUT) {
        netReset();
        ultimoWifiOk = millisAtual;
    }
}

inline bool conectado() {
    return WiFi.status() == WL_CONNECTED; 
}