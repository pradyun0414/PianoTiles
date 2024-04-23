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


extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
extern "C" void TIMG6_IRQHandler(void);


#define B 0x0000
#define W 0x0FFF
#define T33ms 2666666




uint8_t song[] = {15, 3, 5, 0, 7, 8, 11, 14, 15, 14, 11, 5, 10, 12, 1, 10, 7, 0, 13, 8, 4, 6, 9, 4, 10, 14, 5, 5, 1, 0, 8, 7, 13, 1, 10, 4, 12, 14, 8, 1, 0, 10, 6, 10, 11, 12, 7, 6, 15, 9};
uint16_t songLength = 50;
uint16_t topRow = 0; //topRow is a later note, so higher index
uint16_t bottomRow = 0;
Row rowArray[50];
//bool needsRedraw;
uint16_t g6counter = 0;
bool switchingMode = false; //needs mode initialization if true
bool switchingMenuState = false; //needs to redraw menu when switching menu states

uint32_t startTime,stopTime, Offset, Converttime;


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

SlidePot Sensor(1500,0); // copy calibration from Lab 7


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
    for(int i = 0; i < songLength; i++){
        rowArray[i].initializeRow(song[i], 0);
    }
}

void startGameRows(){

    ST7735_FillScreen(0xFFFF);            // set screen to white to reset screen
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


    bottomRow = 0;
    topRow = 3;

    //needsRedraw = false;
}

void adjustVisible(){
    if(bottomRow >= songLength)
        return;
    if(rowArray[bottomRow].getRowY() > 140){
        rowArray[bottomRow].setOffScreen();
        rowArray[bottomRow].clearRow();
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

void TIMG6_IRQHandler(void){uint32_t pos,msg;
  if((TIMG6->CPU_INT.IIDX) == 1) { // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites
    // 4) start sounds
    // 5) set semaphore
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    g6counter++;

    if(g6counter == 1000){
        startTime = SysTick->VAL;
        moveRows(2);
        stopTime = SysTick->VAL;
        Converttime = ((startTime-stopTime)&0x0FFFFFF)-Offset; // in bus cycles
        g6counter = 0;
    }


 }
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


uint32_t lives = 3; // Decremented in FSM handler

int mainSwitch(void) {    // main switch testing


    __disable_irq();
    LaunchPad_Init();

    IOMUX->SECCFG.PINCM[PB12INDEX] = 0x00040081;     // PB12 Input Key1
    IOMUX->SECCFG.PINCM[PB17INDEX] = 0x00040081;     // PB17 Input Key2
    IOMUX->SECCFG.PINCM[PA13INDEX] = 0x00040081;     // PA13 Input Key3
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

uint32_t clickedKeys = 0;

void TIMG12_IRQHandler(void)
{
    if((TIMG12->CPU_INT.IIDX) == 1) {

        // Key 1
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
            clickedKeys+=8; // Specific Key

//            risingEdge1=0;
//            fallingEdge1=0;

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
            clickedKeys+=4; // Specific Key

//            risingEdge2=0;
//            fallingEdge2=0;

        }

        // Key 3

        uint32_t userInput3 = GPIOA->DIN31_0 & (1<<13);
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
            clickedKeys+=2; // Specific Key

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
            clickedKeys+=1; // Specific Key

//            risingEdge4=0;
//            fallingEdge4=0;

        }

    }

}

// FSM stuff

bool language = false; // false for English, true for Spanish

struct State {

    uint32_t mode;  // 0 for menu, 1 for game, 2 lastnote, 3 done
    uint32_t outputKeys;  // indicates what keys should be pressed
    uint32_t noteFrequency; // indicates the frequency of the note being played
    uint32_t next[4];

};

uint32_t stateIndex;
State FSM[50];

// FSM INDICES:
// Game next 0 for next note, 1 for english lose, 2 for spanish lose
// Last note next: 0 english lose, 1 spanish lose, 2 english win, 3 spanish win
// Menu, 0 toggle language, 1 toggle song, 2 start game
// Done, click anything means go back to menu

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

            //OUTPUT SOUND HERE (OR CALL SOMETHING THAT WILL)
        }


        if(rowArray[bottomRow].getRowY() == 140){
            if(clickedKeys != FSM[stateIndex].outputKeys)
            {
                lives--;
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
        if(clickedKeys == 4)
        {
            stateIndex = FSM[stateIndex].next[0];
            language = !language;
            switchingMenuState = true;

        }
        if(clickedKeys == 2)
        {
            stateIndex = FSM[stateIndex].next[1];
            switchingMenuState = true;
        }
        if(clickedKeys == 1)
        {
            stateIndex = FSM[stateIndex].next[2];
            switchingMode = true;
        }

        // Do nothing otherwise (stay in current state)

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


    else if(FSM[stateIndex].mode==2) // CHECK IF AT BOTTOM AGAIN
    {
        if(clickedKeys == FSM[stateIndex].outputKeys){
            for(uint8_t i = 0; i < 4; i++){
                if(rowArray[bottomRow].getKey(i).getArray() == Key::black_key){
                    rowArray[bottomRow].getKey(i).switchToClicked();
                }
            }

            //OUTPUT SOUND HERE (OR CALL SOMETHING THAT WILL)
        }

        if(rowArray[bottomRow].getRowY() == 140){
            if(clickedKeys != FSM[stateIndex].outputKeys)
            {
                lives--;
            }
            if(lives==0)
            {
                if(language==0)
                {
                    stateIndex = FSM[stateIndex].next[0];
                    switchingMode = true;
                }
                else
                {
                    stateIndex = FSM[stateIndex].next[1];
                    switchingMode = true;
                }
            }
            else
            {
                if(language==0)
                 {
                     stateIndex = FSM[stateIndex].next[2];
                     switchingMode = true;
                 }
                 else
                 {
                     stateIndex = FSM[stateIndex].next[3];
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
        if(clickedKeys!=0)
        {
            stateIndex = FSM[stateIndex].next[0];
            switchingMode = true;
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


////////////////////////////////////////////////////////////////////////////////////




int mainEdge(void) {        // Edge triggered main

    // Using PB14 as switch to test

    __disable_irq();
    LaunchPad_Init();   // Doesn't do anything with PB14 actually, I do that myself
//    ST7735_InitPrintf();
//    ST7735_FillScreen(0xFFFF);

    IOMUX->SECCFG.PINCM[PB12INDEX] = 0x00040081;     // PB12 Input

    // EdgeTriggered Initializations

//    GPIOB->POLARITY15_0 = (3<<24);  // Will set interrupt on both rising and falling edge
//    GPIOB->CPU_INT.ICLR = (1<<12);
//    GPIOB->CPU_INT.IMASK = (1<<12); // Arm PB12
//
//    NVIC->IP[0] = (NVIC->IP[0]&(~0x0000FF00)) | 2<<14; // priority=2
//    NVIC->ISER[0] = 1 << 1; // Groupl interrupt


    // To test inputs, using an LED

    IOMUX->SECCFG.PINCM[PB16INDEX] =  0x00000081;    // Output
    GPIOB->DOE31_0 |= (1<<16);  // Output Enable

    __enable_irq();

    while(1)
    {

// Testing
        uint32_t userInput = GPIOB->DIN31_0;
        userInput = userInput & (1<<12);
        if(userInput == 0)
        {
            GPIOB->DOUTCLR31_0 |= (1<<16);
        }
        else
        {
            GPIOB->DOUTSET31_0 |= (1<<16);
        }
    }

}

void GROUP1_IRQHandler(void) {

    GPIOB->DOUTTGL31_0 |= (1<<16);
    GPIOB->CPU_INT.ICLR = (1<<12);

    //moveRows(2);

    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)

}

// use main1 to observe special characters
int main(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0xFFFF);            // set screen to black

  SysTick->LOAD = 0xFFFFFF;    // max
    SysTick->VAL = 0;            // any write to current clears it
    SysTick->CTRL = 0x00000005;  // enable SysTick with core clock

    startTime = SysTick->VAL;
            stopTime = SysTick->VAL;
            Offset = (startTime-stopTime)&0x0FFFFFF; // in bus cycles

  //TimerG12_IntArm(80000000/30,2);
  //TimerG0_IntArm(40000000/30000, 1000 ,2);
  TimerG6_IntArm(2667, 1,2);




//  ST7735_DrawBitmap(0, 50, white_key, 32, 30);
//  ST7735_DrawBitmap(32, 50, white_key, 32, 30);
//  ST7735_DrawBitmap(64, 50, black_key, 32, 30);
//  ST7735_DrawBitmap(96, 50, white_key, 32, 30);
  generateRowArray();
  __enable_irq();

  while(1){

      if(FSM[stateIndex].mode == 0){
          if(switchingMode){
              ST7735_FillScreen(0xFFFF);            // set screen to white
              //clear whole screen, draw necessary sprites
              switchingMode = false;
          }
          else if(switchingMenuState){
              //replace necessary sprites
              switchingMenuState = false;
          }
      }
      if(FSM[stateIndex].mode == 1){ //initialize if switching mode, otherwise redraw keys
          if(switchingMode){
              startGameRows();
              switchingMode = false;
          }
          else{
              for(int i = bottomRow; i <= topRow; i++){
                  for(int j = 0; j < 4; j++){
                      rowArray[i].getKey(j).redrawKey();
                  }

              }
              adjustVisible();
          }
      }
      if(FSM[stateIndex].mode == 2){ //always try to redraw key and adjust
          for(int i = bottomRow; i <= topRow; i++){
              for(int j = 0; j < 4; j++){
                  rowArray[i].getKey(j).redrawKey();
              }

          }
          adjustVisible();
      }
      if(FSM[stateIndex].mode == 3){
          if(switchingMode){
              ST7735_FillScreen(0xFFFF);            // set screen to white
              //clear whole screen, draw necessary sprites
              switchingMode = false;
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

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    // write code to test switches and LEDs
   
  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main5(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    // initialize interrupts on TimerG12 at 30 Hz
  //TimerG12_IntArm(80000000/30,2);
  // initialize all data structures
  __enable_irq();

  while(1){
    // wait for semaphore
       // clear semaphore
       // update ST7735R
    // check for end game or level switch
  }
}
