/*
 * Row.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Iyer_
 */

#ifndef ROW_H_
#define ROW_H_

#include "../inc/ST7735.h"
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <Key.h>


class Row
{
public:
    Row(uint8_t keyColors, int16_t rowY);
    virtual ~Row();

private:
    uint8_t keyColors; //bits will have 1=black, 0=white, 4 bits used for 4 switches
    Key* keys;
    int16_t rowY;
    uint16_t rowHeight;
    bool onScreen;


};

#endif /* ROW_H_ */