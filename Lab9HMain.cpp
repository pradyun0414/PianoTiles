// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Your name
// Last Modified: 1/1/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
#include "Key.h"
#include "Row.h"
#include "Sprite.h"


extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
extern "C" void TIMG6_IRQHandler(void);
extern "C" void SysTick_Handler(void);

void FSM_Handler();

#define B 0x0000
#define W 0x0FFF
#define T33ms 2666666
#define Tchinesems 26666666

#define PB0INDEX  11 // UART0_TX  SPI1_CS2  TIMA1_C0  TIMA0_C2
#define PB1INDEX  12 // UART0_RX  SPI1_CS3  TIMA1_C1  TIMA0_C2N
#define PB2INDEX  14 // UART3_TX  UART2_CTS I2C1_SCL  TIMA0_C3  UART1_CTS TIMG6_C0  TIMA1_C0
#define PB3INDEX  15 // UART3_RX  UART2_RTS I2C1_SDA  TIMA0_C3N UART1_RTS TIMG6_C1  TIMA1_C1
#define PB4INDEX  16 // UART1_TX  UART3_CTS TIMA1_C0  TIMA0_C2  TIMA1_C0N


//////////////////////////////////////////////////////////////

// SOUND STUFF IS HERE

// Twinkle Twinkle Notes
#define C4  9556   // 261.6 Hz
#define G4  6378   // 392 Hz
#define A4 5682   // 440 Hz
#define F4  7159   // 349.2 Hz
#define E4  7584   // 329.6 Hz
#define D4  8513   // 293.7 Hz

// Happy Birthday - uses same notes as above plus a few more (C5 and Bb4)

#define Bflat4 5062 // 493.88 Hz
#define C5 4778 // 523.25 Hz

// SlidePot - ADC Stuff

SlidePot Sensor(1832,176); // Calibrated
uint32_t Reading;        // 12-bit ADC
uint32_t ADCflag = 0;


////////////////////////////////////////////////////////////////

uint32_t lives = 3; // Decremented in FSM handler

// FSM stuff

uint32_t language = 0; // false for English, true for Spanish
uint32_t china = 0;
uint32_t stateIndex = 0;
uint32_t clickedKeys = 0;


struct State {

    uint32_t mode;  // 0 for menu, 1 for game, 2 lastnote, 3 done
    uint32_t outputKeys;  // indicates what keys should be pressed
    uint32_t noteFrequency; // indicates the frequency of the note being played
    uint32_t next[4];

};


const State FSM[91] =         // CHANGE FSM SIZE!!!
{

     {0, 0, 0, {1, 2, 56, 0}},    // English Menu Song 1           0
     {0, 0, 0, {0, 3, 56, 1}},    // Spanish Menu Song 1           1         MAKE SURE THE 8 IS THE FIRST INDEX OF SONG 1 (56 NOW)
     {0, 0, 0, {3, 0, 59, 2}},    // English Menu Song 2           2         REPLACE THE 40 WITH STARTING INDEX OF SONG 2
     {0, 0, 0, {2, 1, 59, 3}},    // Spanish Menu Song 2           3
     {3, 0, 0, {0, 0, 0, 0}},     // English Lose                  4
     {3, 0, 0, {1, 1, 1, 1}},     // Spanish Lose                  5
     {3, 0, 0, {0, 0, 0, 0}},     // English Win                   6
     {3, 0, 0, {1, 1, 1, 1}},     // Spanish Win                   7

     // For empty space, make sure 0 for frequency works. If not, add a conditional where it just doesn't call the SoundStart method if frequency is 0

     {1, 8, C4, {9, 4, 5, 0}},               // First state of Song 1                    8
     {1, 8, C4, {10, 4, 5, 0}},              //                                          9
     {1, 2, G4, {11, 4, 5, 0}},              //                                          10
     {1, 2, G4, {12, 4, 5, 0}},              //                                          11
     {1, 1, A4, {13, 4, 5, 0}},              //                                          12
     {1, 1, A4, {14, 4, 5, 0}},              //                                          13
     {1, 10, G4, {15, 4, 5, 0}},             //                                          14
     {1, 0, 1, {16, 4, 5, 0}},               //                                          15

     {1, 1, F4, {17, 4, 5, 0}},              //                                          16
     {1, 1, F4, {18, 4, 5, 0}},              //                                          17
     {1, 2, E4, {19, 4, 5, 0}},              //                                          18
     {1, 2, E4, {20, 4, 5, 0}},              //                                          19
     {1, 4, D4, {21, 4, 5, 0}},              //                                          20
     {1, 4, D4, {22, 4, 5, 0}},              //                                          21
     {1, 10, C4, {23, 4, 5, 0}},             //                                          22
     {1, 0, 1, {24, 4, 5, 0}},               //                                         23

     {1, 2, G4, {25, 4, 5, 0}},              //                                          24
     {1, 2, G4, {26, 4, 5, 0}},              //                                          25
     {1, 4, F4, {27, 4, 5, 0}},              //                                          26
     {1, 4, F4, {28, 4, 5, 0}},              //                                          27
     {1, 8, E4, {29, 4, 5, 0}},              //                                          28
     {1, 8, E4, {30, 4, 5, 0}},              //                                          29
     {1, 5, D4, {31, 4, 5, 0}},              //                                          30
     {1, 0, 1, {32, 4, 5, 0}},               //                                          31

     {1, 2, G4, {33, 4, 5, 0}},              //                                          32
     {1, 2, G4, {34, 4, 5, 0}},              //                                          33
     {1, 4, F4, {35, 4, 5, 0}},              //                                          34
     {1, 4, F4, {36, 4, 5, 0}},              //                                          35
     {1, 8, E4, {37, 4, 5, 0}},              //                                          36
     {1, 8, E4, {38, 4, 5, 0}},              //                                          37
     {1, 5, D4, {39, 4, 5, 0}},              //                                          38
     {1, 0, 1, {40, 4, 5, 0}},               //                                          39

     {1, 8, C4, {41, 4, 5, 0}},              //                                          40
     {1, 8, C4, {42, 4, 5, 0}},              //                                          41
     {1, 2, G4, {43, 4, 5, 0}},              //                                          42
     {1, 2, G4, {44, 4, 5, 0}},              //                                          43
     {1, 1, A4, {45, 4, 5, 0}},              //                                          44
     {1, 1, A4, {46, 4, 5, 0}},              //                                          45
     {1, 10, G4, {47, 4, 5, 0}},             //                                          46
     {1, 0, 1, {48, 4, 5, 0}},               //                                          47

     {1, 1, F4, {49, 4, 5, 0}},              //                                          48
     {1, 1, F4, {50, 4, 5, 0}},              //                                          49
     {1, 2, E4, {51, 4, 5, 0}},              //                                          50
     {1, 2, E4, {52, 4, 5, 0}},              //                                          51
     {1, 4, D4, {53, 4, 5, 0}},              //                                          52
     {1, 4, D4, {54, 4, 5, 0}},              //                                          53
     {1, 15, C4, {55, 4, 5, 0}},             //                                          54
     {2, 0, 1, {4, 5, 6, 7}},                // LastNote1                                55

     {1, 0, 1, {57, 4, 5, 0}},               //  EMPTY START BUFFER! SONG STARTS HERE    56
     {1, 0, 1, {58, 4, 5, 0}},               //                                          57
     {1, 0, 1, {8, 4, 5, 0}},                //                                          58

//////////////////////////////////////////////////////////////////////////////////////////////////////

     {1, 0, 1, {60, 4, 5, 0}},               //  First state of Song 2 (Empty)           59
     {1, 0, 1, {61, 4, 5, 0}},               //  (Empty)                                 60
     {1, 0, 1, {62, 4, 5, 0}},                //  (Empty)                                 61

     {1, 8, C4, {63, 4, 5, 0}},              //                                          62
     {1, 8, C4, {64, 4, 5, 0}},              //                                          63
     {1, 4, D4, {65, 4, 5, 0}},              //                                          64
     {1, 8, C4, {66, 4, 5, 0}},              //                                          65
     {1, 2, F4, {67, 4, 5, 0}},              //                                          66
     {1, 10, E4, {68, 4, 5, 0}},              //                                          67
     {1, 0, 1, {69, 4, 5, 0}},               //                                          68

     {1, 8, C4, {70, 4, 5, 0}},              //                                          69
     {1, 8, C4, {71, 4, 5, 0}},              //                                          70
     {1, 4, D4, {72, 4, 5, 0}},              //                                          71
     {1, 8, C4, {73, 4, 5, 0}},              //                                          72
     {1, 1, G4, {74, 4, 5, 0}},              //                                          73
     {1, 5, F4, {75, 4, 5, 0}},              //                                          74
     {1, 0, 1, {76, 4, 5, 0}},               //                                          75

     {1, 8, C4, {77, 4, 5, 0}},              //                                          76
     {1, 8, C4, {78, 4, 5, 0}},              //                                          77
     {1, 1, C5, {79, 4, 5, 0}},              //                                          78
     {1, 2, A4, {80, 4, 5, 0}},              //                                          79
     {1, 4, F4, {81, 4, 5, 0}},              //                                          80
     {1, 8, E4, {82, 4, 5, 0}},              //                                          81
     {1, 9, D4, {83, 4, 5, 0}},              //                                          82
     {1, 0, 1, {84, 4, 5, 0}},               //                                          83

     {1, 1, Bflat4, {85, 4, 5, 0}},          //                                          84
     {1, 1, Bflat4, {86, 4, 5, 0}},          //                                          85
     {1, 2, A4, {87, 4, 5, 0}},              //                                          86
     {1, 8, F4, {88, 4, 5, 0}},              //                                          87
     {1, 4, G4, {89, 4, 5, 0}},              //                                          88
     {1, 11, F4, {90, 4, 5, 0}},             //                                          89
     {2, 0, 1, {4, 5, 6, 7}},                //   Last Note Song 2                       90


};

// FSM INDICES:
// Game next 0 for next note, 1 for english lose, 2 for spanish lose
// Last note next: 0 english lose, 1 spanish lose, 2 english win, 3 spanish win
// Menu, 0 toggle language, 1 toggle song, 2 start game
// Done, click anything means go back to menu


/// Song Inits: /////////////////////////////////////////////////////////////

uint8_t song1[] = {0, 0, 0, 8, 8, 2, 2, 1, 1, 10, 0, 1, 1, 2, 2, 4, 4, 10, 0, 2, 2, 4, 4, 8, 8, 5, 0, 2, 2, 4, 4, 8, 8, 5, 0, 8, 8, 2, 2, 1, 1, 10, 0, 1, 1, 2, 2, 4, 4, 15, 0};
uint16_t songLength1 = 51;
Row rowArray1[51];

uint8_t song2[] = {0, 0, 0, 8, 8, 4, 8, 2, 10, 0, 8, 8, 4, 8, 1, 5, 0, 8, 8, 1, 2, 4, 8, 9, 0, 1, 1, 2, 8, 4, 11, 0};
uint16_t songLength2 = 32;
Row rowArray2[32];

uint8_t* song;
uint16_t songLength = 51;
Row* rowArray;

uint16_t topRow = 0; //topRow is a later note, so higher index
uint16_t bottomRow = 0;

////////////////////////////////////////////////////////////////

//bool needsRedraw;
uint16_t g6counter = 0;
bool switchingMode = true; //needs mode initialization if true
bool switchingMenuState = false; //needs to redraw menu when switching menu states

uint32_t startTime,stopTime, Offset, Converttime;

Sprite sprite1;


// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}


uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};


void generateNewRow(){

}

void generateRowArray(){
    for(int i = 0; i < songLength1; i++){
        rowArray1[i].initializeRow(song1[i], 0);
    }
    for(int i = 0; i < songLength2; i++){
        rowArray2[i].initializeRow(song2[i], 0);
    }

}

void startGameRows(){

    bottomRow = 0;
    topRow = 3;
    lives = 3;

    ST7735_FillScreen(0xFFFF);            // set screen to white to reset screen

    for(uint8_t j = 0; j < songLength; j++){
        for(uint8_t i = 0; i < 4; i++){
            if(rowArray[j].getKey(i).getArray() == Key::gray_key){
                rowArray[j].getKey(i).switchToUnclicked();
            }
        }
    }


    rowArray[0].setOnScreen();
    rowArray[0].setRowY(110);
    rowArray[0].drawRow();

    rowArray[1].setOnScreen();
    rowArray[1].setRowY(80);
    rowArray[1].drawRow();


    rowArray[2].setOnScreen();
    rowArray[2].setRowY(50);
    rowArray[2].drawRow();

    rowArray[3].setOnScreen();
    rowArray[3].setRowY(20);
    rowArray[3].drawRow();




    //needsRedraw = false;
}

void adjustVisible(){
    if(bottomRow >= songLength)
        return;
//    if(topRow == songLength - 1 && rowArray[topRow].getRowY() > 20 && rowArray[topRow].getRowY() < 140){//last note is visible
//        ST7735_DrawFastHLine(0, rowArray[topRow].getRowY() - 2, 128, 0xFFFF);
//        ST7735_DrawFastHLine(0, rowArray[topRow].getRowY() - 1, 128, 0xFFFF);
//    }
    if(rowArray[bottomRow].getRowY() > 140){
        rowArray[bottomRow].setOffScreen();
        //rowArray[bottomRow].clearRow();
        bottomRow++;
    }

    if(rowArray[topRow].getRowY() > 20 && topRow < songLength){
        rowArray[topRow + 1].setOnScreen();
        rowArray[topRow + 1].setRowY(rowArray[topRow].getRowY() - 30);
        topRow++;
        rowArray[topRow].drawRow();
    }
}

void moveRows(int16_t y){
//    for(uint8_t i = 0; i < songLength; i++){
//        if(rowArray[i].getDisplayState()){
//            rowArray[i].moveRow(y);
//        }
//    }
    for(int i = bottomRow; i <= topRow; i++){
        rowArray[i].moveRow(y);
    }

    //adjustVisible();

    //needsRedraw = true;
}

// games  engine runs at 30Hz

void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1) { // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites
    // 4) start sounds
    // 5) set semaphore
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    //GPIOB->DOUTTGL31_0 |= (1<<16);

    //startTime = SysTick->VAL;
    if(FSM[stateIndex].mode==1 || FSM[stateIndex].mode==2){
        moveRows(2);
    }
    //stopTime = SysTick->VAL;
    //Converttime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
    FSM_Handler();



 }
}

void SysTick_Handler(void){ // called at 11 kHz

    const uint8_t wave[32] = {16,19,22,24,27,28,30,31,31,31,30,28,27,24,22,19,16,13,10,8,5,4,2,1,1,1,2,4,5,8,10,13};
    static uint32_t i=0;
    DAC5_Out((wave[i]*Data)/4095);
    i = (i+1) & 0x1F; // Cycles

}

//uint8_t song[] = {15, 3, 5, 0, 7, 8, 11, 14, 15, 14, 11, 5, 10, 12, 1, 10, 7, 0, 13, 8, 4, 6, 9, 4, 10, 14, 5, 5, 1, 0, 8, 7, 13, 1, 10, 4, 12, 14, 8, 1, 0, 10, 6, 10, 11, 12, 7, 6, 15, 9};
//uint16_t songLength = 50;
//uint16_t topRow = 0; //topRow is a later note, so higher index
//uint16_t bottomRow = 0;
//Row rowArray[50];


// Globals
uint32_t risingEdge1 = 0;    // Used in Timer Handler
uint32_t fallingEdge1 = 0;
uint32_t risingEdge2 = 0;
uint32_t fallingEdge2 = 0;
uint32_t risingEdge3 = 0;
uint32_t fallingEdge3 = 0;
uint32_t risingEdge4 = 0;
uint32_t fallingEdge4 = 0;



int mainSwitch(void) {    // main switch testing


    __disable_irq();
    LaunchPad_Init();

    IOMUX->SECCFG.PINCM[PB12INDEX] = 0x00040081;     // PB12 Input Key1
    IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081;     // PB17 Input Key2
    IOMUX->SECCFG.PINCM[PA31INDEX] = 0x00040081;     // PA31 Input Key3
    IOMUX->SECCFG.PINCM[PA12INDEX] = 0x00040081;     // PA12 Input Key4


    IOMUX->SECCFG.PINCM[PB16INDEX] =  0x00000081;    // Output Key1
    GPIOB->DOE31_0 |= (1<<16);  // Output Enable
    IOMUX->SECCFG.PINCM[PA25INDEX] =  0x00000081;    // Output Key2
    GPIOA->DOE31_0 |= (1<<25);  // Output Enable
    IOMUX->SECCFG.PINCM[PA26INDEX] =  0x00000081;    // Output Key3
    GPIOA->DOE31_0 |= (1<<26);  // Output Enable
    IOMUX->SECCFG.PINCM[PA27INDEX] =  0x00000081;    // Output Key4
    GPIOA->DOE31_0 |= (1<<27);  // Output Enable

    //TimerG12_IntArm(80000000/100, 1);   // Potentially look into the frequency later
    __enable_irq();
    while(1){}

}


void TIMG6_IRQHandler(void)
{
    if((TIMG6->CPU_INT.IIDX) == 1) {

        g6counter++;
        if(g6counter == 40)
        {
            g6counter = 0;
            //GPIOB->DOUTTGL31_0 |= (1<<16);
            // Key 1

                    Reading = Sensor.In();
                    Sensor.Save(Reading);   // Updates the data and also sets flag within the sensor class
                    ADCflag = 1;

                    uint32_t userInput1 = GPIOB->DIN31_0 & (1<<12);
                    if(userInput1!=0 && risingEdge1 == 0)
                    {
                        risingEdge1=1;
                    }
                    if(userInput1==0 && risingEdge1 ==1)
                    {
                        fallingEdge1=1;
                    }
                    if(risingEdge1==1 && fallingEdge1==1)
                    {
                        GPIOB->DOUTTGL31_0 |= (1<<16);
                        clickedKeys|=8; // Specific Key

//                        risingEdge1=0;
//                        fallingEdge1=0;

                    }

                    // Key 2

                    uint32_t userInput2 = GPIOB->DIN31_0 & (1<<17);
                    if(userInput2!=0 && risingEdge2 == 0)
                    {
                        risingEdge2=1;
                    }
                    if(userInput2==0 && risingEdge2 ==1)
                    {
                        fallingEdge2=1;
                    }
                    if(risingEdge2==1 && fallingEdge2==1)
                    {
                        GPIOA->DOUTTGL31_0 |= (1<<25);
                        clickedKeys|=4; // Specific Key

            //            risingEdge2=0;
            //            fallingEdge2=0;

                    }

                    // Key 3

                    uint32_t userInput3 = GPIOA->DIN31_0 & (1<<31);
                    if(userInput3!=0 && risingEdge3 == 0)
                    {
                        risingEdge3=1;
                    }
                    if(userInput3==0 && risingEdge3 ==1)
                    {
                        fallingEdge3=1;
                    }
                    if(risingEdge3==1 && fallingEdge3==1)
                    {
                        GPIOA->DOUTTGL31_0 |= (1<<26);
                        clickedKeys|=2; // Specific Key

            //            risingEdge3=0;
            //            fallingEdge3=0;

                    }


                    // Key 4

                    uint32_t userInput4 = GPIOA->DIN31_0 & (1<<12);
                    if(userInput4!=0 && risingEdge4 == 0)
                    {
                        risingEdge4=1;
                    }
                    if(userInput4==0 && risingEdge4 ==1)
                    {
                        fallingEdge4=1;
                    }
                    if(risingEdge4==1 && fallingEdge4==1)
                    {
                        GPIOA->DOUTTGL31_0 |= (1<<27);
                        clickedKeys|=1; // Specific Key

            //            risingEdge4=0;
            //            fallingEdge4=0;

                    }
        }
    }
}


// Will control the state, gets called each time the "row reached bottom" semaphore is set (indicating the player should have clicked the right keys by now)
void FSM_Handler() {

    if(FSM[stateIndex].mode==1) // MAKE SURE TO ADD CONDITIONAL TO CHECK IF AT BOTTOM !!!!!!!!
    {
        if(clickedKeys == FSM[stateIndex].outputKeys){
            for(uint8_t i = 0; i < 4; i++){
                if(rowArray[bottomRow].getKey(i).getArray() == Key::black_key){
                    rowArray[bottomRow].getKey(i).switchToClicked();
                }
            }

            if(FSM[stateIndex].noteFrequency != 1){
                Sound_Start(FSM[stateIndex].noteFrequency);
            }
            //OUTPUT SOUND HERE (OR CALL SOMETHING THAT WILL)
        }


        if(rowArray[bottomRow].getRowY() == 140){
            if(clickedKeys != FSM[stateIndex].outputKeys)
            {
                lives--;
            }
            else{
//                Sound_Start(FSM[stateIndex].noteFrequency);
//                Sound_Stop();
            }

            if(lives==0)
            {
                if(language==0)
                {
                    stateIndex = FSM[stateIndex].next[1];
                    switchingMode = true;
                }
                else
                {
                    stateIndex = FSM[stateIndex].next[2];
                    switchingMode = true;
                }
            }
            else
            {
                stateIndex = FSM[stateIndex].next[0];
            }

            risingEdge1=0;
            fallingEdge1=0;
            risingEdge2=0;
            fallingEdge2=0;
            risingEdge3=0;
            fallingEdge3=0;
            risingEdge4=0;
            fallingEdge4=0;

            clickedKeys = 0;    // Reset to 0 because done
        }

    }

    else if(FSM[stateIndex].mode==0)
    {
        Sound_Start(1);
        if(clickedKeys == 4)
        {
            stateIndex = FSM[stateIndex].next[0];
            if(language == 0)
                language = 1;
            else if(language==1)
            {
                language = 0;
            }
            //language = !language;
            switchingMenuState = true;

        }
        else if(clickedKeys == 2)
        {
            stateIndex = FSM[stateIndex].next[1];
            switchingMenuState = true;

            if(song == song2){
                song = song1;
                rowArray = rowArray1;
                songLength = songLength1;
            }
            else if(song == song1){
                song = song2;
                rowArray = rowArray2;
                songLength = songLength2;
            }

        }
        else if(clickedKeys == 1)
        {
            stateIndex = FSM[stateIndex].next[2];
            switchingMode = true;
        }

        // Do nothing otherwise (stay in current state)
        if(risingEdge1 && fallingEdge1){
            risingEdge1=0;
            fallingEdge1=0;
            clickedKeys = 0;
        }
        if(risingEdge2 && fallingEdge2){
            risingEdge2=0;
            fallingEdge2=0;
            clickedKeys = 0;
        }
        if(risingEdge3 && fallingEdge3){
            risingEdge3=0;
            fallingEdge3=0;
            clickedKeys = 0;
        }
        if(risingEdge4 && fallingEdge4){
            risingEdge4=0;
            fallingEdge4=0;
            clickedKeys = 0;
        }
//        risingEdge1=0;
//        fallingEdge1=0;
//        risingEdge2=0;
//        fallingEdge2=0;
//        risingEdge3=0;
//        fallingEdge3=0;
//        risingEdge4=0;
//        fallingEdge4=0;

//        clickedKeys = 0;    // Reset to 0 because done
    }

    else if(FSM[stateIndex].mode==2) // MAKE SURE TO ADD CONDITIONAL TO CHECK IF AT BOTTOM !!!!!!!!
    {
        switchingMode = false;
        uint32_t curstate = stateIndex;
        if(clickedKeys == FSM[stateIndex].outputKeys){
            for(uint8_t i = 0; i < 4; i++){
                if(rowArray[bottomRow].getKey(i).getArray() == Key::black_key){
                    rowArray[bottomRow].getKey(i).switchToClicked();
                }
            }

            //OUTPUT SOUND HERE (OR CALL SOMETHING THAT WILL)

            if(FSM[stateIndex].noteFrequency != 1){
                Sound_Start(FSM[stateIndex].noteFrequency);
            }
        }


        if(rowArray[bottomRow].getRowY() == 140){
            if(clickedKeys != FSM[curstate].outputKeys)
            {
                lives--;
            }
            else{
//                Sound_Start(FSM[stateIndex].noteFrequency);
//                Sound_Stop();

            }

            if(lives==0)
            {
                if(language==0)
                {
                    stateIndex = FSM[curstate].next[0];
                    switchingMode = true;
                }
                else
                {
                    stateIndex = FSM[curstate].next[1];
                    switchingMode = true;
                }
            }
            else
            {
                if(language==0)
                 {
                     stateIndex = FSM[curstate].next[2];
                     switchingMode = true;
                 }
                 else
                 {
                     stateIndex = FSM[curstate].next[3];
                     switchingMode = true;
                 }
            }

            risingEdge1=0;
            fallingEdge1=0;
            risingEdge2=0;
            fallingEdge2=0;
            risingEdge3=0;
            fallingEdge3=0;
            risingEdge4=0;
            fallingEdge4=0;

            clickedKeys = 0;    // Reset to 0 because done
        }

    }


    else if(FSM[stateIndex].mode==3)
    {
        Sound_Start(1);
        if(clickedKeys!=0)
        {
            stateIndex = FSM[stateIndex].next[0];
            switchingMode = true;
                risingEdge1=0;
                fallingEdge1=0;
                risingEdge2=0;
                fallingEdge2=0;
                risingEdge3=0;
                fallingEdge3=0;
                risingEdge4=0;
                fallingEdge4=0;

                clickedKeys = 0;    // Reset to 0 because done
        }

        if(risingEdge1 && fallingEdge1){
            risingEdge1=0;
            fallingEdge1=0;
            clickedKeys = 0;
        }
        if(risingEdge2 && fallingEdge2){
            risingEdge2=0;
            fallingEdge2=0;
            clickedKeys = 0;
        }
        if(risingEdge3 && fallingEdge3){
            risingEdge3=0;
            fallingEdge3=0;
            clickedKeys = 0;
        }
        if(risingEdge4 && fallingEdge4){
            risingEdge4=0;
            fallingEdge4=0;
            clickedKeys = 0;
        }
//        risingEdge1=0;
//        fallingEdge1=0;
//        risingEdge2=0;
//        fallingEdge2=0;
//        risingEdge3=0;
//        fallingEdge3=0;
//        risingEdge4=0;
//        fallingEdge4=0;
//
//        clickedKeys = 0;    // Reset to 0 because done
    }

//    risingEdge1=0;
//    fallingEdge1=0;
//    risingEdge2=0;
//    fallingEdge2=0;
//    risingEdge3=0;
//    fallingEdge3=0;
//    risingEdge4=0;
//    fallingEdge4=0;
//
//    clickedKeys = 0;    // Reset to 0 because done




}

////////////////////////////////////////////////////////////////////////////////////

int mainchina(void){    //mainchina
    //char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  DAC5_Init();
  Sound_Init(1,1);
  ST7735_InitPrintf();
  ST7735_FillScreen(0xFFFF);
  __enable_irq();

  while(1){
      Sound_Start(9956);
      Clock_Delay1ms(1000);
//      Clock_Delay(Tchinesems);
//      Sound_Stop();
//      Clock_Delay(Tchinesems);

  }
}

// use main1 to observe special characters
int main(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  DAC5_Init();
  Sound_Init(1,1);
  Sensor.Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0xFFFF);            // set screen to black

//  SysTick->LOAD = 0xFFFFFF;    // max
//  SysTick->VAL = 0;            // any write to current clears it
//  SysTick->CTRL = 0x00000005;  // enable SysTick with core clock
//
//  startTime = SysTick->VAL;
//  stopTime = SysTick->VAL;
//  Offset = (startTime-stopTime)&0x0FFFFFF; // in bus cycles

  TimerG12_IntArm(80000000/30,1);
  //TimerG0_IntArm(40000000/30000, 1000 ,2);
//  TimerG6_IntArm(2667, 1,2);
  TimerG6_IntArm(20000,1,2);

  IOMUX->SECCFG.PINCM[PB12INDEX] = 0x00040081;     // PB12 Input Key1
  IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081;     // PB17 Input Key2
  IOMUX->SECCFG.PINCM[PA31INDEX] = 0x00040081;     // PA31 Input Key3
  IOMUX->SECCFG.PINCM[PA12INDEX] = 0x00040081;     // PA12 Input Key4


  IOMUX->SECCFG.PINCM[PB16INDEX] =  0x00000081;    // Output Key1
  GPIOB->DOE31_0 |= (1<<16);  // Output Enable
  IOMUX->SECCFG.PINCM[PA25INDEX] =  0x00000081;    // Output Key2
  GPIOA->DOE31_0 |= (1<<25);  // Output Enable
  IOMUX->SECCFG.PINCM[PA26INDEX] =  0x00000081;    // Output Key3
  GPIOA->DOE31_0 |= (1<<26);  // Output Enable
  IOMUX->SECCFG.PINCM[PA27INDEX] =  0x00000081;    // Output Key4
  GPIOA->DOE31_0 |= (1<<27);  // Output Enable


//  ST7735_DrawBitmap(0, 50, white_key, 32, 30);
//  ST7735_DrawBitmap(32, 50, white_key, 32, 30);
//  ST7735_DrawBitmap(64, 50, black_key, 32, 30);
//  ST7735_DrawBitmap(96, 50, white_key, 32, 30);
  generateRowArray();
  __enable_irq();

  while(1){

      if(FSM[stateIndex].mode == 0){
          if(switchingMode){
              switchingMode = false;
              uint32_t curstate = stateIndex;

              song = song1;
              rowArray = rowArray1;
              songLength = songLength1;

              ST7735_FillScreen(0xbebf);            // set screen to white
              //ST7735_FillScreen(0xFFFF);
              //clear whole screen, draw necessary sprites
              ST7735_DrawBitmap(11, 30, Sprite::PianoTilesTitle, 106, 20);
              ST7735_DrawBitmap(44, 145, Sprite::PlayButton, 40, 20);


              if(curstate == 0){
                  ST7735_DrawBitmap(44, 65, Sprite::EnglishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song1English, 40, 20);

              }
              if(curstate == 1){
                  ST7735_DrawBitmap(44, 65, Sprite::SpanishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song1Spanish, 40, 20);
              }
              if(curstate == 2){
                  ST7735_DrawBitmap(44, 65, Sprite::EnglishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song2English, 40, 20);
              }
              if(curstate == 3){
                  ST7735_DrawBitmap(44, 65, Sprite::SpanishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song2Spanish, 40, 20);
              }
          }
          else if(switchingMenuState){
              //replace necessary sprites
              switchingMenuState = false;
              uint32_t curstate = stateIndex;
              if(curstate == 0){
                  ST7735_DrawBitmap(44, 65, Sprite::EnglishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song1English, 40, 20);

              }
              if(curstate == 1){
                  ST7735_DrawBitmap(44, 65, Sprite::SpanishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song1Spanish, 40, 20);
              }
              if(curstate == 2){
                  ST7735_DrawBitmap(44, 65, Sprite::EnglishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song2English, 40, 20);
              }
              if(curstate == 3){
                  ST7735_DrawBitmap(44, 65, Sprite::SpanishButton, 40, 20);
                  ST7735_DrawBitmap(44, 100, Sprite::Song2Spanish, 40, 20);
              }
          }
      }
      else if(FSM[stateIndex].mode == 1){ //initialize if switching mode, otherwise redraw keys
          if(switchingMode){
              switchingMode = false;
              startGameRows();
              ST7735_DrawBitmap(0, 160, Sprite::BottomBlock, 128, 20);
              ST7735_DrawBitmap(0, 20, Sprite::TopBlock, 128, 20);

          }
          else{

              for(int i = bottomRow; i <= topRow; i++){
                  for(int j = 0; j < 4; j++){
                      rowArray[i].getKey(j).redrawKey();
                  }
                  if(i == bottomRow){
                      //ST7735_DrawBitmap(0, 160, Sprite::BottomBlock, 128, 20);
                  }
                  if(i == topRow){
                     // ST7735_DrawBitmap(0, 20, Sprite::TopBlock, 128, 20);
                  }

              }
              for(int i = 3; i > 0; i--){
                  if(lives < i){
                      ST7735_DrawBitmap(128 - i * 10, 10, Sprite::EmptyHeart, 8, 8);
                  }
                  else{
                      ST7735_DrawBitmap(128 - i * 10, 10, Sprite::Heart, 8, 8);
                  }
              }

              adjustVisible();
          }
      }
      else if(FSM[stateIndex].mode == 2){ //always try to redraw key and adjust
          switchingMode = false;
          for(int i = bottomRow; i <= topRow; i++){
              for(int j = 0; j < 4; j++){
                  rowArray[i].getKey(j).redrawKey();
              }

          }
          //ST7735_DrawBitmap(0, 20, Sprite::TopBlock, 128, 20);
          //ST7735_DrawBitmap(0, 160, Sprite::BottomBlock, 128, 20);
          for(int i = 3; i > 0; i--){
              if(lives < i){
                  ST7735_DrawBitmap(128 - i * 10, 10, Sprite::EmptyHeart, 8, 8);
              }
              else{
                  ST7735_DrawBitmap(128 - i * 10, 10, Sprite::Heart, 8, 8);
              }
          }
          adjustVisible();
      }
      else if(FSM[stateIndex].mode == 3){
          if(switchingMode){
              switchingMode = false;
              uint32_t curstate = stateIndex;
              ST7735_FillScreen(0xFFFF);            // set screen to white
              //ST7735_SetCursor(0,0);

              if(curstate == 4 || curstate == 5){
                  //char str[] = "You lose";
                  ST7735_DrawBitmap(50, 50, Sprite::Heart, 8, 8);

              }
              else{
                  //char str[] = "You win";
                  ST7735_DrawBitmap(50, 50, Sprite::EmptyHeart, 8, 8);
              }
              //clear whole screen, draw necessary sprites
          }
      }


  }


      //moveRows(2);
      //Clock_Delay(T33ms) ;
//      Row tempRow = Row(11, 20);
//      tempRow.drawRow();

//      for(int i = 0; i < 50; i++){
//          Clock_Delay(T33ms) ;
//          tempRow.moveRow(2);
//      }
//      tempRow.clearRow();


//  Row tempRow = Row(11, 20);
//  tempRow.drawRow();
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);
//  Clock_Delay(T33ms) ;
//  tempRow.moveRow(0, 2);

  //  Key tempKey = Key(0, -12, 32, 30, Key::white_key);
  //  tempKey.drawKey();
  //  while(1){
  //      Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, 10);
  //        Clock_Delay(T33ms) ;
  //        tempKey.moveKey(0, -90);
  //  }


//  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
//    for(int myL=0; myL<= 3; myL++){
//         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
//      ST7735_OutChar(' ');
//         ST7735_OutString((char *)Phrases[myPhrase][myL]);
//      ST7735_OutChar(13);
//    }
//  }
//  Clock_Delay1ms(3000);
//  ST7735_FillScreen(0x0000);       // set screen to black
//  l = 128;
//  while(1){
//    Clock_Delay1ms(2000);
//    for(int j=0; j < 3; j++){
//      for(int i=0;i<16;i++){
//        ST7735_SetCursor(7*j+0,i);
//        ST7735_OutUDec(l);
//        ST7735_OutChar(' ');
//        ST7735_OutChar(' ');
//        ST7735_SetCursor(7*j+4,i);
//        ST7735_OutChar(l);
//        l++;
//      }
//    }
//  }
}

void moveKey(){

}

