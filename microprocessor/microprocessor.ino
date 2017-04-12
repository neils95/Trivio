#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Wire.h>
#include "EMIC2.h"
#include "WiFiEsp.h"
#include "Battery.h"
#include "VoltageReference.h"

#define rxpin 3
#define txpin 4
int16_t startingAddress = 4;

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

int userID = 3;                 // which user is using ball
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "triviotoy.azurewebsites.net";

//char factFilename[] = "0";  // filename of next fact to be played
const int MAXFILENUM = 2000;  // maximum number of files for facts
int writeFileNumber = 0;  // filename of next fact to be stored
String fact = "";
int NUMBER_OF_FILES = 1;

Battery battery(3700, 4300, A0);
VoltageReference vRef;
bool battery_low;

WiFiEspClient client;
EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration

// led
#define redPin 16
#define greenPin 15
#define bluePin 14

// buttons
#define volumeDownButton 5
#define volumeUpButton 6
#define wifiButton 7

int volumeDownButtonState;            // current reading of button
int lastVolumeDownButtonState = LOW;  // the previous reading from the input pin
long lastVolumeDownDebounceTime = 0;  // the last time button was toggled

int volumeUpButtonState;              // current reading of button
int lastVolumeUpButtonState = LOW;    // the previous reading from the input pin
long lastVolumeUpDebounceTime = 0;    // the last time button was toggled

int wifiButtonState;            // current reading of button
int lastWifiButtonState = LOW;  // the previous reading from the input pin
long lastWifiDebounceTime = 0;  // the last time button was toggled
bool wifiSetupMode = false;

long debounceDelay = 100;    // the debounce time, increase if output flickers

//volume control
short int volume = 10;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  

  // initialize emic devices
  Serial.println(F("Intializing emic device..."));
  emic.begin(rxpin, txpin);
  emic.setVoice(8); // sets the voice, there are 9 types, 0 - 8
  emic.setVolume(volume); // sets the volume, 10 is max 
  
  
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  accelgyro.initialize();
  
  // verify connection
  testConnection();
//
//  // initialize serial for ESP module
//  Serial1.begin(9600);
//  // initialize ESP module
//  WiFi.init(&Serial1);
//
//  // attempt to connect to WiFi network
//  connectToNetwork();

  batteryVoltageSetup();
  setupSD();
  // get filename of last stored fact
  getFactStorageIndex();

  // set LED outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(volumeDownButton, INPUT);
  pinMode(volumeUpButton, INPUT);
  pinMode(wifiButton, INPUT);
  setColor(1);
}

void testConnection() {
  Serial.println(F("Testing device connections..."));
  bool accelConnection = accelgyro.testConnection();
  Serial.println(accelConnection ? "MPU6050 connection successful" : "MPU6050 connection failed. Retrying.");

  while (accelConnection == false) { // keep testing connection if failed
    accelConnection = accelgyro.testConnection();
    delay(500);
  }
  Serial.println(F("MPU6050 connected"));

  // set previous accelerations
  accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
}

void batteryVoltageSetup() {
  vRef.begin();
  int vcc = vRef.readVcc();
  battery.begin(vcc, 1); 
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

//Call this function every 5 mins when merged
//When it returns true, make the led red
bool batteryPercent() {
  int volt = battery.voltage();
  int percent = battery.level();
  if(percent <= 15) {
    battery_low = true;
  }
  else {
    battery_low = false;
  }
  return battery_low;
}

/**
 * Attempts to connect to WiFi if not already connnected
 * status global variable holds WiFi status 
 */
void connectToNetwork()
{
  char ssid[] = "Neil";            // your network SSID (name)
  char pass[] = "0123456789";        // your network password
  if ( status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
}

// stores user id
void setUserId(String id) {
  char filename[] = "u.txt";
  File file;
  if(SD.exists(filename)) {
    file = SD.open(filename);
    file.println(id);
    file.close();
  } else { // create file
    File file = SD.open(filename,FILE_WRITE);
    file.println(id);
    file.close();
  }
  // update global variable
  userID = id.toInt();
  delay(500);
}

// gets user id
void getUserId() {
  char filename[] = "u.txt";
  File file;
  String number = "3";
  // get filename to store fact as
  if(SD.exists(filename)) {
    number = "";
    file = SD.open(filename);
    while(file.available()) { // read from file
      char num = file.read();
      if(num != '\n') {
        number += num;
      }
    }
    number.trim();
    file.close();
  } else {
    // create file
    file = SD.open(filename,FILE_WRITE);
    file.println(number);
    file.close();
  }
  Serial.print(F("ID: "));
  Serial.println(number);
  userID = number.toInt(); // update global variable
  delay(500);
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
          factFilename += ltr;
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
  delay(500);
  return factFilename;
}

// writes to file what fact to play next time
void updatePlayFileName(String factFilename) {
  // increment and store fact index to be played next time
  int number = factFilename.toInt();
  number++;
  factFilename = String(number);
  char playFilename[] = "p.txt";
  if(SD.exists(playFilename)) {
    // read/write from file
    File file = SD.open(playFilename);
    file.println(factFilename);
    file.close();
  } else {
    // create file
    File file = SD.open(playFilename,FILE_WRITE);
    file.println("0");
    file.close();
  }
  Serial.print(F("Updating fact Index: "));
  Serial.println(number);
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
  factFilename.trim();

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
    // increment and store fact index to be played next time
    updatePlayFileName(factFilename);
  } else {
    factString = "No fact available.";
  }
  
  return factString;
}

/**
 * Call this function when storing fact from server
 */
// stores fact
void storeFact(String factString) {
  File file;
  String filename = String(writeFileNumber);
  filename = filename + ".txt";
  // create/open file and store fact
  if(SD.exists(filename)) {
    file = SD.open(filename);
    file.println(factString);
    file.close();
  } else {
    File file = SD.open(filename,FILE_WRITE);
    file.println(factString);
    file.close();
  }

  // increment name of file to store next fact as
  writeFileNumber++;
  delay(500);
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
    // update fact index of last stored
    file = SD.open(writeFilename);
    if(file) {
      // if max reached, go back to 0
      if(writeFileNumber > MAXFILENUM) {
        writeFileNumber = 0;
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
void playFact(String fact) {
  Serial.print(F("Fact: "));
  Serial.println(fact);
  emic.speak(fact);
  updateServerPlayCount();
}

/**
 * Call this function when throw is detected
 */
// get fact when shake or throw is detected
void getFact() {
  String fact = getFactFromFile();
  delay(100);
  playFact(fact);
}

/**
 * Detects if server is sending over bits of fact string data
 * and reads into a String
 */
void readInFact(){
  boolean isFact = false;
  while (client.available()) {
    char c = client.read();
    //begin counting characters in string on " character
    if(c == 34){
      isFact = true;
    }
    //write character in to fact string
    if(isFact == true && c != 34){
      fact += c;
    }
  }
}

/**
 * Resets fact string for next retrieval and calls storeFact function
 */
void resetFact(){
  if(fact.length() > 20){
    storeFact(fact);
    Serial.println(fact);
    fact = "";
  }
}

void makeFactRequest() {
  if(factRequestSuccessful()){ //make request to server for another fact
      //TODO: Send request with history counter to server
      //TODO: Reset history counter
    }else{
      //TODO: Increment history counter on toy to send when connection finally made
    }
}

// Checks difference in acceleration for throw
void checkAcceleration() {
  accelgyro.getAcceleration(&ax, &ay, &az);
  //printDebugging(0);
  x_diff = abs(ax - x_prev);
  y_diff = abs(ay - y_prev);
  z_diff = abs(az - z_prev);
  //printDebugging(1);
  //makeFactRequest();
  if ( x_diff > threshold || y_diff > threshold || z_diff > threshold ) {
    Serial.println(F("Throw or shake detected"));
    getFact(); // Read fact from EEPROM and plays it
    
    sampleAcceleration(50); // set delay long enough to  for fact to be played
  } else {
    x_prev = ax;
    y_prev = ay;
    z_prev = az;
    checkButtons();
    delay(500); // delay in acceleration sampling rate
  }
}

void sampleAcceleration(int samples) {
  for (int i = 0; i < samples; i++) {
    checkButtons();
    accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
    delay(100);
  }
}

// this method makes a HTTP GET connection to the server
bool factRequestSuccessful()
{
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    
    // send the HTTP PUT request
    client.println("GET /Trivia/" + String(userID) + " HTTP/1.1");
    client.println("Host: triviotoy.azurewebsites.net");
    client.println("Connection: close");
    client.println();

    //successful connection
    return true;
  }
  else {
    // if you couldn't make a connection
    return false;
  }
}

//Write to analog outputs
void setColor(int setting) {
  int red = 0;
  int green = 0;
  int blue = 0;

  switch(setting) {
    case 0: { // red - low battery
      red = 255;
      break;
    }
    case 1: { // green - connected to wifi
      green = 255;
      break;
    }
    case 2: { // purple - not connected to wifi
      red = 255;
      blue = 255;
      break;
    }
    case 3: { // blue - updating from server
      blue = 255;
      break;
    }
    case 4: { // white - wifi connect mode
      red = 255;
      blue = 255;
      green = 255;
      break;
    }
    default: { // off
      break;
    }
  }
  
  uint8_t redIn = 255 - uint8_t(red);
  uint8_t greenIn = 255 - uint8_t(green);
  uint8_t blueIn = 255 - uint8_t(blue);

  analogWrite(redPin, redIn);
  analogWrite(greenPin, greenIn);
  analogWrite(bluePin, blueIn);
}

// detects input of volume button with debouncing
void checkVolumeDownInput() {
  int reading = digitalRead(volumeDownButton);  // current reading of button, debouncing

  // switch changed due to noise or pressing
  if (reading != lastVolumeDownButtonState) {
    lastVolumeDownDebounceTime = millis();      // reset the debouncing timer
  }

  // delay time for debouncing has been reached
  if ((millis() - lastVolumeDownDebounceTime) > debounceDelay) {

    if (reading != volumeDownButtonState) {     // button state changed
      volumeDownButtonState = reading;

      // button press was detected with debouncing taken into account
      if (volumeDownButtonState == HIGH) {
        Serial.println(F("V down"));
        if(volume > 0) {
          volume--;
        }
        emic.setVolume(volume);
      }
    }
  }
  lastVolumeDownButtonState = reading;
}

// detects input of volume button with debouncing
void checkVolumeUpInput() {
  int reading = digitalRead(volumeUpButton);  // current reading of button, debouncing

  // switch changed due to noise or pressing
  if (reading != lastVolumeUpButtonState) {
    lastVolumeUpDebounceTime = millis();      // reset the debouncing timer
  }

  // delay time for debouncing has been reached
  if ((millis() - lastVolumeUpDebounceTime) > debounceDelay) {

    if (reading != volumeUpButtonState) {     // button state changed
      volumeUpButtonState = reading;

      // button press was detected with debouncing taken into account
      if (volumeUpButtonState == HIGH) {
        Serial.println(F("V up"));
        if(volume < 10) {
          volume++;
        }
        emic.setVolume(volume);
      }
    }
  }
  lastVolumeUpButtonState = reading;
}

// detects input of volume button with debouncing
void checkWifiButtonInput() {
  int reading = digitalRead(wifiButton);  // current reading of button, debouncing

  // switch changed due to noise or pressing
  if (reading != lastWifiButtonState) {
    lastWifiDebounceTime = millis();      // reset the debouncing timer
  }

  // delay time for debouncing has been reached
  if ((millis() - lastWifiDebounceTime) > (debounceDelay * 5)) {

    if (reading != wifiButtonState) {     // button state changed
      wifiButtonState = reading;

      // button press was detected with debouncing taken into account
      if (wifiButtonState == HIGH) {
        Serial.println(F("Wifi setup mode"));
        wifiSetupMode = !wifiSetupMode;
        
        if(wifiSetupMode) {
          setColor(4);
          //TODO: put function to call wifi setup mode here
        }
      }
    }
  }
  lastWifiButtonState = reading;
}

void checkButtons() {
  /*checkVolumeDownInput();
  checkVolumeUpInput();
  checkWifiButtonInput();*/
}

void loop() {
  //if there's incoming data over server connection, read in the fact
  //readInFact();

  //if fact has been read in -> cache fact and reset string for next retrieval
  //resetFact();
  
  checkAcceleration();

  checkButtons();
}
/*
// Function serial prints for debuging purposes
void printDebugging(int function) {
  switch (function) {
    case 0: // Print acceleration
      Serial.print(F("Ax: "));
      Serial.println(ax);
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
