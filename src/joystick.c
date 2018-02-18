#include "./../headers/joystick.h"
#define DEADZONE 550

//DO NOT DELETE
//USED FOR DEBUGGING
//displays adc value on the lcd
/*
char str[16];
sprintf(str, "%u", adc_read(0));
LCD_DisplayString(1,str);
*/

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
    //else if(horizontal > DEADZONE + 200){
    //    return 3;
    //}
    //else if(horizontal < DEADZONE - 200){
    //    return 4;
    //}
    return 0;
}




