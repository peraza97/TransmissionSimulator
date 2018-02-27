#ifndef __servo_h__
#define __servo_h__

//pragmas for servo1 positions
#define servo1_left_pos 2500
#define servo1_mid_pos  1500
#define servo1_right_pos 600
//pragmas for servo2 positions
#define servo2_left_pos 2200
#define servo2_mid_pos  1300
#define servo2_right_pos 500

void servosInit();
void turnServo1(unsigned long int);
void turnServo2(unsigned long int);

void updateServos(unsigned char x);






#endif
