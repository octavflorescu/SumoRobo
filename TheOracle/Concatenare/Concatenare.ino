#include "Declarations.h"

#include "saca.h"
#include "Oracle.h"
#include "rotate.h"
#include "triangle.h"
#include "sacaPassive.h"

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

void (*extraSetup)();
void (*looper)();
int switchModuleValue;
unsigned int putere;

void setup() {
  Serial1.begin(57600);
  
  whiteSensorStg = A1;
  whiteSensorDrt = A0;
  switchPin = A2;

  powStg = 0;
  powDrt = 0;
  
  DDRB = 0b00000000;
  pinAsInput(startModule);
  
  while (millis() < 2500) {
    WriteMotors(0, 0);
  }

  setupStrat();
  setupPowers(putere);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
  stateTimer = millis();
  extraSetup();
  WriteMotors(powStg, powDrt);
}

void loop() {
  looper();
  WriteMotors(powStg, powDrt);
//  Serial.print(powStg);
//    Serial.print(" ");
//    Serial.print(powDrt);
//    Serial.print("\n");
//    delay(10000);
}

void setupStrat() {
  //verifica input-ul de la switch pentru strategie si putere la motoare: vezi reference file = values.txt
  switchModuleValue = analogRead(switchPin);
  //Serial.println(switchModuleValue);
  switch (switchModuleValue) {
    case 1021://passive shit
    case 1022:
    case 1023:
      looper = &sacaPassiveLoop;
      extraSetup = &sacaPassiveExtraSetup;
      sacaPassiveSetup();
      //
      directionDrt = false;
      putere = 255;
    break;
    
    case 383: // 00001.10000
    case 384: // Oracle
    case 385:
      looper = &oracleLoop;
      extraSetup = &oracleExtraSetup;
      oracleSetup();
      //
      directionDrt = false;
      putere = 255;
    break;

    case 414:// 00001.01000
    case 415:
    case 416:
      looper = &oracleLoop;
      extraSetup = &oracleExtraSetup;
      oracleSetup();
      //
      directionDrt = switchModuleValue % 2 == 0 ? true : false;
      putere = 190;
    break;

    case 452:// 00001.00100
    case 453:
    case 454:
      looper = &oracleLoop;
      extraSetup = &oracleExtraSetup;
      oracleSetup();
      //
      directionDrt = true;
      putere = 255;
    break;
    
    case 327:// 00010.10000
    case 328: // saca
    case 329:
    case 330:
      //a doua strategie
      looper = &sacaLoop;
      extraSetup = &sacaExtraSetup;
      sacaSetup();
      //
      directionDrt = false;
      putere = 255;
      break;

    case 350:// 00010.01000
    case 351:
    case 352:
      //a doua strategie
      looper = &sacaLoop;
      extraSetup = &sacaExtraSetup;
      sacaSetup();
      //
      directionDrt = switchModuleValue % 2 == 0 ? true : false;
      putere = 190;
      break;

    case 376:// 00010.00100
    case 377:
    case 378:
      //a doua strategie
      looper = &sacaLoop;
      extraSetup = &sacaExtraSetup;
      sacaSetup();
      //
      directionDrt = true;
      putere = 255;
      break;

    case 246:// 00011.10000
    case 247://rotate
    case 248:
      //a treia strategie
      looper = &rotateLoop;
      extraSetup = &rotateExtraSetup;
      rotateSetup();
      //
      directionDrt = false;
      putere = 255;
      break;

    case 259:// 00011.01000
    case 260:// 
    case 261:
      //a treia strategie
      looper = &rotateLoop;
      extraSetup = &rotateExtraSetup;
      rotateSetup();
      //
      directionDrt = switchModuleValue % 2 == 0 ? true : false;
      putere = 190;
      break;

    case 274:// 00011.00100
    case 275:
    case 276:
      //a treia strategie
      looper = &rotateLoop;
      extraSetup = &rotateExtraSetup;
      rotateSetup();
      //
      directionDrt = true;
      putere = 255;
      break;

    case 268:// 00100.10000
    case 269: // triangle
    case 270:
      //a patra strategie
      looper = &triangleLoop;
      extraSetup = &triangleExtraSetup;
      triangleSetup();
      //
      putere = 255;
      break;

    case 283:// 00100.01000
    case 284:
    case 285:
      //a patra strategie
      looper = &triangleLoop;
      extraSetup = &triangleExtraSetup;
      triangleSetup();
      //
      directionDrt = switchModuleValue % 2 == 0 ? true : false;
      putere = 190;
      break;

    case 300:// 00100.00100
    case 301:
    case 302:
      //a patra strategie
      looper = &triangleLoop;
      extraSetup = &triangleExtraSetup;
      triangleSetup();
      //
      directionDrt = true;
      putere = 255;
      break;

//    case 211:// 00101.10000
//    case 212:
//    case 213:
//      //a 5-a strategie
//      looper = 
//      extraSetup = 
//      //
//      putere = 255;
//      break;
//
//    case 221:// 00101.01000
//    case 222:
//    case 223:
//      //a 5-a strategie
//      looper = 
//      extraSetup = 
//      //
//      putere = 190;
//      break;
//
//    case 231:// 00101.00100
//    case 232:
//    case 233:
//      //a 5-a strategie
//      looper = 
//      extraSetup = 
//      //
//      putere = 127;
//      break;
//    
    default:
      switch(switchModuleValue % 2) {
//        case 0:
//          looper = &oracleLoop;
//          extraSetup = &oracleExtraSetup;
//          oracleSetup();
//          //
//          directionDrt = (switchModuleValue % 7) % 2 == 0 ? true : false;
//          putere = 255;
//        break;

//        case 1:
        case 0:
          looper = &sacaLoop;
          extraSetup = &sacaExtraSetup;
          sacaSetup();
          //
          directionDrt = (switchModuleValue % 7) % 2 == 0 ? true : false;
          putere = 255;
        break;

//        case 2:
//          looper = &rotateLoop;
//          extraSetup = &rotateExtraSetup;
//          rotateSetup();
//          //
//          directionDrt = (switchModuleValue % 7) % 2 == 0 ? true : false;
//          putere = 255;
//        break;

//        case 3:
        case 1:
          looper = &triangleLoop;
          extraSetup = &triangleExtraSetup;
          triangleSetup();
          //
          directionDrt = (switchModuleValue % 7) % 2 == 0 ? true : false;
          putere = 255;
        break;

        default:
          looper = &sacaLoop;
          extraSetup = &sacaExtraSetup;
          sacaSetup();
          //
          directionDrt = (switchModuleValue % 7) % 2 == 0 ? true : false;
          putere = 255;
        break;
      }
    break;
  }
}

