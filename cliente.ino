#include <Wire.h>
#include <Adafruit_ST7735.h>    //library for the TFT
#include <Adafruit_NeoPixel.h>  //library for the neopixel led
//#include <TonePitch.h>          //library for the Buzzer

#define piezzoPin 27  //pin for the Piezzo

//pin declaration for the TFT
#define cs 33
#define dc 25
#define rst 26
Adafruit_ST7735 TFTscreen = Adafruit_ST7735(cs, dc, rst);

//pin declaration for the Neopixels
#define neoPixelPin 16  // The ESP32 pin GPIO16 connected to NeoPixel
#define numNeopixel 12  // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numNeopixel, neoPixelPin, NEO_GRB + NEO_KHZ800);

//ssid and pass for the wireless connections (created by the ESP32 SERVER)
/*char ssid[] = "esp32";
char pass[] = "pass";*/

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  //font, backround color, text color and text size for the TFT
  TFTscreen.setFont();
  TFTscreen.fillScreen(ST77XX_WHITE);
  TFTscreen.setTextColor(ST77XX_RED);
  TFTscreen.setTextSize(1);
}

void loop() {
  if (Serial1.available()) {
    String receivedCommand = Serial1.readStringUntil('\n');
    receivedCommand.trim();  // removes adicional black spaces

    Serial.print("[COMMAND RECIVED] ");
    Serial.println(receivedCommand);

    // Executes action based on the reciveing command
    if (receivedCommand.equals("ESPERA")) {
      executeESPERA();
    } else if (receivedCommand.equals("ACEITE")) {
      executeACEITE();
    } else if (receivedCommand.equals("PREPARAR")) {
      executePREPARAR();
    } else if (receivedCommand.equals("PRONTO")) {
      executePRONTO();
    } else {
      unknownCommand();
    }
  }
}

void executeESPERA() {

  Serial.println("[EXECUTING] ESPERA");
}

void executeACEITE() {
  //============================================================================PROGRESS BAR NEOPIXEL LED==================================================================================
  strip.clear();
  for (int pixel = 0; pixel <= 2; pixel++) {
    strip.setPixelColor(pixel, strip.Color(0, 255, 0));
    strip.show();
    delay(50);
  }

  Serial.println("[EXECUTING] ACEITE");
}

void executePREPARAR() {
  //============================================================================PROGRESS BAR NEOPIXEL LED==================================================================================
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
  Serial.println("[EXCUTING] PREPARAR");
  //============================================================FOR THE INFORMATION ON THE FTF=============================================================================================
  TFTscreen.print("A preparar");
  while (1) {
    for (int i = 0; i < 4; i++) {
      TFTscreen.print(".");
    }
  TFTscreen.fillScreen(ST77XX_WHITE); //this library doesn't have a clear function so we fill the screen the color of the background
  }
}

void executePRONTO() {
  // Lógica para o novo PRONTO
  Serial.println("[EXCUTING] PRONTO");
}

void unknownCommand() {
  // Logic for unknown commands
  Serial.println("[ERROR] Unknown Command");
}
