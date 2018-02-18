#ifndef __adc_h__
#define __adc_h__
#include <avr/io.h>
#include <stdio.h>

void adc_init();
uint16_t adc_read(uint8_t ch);

#endif
