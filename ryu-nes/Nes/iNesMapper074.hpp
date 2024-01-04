#ifndef iNesMapper074_hpp
#define iNesMapper074_hpp

#include "iNesInstance.hpp"

#include <stdio.h>

bool INesMapper074Init(INesInstance* instance);
void INesMapper074Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper074Read(INesInstance* instance, uint16_t addr);
void INesMapper074PPUTick(INesInstance* instance);

#endif /* iNesMapper074_hpp */
