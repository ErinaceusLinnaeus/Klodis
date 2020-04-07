/* (C)2020 Benji Schmid

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

//integers that fill, when it checks for a running computer
int highs = 0;
int lows = 0;

void setup() {
  Serial.begin(9600);

//that's where Modis PC is connected
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
  tft.setTextSize(7);  
  tft.setCursor(132, 50);
  tft.print(":");
  tft.setCursor(132, tft.height()/2+50);
  tft.print(":");
}

void loop() {
    
  print_time(cur_mode, abs(timer/60), abs(timer%60));
  
  count_time();

  overdraw();

}

//overdraw the times with black rectangles
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

//print the time
void print_time(mode cur_mode, int hours, int minutes) {
  
  tft.setTextSize(7);
  
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
    if (i%100 == 0) {
      print_indicator();
    }
    delay(10);
    check_pcstatus();
  }

  Serial.print("HIGHS: ");
  Serial.println(highs);
  Serial.print("LOWS: ");
  Serial.println(lows);

  if (lows > 3000) {
    Serial.println("more lows than highs -> off");
    cur_mode = play;
    timer += 2;
  }
  else if (highs > 3000) {
  Serial.println("more highs than lows -> on");
    cur_mode = pc;
    timer -= 1;
  }
}

int check_pcstatus() {

    
//if PC is on...A5 is "connected", only highs happen
//if PC is off..A5 is "not connected", only lows happen
  if (digitalRead (A5) == HIGH) {
    highs += 1;
    return 1;  
  }
  else {
    lows += 1;
    return 0;
  }
}

void print_indicator() {
  
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  tft.setCursor(tft.width()-100, 10);
  tft.print("(ON)");
  tft.setCursor(tft.width()-100, 10);
  tft.print("(OFF)");
  
  if (check_pcstatus() == 1) {
    tft.setTextColor(YELLOW);
    tft.setCursor(tft.width()-100, 10);
    tft.print("(ON)");
  }
  else{
    tft.setTextColor(YELLOW);
    tft.setCursor(tft.width()-100, 10);
    tft.print("(OFF)");
  }
}
