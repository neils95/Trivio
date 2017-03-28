#include "ESP8266.h"

SoftwareSerial mySerial(9, 8);
ESP8266 wifi(mySerial);
String ssid;
String pass;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Begin!");

  //sets the baud rate between the esp and arduino
  Serial.println("Starting wifi setup");
  bool baud = wifi.autoSetBaud();
  Serial.print("Baud set: ");
  Serial.println(baud);

  //setting the mode (mode 3) of the esp to operate in both AP and Station mode
  bool mode = wifi.setOprToStationSoftAP();

  //If mode is correctly set, create the esp's wifi network
  if(mode) {
    wifi.setSoftAPParam("myESP", "1234", 3, 0);
    Serial.print("Set AP: ");
    Serial.println("complete");
  }

  //Making the esp in single mode so that there is only one connection at a time
  bool mux_disabled = wifi.disableMUX();

  //Attempt tcp connection
  bool tcp_created = create_TCP_connection();

  if(tcp_created) {
    //function to carry out the connection to the wifi
    connect_to_wifi();
  }  
}

bool disconnect_wifi() {
  //Disconnect from the wifi for now as I already connected it to our wifi once
  //so it always automatically connects, but we dont want that
  bool disconnected = wifi.leaveAP();
  Serial.print("Disconnected: ");
  Serial.println(disconnected);
  return disconnected;
}

bool create_TCP_connection() {
  //creating a TCP connection to the phone
  Serial.println("Trying to create a TCP connection");
  while(create_TCP() == false) {
    Serial.println("Couldn't create");
    create_TCP();
  }
  Serial.println("TCP connection initiated");
  String ack = receive_data();
  while(ack.length() < 0) {
    Serial.println("waiting for ack");
    ack = receive_data();
  }
  
  if(ack == "ok") {
    Serial.println("Ack received");
    //sending ack back
    bool sent = send_data("ok");
    while(sent == false) {
      Serial.println("Trying to send ack");
      sent = send_data("ok");
    }
    Serial.println("Ack Sent");
    return sent;
  }
}

void connect_to_wifi() {
    bool send_confirmation;
    bool send_failure;  
    //Waiting for wifi credentials
    Serial.println("Credentials");
    String cred = receive_data();
    while(cred.length() < 0) {
      Serial.println("Waiting for creds");
      cred = receive_data();
    }
    Serial.println("Credentials received");

    //separating the credentials
    bool break_cred = break_credentials(cred);
    Serial.print("Cred separated: ");
    Serial.println(break_cred);
  
    //Trying to connect to the wifi
    Serial.println("Connecting to the wifi...");
    bool connect_wifi = wifi.joinAP(ssid, pass);
    if(connect_wifi == true) {
      Serial.println("Success!!!");
      send_confirmation = send_data("yes");
      while(send_confirmation == false) {
        Serial.println("Trying to send success");
        send_confirmation = send_data("yes");
      }
    }
    else {
      Serial.println("Failed :(");
      send_failure = send_data("no");
      while(send_confirmation == false) {
        Serial.println("Trying to send success");
        send_confirmation = send_data("no");
      }
    }
    bool closed = wifi.releaseTCP();
    Serial.println(closed);
  }


//breaking the credentials received from the phone in the format "ssid:pass"
bool break_credentials(String data) {
  if(data.length() > 0) {
    int colon_index = data.indexOf(':');
    ssid = data.substring(0, colon_index);
    pass = data.substring(colon_index + 1);
    Serial.print("ssid: ");
    Serial.println(ssid);
    Serial.print("pass: ");
    Serial.println(pass);
    return true;
  }
  return false;
}

//Breaking the IP of device connected as it is given as "ip,mac-address" 
String break_ip(String ip) {
  String actual_ip;
  if(ip.length() > 0) {
      int comma_index = ip.indexOf(',');
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
  Serial.print("Data sending: ");
  Serial.println(data);
  for(int i = 0; i < len; i++) {
    Serial.println(buff[i]);
  }
  return sent;
}

//Used to receive data from the phone
String receive_data() {
  uint8_t buff[128] = {0};
  uint32_t len = wifi.recv(buff, sizeof(buff), 120000);
  String data = "";
  
  if(len > 0) {
    Serial.println("Incoming: ");
    for(int i = 0; i < len; i++) {
      data += char(buff[i]);
    }
    data[len] = '\0';
    Serial.print(data);
    return data;
  }
}

void loop() {
}

