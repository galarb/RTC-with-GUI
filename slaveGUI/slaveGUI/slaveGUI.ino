#include "uartclicli.h"
#include "RTCDateTime.h"
#include <Wire.h>
#include <ButtonIRQ.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMono24pt7b.h>
#include <SD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <RTClib.h>

#define BLACK   0xFFFF
#define BLUE    0xFFE0
#define RED     0x07FF
#define GREEN   0xF81F
#define CYAN    0xF22F
#define MAGENTA 0x0F2F
#define YELLOW  0x001F
#define WHITE   0x0000
#define CREAM   0x000F
#define LIGHTPURPLE 0x03E0
#define PINKISH 0x03E0
#define LIGHTNAVY 0x780F
#define SPURPLE 0x7BE0
#define GRAY 0x7BE0
#define GRAYISH 0x7BEF
#define PINK 0x07E0
#define REDISH 0x07FF
#define LIGHBLUE 0xF800
#define LIGHTGREEN 0xF81F
#define BLUISH 0xFFE0
#define BLUISH2 0xFDA0
#define PURPLE 0xB7E0
#define DARKGREEN 0xFC9F

#define MAX_LINES 8
#define LINE_HEIGHT 40 // Adjust this according to your font size
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320

uartclicli mycli;
const int XP = 8, XM = A2, YP = A3, YM = 9; //320x480. ID=0x9090

MCUFRIEND_kbv tft(A3, A2, A1, A0, A4);
TSPoint tp; // This object gets the touch xyz
TouchScreen ts(XP, YP, XM, YM, 300); // Re-initialized after diagnose

int x, y;
bool togshow = false; // Time/date toggle 
bool togg = false;
char t[32];
int mode;
int lasthour, lastmin, lastsec, lastday, lastmonth, lastyear;
int currentLine = 0; // For Serial printing

void setup() {
  Serial.begin(4800);
  Serial.println("Slave here!");
  tft.begin(0x9090); // That's my shield's driver
  tft.setRotation(3);
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("SD Card initialization failed.");
    return;
  } else {
    Serial.println("SD Card initialized.");
  }
  mycli.begin(4800);
  drawButtons();
  while (!arrow()) {
    checkTouch();
  }
  // Initialize the last known values
  lastday = globalDateTime.day;
  lastmonth = globalDateTime.month;
  lastyear = globalDateTime.year;
  lasthour = globalDateTime.hour;
  lastmin = globalDateTime.minute;
  lastsec = globalDateTime.second;
}

void loop() { // Run over and over
  switch (mode) {
    case 0:
      Serial.println("Clock program running");
      while (true) {
        mycli.run(); // Sets the global time variables
        if (!togg) {
          drawclock();
          delay(100);
        } else {
          drawdate();
          delay(100);
        }
      }
      break;

    case 1:
      Serial.println("Maintenance program running");
      restoreLCDPins();
      tft.fillScreen(BLACK);
      tft.setTextColor(CREAM);
      tft.setFont();
      tft.setTextSize(3);
      while (true) {
        printSerialData();
        delay(100); // Add a delay to avoid excessive CPU usage
      }
      break;
  }
}

void drawButtons() {
  tft.fillScreen(WHITE);
  tft.setFont(&FreeMono24pt7b);
  tft.setCursor(60, 50);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.print("Choose Mode");
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.fillRect(30, 100, 200, 100, CYAN); // x,y,w,h,col
  tft.fillRect(240, 100, 200, 100, MAGENTA); // x,y,w,h,col

  tft.setCursor(40, 150);
  tft.print("CLOCK");
  tft.setCursor(260, 150);
  tft.print("maint");

  // Draw proceed button
  tft.fillRoundRect(125, 220, 240, 80, 10, BLUE);
  tft.setCursor(145, 270);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.print("proceed");
}

bool arrow() {
  tp = ts.getPoint(); // tp.y is always the long dimension. tp.z is the pressure

  if (tp.z > 20) {
    x = map(tp.y, 90, 935, 0, 480); // The long dimension
    y = map(tp.x, 144, 900, 0, 320);
    Serial.print("x = "); 
    Serial.println(x);
    Serial.print("y = ");
    Serial.println(y);
    if (x > 125 && x < 365) {
      if (y > 220 && y < 300) {
        tft.fillRoundRect(125, 220, 240, 100, 2, BLUE);
        return true;
      }
    } else {
      return false;
    }
  }
}

void checkTouch() {
  // Because we are sharing pins with the TFT (A2 and A3), we have to change them back
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  tp = ts.getPoint(); // tp.y is always the long dimension. tp.z is the pressure

  if (tp.z > 20) {
    x = map(tp.y, 90, 935, 0, 480); // The long dimension
    y = map(tp.x, 144, 900, 0, 320);
    Serial.print("x = "); 
    Serial.println(x);
    Serial.print("y = ");
    Serial.println(y);    
    tft.fillRect(30, 100, 200, 100, CYAN); // x,y,w,h,col
    tft.fillRect(240, 100, 200, 100, MAGENTA); // x,y,w,h,col
    if (y > 100 && y < 190) {
      if (x > 30 && x < 230) { // Mode 0 selected
        Serial.println("Clock mode Selected");
        mode = 0;
      } else if (x > 240 && x < 440) { // Mode 1 selected
        Serial.println("Maintenance mode Selected");
        mode = 1;
      } 
    }          
  } else {
    delay(50);
  }
  restoreLCDPins();
}

void drawclock() {
  restoreLCDPins();
  if (!togshow) {
    tft.fillScreen(LIGHTNAVY);
    tft.setFont(&FreeMono24pt7b);
    tft.setTextSize(1);
    tft.fillCircle(240, 240, 80, BLUISH2); // Toggle button
    tft.setCursor(180, 280);
    tft.setTextColor(CREAM);
    tft.print("DATE");
    tft.fillCircle(110, 130, 115, PINKISH); // x0, y0, r, color
    tft.fillCircle(245, 130, 115, PINKISH); // x0, y0, r, color
    tft.fillCircle(370, 130, 115, PINKISH); // x0, y0, r, color
    tft.setTextColor(RED);
    tft.setTextSize(2);

  }  
  if (lasthour != globalDateTime.hour) {
    tft.fillCircle(88, 130, 63, PINKISH); // "Delete" the value from screen
  }
  tft.setCursor(20, 150);
  tft.print(globalDateTime.hour); tft.print(":");
  if (lastmin != globalDateTime.minute) {
    tft.fillCircle(230, 130, 63, PINKISH); // "Delete" the value from screen
  }
  tft.setCursor(170, 150);
  tft.print(globalDateTime.minute); tft.print(":");
  if (lastsec != globalDateTime.second) {
    tft.fillCircle(410, 130, 63, PINKISH); // "Delete" the value from screen
  }
  tft.setCursor(350, 150);
  tft.print(globalDateTime.second);
  
  lasthour = globalDateTime.hour;
  lastmin = globalDateTime.minute;
  lastsec = globalDateTime.second;
  lastday = globalDateTime.day;
  lastmonth = globalDateTime.month;
  lastyear = globalDateTime.year;
  togshow = true;
  checktoggle();

}

void drawdate() {
  restoreLCDPins();
  if (!togshow) {
    tft.fillScreen(LIGHTNAVY);
    tft.setFont(&FreeMono24pt7b);
    tft.setTextSize(1);
    tft.fillCircle(240, 240, 80, GREEN); // Toggle button
    tft.setCursor(170, 280);
    tft.setTextColor(REDISH);
    tft.print("CLOCK");
    tft.fillCircle(110, 130, 115, CREAM); // x0, y0, r, color
    tft.fillCircle(245, 130, 115, CREAM); // x0, y0, r, color
    tft.fillCircle(370, 130, 115, CREAM); // x0, y0, r, color
    tft.setTextColor(PURPLE);    
    tft.setTextSize(2);

  }  
  if (lastday != globalDateTime.day) {
    tft.fillCircle(70, 125, 60, CREAM); // "Delete" the value from screen
  }
  tft.setCursor(15, 150);
  tft.print(globalDateTime.day); tft.print("/");
  if (lastmonth != globalDateTime.month) {
    tft.fillCircle(220, 125, 60, CREAM); // "Delete" the value from screen
  }
  tft.setCursor(180, 150);
  tft.print(globalDateTime.month); tft.print("/");
  if (lastyear != globalDateTime.year) {
    tft.fillCircle(350, 125, 60, CREAM); // "Delete" the value from screen
  }
  tft.setCursor(290, 150);
  tft.print(globalDateTime.year);

  lasthour = globalDateTime.hour;
  lastmin = globalDateTime.minute;
  lastsec = globalDateTime.second;
  lastday = globalDateTime.day;
  lastmonth = globalDateTime.month;
  lastyear = globalDateTime.year;
  togshow = true;
  checktoggle();
}

void checktoggle() {
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  tp = ts.getPoint(); // tp.y is always the long dimension. tp.z is the pressure

  if (tp.z > 20) { // Applied pressure
    x = map(tp.y, 90, 935, 0, 480); // The long dimension
    y = map(tp.x, 144, 900, 0, 320);
    Serial.print("x = "); 
    Serial.println(x);
    Serial.print("y = ");
    Serial.println(y); 
       
    if (y > 160 && y < 320) {
      if (x > 160 && x < 320) { // Toggle pressed
        togg = !togg;
        togshow = false;
      } 
    }          
  } else {
    delay(10);
  }
}

void restoreLCDPins() {
    // Restore the LCD control pins to their default states
    pinMode(A3, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A0, OUTPUT);
    pinMode(A4, OUTPUT);
}

void printSerialData() {
  while (Serial.available() > 0) {
    char incomingChar = (char)Serial.read();
    
    if (incomingChar == '\n') {
      // Move to the next line
      currentLine++;
      if (currentLine >= MAX_LINES) {
        // Clear screen and reset line count
        tft.fillScreen(BLACK);
        currentLine = 0;
      }
      tft.setCursor(0, currentLine * LINE_HEIGHT);
    } else {
      // Print the character
      tft.print(incomingChar);
    }
  }
}
