#include <QTRSensors.h>

#define ADDRESS 128 // READ ONLY (DON'T TOUCH)
#define MR_FWD 0 //
#define MR_BACK 1 //
#define ML_FWD 4 //
#define ML_BACK 5 // READ ONLY (DON'T TOUCH)

#define ROBOCLAW_MESSAGE(MODE, SPEED) {ADDRESS, MODE, SPEED, ((ADDRESS + MODE + SPEED) & 0X7f)}
#define swrite(arr) Serial1.write(arr, sizeof(arr))

#define LEFT_ERROR_CORRECTION 100/127
#define RIGHT_ERROR_CORRECTION 1

#define omron_l 7
#define omron_r 8
#define pepperl_l 9
#define pepperl_r 10
#define wenglor_l 11
#define wenglor_r 12
#define LEFTMOTOR true
#define RIGHTMOTOR false
boolean on = true;
//#define LED 13

/**
* speed E [-127, 127]
*/

void delay_5_sec(){
  delay(5000);
  on = true;
}

void mWrite(boolean left, int  speed){
  if(left){
    speed = speed * LEFT_ERROR_CORRECTION;
    if(speed < 0){
      speed = -speed;
      byte motor_command[] = ROBOCLAW_MESSAGE(ML_BACK, speed);
      swrite(motor_command);
    }
    else{
      byte motor_command[] = ROBOCLAW_MESSAGE(ML_FWD, speed);
      swrite(motor_command);
    }
  }
  else{
    speed = speed * RIGHT_ERROR_CORRECTION;
    if(speed < 0){
      speed = -speed;
      byte motor_command[] = ROBOCLAW_MESSAGE(MR_BACK, speed);
      swrite(motor_command);
    }
    else{
      byte motor_command[] = ROBOCLAW_MESSAGE(MR_FWD, speed);
      swrite(motor_command);
    }
  }
}


QTRSensorsRC qtrA0((unsigned char[]) {A0}, 1);
QTRSensorsRC qtrA1((unsigned char[]) {A1}, 1);
QTRSensorsRC qtrA2((unsigned char[]) {A2}, 1);
QTRSensorsRC qtrA3((unsigned char[]) {A3}, 1);
unsigned int sensorValuesA0[1];
unsigned int sensorValuesA1[1];
unsigned int sensorValuesA2[1];
unsigned int sensorValuesA3[1];
char whiteSensor[5];
int sensorCase;

//#define LED 13
#define START 13

void setup()
{
  pinMode (wenglor_l,INPUT);
  pinMode (wenglor_r,INPUT);
  pinMode (pepperl_l, INPUT);
  pinMode (pepperl_r, INPUT);

//  pinMode (LED, OUTPUT);
  pinMode (A0, INPUT);
  pinMode (A1, INPUT);
  pinMode (A2, INPUT);
  pinMode (A3, INPUT);
//  pinMode (LED, OUTPUT);
  pinMode (START, INPUT);
  Serial1.begin(19200);
  mWrite(LEFTMOTOR, 0);
  mWrite(RIGHTMOTOR, 0);

  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
}
  int s;
  int hs = 0;
  int start;
  int index = 0;
  int sensor_l;
  int sensor_r;
  int m1 = 0; //cazuri: m1=-2 full_back; m1=-1 half_back; m1=0 stop; m1=1 half_fwd; m1=2 full_fwd;
  int m2 = 0; //la fel
  int sens = 0; //sensul de rotire 0 pt stanga, 1 pt dreapta

void Linie()
{
  switch(sensorCase){
    case 1100 : if(m1 != 2)
                {
                  mWrite(RIGHTMOTOR, 127);
                  m1=2;
                }
                if(m2 != 2)
                {
                  mWrite(LEFTMOTOR, 127);
                  m2=2;
                }
                delay(200);
                break;

    case 11 : if(m1 != -2)
              {
                mWrite(RIGHTMOTOR, -127);
                m2 = -2;
              }
              if(m2 != -2)
              {
                mWrite(LEFTMOTOR, -127);
                m2 = -2;
              }
              delay(200);
              break;

    case 110 : if(m2 != 2)
               {
                 mWrite(LEFTMOTOR, 127);
                 m2 = 2;
               }
               if(m1 != 1)
               {
                 mWrite(RIGHTMOTOR, 63);
                 m1 = 1;
               }
               sens = 1;
               delay(600);
               break;

    case 1001 : if(m1 != 2)
                {
                  mWrite(RIGHTMOTOR, 127);
                  m1 = 2;
                }
                if(m2 != 1)
                {
                  mWrite(LEFTMOTOR, 63);
                  m2 = 1;
                }
                sens = 0;
                delay(600);
                break;

    case 1000 : if(m1 != 2)
                {
                  mWrite(RIGHTMOTOR, 127);
                  m1 = 2;
                }
                if(m2 != 1)
                {
                  mWrite(LEFTMOTOR, 63);
                  m2 = 1;
                }
                sens = 0;
                delay(400);
                break;

    case 100 : if(m2 != 2)
               {
                 mWrite(LEFTMOTOR, 127);
                 m2 = 2;
               }
               if(m1 != 1)
               {
                 mWrite(RIGHTMOTOR, 63);
                 m1 = 1;
               }
               sens = 1;
               delay(400);
               break;

    case 10 : if(m2 != -2)
              {
                mWrite(LEFTMOTOR, -127);
                m2 = -2;
              }
              if(m1 != -1)
              {
                mWrite(RIGHTMOTOR, -63);
                m1 = -1;
              }
              sens = 0;
              delay(400);
              break;
    case 1 : if(m2 != -1)
             {
               mWrite(LEFTMOTOR, -63);
               m2 = -1;
             }
             if(m1 != -2)
             {
               mWrite(RIGHTMOTOR, -127);
               m1 = -2;
             }
             sens = 1;
             delay(400);
             break;
  }

}

void runBasicStrategy(){

    sensor_l = digitalRead(wenglor_l);
    sensor_r = digitalRead(wenglor_r);
    s=sensor_l*10+sensor_r;
    if(digitalRead(pepperl_l)){
      hs = 10;
      index = 0;
    }
    if(digitalRead(pepperl_r)){
      hs = 1;
      index = 0;
    }
    if(digitalRead(omron_l)) {
        hs = 1;
        index = 0;
    }
    if(digitalRead(omron_r)) {
        hs = 10;
        index = 0;
    }

    //constructie senzor linie
    // senzor spate dreapta
    if(sensorValuesA0[0] >= 0 && sensorValuesA0[0] <= 500)
      whiteSensor[0] = '1';
    else
     whiteSensor[0] = '0';

    // senzor spate stanga
    if(sensorValuesA1[0] >= 0 && sensorValuesA1[0] <= 500)
      whiteSensor[1] = '1';
    else
      whiteSensor[1] = '0';

    // senzor fata dreapta
    if(sensorValuesA2[0] >= 0 && sensorValuesA2[0] <= 150)
      whiteSensor[3] = '1';
    else
      whiteSensor[3] = '0';

    // senzor fata stanga
    if(sensorValuesA3[0] >= 0 && sensorValuesA3[0] <= 320)
      whiteSensor[2] = '1';
    else
      whiteSensor[2] = '0';

    sensorCase = atoi(whiteSensor); // dreapta spate ; stanga spate ; stanga fata ; dreapta fata

    switch (s)
    {
      case 11: if(m1 != 2) //
               {
                 mWrite(RIGHTMOTOR, 127);
                 m1 = 2;
               }
               if(m2 != 2)
               {
                 mWrite(LEFTMOTOR, 127);
                 m2 = 2;
               }
               break;

      case 10 : if (sensorCase) Linie();
                if(m1 != 2)
                {
                  mWrite(RIGHTMOTOR, 127);
                  m1 = 2;
                }
                if(m2 != 0)
                {
                  mWrite(LEFTMOTOR, 0);
                  m2 = 0;
                }
                hs = s;
                index = 0;

                break;

      case 1 : if(sensorCase) Linie();
               if(m1 != 0)
               {
                 mWrite(RIGHTMOTOR, 0);
                 m1 = 0;
               }
               if(m2 != 2)
               {
                 mWrite(LEFTMOTOR, 127);
                 m2 = 2;
               }
               hs = s;
               index = 0;
               break;

      case 0:  switch(hs)
              {
                  case 0 : if(sensorCase) Linie();
                           if(sens == 1)
                           {
                             if(m1 != -1)
                             {
                               mWrite(RIGHTMOTOR, -127);
                               m1 = -1;
                             }
                             if(m2 != 1)
                             {
                               mWrite(LEFTMOTOR, 127);
                               m2 = 1;
                             }

                            //  Serial.println("case 0, if");
                            //  Serial.print ("hs: ");
                            //  Serial.println(hs);
                            //  Serial.print ("sens: ");
                            //  Serial.println(sens);
                            //  Serial.println ("Motor dreapta: -127");
                            //  Serial.println ("Motor stanga: 127");
                           }
                           else
                           {
                             if(m1 != 1)
                             {
                               mWrite(RIGHTMOTOR, 127);
                               m1 = 1;
                             }
                             if(m2 != -1)
                             {
                               mWrite(LEFTMOTOR, -127);
                               m2 = -1;
                             }

                            //  Serial.println("case 0, else");
                            //  Serial.print ("hs: ");
                            //  Serial.println(hs);
                            //  Serial.print ("sens: ");
                            //  Serial.println(sens);
                            //  Serial.println ("Motor dreapta: -127");
                            //  Serial.println ("Motor stanga: 127");
                           }
                           break;

                  case 1 : if(sensorCase) Linie();
                           if(m1 != 0)
                           {
                             mWrite(RIGHTMOTOR, 0);
                             m1 = 0;
                           }
                           if(m2 != 2)
                           {
                             mWrite(LEFTMOTOR, 127);
                             m2 = 2;
                           }
                           delay(1); index ++;
                           if(index == 700)
                           {
                             hs = 0;
                             index = 0;
                           }
                           break;

                          //  Serial.println("case 1");
                          //  Serial.println ("Motor dreapta: 0");
                          //  Serial.println ("Motor stanga: 127");

                  case 10 : if(sensorCase) Linie();
                            if(m1 != 2)
                            {
                              mWrite(RIGHTMOTOR, 127);
                              m1 = 2;
                            }
                            if(m2 != 0)
                            {
                              mWrite(LEFTMOTOR, 0);
                              m2 = 0;
                            }
                            delay(1);
                            index ++;
                            if(index == 700)
                            {
                              hs = 0;
                              index = 0;
                            }
                            break;

                            // Serial.println("case 10");
                            // Serial.println ("Motor dreapta: 127");
                            // Serial.println ("Motor stanga: 0");
               }
    }

}

bool turn(bool left){
  static bool used = false;
  static unsigned long timer;
  if(!used){
    timer = millis();
    used = true;
  }
  if(millis() - timer <300){
    if(millis() - timer < 150){
        mWrite(!left, 127);
        mWrite(left, 50);
    }
    else{
       mWrite(!left, 127);
       mWrite(left, 127);
    }
    return true;
  }
  return false;

}

void loop()
{
  // int strategy = (((digitalRead(2)*2 + digitalRead(3))*2 + digitalRead(4))*2 + digitalRead(5))*2 + digitalRead(6);
  int programmer = digitalRead(2) << 4 |
                   digitalRead(3) << 3 |
                   digitalRead(4) << 2 |
                   digitalRead(5) << 1 |
                   digitalRead(6);
  int strategy = programmer & 7;

  int qtrFata = programmer & 8;
  int qtrSpate = programmer & 16;

  if (qtrSpate) {
    qtrA0.read(sensorValuesA0);
    qtrA1.read(sensorValuesA1);
  }
  if (qtrFata) {
    qtrA2.read(sensorValuesA2);
    qtrA3.read(sensorValuesA3);
  }
  start = digitalRead(START);

  if(start == HIGH){
    if(on == false){
      delay_5_sec();
    }
    switch(strategy){
      case 0:
        sens = 0;
        runBasicStrategy();
        break;
      case 1:
        sens = 1;
        runBasicStrategy();
        break;
      case 2:
      sens = 0;
        if(!turn(true))
          runBasicStrategy();
        break;
      case 3:
        sens = 1;
        if(!turn(false))
          runBasicStrategy();
        break;

      default:
          sens = 0;
          runBasicStrategy();
          break;
    }
  }
  else
  {
    mWrite(RIGHTMOTOR, 0);
    mWrite(LEFTMOTOR, 0);
  }

}
