#include <avr/io.h>
#include <util/delay.h>
#include "./../headers/motor.h"

void motorInit(){
    TCCR0A |= 1 << COM0A1 | 1 << WGM00 | 1 << WGM01;
    TCCR0B |= 1 << CS01;
}

void updateMotor(unsigned char x){
    switch (x) {
        case 0:
            OCR0A = motor_gear1_speed;
            break;
        case 1:
            OCR0A = motor_gear2_speed;
            break;
        case 2:
            OCR0A = motor_gear3_speed;
            break;
        case 3:
            OCR0A = motor_gear4_speed;
            break;
        default:
            break;
    }
}

/*
void motorChangeGear(unsigned char x){
    OCR0A = x;
}
 */

/*
void motorGear1(){
        OCR0A = 100;
}
void motorGear2(){
        OCR0A = 128;
}
void motorGear3(){
        OCR0A = 200;
}
void motorGear4(){
        OCR0A = 255;
}
 */
