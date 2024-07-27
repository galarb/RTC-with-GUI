/*

                    CLI Software for slave GUI

               utilizing the familiar clicli formatting tool
               to parse UART messages.
               for use with the 3.5 uno OLED shield

  Example functions:              
              Feature                |  CLI Usage
___________________________________________________
 get time                            |  a ()
 
               
      by Gal Arbel
      July 2024
*/


#include "uartclicli.h"
#include "Arduino.h"
#include "HardwareSerial.h"
#include <SoftwareSerial.h>
#include "RTCDateTime.h"

const unsigned int MAX_MESSAGE_LENGTH = 128;

//SoftwareSerial mySerial(0, 1); // RX, TX
RTCDateTime globalDateTime;


//clicli::clicli(hvacontrol &hvacontrol):myhvacontrol(hvacontrol), number(7) {

//}

void uartclicli::begin(int bdrate) {
   Serial.begin(bdrate);
  //
}

void uartclicli::run() {
  static char message[MAX_MESSAGE_LENGTH];
  static unsigned int message_pos = 0;

  while (Serial.available() > 0) {
    char inByte = Serial.read();   // Read the next available byte in the serial receive buffer

    // Check for end of message character (newline)
    if (inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1)) {
      message[message_pos] = inByte;  // Add the incoming byte to our message
      message_pos++;
    } else {
      // End of message received
      message[message_pos] = '\0';  // Add null character to string

      // Debug print the received message
     // Serial.print("Received message: ");
     // Serial.println(message);
      
      // Parse message
      int command[7] = {0};
      int argindex = 0;
      char cmd = '\0';
      char delim[] = " ";
      char tmpmsg[MAX_MESSAGE_LENGTH];
      strcpy(tmpmsg, message);  // Copy message to temporary buffer for strtok

      char *ptr = strtok(tmpmsg, delim);
      while (ptr != NULL) {
        if (argindex == 0) {
          cmd = ptr[0];
          // Debug print the command character
          //Serial.print("Command: ");
          //Serial.println(cmd);
        } else if (argindex <= 7) {
          command[argindex - 1] = atoi(ptr);
          // Debug print each argument
          //Serial.print("Argument ");
          //Serial.print(argindex);
          //Serial.print(": ");
          //Serial.println(command[argindex - 1]);
        }
        argindex++;
        ptr = strtok(NULL, delim);
      }

      // Handle command
      switch (cmd) {
        case 'a': // handle time message
        //set the global variables
          globalDateTime.hour = (int)command[0];
          globalDateTime.minute = (int)command[1];
          globalDateTime.second = (int)command[2];
          globalDateTime.day = (int)command[3];
          globalDateTime.month = (int)command[4];
          globalDateTime.year = (int)command[5];

          // Print the DateTime structure
         /* Serial.print("Date and Time: ");
          Serial.print(globalDateTime.day);
          Serial.print("/");
          Serial.print(globalDateTime.month);
          Serial.print("/");
          Serial.print(globalDateTime.year);
          Serial.print(" ");
          Serial.print(globalDateTime.hour);
          Serial.print(":");
          Serial.print(globalDateTime.minute);
          Serial.print(":");
          Serial.println(globalDateTime.second);*/
          break;

        // Add more cases as needed

        default:
          Serial.println("Unknown command");
          break;
      }

      // Reset for the next message
      message_pos = 0;
    }
  }
  delay(1000);
}
