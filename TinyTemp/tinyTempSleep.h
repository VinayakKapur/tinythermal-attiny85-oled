/* tinyTempSleep.h 
All the #includes for tinyTempSleep.c
Version     Authors
 *  1.0     Robert K, Vinayak K
 **************************************************************/

// Standard AVR-GCC or WIN-AVR Library Includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "tinyOLED.h" // SSD & i2c library we're using for this project
#include <string.h>
#include <stdlib.h> //itoa

// Prototypes for my own functions
void adc_init(void);
unsigned int get_adc(void);
void WDT_OFF(void);