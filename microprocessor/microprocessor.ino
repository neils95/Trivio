#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Wire.h>
#include <EEPROM.h>
#include "EMIC2.h"
//need to add the emic 2 library

#define rxpin 4
#define txpin 5
int16_t startingAddress = 4;

const String playFilename = "factNumber.txt"; // file that stores filename of next fact to be played
String factFilename = "0";  // filename of next fact to be played
const int MAXFILENUM = 2000;  // maximum number of files for facts
const String writeFilename = "writeNumber.txt"; // file that stores filename of next fact to be stored
int writeFileNumber = 0;  // filename of next fact to be stored
const String serverCountFile = "serverCountFile.txt"; // file that stores how many facts have been played since last server update
const String txt = ".txt"; 

EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // initialize emic devices
  Serial.println("Intializing emic device...");
  emic.begin(rxpin, txpin);
  emic.setVoice(8); // sets the voice, there are 9 types, 0 - 8
  emic.setVolume(10); // sets the vloume, 10 is max 
  
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  testConnection();

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());
  //resetEEPROM(); // clears fact storage

  // get filename of last stored fact
  getFactStorageIndex();
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

void updateServerPlayCount() {
  if(SD.exists(serverCountFile) {
    // read/write from file
    File file = SD.open(serverCountFile);
    while(file.available()) {
      factFilename = file.read();
    }
  } else {
    // create file
    File file = SD.open(serverCountFile,"FILE_WRITE");
    file.write("0");
    file.close();
  }
}

// resets count played back to 0 since last server update
void resetServerPlayCount() {
  if(SD.exists(serverCountFile) {
    // read/write from file
    File file = SD.open(serverCountFile);
    while(file.available()) {
      factFilename = file.read();
    }
  } else {
    // create file
    File file = SD.open(serverCountFile,"FILE_WRITE");
    file.write("0");
    file.close();
  }
}

// gets name of file for fact to be played and stores in global variable
void getPlayFilename() {
  if(SD.exists(playFilename) {
    // read/write from file
    File file = SD.open(playFilename);
    while(file.available()) {
      factFilename = file.read();
    }
  } else {
    // create file
    File file = SD.open(playFilename,"FILE_WRITE");
    file.write("0");
    file.close();
  }
}

// writes to file what fact to play next time
void updatePlayFileName() {
  // increment and store fact index to be played next time
  int number = factFilename.toInt();
  number++;
  factFilename = String(number);
  
  if(SD.exists(playFilename) {
    // read/write from file
    File file = SD.open(playFilename);
    file.write(factFilename);
  } else {
    // create file
    File file = SD.open(playFilename,"FILE_WRITE");
    file.write("0");
    file.close();
  }
}

// gets fact string to play
void String getFactFromFile() {
  // get name of file to play
  getPlayFilename();

  // get fact from file
  String factString = "";
  if(SD.exists(factFilename + txt) {
    // read from file
    File file = SD.open(factFilename + txt);
    factString =  file.read();
  }
  // increment and store fact index to be played next time
  updatePlayFileName();
  return factString;
}

/**
 * Call this function when storing fact from server
 */
// stores fact
void storeFact(String factString) {
  File file;
  String filename = String(writeFileNumber) + txt;
  // create/open file and store fact
  if(SD.exists(filename)) {
    file = SD.open(filename);
    file.write(factString);
    file.close();
  } else {
    File file = SD.open(filename,"FILE_WRITE");
    file.write(factString);
    file.close();
  }

  // increment name of file to store next fact as
  writeFileNumber++;
}

// gets index of last stored fact
void getFactStorageIndex() {
  File file;
  String number = "0";
  // get filename to store fact as
  if(SD.exists(writeFilename) {
    // read/write from file
    file = SD.open(writeFilename);
    while(file.available()) {
      number = file.read();
    }
    file.close();
  } else {
    // create file
    File file = SD.open(writeFilename,"FILE_WRITE");
    file.write("0");
    file.close();
  }

  // sets it as global variable
  writeFileNumber = number.toInt();
}

// updates saving index of last stored fact
void updateFactStorageIndex() {
  File file;
  if(SD.exists(writeFilename) {
    // update fact index of last stored
    file = SD.open(writeFilename);
    // if max reached, go back to 0
    if(writeFactNumber > MAXFILENUM) {
      writeFactNumber = 0;
    }
    file.write(String(writeFactNumber));
    file.close();
  } else {
    // create file
    File file = SD.open(writeFilename,"FILE_WRITE");
    file.write("0");
    file.close();
  }
}

// plays fact on TTS module
void playFact(String fact) {
  Serial.println(fact);
  emic.speak(fact);
}

/**
 * Call this function when throw is detected
 */
// get fact when shake or throw is detected
void getFact() {
  String fact;
  fact = getFactFromFile();
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
}

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
