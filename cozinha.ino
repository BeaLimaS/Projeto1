#include <TFT.h>
#include <SPI.h>
#include <JPEGDecoder.h>  // JPEG decoder library

#define minumum(a, b) (((a) < (b) ? (a) : (b)))

#define buttonPin1 22  //button pin for the Cozinha
#define piezzoPin 27   //pin for the Piezzo

//pin declaration for the TFT
#define cs 23
#define dc 25
#define rst 26
#define sd 34  //pin for the SD Card

TFT TFTscreen = TFT(cs, dc, rst);

//notes for the piezzo

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#define REST 0

// change this to make the song slower or faster
int tempo_game_of_thrones = 85;
int tempo_christmas = 140;
int tempo_mario = 200;
int tempo_tetris = 144;
int tempo_never_gonna_give_you_up = 114;

//variables for the State Machine
bool lastState = 0;  //last state of the terminal

enum States { ACEITA,
              PREPARAR,
              PRONTO,
              ESPERA };
States currentState = ESPERA;
bool buttonPressed = 0;

void detectPress() {  //function to detect if the button is pressed
  bool currentState = digitalRead(buttonPin);
  if ((lastState == LOW) && (currentState == HIGH)) {
    buttonPressed = 1;
  } else {
    buttonPressed = 0;
  }
  lastState = currentState;
}

void jpegInfo() {  //informations of the image
  Serial.println(F("==============="));
  Serial.println(F("JPEG image info"));
  Serial.println(F("==============="));
  Serial.print(F("Width      :"));
  Serial.println(JpegDec.width);
  Serial.print(F("Height     :"));
  Serial.println(JpegDec.height);
  Serial.print(F("Components :"));
  Serial.println(JpegDec.comps);
  Serial.print(F("MCU / row  :"));
  Serial.println(JpegDec.MCUSPerRow);
  Serial.print(F("MCU / col  :"));
  Serial.println(JpegDec.MCUSPerCol);
  Serial.print(F("Scan type  :"));
  Serial.println(JpegDec.scanType);
  Serial.print(F("MCU width  :"));
  Serial.println(JpegDec.MCUWidth);
  Serial.print(F("MCU height :"));
  Serial.println(JpegDec.MCUHeight);
  Serial.println(F("==============="));
}

//Function for displaying the image on the TFT
void renderJPEG(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while (JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right and bottom edges
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image block if it will fit on the screen
    if ((mcu_x + win_w) <= TFTscreen.width() && (mcu_y + win_h) <= TFTscreen.height()) {
      // open a window onto the screen to paint the pixels into
      //TFTscreen.setAddrWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);
      TFTscreen.setAddrWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);
      // push all the image block pixels to the screen
      while (mcu_pixels--) TFTscreen.pushColor(*pImg++);  // Send to TFT 16 bits at a time
    }

    // stop drawing blocks if the bottom of the screen has been reached
    // the abort function will close the file
    else if ((mcu_y + win_h) >= TFTscreen.height())
      JpegDec.abort();
  }
}

void music() {
  int melody_game_of_thrones[] = {
    NOTE_G4,
8,
    NOTE_C4,
    8,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,  //1
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_E4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_E4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_E4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    8,
    NOTE_C4,
    8,
    NOTE_E4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    -4,
    NOTE_C4,
    -4,  //5

    NOTE_DS4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    4,
    NOTE_C4,
    4,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,  //6
    NOTE_D4,
    -1,  //7 and 8
    NOTE_F4,
    -4,
    NOTE_AS3,
    -4,
    NOTE_DS4,
    16,
    NOTE_D4,
    16,
    NOTE_F4,
    4,
    NOTE_AS3,
    -4,
    NOTE_DS4,
    16,
    NOTE_D4,
    16,
    NOTE_C4,
    -1,  //11 and 12

    //repeats from 5
    NOTE_G4,
    -4,
    NOTE_C4,
    -4,  //5

    NOTE_DS4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    4,
    NOTE_C4,
    4,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,  //6
    NOTE_D4,
    -1,  //7 and 8
    NOTE_F4,
    -4,
    NOTE_AS3,
    -4,
    NOTE_DS4,
    16,
    NOTE_D4,
    16,
    NOTE_F4,
    4,
    NOTE_AS3,
    -4,
    NOTE_DS4,
    16,
    NOTE_D4,
    16,
    NOTE_C4,
    -1,  //11 and 12
    NOTE_G4,
    -4,
    NOTE_C4,
    -4,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,
    NOTE_G4,
    4,
    NOTE_C4,
    4,
    NOTE_DS4,
    16,
    NOTE_F4,
    16,

    NOTE_D4,
    -2,  //15
    NOTE_F4,
    -4,
    NOTE_AS3,
    -4,NOTE_D4,-8,NOTE_DS4,-8,NOTE_D4,-8,NOTE_AS3,-8,NOTE_C4,-1,NOTE_C5,-2,NOTE_AS4,-2,NOTE_C4,-2,NOTE_G4,-2,NOTE_DS4,-2,NOTE_DS4,-4,NOTE_F4,-4,NOTE_G4,-1,
    
    NOTE_C5,-2,  //28NOTE_AS4,-2,NOTE_C4,-2,NOTE_G4,-2,
    NOTE_DS4,-2,NOTE_DS4,-4,NOTE_D4,-4,NOTE_C5,8,NOTE_G4,8,
    NOTE_GS4,16,NOTE_AS4,16,NOTE_C5,8,NOTE_G4,8,NOTE_GS4,16,NOTE_AS4,16,NOTE_C5,8,
    NOTE_G4,8,NOTE_GS4,16,NOTE_AS4,16,NOTE_C5,8,NOTE_G4,8,NOTE_GS4,16,NOTE_AS4,16,

    REST,4,NOTE_GS5,16,NOTE_AS5,16,NOTE_C6,8,NOTE_G5,8,NOTE_GS5,16,NOTE_AS5,16,NOTE_C6,8,NOTE_G5,16,NOTE_GS5,16,NOTE_AS5,16,NOTE_C6,8,NOTE_G5,8, NOTE_GS5,16,NOTE_AS5,16,
  };
  int notes_game_of_thrones = sizeof(melody_game_of_thrones) / sizeof(melody_game_of_thrones[0]) / 2;
  int wholenote_game_of_thrones = (60000 * 4) / tempo_game_of_thrones;

  int melody_christmas[] = {
    NOTE_C5, 4,  //1
    NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
    NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
    NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
    NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
    NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
    NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,

    NOTE_F5, 2, NOTE_C5, 4,  //8
    NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
    NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
    NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
    NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
    NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
    NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
    NOTE_F5, 2, NOTE_C5, 4,

    NOTE_F5, 4, NOTE_F5, 4, NOTE_F5, 4,  //17
    NOTE_E5, 2, NOTE_E5, 4,
    NOTE_F5, 4, NOTE_E5, 4, NOTE_D5, 4,
    NOTE_C5, 2, NOTE_A5, 4,
    NOTE_AS5, 4, NOTE_A5, 4, NOTE_G5, 4,
    NOTE_C6, 4, NOTE_C5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
    NOTE_F5, 2, NOTE_C5, 4,
    NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
    NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,

    NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,  //27
    NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
    NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
    NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
    NOTE_F5, 2, NOTE_C5, 4,
    NOTE_F5, 4, NOTE_F5, 4, NOTE_F5, 4,
    NOTE_E5, 2, NOTE_E5, 4,
    NOTE_F5, 4, NOTE_E5, 4, NOTE_D5, 4,

    NOTE_C5, 2, NOTE_A5, 4,  //36
    NOTE_AS5, 4, NOTE_A5, 4, NOTE_G5, 4,
    NOTE_C6, 4, NOTE_C5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
    NOTE_F5, 2, NOTE_C5, 4,
    NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
    NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
    NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
    NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,

    NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,  //45
    NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
    NOTE_F5, 2, NOTE_C5, 4,
    NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
    NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
    NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
    NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,

    NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,  //53
    NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4,
    NOTE_F5, 2, REST, 4
  };

  int notes_christmas = sizeof(melody_christmas) / sizeof(melody_christmas[0]) / 2;
  int wholenote_christmas = (60000 * 4) / tempo_christmas;

  int melody_mario[] = {
    NOTE_E5,
    8,
    NOTE_E5,
    8,
    REST,
    8,
    NOTE_E5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_E5,
    8,  //1
    NOTE_G5,
    4,
    REST,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_C5,
    -4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_E4,
    -4,  // 3
    NOTE_A4,
    4,
    NOTE_B4,
    4,
    NOTE_AS4,
    8,
    NOTE_A4,
    4,
    NOTE_G4,
    -8,
    NOTE_E5,
    -8,
    NOTE_G5,
    -8,
    NOTE_A5,
    4,
    NOTE_F5,
    8,
    NOTE_G5,
    8,
    REST,
    8,
    NOTE_E5,
    4,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    NOTE_B4,
    -4,
    NOTE_C5,
    -4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_E4,
    -4,  // repeats from 3
    NOTE_A4,
    4,
    NOTE_B4,
    4,
    NOTE_AS4,
    8,
    NOTE_A4,
    4,
    NOTE_G4,
    -8,
    NOTE_E5,
    -8,
    NOTE_G5,
    -8,
    NOTE_A5,
    4,
    NOTE_F5,
    8,
    NOTE_G5,
    8,
    REST,
    8,
    NOTE_E5,
    4,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    NOTE_B4,
    -4,


    REST,
    4,
    NOTE_G5,
    8,
    NOTE_FS5,
    8,
    NOTE_F5,
    8,
    NOTE_DS5,
    4,
    NOTE_E5,
    8,  //7
    REST,
    8,
    NOTE_GS4,
    8,
    NOTE_A4,
    8,
    NOTE_C4,
    8,
    REST,
    8,
    NOTE_A4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    REST,
    4,
    NOTE_DS5,
    4,
    REST,
    8,
    NOTE_D5,
    -4,
    NOTE_C5,
    2,
    REST,
    2,

    REST,
    4,
    NOTE_G5,
    8,
    NOTE_FS5,
    8,
    NOTE_F5,
    8,
    NOTE_DS5,
    4,
    NOTE_E5,
    8,  //repeats from 7
    REST,
    8,
    NOTE_GS4,
    8,
    NOTE_A4,
    8,
    NOTE_C4,
    8,
    REST,
    8,
    NOTE_A4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    REST,
    4,
    NOTE_DS5,
    4,
    REST,
    8,
    NOTE_D5,
    -4,
    NOTE_C5,
    2,
    REST,
    2,

    NOTE_C5,
    8,
    NOTE_C5,
    4,
    NOTE_C5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,  //11
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_A4,
    8,
    NOTE_G4,
    2,

    NOTE_C5,
    8,
    NOTE_C5,
    4,
    NOTE_C5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    NOTE_E5,
    8,  //13
    REST,
    1,
    NOTE_C5,
    8,
    NOTE_C5,
    4,
    NOTE_C5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_A4,
    8,
    NOTE_G4,
    2,
    NOTE_E5,
    8,
    NOTE_E5,
    8,
    REST,
    8,
    NOTE_E5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_G5,
    4,
    REST,
    4,
    NOTE_G4,
    4,
    REST,
    4,
    NOTE_C5,
    -4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_E4,
    -4,  // 19

    NOTE_A4,
    4,
    NOTE_B4,
    4,
    NOTE_AS4,
    8,
    NOTE_A4,
    4,
    NOTE_G4,
    -8,
    NOTE_E5,
    -8,
    NOTE_G5,
    -8,
    NOTE_A5,
    4,
    NOTE_F5,
    8,
    NOTE_G5,
    8,
    REST,
    8,
    NOTE_E5,
    4,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    NOTE_B4,
    -4,

    NOTE_C5,
    -4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_E4,
    -4,  // repeats from 19
    NOTE_A4,
    4,
    NOTE_B4,
    4,
    NOTE_AS4,
    8,
    NOTE_A4,
    4,
    NOTE_G4,
    -8,
    NOTE_E5,
    -8,
    NOTE_G5,
    -8,
    NOTE_A5,
    4,
    NOTE_F5,
    8,
    NOTE_G5,
    8,
    REST,
    8,
    NOTE_E5,
    4,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    NOTE_B4,
    -4,

    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_GS4,
    4,  //23
    NOTE_A4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_A4,
    2,
    NOTE_D5,
    -8,
    NOTE_A5,
    -8,
    NOTE_A5,
    -8,
    NOTE_A5,
    -8,
    NOTE_G5,
    -8,
    NOTE_F5,
    -8,

    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_A4,
    8,
    NOTE_G4,
    2,  //26
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_GS4,
    4,
    NOTE_A4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_A4,
    2,
    NOTE_B4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_F5,
    -8,
    NOTE_E5,
    -8,
    NOTE_D5,
    -8,
    NOTE_C5,
    8,
    NOTE_E4,
    4,
    NOTE_E4,
    8,
    NOTE_C4,
    2,

    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_GS4,
    4,  //repeats from 23
    NOTE_A4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_A4,
    2,
    NOTE_D5,
    -8,
    NOTE_A5,
    -8,
    NOTE_A5,
    -8,
    NOTE_A5,
    -8,
    NOTE_G5,
    -8,
    NOTE_F5,
    -8,

    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_A4,
    8,
    NOTE_G4,
    2,  //26
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_GS4,
    4,
    NOTE_A4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_A4,
    2,
    NOTE_B4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_F5,
    -8,
    NOTE_E5,
    -8,
    NOTE_D5,
    -8,
    NOTE_C5,
    8,
    NOTE_E4,
    4,
    NOTE_E4,
    8,
    NOTE_C4,
    2,
    NOTE_C5,
    8,
    NOTE_C5,
    4,
    NOTE_C5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    8,
    NOTE_E5,
    8,
    REST,
    1,

    NOTE_C5,
    8,
    NOTE_C5,
    4,
    NOTE_C5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,  //33
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_A4,
    8,
    NOTE_G4,
    2,
    NOTE_E5,
    8,
    NOTE_E5,
    8,
    REST,
    8,
    NOTE_E5,
    8,
    REST,
    8,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_G5,
    4,
    REST,
    4,
    NOTE_G4,
    4,
    REST,
    4,
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_GS4,
    4,
    NOTE_A4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_A4,
    2,
    NOTE_D5,
    -8,
    NOTE_A5,
    -8,
    NOTE_A5,
    -8,
    NOTE_A5,
    -8,
    NOTE_G5,
    -8,
    NOTE_F5,
    -8,

    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_A4,
    8,
    NOTE_G4,
    2,  //40
    NOTE_E5,
    8,
    NOTE_C5,
    4,
    NOTE_G4,
    8,
    REST,
    4,
    NOTE_GS4,
    4,
    NOTE_A4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_A4,
    2,
    NOTE_B4,
    8,
    NOTE_F5,
    4,
    NOTE_F5,
    8,
    NOTE_F5,
    -8,
    NOTE_E5,
    -8,
    NOTE_D5,
    -8,
    NOTE_C5,
    8,
    NOTE_E4,
    4,
    NOTE_E4,
    8,
    NOTE_C4,
    2,

    //game over sound
    NOTE_C5,
    -4,
    NOTE_G4,
    -4,
    NOTE_E4,
    4,  //45
    NOTE_A4,
    -8,
    NOTE_B4,
    -8,
    NOTE_A4,
    -8,
    NOTE_GS4,
    -8,
    NOTE_AS4,
    -8,
    NOTE_GS4,
    -8,
    NOTE_G4,
    8,
    NOTE_D4,
    8,
    NOTE_E4,
    -2,
  };

  int notes_mario = sizeof(melody_mario) / sizeof(melody_mario[0]) / 2;
  int wholenote_mario = (60000 * 4) / tempo_mario;

  int melody_tetris[] = {
    NOTE_E5,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_C5,
    8,
    NOTE_B4,
    8,
    NOTE_A4,
    4,
    NOTE_A4,
    8,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_D5,
    8,
    NOTE_C5,
    8,
    NOTE_B4,
    -4,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_E5,
    4,
    NOTE_C5,
    4,
    NOTE_A4,
    4,
    NOTE_A4,
    8,
    NOTE_A4,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,

    NOTE_D5,
    -4,
    NOTE_F5,
    8,
    NOTE_A5,
    4,
    NOTE_G5,
    8,
    NOTE_F5,
    8,
    NOTE_E5,
    -4,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_D5,
    8,
    NOTE_C5,
    8,
    NOTE_B4,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_E5,
    4,
    NOTE_C5,
    4,
    NOTE_A4,
    4,
    NOTE_A4,
    4,
    REST,
    4,

    NOTE_E5,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_C5,
    8,
    NOTE_B4,
    8,
    NOTE_A4,
    4,
    NOTE_A4,
    8,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_D5,
    8,
    NOTE_C5,
    8,
    NOTE_B4,
    -4,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_E5,
    4,
    NOTE_C5,
    4,
    NOTE_A4,
    4,
    NOTE_A4,
    8,
    NOTE_A4,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,

    NOTE_D5,
    -4,
    NOTE_F5,
    8,
    NOTE_A5,
    4,
    NOTE_G5,
    8,
    NOTE_F5,
    8,
    NOTE_E5,
    -4,
    NOTE_C5,
    8,
    NOTE_E5,
    4,
    NOTE_D5,
    8,
    NOTE_C5,
    8,
    NOTE_B4,
    4,
    NOTE_B4,
    8,
    NOTE_C5,
    8,
    NOTE_D5,
    4,
    NOTE_E5,
    4,
    NOTE_C5,
    4,
    NOTE_A4,
    4,
    NOTE_A4,
    4,
    REST,
    4,


    NOTE_E5,
    2,
    NOTE_C5,
    2,
    NOTE_D5,
    2,
    NOTE_B4,
    2,
    NOTE_C5,
    2,
    NOTE_A4,
    2,
    NOTE_GS4,
    2,
    NOTE_B4,
    4,
    REST,
    8,
    NOTE_E5,
    2,
    NOTE_C5,
    2,
    NOTE_D5,
    2,
    NOTE_B4,
    2,
    NOTE_C5,
    4,
    NOTE_E5,
    4,
    NOTE_A5,
    2,
    NOTE_GS5,
    2,
  };

  int notes_tetris = sizeof(melody_tetris) / sizeof(melody_tetris[0]) / 2;
  int wholenote_tetris = (60000 * 4) / tempo_tetris;


  int melody_never_gonna_give_you_up[] = {
    NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 4,  //1
    NOTE_E5, -4, NOTE_FS5, -4, NOTE_A5, 16, NOTE_G5, 16, NOTE_FS5, 8,
    NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 2,
    NOTE_A4, 16, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 8, NOTE_D5, 16,
    NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 4,  //repeat from 1
    NOTE_E5, -4, NOTE_FS5, -4, NOTE_A5, 16, NOTE_G5, 16, NOTE_FS5, 8,
    NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 2,
    NOTE_A4, 16, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 8, NOTE_D5, 16,
    REST, 4, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_D5, 8, NOTE_E5, 8, NOTE_CS5, -8,
    NOTE_B4, 16, NOTE_A4, 2, REST, 4,

    REST, 8, NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 4, NOTE_A4, 8,  //7
    NOTE_A5, 8, REST, 8, NOTE_A5, 8, NOTE_E5, -4, REST, 4,
    NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_D5, 8, NOTE_E5, 8, REST, 8,
    REST, 8, NOTE_CS5, 8, NOTE_B4, 8, NOTE_A4, -4, REST, 4,
    REST, 8, NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_A4, 4,
    NOTE_E5, 8, NOTE_E5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 4, REST, 4,

    NOTE_D5, 2, NOTE_E5, 8, NOTE_FS5, 8, NOTE_D5, 8,  //13
    NOTE_E5, 8, NOTE_E5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 4, NOTE_A4, 4,
    REST, 2, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8,
    REST, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,

    NOTE_E5, -8, NOTE_E5, -8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, -8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,  //18
    NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 8, NOTE_A4, 8, NOTE_A4, 8,
    NOTE_E5, 4, NOTE_D5, 2, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_A5, 4, NOTE_CS5, 8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, 8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,

    NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 4, NOTE_A4, 8,  //23
    NOTE_E5, 4, NOTE_D5, 2, REST, 4,
    REST, 8, NOTE_B4, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_D5, 8, NOTE_E5, 4, REST, 8,
    REST, 8, NOTE_CS5, 8, NOTE_B4, 8, NOTE_A4, -4, REST, 4,
    REST, 8, NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_A4, 4,
    REST, 8, NOTE_A5, 8, NOTE_A5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 8, NOTE_D5, 8,

    REST, 8, NOTE_A4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8,  //29
    REST, 8, NOTE_CS5, 8, NOTE_B4, 8, NOTE_A4, -4, REST, 4,
    NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_A4, 4, REST, 8,
    REST, 8, NOTE_E5, 8, NOTE_E5, 8, NOTE_FS5, 4, NOTE_E5, -4,
    NOTE_D5, 2, NOTE_D5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 4,
    NOTE_E5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 8, NOTE_A4, 8, NOTE_A4, 4,

    REST, -4, NOTE_A4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8,  //35
    REST, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_E5, -8, NOTE_E5, -8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, 8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 4, NOTE_A4, 8,

    NOTE_E5, 4, NOTE_D5, 2, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,  //40
    NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_A5, 4, NOTE_CS5, 8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, 8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 4, NOTE_A4, 8,
    NOTE_E5, 4, NOTE_D5, 2, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,

    NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,  //45
    NOTE_A5, 4, NOTE_CS5, 8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, 8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 4, NOTE_A4, 8,
    NOTE_E5, 4, NOTE_D5, 2, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,  //45

    NOTE_A5, 4, NOTE_CS5, 8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, 8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
    NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 4, NOTE_A4, 8,

    NOTE_E5, 4, NOTE_D5, 2, REST, 4
  };

  int notes_never_gonna_give_you_up = sizeof(melody_never_gonna_give_you_up) / sizeof(melody_never_gonna_give_you_up[0]) / 2;
  int wholenote_never_gonna_give_you_up = (60000 * 4) / tempo_never_gonna_give_you_up;

  void playMelody(int melody[], int notes, int wholenote) {
    // iterate over the notes of the melody
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
      int divider = melody[thisNote + 1];
      int noteDuration = (divider > 0) ? wholenote / divider : wholenote / abs(divider) * 1.5;

      tone(buzzer, melody[thisNote], noteDuration * 0.9);
      delay(noteDuration);
      noTone(buzzer);
    }

    // play Game of Thrones melody
    playMelody(melody_game_of_thrones, notes_game_of_thrones, wholenote_game_of_thrones);
    // wait for 3 seconds
    delay(3000);

    // play We Wish You a Merry Christmas melody
    playMelody(melody_christmas, notes_christmas, wholenote_christmas);
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

  void setup() {
    Serial.begin(9600);

    pinMode(buttonPin, INPUT);  //BUTTON_PIN is an INPUT


    TFTscreen.begin();
    TFTscreen.background(204, 51, 77);
    TFTscreen.setTextColor(51, 178, 204);

    //see if the SD Card has been detected
    if (!SD.begin(sd)) {
      Serial.println(F("failed!"));
      while (1)
        ;  // SD initialisation failed so wait here
    }
    Serial.println(F("OK!"));

    //if the SD Card was detected, see if the image is on the card
    if (SD.exists("preparar.jpg")) {
      Serial.println("preparar.jpg found on SD card.");
    } else {
      Serial.println("preparar.jpg not found on SD card.");
      while (1)
        ;  // Image file missing so stay here
    }
  }

  void loop() {
    switch (currentState) {
      case ESPERA:
        {
          //FALTA METER O QUE O CLIENTE VAI RECEBER
          //ou seja, vai desligar tudo o que estava ligado (entra em modo de espera)
          if (detectPress) { currentState = ACEITA; }
        }

      case ACEITA:
        {
          //FALTA METER O QUE O CLIENTE VAI RECEBER
          //ou seja, liga 2 primeiros LEDs do NeoPixel; Som de início == fur elise
          if (detectPress) { currentState = PREPARAR; }
        }

      case PREPARAR:
        {
          //FALTA METER O QUE O CLIENTE VAI RECEBER
          //ou seja, passar imagens no TFT; loading atraves dos NeoPixeis (barra de progresso); piezzo toca musicas
          //MÚSICAS==> Game of Thrones || Super Mario || Never Gonna Give You Up || Tetris || Merry Christmas

          //==========================================================FOR THE IMAGES ON THE FTF==================================================================================================
          // open the image file
          File jpgFile = SD.open("arduino.jpg", FILE_READ);

          // initialise the decoder to give access to image information
          JpegDec.decodeSdFile(jpgFile);

          // print information about the image to the serial port
          jpegInfo();

          // render the image onto the screen at coordinate 0,0
          renderJPEG(0, 0);

          // wait a little bit before clearing the screen to random color and drawing again
          delay(4000);

          // clear screen
          TFTscreen.fillScreen(random(0xFFFF));
          displayImage();
          //==================================================================MUSICS FOR THE PIEZZO==============================================================================================

          music();

          //=====================================================================================================================================================================================

          if (detectPress) { currentState = PRONTO; }
        }

      case PRONTO:
        {
          //FALTA METER O QUE O CLIENTE VAI RECEBER
          //ou seja, Piezzo toca; NeoPixeis piscam; TFT mostra imagem de pronto
          if (detectPress) { currentState = ESPERA; }
        }
    }
  }
