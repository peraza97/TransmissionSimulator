#include <avr/io.h>
#include <util/delay.h>
#include "./../headers/servo.h"

//init servo1 on port D5
void servosInit(){
    TCCR3A |= 1 << WGM31 | 1 << COM3A1 | 1 << COM3A0 | 1 << COM3B1 | 1 << COM3B0;
    TCCR3B |= 1 << WGM32 | 1 << WGM33 | 1 << CS31;
    
    ICR3 = 19999;
    
}


void turnServo1(unsigned long int degree){
    
    OCR3A = ICR3 - degree;
    _delay_ms(500);
}
void turnServo2(unsigned long int degree){
    
    OCR3B = ICR3 - degree;
    _delay_ms(500);
}

void updateServos(unsigned char x){
    switch(x){
        //gear 1
        case 0:
            turnServo2(servo2_left_pos);
            turnServo1(servo1_right_pos);
            break;
        //gear 2
        case 1:
            turnServo2(servo2_right_pos);
            turnServo1(servo1_right_pos);
            break;
        //gear 3
        case 2:
            turnServo2(servo2_left_pos);
            turnServo1(servo1_left_pos);
            break;
        //gear 4
        case 3:
            turnServo2(servo2_right_pos);
            turnServo1(servo1_left_pos);
            break;
        //set to the middle
        case 4:
            turnServo2(servo2_mid_pos);
            turnServo1(servo1_mid_pos);
            break;
        default:
            break;
    }
}

