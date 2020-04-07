/* (C)2020 Benji Schmid

    Klodis is a clock to manage Modis gaming habits.
    Balances between PC time and "outside" time.
    2x outside = pc

    Parts:
      Arduino Uno
      2.8" TFT Touchscreen
*/

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

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

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

//Stores the minutes that need to be balanced
int timer = 0;
int hours = 0;
int minutes = 0;

//to see in what mode we are in
typedef enum {pc, play} mode;
mode cur_mode = play;

int highs = 0;
int lows = 0;

void setup() {
  Serial.begin(9600);

  pinMode(A5, INPUT);
  
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);

  tft.fillScreen(BLACK);
  
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);

  tft.setCursor(10, 10);
  tft.print("PC-Time:");
  
  tft.drawFastHLine(0, tft.height()/2, tft.width(), YELLOW);
  
  tft.setCursor(10, tft.height()/2+10);
  tft.print("Play outside:");
  
  tft.setTextSize(7);
  
  tft.setCursor(132, 50);
  tft.print(":");
  tft.setCursor(132, tft.height()/2+50);
  tft.print(":");
}

void loop() {
  
//  check_pcstatus();
/*
//later: if pc is on
  if (timer > 200)
    cur_mode = pc;
//later: if pc is off  
  if (timer < -200)
    cur_mode = play;
*/
  count_time();

  overdraw();
  
  print_time(cur_mode, abs(timer/60), abs(timer%60));

}

void overdraw() {
  
  //overdraw the upper hours
  tft.fillRect(50, 50, 78, 49, BLACK);
  //overdraw the upper minutes
  tft.fillRect(170, 50, 78, 49, BLACK);
  
  //overdraw the lower hours
  tft.fillRect(50, tft.height()/2+50, 78, 49, BLACK);
  //overdraw the lower minutes
  tft.fillRect(170, tft.height()/2+50, 78, 49, BLACK);
}

void print_time(mode cur_mode, int hours, int minutes) {
    
  if (timer  >= 0) {

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
  else {
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

void count_time() {
  
  highs = 0;
  lows = 0;
  
  for (int i = 0; i<6000; i++) {
    delay(10);
    check_pcstatus();
  }
  
//  Serial.print("HIGHS: ");
//  Serial.println(highs);
//  Serial.print("LOWS: ");
//  Serial.println(lows);

  if (lows > 0) {
//    Serial.println("lows happened -> off");
    cur_mode = play;
    timer += 2;
  }
  else if (highs > lows) {
//  Serial.println("more highs -> on");
    cur_mode = pc;
    timer -= 1;
  }
}

void check_pcstatus() {

//if PC is on...A5 is "connected", only highs happen
//if PC is off..A5 is "not connected, lows happen
  if (digitalRead (A5) == HIGH) {
    highs += 1;
  }
  else {
    lows += 1;
  }
}
