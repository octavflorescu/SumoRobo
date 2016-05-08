#include "MotorCommands.h"

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <iostream>

#define power 255
#define zeroTo100 70

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define BUFFER_LENGTH 9

typedef struct {
    uint8_t packetStart[3];
    int16_t pMotorR_sds16;
    int16_t pMotorL_sds16;
    int16_t yCheck_sds16;
} motorCommand;

typedef union {
    motorCommand sMotorCommand;
    uint8_t data[BUFFER_LENGTH];
} rawPacket;

//(int16_t)(m1 * 0.925);

#define WriteMotors(m1, m2) \
({ \
std::cout << m1 << " " << m2 << "\n"; \
})
//rawPacket sRawPacket_mds = { '#', '#', '#', 0, 0, 0 }; \
//sRawPacket_mds.sMotorCommand.pMotorR_sds16 = (int16_t)(m1 * power); \
//sRawPacket_mds.sMotorCommand.pMotorL_sds16 = -(int16_t)(m2 * power); \
// sRawPacket_mds.data; \


/////////////////////////////////////////////////////////////////////
//////////////////// Variable Declarations //////////////////////////

bool direct = false;

typedef struct {
    int stg;
    int drt;
} motorPower;

typedef struct {
    long t0;//time at calculation
    long stg;
    long drt;
} motorDeltaTime;

motorPower currentMotorPower = {0, 0};
motorPower targetMotorPower = {0, 0};
motorPower initialMotorPower = {0, 0};
motorPower powerSign = {1, 1};

motorDeltaTime timeNeeded = {LONG_MAX, LONG_MAX, LONG_MAX};
motorDeltaTime variableTime = {LONG_MAX, 0, 0};
long passedTime = 0;

/////////////////////////////////////////////////////////////////////
//////////////////////////// Private ////////////////////////////////

motorPower currentPower() {
    powerSign = {targetMotorPower.stg >= 0 ? 1 : -1, targetMotorPower.drt >= 0 ? 1 : -1};
    
    passedTime = std::min(std::time(&appInitialTimer) - timeNeeded.t0, (long)1);
    
    if (direct) {
        //TODO: (FOC) check this out
        
        
        initialMotorPower.stg = int(initialMotorPower.stg +
                    abs(initialMotorPower.stg - targetMotorPower.stg) * std::min(passedTime / std::max(timeNeeded.stg, (long)1), (long)(zeroTo100 / 100)) * powerSign.stg);
        initialMotorPower.drt = int(initialMotorPower.drt +
                    abs(initialMotorPower.drt - targetMotorPower.drt) * std::min(passedTime / std::max(timeNeeded.drt, (long)1), (long)(zeroTo100 / 100)) * powerSign.drt);
        return initialMotorPower;
    } else {
        //TODO: (FOC) check this out
        if (passedTime - std::max(timeNeeded.stg, timeNeeded.drt) < 0) {
            
            if (timeNeeded.stg == timeNeeded.drt) {
                currentMotorPower.stg = currentMotorPower.stg + abs(currentMotorPower.stg - targetMotorPower.stg);
                currentMotorPower.drt = currentMotorPower.drt + abs(currentMotorPower.drt - targetMotorPower.drt);
                
            } else if (timeNeeded.stg > timeNeeded.drt) {
                currentMotorPower.stg = int(currentMotorPower.stg + abs(abs(currentMotorPower.stg - targetMotorPower.stg) - abs(currentMotorPower.drt - targetMotorPower.drt)) * powerSign.stg / passedTime + 0.5);
                currentMotorPower.drt = int(currentMotorPower.drt + abs(currentMotorPower.drt - targetMotorPower.drt) / timeNeeded.stg * powerSign.drt);
                
            } else {
//                timeNeeded.stg < timeNeeded.drt
                currentMotorPower.drt = int(currentMotorPower.drt + abs(abs(currentMotorPower.drt - targetMotorPower.drt) - abs(currentMotorPower.stg - targetMotorPower.stg)) * powerSign.drt / std::max(passedTime, (long)(zeroTo100 / 100)) + 0.5);
                currentMotorPower.stg = int(currentMotorPower.stg + abs(currentMotorPower.stg - targetMotorPower.stg) / timeNeeded.drt * powerSign.stg);
            }
        }
    }
    return currentMotorPower;
}

motorDeltaTime deltaTime() {
    variableTime.t0 = std::time(&appInitialTimer);
    variableTime.stg = long(abs(currentMotorPower.stg - targetMotorPower.stg) * zeroTo100 / 100);
    variableTime.drt = long(abs(currentMotorPower.drt - targetMotorPower.drt) * zeroTo100 / 100);
    
    return variableTime;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// Functions ///////////////////////////////

void writeMotors(int stg, int drt) {
    if (!direct) {
        initialMotorPower = currentPower();
    }
    
    direct = true; // motor command state
    
    targetMotorPower.stg = stg;
    targetMotorPower.drt = drt;
    
    timeNeeded = deltaTime();
    initialMotorPower = currentPower();
    
//    WriteMotors(stg, drt);
    std::cout << stg << " " << drt << "\n";
}

void controlledWriteMotors(int stg, int drt) {
    targetMotorPower.stg = stg;
    targetMotorPower.drt = drt;
    
    if (direct) {
        currentMotorPower = currentPower();
    }
    timeNeeded = deltaTime();
    
    direct = false; // motor command state
    currentMotorPower = currentPower();
    
//    WriteMotors(currentMotorPower.stg, currentMotorPower.drt);
    std::cout << currentMotorPower.stg << " " << currentMotorPower.drt << "\n";
}
