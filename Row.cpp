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



    for(uint8_t i = 0; i < 4; i++){
        uint8_t adjustedVal = (keyColors >> (3 - i)) & 0x01;
        if(adjustedVal == 1)
            keys[i].initializeKey(i * 32, rowY, 32, 30, Key::black_key);
        else
            keys[i].initializeKey(i * 32, rowY, 32, 30, Key::white_key);
    }

}

Row::Row(){

}

void Row::initializeRow(uint8_t keyColors, int16_t rowY){
    rowHeight = 30;
    onScreen = false;
    this->keyColors = keyColors;
    this->rowY = rowY;



    for(uint8_t i = 0; i < 4; i++){
        uint8_t adjustedVal = (keyColors >> (3 - i)) & 0x01;
        if(adjustedVal == 1)
            keys[i].initializeKey(i * 32, rowY, 32, 30, Key::black_key);
        else
            keys[i].initializeKey(i * 32, rowY, 32, 30, Key::white_key);
    }
}

void Row::drawRow(){
    for(uint8_t i = 0; i < 4; i++){
        keys[i].drawKey();
    }
}

void Row::moveRow(int16_t y){
    rowY += y;
    for(uint8_t i = 0; i < 4; i++){
        keys[i].moveKey(y);
    }
}

void Row::clearRow(){
    for(uint8_t i = 0; i < 4; i++){
        keys[i].clearKey();
    }
}

void Row::setRowY(int16_t rowY){
    this->rowY = rowY;
    for(uint8_t i = 0; i < 4; i++){
        keys[i].setKeyY(rowY);
    }
}

int16_t Row::getRowY(){
    return rowY;
}

Key& Row::getKey(uint8_t index){
    return keys[index];
}

void Row::setOnScreen(){
    onScreen = true;
}

void Row::setOffScreen(){
    onScreen = false;
}

bool Row::getDisplayState(){
    return onScreen;
}

Row::~Row()
{
    // TODO Auto-generated destructor stub
}

