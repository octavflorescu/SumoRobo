//
//  Line.hpp
//  SensorCases
//
//  Created by OctavF on 05/05/16.
//  Copyright © 2016 skobbler. All rights reserved.
//

#ifndef Line_hpp
#define Line_hpp

#include <stdio.h>
#include "Definitions.h"

class Line {
    string lineID;
    float slope, intercept;
    
public:
    Line() {lineID = random(); slope = 1.0; intercept = 0.0;}
    Line(string, point, point);
    Line(string, float, float);
    
    string getLineID() const {return lineID;}
    float getSlope() const {return slope;}
    float getIntercept() const {return intercept;}
    
    bool intersectsSquare(square20 sq);
    
    point intersectionPoint(Line l2);
};

Line::Line(string i, point a, point b) {
    lineID = i;
    if (b.x == a.x) {
        slope = b.x;
        intercept = INFINITY;
        
    } else {
        slope = (b.y - a.y) / (b.x - a.x);
        intercept = a.y - slope * a.x;
    }
}

Line::Line(string i, float a, float b) {
    lineID = i;
    slope = a;
    intercept = b;
}

bool Line::intersectsSquare(square20 sq) {
    Line variableLine = Line("", sq.p1, sq.p2);
    point intersection;
    
    if (variableLine.slope == slope && variableLine.intercept == intercept) {
        return true;
    } else if (isfinite(variableLine.intercept)) {
        intersection = intersectionPoint(variableLine);
        if (intersection.isBetweenPoints(sq.p1, sq.p2)) {
            return true;
        }
    }
    
    variableLine = Line("", sq.p2, sq.p3);
    if (variableLine.slope == slope && variableLine.intercept == intercept) {
        return true;
    } else if (isfinite(variableLine.intercept)) {
        intersection = intersectionPoint(variableLine);
        if (intersection.isBetweenPoints(sq.p2, sq.p3)) {
            return true;
        }
    }

    variableLine = Line("", sq.p3, sq.p4);
    if (variableLine.slope == slope && variableLine.intercept == intercept) {
        return true;
    } else if (isfinite(variableLine.intercept)) {
        intersection = intersectionPoint(variableLine);
        if (intersection.isBetweenPoints(sq.p3, sq.p4)) {
            return true;
        }
    }
    
    variableLine = Line("", sq.p4, sq.p1);
    if (variableLine.slope == slope && variableLine.intercept == intercept) {
        return true;
    } else if (isfinite(variableLine.intercept)) {
        intersection = intersectionPoint(variableLine);
        if (intersection.isBetweenPoints(sq.p4, sq.p1)) {
            return true;
        }
    }
    
    return false;
}

point Line::intersectionPoint(Line l2) {
    point x;
    
    if (isfinite(intercept)) {
        x.x = (l2.intercept - intercept) / (slope - l2.slope);
        x.y = (slope * intercept - l2.intercept * l2.slope) / (slope - l2.slope);
        
    } else {
        x.x = slope;
        x.y = l2.slope * x.x + l2.intercept;
    }
    return x;
}

bool operator<(const Line& lhs, const Line& rhs) {
    return lhs.getSlope() < rhs.getSlope();
}

bool operator==(const Line& lhs, const Line& rhs) {
    return lhs.getSlope() == rhs.getSlope() && lhs.getIntercept() == rhs.getIntercept();
}

bool operator!=(const Line& lhs, const Line& rhs) {
    return lhs.getSlope() != rhs.getSlope() || lhs.getIntercept() != rhs.getIntercept();
}

#endif /* Line_hpp */
