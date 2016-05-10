#include "Utils.h"
#include "Config.h"

#define startModule 12
#define kHaltTime 10000

int const switchPin = A4;

void setup() {
  Serial.begin(57600);
}

void loop() {
  Serial.print(analogRead(switchPin));
  Serial.write("\n");
  delay(3000);
}

/*
 * 00000000 == 1023
 * 
 * 10000000 == 91
 * 01000000 == 184
 * 00100000 == 325
 * 00010000 == 417
 * 
 * 10001000 == 83
 * 01001000 == 155
 * 00101000 == 246
 * 00011000 == 295
 * 
 * 10000100 == 85
 * 01000100 == 164
 * 00100100 == 268
 * 00010100 == 327
 * 
 */
