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
  
  
  switch (b & 0b10111111) { // combinations of ca 1-4 (without b)
//    case 0b00000001://1
//      
//    break;
//    
//    case 0b00001001://14
//      
//    break;
//
//    case 0b00101001://14a
//      
//    break;
//    
//    case 0b00100001://1a
//      
//    break;

    case 0b00101101://134a
      
    break;
    
    case 0b00001101://134
      
    break;
    
    case 0b00111101://1345a
    
    break;

    case 0b00011101://1345
    
    break;

    case 0b00000101://13
      
    break;

    case 0b10000101://13c
      
    break;

    case 0b00110101://135a
      
    break;

    case 0b00010101://135
      
    break;

    case 0b10010101://135c
      
    break;
    
//    case 0b00000011://12
//      
//    break;
//
//    case 0b00101011://124a
//      
//    break;
//
//    case 0b00100011://12a
//      
//    break;

    case 0b00101111://1234a
      
    break;

    case 0b10101111://1234ac
      
    break;

    case 0b00001111://1234
      
    break;

    case 0b10001111://1234c
      
    break;

    case 0b00111111://12345a
      
    break;

    case 0b10111111://12345ac
      
    break;

    case 0b00011111://12345
      
    break;

    case 0b10011111://12345c
      
    break;

    case 0b00100111://123a
      
    break;

    case 0b00000111://123
      
    break;

    case 0b00010111://1235
      
    break;

//    case 0b00001000://4
//      
//    break;
//
//    case 0b00101000://4a
//      
//    break;
//
//    case 0b00011000://45
//      
//    break;
//
//    case 0b10011000://45c
//      
//    break;

//    case 0b00100000: //a
//    
//    break;

    case 0b00011100: //345
    
    break;

    case 0b10011100: //345c
    
    break;

    case 0b00110100: //35a
    
    break;

    case 0b00010100: //35
    
    break;

//    case 0b10000000: //c
//    
//    break;
//
//    case 0b00000010: //2
//    
//    break;
//
//    case 0b10011010: //245c
//    
//    break;

    case 0b00111110: //2345a
    
    break;

    case 0b10111110: //2345ac
    
    break;

    case 0b00011110: //2345
    
    break;

    case 0b10011110: //2345c
    
    break;

    case 0b00011010: //235
    
    break;

    case 0b100011010: //235c
    
    break;

//    case 0b100000010: //2c
//    
//    break;
//
//    case 0b000010010: //25
//    
//    break;
//
//    case 0b000010000: //5
//    
//    break;
//
//    case 0b100010000: //5c
//    
//    break;


    default:
      if (b & 0b01000000) {
        if(analogRead(whiteSensorStgF) > kWhiteThreshold &&
           analogRead(whiteSensorDrtF) > kWhiteThreshold) {
          //check white line sensors
          powStg = power;
          powDrt = power;
        } else {
          //APPLY ciocane
        }
      } else {
        //APPLY search
      }
    break;
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
