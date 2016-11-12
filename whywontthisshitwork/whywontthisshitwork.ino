#include <SoftwareSerial.h>
#include <EMIC2.h>
#define rxpin 2
#define txpin 3

EMIC2 emic;

void setup() {
  // initialize emic devices
  emic.begin(rxpin, txpin);
  //Serial.println("hello1");
  emic.setVoice(8); //sets the voice, there are 9 types, 0 - 8
  emic.setVolume(5); //sets the vloume, 10 is max 
  Serial.println("test"); // this is not getting printed

}


// plays fact on TTS module
void playFact(String fact) {
  Serial.println(fact);
  emic.speak(fact);
    delay(1000); //adds a pause after 1 second
    ~emic;
    delay(1000); //unpauses after 1 second
    ~emic;
}

void loop() {
  // put your main code here, to run repeatedly:
  //playFact("Hello World");
}
