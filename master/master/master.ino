#include "clicli.h"
#include <SoftwareSerial.h>
#include <RTClib.h>

clicli mycli;
SoftwareSerial mySerial(10, 11); // RX, TX
RTC_DS3231 rtc;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println("master here");
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello master here");
}

void loop() { // run over and over
  //mycli.run();
  DateTime now = rtc.now(); //a structure to hold the time
  //Serial.println(rtc.isrunning());
  // Send the `a` command followed by 6 float arguments
  //now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year())
  int arg1 = now.hour();
  int arg2 = now.minute();
  int arg3 = now.second();
  int arg4 = now.day();
  int arg5 = now.month();
  int arg6 = now.year() % 100; // Get the last two digits of the year
  
  // Create a message string
  String message = "a ";
  message += String(arg1) + " ";
  message += String(arg2) + " ";
  message += String(arg3) + " ";
  message += String(arg4) + " ";
  message += String(arg5) + " ";
  message += String(arg6) + "\n";

  // Send the message
  mySerial.print(message);
  Serial.println(message);
  delay(1000);
}
