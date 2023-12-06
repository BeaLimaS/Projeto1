#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Pin configuration for TFT display
#define TFT_CS    5    // Chip select line for TFT display
#define TFT_DC    4    // Data/Command line for TFT display
#define TFT_RST   16   // Reset line for TFT display (can be any GPIO pin)
#define TFT_MOSI  23   // SPI MOSI line
#define TFT_SCLK  18   // SPI Clock line

// Create an instance of the TFT display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing TFT display...");

  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);  // You may need to adjust the rotation based on your specific display

  // Set the background color to black
  tft.fillScreen(ST7735_BLACK);

  // Centralizar o texto "A preparar"
  int textWidth = tft.width() / 2 - (16 * 5);  // Metade da largura do display menos metade do comprimento do texto
  int textHeight = tft.height() / 2 - 8;      // Metade da altura do display menos metade da altura do texto
  int textSize = 2;                           // Tamanho do texto

  tft.setCursor(textWidth, textHeight); // Definir a posição do cursor centralizada
  tft.setTextSize(textSize);             // Definir o tamanho do texto
  tft.setTextColor(ST7735_WHITE);        // Definir a cor do texto (branco)
  tft.print("A preparar...");

  Serial.println("TFT display initialized successfully!");

  // Your setup code goes here
}

void loop() {
  // Your main loop code goes here
}
