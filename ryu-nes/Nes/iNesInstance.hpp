#ifndef iNesInstance_hpp
#define iNesInstance_hpp

#include <stdio.h>
#include "iNesFile.hpp"
#include "iNesPPU.hpp"
#include "iNesPad.hpp"
#include "iNesAPU.hpp"
#include "iNesMapper.hpp"
#include "NesCPUImpl.hpp"

#define KB4     (4096U)
#define KB8     (8192U)
#define KB16    (16384U)
#define KB32    (32768U)
#define KB64    (65536U)
#define KB128   (131072U)
#define KB256   (262144U)
#define KB512   (524288U)

enum INesInstanceMirror {
    INesInstanceMirrorOneScreen = 0,
    INesInstanceMirrorVertical = 1,
    INesInstanceMirrorHorizontal = 2,
    INesInstanceMirrorFour = 3
};

struct CPU2A03;
struct INesPPU;
struct INesPad;
struct INesAPU;
struct INesMapper;

struct INesInstance {
    INesFile* file;
    uint8_t mem[0x8000];
    CPU2A03* cpu;
    INesPPU* ppu;
    INesPad* pad;
    INesAPU* apu;
    INesMapper* mapper;
    enum INesInstanceMirror mirror;
};

INesInstance* INesInstanceCreate(const uint8_t* data, size_t size);
void INesInstanceSetFilePath(INesInstance* instance, const char* filePath);
void INesInstanceSetSaveRamFilePath(INesInstance* instance, const char* saveRamFilePath);
void INesInstanceWrite(INesInstance* instance, uint16_t addr, uint8_t data);
uint8_t INesInstanceRead(INesInstance* instance, uint16_t addr);
void INesInstanceInc(INesInstance* instance, uint16_t addr);
void INesInstanceDec(INesInstance* instance, uint16_t addr);
uint16_t INesInstanceMappingPPUAddr(uint16_t addr);
uint8_t INesInstancePPURead(INesInstance* instance, uint16_t addr);
void INesInstancePPUWrite(INesInstance* instance, uint16_t addr, uint8_t data);
void INesInstanceTickCPU(INesInstance* instance);
void INesInstanceDestroy(INesInstance* instance);
void INesInstanceFrame(INesInstance* instance);
void INesInstanceOnPPUTick(INesInstance* instance);


#endif /* iNesInstance_hpp */
