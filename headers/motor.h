#ifndef __motor_h__
#define __motor_h__

//pragmas for motor duty cycle
#define motor_gear1_speed 100
#define motor_gear2_speed 128
#define motor_gear3_speed 200
#define motor_gear4_speed 255

void motorInit();
void updateMotor(unsigned char x);
void manualMotorUpdate(unsigned long tempY, unsigned long tempX);

#endif
