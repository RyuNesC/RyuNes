#ifndef iNesPad_hpp
#define iNesPad_hpp
#include "iNesInstance.hpp"

#include <stdio.h>
#include <stdint.h>

enum INesPadButton {
    INesPadButtonA = 0,
    INesPadButtonB = 1,
    INesPadButtonSelect = 2,
    INesPadButtonStart = 3,
    INesPadButtonUp = 4,
    INesPadButtonDown = 5,
    INesPadButtonLeft = 6,
    INesPadButtonRight = 7
};

enum INesPadPlayer {
    INesPadPlayer1 = 0,
    INesPadPlayer2 = 1
};

struct INesPad {
    uint8_t mask[2];
    uint8_t shift[2]; // 0...7
};

uint8_t INesPadReadPort(INesPad* pad, uint16_t addr);
void INesPadWritePort(INesPad* pad, uint16_t addr, uint8_t data);


void INesPadPressButton(INesPad* pad, INesPadPlayer player, INesPadButton button);
void INesPadReleaseButton(INesPad* pad, INesPadPlayer player, INesPadButton button);
void INesPadReleaseAll(INesPad* pad);

#endif /* iNesPad_hpp */
