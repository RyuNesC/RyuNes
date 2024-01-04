#ifndef iNesMapper004_hpp
#define iNesMapper004_hpp

#include "iNesInstance.hpp"

#include <stdio.h>

bool INesMapper004Init(INesInstance* instance);
void INesMapper004Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper004Read(INesInstance* instance, uint16_t addr);
void INesMapper004PPUTick(INesInstance* instance);

#endif /* iNesMapper004_hpp */
