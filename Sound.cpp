// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jonathan Valvano
// 11/15/2021 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"


//void SysTick_IntArm(uint32_t period, uint32_t priority){
//  // write this
//}
//// initialize a 11kHz SysTick, however no sound should be started
//// initialize any global variables
//// Initialize the 5 bit DAC

void Sound_Init(uint32_t period, uint32_t priority){

       SysTick->CTRL = 0x00; // disable during initialization
       SysTick->LOAD = period-1; // set reload register
       SCB->SHP[1] = (SCB->SHP[1] & (~0xC0000000)) | priority<<30;
       SysTick->VAL = 0; // clear count, cause reload
       SysTick->CTRL = 0x07; // Enable SysTick IRQ and SysTick Timer

}

extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz

    const uint8_t wave[32] = {16,19,22,24,27,28,30,31,31,31,30,28,27,24,22,19,16,13,10,8,5,4,2,1,1,1,2,4,5,8,10,13};
    static uint32_t i=0;
    DAC5_Out(wave[i]);
    i = (i+1) & 0x1F; // Cycles

}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement

void Sound_Start(uint32_t period){

    SysTick->CTRL = 0x00; // disable during initialization
    SysTick->LOAD = period-1; // set reload register
    SysTick->VAL = 0; // clear count, cause reload
    SysTick->CTRL = 0x07; // Enable SysTick IRQ and SysTick Timer

}



void Sound_Shoot(void){
// write this
  
}
void Sound_Killed(void){
// write this
  
}
void Sound_Explosion(void){
// write this
  
}

void Sound_Fastinvader1(void){
  
}
void Sound_Fastinvader2(void){
  
}
void Sound_Fastinvader3(void){
  
}
void Sound_Fastinvader4(void){
  
}
void Sound_Highpitch(void){
  
}
