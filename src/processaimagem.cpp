#include <Arduino.h>
#include "esp_heap_caps.h"
#include "esp_heap_trace.h"
#include <math.h>

uint32_t vetoriza(uint32_t x, uint32_t y, int largura)
{
    return x + (y * largura);
}

void vals_rgb(uint32_t ponto, uint8_t *rgb, int *array)
{
    array[0] = rgb[3 * ponto];
    array[1] = rgb[(3 * ponto) + 1];
    array[2] = rgb[(3 * ponto) + 2];
    return;
}

int hue(double r, double g, double b)
{
    r = r / 255.0;
    g = g / 255.0;
    b = b / 255.0;

    double cmax = max(r, max(g, b)); // maximum of r, g, b
    double cmin = min(r, min(g, b)); // minimum of r, g, b
    double diff = cmax - cmin;       // diff of cmax and cmin.
    double h = -1;

    if (cmax == cmin)
        h = 0;

    else if (cmax == r)
        h = fmod(60 * ((g - b) / diff) + 360, 360);

    else if (cmax == g)
        h = fmod(60 * ((b - r) / diff) + 120, 360);

    else if (cmax == b)
        h = fmod(60 * ((r - g) / diff) + 240, 360);
    return h;
}

String mais_proximo_util_2(int i)
{
    if (i == 0)
    {   
        //Serial.println("ESQUERDA");
        return "ALERT (0.05 ppm)";
    }
    else if (i == 1)
    {
        //Serial.println("DIREITA");
        return "TOXIC (0.5 ppm)";
    }
    else if (i == 2)
    {
        //Serial.println("SUPERIOR");
        return "ALARM (0.2 ppm)";
    }
    else
    {
        //Serial.println("INFERIOR");
        return "SAFE (< 0.02 ppm)";
    }
}

String mais_proximo_util(double valor, double *lista)
{
    double min = INT_MAX;
    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        if (abs(((int)valor) - ((int)lista[i])) < min)
        {
            min = abs(((int)valor) - ((int)lista[i]));
            index = i;
        }
    }

    return mais_proximo_util_2(index);
}

void yLine(uint8_t *rgb, uint32_t x, uint32_t y1, uint32_t y2, int largura){
    while (y1 <= y2){
        uint32_t pos = vetoriza(x, y1, largura);
        // pinta de vermelho
        rgb[3*pos] = 0;        // B
        rgb[3*pos + 1] = 0;    // G
        rgb[3*pos + 2] = 255;  // R
        y1++;
    }
}

void xLine(uint8_t *rgb, uint32_t x1, uint32_t x2, uint32_t y, int largura){
    while (x1 <= x2){
        uint32_t pos = vetoriza(x1, y, largura);
        // pinta de vermelho
        rgb[3*pos] = 0;        // B
        rgb[3*pos + 1] = 0;    // G
        rgb[3*pos + 2] = 255;  // R
        x1++;
    }
}

void faz_circulo(uint8_t *rgb, uint32_t xc, uint32_t yc, int r, int grossura, int largura){


    //ro = thick - 1 + r

    int xo = grossura + r - 1;
    int xi = r;
    int y = 0;
    int erro = 1 - xo;
    int erri = 1 - xi;

    while(xo >= y) {
        xLine(rgb, xc + xi, xc + xo, yc + y, largura);
        yLine(rgb, xc + y,  yc + xi, yc + xo, largura);
        xLine(rgb, xc - xo, xc - xi, yc + y, largura);
        yLine(rgb, xc - y,  yc + xi, yc + xo, largura);
        xLine(rgb, xc - xo, xc - xi, yc - y, largura);
        yLine(rgb, xc - y,  yc - xo, yc - xi, largura);
        xLine(rgb, xc + xi, xc + xo, yc - y, largura);
        yLine(rgb, xc + y,  yc - xo, yc - xi, largura);

        y++;

        if (erro < 0) {
            erro += 2 * y + 1;
        } else {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > r) {
            xi = y;
        } else {
            if (erri < 0) {
                erri += 2 * y + 1;
            } else {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}


String processa_imagem(uint8_t *rgb, uint32_t pontos[][2], int altura, int largura)
{

    uint32_t ponto_esquerdo_vetorizado = vetoriza(pontos[0][0], pontos[0][1], largura);
    uint32_t ponto_direito_vetorizado = vetoriza(pontos[1][0], pontos[1][1], largura);
    uint32_t ponto_superior_vetorizado = vetoriza(pontos[2][0], pontos[2][1], largura);
    uint32_t ponto_inferior_vetorizado = vetoriza(pontos[3][0], pontos[3][1], largura);
    uint32_t ponto_central_vetorizado = vetoriza(pontos[4][0], pontos[4][1], largura);

    int esquerdo[3];
    vals_rgb(ponto_esquerdo_vetorizado, rgb, esquerdo);

    int direito[3];
    vals_rgb(ponto_direito_vetorizado, rgb, direito);

    int superior[3];
    vals_rgb(ponto_superior_vetorizado, rgb, superior);

    int inferior[3];
    vals_rgb(ponto_inferior_vetorizado, rgb, inferior);

    int centro[3];
    vals_rgb(ponto_central_vetorizado, rgb, centro);

    double hues[4];
    hues[0] = hue(esquerdo[0], esquerdo[1], esquerdo[2]);
    hues[1] = hue(direito[0], direito[1], direito[2]);
    hues[2] = hue(superior[0], superior[1], superior[2]);
    hues[3] = hue(inferior[0], inferior[1], inferior[2]);
    double hue_centro = hue(centro[0], centro[1], centro[2]);

    return mais_proximo_util(hue_centro, hues);
}