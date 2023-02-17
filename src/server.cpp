#include <Arduino.h>
#include <WebServer.h>
#include <processaimagem.h>
#include "esp_camera.h"
#include "esp_heap_caps.h"
#include "esp_heap_trace.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define CONEXAO_H
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

namespace Server {
	WebServer server(80); 

	framesize_t FRAME_SIZE_IMAGE = FRAMESIZE_VGA;        // Image resolution:
															//               default = "const framesize_t FRAME_SIZE_IMAGE = FRAMESIZE_VGA"
															//               160x120 (QQVGA), 128x160 (QQVGA2), 176x144 (QCIF), 240x176 (HQVGA),
															//               320x240 (QVGA), 400x296 (CIF), 640x480 (VGA, default), 800x600 (SVGA),
															//               1024x768 (XGA), 1280x1024 (SXGA), 1600x1200 (UXGA)
	uint32_t altura = 640;
	uint32_t largura = 480;
	
	#define PIXFORMAT PIXFORMAT_JPEG;                    // image format, Options =  YUV422, GRAYSCALE, RGB565, JPEG, RGB888

	String ImageResDetails = "Unknown";       // image resolution info
	String resultado;
	char buffer[50];
 	uint16_t imagerefresh = 5;                             // Tempo até resetar a imagem na página root (em segundos)

	bool iniciaCamera() {
		camera_config_t config;
		config.ledc_channel = LEDC_CHANNEL_0;
		config.ledc_timer = LEDC_TIMER_0;
		config.pin_d0 = Y2_GPIO_NUM;
		config.pin_d1 = Y3_GPIO_NUM;
		config.pin_d2 = Y4_GPIO_NUM;
		config.pin_d3 = Y5_GPIO_NUM;
		config.pin_d4 = Y6_GPIO_NUM;
		config.pin_d5 = Y7_GPIO_NUM;
		config.pin_d6 = Y8_GPIO_NUM;
		config.pin_d7 = Y9_GPIO_NUM;
		config.pin_xclk = XCLK_GPIO_NUM;
		config.pin_pclk = PCLK_GPIO_NUM;
		config.pin_vsync = VSYNC_GPIO_NUM;
		config.pin_href = HREF_GPIO_NUM;
		config.pin_sscb_sda = SIOD_GPIO_NUM;
		config.pin_sscb_scl = SIOC_GPIO_NUM;
		config.pin_pwdn = PWDN_GPIO_NUM;
		config.pin_reset = RESET_GPIO_NUM;
		config.xclk_freq_hz = 20000000;
		config.pixel_format = PIXFORMAT_JPEG; 
		config.frame_size = FRAME_SIZE_IMAGE;
		
		if(psramFound()){
			config.frame_size = FRAMESIZE_SVGA; //UXGA;
			config.jpeg_quality = 10;
			config.fb_count = 2;
		} else {
			config.frame_size = FRAMESIZE_SVGA;
			config.jpeg_quality = 12;
			config.fb_count = 1;
		}
		
		// Camera init
		esp_err_t err = esp_camera_init(&config);
		if (err != ESP_OK) {
			Serial.printf("Camera init failed with error 0x%x", err);
			return;
		}
	}

	void sendText(WiFiClient &client, String theText) {
		if (theText.indexOf("error") > 0) Serial.println(theText);   // Exibe na tela se a mensagem contem erro
		client.print(theText + "<br>\n");
	}



 
	void sendHeader(WiFiClient &client, char* hTitle) {
		// Start page
		client.write("HTTP/1.1 200 OK\r\n");
		client.write("Content-Type: text/html\r\n");
		client.write("Connection: close\r\n");
		client.write("\r\n");
		client.write("<!DOCTYPE HTML><html lang='en'>\n");
		// HTML / CSS
		client.printf(R"=====(
			<head>
			<meta name='viewport' content='width=device-width, initial-scale=1.0'>
			<title>%s</title>
			<style>
				body {
				color: black;
				background-color: #FFFF00;
				text-align: center;
				}
				input {
				background-color: #FF9900;
				border: 2px #FF9900;
				color: blue;
				padding: 3px 6px;
				text-align: center;
				text-decoration: none;
				display: inline-block;
				font-size: 16px;
				cursor: pointer;
				border-radius: 7px;
				}
				input:hover {
				background-color: #FF4400;
				}
			</style>
			</head>
			<body>
			<h1 style='color:red;'>%s</H1>
		)=====", hTitle, hTitle);
	}

	void sendFooter(WiFiClient &client) {
		client.write("</body></html>\n");
		delay(3);
		client.stop();
	}


	void initServer(){
		server.begin();                               // start web server

	// define the web pages (i.e. call these procedures when url is requested)
		server.on("/", handleRoot);                   // root page
		//server.on("/jpg", handleJPG);                 // capture image and send as jpg
		server.on("/jpg", handleRGBinfo);              // demo converting image to RGB
		//server.onNotFound(handleNotFound);            // invalid url requested
	}


	void updateServer() {
		server.handleClient(); 
	}

	bool handleRGBinfo() {
		WiFiClient client = server.client();          // open link with client
		char buf[32];

		// capture the jpg image from camera
		camera_fb_t * fb = esp_camera_fb_get();
		if (!fb) {
			Serial.println("Error: failed to capture image");
			return 0;
		}

		// store image resolution info.
		ImageResDetails = String(fb->width) + "x" + String(fb->height);

		// html to send a jpg
		const char HEADER[] = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n";
		const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
		const int hdrLen = strlen(HEADER);
		const int cntLen = strlen(CTNTTYPE);
		client.write(HEADER, hdrLen);
		client.write(CTNTTYPE, cntLen);
		sprintf( buf, "%d\r\n\r\n", fb->len);      // put text size in to 'buf' char array and send
		client.write(buf, strlen(buf));

		// send the captured jpg data
		client.write((char *)fb->buf, fb->len);

		// AGORA VAMOS CONVERTER A IMAGEM PARA RGB E ANALISAR AS INFORMAÇÕES.

		// Aloca a memória para guardar a imagem RGB: 3 bytes por pixel 
		sendText(client,"<br>Free psram before rgb data allocated = " + String(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024) + "K");
		void *ptrVal = NULL;                                                                                 // create a pointer for memory location to store the data
		uint32_t ARRAY_LENGTH = fb->width * fb->height * 3;                                                  // calculate memory required to store the RGB data (i.e. number of pixels in the jpg image x 3)
		
		if (heap_caps_get_free_size( MALLOC_CAP_SPIRAM) <  ARRAY_LENGTH) {
			sendText(client,"error: not enough free psram to store the rgb data");
			return;
		}
		
		ptrVal = heap_caps_malloc(ARRAY_LENGTH, MALLOC_CAP_SPIRAM);                                          // allocate memory space for the rgb data
		uint8_t *rgb = (uint8_t *)ptrVal;                                                                    // create the 'rgb' array pointer to the allocated memory space
		sendText(client,"Free psram after rgb data allocated = " + String(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024) + "K");

	    // Converte a imagem de jpeg para RGB                                                                               
		bool jpeg_converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgb);



		if (!jpeg_converted) {
			sendText(client,"error: failed to convert image to RGB data");
			return;
		}

		resultado = processa_imagem(rgb, altura, largura);
		
		esp_camera_fb_return(fb);   // camera frame buffer
		heap_caps_free(ptrVal);     // rgb data
		delay(3); // não acho que seja necessário
		client.stop();

		return 1;
	}  

void handleRoot() {

	WiFiClient client = server.client();                       // open link with client

	// html header
	sendHeader(client, "Sensor de amônia");

	// --------------------------------------------------------------------

	// html main body
	//                    Info on the arduino ethernet library:  https://www.arduino.cc/en/Reference/Ethernet
	//                                            Info in HTML:  https://www.w3schools.com/html/
	//     Info on Javascript (can be inserted in to the HTML):  https://www.w3schools.com/js/default.asp
	//                               Verify your HTML is valid:  https://validator.w3.org/


	// ---------------------------------------------------------------------------------------------


	// links to the other pages available
	client.write("<br><br>LINKS: \n");
	client.write("<a href='/jpg'>Baixa a imagem</a> - \n");

	// capture and show a jpg image
	client.write("<br><a href='/jpg'>");         // make it a link
	client.write("<img id='image1' src='/jpg' width='320' height='240' /> </a>");     // show image from http://x.x.x.x/jpg
		
	// Exibe para o usuário o estado do sensor
	sprintf(buffer, "<br> O estado atual do sensor é: %s <br>", resultado.c_str());
	client.write(buffer);

	// javascript to refresh the image periodically
	client.printf(R"=====(
		<script>
		function refreshImage(){
			var timestamp = new Date().getTime();
			var el = document.getElementById('image1');
			var queryString = '?t=' + timestamp;
			el.src = '/jpg' + queryString;
		}
		setInterval(function() { refreshImage(); }, %d);
		</script>
	)=====", imagerefresh * 1013);        // 1013 is just to stop it refreshing at the same time as /data

	client.println("<br><br><a href='https://github.com/alanesq/esp32cam-demo'>Sketch Info</a>");


	// --------------------------------------------------------------------


	sendFooter(client);     // close web page

    }  // handleRoot*/

}

void iniciaServidor() {
	Server::initServer();
}

void updateServidor() {
	Server::updateServer();
}