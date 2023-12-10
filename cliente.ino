#include <WiFi.h>               //library for the wireless connections
#include <Adafruit_ST7735.h>    //library for the TFT
#include <Adafruit_NeoPixel.h>  //library for the neopixel led
#include <TaskScheduler.h>

#define piezzoPin 27  //pin for the Piezzo

//ssid and pass for the wireless connections (created by the ESP32 SERVER)
const char *ssid = "Cozinha";
const char *password = "123456789";

IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);


TaskHandle_t neoPixelMode;
TaskHandle_t piezzoMode;


//pin declaration for the TFT
#define cs    5    // Chip select line for TFT display
#define dc    4    // Data/Command line for TFT display
#define rst   16   // Reset line for TFT display (can be any GPIO pin)
#define TFT_MOSI  23   // SPI MOSI line
#define TFT_SCLK  18   // SPI Clock line
Adafruit_ST7735 TFT = Adafruit_ST7735(cs, dc, rst);

//pin declaration for the Neopixels
#define neoPixelPin 13  // The ESP32 pin GPIO16 connected to NeoPixel
#define numNeopixel 60  // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numNeopixel, neoPixelPin, NEO_GRB + NEO_KHZ800);

bool isPinLocked = false;

//====================================================================================================================================================================================
//====================================================================================================================================================================================
//====================================================================MUSIC NOTES AND FUNCITIONS TO PLAY==============================================================================
//====================================================================================================================================================================================
//====================================================================================================================================================================================

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
//====================================================================================================================================================================================
//====================================================================================================================================================================================
//============================================================================SETUP FUNCTION==========================================================================================
//====================================================================================================================================================================================
//====================================================================================================================================================================================

void setup() {
  Serial.begin(9600);
  // Initialize the TFT
  int16_t receibed=TFT.read16();
  TFT.initR(receibed);
  TFT.setRotation(3);
  TFT.fillScreen(ST7735_BLACK);
 
// Connect to WiFi
  Serial.println("Attempting to connect to WPA network...");
  WiFi.softAPConfig (local_IP, gateway, subnet);
  bool status = WiFi.softAP(ssid, password);
  if ( status != true) {
    Serial.println("Couldn't get a wifi connection");
    while(true);
  }
  else {
    server.begin();
    Serial.print("Connected to wifi. My address:");
    IPAddress myAddress = WiFi.localIP();
    Serial.println(myAddress);
  }

  // Initialize NeoPixel and set the brightness
  strip.begin();
  strip.setBrightness(1);
  strip.clear();
  strip.show();


  xTaskCreatePinnedToCore(neoPixelMODO, "NeoPixel", 1000, NULL, 1, &neoPixelMode, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(piezzoMODO, "Piezzo", 1000, NULL, 1, &piezzoMode, tskNO_AFFINITY);
  //vTaskSuspend(); para suspender a task
}

//====================================================================================================================================================================================
//====================================================================================================================================================================================
//============================================================================FUNCTIONS TASKS=========================================================================================
//====================================================================================================================================================================================
//====================================================================================================================================================================================

String lastState;// = "PRONTO"

void piezzoMODO(void*) {
  while(true){
  if(lastState == "PREPARAR"){
  // play We Wish You a Merry Christmas melody
  playMelody(melody_christmas, notes_christmas, wholenote_christmas);
  delay(1000);

  // play Game of Thrones melody
  playMelody(melody_game_of_thrones, notes_game_of_thrones, wholenote_game_of_thrones);
  // wait for 3 seconds
  delay(1000);

  // play Mario
  playMelody(melody_mario, notes_mario, wholenote_mario);
  delay(1000);

  // play Tetris
  playMelody(melody_tetris, notes_tetris, wholenote_tetris);
  delay(1000);

  // play Never gonna give you up
  playMelody(melody_never_gonna_give_you_up, notes_never_gonna_give_you_up, wholenote_never_gonna_give_you_up);
  delay(1000);

  }else if(lastState == "PRONTO"){
    tone(piezzoPin,261);
    delay(10);}
  }
}

void neoPixelMODO(void*) {
  while(true){
  if(lastState == "PREPARAR"){
  strip.clear();
  strip.show();

  for (int pixel = 0; pixel < numNeopixel; pixel++) {           
    strip.setPixelColor(pixel, strip.Color(255, 255, 0));  
    strip.show();                                           
    delay(500);
  }
  strip.show();

  }else if(lastState == "PRONTO"){

  for (int pixel = 0; pixel < numNeopixel; pixel++) {           
    strip.setPixelColor(pixel, strip.Color(0, 255, 0));
  }
  strip.show();  // update to the NeoPixel Led Strip
  }
  }
}

//====================================================================================================================================================================================
//====================================================================================================================================================================================
//============================================================================FUNCTIONS EXECUTE=======================================================================================
//====================================================================================================================================================================================
//====================================================================================================================================================================================

void executeESPERA() {  //TFT: "A esperar";NEOPIXEIS: Desligado;PIEZO: Desligado
  vTaskSuspend(neoPixelMode);            // To end the last tasks
  vTaskSuspend(piezzoMode);

  TFT.fillScreen(ST7735_BLACK);
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  TFT.setCursor(0, 54);
  TFT.print("A esperar...");  //FOR THE TFT

  strip.clear();
  strip.show();

  Serial.println("[EXECUTING] ESPERA");
}

void executeACEITE() {  //TFT: "Aceite";NEOPIXEIS: Loading com leds vermelhos;PIEZO: Desligado
  TFT.fillScreen(ST7735_BLACK);
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  TFT.setCursor(0, 54);
  TFT.print("Pedido aceite");  //FOR THE TFT

  strip.clear();
  strip.show();

  for (int pixel = 0; pixel < numNeopixel; pixel++) {           
    strip.setPixelColor(pixel, strip.Color(255, 0, 0));  
    strip.show();                                           
    delay(500);
  }
  strip.show();
  Serial.println("[EXECUTING] ACEITE");
}

void executePREPARAR() {  //TFT: "A preparar";NEOPIXEIS: Loading com leds amarelos;PIEZO:Toca música para entreter o cliente

  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  TFT.setCursor(0, 54);

  TFT.print("A preparar...");

  vTaskResume(neoPixelMode);
  vTaskResume(piezzoMode);
}

void executePRONTO() {  //TFT: "Pronto";NEOPIXEIS: Acendem todos verdes;PIEZO: Toque intermitente
  vTaskSuspend(neoPixelMode);            // To end the last tasks
  vTaskSuspend(piezzoMode);         // To end the last tasks
  
  //TFT.fillScreen(ST7735_BLACK);
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  TFT.setCursor(0, 54);
  TFT.print("Pronto!");

  vTaskResume(neoPixelMode);
  vTaskResume(piezzoMode);

}
//====================================================================================================================================================================================
//====================================================================================================================================================================================
//============================================================================LOOP FUNCTION===========================================================================================
//====================================================================================================================================================================================
//====================================================================================================================================================================================

void loop() {
  String currentState;  
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {

    if (client.connected()) {
        for (int i = 0; i < 255; i++) {
          char c = client.read();
          if (!('A' <= c && c <= 'Z')) { break; } // If there isn't a byte available, function returns -1.

          currentState += String(c);
        }
    }
    client.write(currentState.c_str());
    client.stop();
    if(currentState != lastState){
      lastState = currentState;
    }
  }

    if (lastState.indexOf("ESPERA") >= 0) {
      Serial.println("Executing [ESPERA] state");
      executeESPERA();
    } else if (lastState.indexOf("ACEITE") >= 0) {
      Serial.println("Executing [ACEITE] state");
      executeACEITE();
    } else if (lastState.indexOf("PREPARAR") >= 0) {
      Serial.println("Executing [PREPARAR] state");
      executePREPARAR();
    } else if (lastState.indexOf("PRONTO") >= 0) {
      Serial.println("Executing [PRONTO] state");
      executePRONTO();
    } else {
      Serial.println("Unknown state!");
    }
}
