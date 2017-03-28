#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
//#include <Wire.h>
#include "EMIC2.h"
#include "WiFiEsp.h"
#include "ESP8266.h"
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

ESP8266 wifi(Serial1);
String ssid;
String pass;

WiFiEspClient client;
EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration
int factCount = 1;

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

  //espSetup();

  // get filename of last stored fact
  getFactStorageIndex();

  numPlayed = getServerPlayCount();
  hardwareSetup();
}

void hardwareSetup() {
  // set LED outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(volumeDownButton, INPUT);
  pinMode(volumeUpButton, INPUT);
  pinMode(wifiButton, INPUT);
  setColor(1);
}

void espSetup() {
  //sets the baud rate between the esp and arduino
  Serial.println(F("Starting wifi setup"));
  bool baud = wifi.autoSetBaud();
  
  //setting the mode (mode 3) of the esp to operate in both AP and Station mode
  bool mode = wifi.setOprToStationSoftAP();

  //If mode is correctly set, create the esp's wifi network
  if(mode) {
    wifi.setSoftAPParam("myESP", "1234", 3, 0);
    Serial.print(F("Set AP: "));
    Serial.println(F("complete"));
  }

  //Making the esp in single mode so that there is only one connection at a time
  bool mux_disabled = wifi.disableMUX();
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
          tcpMode();
        }
      }
    }
  }
  lastWifiButtonState = reading;
}

void checkButtons() {
  checkVolumeDownInput();
  checkVolumeUpInput();
  checkWifiButtonInput();
}

bool create_TCP_connection() {
  //creating a TCP connection to the phone
  while(create_TCP() == false) {
    Serial.println("Couldn't create");
    create_TCP();
  }
  String ack = receive_data();
  while(ack.length() < 0) {
    ack = receive_data();
  }
  
  if(ack == "ok") {
//    Serial.println("Ack received");
    //sending ack back
    bool sent = send_data("ok");
    while(sent == false) {
//      Serial.println("Trying to send ack");
      sent = send_data("ok");
    }
//    Serial.println("Ack Sent");
    return sent;
  }
}

void connect_to_wifi() {
    bool send_confirmation;
    bool send_failure;  
    //Waiting for wifi credentials
    String cred = receive_data();
    while(cred.length() < 0) {
      cred = receive_data();
    }
    //separating the credentials
    bool break_cred = break_credentials(cred);
    //Trying to connect to the wifi
    Serial.println(F("Connecting to the wifi..."));
    bool connect_wifi = wifi.joinAP(ssid, pass);
    if(connect_wifi == true) {
      send_confirmation = send_data("yes");
      while(send_confirmation == false) {
        send_confirmation = send_data("yes");
      }
    }
    else {
      send_failure = send_data("no");
      while(send_failure == false) {
        send_failure = send_data("no");
      }
    }
    bool closed = wifi.releaseTCP();
    Serial.println(closed);
}

//breaking the credentials received from the phone in the format "ssid:pass"
bool break_credentials(String data) {
  if(data.length() > 0) {
    int colon_first = data.indexOf(':');
    String userid = data.substring(0, colon_first);
    String creds = data.substring(colon_first + 1);
    int colon_second = creds.indexOf(':');
    ssid = creds.substring(0, colon_second);
    pass = creds.substring(colon_second + 1);
    return true;
  }
  return false;
}

//Breaking the IP of device connected as it is given as "ip,mac-address" 
String break_ip(String ip) {
  int comma_index = ip.indexOf(',');
  String actual_ip = "";
  if(ip.length() > 0) {
      actual_ip = ip.substring(0, comma_index);
      return actual_ip;
    }
  return "";
}

//create a tcp connection with the phone
bool create_TCP() {
  bool tcp_created;
  String ip_device = wifi.getJoinedDeviceIP();
  ip_device = break_ip(ip_device);
  Serial.println(ip_device);
  if(ip_device.length() > 0) {
    tcp_created = wifi.createTCP(ip_device, 8080);
  }
  return tcp_created;
}

//Used to send data to the phone and if succesful will return true
bool send_data(String data) {
  int len = data.length();
  byte *buff = new byte[len];
  for(int i = 0; i < len; i++) {
    buff[i] = data[i];
  }
  bool sent = wifi.send(buff, len);
  return sent;
}

//Used to receive data from the phone
String receive_data() {
  uint8_t buff[128] = {0};
  uint32_t len = wifi.recv(buff, sizeof(buff), 120000);
  String data = "";
  
  if(len > 0) {
    for(int i = 0; i < len; i++) {
      data += char(buff[i]);
    }
    data[len] = '\0';
    return data;
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
