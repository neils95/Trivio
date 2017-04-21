#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "EMIC2.h"
#include "ESP8266.h"
#include <WiFi.h>
#include "Battery.h"
#include "VoltageReference.h"
#include <EEPROM.h>

#define rxpin 10
#define txpin 11
int16_t startingAddress = 4;

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
#endif


const int MAXFILENUM = 2000;    // maximum number of files for facts
int writeFileNumber = 0;        // filename of next fact to be stored
int serverCount = 0;            // number of distinct facts played since last server request
int downloadCount = 0;          // number of facts to download since last server request

#define sdPin 53
#define HOST_NAME   "triviotoy.azurewebsites.net"
#define HOST_PORT   (80)

EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 15000;      // threshold for difference in acceleration

ESP8266 wifi(Serial1);
String ssid = "Christine";
String pass = "0123456789";
String userID;

// led
#define redPin 3
#define greenPin 4
#define bluePin 5

// buttons and volume control
#define volumeDownButton 6
#define volumeUpButton 8
#define wifiButton 7
long debounceDelay = 10;    // debounce time
short int volume = 10;

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

bool enableAcceleration = true;

int lastSetting = 10;

Battery battery(3700, 4300, A0);
VoltageReference vRef;

int EEPROMaddress = 0;

void setup() {
  Serial.begin(9600);
  hardwareSetup();
  setColor(5);

  setupSD();
  // initialize emic devices
  Serial.println(F("Intializing emic device..."));
  emic.begin(rxpin, txpin, sdPin);
  emic.setVoice(0);   // sets the voice, there are 9 types, 0 - 8
  emic.setVolume(18); // sets the volume
  emic.setRate(150);

  // initialize accelerometer
  Serial.println(F("Initializing I2C devices..."));
  accelgyro.initialize();
  accelerometerSetup();     // verify connection

  getEEPROMaddress();

  Serial1.begin(9600);  // initialize serial for ESP module
  setupWifiConnection();
  //connectToNetwork(ssid, pass);   // attempt to connect to WiFi network
//  if(!isConnected()) {
//    connectStoredWifi();
//  }

  getFactStorageIndex(); // get filename of last stored fact
  serverCount = getServerPlayCount(); // get number of facts played offline
  downloadCount = getDownloadCount(); // gets number of facts to download

  getUserId();  // get user id stored

  batteryVoltageSetup();

  setLedWifiStatus(true);
}

void setupSD() {
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(sdPin, OUTPUT);
  
  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(10)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
}

void hardwareSetup() {
  // set LED outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(volumeDownButton, INPUT);
  pinMode(volumeUpButton, INPUT);
  pinMode(wifiButton, INPUT);
}

void accelerometerSetup() {
  Serial.println(F("Testing device connections..."));
  bool accelConnection = accelgyro.testConnection();
  if (!accelConnection) {
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

void batteryVoltageSetup() {
  vRef.begin();
  int vcc = vRef.readVcc();
  battery.begin(vcc, 1); 
}

void setupWifiConnection() {
  Serial.println(F("Setting up wifi connect."));

  if (wifi.setOprToStation()) {
    Serial.print(F("to station ok\r\n"));
  } else {
    Serial.print(F("to station err\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

  Serial.print(F("setup end\r\n"));
}

void connectToNetwork(String wifiName, String password) {
  Serial.print(F("Connecting to: "));
  Serial.println(wifiName);

  if (wifi.setOprToStation()) {
    Serial.print(F("to station ok\r\n"));
  } else {
    Serial.print(F("to station err\r\n"));
  }

  if (wifi.joinAP(wifiName, password)) {
    Serial.print(F("Join AP success\r\n"));
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

  Serial.print(F("setup end\r\n"));
}

bool isConnected() {
  String ipStatus = wifi.getIPStatus();
  checkButtons();
  int index = ipStatus.indexOf('\n');
  if(index != ipStatus.length() - 1) {
    ipStatus = ipStatus.substring(0, index);
  }
  Serial.print("IP status: ");
  Serial.println(ipStatus);
  ipStatus.trim();
  if (ipStatus == "STATUS:2" || ipStatus == "STATUS:3") {
    return true;
  } else {
    return false;
  }
}

bool isBatteryLow() {
  int volt = battery.voltage();
  int percent = battery.level();
  checkButtons();
  if(percent <= 15) {
    return true;
  }
  else {
    return false;
  }
}

void setLedWifiStatus(bool checkBattery) {
  // Low battery takes priority over wifi
  if(checkBattery) {
    if(isBatteryLow()) {
      setColor(0);
      return;
    }
  }
  checkButtons();
  // wifi connection status
  if (isConnected()) {
    setColor(1);
  } else {
    setColor(2);
  }
}

/*
   Call this function to get number to return to server for how many facts has been played
*/
// get number of facts played since last server pull
int getServerPlayCount() {
  char serverCountFile[] = "s.txt";
  String numString = "";
  int number = serverCount;
  if (SD.exists(serverCountFile)) {
    File file = SD.open(serverCountFile);
    while (file.available()) {
      char numChar = file.read();
      if (numChar != '\n') {
        numString += numChar;
      }
    }
    number = numString.toInt();
    file.close();
  } else {
    // create file
    File file = SD.open(serverCountFile, FILE_WRITE);
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
  if (SD.exists(serverCountFile)) {
    SD.remove(serverCountFile);
  }
  // write file
  File file = SD.open(serverCountFile, FILE_WRITE);
  file.print(numString);
  file.close();

  Serial.print(F("Updated # of facts since last server pull: "));
  Serial.println(numString);

  delay(500);
}

// get number of facts to download once connected to wifi again
int getDownloadCount() {
  char downloadCountFile[] = "d.txt";
  String numString = "";
  int number = downloadCount;
  if (SD.exists(downloadCountFile)) {
    File file = SD.open(downloadCountFile);
    while (file.available()) {
      char numChar = file.read();
      if (numChar != '\n') {
        numString += numChar;
      }
    }
    number = numString.toInt();
    file.close();
  } else {
    // create file
    File file = SD.open(downloadCountFile, FILE_WRITE);
    numString = String(downloadCount);
    file.print(numString);
    file.close();
  }
  delay(500);
  return number;
}

// updates number of facts to download once connected to wifi again
void updateDownloadCount() {
  char downloadCountFile[] = "d.txt";

  String numString = String(downloadCount);
  if (SD.exists(downloadCountFile)) {
    SD.remove(downloadCountFile);
  }
  // write file
  File file = SD.open(downloadCountFile, FILE_WRITE);
  file.print(numString);
  file.close();

  Serial.print(F("# of Facts left to download: "));
  Serial.println(numString);

  delay(500);
}

// gets name of file for fact to be played and stores in global variable
String getPlayFilename() {
  Serial.print(F("Getting file to be played: "));
  String factFilename = "0";
  char playFilename[] = "p.txt";
  if (SD.exists(playFilename)) {
    // read/write from file
    File file = SD.open(playFilename);
    if (file) {
      factFilename = "";
      while (file.available()) {
        char ltr = file.read();
        if (ltr != '\n') {
          factFilename = factFilename + ltr;
        }
      }
      Serial.println(factFilename);
      file.close();
    }
  } else {
    // create file
    File file = SD.open(playFilename, FILE_WRITE);
    if (file) {
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
  if (SD.exists(playFilename)) {
    if (!SD.exists(numberName)) {
      factFilename = "0";
    }
    SD.remove(playFilename);
    // read/write from file
    File file = SD.open(playFilename, FILE_WRITE);
    file.println(factFilename);
    file.close();
    Serial.print(F("Updating fact Index: "));
    Serial.println(number);
  } else {
    // create file
    File file = SD.open(playFilename, FILE_WRITE);
    file.println("0");
    file.close();
  }
  delay(500);
}

/**
   Call this function when storing fact from server
*/
void storeFact(String factString) {
  File file;
  String filename = String(writeFileNumber);
  filename = filename + ".txt";
  // create/open file and store fact
  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  
  Serial.print(F("Storing Fact in file: "));
  Serial.println(filename);
  file = SD.open(filename, FILE_WRITE);
  if (file) {
    Serial.println(factString);
    file.println(factString);
    file.close();
  }

  // increment name of file to store next fact as
  updateFactStorageIndex();
  // decrement number of facts to retrieve
  if (downloadCount > 0) {
    downloadCount = downloadCount - 1;
    updateDownloadCount();
  }
  delay(500);
  setLedWifiStatus(true);
}

// gets index of last stored fact
void getFactStorageIndex() {
  char writeFilename[] = "w.txt";
  File file;
  String number = "0";
  // get filename to store fact as
  if (SD.exists(writeFilename)) {
    number = "";
    // read/write from file
    file = SD.open(writeFilename);
    while (file.available()) {
      char num = file.read();
      if (num != '\n') {
        number = number + String(num);
      }
    }
    file.close();
  } else {
    // create file
    File file = SD.open(writeFilename, FILE_WRITE);
    file.print(number);
    file.close();
  }
  // sets it as global variable
  writeFileNumber = number.toInt();
  delay(500);
}

// updates saving index of last stored fact
void updateFactStorageIndex() {
  Serial.println(F("Update Fact Storage Index: "));
  char writeFilename[] = "w.txt";
  File file;
  if (SD.exists(writeFilename)) {
    SD.remove(writeFilename);
  }

  // update fact index of last stored
  file = SD.open(writeFilename, FILE_WRITE);
  // if max reached, go back to 0
  if (writeFileNumber > MAXFILENUM) {
    writeFileNumber = 0;
  } else {
    writeFileNumber++;
  }
  file.print(String(writeFileNumber));
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
  emic.speak(filename, 10);
  int num = factFile.toInt();
  // increment and store fact index to be played next time
  updatePlayFileName(factFile);
}

/**
   Call this function when throw is detected
*/
void getFact() {
  enableAcceleration = false;
  String factFile = getPlayFilename();
  factFile.trim();
  delay(100);
  playFact(factFile);
}

// Checks difference in acceleration for throw
bool checkAcceleration() {
  accelgyro.getAcceleration(&ax, &ay, &az);
  x_diff = abs(ax - x_prev);
  y_diff = abs(ay - y_prev);
  z_diff = abs(az - z_prev);
  if ( x_diff > threshold || y_diff > threshold || z_diff > threshold ) {
    Serial.println(F("accel detected"));
    getFact(); // Read fact from EEPROM and plays it
    bool serverSuccess = false;
    if (isConnected()) {
      if(getFactFromServer()) { // Pull new fact in from server
        serverSuccess = store1CountOnServer();
      }
    } 
    
    if(!serverSuccess) {
      serverCount++;
      downloadCount++;
      updateServerPlayCount();
      updateDownloadCount();
    }
    
    sampleAcceleration(50); // set delay long enough to  for fact to be played
    enableAcceleration = true;
    return true;
  } else {
    checkButtons();
    x_prev = ax;
    y_prev = ay;
    z_prev = az;
    delay(500); // delay in acceleration sampling rate
    return false;
  }
}

void sampleAcceleration(int samples) {
  for (int i = 0; i < samples; i++) {
    checkButtons();
    accelgyro.getAcceleration(&x_prev, &y_prev, &z_prev);
    delay(100);
  }
}


/**
   REPLACE WITH NEW REQUEST CODE
*/
bool getFactFromServer() {
  setColor(3);
  uint8_t buffer[1024] = {0};
  bool isFact = false;

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print(F("create server connection ok\r\n"));
  } else {
    Serial.print(F("create server connection err\r\n"));
    return false;
  }
  
  String fact = "";

  String tempId = "2";

  char getRequest[100];
  sprintf(getRequest, "GET /Trivia/%i  HTTP/1.1\r\nHost: triviotoy.azurewebsites.net\r\nConnection: keep-alive\r\n\r\n", tempId.toInt());
  wifi.send((const uint8_t*)getRequest, strlen(getRequest));
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0) {
    for (uint32_t i = 0; i < len; i++) {
      //Serial.print((char)buffer[i]);
      if ((char)buffer[i] == 34) {
        isFact = !isFact;
      }

      if (isFact && (char)buffer[i] != 34) {
        fact = fact + (char)buffer[i];
      }
    }
  }

  if(fact != "") {
    Serial.println(fact);
    storeFact(fact);

    return true;
  }

  return false;
}

// updates server with one fact played
bool store1CountOnServer() {
  setColor(3);
  uint8_t buffer[1024] = {0};
  bool isFact = false;

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print(F("create get count request  ok\r\n"));

    String tempId = "2";
    char putRequest[100];
    sprintf(putRequest, "GET /User/History/%i/1 HTTP/1.1\r\nHost: triviotoy.azurewebsites.net\r\nConnection: keep-alive\r\n\r\n", tempId.toInt());
    wifi.send((const uint8_t*)putRequest, strlen(putRequest));
    return true;
  } else {
    Serial.print(F("create get count err\r\n"));
    return false;
  }
}

// Updates server with how many facts played since last connection
bool storeOfflineCountOnServer() {
  setColor(3);
  uint8_t buffer[1024] = {0};
  bool isFact = false;

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print(F("create get count request  ok\r\n"));

    String tempId = "2";
    char putRequest[100];
    sprintf(putRequest, "GET /User/History/%i/%i HTTP/1.1\r\nHost: triviotoy.azurewebsites.net\r\nConnection: keep-alive\r\n\r\n", tempId.toInt(), serverCount);
    wifi.send((const uint8_t*)putRequest, strlen(putRequest));

    serverCount = 0;
    updateServerPlayCount();
    return true;
  } else {
    Serial.print(F("create get count err\r\n"));
    return false;
  }
}

// Write to analog outputs
void setColor(int setting) {
  if (setting == lastSetting) {
    return;
  }

  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;

  switch (setting) {
    case 0: { // red - low battery
        red = 255;
        green = 0;
        blue = 0;
        Serial.println(F("red - low battery"));
        break;
      }
    case 1: { // green - connected to wifi
        red = 0;
        green = 255;
        blue = 0;
        Serial.println(F("green - connected to wifi"));
        break;
      }
    case 2: { // purple - not connected to wifi
        red = 128;
        green = 0;
        blue = 128;
        Serial.println(("purple - not connected to wifi"));
        break;
      }
    case 3: { // blue - updating from server
        red = 0;
        green = 0;
        blue = 255;
        Serial.println(F("blue - updating from server"));
        break;
      }
    case 4: { // white - wifi setup mode
        red = 255;
        green = 255;
        blue = 255;
        Serial.println(F("white - wifi setup mode"));
        break;
      }
    case 5: { // orange - turning on
      red = 255;
      green = 70;
      blue = 0;
      Serial.println(F("orange - turning on"));
      break;
    }
    case 6: { // pink - attempting to connect
      red = 255;
      green = 10;
      blue = 10;
      Serial.println(F("pink - attempting to connect"));
      break;
    }
    default: { // off
      red = 0;
      green = 0;
      blue = 0;
      Serial.println("off");
      break;
    }
  }

  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);

  lastSetting = setting;
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
        if (volume > 0) {
          volume = volume - 1;;
        }
        emic -= 10;
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
        if (volume < 10) {
          volume++;
        }
        emic += 10;
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
      if (wifiButtonState == 1) {
        wifiSetupMode = true;

        if (wifiSetupMode) {
          Serial.println(F("Wifi setup mode"));
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

void tcpMode() {
  espSetup();
  //Attempt tcp connection
  bool tcp_created = create_TCP_connection();

  if (tcp_created) {
    //function to carry out the connection to the wifi
    connect_to_wifi();
  }

  // end of tcp mode?
  wifiSetupMode = false;
  setLedWifiStatus(false);
}

void espSetup() {
  //sets the baud rate between the esp and arduino
  Serial.println(F("Starting wifi setup"));
  //bool baud = wifi.autoSetBaud();
  Serial.print(F("Baud set: "));
  //Serial.println(baud);

  //setting the mode (mode 3) of the esp to operate in both AP and Station mode
  bool mode = wifi.setOprToStationSoftAP();
  Serial.print(F("Mode: "));
  Serial.println(mode);

  // If mode is correctly set, create the esp's wifi network
  if (mode) {
    wifi.setSoftAPParam("Trivio", "1234", 3, 0);
    Serial.print(F("Set AP: "));
    Serial.println(F("complete"));
  }

  //Making the esp in single mode so that there is only one connection at a time
  bool mux_disabled = wifi.disableMUX();
}

bool disconnect_wifi() {
  //Disconnect from the wifi for now as I already connected it to our wifi once
  //so it always automatically connects, but we dont want that
  bool disconnected = wifi.leaveAP();
  Serial.print(F("Disconnected: "));
  Serial.println(disconnected);
  return disconnected;
}

void restartTCPConnection() {
  Serial.print(F("Restarting tcp connection: "));
  bool cl = wifi.releaseTCP();
  Serial.println(cl);
  create_TCP_connection();
}

bool create_TCP_connection() {
  //creating a TCP connection to the phone
  Serial.println(F("Trying to create a TCP connection"));
  bool tcpCreated = create_TCP();
  while (tcpCreated == false) {
    Serial.println(F("Couldn't create"));
    tcpCreated = create_TCP();
  }

  Serial.println(F("TCP connection initiated"));
  String ack = receive_data(2000);

  Serial.print(F("Data received: "));
  Serial.println(ack);

  if (ack.length() <= 0) {
    Serial.println(F("waiting for ack"));
    restartTCPConnection();
    return false;
  }

  if (ack == "ok") {
    Serial.println(F("Ack received"));
    //sending ack back
    bool sent = send_data("ok");
    while (sent == false) {
      Serial.println(F("Trying to send ack"));
      sent = send_data("ok");
    }
    Serial.println(F("Ack Sent"));
    return sent;
  }
  else {
    restartTCPConnection();
    return false;
  }
}

void connect_to_wifi() {
  bool send_confirmation;
  bool send_failure;
  //Waiting for wifi credentials
  Serial.println(F("Credentials"));
  String cred = receive_data(120000);

  if(cred.length() == 0) {
    Serial.print(F("Turning AP mode off: "));
    Serial.println(wifi.setOprToStation());
    bool closed = wifi.releaseTCP();
    Serial.println(closed);
    return;
  }
  
  Serial.println(F("Credentials received"));

  //separating the credentials
  bool break_cred = break_credentials(cred);
  Serial.print(F("Cred separated: "));
  Serial.println(break_cred);

  //Trying to connect to the wifi
  Serial.println(F("Connecting to the wifi..."));
  
  setColor(6);
  Serial.print(F("Turning AP mode off: "));
  Serial.println(wifi.setOprToStation());
  bool closed = wifi.releaseTCP();
  Serial.println(closed);

  connectToNetwork(ssid, pass);
  setLedWifiStatus(false);
  if(isConnected()) {
    storeWifi(ssid, pass);
  }
}


//breaking the credentials received from the phone in the format "ssid:pass"
bool break_credentials(String data) {
  if (data.length() > 0) {
    int colon_index = data.indexOf(':');
    String userid = data.substring(0, colon_index);
    String rest = data.substring(colon_index + 1);
    int second_colon = rest.indexOf(':');
    ssid = rest.substring(0, second_colon);
    pass = rest.substring(second_colon + 1);
    Serial.print(F("userid: "));
    Serial.println(userid);
    Serial.print(F("ssid: "));
    Serial.println(ssid);
    Serial.print(F("pass: "));
    Serial.println(pass);
    setUserId(userid);
    return true;
  }
  return false;
}

//Breaking the IP of device connected as it is given as "ip,mac-address"
String break_ip(String ip) {
  String actual_ip = "";
  if (ip.length() > 0) {
    int comma_index = ip.indexOf(',');
    actual_ip = ip.substring(0, comma_index);
    Serial.println(actual_ip);
    return actual_ip;
  }
  return "";
}

//create a tcp connection with the phone
bool create_TCP() {
  bool tcp_created;
  String ip_device = wifi.getJoinedDeviceIP();
  ip_device = break_ip(ip_device);
  Serial.print("Ip: ");
  Serial.println(ip_device);
  if (ip_device.length() > 0) {
    tcp_created = wifi.createTCP(ip_device, 8080);
  }
  return tcp_created;
}

//Used to send data to the phone and if succesful will return true
bool send_data(String data) {
  int len = data.length();
  byte *buff = new byte[len];
  for (int i = 0; i < len; i++) {
    buff[i] = data[i];
  }
  bool sent = wifi.send(buff, len);
  Serial.print(F("Data sending: "));
  Serial.println(data);
  for (int i = 0; i < len; i++) {
    Serial.println(buff[i]);
  }
  return sent;
}

//Used to receive data from the phone
String receive_data(uint16_t timeout) {
  uint8_t buff[128] = {0};
  uint32_t len = wifi.recv(buff, sizeof(buff), timeout);
  String data = "";

  if (len > 0) {
    Serial.println(F("Incoming: "));
    for (int i = 0; i < len; i++) {
      data += char(buff[i]);
    }
    data[len] = '\0';
    Serial.print(data);
  }
  return data;
}

// stores user id
void setUserId(String id) {
  char filename[] = "u.txt";
  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  File file = SD.open(filename, FILE_WRITE);
  file.print(id);
  file.close();

  delay(500);
}

// gets user id
void getUserId() {
  char filename[] = "u.txt";
  File file;
  String number = userID;
  // get filename to read userID
  if (SD.exists(filename)) {
    number = "";
    file = SD.open(filename);
    while (file.available()) { // read from file
      char num = file.read();
      if (num != '\n') {
        number = number + num;
      }
    }
    number.trim();
    file.close();
  } else {
    // create file
    file = SD.open(filename, FILE_WRITE);
    file.println(number);
    file.close();
  }
  Serial.print(F("User ID: "));
  Serial.println(number);
  userID = number; // update global variable
  delay(500);
}

void storeWifi(String wifiName, String password) {
  char filename[] = "wifi.txt";
  String credential = wifiName;
  credential = credential + ":";
  credential = credential + String(EEPROMaddress);

  File file;
  file = SD.open(filename, FILE_WRITE);
  file.println(credential);
  file.close();
  storePassword(password);
}

String getWifiName(String credential) {
  int colon_index = credential.indexOf(':');
  String wifiName = credential.substring(0,colon_index);
  return wifiName;
}
int getPasswordAddress(String credential) {
  int colon_index = credential.indexOf(':');
  String address = credential.substring(colon_index + 1);
  address.trim();
  int passwordAddress = address.toInt();
  return passwordAddress;
}

void connectStoredWifi() {
  char filename[] = "wifi.txt";
  String credential = "";
  String password = "";

  File file;
  // get filename to read userID
  if (SD.exists(filename)) {
    file = SD.open(filename);
    while (file.available()) { 
      // read from file
      char letter = file.read();
      if (letter != '\n') {
        credential = credential + letter;
      } else {
        int passwordAddress = getPasswordAddress(credential);
        credential = getWifiName(credential);
        password = getPassword(passwordAddress);
        connectToNetwork(credential, password);
        if(isConnected()) {
          break;
        }
        credential = "";
      }
    }
    file.close();
  }
}

void storePassword(String password) {
  for(int i = 0; i < pass.length(); i++) {
    EEPROM.write(EEPROMaddress, password[i]);
    EEPROMaddress++;
  }
  EEPROM.write(EEPROMaddress,"\0");
  EEPROMaddress++;

  storeAddress();
}

String getPassword(int address) {
  String password = "";
  char letter = EEPROM.read(address);
  while(letter != '\0') {
    letter = EEPROM.read(address);
    password = password + letter;
  }

  return password;
}

void getEEPROMaddress() {
  char filename[] = "eeprom.txt";
  String address = "";
  
  File file;
  // get filename to read userID
  if (SD.exists(filename)) {
    file = SD.open(filename);
    while (file.available()) { 
      // read from file
      char num = file.read();
      if (num != '\n') {
        address = address + num;
      } 
    }
    file.close();
  } else {
    file = SD.open(filename, FILE_WRITE);
    address = "0";
    file.print(address);
    file.close();
  }
  delay(100);

  EEPROMaddress = address.toInt();
}

// stores next address for password
void storeAddress() {
  char filename[] = "eeprom.txt";
  File file;
  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  file = SD.open(filename, FILE_WRITE);
  String address = String(EEPROMaddress);
  file.print(address);
  file.close();
  delay(100);
}

bool checkBattery = true;
int batteryCount = 0;

void loop() {
  checkButtons();
  
  if (enableAcceleration) {
    checkAcceleration();
    if(batteryCount == 4) {
      batteryCount = 0;
      checkBattery = !checkBattery;
    } else {
      batteryCount++;
    }
    setLedWifiStatus(checkBattery);
  }

  // get http request for updating fact history
  if (serverCount > 0 && isConnected()) {
    storeOfflineCountOnServer();
  }
  // get http request to download new facts
  if (downloadCount > 0 && isConnected()) {
    getFactFromServer();
  }
}
