#ifndef __joystick_h__
#define __joystick_h__
#include "./../headers/adc.h"
#include <stdlib.h>

unsigned char getJoystick();
unsigned long convertInput(unsigned char x, unsigned long value);

#endif
