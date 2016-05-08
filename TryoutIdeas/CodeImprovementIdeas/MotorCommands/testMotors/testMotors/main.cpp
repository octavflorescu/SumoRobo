//
//  main.cpp
//  testMotors
//
//  Created by OctavF on 02/05/16.
//  Copyright © 2016 skobbler. All rights reserved.
//

#include <iostream>

#include "MotorCommands.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    appInitialTimer = time(NULL);
    
    writeMotors(0, 0);
    
    long timeS = time(&appInitialTimer);
    
    while (true) {
//        timeS = time(&appInitialTimer) % 10000;
//        
//        if (timeS > 20000 && timeS < 70000) {
            controlledWriteMotors(70, 100);
//        } else {
//            writeMotors(0, 0);
//        }
    }
    return 0;
}
