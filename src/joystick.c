#include "./../headers/joystick.h"
#define DEADZONE 550

//DO NOT DELETE
//USED FOR DEBUGGING
//displays adc value on the lcd

unsigned char getJoystick(){
    uint16_t vertical = adc_read(0);
    uint16_t horizontal = adc_read(1);
    //only capture one input at a time
    if(vertical > DEADZONE + 200){
        return 1;
    }
    else if(vertical < DEADZONE - 200){
        return 2;
    }
    else if(horizontal > DEADZONE + 200){
        return 3;
    }
    else if(horizontal < DEADZONE - 200){
        return 4;
    }
    return 0;
}

unsigned long convertInput(unsigned char x, unsigned long value){
    //converting adc 0 input
    unsigned long temp = 0;
    if(x == 0){
        temp = (value * 1.86) + 600;
    }
    //converting adc 1 input
    else if(x == 1){
        temp = (value * 1.66) + 500;
        if(temp < 1100){
            temp = 1100;
        }
        else if(temp > 2000){
            temp = 2000;
        }
    }
    
    return temp;
}




