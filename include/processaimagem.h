#include <Arduino.h>

#ifndef CONEXAO_H
#define CONEXAO_H

void faz_quadrado(uint8_t *rgb, uint32_t x, uint32_t y, int k, int largura);

String processa_imagem(uint8_t *rgb, uint32_t pontos[][2], int altura, int largura);


#endif