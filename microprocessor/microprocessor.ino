#include<Wire.h>
#include <EEPROM.h>
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

float x_prev, y_prev, z_prev; // previous acceleration values
float x, y, z;                // current acceleration values
float x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int threshold = 20;           // threshold for difference in acceleration

int16_t address = 0;          // address of next available space for fact storage
const int maxFactSize = 140;  // maximum size of fact
int factIndex = 2;            // address of fact to play

void clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

// breaks up 16 bit address to store into EEPROM
void writeAddress() {
  int lower_8bits = address & 0xff;
  int upper_8bits = (address >> 8) & 0xff;
  EEPROM.write(0,lower_8bits);
  EEPROM.write(1,upper_8bits);

  printDebugging(2);
}

// reads 16 bit address from EEPROM
void readAddress() {
  int lower_8bits = EEPROM.read(0);
  int upper_8bits = EEPROM.read(1);
  address = (upper_8bits << 8) | lower_8bits;

  printDebugging(3);
}

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  //clearEEPROM();

  // read last address index 
  readAddress();
}

void cacheFactLocally(char* fact, int factLength) {
  if(address == 0) address = 1; // fact storage address starts at 2
  
  if( (address + factLength) > EEPROM.length()) { // EEPROM memory does not have enough room to store fact
    Serial.println("Memory full. Can't cache fact");
  } else{
    if(strlen(fact) < maxFactSize) { // fact does not exceed max fact size
      // store fact byte by byte in EEPROM
      for(int i = 0; i <= factLength; i++) {
        address++;
        EEPROM.write(address, fact[i]);
        Serial.print((char)EEPROM.read(address));
        if(fact[i] == '\0') break;
      }
      Serial.println();
      writeAddress(); // update address for next stored fact
    } else {
      Serial.println("Fact exceeds maximum string size");
    }
  }
}

void playFact(){
  // reset factIndex if all facts have been played
  if(factIndex >= address) factIndex = 2;
  
  String fact;
  printDebugging(4);
  // read fact from EEPROM
  while (EEPROM.read(factIndex) != '\0') {
    fact += (char)EEPROM.read(factIndex);
    factIndex++;
  }

  // update fact address for next fact to be played
  factIndex++;
  Serial.println(fact);
}

void checkAcceleration() {
  //accel.getXYZ(x, y, z);
  x = AcX;
  y = AcY;
  z = AcZ;
  x_diff = abs(x - x_prev);
  y_diff = abs(y - y_prev);
  z_diff = abs(z - z_prev);

  printDebugging(1);

  if ( x_diff > threshold || y_diff > threshold || z_diff > threshold ) {
    Serial.println("Throw or shake detected");
    Serial.println("Stating fact...");
    // Read fact from EEPROM
    playFact();
    delay(5000); // set delay long enough to  for fact to be played
  } else {
    x_prev = x;
    y_prev = y;
    z_prev = z;
    delay(200); // delay in acceleration sampling rate
  }
}

void getAcceleration() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  /*Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  delay(333);*/

  printDebugging(0);
}

char factArray[][maxFactSize] = {
  "The young kangaroo, or joey, is born alive at a very immature stage when it is only about 2 cm long and weighs less than a gram.",
  "Pteronophobia is the fear of being tickled by feathers!",
  "A flock of crows is known as a murder.",
  "Cherophobia is the fear of fun.",
  "Human saliva has a boiling point three times that of regular water.",
  "You cannot snore and dream at the same time.",
  "Recycling one glass jar saves enough energy to watch TV for 3 hours.",
  "The Titanic was the first ship to use the SOS signal.",
  /*"J.K. Rowling chose the unusual name ‘Hermione’ so young girls wouldn’t be teased for being nerdy!",
  "In the Caribbean there are oysters that can climb trees.",
  "A duel between three people is actually called a truel.",
  "The television was invented only two years after the invention of sliced bread.",
  "Alligators will give manatees the right of way if they are swimming near each other.",
  "Sunsets on Mars are blue."*/
};

void loop(){
  //getAcceleration();
  //checkAcceleration();
  /*for( int i = 0; i < 8; i++) {
    cacheFactLocally(factArray[i], strlen(factArray[i]));
    playFact();
    delay(5000);
  }*/
}

void printDebugging(int function) {
  switch(function) {
    case 0: // Print acceleration
      Serial.print("AcX = "); 
      Serial.println(AcX);
      Serial.print("AcY = "); 
      Serial.println(AcY);
      Serial.print("AcZ = "); 
      Serial.println(AcZ);
      Serial.print("GyX = "); 
      Serial.println(GyX);
      Serial.print("GyY = "); 
      Serial.println(GyY);
      Serial.print("GyZ = "); 
      Serial.println(GyZ);
      break;
    case 1: // Print acceleration differences for testing purposes
      Serial.print("x diff: ");
      Serial.println(x_diff);
      Serial.print("y diff: ");
      Serial.println(y_diff);
      Serial.print("z diff: ");
      Serial.println(z_diff);
      break;
    case 2: // print writing 16 bit address to EEPROM
      readAddress();
      break;
    case 3: // print reading 16 bit address from EEPROM
      Serial.print("Last saved address: ");
      Serial.println(address);
      break;
    case 4: // print address of fact playing
      Serial.print("Fact at address ");
      Serial.print(factIndex);
      Serial.print(": ");
      break;
  }
  Serial.println();
}
