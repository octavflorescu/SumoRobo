#include "Utils.h"
#include "Config.h"

#define startModule 12
#define kHaltTime 10000

int powStg, powDrt;
long stateTimer;
long aimingTime;
short b;

State currentState;

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
  Serial1.begin(57600);

  DDRB = 0b00000000;
  
  pinAsInput(startModule);
  powStg = 0;
  powDrt = 0;
  currentState = StateSearch;
  
  WriteMotors(0, 0);
  delay(50);
  WriteMotors(0, 0);
  delay(50);
  
  while (isLow(startModule)) {
    WriteMotors(0, 0);
  }
}

void loop() {
  switch (currentState) {
    case StateSearch:
      search();
      break;
      
    case StateAim:
      rotateAim();
      break;
      
    case StateHalt:
      halt();
      break;
      
    default:
      break;
  }
  WriteMotors(powStg, powDrt);
}

/////////////////////// private

void search() {
  b = PINB; // 0b cba54321
  switch (b) {
    case 0b00000001://1
      
    break;
    
    case 0b00001001://14
      
    break;

    case 0b00101001://14a
      
    break;
//1a
//134ab
//134b
//134bc
//1345ab
//1345b
//1345bc
//13ab
//13b
//13bc
//135ab
//135b
//135bc
//12
//124a
//12a
//1234ab
//1234abc
//1234b
//1234bc
//12345ab
//12345abc
//12345b
//12345bc
//123b
//1235b
//4
//4a
//45
//45c
//a
//345b
//35b
//35bc
//c
//2
//245c
//2345abc
//2345b
//2345bc
//235b
//235bc
//2c
//25
//25c
//5
//5c
  }
}

/////////////////////////////////////////////////////
// Other
/////////////////////////////////////////////////////

void halt() {
  if (millis() - stateTimer > kHaltTime) {
    currentState = StateSearch;
  }
}

void rotateAim() {
  if (millis() - stateTimer > aimingTime) {
    currentState = StateHalt;
    powDrt = 0;
    powStg = 0;
    stateTimer = millis();
  }
}
