void faz_quadrado(uint8_t *rgb, uint32_t x, uint32_t y, int k, int largura){
    for(int i = (x-k); i < (x+k+1); i++){
        for(int j = (y-k); j < (y+k+1); j++){
            if((i!=x) || (j!=y)){
                uint32_t pos = vetoriza(i, j, largura);
                // pinta de vermelho
                rgb[3*pos] = 0;        // B
                rgb[3*pos + 1] = 0;    // G
                rgb[3*pos + 2] = 255;  // R
            }
        }
    }
}

void novo_JPEG(){
    faz_quadrado();
    // converte para JPEG
}