#ifndef iNesMapper002_hpp
#define iNesMapper002_hpp

#include "iNesInstance.hpp"

#include <stdio.h>

bool INesMapper002Init(INesInstance* instance);
void INesMapper002Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper002Read(INesInstance* instance, uint16_t addr);

#endif /* iNesMapper002_hpp */
