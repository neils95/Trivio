#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
//#include <Wire.h>
#include "EMIC2.h"
#include "WiFiEsp.h"
//need to add the emic 2 library

#define rxpin 3
#define txpin 4
int16_t startingAddress = 4;

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(9, 8); // RX, TX
#endif

int status = WL_IDLE_STATUS;     // the Wifi radio's status

//char factFilename[] = "0";  // filename of next fact to be played
const int MAXFILENUM = 2000;  // maximum number of files for facts
int writeFileNumber = 0;  // filename of next fact to be stored
int NUMBER_OF_FILES = 1;
bool readingFromServer = false;
short numPlayed = 0;
//char fact[140];

WiFiEspClient client;
EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration
int factCount = 1;
void setup() {
  Serial.begin(9600);
  //Wire.begin();
  

  //setupSD();
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
//
//  // initialize serial for ESP module
  Serial1.begin(9600);
//  // initialize ESP module
  WiFi.init(&Serial1);
//
//  // attempt to connect to WiFi network
  connectToNetwork();

  // get filename of last stored fact
  getFactStorageIndex();

  numPlayed = getServerPlayCount();
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

void setupSD() {
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(10, OUTPUT);
  
  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(10)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
}

/**
 * Attempts to connect to WiFi if not already connnected
 * status global variable holds WiFi status 
 */
void connectToNetwork()
{
//  char ssid[] = "DanseyPhone";            // your network SSID (name)
//  char pass[] = "12345679";        // your network password
  char ssid[] = "Neil";            // your network SSID (name)
  char pass[] = "0123456789";
  if ( status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
}

/*
 * Call this function to get number to return to server for how many facts has been played
 */
// get number of facts played since last server pull
int getServerPlayCount() {
  char serverCountFile[] = "s.txt";
  String numString = "";
  int number = 0;
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
    file.println("0");
    file.close();
  }
  delay(500);
  return number;
}

// updates count of how many facts have been played since last server pull
void updateServerPlayCount() {
  char serverCountFile[] = "s.txt";
  int number = getServerPlayCount();
  if(number < MAXFILENUM) {
    number++;
  
    String numString = "";
    if(SD.exists(serverCountFile)) {
      SD.remove(serverCountFile);
      // write file
      File file = SD.open(serverCountFile);
      file.println(String(number));
      file.close();
    } else {
      // create file
      File file = SD.open(serverCountFile,FILE_WRITE);
      file.println("0");
      file.close();
    }
  }
  delay(500);
}

// resets count played back to 0 since last server update
void resetServerPlayCount() {
  char serverCountFile[] = "s.txt";
  if(SD.exists(serverCountFile)) {
    SD.remove(serverCountFile);
    // read/write from file
    File file = SD.open(serverCountFile);
    file.println("0");
    file.close();
  } else {
    // create file
    File file = SD.open(serverCountFile,FILE_WRITE);
    file.println("0");
    file.close();
  }
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
      Serial.println(factFilename);
      file.close();
    }
  } else {
    // create file
    File file = SD.open(playFilename,FILE_WRITE);
    if(file) {
      file.println("0");
      file.close();
      Serial.println(F("DNE. File created."));
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
    file.println(factFilename);
    file.close();
    Serial.print(F("Updating fact Index: "));
    Serial.println(number);
  } else {
    // create file
    File file = SD.open(playFilename,FILE_WRITE);
    file.println("0");
    file.close();
  }
  delay(500);
}

// gets fact string to play
String getFactFromFile() {
  // get name of file to play
  String factFilename = getPlayFilename();
  factFilename.trim();
  int number = factFilename.toInt();
  factFilename = factFilename + ".txt";
  Serial.print(F("Opening file: "));
  Serial.println(factFilename);

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
    factString = "No fact available.";
  }
  
  return factString;
}

/**
 * Call this function when storing fact from server
 */
// stores fact
void storeFact(String factString, int fileNum) {
  File file;
  String filename = String(fileNum);
  filename = filename + ".txt";
  // create/open file and store fact
  if(SD.exists(filename)) {
    SD.remove(filename);
  }
  
  Serial.println(F("Storing Fact: "));
  file = SD.open(filename,FILE_WRITE);
  if(file) {
    Serial.println(factString);
    file.println(factString);
    file.close();
  }

  // increment name of file to store next fact as
  //writeFileNumber++;
  delay(500);
}

// gets index of last stored fact
void getFactStorageIndex() {
  char writeFilename[] = "w.txt";
  File file;
  String number = "0";
  // get filename to store fact as
  if(SD.exists(writeFilename)) {
    SD.remove(writeFilename);
    number = "";
    // read/write from file
    file = SD.open(writeFilename);
    while(file.available()) {
      char num = file.read();
      if(num != '\n') {
        number += num;
      }
    }
    file.close();
  } else {
    // create file
    File file = SD.open(writeFilename,FILE_WRITE);
    file.println(number);
    file.close();
  }
  Serial.println(F("Last stored fact index: "));
  Serial.println(number);
  // sets it as global variable
  writeFileNumber = number.toInt();
  delay(500);
}

// updates saving index of last stored fact
void updateFactStorageIndex() {
  Serial.println(F("Update Fact Storage Index: "));
  char writeFilename[] = "w.txt";
  File file;
  if(SD.exists(writeFilename)) {
    SD.remove(writeFilename);
    
    // update fact index of last stored
    file = SD.open(writeFilename,FILE_WRITE);
    if(file) {
      // if max reached, go back to 0
      if(writeFileNumber > MAXFILENUM) {
        writeFileNumber = 0;
      } else {
        writeFileNumber++;
      }
      file.println(String(writeFileNumber));
      file.close();
      Serial.println(writeFileNumber);
    }
  } else {
    // create file
    File file = SD.open(writeFilename,FILE_WRITE);
    file.println("0");
    file.close();
  }
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

//char fact[140];

/**
 * //Detects if server is sending over bits of fact string data
 * //and reads into a String
 */
void readInFact(){
  boolean isFact = false;
  int i = 0;
  String fact = "";
  while (client.available()) {
    char c = client.read();
    //begin counting characters in string on " character
    if(c == 34){
      isFact = true;
    }
    //write character in to fact string
    if(isFact == true && c != 34){
      Serial.write(c);
      fact = fact + c;
      i++;
    }
  }
  
  //if fact was pulled in, i will be greater than 1. 
  //Store end character and flip reading from server to false
  if(fact.length() > 20){
    Serial.println(fact);
    storeFact(fact, factCount);
    factCount++;
    //Serial.write('\0');
    //Serial.write('\n');
    readingFromServer = false;
    client.stop();
  } else {
    delay(500);
  }
}

void makeFactRequest() {
  if(factRequestSuccessful()){ //make request to server for another fact
      readingFromServer = true;
    }else{
      //TODO: Increment history counter on toy to send when connection finally made
    }
}

// Checks difference in acceleration for throw
bool checkAcceleration() {
  accelgyro.getAcceleration(&ax, &ay, &az);
  //printDebugging(0);
  x_diff = abs(ax - x_prev);
  y_diff = abs(ay - y_prev);
  z_diff = abs(az - z_prev);
  //printDebugging(1);
  //makeFactRequest();
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

bool factRequestSuccessful()
{
  char server[] = "triviotoy.azurewebsites.net";
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();
  Serial.println(F("about to try to connect"));
  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println(F("Connecting..."));
    
    // send the HTTP PUT request
    client.println(F("GET /Trivia/3 HTTP/1.1"));
    client.println(F("Host: triviotoy.azurewebsites.net"));
    client.println(F("Connection: close"));
    client.println();

    return true;
  }
  else {
    // if you couldn't make a connection
    return false;
  }
}

void loop() {
  //if there's incoming data over server connection, read in the fact
  readInFact();

  //if fact has been read in -> cache fact and reset string for next retrieval
  //resetFact();

//  if(factCount < 50 && status == WL_CONNECTED) {
//    makeFactRequest();
//  }
//  

  checkAcceleration();
  /*if(!readingFromServer){
    if(numPlayed <= 0){
      bool factRead = checkAcceleration();
      if(factRead){
       updateServerPlayCount();
       makeFactRequest();
      }
    }else{
      makeFactRequest();
      numPlayed--;
    }
  }*/
}

// Function serial prints for debuging purposes
/*void printDebugging(int function) {
  switch (function) {
    case 0: // Print acceleration
      Serial.print(F("Ax: "));
      Serial.println(F(ax));
      Serial.print(F("Ay: "));
      Serial.println(ay);
      Serial.print(F("Az: "));
      Serial.println(az);
      Serial.print(F("Ax previous: "));
      Serial.println(x_prev);
      Serial.print(F("Ay previous: "));
      Serial.println(y_prev);
      Serial.print(F("Az previous: "));
      Serial.println(z_prev);
      break;
    case 1: // Print acceleration differences for testing purposes
      Serial.print(F("x diff: "));
      Serial.println(x_diff);
      Serial.print(F("y diff: "));
      Serial.println(y_diff);
      Serial.print(F("z diff: "));
      Serial.println(z_diff);
      break;
  }
  Serial.println();
}*/
