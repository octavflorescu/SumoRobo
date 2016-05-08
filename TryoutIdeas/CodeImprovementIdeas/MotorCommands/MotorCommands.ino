////////////////////////////////////////////////////////////////////////////////
/////////////////////////TEST MOTOR COMMANDS////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "Utils.h"
#include "Config.h"
#include "MotorCommands.h"

#define startModule 11//pin number for start module

int powStg, powDrt;
long timer;

void setup() {
  Serial.begin(57600);
  powStg = 0;
  powDrt = 0;
  // READ FROM SENSORS
  DDRB = 0b00000000;

//  // START MODULE
//  pinAsInput(startModule);
//  delay(100);

//  while (isLow(startModule)) {
    writeMotors(0, 0);
//  }
}

void loop() {
  delay(60);
  timer = micros();
  
  if (timer > 500000 && timer < 1000000) {
    controlledWriteMotors(70, 100);
  } else {
    writeMotors(80, -90);
  }
}

