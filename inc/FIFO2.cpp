// FIFO2.cpp
// Runs on any microcontroller
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.  The file includes a transmit FIFO
// using index implementation and a receive FIFO using pointer
// implementation.  Other index or pointer implementation FIFOs can be
// created using the macros supplied at the end of the file.
// Created: 1/16/2020 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly


#include <stdint.h>
#include <stdio.h>

#include "../inc/FIFO2.h"
#include "../inc/ST7735.h"


// A class named Queue that defines a FIFO
Queue::Queue(){
  // Constructor - set PutI and GetI as 0. 
  // We are assuming that for an empty Queue, both PutI and GetI will be equal

    PutI = 0;
    GetI = 0;

// add code here to initialize on creation
}

// To check whether Queue is empty or not
bool Queue::IsEmpty(void){

  if(PutI==GetI)
  {
      return true;
  }
  return false;

}

  // To check whether Queue is full or not
bool Queue::IsFull(void){

    //return(GetI+1 == PutI);
    return (((PutI + FIFOSIZE - 1) & (FIFOSIZE-1)) == GetI);
}

  // Inserts an element in queue at rear end
bool Queue::Put(char x) {

    if(IsFull())
    {
        return false;
    }
    Buf[PutI] = x;
    PutI = ((PutI + FIFOSIZE - 1) & (FIFOSIZE-1));
    return true;



}

  // Removes an element in Queue from front end. 
bool Queue::Get(char *pt) {
    if(IsEmpty()){
        return false;
    }

    *pt = Buf[GetI];
    GetI = ((GetI + FIFOSIZE - 1) & (FIFOSIZE-1));
    return true;

}

  /* 
     Printing the elements in queue from front to rear. 
     This function is only to test the code. 
     This is not a standard function for Queue implementation. 
  */
void Queue::Print(void){
    // Finding number of elements in queue  
  // output to ST7735R


    for(int i = GetI; i!=PutI; i--)
    {
        if(i < 0)
        {
            i += FIFOSIZE;
        }
        printf("%c ", Buf[i]);
    }
    printf("\n");

}


