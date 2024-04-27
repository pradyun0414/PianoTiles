// FIFO2.h
// Runs on any microcontroller
// Student names: put your names here
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/1/2024 


#ifndef __FIFO2_H__
#define __FIFO2_H__
#include <stdint.h>

#define FIFOSIZE 32  // maximum storage is FIFO_SIZE-1 elements 
class Queue{ 
private:
  char Buf[FIFOSIZE];
  int PutI; // index to an empty place, next place to put
  int GetI; // index to oldest data, next to get

public:
  Queue();            // initialize queue
  bool IsEmpty(void); // true if empty
  bool IsFull(void);  // true if full
  bool Put(char x);   // enter data into queue
  bool Get(char *pt); // remove data from queue
  void Print(void);   // display element of queue on LCD
};


#endif //  __FIFO2_H__
