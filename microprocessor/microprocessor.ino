#include <SoftwareSerial.h>
#include <SD.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "EMIC2.h"
#include "WiFiEsp.h"
#include "ESP8266.h"

#define rxpin 10
#define txpin 11
int16_t startingAddress = 4;

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
//SoftwareSerial Serial1(9, 8); // RX, TX
#endif

int status = WL_IDLE_STATUS;    // the Wifi radio's status

const int MAXFILENUM = 2000;    // maximum number of files for facts
int writeFileNumber = 0;        // filename of next fact to be stored
int serverCount = 0;            // number of distinct facts played since last server request
bool readingFromServer = false;

#define sdPin 53
WiFiEspClient client;
EMIC2 emic;
MPU6050 accelgyro;
int16_t ax, ay, az;             // current acceleration values

int16_t x_prev, y_prev, z_prev; // previous acceleration values
int16_t x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int32_t threshold = 10000;      // threshold for difference in acceleration

ESP8266 wifi(Serial1);
String ssid;
String pass;
String userID = "3";

// led
#define redPin 3
#define greenPin 4
#define bluePin 5

// buttons and volume control
#define volumeDownButton 6
#define volumeUpButton 7
#define wifiButton 8
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

void setup() {
  Serial.begin(9600);
  hardwareSetup();

  //setupSD();
  // initialize emic devices
  Serial.println(F("Intializing emic device..."));
  emic.begin(rxpin, txpin, sdPin);
  emic.setVoice(8);   // sets the voice, there are 9 types, 0 - 8
  emic.setVolume(48); // sets the volume, 10 is max 
  
  // initialize accelerometer
  Serial.println(F("Initializing I2C devices..."));
  accelgyro.initialize();
  testConnection();     // verify connection
  
  Serial1.begin(9600);  // initialize serial for ESP module
  WiFi.init(&Serial1);  // initialize ESP module
  //connectToNetwork();   // attempt to connect to WiFi network

  getFactStorageIndex(); // get filename of last stored fact
  serverCount = getServerPlayCount();

  setLedWifiStatus();

  emic.speak("HELLLOOO");
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
  pinMode(sdPin, OUTPUT);
  
  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(10)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
}

void espSetup() {
  //sets the baud rate between the esp and arduino
  Serial.println(F("Starting wifi setup"));
  //bool baud = wifi.autoSetBaud();
  Serial.print(F("Baud set: "));
  //Serial.println(baud);

  //setting the mode (mode 3) of the esp to operate in both AP and Station mode
//  bool mode = wifi.setOprToStationSoftAP();
  Serial.print(F("Mode: "));
//  Serial.println(mode);

  int apMode = WiFi.beginAP("myESP", 3, "1234", ENC_TYPE_WPA2_PSK, false);
  Serial.println(apMode);

  //If mode is correctly set, create the esp's wifi network
//  if(mode) {
//    wifi.setSoftAPParam("myESP", "1234", 3, 0);
//    Serial.print(F("Set AP: "));
//    Serial.println(F("complete"));
//  }

  //Making the esp in single mode so that there is only one connection at a time
  bool mux_disabled = wifi.disableMUX(); 
}

void setLedWifiStatus() {
  if(WiFi.status() == WL_CONNECTED) {
    setColor(1);
   } else {
    setColor(2);
   }
}

void hardwareSetup() {
  // set LED outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(volumeDownButton, INPUT);
  pinMode(volumeUpButton, INPUT);
  pinMode(wifiButton, INPUT);
  
  setColor(5);
}

/**
 * Attempts to connect to WiFi if not already connnected
 * status global variable holds WiFi status 
 */
void connectToNetwork()
{
//  char ssid[] = "DanseyPhone";            // your network SSID (name)
//  char pass[] = "12345679";        // your network password
  char ssid[] = "Christine";            // your network SSID (name)
  char pass[] = "0123456789";
  if ( WiFi.status() != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
}

/*
 * Call this function to get number to return to server for how many facts has been played
 */
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

/**
 * Call this function when storing fact from server
 */
void storeFact(String factString) {
  File file;
  String filename = String(writeFileNumber);
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
  updateFactStorageIndex();
  // decrement number of facts to retrieve
  if(serverCount > 0) {
    serverCount = serverCount - 1;
    updateServerPlayCount();
  }
  delay(500);
  setLedWifiStatus();
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
  Serial.println(F("Update Fact Storage Index: "));
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
  emic.speak(filename,10);
  int num = factFile.toInt();
  // increment and store fact index to be played next time
  updatePlayFileName(factFile);
  makeFactRequest();
}

/**
 * Call this function when throw is detected
 */
void getFact() {
  enableAcceleration = false;
  String factFile = getPlayFilename();
  factFile.trim();
  delay(100);
  playFact(factFile);
}

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

  enableAcceleration = true;
 
  //if fact was pulled in, i will be greater than 1. 
  //Store end character and flip reading from server to false
  if(fact.length() > 20){
    Serial.println(fact);
    storeFact(fact);
    readingFromServer = false;
    client.stop();
  } else {
    delay(500);
  }
}

void makeFactRequest() {
  if(WiFi.status() != WL_CONNECTED) {
    if(serverCount < MAXFILENUM) {
      serverCount++;
      updateServerPlayCount();
    }
    Serial.println(F("Not connected to wifi"));
    enableAcceleration = false;
    return;
  }

  setColor(3);
  
  if(factRequestSuccessful()){ //make request to server for another fact
    readingFromServer = true;
  }else{
    //TODO: Increment history counter on toy to send when connection finally made
  }
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
    
    sampleAcceleration(50); // set delay long enough to  for fact to be played
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
    setLedWifiStatus();
    return false;
  }
}


//Write to analog outputs
void setColor(int setting) {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;

  switch(setting) {
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
        if(volume < 10) {
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
        
        if(wifiSetupMode) {
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

  if(tcp_created) {
    //function to carry out the connection to the wifi
    connect_to_wifi();
  }

  // end of tcp mode?
  wifiSetupMode = false;
  setLedWifiStatus();
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
  while(tcpCreated == false) {
    Serial.println(F("Couldn't create"));
    tcpCreated = create_TCP();
  }
  
  Serial.println(F("TCP connection initiated"));
  String ack = receive_data(2000);

  Serial.print(F("Data received: "));
  Serial.println(ack);

  if(ack.length() <= 0) {
    Serial.println(F("waiting for ack"));
    restartTCPConnection();
    return false;
  }
  
  if(ack == "ok") {
    Serial.println(F("Ack received"));
    //sending ack back
    bool sent = send_data("ok");
    while(sent == false) {
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
    Serial.println(F("Credentials received"));

    //separating the credentials
    bool break_cred = break_credentials(cred);
    Serial.print(F("Cred separated: "));
    Serial.println(break_cred);
  
    //Trying to connect to the wifi
    Serial.println(F("Connecting to the wifi..."));
    bool connect_wifi = wifi.joinAP(ssid, pass);
    if(connect_wifi == true) {
      Serial.println(F("Success!!!"));
      send_confirmation = send_data("yes");
      while(send_confirmation == false) {
        Serial.println(F("Trying to send success"));
        send_confirmation = send_data("yes");
      }
    }
    else {
      Serial.println(F("Failed :("));
      send_failure = send_data("no");
      while(send_failure == false) {
        Serial.println(F("Trying to send failure"));
        send_failure = send_data("no");
      }
    }
    bool closed = wifi.releaseTCP();
    Serial.println(closed);
  }


//breaking the credentials received from the phone in the format "ssid:pass"
bool break_credentials(String data) {
  if(data.length() > 0) {
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
    return true;
  }
  return false;
}

//Breaking the IP of device connected as it is given as "ip,mac-address" 
String break_ip(String ip) {
  String actual_ip = "";
  if(ip.length() > 0) {
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
  Serial.print(F("Data sending: "));
  Serial.println(data);
  for(int i = 0; i < len; i++) {
    Serial.println(buff[i]);
  }
  return sent;
}

//Used to receive data from the phone
String receive_data(uint16_t timeout) {
  uint8_t buff[128] = {0};
  uint32_t len = wifi.recv(buff, sizeof(buff), timeout);
  String data = "";

  if(len > 0) {
    Serial.println(F("Incoming: "));
    for(int i = 0; i < len; i++) {
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
  if(SD.exists(filename)) {
    SD.remove(filename);
  }
  File file = SD.open(filename,FILE_WRITE);
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
  if(SD.exists(filename)) {
    number = "";
    file = SD.open(filename);
    while(file.available()) { // read from file
      char num = file.read();
      if(num != '\n') {
        number = number + num;
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
  userID = number; // update global variable
  delay(500);
}

void loop() {
  //if there's incoming data over server connection, read in the fact
  readInFact();

  checkButtons();
  //setLedWifiStatus();

  if(enableAcceleration) {
    checkAcceleration();
  }
}
