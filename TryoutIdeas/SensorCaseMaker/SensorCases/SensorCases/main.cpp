//
//  main.cpp
//  SensorCases
//
//  Created by OctavF on 05/05/16.
//  Copyright © 2016 skobbler. All rights reserved.
//

#include <iostream>

#include <vector>
#include <map>
#include "Line.h"

#define kRange 130

string stringFromLines(vector<Line> lines) {
    string cKey = "";
    
    for (int i = 0; i < lines.size(); i++) {
        cKey += lines[i].getLineID();
    }
    return cKey;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    Line lines[] = {
        Line("1", {-8, 0}, {-8, 20}),
        Line("2", {-3.5, 0}, {0, 20}),
        Line("3", {0, 0}, {0, 20}),
        Line("4", {3.5, 0}, {0, 20}),
        Line("5", {8, 0}, {8, 20})//,
//        Line("a", {-8, 0}, {-8, 20}),
//        Line("b", {-8, 0}, {-8, 20}),
//        Line("c", {-8, 0}, {-8, 20})
    };
    unsigned int numberOfLines = sizeof(lines) / sizeof(Line);
    
    map<vector<Line>, vector<square20>> cases = map<vector<Line>, vector<square20>>();
    map<string, vector<Line>> caseKeys = map<string, vector<Line>>();
    vector<Line> vecLine, caseKey;
    string stringCaseKey;
    Line currentLine;
    
    square20 square = square20({0,0});
    
    // move on X
    for (float i = -kRange; i < kRange; i++) {
        
        // move on Y
        for (float j = kRobotSize / 2.0; j < kRange; j++) {
            square = square20({i, j});
            
            // rotate square
            for (float k = 0.0; k <= 3.2; k += 0.2) {
                square.rotateAtAngle(k);
                vecLine = vector<Line>();
                
                // check each line
                for (int l = 0; l < numberOfLines; l++) {
                    currentLine = lines[l];
                    
                    if (currentLine.intersectsSquare(square)) {
                        vecLine.push_back(currentLine);
                    }
                }
                if (!vecLine.empty()) {
                    stringCaseKey = stringFromLines(vecLine);
                    caseKey = caseKeys[stringCaseKey];
                    
                    // if the re is no line vector in main map, add it to both
                    if (caseKey.empty()) {
                        caseKeys[stringCaseKey] = vecLine;
                        cases[vecLine].push_back(square);
                        
                    } else { // otherwise just append the square to the main map
                        cases[caseKey].push_back(square);
                    }
                }
            }
        }
    }
    for (std::map<vector<Line>, vector<square20>>::iterator it=cases.begin(); it!=cases.end(); ++it) {
        cout << stringFromLines(it->first) << "\n";
    }
    return 0;
}
