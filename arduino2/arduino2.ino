#include <SoftwareSerial.h>
#include "WiFiEsp.h"
#include "ESP8266.h"
#include <Wire.h>

#define rxpin 3
#define txpin 4

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(9, 8); // RX, TX
#endif

int status = WL_IDLE_STATUS;     // the Wifi radio's status

bool readingFromServer = false;

ESP8266 wifi(Serial1);
String ssid;
String pass;

WiFiEspClient client;
// led
#define redPin 16
#define greenPin 15
#define bluePin 14

// buttons
#define wifiButton 7

int wifiButtonState;            // current reading of button
int lastWifiButtonState = LOW;  // the previous reading from the input pin
long lastWifiDebounceTime = 0;  // the last time button was toggled
bool wifiSetupMode = false;

int requestNum = 0;

long debounceDelay = 50;    // the debounce time, increase if output flickers

void setup() {
  Serial.begin(9600);
  Wire.begin(); // I2C bus for arduino to arduino communication
  Wire.onReceive(arduinoReceive);

  hardwareSetup();

  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);
  
  // attempt to connect to WiFi network
  connectToNetwork();

  espSetup();

  setLedWifiStatus();
}

void setLedWifiStatus() {
  if(status == WL_CONNECTED) {
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
  pinMode(wifiButton, INPUT);
  setColor(2);
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
    //storeFact(fact);
    readingFromServer = false;
    client.stop();
    arduinoSend(fact, true);
  } else {
    delay(500);
  }
}

void makeFactRequest() {
  if(status != WL_CONNECTED) {
    return;
  }
  setColor(3);
  if(factRequestSuccessful()){ //make request to server for another fact
    readingFromServer = true;
  }else{
    //TODO: Increment history counter on toy to send when connection finally made
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
      green = 0;
      blue = 0;
      break;
    }
    case 1: { // green - connected to wifi
      red = 0;
      green = 255;
      blue = 0;
      break;
    }
    case 2: { // purple - not connected to wifi
      red = 128;
      green = 0;
      blue = 128;
      break;
    }
    case 3: { // blue - updating from server
      red = 0;
      green = 0;
      blue = 255;
      break;
    }
    case 4: { // white - wifi setup mode
      red = 255;
      blue = 255;
      green = 255;
      break;
    }
    default: { // off
      red = 0;
      green = 0;
      blue = 0;
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
        wifiSetupMode = !wifiSetupMode;
        
        if(wifiSetupMode) {
          Serial.println(F("Wifi setup mode"));
          setColor(4);
          //TODO: put function to call wifi setup mode here
          arduinoSend("w",false);
          tcpMode();
        }
      }
    }
  }
  lastWifiButtonState = reading;
}

void checkButtons() {
  checkWifiButtonInput();
}

void tcpMode() {
  //Attempt tcp connection
  bool tcp_created = create_TCP_connection();

  if(tcp_created) {
    //function to carry out the connection to the wifi
    connect_to_wifi();
  }

  // end of tcp mode?
  setLedWifiStatus();
  arduinoSend("d", false);
}

bool create_TCP_connection() {
  String ok = "ok";
  //creating a TCP connection to the phone
  while(create_TCP() == false) {
    Serial.println(F("Couldn't create"));
    create_TCP();
  }
  String ack = receive_data();
  while(ack.length() < 0) {
    ack = receive_data();
  }
  
  if(ack == ok) {
//    Serial.println("Ack received");
    //sending ack back
    bool sent = send_data(ok);
    while(sent == false) {
//      Serial.println(F("Trying to send ack"));
      sent = send_data(ok);
    }
//    Serial.println(F("Ack Sent"));
    return sent;
  }
}

void connect_to_wifi() {
    bool send_confirmation;
    bool send_failure;  
    String yes = "yes";
    String no = "no";
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
      send_confirmation = send_data(yes);
      while(send_confirmation == false) {
        send_confirmation = send_data(yes);
      }
    }
    else {
      send_failure = send_data(no);
      while(send_failure == false) {
        send_failure = send_data(no);
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

/**
 * f<fact string>: send fact
 * w: wifi setup mode
 * d: done with wifi setup mode
 */
void arduinoSend(String sendStr, bool isFact) {
  if(isFact) {
    sendStr = "f" + sendStr;
    if(requestNum > 0) {
      requestNum = requestNum - 1;
    }
  }
  Wire.beginTransmission(8);
  Wire.write(sendStr.c_str());
  Wire.endTransmission();
  delay(500);
}

/**
 * m: make fact request
 */
void arduinoReceive(int howMany) {
  bool factRequest = false;
  String numRequests = "";
  while(1 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
    if(factRequests) {
      numRequests = numRequests + String(c);
    }
    if(c == 'm') {
      factRequest = true;
    }
  }
  
  delay(500);

  // make fact request if connected to wifi
  if(factRequest) {
    requestNum = numRequests.toInt();
    makeFactRequest();
  }
}

void loop() {
  //if there's incoming data over server connection, read in the fact
  if(!wifiSetupMode) {
    readInFact();
    if(requestNum > 0) {
      makeFactRequest();
    }
    checkButtons();
  }
}
