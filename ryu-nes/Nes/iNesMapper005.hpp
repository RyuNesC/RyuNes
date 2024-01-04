#ifndef iNesMapper005_hpp
#define iNesMapper005_hpp

#include "iNesInstance.hpp"

#include <stdio.h>

bool INesMapper005Init(INesInstance* instance);
void INesMapper005Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper005Read(INesInstance* instance, uint16_t addr);
void INesMapper005WriteNameTable(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper005ReadNameTable(INesInstance* instance, uint16_t addr);
void INesMapper005PPUTick(INesInstance* instance);

#endif /* iNesMapper005_hpp */
