#include "Utils.h"
#include "Config.h"

#define startModule 12
#define kHaltTime 10000

int const whiteSensorStgF = A0;
int const whiteSensorDrtF = A1;
int const whiteSensorStgS = A2;
int const whiteSensorDrtS = A3;

void setup() {
  Serial.begin(57600);
}

void loop() {
  Serial.print(analogRead(whiteSensorStgF));
  Serial.write(" ");
  Serial.print(analogRead(whiteSensorDrtF));
  Serial.write(" ");
  Serial.print(analogRead(whiteSensorStgS));
  Serial.write(" ");
  Serial.print(analogRead(whiteSensorDrtS));
  Serial.write("\n");
}
