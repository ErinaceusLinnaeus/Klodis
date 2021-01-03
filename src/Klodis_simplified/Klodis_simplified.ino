/* (C)2020 Benji
 *  Version 1.1

    Simplified Edition:
        1.5 hrs a day !

    Klodis is a clock to manage Modis gaming habits.
    Balances between PC time and "outside" time.
    2x outside = pc
    
    Parts:
      Arduino Mega
      2.8" TFT Touchscreen
      DS3231 clock
*/

//#define DEBUG
//#define TIMEDEBUG
//#define SERIALINFORMATION

#include <Elegoo_GFX.h>     // Core graphics library
#include <Elegoo_TFTLCD.h>  // Hardware-specific library
#include <DS3231.h>           // clock library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_RESET  A4 // Can alternately just connect to Arduino's reset pin (PC6)
#define LCD_CS A3     // Chip Select goes to Analog 3
#define LCD_CD A2     // Command/Data goes to Analog 2 //sometimes: LCD_RS
#define LCD_WR A1     // LCD Write goes to Analog 1
#define LCD_RD A0     // LCD Read goes to Analog 0

#define MODIS A11     // Where Módís PC is connected to

// The times Módís comes home from shool / wakes up
#define WEEKAWAKE 13
#define WEEKENDAWAKE 9

// The times Módís goes to sleep
#define TIMETOSLEEP 23

//How long she's allowed to play [min]
#define PLAYTIME 90

// Init the display
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// To store the actual time
Time now;
// To store the actual UNIXtime
long unixTime;

// Boolean to switch the device off when it's sleeping time
bool sleepyTime = true;

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
int timer = PLAYTIME;

// Enum to see if the PC is turned on
typedef enum {OFF, ON} PCmode;

// Integer to check for a running computer
int PCis;

// To "suspend" operation while everybody's asleep anyways
long suspend = 60000;

void wakeUP() {

#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("function: wakeUP()");
#endif
  
  //print all the static text and stuff
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.print("You can play for:");
  tft.drawFastHLine(0, tft.height()/2, tft.width(), YELLOW);
}

void setup() {
  
  Serial.begin(9600);
  Serial.println("Serial initialized");

  // Initialize the rtc object
  rtc.begin();
  Serial.println("rtc initialized");

  // Get the time
  now = rtc.getTime();
  unixTime = rtc.getUnixTime(now);
  Serial.println("Time taken");
  Serial.print("UNIX: ");
  Serial.println(rtc.getUnixTime(rtc.getTime()));
  Serial.print("time: ");
  Serial.println(rtc.getTimeStr());
  
  // That's where Módís' PC is connected to
  pinMode(MODIS, INPUT);
  
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(3);
  Serial.println("tft initialized");

  Serial.println();
  Serial.println("SETUP finished");
#ifdef DEBUG
  Serial.println("DEBUG activated");
#else
  Serial.println("DEBUG deactivated");
#endif
#ifdef TIMEDEBUG
  Serial.println("TIMEDEBUG activated");
#else
  Serial.println("TIMEDEBUG deactivated");
#endif
#ifdef SERIALINFORMATION
  Serial.println("SERIALINFORMATION activated");
#else
  Serial.println("SERIALINFORMATION deactivated");
#endif
  Serial.println();
  
}

void loop() {

  // Get the time
  now = rtc.getTime();
  
  // Usually don't suspend. suspend = 0
  // Suspend for 1 minute while everybody's asleep
  delay(suspend);

  // Check, if it's time to go to sleep
  if (!sleepyTime && checkTIME2SLEEP()) {
    
#ifdef SERIALINFORMATION
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("System is asleep.");
#endif
    
    sleepyTime = true;
    tft.fillScreen(BLACK);
    // Suspend for 1 minutes while everybody's asleep
    suspend = 60000;
  }

  // Check if it's time to wake up
  else if (sleepyTime && checkTIME2WAKE()) {

#ifdef SERIALINFORMATION
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("System is waking up.");
#endif
    
    sleepyTime = false;
    timer = PLAYTIME;
    suspend = 0;
    wakeUP();
    
  }

  // Do all of this only when everybody's awake
  if (!sleepyTime) {
    
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
          PCis -= 1;
          printINDICATOR(ON);
        }
        else {
          PCis += 1;
          printINDICATOR(OFF);
        }
        
#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.print("PCis : ");
  Serial.println(PCis);
#endif
      }
      
    // Wait a second
    // Almost a second... With delay(1000); we get 1 second too much every 3 minutes
    delay(994);
    
    // Get the times
    Time now = rtc.getTime();
    unixTime = rtc.getUnixTime(now);
      
    }

#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.print("final PCis : ");
  Serial.println(PCis);
#endif

    // If PCis is positive or negative determines if the PC
    // was mostly switched on or off in the last minute
    //negative means PC is on and 1 minute will be substracted from the remaining time
    if (PCis < 0 && timer > 0)
      timer -= 1;
  }
}

int convertDAW2INT() {

#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("function: convertDAW2INT()");
#endif

#ifdef TIMEDEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - It's ");
#endif

  if (rtc.getDOWStr(FORMAT_SHORT) == "Mon") {
#ifdef TIMEDEBUG
  Serial.println("Monday.");
#endif
    return 1;
  }
  if (rtc.getDOWStr(FORMAT_SHORT) == "Tue") {
#ifdef TIMEDEBUG
  Serial.println("Tuesday.");
#endif
    return 2;
  }
  if (rtc.getDOWStr(FORMAT_SHORT) == "Wed") {
#ifdef TIMEDEBUG
  Serial.println("Wednesday.");
#endif
    return 3;
  }
  if (rtc.getDOWStr(FORMAT_SHORT) == "Thu") {
#ifdef TIMEDEBUG
  Serial.println("Thursday.");
#endif
    return 4;
  }
  if (rtc.getDOWStr(FORMAT_SHORT) == "Fri") {
#ifdef TIMEDEBUG
  Serial.println("Friday.");
#endif
    return 5;
  }
  if (rtc.getDOWStr(FORMAT_SHORT) == "Sat") {
#ifdef TIMEDEBUG
  Serial.println("Saturday.");
#endif
    return 6;
  }
  if (rtc.getDOWStr(FORMAT_SHORT) == "Sun") {
#ifdef TIMEDEBUG
  Serial.println("Sunday.");
#endif
    return 7;
  }
}

bool checkTIME2WAKE() {

#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("function: checkTIME2WAKE()");
#endif

  int intDay = convertDAW2INT();

  switch (intDay) {
    case 1:
      if (now.hour >= WEEKAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    case 2:
      if (now.hour >= WEEKAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    case 3:
      if (now.hour >= WEEKAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    case 4:
      if (now.hour >= WEEKAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    case 5:
      if (now.hour >= WEEKAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    case 6:
      if (now.hour >= WEEKENDAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    case 7:
      if (now.hour >= WEEKENDAWAKE && now.hour < TIMETOSLEEP)
        return true;
      else
        return false;
    default:
      return false;
  }
}

bool checkTIME2SLEEP() {
  
#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("function: checkTIME2SLEEP()");
#endif

  int intDay = convertDAW2INT();

  switch (intDay) {
    case 1:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKAWAKE)
        return true;
      else
        return false;
    case 2:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKAWAKE)
        return true;
      else
        return false;
    case 3:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKAWAKE)
        return true;
      else
        return false;
    case 4:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKAWAKE)
        return true;
      else
        return false;
    case 5:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKAWAKE)
        return true;
      else
        return false;
    case 6:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKENDAWAKE)
        return true;
      else
        return false;
    case 7:
      if (now.hour >= TIMETOSLEEP || now.hour < WEEKENDAWAKE)
        return true;
      else
        return false;
    default:
      return false;
  }
}

void blinkDOTS(int second) {

  tft.setTextSize(7);
    
  // RED dots if time is up
  if (timer == 0) {
    tft.setTextColor(RED);
    tft.setCursor(132, 50);
    tft.print(":");
    }
  else {
    // second is even, show yellow dots
    if (second%2 == 0) {
      tft.setTextColor(YELLOW);

      tft.setCursor(132, 50);
      tft.print(":");
    }
    // second is odd, "show" black dots
    else {
      tft.setTextColor(BLACK);
      tft.setCursor(132, 50);
      tft.print(":");
    }
  }
}

//print the time
void printTIME() {
  
#ifdef TIMEDEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.print("function: printTime()");
  Serial.print(" - timer : ");
  Serial.print(timer);
#endif

  //overdraw the upper hours
  tft.fillRect(50, 50, 78, 49, BLACK);
  //overdraw the upper minutes
  tft.fillRect(170, 50, 78, 49, BLACK);
  
    
  tft.setTextSize(7);
  tft.setTextColor(YELLOW);

  // Time is up
  if (timer == 0) {
    
#ifdef TIMEDEBUG
  Serial.println(" == 0");
#endif

#ifdef SERIALINFORMATION
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.println("Time is up!");
#endif
    
    tft.setTextColor(RED);
    
    tft.setCursor(93, 50);
    tft.print("0");
    tft.setCursor(170, 50);
    tft.print("00");
    
    tft.setTextSize(5);
    tft.setCursor(10, tft.height()/2+30);
    tft.print("Time is up");
  }
  // Time left to play PC
  else if (timer >= 0) {
    
#ifdef TIMEDEBUG
  Serial.println(" >= 0");
#endif
    
    int hours = abs(timer/60);
    int minutes = abs(timer%60);
    
#ifdef SERIALINFORMATION
  Serial.print("You can play for ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.println(".");
#endif
      
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
    
#ifndef DEBUG
  #ifdef SERIALINFORMATION
    Serial.print("PC is ");
    if (checkPC())
      Serial.println("on!");
    else
      Serial.println("off!");
  #endif
#endif
}

PCmode checkPC() {
  
#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.print("function: checkPC()");
#endif
  
//if PC is on...MODIS is "connected", only highs happen
//if PC is off..MODIS is "not connected", only lows happen
  if (digitalRead (MODIS) == HIGH) {
    
#ifdef DEBUG
  Serial.println(" - (ON)");
#endif
    return ON;
  }
  else {
    
#ifdef DEBUG
  Serial.println(" - (OFF)");
#endif
    return OFF;
  }
}

void printINDICATOR(PCmode PC) {

#ifdef DEBUG
  Serial.print(rtc.getTimeStr());
  Serial.print(" - ");
  Serial.print("function: printINDICATOR()");
#endif
  
  // Overdraw the last PC status
  tft.fillRect(20, tft.height()-20, tft.width()-20, tft.height()-20, BLACK);

  // Draw the new status
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(20, tft.height()-20);

  if (PC == ON) {
    tft.print("PC is ON ");
    
#ifdef DEBUG
  Serial.println(" - (ON)");
#endif
  }
  else {
    tft.print("PC is OFF");
    
#ifdef DEBUG
  Serial.println(" - (OFF)");
#endif
  }
}
