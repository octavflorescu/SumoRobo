#include "triangle.h"

#include "Declarations.h"

void triangleSetup() {
    currentState = StateForward;
    powStg = power;
    powDrt = power;
}

void triangleExtraSetup() {
    
}

void triangleLoop()
{
  if (millis() - stateTimer > 225)    //300
  {
    currentState = StateGetOnTrack;
    
    if (directionDrt) {
      powDrt = -power40P;
      powStg = -power60P;
    } else {
      powDrt = -power60P;
      powStg = -power40P;
    }
    //stateTimer = millis();
  }
  
  b = PINB & 0b01111111;
  
  switch (b)
  {
    
    case 0b01100000:
      //onLeftBoth = TRUE;
      powStg = power60P;
      powDrt = power;
      break;
    case 0b01000000:
      //currentState = stateWatchYourLeft;
      powStg = power55P;
      powDrt = power;
      break;
    case 0b00100000:
      //currentState = stateWatchYourLeft;
      powStg = power65P;
      powDrt = power;
      break;
    case 0b00010000:
      powStg = power80P;
      powDrt = power;
      break;

    case 0b00001100:
      powStg = power90P;
      powDrt = power;
      break;

    case 0b00011000:
      powStg = power;
      powDrt = power90P;
      break;

    case 0b00011100:
      powStg = power;
      powDrt = power;
      break;

    case 0b00001000:
          if (directionDrt) {
              powStg = power90P;
              powDrt = power;
          } else {
              powStg = power;
              powDrt = power90P;
          }
      break;

    case 0b00000100:
      powStg = power;
      powDrt = power80P;
      break;
          
    case 0b00000011:
      powStg = power;
      powDrt = power60P;
      //onRightBoth = TRUE;
      break;
          
    case 0b00000010:
      //currentState = stateWatchYourRight;
      powStg = power;
      powDrt = power65P;
      //stateTimer = millis();
      break;
          
    case 0b00000001:
      //currentState = stateWatchYourRight;
      powStg = power;
      powDrt = power55P;
      //stateTimer = millis();
      break;
      
    default:    //no sensor sees
      if(currentState == StateGetOnTrack)
      {
        whiteSensorStgSaw = analogRead(whiteSensorStg <= whiteThreshold);
        whiteSensorDrtSaw = analogRead(whiteSensorDrt <= whiteThreshold);
          
        if (whiteSensorStgSaw || whiteSensorDrtSaw)
          {
            currentState = StateForward;
            powStg = power;
            powDrt = power;
            stateTimer = millis();
          }
      }
      break;
  }
}

