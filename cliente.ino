#include <TFT.h>
#include <SPI.h>
#include <JPEGDecoder.h>  // JPEG decoder library
#include <Wifi.h> //library for the wireless connections
#include <WiFiClient.h> //library for the 

#include <Adafruit_NeoPixel.h> //library for the neopixel led


#define minumum(a, b) (((a) < (b) ? (a) : (b)))

#define buttonPin 22  //button pin for the Cozinha
#define piezzoPin 27  //pin for the Piezzo

//pin declaration for the TFT
#define cs 23
#define dc 25
#define rst 26
#define sd 34  //pin for the SD Card

TFT TFTscreen = TFT(cs, dc, rst);


const char* ssid = " ";
const char* pass = " ";

void setup() {
  Serial.begin(9600);
  pinMode(pinButton, INPUT);

  WiFi.begin(ssid, password);

  //check if the Wifi is conneced
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println(" ");
  Serial.println("Access point conneted!");
  Serial.print("IP Adress:");
  Serial.println(Wifi.localIP());

}

void loop() {

}
