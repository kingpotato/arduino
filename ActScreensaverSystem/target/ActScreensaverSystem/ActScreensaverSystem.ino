/*

  Activate/deactivate screen saver on Ubuntu
  Language: Wiring/Arduino

  This program sends a digital detection message to host PC in the serial port,
  when detected something by IR sensor.

  Thanks to Minsu Kim for the reference source code and HW materials

  Created 4 Feb. 2013
  by Steven Kim

*/

#include <IRremote.h>

#define PIN_IR 3
#define PIN_DETECT 11

IRsend irsend;

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_IR, OUTPUT);  
  pinMode(PIN_DETECT, INPUT);
  irsend.enableIROut(38);
  irsend.mark(0);
  int status = 0;
}


void loop() {
    int pr = digitalRead(PIN_DETECT);

    if(pr)
        Serial.println(1);
    else
        Serial.println(0);
  
    delay(2000);
}
