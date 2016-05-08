#include "Utils.h"
#include "Config.h"

#define startModule 11//pin number for start module

long timer;
bool start;
bool rotating;
int pas;
int lastPas;
byte b;
int powStg, powDrt;
long asdftimer;

#define BUFFER_LENGTH 9

typedef struct 
{
  uint8_t packetStart[3];
  int16_t pMotorR_sds16;
  int16_t pMotorL_sds16;
  int16_t yCheck_sds16;
} motorCommand;

typedef union
{
  motorCommand sMotorCommand;
  uint8_t data[BUFFER_LENGTH];
} rawPacket;

#define WriteMotors(m1, m2) \
  ({ \
    rawPacket sRawPacket_mds = { '#', '#', '#', 0, 0, 0 }; \
    sRawPacket_mds.sMotorCommand.pMotorR_sds16 = m1; \
    sRawPacket_mds.sMotorCommand.pMotorL_sds16 = -m2; \
    Serial1.write(sRawPacket_mds.data, BUFFER_LENGTH); \
  })

void setup() {
  // SETUP
  start = true;
  rotating = false;
  pas = 0;
  lastPas = 0;
  Serial1.begin(57600);
  powStg = 0;
  powDrt = 0;
  // READ FROM SENSORS
  DDRB = 0b00000000;

  // START MODULE
  bool startModuleVal = false;
  pinAsInput(startModule);
  WriteMotors(0, 0);
  asdftimer = millis();
  delay(100);
  while (!startModuleVal) {
    startModuleVal = digitalState(startModule) == 1 ? true : false;
  // DO NOT START UNTIL START/STOP MODULE: STARTED
  // ALL OTHER INIT CODE APART FROM TIMER SHOULD BE ABOVE THIS
  if (millis() > (asdftimer + 5000)) {
    break;
  }
    WriteMotors(0, 0);
  }
  
  forwardFullSpeed();
  rotate();
//  forwardFullSpeed();
//  rotate();
  asdftimer = millis();
}

void loop() {
  // START PASSED, GO TO SENSOR CHECK
  b = PINB & 0b01111111;

if (millis() > (asdftimer + 2000)) {
  WriteMotors(0, 0);
  return;
}

  if(b == 0) {
      WriteMotors(powStg, powDrt);
      
  } else if (b & 0b00011100) {
    if (b & 0b00011100 == 0b00011100){
      WriteMotors(power, power); // BLANA FATA 100 100
      powStg = power;
      powDrt = power;
      
    } else if(b & 0b00011000 == 0b00011000){
      WriteMotors(power, power); // BLANA+STANGA 90 100
      powStg = power;
      powDrt = power;
      
    } else if(b & 0b00001100 == 0b00001100){
      WriteMotors(power, power); // BLANA+DREAPTA 100 90
      powStg = power;
      powDrt = power;
      
    } else if (b & 0b00001000){  
      WriteMotors(power, power); // BLANA FATA 100 100
      powStg = power;
      powDrt = power;
      
    } else if (b & 0b00010000) {
      WriteMotors(power80P, power); // BLANA+STANGA mai mult 80 100
      powStg = power80P;
      powDrt = power;
      
    } else {
      WriteMotors(power, power80P); // BLANA+DREAPTA mai mult 100 80
      powStg = power;
      powDrt = power80P;
    }
  } else if(b & 0b01100000){
        
    if (b & 0b00100000) {
      WriteMotors(-power25P, power20P);
      powStg = -power25P;
      powDrt = power20P;
      
    } else {
      WriteMotors(-power30P, power15P);
      powStg = -power30P;
      powDrt = power15P;
    }
  } else {
    
    if (b & 0b00000010) {
      WriteMotors(power20P, -power25P);
      powStg = power20P;
      powDrt = -power25P;
      
    } else {
      WriteMotors(power15P, -power30P);
      powStg = power15P;
      powDrt = -power30P;
    }
  }
}

void forwardFullSpeed() {
  timer = millis();
  while (millis() - timer < 225) { // Pt roboch //50) { //Pentru 255
//    if (PINB & 0b01111111 != 0) {
//      return;
//    }
    WriteMotors(power, power);
  } 
}

void rotate() {
  timer = millis();
  int counter = 1;
  int offset = 0;
  while(millis() - timer < 175) {
    counter++;
    if (counter % 12 == 0)
    {
      offset ++;
    }
    if(PINB & 0b01111111 != 0) {
      WriteMotors(0, 0);
      return;
    }
    WriteMotors(power35P - offset, -power50P + offset);
  }
  WriteMotors(0, 0);
}

