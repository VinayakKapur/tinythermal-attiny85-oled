/******************************************************************
 * tinyTemp.c Firmware for a digital thermometer based on ATtiny85,
 * TMP-36 sensor and SSD1306 OLED display. Uses watchdog timer (WDT) to sleep
 * system in between temp readings to conserve battery power. Displays
 * Temp in F and C.
 
 *  Version     Authors
 *  1.0         Robert K, Vinayak K
 **************************************************************/

#include "tinyTempSleep.h"
#define NUMSAMPLES 25 // #ADC Samples to average
#define VREF 1.1      // ADC reference voltage
#define MAXTEMP 80    // Too-Hot indication  at this temp (Deg F)

// messages to print on OLED
const char TOOHOT[] PROGMEM = "TOO HOT!";

int main(void)
{
    WDT_OFF(); // Make sure the WDT is off at startup
    float tempC, tempF, analogVoltage;
    unsigned char too_hot;
    unsigned int digitalValue;
    unsigned long int totalValue, tempCinteger, tempFinteger;
    char buffer[6];

    adc_init();
    OLED_init(); // *** I deleted Display off in this function **

    // Acquire & average NUMSAMPLES temp measurements
    totalValue = 0;
    for (int i = 0; i < NUMSAMPLES; i++)
        totalValue += get_adc(); // Get a sample from temp sensor
    digitalValue = totalValue / NUMSAMPLES;

    // Convert to temp & analog voltage in mV, mult by 10
    tempC = digitalValue * 1.0 * VREF / 10.24 - 50.;
    tempF = tempC * 9. / 5. + 32.;
    analogVoltage = digitalValue * 1.0 * VREF / 1024;
    analogVoltage = analogVoltage * 1000;
    too_hot = (tempF > MAXTEMP);
    tempCinteger = tempC * 10;
    tempFinteger = tempF * 10;
    
    OLED_clear();

    OLED_cursor(25,1);
    itoa(tempFinteger/10, buffer, 10);
    for (unsigned char i = 0; i < strlen(buffer); i++)
    {
      OLED_printC(buffer[i]);
    }
    OLED_printC('.');
    itoa(tempFinteger%10, buffer, 10);
    for (unsigned char i = 0; i < strlen(buffer); i++)
    {
      OLED_printC(buffer[i]);
    }
    OLED_printC('F');

    OLED_cursor(75, 1);
    itoa(tempCinteger/10, buffer, 10);
    for (unsigned char i = 0; i < strlen(buffer); i++)
    {
      OLED_printC(buffer[i]);
    }
    OLED_printC('.');
    itoa(tempCinteger%10, buffer, 10);
    for (unsigned char i = 0; i < strlen(buffer); i++)
    {
      OLED_printC(buffer[i]);
    }
    OLED_printC('C');

    if (too_hot)
    {
        OLED_cursor(40,2);
        OLED_printP(TOOHOT);
    }

    wdt_enable(WDTO_8S);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
    while (1)
        ;

    return 0;
}

// Initialize ADC peripheral
void adc_init(void)
{
    DDRB = 0x00;  // Make all ADC pins inputs
    ADMUX = 0x83; // Select ADC3; Vref=1.1
    if (F_CPU > 1000000)
        ADCSRA = 0x87; // Enable ADC; divide by 128 for 16 MHz clock
    else
        ADCSRA = 0x83; // Enable ADC; divide by 8 for 1 MHZ clock
}

// Read ADC value
unsigned int get_adc()
{
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    while ((ADCSRA & (1 << ADIF)) == 0)
        ;                      // Wait till ADC finishes
    ADCSRA |= (1 << ADIF);     // Clear ADIF flag
    return ADCL | (ADCH << 8); // Read ADCL first !
}

// Disable WTD and clear reset flag immediately at startup
void WDT_OFF()
{
    MCUSR &= ~(1 << WDRF);
    WDTCR = (1 << WDCE) | (1 << WDE);
    WDTCR = 0x00;
}