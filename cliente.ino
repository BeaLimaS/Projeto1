#include <WiFi.h>               //library for the wireless connections
#include <Adafruit_ST7735.h>    //library for the TFT
#include <Adafruit_NeoPixel.h>  //library for the neopixel led

#define piezzoPin 27  //pin for the Piezzo

//ssid and pass for the wireless connections (created by the ESP32 SERVER)
const char *ssid = "Cozinha";
const char *password = "123456789";
const uint16_t port = 56789;

WiFiServer server(port);
WiFiClient client;

enum State {
  ESPERA,
  ACEITE,
  PREPARAR,
  PRONTO
};
State currentState = PREPARAR;

//pin declaration for the TFT
#define cs    5    // Chip select line for TFT display
#define dc    4    // Data/Command line for TFT display
#define rst   16   // Reset line for TFT display (can be any GPIO pin)
#define TFT_MOSI  23   // SPI MOSI line
#define TFT_SCLK  18   // SPI Clock line
Adafruit_ST7735 TFT = Adafruit_ST7735(cs, dc, rst);

//pin declaration for the Neopixels
#define neoPixelPin 13  // The ESP32 pin GPIO16 connected to NeoPixel
#define numNeopixel 29  // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numNeopixel, neoPixelPin, NEO_GRB + NEO_KHZ800);

unsigned long previousMillisBlink = 0;
unsigned long intervalBlink = 500;

unsigned long previousMillisMelody = 0;
unsigned long intervalMelody = 3000;

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

int tempo_game_of_thrones = 85;
int tempo_christmas = 140;
int tempo_mario = 200;
int tempo_tetris = 144;
int tempo_never_gonna_give_you_up = 114;

int wholenote_game_of_thrones = (60000 * 4) / tempo_game_of_thrones;
int wholenote_christmas = (60000 * 4) / tempo_christmas;
int wholenote_mario = (60000 * 4) / tempo_mario;
int wholenote_tetris = (60000 * 4) / tempo_tetris;
int wholenote_never_gonna_give_you_up = (60000 * 4) / tempo_never_gonna_give_you_up;

int melody_game_of_thrones[] = { NOTE_G4, 8, NOTE_C4, 8, NOTE_DS4, 16, NOTE_F4, 16, NOTE_G4, 8, NOTE_C4, 8, NOTE_DS4, 16, NOTE_F4, 16, //1
  NOTE_G4, 8, NOTE_C4, 8, NOTE_DS4, 16, NOTE_F4, 16, NOTE_G4, 8, NOTE_C4, 8, NOTE_DS4, 16, NOTE_F4, 16,
  NOTE_G4, 8, NOTE_C4, 8, NOTE_E4, 16, NOTE_F4, 16, NOTE_G4, 8, NOTE_C4, 8, NOTE_E4, 16, NOTE_F4, 16,
  NOTE_G4, 8, NOTE_C4, 8, NOTE_E4, 16, NOTE_F4, 16, NOTE_G4, 8, NOTE_C4, 8, NOTE_E4, 16, NOTE_F4, 16,
  NOTE_G4, -4, NOTE_C4, -4, //5

  NOTE_DS4, 16, NOTE_F4, 16, NOTE_G4, 4, NOTE_C4, 4, NOTE_DS4, 16, NOTE_F4, 16, //6
  NOTE_D4, -1, //7 and 8
  NOTE_F4, -4, NOTE_AS3, -4,
  NOTE_DS4, 16, NOTE_D4, 16, NOTE_F4, 4, NOTE_AS3, -4,
  NOTE_DS4, 16, NOTE_D4, 16, NOTE_C4, -1, //11 and 12

  //repeats from 5
  NOTE_G4, -4, NOTE_C4, -4, //5

  NOTE_DS4, 16, NOTE_F4, 16, NOTE_G4, 4, NOTE_C4, 4, NOTE_DS4, 16, NOTE_F4, 16, //6
  NOTE_D4, -1, //7 and 8
  NOTE_F4, -4, NOTE_AS3, -4,
  NOTE_DS4, 16, NOTE_D4, 16, NOTE_F4, 4, NOTE_AS3, -4,
  NOTE_DS4, 16, NOTE_D4, 16, NOTE_C4, -1, //11 and 12
  NOTE_G4, -4, NOTE_C4, -4,
  NOTE_DS4, 16, NOTE_F4, 16, NOTE_G4, 4,  NOTE_C4, 4, NOTE_DS4, 16, NOTE_F4, 16,

  NOTE_D4, -2, //15
  NOTE_F4, -4, NOTE_AS3, -4,
  NOTE_D4, -8, NOTE_DS4, -8, NOTE_D4, -8, NOTE_AS3, -8,
  NOTE_C4, -1,
  NOTE_C5, -2,
  NOTE_AS4, -2,
  NOTE_C4, -2,
  NOTE_G4, -2,
  NOTE_DS4, -2,
  NOTE_DS4, -4, NOTE_F4, -4,
  NOTE_G4, -1,

  NOTE_C5, -2, //28
  NOTE_AS4, -2,
  NOTE_C4, -2,
  NOTE_G4, -2,
  NOTE_DS4, -2,
  NOTE_DS4, -4, NOTE_D4, -4,
  NOTE_C5, 8, NOTE_G4, 8, NOTE_GS4, 16, NOTE_AS4, 16, NOTE_C5, 8, NOTE_G4, 8, NOTE_GS4, 16, NOTE_AS4, 16,
  NOTE_C5, 8, NOTE_G4, 8, NOTE_GS4, 16, NOTE_AS4, 16, NOTE_C5, 8, NOTE_G4, 8, NOTE_GS4, 16, NOTE_AS4, 16,

  REST, 4, NOTE_GS5, 16, NOTE_AS5, 16, NOTE_C6, 8, NOTE_G5, 8, NOTE_GS5, 16, NOTE_AS5, 16,
  NOTE_C6, 8, NOTE_G5, 16, NOTE_GS5, 16, NOTE_AS5, 16, NOTE_C6, 8, NOTE_G5, 8, NOTE_GS5, 16, NOTE_AS5, 16,};
int melody_christmas[] = {NOTE_C5, 4, //1
  NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
  NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
  NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
  NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
  NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,

  NOTE_F5, 2, NOTE_C5, 4, //8
  NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
  NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
  NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
  NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
  NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
  NOTE_F5, 2, NOTE_C5, 4,

  NOTE_F5, 4, NOTE_F5, 4, NOTE_F5, 4, //17
  NOTE_E5, 2, NOTE_E5, 4,
  NOTE_F5, 4, NOTE_E5, 4, NOTE_D5, 4,
  NOTE_C5, 2, NOTE_A5, 4,
  NOTE_AS5, 4, NOTE_A5, 4, NOTE_G5, 4,
  NOTE_C6, 4, NOTE_C5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
  NOTE_F5, 2, NOTE_C5, 4,
  NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
  NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,

  NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8, //27
  NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
  NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
  NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
  NOTE_F5, 2, NOTE_C5, 4,
  NOTE_F5, 4, NOTE_F5, 4, NOTE_F5, 4,
  NOTE_E5, 2, NOTE_E5, 4,
  NOTE_F5, 4, NOTE_E5, 4, NOTE_D5, 4,

  NOTE_C5, 2, NOTE_A5, 4, //36
  NOTE_AS5, 4, NOTE_A5, 4, NOTE_G5, 4,
  NOTE_C6, 4, NOTE_C5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
  NOTE_F5, 2, NOTE_C5, 4,
  NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
  NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
  NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,

  NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8, //45
  NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
  NOTE_F5, 2, NOTE_C5, 4,
  NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
  NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
  NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,

  NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8, //53
  NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
  NOTE_F5, 2, REST, 4};
int melody_mario[] = { NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 8, //1
  NOTE_G5, 4, REST, 4, NOTE_G4, 8, REST, 4,
  NOTE_C5, -4, NOTE_G4, 8, REST, 4, NOTE_E4, -4, // 3
  NOTE_A4, 4, NOTE_B4, 4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, -8, NOTE_E5, -8, NOTE_G5, -8, NOTE_A5, 4, NOTE_F5, 8, NOTE_G5, 8,
  REST, 8, NOTE_E5, 4, NOTE_C5, 8, NOTE_D5, 8, NOTE_B4, -4,
  NOTE_C5, -4, NOTE_G4, 8, REST, 4, NOTE_E4, -4, // repeats from 3
  NOTE_A4, 4, NOTE_B4, 4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, -8, NOTE_E5, -8, NOTE_G5, -8, NOTE_A5, 4, NOTE_F5, 8, NOTE_G5, 8,
  REST, 8, NOTE_E5, 4, NOTE_C5, 8, NOTE_D5, 8, NOTE_B4, -4,


  REST, 4, NOTE_G5, 8, NOTE_FS5, 8, NOTE_F5, 8, NOTE_DS5, 4, NOTE_E5, 8, //7
  REST, 8, NOTE_GS4, 8, NOTE_A4, 8, NOTE_C4, 8, REST, 8, NOTE_A4, 8, NOTE_C5, 8, NOTE_D5, 8,
  REST, 4, NOTE_DS5, 4, REST, 8, NOTE_D5, -4,
  NOTE_C5, 2, REST, 2,

  REST, 4, NOTE_G5, 8, NOTE_FS5, 8, NOTE_F5, 8, NOTE_DS5, 4, NOTE_E5, 8, //repeats from 7
  REST, 8, NOTE_GS4, 8, NOTE_A4, 8, NOTE_C4, 8, REST, 8, NOTE_A4, 8, NOTE_C5, 8, NOTE_D5, 8,
  REST, 4, NOTE_DS5, 4, REST, 8, NOTE_D5, -4,
  NOTE_C5, 2, REST, 2,

  NOTE_C5, 8, NOTE_C5, 4, NOTE_C5, 8, REST, 8, NOTE_C5, 8, NOTE_D5, 4, //11
  NOTE_E5, 8, NOTE_C5, 4, NOTE_A4, 8, NOTE_G4, 2,

  NOTE_C5, 8, NOTE_C5, 4, NOTE_C5, 8, REST, 8, NOTE_C5, 8, NOTE_D5, 8, NOTE_E5, 8, //13
  REST, 1,
  NOTE_C5, 8, NOTE_C5, 4, NOTE_C5, 8, REST, 8, NOTE_C5, 8, NOTE_D5, 4,
  NOTE_E5, 8, NOTE_C5, 4, NOTE_A4, 8, NOTE_G4, 2,
  NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 4,
  NOTE_G5, 4, REST, 4, NOTE_G4, 4, REST, 4,
  NOTE_C5, -4, NOTE_G4, 8, REST, 4, NOTE_E4, -4, // 19

  NOTE_A4, 4, NOTE_B4, 4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, -8, NOTE_E5, -8, NOTE_G5, -8, NOTE_A5, 4, NOTE_F5, 8, NOTE_G5, 8,
  REST, 8, NOTE_E5, 4, NOTE_C5, 8, NOTE_D5, 8, NOTE_B4, -4,

  NOTE_C5, -4, NOTE_G4, 8, REST, 4, NOTE_E4, -4, // repeats from 19
  NOTE_A4, 4, NOTE_B4, 4, NOTE_AS4, 8, NOTE_A4, 4,
  NOTE_G4, -8, NOTE_E5, -8, NOTE_G5, -8, NOTE_A5, 4, NOTE_F5, 8, NOTE_G5, 8,
  REST, 8, NOTE_E5, 4, NOTE_C5, 8, NOTE_D5, 8, NOTE_B4, -4,

  NOTE_E5, 8, NOTE_C5, 4, NOTE_G4, 8, REST, 4, NOTE_GS4, 4, //23
  NOTE_A4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_A4, 2,
  NOTE_D5, -8, NOTE_A5, -8, NOTE_A5, -8, NOTE_A5, -8, NOTE_G5, -8, NOTE_F5, -8,

  NOTE_E5, 8, NOTE_C5, 4, NOTE_A4, 8, NOTE_G4, 2, //26
  NOTE_E5, 8, NOTE_C5, 4, NOTE_G4, 8, REST, 4, NOTE_GS4, 4,
  NOTE_A4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_A4, 2,
  NOTE_B4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_F5, -8, NOTE_E5, -8, NOTE_D5, -8,
  NOTE_C5, 8, NOTE_E4, 4, NOTE_E4, 8, NOTE_C4, 2,

  NOTE_E5, 8, NOTE_C5, 4, NOTE_G4, 8, REST, 4, NOTE_GS4, 4, //repeats from 23
  NOTE_A4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_A4, 2,
  NOTE_D5, -8, NOTE_A5, -8, NOTE_A5, -8, NOTE_A5, -8, NOTE_G5, -8, NOTE_F5, -8,

  NOTE_E5, 8, NOTE_C5, 4, NOTE_A4, 8, NOTE_G4, 2, //26
  NOTE_E5, 8, NOTE_C5, 4, NOTE_G4, 8, REST, 4, NOTE_GS4, 4,
  NOTE_A4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_A4, 2,
  NOTE_B4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_F5, -8, NOTE_E5, -8, NOTE_D5, -8,
  NOTE_C5, 8, NOTE_E4, 4, NOTE_E4, 8, NOTE_C4, 2,
  NOTE_C5, 8, NOTE_C5, 4, NOTE_C5, 8, REST, 8, NOTE_C5, 8, NOTE_D5, 8, NOTE_E5, 8,
  REST, 1,

  NOTE_C5, 8, NOTE_C5, 4, NOTE_C5, 8, REST, 8, NOTE_C5, 8, NOTE_D5, 4, //33
  NOTE_E5, 8, NOTE_C5, 4, NOTE_A4, 8, NOTE_G4, 2,
  NOTE_E5, 8, NOTE_E5, 8, REST, 8, NOTE_E5, 8, REST, 8, NOTE_C5, 8, NOTE_E5, 4,
  NOTE_G5, 4, REST, 4, NOTE_G4, 4, REST, 4,
  NOTE_E5, 8, NOTE_C5, 4, NOTE_G4, 8, REST, 4, NOTE_GS4, 4,
  NOTE_A4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_A4, 2,
  NOTE_D5, -8, NOTE_A5, -8, NOTE_A5, -8, NOTE_A5, -8, NOTE_G5, -8, NOTE_F5, -8,

  NOTE_E5, 8, NOTE_C5, 4, NOTE_A4, 8, NOTE_G4, 2, //40
  NOTE_E5, 8, NOTE_C5, 4, NOTE_G4, 8, REST, 4, NOTE_GS4, 4,
  NOTE_A4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_A4, 2,
  NOTE_B4, 8, NOTE_F5, 4, NOTE_F5, 8, NOTE_F5, -8, NOTE_E5, -8, NOTE_D5, -8,
  NOTE_C5, 8, NOTE_E4, 4, NOTE_E4, 8, NOTE_C4, 2,

  //game over sound
  NOTE_C5, -4, NOTE_G4, -4, NOTE_E4, 4, //45
  NOTE_A4, -8, NOTE_B4, -8, NOTE_A4, -8, NOTE_GS4, -8, NOTE_AS4, -8, NOTE_GS4, -8,
  NOTE_G4, 8, NOTE_D4, 8, NOTE_E4, -2,};
int melody_tetris[] = {NOTE_E5, 4,  NOTE_B4, 8,  NOTE_C5, 8,  NOTE_D5, 4,  NOTE_C5, 8,  NOTE_B4, 8,
  NOTE_A4, 4,  NOTE_A4, 8,  NOTE_C5, 8,  NOTE_E5, 4,  NOTE_D5, 8,  NOTE_C5, 8,
  NOTE_B4, -4,  NOTE_C5, 8,  NOTE_D5, 4,  NOTE_E5, 4,
  NOTE_C5, 4,  NOTE_A4, 4,  NOTE_A4, 8,  NOTE_A4, 4,  NOTE_B4, 8,  NOTE_C5, 8,

  NOTE_D5, -4,  NOTE_F5, 8,  NOTE_A5, 4,  NOTE_G5, 8,  NOTE_F5, 8,
  NOTE_E5, -4,  NOTE_C5, 8,  NOTE_E5, 4,  NOTE_D5, 8,  NOTE_C5, 8,
  NOTE_B4, 4,  NOTE_B4, 8,  NOTE_C5, 8,  NOTE_D5, 4,  NOTE_E5, 4,
  NOTE_C5, 4,  NOTE_A4, 4,  NOTE_A4, 4, REST, 4,

  NOTE_E5, 4,  NOTE_B4, 8,  NOTE_C5, 8,  NOTE_D5, 4,  NOTE_C5, 8,  NOTE_B4, 8,
  NOTE_A4, 4,  NOTE_A4, 8,  NOTE_C5, 8,  NOTE_E5, 4,  NOTE_D5, 8,  NOTE_C5, 8,
  NOTE_B4, -4,  NOTE_C5, 8,  NOTE_D5, 4,  NOTE_E5, 4,
  NOTE_C5, 4,  NOTE_A4, 4,  NOTE_A4, 8,  NOTE_A4, 4,  NOTE_B4, 8,  NOTE_C5, 8,

  NOTE_D5, -4,  NOTE_F5, 8,  NOTE_A5, 4,  NOTE_G5, 8,  NOTE_F5, 8,
  NOTE_E5, -4,  NOTE_C5, 8,  NOTE_E5, 4,  NOTE_D5, 8,  NOTE_C5, 8,
  NOTE_B4, 4,  NOTE_B4, 8,  NOTE_C5, 8,  NOTE_D5, 4,  NOTE_E5, 4,
  NOTE_C5, 4,  NOTE_A4, 4,  NOTE_A4, 4, REST, 4,


  NOTE_E5, 2,  NOTE_C5, 2,
  NOTE_D5, 2,   NOTE_B4, 2,
  NOTE_C5, 2,   NOTE_A4, 2,
  NOTE_GS4, 2,  NOTE_B4, 4,  REST, 8,
  NOTE_E5, 2,   NOTE_C5, 2,
  NOTE_D5, 2,   NOTE_B4, 2,
  NOTE_C5, 4,   NOTE_E5, 4,  NOTE_A5, 2,
  NOTE_GS5, 2,};
int melody_never_gonna_give_you_up[] = {NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //1
  NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
  NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //repeat from 1
  NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
  NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
  REST,4, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_D5,8, NOTE_E5,8, NOTE_CS5,-8,
  NOTE_B4,16, NOTE_A4,2, REST,4, 

  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,4, NOTE_A4,8, //7
  NOTE_A5,8, REST,8, NOTE_A5,8, NOTE_E5,-4, REST,4, 
  NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,8, REST,8,
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, REST,4,
   
  NOTE_D5,2, NOTE_E5,8, NOTE_FS5,8, NOTE_D5,8, //13
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, NOTE_A4,4,
  REST,2, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8,
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,-8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //18
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,8, NOTE_A4,8, NOTE_A4,8, 
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  //23
  NOTE_E5,4, NOTE_D5,2, REST,4,
  REST,8, NOTE_B4,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,4, REST,8,
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
  REST,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_D5,8,
  
  REST,8, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //29
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4, REST,8,
  REST,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,4, NOTE_E5,-4, 
  NOTE_D5,2, NOTE_D5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, 
  NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_A4,8, NOTE_A4,4,

  REST,-4, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //35
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

   NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //40
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
   
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
  
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

  NOTE_E5,4, NOTE_D5,2, REST,4};




unsigned int notes_game_of_thrones = sizeof(melody_game_of_thrones) / sizeof(melody_game_of_thrones[0]) / 2;
unsigned int notes_christmas = sizeof(melody_christmas) / sizeof(melody_christmas[0]) / 2;
unsigned int notes_mario = sizeof(melody_mario) / sizeof(melody_mario[0]) / 2;
unsigned int notes_tetris = sizeof(melody_tetris) / sizeof(melody_tetris[0]) / 2;
unsigned int notes_never_gonna_give_you_up = sizeof(melody_never_gonna_give_you_up) / sizeof(melody_never_gonna_give_you_up[0]) / 2;

void playMelody(int melody[], int notes, int wholenote) {
  // iterate over the notes of the melody
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    int divider = melody[thisNote + 1];
    int noteDuration = (divider > 0) ? wholenote / divider : wholenote / abs(divider) * 1.5;

    tone(piezzoPin, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(piezzoPin);
  }
}

void setup() {
  Serial.begin(9600);
  // Initialize the TFT
  int16_t receibed=TFT.read16();
  TFT.initR(receibed);
  TFT.setRotation(1);
  TFT.fillScreen(ST7735_BLACK);
 
// Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi") ;

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Initialize NeoPixel and set the brightness
  strip.begin();
  strip.setBrightness(1);
}

void executeESPERA() {
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  TFT.setCursor(0, 54);

  TFT.print("A esperar...");  //FOR THE TFT
  for (int pixel = 0; pixel <= 2; pixel++) {
    strip.setPixelColor(pixel, strip.Color(0, 255, 0));
    strip.show();
    delay(10);
  }
  delay(1000);
  TFT.fillScreen(ST7735_BLACK);
  Serial.println("[EXECUTING] ESPERA");
}

void executeACEITE() {
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  TFT.setCursor(0, 54);
  strip.clear();
  TFT.print("Pedido aceite");  //FOR THE TFT

  for (int pixel = 0; pixel <= 29; pixel++) {
    strip.setPixelColor(pixel, strip.Color(255, 0, 0));
    strip.show();
    delay(50);
  }

  Serial.println("[EXECUTING] ACEITE");
}

void blinkTask() {
  static bool toggle = false;

  for (int pixel = 0; pixel < 29; pixel++) {
    strip.setPixelColor(pixel, toggle ? strip.Color(255, 0, 0) : strip.Color(0, 0, 0));
  }
  strip.show();

  toggle = !toggle;
}

void melodyTask() {
  // Aqui coloque o código para tocar a melodia
  
  // play We Wish You a Merry Christmas melody
  playMelody(melody_christmas, notes_christmas, wholenote_christmas);
  delay(3000);

  // play Game of Thrones melody
  playMelody(melody_game_of_thrones, notes_game_of_thrones, wholenote_game_of_thrones);
  // wait for 3 seconds
  delay(3000);

  // play Mario
  playMelody(melody_mario, notes_mario, wholenote_mario);
  delay(3000);

  // play Tetris
  playMelody(melody_tetris, notes_tetris, wholenote_tetris);
  delay(3000);

  
  // play Never gonna give you up
  playMelody(melody_never_gonna_give_you_up, notes_never_gonna_give_you_up, wholenote_never_gonna_give_you_up);
  delay(3000);

}
void executePREPARAR() {
/*  //============================================================================PIEZZO MUSICS==================================================================================
  strip.clear();  //cleans the 2 pixels left on for the ACEITE state
  while (1) {
    strip.clear();  // Set all pixel colors to 'off'
    // turn pixels to red one-by-one with delay between each pixel
    for (int pixel = 0; pixel < numNeopixel; pixel++) {    // for each pixel
      strip.setPixelColor(pixel, strip.Color(255, 0, 0));  // it only takes effect if pixels.show() is called
      strip.show();                                        // update to the NeoPixel Led Strip

      delay(500);  // 500ms pause between each pixel
    }
  }
  TFT.print("A preparar...)");
  
  //============================================================================PROGRESS BAR NEOPIXEL LED==================================================================================
  

  Serial.println("[EXCUTING] PREPARAR");
*/
unsigned long currentMillis = millis();

  // Executa a função de piscar a cada intervalo definido
  if (currentMillis - previousMillisBlink >= intervalBlink) {
    blinkTask();
    previousMillisBlink = currentMillis;
  }

  // Executa a função da melodia a cada intervalo definido
  if (currentMillis - previousMillisMelody >= intervalMelody) {
    melodyTask();
    previousMillisMelody = currentMillis;
  }

  // Outras ações podem ser adicionadas aqui, sem bloquear o código principal

}

void executePRONTO() {
  //============================================================FOR THE INFORMATION ON THE FTF=============================================================================================
  TFT.print("Pronto...)");
      // Aciona o buzzer na frequencia relativa ao Dó em Hz
    tone(buzzer,261);
     delay(10);

  //============================================================================PROGRESS BAR NEOPIXEL LED==================================================================================
  strip.clear();
  for (int pixel = 0; pixel < numNeopixel; pixel++) {    // for each pixel
    strip.setPixelColor(pixel, strip.Color(0, 255, 0));  // it only takes effect if pixels.show() is called
    strip.show();                                        // update to the NeoPixel Led Strip
  }
  //all leds turn green
  Serial.println("[EXCUTING] PRONTO");
}

void processState(int stateValue) {
  currentState = static_cast<State>(stateValue);
  Serial.print("Received State: ");
  Serial.println(currentState);
}

void executarComando() {
  switch (currentState) {
    case ESPERA:
      Serial.println("Executing [ESPERA] state");
      executeESPERA();
      break;

    case ACEITE:
      Serial.println("Executing [ACEITE] state");
      executeACEITE();
      break;

    case PREPARAR:
      Serial.println("Executing [PREPARAR] state");
      executePREPARAR();
      break;

    case PRONTO:
      Serial.println("Executing [PRONTO] state");
      executePRONTO();
      break;

    default:
      Serial.println("Unknown state!");
      break;
  }
}



void loop() {
  // Check for a client
  if (!client.connected()) {
    client = server.available();
    if (client) {
      Serial.println("Client connected");
    }
  }

  if (Serial.available() > 0) {
    int receivedValue = Serial.read() - '0';
    processState(receivedValue);
  }
  // Execute the corresponding function based on the current state
  delay(10);
  executarComando();
}
