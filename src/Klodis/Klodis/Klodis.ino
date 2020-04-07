/* (C)2020 Benji Schmid

    Klodis is a clock to manage Modis gaming habits.
    Balances between PC time and "outside" time.
    2x outside = pc

    Parts:
      Arduino Uno
      2.8" TFT Touchscreen
*/

//#include <TouchScreen.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

// Assign human-readable names to some common 16-bit color values:
#define BLACK     0x0000
#define DARKGREY  0x7BEF
#define LIGHTGREY 0xBDF7
#define WHITE     0xFFFF
#define BROWN     0x79E0
#define RED       0xF800
#define GREEN     0x07E0
#define BLUE      0x001F
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFBE0

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 570);

void setup() {
  Serial.begin(9600);
  
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

}

void loop() {
  
}
