/* To set the DS3231 to a specific time and date
 *  (C) 2020 Benji
 *  Compile and upload takes exactly 19 seconds
*/

#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

Time act_time;
int last_second;

void setup(void) {

  Serial.begin(9600);
  Serial.println("serial initialized");

  // Initialize the rtc object
  rtc.begin();
  Serial.println("rtc initialized");

  //Zeit und Datum, die beim Anschalten gesetzt werden
  rtc.setDOW(SATURDAY);
  rtc.setTime(12, 00, 19);
  rtc.setDate(4, 11, 2020);
  //Debugausgabe
  Serial.print("Clock set: ");
  Serial.print(rtc.getDOWStr());
  Serial.print(" - ");
  Serial.print(rtc.getDateStr());
  Serial.print(" - ");
  Serial.println(rtc.getTimeStr());
  Serial.println();

}


void loop(void) {

  act_time = rtc.getTime();

  if (act_time.sec != last_second) {
    last_second = act_time.sec;
    //Debugausgabe
    Serial.print("Time is: ");
    Serial.print(rtc.getDOWStr());
    Serial.print(" - ");
    Serial.print(rtc.getDateStr());
    Serial.print(" - ");
    Serial.println(rtc.getTimeStr());
  }
}
