* salvamos imagem em um sistema de armazenamento chamado spiffs;

* estudar tipo byte de cpp.

* procedimento que lida com a captura de imagens e armazenamento em spiffs ou SDcard é o mesmo: "byte storeImage()"

* Isso ocorre pois, a captura de imagem ocorre na linha 706:
camera_fb_t *fb = esp_camera_fb_get();       // capture image frame from camera


* O que é essa classe "camera_fb_t", foi incluida?
estudar o que a função esp_camera_fb_get(); retorna.

* Na prática, não usamos esses procedimentos para ler as informações da imagem RGB.
Isso ocorre pois não salvamos a imagem de fato. Ela fica armazenada de maneira "crua" na memória do chip. De qualquer forma, note que a captura da imagem é feita de forma bem semelhante da que ocorre na linha 706.

* O fato é que a imagem, inicialmente, sempre é guardada no objeto fb. Isso está em formato jpg. Então, para obter as informações em RGB, precisamos converter uma imagem que está em jpg para RGB. Isso é feito na linha 1280:
     bool jpeg_converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgb);

     Agora, de onde essa função fmt2rgb888 veio?

Perceba que o parâmetro rgb que foi mandado para essa função é o espaço na memória que foi alocado. Isso é feito algumas linhas antes com o uso de malloc (altura * largura * 3);

O que acontece é que a função fmt2rgb888 escreve as infos em RGB na variável rgb. Então, depois dessa função, essa variável já corresponde à imagem em RGB (só que de forma vetorizada). Agora, como obter os valores RGB de um pixel em uma posição (x,y) arbitrária?


O array devolvido segue o seguinte padrão:

B_pixel0, G_pixel0, R_pixel0, B_pixel1, G_pixel1, R_pixel1, B_pixel2, G_pixel2, R_pixel2, ...

Agora, se temos posição (x, y) isso vira qual posição z?

vamos supor que a largura da imagem seja W.

O valor de x começa com 0 e pode assumir até o valor de W-1.
(0,0) -> 0
(1,0) -> 1
(W-1,0) -> W-1


O valor de y só muda quando já passamos por uma linha inteira.

(0,0) -> 0
(0,1) -> W
(0,2) -> 2W

Portanto, a posição desse pixel em uma imagem vetorizada corresponde à quantas linhas para baixo este pixel está, em pixels, (y*W) somado à posição desse pixel em uma linha (x)

(x, y) -> x + y*W


Agora, cada pixel possui 3 valores:

O pixel 0 vai de 0 a 2. O pixel 1 vai de 3 a 5, o pixel 2 vai de 6 a 8, o pixel 3 vai de 9 a 11. O pixel x vai de (3 * x) a ((3 * x) + 2).


Então, se eu quiser saber as infos RGB do pixel (x, y), quais valores do array eu preciso retornar?


int* valRGB(int x, int y){
    int pos = x + y*W;
    int vals[3];
    vals[0] = 3*pos;
    vals[1] = 3*pos + 1;
    vals[2] = 3*pos + 2;
    return vals;
}


Agora, depois que tivermos os valores RGB, precisamos do valor de HUE desse pixel.
Para isso, temos a seguinte abordagem:

1-Divide r, g, b by 255
2-Compute cmax, cmin, difference
3-Hue calculation : 
    if cmax and cmin equal 0, then h = 0
    if cmax equal r then compute h = (60 * ((g – b) / diff) + 360) % 360
    if cmax equal g then compute h = (60 * ((b – r) / diff) + 120) % 360
    if cmax equal b then compute h = (60 * ((r – g) / diff) + 240) % 360




int hue (double r, double g, double b){

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



Então, o procedimento será:

* Efetuar uma captura de "live image" (fb)
* Postar no servidor web
* Converter para RGB
* Obter valores RGB de 5 pixels específcos
* Obter os valores de HUE desses 5 pixels
* Comparar esses valores de HUE
* Saber de qual o do centro  está mais próximo
* Postar resposta no servidor web

Talvez piscar led quando formos capturar a imagem live.

* Efetuar uma captura de "live image" (fb)
* Converter para RGB (rgb)
* Liberar memória que fb está ocupando
* Obter valores RGB de 5 pixels específcos
* Obter os valores de HUE desses 5 pixels
* Comparar esses valores de HUE
* Saber de qual o do centro  está mais próximo
* Postar resposta no servidor web
* Desenhar quadrados em rgb
* Converter RGB (rgb) para JPEG buffer (novo_fb)
* Liberar memória que rgb está ocupando
* Postar novo_fb no servidor
* Liberar memória que novo_fb está ocupando





