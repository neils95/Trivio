#include <Wire.h>
#include "mma8653.h"
#include <SoftwareSerial.h>

MMA8653 accel;
const int i2c_scl = 14;
const int i2c_sda = 12;
float x_prev, y_prev, z_prev; // previous acceleration values
float x, y, z;                // current acceleration values
float x_diff, y_diff, z_diff; // difference in accelerations from last sampled time
int threshold = 20;           // threshold for difference in acceleration

void setup() {
  Serial.begin(460800);
  Wire.begin(i2c_sda, i2c_scl);
  accel.setup();
  accel.getXYZ(x_prev, y_prev, z_prev);
}

void checkAcceleration() {
  accel.getXYZ(x, y, z);
  x_diff = abs(x - x_prev);
  y_diff = abs(y - y_prev);
  z_diff = abs(z - z_prev);

  /*
  // Print acceleration differences for testing purposes
  Serial.print("x diff: ");
  Serial.println(x_diff);
  Serial.print("y diff: ");
  Serial.println(y_diff);
  Serial.print("z diff: ");
  Serial.println(z_diff);*/

  if ( x_diff > threshold || y_diff > threshold || z_diff > threshold ) {
    Serial.println("Throw or shake detected");
    Serial.println("Stating fact...");
    delay(5000); // set delay long enough to  for fact to be played
  } else {
    x_prev = x;
    y_prev = y;
    z_prev = z;
    delay(200); // delay in acceleration sampling rate
  }
}

void loop() {
  checkAcceleration();
}
