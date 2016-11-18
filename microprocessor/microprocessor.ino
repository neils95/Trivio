#include <SoftwareSerial.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "factgenerator.h"
#include <Wire.h>
#include <EEPROM.h>
#include "EMIC2.h"
//need to add the emic 2 library

#define rxpin 4
#define txpin 5
int16_t startingAddress = 4;

// uncomment if need to generate facts for testing purposes
//#define GENERATEFACTS

EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration

int16_t address = 0;              // address of next available space for fact storage
const int maxFactSize = 140;      // maximum size of fact
int factIndex = startingAddress;  // address of fact to play

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // initialize emic devices
  Serial.println("Intializing emic device...");
  emic.begin(rxpin, txpin);
  emic.setVoice(8); //sets the voice, there are 9 types, 0 - 8
  emic.setVolume(10); //sets the vloume, 10 is max 
  
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  testConnection();

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());
  //resetEEPROM(); // clears fact storage

  // read last address index stored
  readAddress();
  // read next address after last fact played
  readFactIndex();
}

// clears EEPROM and resets starting address
void resetEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  address = startingAddress;
  factIndex = startingAddress;
  writeAddress();
  writeFactIndex();
  Serial.println("EEPROM reset");
}

// breaks up 16 bit address to store into EEPROM (little endian)
void writeAddress() {
  int lower_8bits = address & 0xff;
  int upper_8bits = (address >> 8) & 0xff;
  EEPROM.write(0, lower_8bits);
  EEPROM.write(1, upper_8bits);
  printDebugging(2);
}

// reads 16 bit address from EEPROM (little endian)
void readAddress() {
  int lower_8bits = EEPROM.read(0);
  int upper_8bits = EEPROM.read(1);
  address = (upper_8bits << 8) | lower_8bits;
  printDebugging(3);
}

// reads last factIndex from EEPROM (little endian)
void writeFactIndex() {
  int lower_8bits = factIndex & 0xff;
  int upper_8bits = (factIndex >> 8) & 0xff;
  EEPROM.write(2, lower_8bits);
  EEPROM.write(3, upper_8bits);
}

// writes last factIndex address to EEPROM (little endian)
void readFactIndex() {
  int lower_8bits = EEPROM.read(2);
  int upper_8bits = EEPROM.read(3);
  factIndex = (upper_8bits << 8) | lower_8bits;
  Serial.println(factIndex);
}

void testConnection() {
  Serial.println("Testing device connections...");
  bool accelConnection = accelgyro.testConnection();
  Serial.println(accelConnection ? "MPU6050 connection successful" : "MPU6050 connection failed. Retrying.");

  while (accelConnection == false) { // keep testing connection if failed
    accelConnection = accelgyro.testConnection();
    delay(500);
  }
  Serial.println("MPU6050 connected");

  // set previous accelerations
  accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
}

// Caches fact when received from server
void cacheFactLocally(char* fact, int factLength) {
  if (address <= startingAddress) address = startingAddress - 1; // fact storage address starts at 4

  if ( (address + factLength) > EEPROM.length()) { // EEPROM memory does not have enough room to store fact
    Serial.println("Memory full. Can't cache fact");
    return;
  }
  else {
    if (strlen(fact) < maxFactSize) { // fact does not exceed max fact size
      // store fact byte by byte in EEPROM
      for (int i = 0; i <= factLength; i++) {
        address++;
        EEPROM.write(address, fact[i]);
        Serial.print(fact[i]);
        if (fact[i] == '\0') break; // end of fact string
      }
      Serial.println();
      // update address for next stored fact
      writeAddress();
    } 
    else {
      Serial.println("Fact exceeds maximum string size");
    }
  }
}

// plays fact on TTS module
void playFact(String fact) {
  Serial.println(fact);
  emic.speak(fact);
}

// get fact when shake or throw is detected
void getFact() {
  if(factIndex == startingAddress && address == startingAddress) {
    Serial.println("No facts in cache");
    return;
  }
  // reset factIndex if all facts have been played
  if (factIndex < startingAddress) factIndex = startingAddress;
  if (factIndex >= address) factIndex = startingAddress;

  String fact;
  printDebugging(4);
  // read fact from EEPROM
  while (EEPROM.read(factIndex) != '\0') {
    fact += (char)EEPROM.read(factIndex);
    factIndex++;
  }

  // update fact address for next fact to be played
  factIndex++;
  writeFactIndex();
  playFact(fact);
}

// Checks difference in acceleration for throw
void checkAcceleration() {
  accelgyro.getAcceleration(&ax, &ay, &az);
  printDebugging(0);
  x_diff = abs(ax - x_prev);
  y_diff = abs(ay - y_prev);
  z_diff = abs(az - z_prev);
  printDebugging(1);

  if ( x_diff > threshold || y_diff > threshold || z_diff > threshold ) {
    Serial.println("Throw or shake detected");
    Serial.println("Stating fact...");
    getFact(); // Read fact from EEPROM and plays it
    sampleAcceleration(50); // set delay long enough to  for fact to be played
  } else {
    x_prev = ax;
    y_prev = ay;
    z_prev = az;
    delay(500); // delay in acceleration sampling rate
  }
}

void sampleAcceleration(int samples) {
  for (int i = 0; i < samples; i++) {
    accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
    delay(100);
  }
}

void loop() {
  checkAcceleration();
  //if (ax == 0 && ay == 0 && az == 0) testConnection();
  
  #ifdef GENERATEFACTS
    generateFacts();
    delay(3000);
  #endif
}



















/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * Testing and Debugging functions
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */
 
// generate facts for testing purposes
#ifdef GENERATEFACTS
  FACTGENERATOR factGenerator;
  int factNum = factGenerator.getNumFacts();
  char fact[140];
  
  void generateFacts() {
    // generate facts for testing purposes
    for( int i = 0; i < factNum; i++) {
      factGenerator.getFact(i, fact);
      cacheFactLocally(fact, strlen(fact));
      delay(5000);
    }
  }
#endif

// Function serial prints for debuging purposes
void printDebugging(int function) {
  switch (function) {
    case 0: // Print acceleration
      Serial.print("Ax: ");
      Serial.println(ax);
      Serial.print("Ay: ");
      Serial.println(ay);
      Serial.print("Az: ");
      Serial.println(az);
      Serial.print("Ax previous: ");
      Serial.println(x_prev);
      Serial.print("Ay previous: ");
      Serial.println(y_prev);
      Serial.print("Az previous: ");
      Serial.println(z_prev);
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
