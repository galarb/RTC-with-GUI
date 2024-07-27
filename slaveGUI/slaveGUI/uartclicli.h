#ifndef UARTCLICLI_H
#define UARTCLICLI_H
//#include "hvacontrol.h"

 class uartclicli {
  private:
  //hvacontrol &myhvacontrol;
  int number;
  
  public:
   //clicli(hvacontrol &myhvacontrol);
   uartclicli;
   void begin(int bdrate);   //must be called from  void setup()
   void run();   //must be called from  void loop()

 };
#endif 