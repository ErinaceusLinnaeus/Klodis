/* (C)2020 Benji

    Klodis is a clock to manage Modis gaming habits.
    Balances between PC time and "outside" time.
    2x outside = pc

    - Unfortunatly, for now, the PC really has to be disconnected
      from the power source for the USB port to "shut off".
      
    - Also the power source needs to be connected to my pc.
      I don't know why.

    Parts:
      Arduino Uno
      2.8" TFT Touchscreen
      DS3231 clock
*/

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_TFTLCD.h>  // Hardware-specific library
#include <DS3231.h>           // clock library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_RESET PC6 //A4 // Can alternately just connect to Arduino's reset pin (PC6)
#define LCD_CS A3     // Chip Select goes to Analog 3
#define LCD_CD A2     // Command/Data goes to Analog 2
#define LCD_WR A1     // LCD Write goes to Analog 1
#define LCD_RD A0     // LCD Read goes to Analog 0

// Init the display
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

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

// Stores the minutes that need to be balanced
int timer = 0;

// Enum to see if the PC is turned on
typedef enum {OFF, ON} PCmode;

// Integer to check for a running computer
int PCis;

void setup() {
  
  Serial.begin(9600);

  // Initialize the rtc object
  rtc.begin();
  
  // That's where Módís' PC is connected to
  pinMode(A5, INPUT);
  
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

  //print all the static text and stuff
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.print("PC-Time:");  
  tft.drawFastHLine(0, tft.height()/2, tft.width(), YELLOW);  
  tft.setCursor(10, tft.height()/2+10);
  tft.print("Play outside:");
}

void loop() {

  PCis = 0;

  printTIME();
  
  //run this loop every second for one minute
  for (int i = 1; i<61; i++) {

    blinkDOTS(i);
    
    //check the pc ever 12 seconds
    if (i%12 == 0) {

      // PCis after 5 checks per minute:
      // positive if mostly ON
      // negative if mostly OFF
      if (checkPC() == ON) {
        PCis += 1;
        printINDICATOR(ON);
      }
      else {
        PCis -= 1;
        printINDICATOR(OFF);
      }
    }
    //wait a second
    delay(1000);
  }

  if (PCis < 0)
    timer += 2;
  else
    timer -= 1;
}

void blinkDOTS(int second) {

  tft.setTextSize(7);
    
  // second is even, show yellow dots
  if (second%2 == 0) {
    tft.setTextColor(YELLOW);

    // Only blink the dotson the active "side"
    if (timer == 0) {
      tft.setCursor(132, 50);
      tft.print(":");
      tft.setCursor(132, tft.height()/2+50);
      tft.print(":");
    }
    else if (timer > 0) {
      tft.setCursor(132, 50);
      tft.print(":");
    }
    else {
      tft.setCursor(132, tft.height()/2+50);
      tft.print(":");
    }
  }
  // second is odd, "show" black dots
  else {
    tft.setTextColor(BLACK);
    tft.setCursor(132, 50);
    tft.print(":");
    tft.setCursor(132, tft.height()/2+50);
    tft.print(":");
  }
}

//print the time
void printTIME() {
  
  //overdraw the upper hours
  tft.fillRect(50, 50, 78, 49, BLACK);
  //overdraw the upper minutes
  tft.fillRect(170, 50, 78, 49, BLACK);
  
  //overdraw the lower hours
  tft.fillRect(50, tft.height()/2+50, 78, 49, BLACK);
  //overdraw the lower minutes
  tft.fillRect(170, tft.height()/2+50, 78, 49, BLACK);

  int hours = abs(timer/60);
  int minutes = abs(timer%60);
    
  tft.setTextSize(7);
  tft.setTextColor(YELLOW);

  // Time is even
  if (timer == 0) {
    
    
    tft.setCursor(93, 50);
    tft.print("0");
    tft.setCursor(170, 50);
    tft.print("00");
  
    tft.setCursor(93, tft.height()/2+50);
    tft.print("0");
    tft.setCursor(170, tft.height()/2+50);
    tft.print("00");
  }
  // Time left to play PC
  else if (timer >= 0) {
      
    if (hours <= 9)
      tft.setCursor(93, 50);
    else
      tft.setCursor(50, 50);
      
    tft.print(hours);
    
    if (minutes <= 9) {
      tft.setCursor(170, 50);
      tft.print("0");
      tft.setCursor(213, 50);
    }
    else
      tft.setCursor(170, 50);
      
    tft.print(minutes);
  }
  // Time needed to spend "outside"
  else {

    hours = hours/2;
    minutes = minutes/2;

    if (hours <= 9)
      tft.setCursor(93, tft.height()/2+50);
    else
      tft.setCursor(50, tft.height()/2+50);
      
    tft.print(hours);
    
    if (minutes <= 9) {
      tft.setCursor(170, tft.height()/2+50);
      tft.print("0");
      tft.setCursor(213, tft.height()/2+50);
    }
    else 
      tft.setCursor(170, tft.height()/2+50);
      
    tft.print(minutes);
    }
}

PCmode checkPC() {
    
//if PC is on...A5 is "connected", only highs happen
//if PC is off..A5 is "not connected", only lows happen
  if (digitalRead (A5) == HIGH) {
    return ON;
  }
  else {
    return OFF;
  }
}

void printINDICATOR(PCmode PC) {

  // Overdraw the last PC status
  tft.fillRect(tft.width()-100, 10, 85, 21, BLACK);

  // Draw the new status
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(tft.width()-100, 10);

  if (PC == ON)
    tft.print("(ON)");
  else
    tft.print("(OFF)");
}
