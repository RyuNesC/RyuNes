#include "iNesMapper002.hpp"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct INesMapper002 {
    uint8_t bankSelectRegister;
};

bool INesMapper002Init(INesInstance* instance) {
    if (instance->mapper->data) {
        free(instance->mapper->data);
    }
    instance->mapper->data = NULL;
    
    if ((INesMapperType)instance->mapper->number != INesMapperTypeUxROM) {
        return false;
    }
    
    if (instance->file->PRGRomSize < 16384) {
        return false;
    }
    
    instance->mapper->data = malloc(sizeof(INesMapper002));
    memset(instance->mapper->data, 0, sizeof(INesMapper002));
    
    return true;
}

void INesMapper002Write(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr >= 0x4020 || addr < 0x2000);
    assert(instance->mapper->number == (uint8_t)INesMapperTypeUxROM);
    INesMapper002* mapper002 = (INesMapper002*)instance->mapper->data;
    
    if (addr >= 0x8000) {
        mapper002->bankSelectRegister = data;
    } else if (addr < 0x2000){
        instance->ppu->mem[addr] = data;
    }
}

uint8_t INesMapper002Read(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x4020 || addr < 0x2000);
    assert(instance->mapper->number == (uint8_t)INesMapperTypeUxROM);
    assert(instance->file->PRGRomSize >= 0x4000);
    INesMapper002* mapper002 = (INesMapper002*)instance->mapper->data;
    
    if (addr >= 0x8000) {
        if (addr >= 0xc000) {
            return instance->file->PRGRom[instance->file->PRGRomSize - 0x4000 + (uint32_t)addr - 0xc000];
        } else {
            return instance->file->PRGRom[(((uint32_t)mapper002->bankSelectRegister & 0xf) << 14) + (uint32_t)addr - 0x8000];
        }
    } else if (addr < 0x2000) {
        // CHR
        return instance->ppu->mem[addr];
    }
    
    return 0;
}
