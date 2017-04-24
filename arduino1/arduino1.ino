#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "EMIC2.h"
#include <Wire.h>

#define rxpin 3
#define txpin 4

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(9, 8); // RX, TX
#endif

const int MAXFILENUM = 2000;  // maximum number of files for facts
int writeFileNumber = 0;  // filename of next fact to be stored
int NUMBER_OF_FILES = 1;
bool readingFromServer = false;

EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration
int factCount = 1;

// buttons
#define volumeDownButton 5
#define volumeUpButton 6

int volumeDownButtonState;            // current reading of button
int lastVolumeDownButtonState = 0;  // the previous reading from the input pin
long lastVolumeDownDebounceTime = 0;  // the last time button was toggled

int volumeUpButtonState;              // current reading of button
int lastVolumeUpButtonState = 0;    // the previous reading from the input pin
long lastVolumeUpDebounceTime = 0;    // the last time button was toggled

long debounceDelay = 50;    // the debounce time, increase if output flickers

//volume control
short int volume = 10;

// arduino states
bool wifiSetupMode = false;

int serverCount = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // I2C bus for arduino to arduino communication
  Wire.onReceive(arduinoReceive);

  // initialize emic devices
  Serial.println(F("Intializing emic device..."));
  emic.begin(rxpin, txpin, 10);
  emic.setVoice(8); // sets the voice, there are 9 types, 0 - 8
  emic.setVolume(10); // sets the volume, 10 is max 
  
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  accelgyro.initialize();
  
  // verify connection
  testConnection();
  
  // get filename of last stored fact
  getFactStorageIndex();
  
  hardwareSetup();

  // get number of files played since last server update
  serverCount = getServerPlayCount();
}

void hardwareSetup() {
  pinMode(volumeDownButton, INPUT);
  pinMode(volumeUpButton, INPUT);
}

void testConnection() {
  Serial.println(F("Testing device connections..."));
  bool accelConnection = accelgyro.testConnection();
  if(!accelConnection){
    Serial.println(F("MPU6050 connection failed. Retrying."));
  }

  while (accelConnection == false) { // keep testing connection if failed
    accelConnection = accelgyro.testConnection();
    delay(500);
  }
  Serial.println(F("MPU6050 connected"));

  // set previous accelerations
  accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
}

/*
 * Call this function to get number to return to server for how many facts has been played
 */
// get number of facts played since last server pull
int getServerPlayCount() {
  char serverCountFile[] = "s.txt";
  String numString = "";
  int number = serverCount;
  if(SD.exists(serverCountFile)) {
    File file = SD.open(serverCountFile);
    while(file.available()) {
      char numChar = file.read();
      if(numChar != '\n') {
        numString += numChar;
      }
    }
    number = numString.toInt();
    file.close();
  } else {
    // create file
    File file = SD.open(serverCountFile,FILE_WRITE);
    numString = String(serverCount);
    file.print(numString);
    file.close();
  }
  delay(500);
  return number;
}

// updates count of how many facts have been played since last server pull
void updateServerPlayCount() {
  char serverCountFile[] = "s.txt";
  
  String numString = String(serverCount);
  if(SD.exists(serverCountFile)) {
    SD.remove(serverCountFile);
  }
  // write file
  File file = SD.open(serverCountFile,FILE_WRITE);
  file.print(numString);
  file.close();
  
  delay(500);
}

// resets count played back to 0 since last server update
void resetServerPlayCount() {
  char serverCountFile[] = "s.txt";
  if(SD.exists(serverCountFile)) {
    SD.remove(serverCountFile);
  }
  // write to file
  File file = SD.open(serverCountFile,FILE_WRITE);
  file.print("0");
  file.close();
  delay(500);
}

// gets name of file for fact to be played and stores in global variable
String getPlayFilename() {
  Serial.print(F("Getting file to be played: "));
  String factFilename = "0";
  char playFilename[] = "p.txt";
  if(SD.exists(playFilename)) {
    // read/write from file
    File file = SD.open(playFilename);
    if(file) {
      factFilename = "";
      while(file.available()) {
        char ltr = file.read();
        if(ltr != '\n') {
          factFilename = factFilename + ltr;
        }
      }
      //Serial.println(factFilename);
      file.close();
    }
  } else {
    // create file
    File file = SD.open(playFilename,FILE_WRITE);
    if(file) {
      file.print("0");
      file.close();
      //Serial.println(F("DNE. File created."));
    }
  }
  factFilename.trim();
  delay(500);
  return factFilename;
}

// writes to file what fact to play next time
void updatePlayFileName(String factFilename) {
  // increment and store fact index to be played next time
  factFilename.trim();
  int number = factFilename.toInt();
  number++;
  factFilename = String(number);
  factFilename.trim();
  String numberName = factFilename;
  numberName = numberName + ".txt";
  numberName.trim();
  char playFilename[] = "p.txt";
  if(SD.exists(playFilename)) {
    if(!SD.exists(numberName)) {
      factFilename = "0";
    }
    SD.remove(playFilename);
    // read/write from file
    File file = SD.open(playFilename,FILE_WRITE);
    file.print(factFilename);
    file.close();
    Serial.print(F("Updating fact Index: "));
    Serial.println(number);
  } else {
    // create file
    File file = SD.open(playFilename,FILE_WRITE);
    file.print("0");
    file.close();
  }
  delay(500);
}

// gets fact string to play
String getFactFromFile(String factFilename) {
  // get name of file to play
  factFilename.trim();
  int number = factFilename.toInt();
  factFilename = factFilename + ".txt";
  //Serial.print(F("Opening file: "));
  //Serial.println(factFilename);

  // get fact from file
  String factString = "";
  // re-open the file for reading:
  File file = SD.open(factFilename);
  if (file) {
    // read from file
    while(file.available()) {
      char ltr =  file.read();
      if(ltr != '\n') {
        factString += ltr;
      }
    }
    file.close();
    delay(500);
  } else {
    factString = "";
  }
  
  return factString;
}

// gets index of last stored fact
void getFactStorageIndex() {
  char writeFilename[] = "w.txt";
  File file;
  String number = "0";
  // get filename to store fact as
  if(SD.exists(writeFilename)) {
    number = "";
    // read/write from file
    file = SD.open(writeFilename);
    while(file.available()) {
      char num = file.read();
      if(num != '\n') {
        number = number + String(num);
      }
    }
    file.close();
  } else {
    // create file
    File file = SD.open(writeFilename,FILE_WRITE);
    file.print(number);
    file.close();
  }
  // sets it as global variable
  writeFileNumber = number.toInt();
  delay(500);
}

// updates saving index of last stored fact
void updateFactStorageIndex() {
  //Serial.println(F("Update Fact Storage Index: "));
  char writeFilename[] = "w.txt";
  File file;
  if(SD.exists(writeFilename)) {
    SD.remove(writeFilename);
  }
    
  // update fact index of last stored
  file = SD.open(writeFilename,FILE_WRITE);
  // if max reached, go back to 0
  if(writeFileNumber > MAXFILENUM) {
    writeFileNumber = 0;
  } else {
    writeFileNumber++;
  }
  file.println(String(writeFileNumber));
  file.close();
  Serial.println(writeFileNumber);
  delay(500);
}

// plays fact on TTS module
void playFact(String factFile) {
  factFile.trim();
  String filename = factFile;
  filename = filename + ".txt";
  filename.trim();
  Serial.print(F("Fact filename: "));
  Serial.println(filename);
  
  emic.speak(filename,10);

  int num = factFile.toInt();
  //updateServerPlayCount(num);
    
  // increment and store fact index to be played next time
  updatePlayFileName(factFile);

  // make fact request
  arduinoSend(serverCount); 
}

/**
 * Call this function when throw is detected
 */
// get fact when shake or throw is detected
void getFact() {
  String factFile = getPlayFilename();
  factFile.trim();
  delay(100);
  playFact(factFile);
}

void storeFact(char* fact, int factLength) {
  String filename = String(writeFileNumber);
  filename = filename + ".txt";
  SD.remove(filename);
  File file = SD.open(filename,FILE_WRITE);
  for(int i = 0; i < factLength; i++) {
    file.print(fact[i]);
  }
  file.close();
  delay(500);
}

// Checks difference in acceleration for throw
bool checkAcceleration() {
  accelgyro.getAcceleration(&ax, &ay, &az);
  //printDebugging(0);
  x_diff = abs(ax - x_prev);
  y_diff = abs(ay - y_prev);
  z_diff = abs(az - z_prev);
  if ( x_diff > threshold || y_diff > threshold || z_diff > threshold ) {
    Serial.println(F("accel detected"));
    getFact(); // Read fact from EEPROM and plays it
    
    sampleAcceleration(50); // set delay long enough to  for fact to be played
    return true;
  } else {
    x_prev = ax;
    y_prev = ay;
    z_prev = az;
    delay(500); // delay in acceleration sampling rate
    return false;
  }
}

void sampleAcceleration(int samples) {
  for (int i = 0; i < samples; i++) {
    accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
    delay(100);
  }
}

void arduinoSend(int numRequests) {
  Wire.beginTransmission(8);
  String sendStr = String(numRequests);
  sendStr = "m" + sendStr;
  Wire.write(sendStr.c_str()); // make fact request
  Wire.endTransmission();
  delay(500);
}

/**
 * f<fact string>: send fact
 * w: wifi setup mode
 * d: done with wifi setup mode
 */
void arduinoReceive(int howMany) {
  bool start = true;
  char state;
  char fact[140];
  bool isFact = false;
  int count = 0;
  while(1 < Wire.available()) {
    char c = Wire.read();
    if(start) {
      state = c;
      start = false;
    } else {
      switch (state) {
        case 'f': { // store fact
          isFact = true;
          fact[count] = c;
          count++;
          break;
        }
        case 'w': { // wifi setup mode
          wifiSetupMode = true;
          break; 
        }
        case 'd': { // done with wifi setup mode
          wifiSetupMode = false;
          break; 
        }
      }
    }
    Serial.print(c);
  }
  delay(500);

  if(isFact) {
    storeFact(fact, count);
  }
}

void loop() {
  if(!wifiSetupMode) { // disable fact playing if setting up wifi on other arduino
    checkAcceleration();
  }
}
