/*
 * Key.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Iyer_
 */

#ifndef KEY_H_
#define KEY_H_
#include "../inc/ST7735.h"
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>


class Key
{
public:
    Key(int16_t x, int16_t y, uint16_t width, uint16_t height, const unsigned short* keyArray);


    uint16_t getX();
    uint16_t getY();
    uint16_t getWidth();
    uint16_t getHeight();
    short* getArray();

    void moveKey(int16_t x, int16_t y);
    void drawKey();
    static const unsigned short white_key[];
    static const unsigned short black_key[];
    static const unsigned short blank_white[];


    virtual ~Key();

private:
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    const unsigned short* keyArray;

    //static const unsigned short* blank_white;

    void clearKey();
};

#endif /* KEY_H_ */