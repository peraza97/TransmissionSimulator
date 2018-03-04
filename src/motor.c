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
        case 4:
            OCR0A = 0;
        default:
            break;
    }
}

void manualMotorUpdate(unsigned long tempY, unsigned long tempX){
    //gear 1
    if(tempY < 700 && tempX > 1900 ){
        updateMotor(0);
    }
    //gear 2
    else if(tempY > 2100 && tempX > 1900){
        updateMotor(1);
    }
    //gear 3
    else if(tempY < 700 && tempX < 1200){
        updateMotor(2);
    }
    //gear 4
    else if(tempY > 2200 && tempX < 1200){
        updateMotor(3);
    }
    else{
        updateMotor(4);
    }
}

