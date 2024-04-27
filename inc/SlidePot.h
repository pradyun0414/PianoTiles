// SlidePot.h
// Runs on MSPM0
// Provide functions that initialize ADC1 channel 5, PB18 and use a slide pot to measure distance
// Created: 12/29/2023
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#ifndef SLIDEPOT_H
#define SLIDEPOT_H
#include <stdint.h>

class SlidePot{ private:
  uint32_t data;     // raw ADC value
  int32_t flag;      // 0 if data is not valid, 1 if valid
  uint32_t distance; // distance in 0.001cm
// distance = (slope*data+offset)/4096
  uint32_t slope;    // calibration coeffients
  uint32_t offset;
public:
  SlidePot(uint32_t m, uint32_t b); // initialize slide pot
  void Init(void);                  // initialize ADC1
  uint32_t In(void);                // return last ADC sample value (0 to 4095)
  void Save(uint32_t n);            // save ADC, set semaphore
  void Sync(void);                  // wait for semaphore
  uint32_t Convert(uint32_t n);     // convert ADC raw sample to fixed-point distance, 0.001cm
  float FloatConvert(uint32_t n);   // do not use this function
  uint32_t Distance(void);          // return last distance value (0 to 2000), 0.001cm
};
#endif
