/*
 * Row.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Iyer_
 */

#include <Row.h>
#include "../inc/ST7735.h"
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <Key.h>

Row::Row(uint8_t keyColors, int16_t rowY)
{
    // TODO Auto-generated constructor stub
    rowHeight = 30;
    onScreen = false;
    this->keyColors = keyColors;
    this->rowY = rowY;

    keys = (Key*) malloc(sizeof(Key)*4);

    for(int i = 0; i < 4; i++){
        uint8_t adjustedVal = (keyColors >> (3 - i)) & 0x01;
        if(adjustedVal)
            keys[i] = Key(i * 32, rowY, 32, 30, Key::black_key);
        else
            keys[i] = Key(i * 32, rowY, 32, 30, Key::white_key);
    }

}

Row::~Row()
{
    // TODO Auto-generated destructor stub
}

