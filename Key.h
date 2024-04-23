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
    Key();

    uint16_t getX();
    uint16_t getY();
    uint16_t getWidth();
    uint16_t getHeight();
    const unsigned short* getArray();
    bool getNeedsDraw();

    void initializeKey(int16_t x, int16_t y, uint16_t width, uint16_t height, const unsigned short* keyArray);

    void moveKey(int16_t y);
    void drawKey();
    void redrawKey();
    void clearKey();
    void setKeyY(int16_t y);
    void switchToClicked();
    void switchToUnclicked();


    static const unsigned short white_key[];
    static const unsigned short black_key[];
    static const unsigned short gray_key[];
    static const unsigned short blank_white[];


    virtual ~Key();

private:
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    int16_t oldY;
    const unsigned short* keyArray;


    bool needsDraw;

    //static const unsigned short* blank_white;

};

#endif /* KEY_H_ */
