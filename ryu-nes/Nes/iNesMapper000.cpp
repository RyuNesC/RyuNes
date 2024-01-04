#include "iNesMapper000.hpp"
#include <stdbool.h>
#include <assert.h>

struct INesMapper001 {
    uint8_t loadRegister;
    uint8_t controlRegister;
    uint8_t chrBank0;
    uint8_t chrBank1;
    uint8_t prgBank;
};

bool INesMapper000Init(INesInstance* instance) {
    if (instance->mapper->data) {
        free(instance->mapper->data);
    }
    instance->mapper->data = NULL;
    
    if ((INesMapperType)instance->mapper->number != INesMapperTypeNROM) {
        return false;
    }
    
    if (instance->file->CHRRomSize != 8192) {
        return false;
    }
    
    if (instance->file->PRGRomSize != 16384 && instance->file->PRGRomSize != 32768) {
        return false;
    }
    
    return true;
}

void INesMapper000Write(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr >= 0x4020 || addr < 0x2000);
    assert(instance->mapper->number == (uint8_t)INesMapperTypeNROM);
    if (addr < 0x2000) {
        instance->ppu->mem[addr] = data;
    }
    return ;
}

uint8_t INesMapper000Read(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x4020 || addr < 0x2000);
    assert(instance->mapper->number == (uint8_t)INesMapperTypeNROM);
    
    if (addr >= 0x8000) {
        // PRG
        if (instance->file->PRGRomSize == 16384) {
            return instance->file->PRGRom[(addr-0x8000)%0x4000];
        } else {
            assert(instance->file->PRGRomSize == 32768);
            return instance->file->PRGRom[addr-0x8000];
        }
    } else {
        assert(instance->file->CHRRomSize == 8192);
        // CHR
        return instance->file->CHRRom[addr];
    }

    return 0;
}
