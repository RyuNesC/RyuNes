#include "iNesMapper.hpp"
#include "iNesMapper000.hpp"
#include "iNesMapper001.hpp"
#include "iNesMapper002.hpp"
#include "iNesMapper003.hpp"
#include "iNesMapper004.hpp"
#include "iNesMapper005.hpp"
#include "iNesMapper074.hpp"
#include <string.h>
#include <assert.h>

static void (*INesMapperWriteFuncs[256])(INesInstance* instance, uint16_t addr, uint8_t data) = {
    INesMapper000Write,
    INesMapper001Write,
    INesMapper002Write,
    INesMapper003Write,
    INesMapper004Write,
    INesMapper005Write,
};

static uint8_t (*INesMapperReadFuncs[256])(INesInstance* instance, uint16_t addr) = {
    INesMapper000Read,
    INesMapper001Read,
    INesMapper002Read,
    INesMapper003Read,
    INesMapper004Read,
    INesMapper005Read,
};

static bool (*INesMapperInitFuncs[256])(INesInstance* instance) = {
    INesMapper000Init,
    INesMapper001Init,
    INesMapper002Init,
    INesMapper003Init,
    INesMapper004Init,
    INesMapper005Init,
};

static void (*INesMapperPPUTickFuncs[256])(INesInstance* instance) = {
};

//MARK: interface
bool INesMapperInit(INesInstance* instance) {
    INesMapperPPUTickFuncs[INesMapperTypeMMC3] = INesMapper004PPUTick;
    INesMapperInitFuncs[INesMapperType074] = INesMapper074Init;
    INesMapperReadFuncs[INesMapperType074] = INesMapper074Read;
    INesMapperWriteFuncs[INesMapperType074] = INesMapper074Write;
    INesMapperPPUTickFuncs[INesMapperType074] = INesMapper074PPUTick;
    INesMapperPPUTickFuncs[INesMapperTypeMMC5] = INesMapper005PPUTick;
    
    bool (*CheckFunc)(INesInstance* instance) = INesMapperInitFuncs[instance->mapper->number];
    return CheckFunc(instance);
}

bool INesMapperRequireMappingNametable(INesInstance* instance) {
    return instance->mapper->number == (uint8_t)INesMapperTypeMMC5;
}

void INesMapperWriteNameTable(INesInstance* instance, uint16_t addr, uint8_t data) {
    if (instance->mapper->number == INesMapperTypeMMC5) {
        INesMapper005WriteNameTable(instance, addr, data);
    }
}

uint8_t INesMapperReadNameTable(INesInstance* instance, uint16_t addr) {
    if (instance->mapper->number == INesMapperTypeMMC5) {
        return INesMapper005ReadNameTable(instance, addr);
    }
    return 0;
}

void INesMapperWrite(INesInstance* instance, uint16_t addr, uint8_t data) {
    void (*WriteFunc)(INesInstance* instance, uint16_t addr, uint8_t data) = INesMapperWriteFuncs[instance->mapper->number];
    WriteFunc(instance, addr, data);
}

uint8_t INesMapperRead(INesInstance* instance, uint16_t addr) {
    uint8_t (*ReadFunc)(INesInstance* instance, uint16_t addr) = INesMapperReadFuncs[instance->mapper->number];
    return ReadFunc(instance, addr);
}

void INesMapperPPUTick(INesInstance* instance) {
    void (*PPUTickFunc)(INesInstance* instance) = INesMapperPPUTickFuncs[instance->mapper->number];
    if (PPUTickFunc) {
        PPUTickFunc(instance);
    }
}

void INesMapperDestroy(INesInstance* instance) {
    if (instance->mapper) {
        if (instance->mapper->data) {
            free(instance->mapper->data);
        }
        free(instance->mapper);
    }
}
