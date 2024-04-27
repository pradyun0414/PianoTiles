// Dump.c
// Your solution to ECE319K Lab 3 Spring 2024
// Author: Pradyun  Bhaskar
// Last Modified: 02/13/2024


#include <ti/devices/msp/msp.h>
#include "../inc/Timer.h"
#define MAXBUF 50
uint32_t DataBuffer[MAXBUF];
uint32_t TimeBuffer[MAXBUF];
uint32_t DebugCnt; // 0 to MAXBUF (0 is empty, MAXBUF is full)

// *****Debug_Init******
// Initializes your index or pointer.
// Input: none
// Output:none
void Debug_Init(void){
// students write this for Lab 3
// This function should also initialize Timer G12, call TimerG12_Init.
  
    DebugCnt = 0;
    //Clock_Init80Mhz(0); // Already in the main function
    TimerG12_Init();


    // Addresses are Base Address + Index*4n, so Buf + 4*Cnt (32 bit elements are 4 bytes)
}

// *****Debug_Dump******
// Records one data and one time into the two arrays.
// Input: data is value to store in DataBuffer
// Output: 1 for success, 0 for failure (buffers full)
uint32_t Debug_Dump(uint32_t data){
// students write this for Lab 3
// The software simply reads TIMG12->COUNTERREGS.CTR to get the current time in bus cycles.

    if(DebugCnt >= MAXBUF)
    {
        return 0; // Full
    }

    DataBuffer[DebugCnt] = data;
    TimeBuffer[DebugCnt] = TIMG12->COUNTERREGS.CTR;
    //DebugCnt = (DebugCnt+1) & (SIZE-1); // Allows count to "roll over" if it reaches its max value of 50 (restarting from 0)
    DebugCnt++;

    return 1; // success

}
// *****Debug_Dump2******
// Always record data and time on the first call to Debug_Dump2
// However, after the first call
//    Records one data and one time into the two arrays, only if the data is different from the previous call.
//    Do not record data or time if the data is the same as the data from the previous call
// Input: data is value to store in DataBuffer
// Output: 1 for success (saved or skipped), 0 for failure (buffers full)
uint32_t Debug_Dump2(uint32_t data){
// optional for Lab 3
// The software simply reads TIMG12->COUNTERREGS.CTR to get the current time in bus cycles.

  return 1; // success
}
// *****Debug_Period******
// Calculate period of the recorded data using mask
// Input: mask specifies which bit(s) to observe
// Output: period in bus cycles
// Period is defined as rising edge (low to high) to the next rising edge.
// Return 0 if there is not enough collected data to calculate period .
uint32_t Debug_Period(uint32_t mask){
// students write this for Lab 3
// This function should not alter the recorded data.
// AND each recorded data with mask,
//    if nonzero the signal is considered high.
//    if zero, the signal is considered low.


    // Declare variables here first

    uint32_t x = 1; // Index through data       CHECK THIS! 1 or 0?
    uint32_t currentData;
    uint32_t currentTime;
    uint32_t totalTime = 0;
    uint32_t transitions = 0;
    uint32_t prevData;
    uint32_t prevTime;

    while(x < DebugCnt) {

        currentData = DataBuffer[x] & mask; // Anded with mask to know if high or low
        prevData = DataBuffer[x-1] & mask;

        if(currentData != 0 && prevData == 0) {

            prevTime = currentTime;
            currentTime = TimeBuffer[x];

            if(currentTime < prevTime) {    // Because time decrements
                totalTime += prevTime-currentTime-13;   // Subtract 13 because of overhead time
                transitions++;
            }
        }
        x++;
    }

    if(transitions==0) {
        return 0;
    }

    uint32_t average = 0;
    average = totalTime/transitions;
    return average;

    // Period is time between two rising edges (A rising edge is a low to high)
    // Average period = sum of all periods / #periods

}


// *****Debug_Duty******
// Calculate duty cycle of the recorded data using mask
// Input: mask specifies which bit(s) to observe
// Output: period in percent (0 to 100)
// Period is defined as rising edge (low to high) to the next rising edge.
// High is defined as rising edge (low to high) to the next falling edge.
// Duty cycle is (100*High)/Period
// Return 0 if there is not enough collected data to calculate duty cycle.
uint32_t Debug_Duty(uint32_t mask){
// optional for Lab 3
// This function should not alter the recorded data.
// AND each recorded data with mask,
//    if nonzero the signal is considered high.
//    if zero, the signal is considered low.

       uint32_t x = 1; // Index through data       CHECK THIS! 1 or 0?
       uint32_t currentData;
       uint32_t time2;
       uint32_t totalTime = 0;
       uint32_t transitions = 0;
       uint32_t prevData;
       uint32_t time1;
       uint32_t risingEdge = 0;
       uint32_t fallingEdge = 0;

       while(x < DebugCnt) {

           currentData = DataBuffer[x] & mask; // Anded with mask to know if high or low
           prevData = DataBuffer[x-1] & mask;

           if(currentData != 0 && prevData == 0) {  // Low to High (Rising Edge)

               time1 = TimeBuffer[x];
               risingEdge=1;
           }

           if(risingEdge == 1) {

               if(currentData == 0 && prevData != 0) {  // High to Low (Falling Edge)

                             time2 = TimeBuffer[x];
                             fallingEdge = 1;
                         }
           }

           if(risingEdge == 1 && fallingEdge == 1)
           {
               if(time2 < time1)
               {
                   totalTime += time1-time2-13;
                   transitions++;
               }
               risingEdge = 0;
               fallingEdge = 0;
           }
           x++;
       }

       if(transitions==0) {
           return 0;
       }

       uint32_t high = 0;
       high = totalTime/transitions;

       uint32_t period = Debug_Period(mask);

       uint32_t dutyCycle = (100*high) / period;

       return dutyCycle;

}


// Lab2 specific debugging code
uint32_t Theperiod;
uint32_t Theduty;

void Dump(void){
  uint32_t out = GPIOB->DOUT31_0&0x0070000; // PB18-PB16 outputs
  uint32_t in = GPIOB->DIN31_0&0x0F;        // PB3-PB0 inputs
  uint32_t data = out|in;                   // PB18-PB16, PB3-PB0
  uint32_t result = Debug_Dump(data);       // calls your Lab3 function
  if(result == 0){ // 0 means full
    Theperiod = Debug_Period(1<<17);        // calls your Lab3 function
    Theduty = Debug_Duty(1<<17);    // Added this to call


//   __asm volatile("bkpt; \n"); // breakpoint here
// observe Theperiod
  }
}




