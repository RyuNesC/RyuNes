#ifndef iNesMapper003_hpp
#define iNesMapper003_hpp

#include "iNesInstance.hpp"

#include <stdio.h>

bool INesMapper003Init(INesInstance* instance);
void INesMapper003Write(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesMapper003Read(INesInstance* instance, uint16_t addr);

#endif /* iNesMapper003_hpp */
