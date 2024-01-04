#ifndef iNesMapper001_hpp
#define iNesMapper001_hpp
#include "iNesInstance.hpp"

#include <stdio.h>


bool INesMapper001Init(INesInstance* instance);
void INesMapper001Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper001Read(INesInstance* instance, uint16_t addr);

#endif
