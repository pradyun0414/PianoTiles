/* ADC1.c
 * Students put your names here
 * Modified: put the date here
 * 12-bit ADC input on ADC1 channel 5, PB18
 */
#include <ti/devices/msp/msp.h>
#include "../inc/Clock.h"
#define ADCVREF_VDDA 0x000
#define ADCVREF_INT  0x200

uint16_t Data;
int Flag;


void ADCinit(void){
// write code to initialize ADC1 channel 5, PB18
// Your measurement will be connected to PB18
// 12-bit mode, 0 to 3.3V, right justified
// software trigger, no averaging

    ADC1->ULLMEM.GPRCM.RSTCTL = 0xB1000003;
    ADC1->ULLMEM.GPRCM.PWREN = 0x26000001;
    Clock_Delay(24);
    ADC1->ULLMEM.GPRCM.CLKCFG = 0xA9000000;
    ADC1->ULLMEM.CLKFREQ = 7;
    ADC1->ULLMEM.CTL0 = 0x03010000;
    ADC1->ULLMEM.CTL1 = 0x00000000;
    ADC1->ULLMEM.CTL2 = 0x00000000;
    ADC1->ULLMEM.MEMCTL[0] = 5;
    ADC1->ULLMEM.SCOMP0 = 0;
    ADC1->ULLMEM.CPU_INT.IMASK = 0;

}
uint32_t ADCin(void){
  // write code to sample ADC1 channel 5, PB18 once
  // return digital result (0 to 4095)

    ADC1->ULLMEM.CTL0 |= 0x00000001;
    ADC1->ULLMEM.CTL1 |= 0x00000100;
    uint32_t volatile delay=ADC1->ULLMEM.STATUS;
    while((ADC1->ULLMEM.STATUS&0x01) == 0x01) {};
    return ADC1->ULLMEM.MEMRES[0];

}

// your function to convert ADC sample to distance (0.001cm)
// use main2 to calibrate the system fill in 5 points in Calibration.xls
//    determine constants k1 k2 to fit Position=(k1*Data + k2)>>12
uint32_t Convert(uint32_t input){

  // 0.4473 x 2^12 is approx. 1832
  // My equation is (1832*Data)>>12 - 176

    return (1832*input-176)>>12;

}

// do not use this function
// it is added just to show you how SLOW floating point in on a Cortex M0+
float FloatConvert(uint32_t input){
  return 0.00048828125*input -0.0001812345;
}

