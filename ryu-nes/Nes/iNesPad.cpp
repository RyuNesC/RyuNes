#include "iNesPad.hpp"
#include <assert.h>
#include <string.h>

uint8_t INesPadReadPort(INesPad* pad, uint16_t addr) {
    assert(addr == 0x4016 || addr == 0x4017);
    uint8_t player = addr - 0x4016;
    uint8_t shift = pad->shift[player];
    pad->shift[player] = (pad->shift[player] + 1) & 7;
    uint8_t i = (pad->mask[player] >> shift) & 1;
    return i;
}

void INesPadWritePort(INesPad* pad, uint16_t addr, uint8_t data) {
    assert(addr == 0x4016);
    if (data & 1) {
        pad->shift[0] = pad->shift[1] = 0;
    }
}

void INesPadPressButton(INesPad* pad, INesPadPlayer player, INesPadButton button) {
    assert(button >= 0 && button <= 7);
    assert(player == 0 || player == 1);
    pad->mask[player] |= ((uint8_t)1 << button);
}

void INesPadReleaseButton(INesPad* pad, INesPadPlayer player, INesPadButton button) {
    assert(button >= 0 && button <= 7);
    assert(player == 0 || player == 1);
    pad->mask[player] &= ~((uint8_t)1 << button);
}

void INesPadReleaseAll(INesPad* pad) {
    pad->mask[0] = pad->mask[1] = 0;
}
