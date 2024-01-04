#ifndef iNesMapper_hpp
#define iNesMapper_hpp

#include <stdio.h>
#include "iNesInstance.hpp"

struct INesInstance;

struct INesMapper {
    uint8_t number;
    void* data;
};

enum INesMapperType {
    INesMapperTypeNROM = 0,
    INesMapperTypeMMC1 = 1,
    INesMapperTypeUxROM = 2,
    INesMapperTypeCNROM = 3,
    INesMapperTypeMMC3 = 4,
    INesMapperTypeMMC5 = 5,
    INesMapperType074 = 74,
};

bool INesMapperInit(INesInstance* instance);
bool INesMapperRequireMappingNametable(INesInstance* instance);
void INesMapperWriteNameTable(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapperReadNameTable(INesInstance* instance, uint16_t addr);
void INesMapperWrite(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapperRead(INesInstance* instance, uint16_t addr);
void INesMapperPPUTick(INesInstance* instance);
void INesMapperDestroy(INesInstance* instance);

#endif /* iNesMapper_hpp */
