#ifndef MotorCommands_H
#define MotorCommands_H

///////////////////////////////////////////////
/////powers are between -100, 0, and 100///////
///////////////////////////////////////////////

/* Writes the given procentages onto the motors
 *  @parameter stg: int in interval [-100, 100]
 *  @parameter drt: int in interval [-100, 100]
 */
void writeMotors(int stg, int drt);

/* Adapts the powers of the motors in dt
 *  @parameter stg: int in interval [-100, 100]
 *  @parameter drt: int in interval [-100, 100]
 */
void controlledWriteMotors(int stg, int drt);

#endif
