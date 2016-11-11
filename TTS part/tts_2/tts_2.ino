#include <SoftwareSerial.h>
#include "EMIC2.h"
//need to add the emic 2 library

#define rxpin 2
#define txpin 3

EMIC2 emic;

void setup() {
  // put your setup code here, to run once:
  emic.begin(rxpin, txpin);
  emic.setVoice(8); //sets the voice, there are 9 types, 0 - 8
  emic.setVolume(10); //sets the vloume, 10 is max 
}
int num = 1;
String say = "Hello. How are you?";
void loop() {
  // put your main code here, to run repeatedly:
  if(num == 1) { //put accelerometer condition here
    emic.speak(say);
    delay(1000); //adds a pause after 1 second
    ~emic;
    delay(1000); //unpauses after 1 second
    ~emic;
  }
}
