#include "iNesMapper003.hpp"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct INesMapper003 {
    uint8_t bankSelectRegister;
};

bool INesMapper003Init(INesInstance* instance) {
    if (instance->mapper->data) {
        free(instance->mapper->data);
    }
    instance->mapper->data = NULL;
    
    if ((INesMapperType)instance->mapper->number != INesMapperTypeCNROM) {
        return false;
    }
    
    if (instance->file->PRGRomSize != 16384 && instance->file->PRGRomSize != 32768) {
        return false;
    }
    
    if (instance->file->CHRRomSize < 8192) {
        return false;
    }
    
    instance->mapper->data = malloc(sizeof(INesMapper003));
    memset(instance->mapper->data, 0, sizeof(INesMapper003));
    
    return true;
}

void INesMapper003Write(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr >= 0x4020 || addr < 0x2000);
    assert(instance->mapper->number == (uint8_t)INesMapperTypeCNROM);
    INesMapper003* mapper003 = (INesMapper003*)instance->mapper->data;
    
    if (addr >= 0x8000) {
        mapper003->bankSelectRegister = data;
    } else if (addr < 0x2000) {
        if (instance->file->CHRRomSize == 0) {
            instance->ppu->mem[addr] = data;
        }
    }
}

uint8_t INesMapper003Read(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x4020 || addr < 0x2000);
    INesMapper003* mapper003 = (INesMapper003*)instance->mapper->data;
    
    if (addr >= 0x8000) {
        // PRG
        if (instance->file->PRGRomSize == 16384) {
            return instance->file->PRGRom[(addr-0x8000)%0x4000];
        } else {
            assert(instance->file->PRGRomSize == 32768);
            return instance->file->PRGRom[addr-0x8000];
        }
    } else if (addr < 0x2000) {
        // CHR
        if (instance->file->CHRRomSize == 0) {
            return instance->ppu->mem[addr];
        }
        
        uint32_t cvtaddr = (((uint32_t)mapper003->bankSelectRegister & 3) << 13) + (uint32_t)addr;
        if (cvtaddr >= instance->file->CHRRomSize) {
            assert(!"chr size error!");
            return 0;
        }
        return instance->file->CHRRom[cvtaddr];
    }
    return 0;
}
