#ifndef UTILS_H
#define UTILS_H

#include "Utils.c"

int digitalToInterruptPin(int pin);
// Board          int.0  int.1  int.2  int.3  int.4  int.5
// Uno, Ethernet      2      3
// Mega2560           2      3     21     20     19     18
// Micro              3      2      0      1      7
#define digitalToInterruptPin(P) (P == 2 ? 0 : P == 3 ? 1 : -1)

#define portOfPin(P)\
  (P >= 0 && P < 5 ? &PORTD :\
    P == 5 ? &PORTC :\
      P == 6 ? &PORTD :\
        P == 7 ? &PORTE :\
          P >= 8 && P <= 11 ? &PORTB :\
            P == 12 ? &PORTD : &PORTC)
            
#define ddrOfPin(P)\    
  (P >= 0 && P < 5 ? &DDRD :\
    P == 5 ? &DDRC :\
      P == 6 ? &DDRD :\
        P == 7 ? &DDRE :\
          P >= 8 && P <12 ? &DDRB :\
            P == 12 ? &DDRD : &DDRC)  
            
#define pinOfPin(P)\    
  (P >= 0 && P < 5 ? &PIND :\
    P == 5 ? &PINC :\
      P == 6 ? &PIND :\
        P == 7 ? &PINE :\
          P >= 8 && P <12 ? &PINB :\
            P == 12 ? &PIND : &PINC)  

#define pinIndex(P)\
    (P == 0 ? 2 :\
      P == 1 ? 3 :\
        P == 2 ? 1 :\
          P == 3 ? 0 :\
            P == 4 || P == 8 ? 4 :\
              P == 5 || P == 7 || P == 10 || P == 12 ? 6 :\
                P == 6 || P == 11 || P == 13 ? 7 :\
                  P == 9 ? 5 : -1)
            
#define pinMask(P)\
  ((uint8_t)\
    1<<pinIndex(P))

#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))

#endif
