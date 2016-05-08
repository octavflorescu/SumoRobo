//
//  Definitions.h
//  SensorCases
//
//  Created by OctavF on 05/05/16.
//  Copyright © 2016 skobbler. All rights reserved.
//

#ifndef Definitions_h
#define Definitions_h

#include <math.h>

#define kRobotSize 20.0

using namespace std;

struct point {
    float x, y;
    
    bool isBetweenPoints(point p1, point p2) {
        bool isBetweenOnX = ((p1.x <= x) && (p2.x >= x)) || ((p1.x >= x) && (p2.x <= x));
        bool isBetweenOnY = ((p1.y <= y) && (p2.y >= y)) || ((p1.y >= y) && (p2.y <= y));
        
        if (p1.x == p2.x) {
            return isBetweenOnY;
        }
        if (p1.y == p2.y) {
            return isBetweenOnX;
        }
        // match the gradients
        return isBetweenOnX && isBetweenOnY;
    }
};

struct square20 {
    point p1, p2, p3, p4;
    
    square20();
    
    square20(point center) {
        p1 = {static_cast<float>(center.x - (kRobotSize / 2.0)),
            static_cast<float>(center.y + kRobotSize / 2.0)};
        p2 = {static_cast<float>(center.x + kRobotSize / 2.0),
            static_cast<float>(center.y + kRobotSize / 2.0)};
        p3 = {static_cast<float>(center.x + kRobotSize / 2.0),
            static_cast<float>(center.y - kRobotSize / 2.0)};
        p4 = {static_cast<float>(center.x - kRobotSize / 2.0),
            static_cast<float>(center.y - kRobotSize / 2.0)};
    }
    
    void rotateAtAngle(float angle)
    {
        float s = sin(angle);
        float c = cos(angle);
        
        float xnew;
        float ynew;
        
        // rotate point 1
        xnew = p1.x * c - p1.y * s;
        ynew = p1.x * s + p1.y * c;
        p1.x = xnew;
        p1.y = ynew;
        
        // rotate point 2
        xnew = p2.x * c - p2.y * s;
        ynew = p2.x * s + p2.y * c;
        p2.x = xnew;
        p2.y = ynew;
        
        // rotate point 3
        xnew = p3.x * c - p3.y * s;
        ynew = p3.x * s + p3.y * c;
        p3.x = xnew;
        p3.y = ynew;
        
        // rotate point 4
        xnew = p4.x * c - p4.y * s;
        ynew = p4.x * s + p4.y * c;
        p4.x = xnew;
        p4.y = ynew;
    }
};

#endif /* Definitions_h */
