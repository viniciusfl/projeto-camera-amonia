#include <Arduino.h>
#include "esp_heap_caps.h"
#include "esp_heap_trace.h"

uint32_t vetoriza(uint32_t x, uint32_t y, int largura) {
    return x + (y*largura);
}

void vals_rgb(uint32_t ponto, uint8_t *rgb, int* array) {
    array[0] = rgb[3*ponto];
    array[1] = rgb[(3*ponto) + 1];
    array[2] = rgb[(3*ponto) + 2];
    return;
}

int hue(double r, double g, double b){
    r = r / 255.0; g = g / 255.0; b = b / 255.0;

    double cmax = max(r, max(g, b)); // maximum of r, g, b
    double cmin = min(r, min(g, b)); // minimum of r, g, b
    double diff = cmax - cmin; // diff of cmax and cmin.
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

String mais_proximo_util_2(int i){
	if(i == 0){
        return "ALERT";
	}  
    else if(i == 1){
		return "TOXIC";
	}   
    else if(i == 2){
        return "SAFE";
	}
    else{ 
        return "ALARM";
	}
}


String mais_proximo_util(double valor, double* lista) {
    double min = INT_MAX;
    int index = 0;

    for(int i = 0; i < 4; i++){
        if(abs(((int) valor) - ((int) lista[i])) < min){
            min = abs(((int) valor) - ((int) lista[i]));
            index = i;
        }
    }

    return mais_proximo_util_2(index);
}


String processa_imagem(uint8_t *rgb, int altura, int largura) {
    uint32_t ponto_esquerdo_x = (uint32_t)(1 / 4. * largura);
    uint32_t ponto_esquerdo_y = (uint32_t)(1 / 2. * altura);


    uint32_t ponto_direito_x = (uint32_t)(3 / 4. * largura);
    uint32_t ponto_direito_y = (uint32_t)(1 / 2. * altura);


    uint32_t ponto_superior_x = (uint32_t)(1 / 2. * largura);
    uint32_t ponto_superior_y = (uint32_t)(1 / 4. * altura);
    

    uint32_t ponto_inferior_x = (uint32_t)(1 / 2. * largura);
    uint32_t ponto_inferior_y = (uint32_t)(3 / 4. * altura);
    

    uint32_t ponto_central_x = (uint32_t)(1 / 2. * largura);
    uint32_t ponto_central_y = (uint32_t)(1 / 2. * altura);

/*
    Serial.print("Coordenadas ponto esquerdo: ");
    Serial.print("(");
    Serial.print(ponto_esquerdo_x); Serial.print(","); Serial.print(ponto_esquerdo_y); 
    Serial.println(")");

    Serial.print("Coordenadas ponto direito: ");
    Serial.print("(");
    Serial.print(ponto_direito_x); Serial.print(","); Serial.print(ponto_direito_y); 
    Serial.println(")");
    
    Serial.print("Coordenadas ponto superior: ");
    Serial.print("(");
    Serial.print(ponto_superior_x); Serial.print(","); Serial.print(ponto_superior_y); 
    Serial.println(")");

    Serial.print("Coordenadas ponto inferior: ");
    Serial.print("(");
    Serial.print(ponto_inferior_x); Serial.print(","); Serial.print(ponto_inferior_y); 
    Serial.println(")");

    Serial.print("Coordenadas ponto central: ");
    Serial.print("(");
    Serial.print(ponto_central_x); Serial.print(","); Serial.print(ponto_central_y); 
    Serial.println(")");

*/    

    uint32_t ponto_esquerdo_vetorizado = vetoriza(ponto_esquerdo_x, ponto_esquerdo_y, largura);
    uint32_t ponto_direito_vetorizado = vetoriza(ponto_direito_x, ponto_direito_y, largura);
    uint32_t ponto_superior_vetorizado = vetoriza(ponto_superior_x, ponto_superior_y, largura);
    uint32_t ponto_inferior_vetorizado = vetoriza(ponto_inferior_x, ponto_inferior_y, largura);
    uint32_t ponto_central_vetorizado = vetoriza(ponto_central_x, ponto_central_y, largura);

/*
    Serial.print("esquerdo vetorizado = ");
    Serial.println(ponto_esquerdo_vetorizado);

    Serial.print("direito vetorizado = ");
    Serial.println(ponto_direito_vetorizado);

    Serial.print("superior vetorizado = ");
    Serial.println(ponto_superior_vetorizado);

    Serial.print("inferior vetorizado = ");
    Serial.println(ponto_inferior_vetorizado);

    Serial.print("central vetorizado = ");
    Serial.println(ponto_central_vetorizado);
*/

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