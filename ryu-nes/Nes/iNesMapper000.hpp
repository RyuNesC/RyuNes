#ifndef iNesMapper000_hpp
#define iNesMapper000_hpp

#include "iNesInstance.hpp"

#include <stdio.h>

bool INesMapper000Init(INesInstance* instance);
void INesMapper000Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper000Read(INesInstance* instance, uint16_t addr);

#endif /* iNesMapper000_hpp */
