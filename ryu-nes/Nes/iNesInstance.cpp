#include "iNesInstance.hpp"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "iNesMapper.hpp"
#include "iNesPad.hpp"

INesInstance* INesInstanceCreate(const uint8_t* data, size_t size) {
    INesInstance* instance = (INesInstance*)malloc(sizeof(INesInstance));
    memset(instance, 0, sizeof(INesInstance));
    instance->file = INesFileCreate(data, size);
    if (instance->file == NULL) {
        INesInstanceDestroy(instance);
        return NULL;
    }
    
    if (INesFileUseFourScreenVRAM(instance->file)) {
        instance->mirror = INesInstanceMirrorFour;
    } else {
        INesFileMirroring fileMirror = INesFileGetMirroring(instance->file);
        if (fileMirror == INesFileMirroringHorizontal) {
            instance->mirror = INesInstanceMirrorHorizontal;
        } else {
            assert(fileMirror == INesFileMirroringVertical);
            instance->mirror = INesInstanceMirrorVertical;
        }
    }
    
    instance->mapper = (INesMapper*)malloc(sizeof(INesMapper));
    memset(instance->mapper, 0, sizeof(INesMapper));
    instance->mapper->number = INesFileGetMapperNumber(instance->file);
    if (!INesMapperInit(instance)) {
        INesInstanceDestroy(instance);
        return NULL;
    }
    
    instance->cpu = (CPU2A03*)malloc(sizeof(CPU2A03));
    memset(instance->cpu, 0, sizeof(CPU2A03));
    CpuInit(instance->cpu);
    CpuReset(instance->cpu, instance);
    
    instance->ppu = (INesPPU*)malloc(sizeof(INesPPU));
    INesPPUReset(instance);
    
    instance->pad = (INesPad*)malloc(sizeof(INesPad));
    memset(instance->pad, 0, sizeof(INesPad));
    
    instance->apu = (INesAPU*)malloc(sizeof(INesAPU));
    memset(instance->apu, 0, sizeof(INesAPU));
    INesAPUReset(instance->apu);
    instance->apu->instance = instance;
    
    return instance;
}

void INesInstanceSetFilePath(INesInstance* instance, const char* filePath) {
    size_t filePathLength = strlen(filePath);
    if (instance->file->filePath) {
        free(instance->file->filePath);
    }
    instance->file->filePath = (char*)malloc(filePathLength + 1);
    memcpy(instance->file->filePath, filePath, filePathLength + 1);
}

void INesInstanceSetSaveRamFilePath(INesInstance* instance, const char* saveRamFilePath) {
    if (!INesFileContainsMemoryChip(instance->file)) {
        return;
    }
    
    size_t saveRamFilePathLength = strlen(saveRamFilePath);
    if (instance->file->saveRamFilePath) {
        free(instance->file->saveRamFilePath);
    }
    instance->file->saveRamFilePath = (char*)malloc(saveRamFilePathLength + 1);
    memcpy(instance->file->saveRamFilePath, saveRamFilePath, saveRamFilePathLength + 1);
}

void INesInstanceWrite(INesInstance* instance, uint16_t addr, uint8_t data) {
    if (addr >= 0x4020) {
        return INesMapperWrite(instance, addr, data);
    }
    
    if ((addr >= 0x2000 && addr <= 0x2007) || addr == 0x4014) {
        return INesPPUWritePort(instance, addr, data);
    }
    
    if (addr == 0x4016) {
        return INesPadWritePort(instance->pad, addr, data);
    }
    
    if ((addr >= 0x4000 && addr <= 0x400C) || (addr >= 0x400E && addr <= 0x4015) || addr == 0x4017) {
        return INesAPUWrite(instance->apu, addr, data);
    }
    
    instance->mem[addr] = data;
}

uint8_t INesInstanceRead(INesInstance* instance, uint16_t addr) {
    if (addr >= 0x4020) {
        return INesMapperRead(instance, addr);
    }
    
    if ((addr >= 0x2000 && addr <= 0x2007) || addr == 0x4014) {
        return INesPPUReadPort(instance, addr);
    }
    
    if (addr == 0x4016 || addr == 0x4017) {
        return INesPadReadPort(instance->pad, addr);
    }
    
    if ((addr >= 0x4000 && addr <= 0x400C) || (addr >= 0x400E && addr <= 0x4015)) {
        return INesAPURead(instance->apu, addr);
    }
    
    return instance->mem[addr];
}

void INesInstanceInc(INesInstance* instance, uint16_t addr) {
    uint8_t data = INesInstanceRead(instance, addr) + 1;
    INesInstanceWrite(instance, addr, data);
}

void INesInstanceDec(INesInstance* instance, uint16_t addr) {
    uint8_t data = INesInstanceRead(instance, addr) - 1;
    INesInstanceWrite(instance, addr, data);
}

uint16_t INesInstanceMappingPPUAddr(uint16_t addr) {
    addr = addr & 0x3fff;
    if (addr >= 0x3f00) {
        return 0x3f00 + ((addr - 0x3f00) & 0x1f);
    }
    if (addr >= 0x2000) {
        return 0x2000 + ((addr - 0x2000) & 0xfff);
    }
    return addr;
}

uint8_t INesInstancePPURead(INesInstance* instance, uint16_t addr) {
    addr = INesInstanceMappingPPUAddr(addr);
    
    if (addr >= 0x2000 && addr < 0x3000 && INesMapperRequireMappingNametable(instance)) {
        return INesMapperReadNameTable(instance, addr);
    }
    
    if (addr < 0x2000) {
        return INesMapperRead(instance, addr);
    }
    if (addr >= 0x3f00) {
        if (addr % 4 == 0) {
            return instance->ppu->mem[0x3f00];
        }
        return instance->ppu->mem[addr];
    }
    if (instance->mirror == INesInstanceMirrorVertical) {
        if (addr >= 0x2800) {
            addr -= 0x800;
        }
    } else if (instance->mirror == INesInstanceMirrorHorizontal) {
        if (addr >= 0x2400 && addr < 0x2c00) {
            addr -= 0x0400;
        } else if (addr >= 0x2c00) {
            addr -= 0x0800;
        }
    }
    
    return instance->ppu->mem[addr];
}

void INesInstancePPUWrite(INesInstance* instance, uint16_t addr, uint8_t data) {
    addr = INesInstanceMappingPPUAddr(addr);
    
    if (addr >= 0x2000 && addr < 0x3000 && INesMapperRequireMappingNametable(instance)) {
        INesMapperWriteNameTable(instance, addr, data);
        return;
    }
    
    if (addr < 0x2000) {
        return INesMapperWrite(instance, addr, data);
    }
    if (addr >= 0x3f00) {
        if (addr % 4 == 0 && (addr == 0x3f00 || addr == 0x3f10)) {
            instance->ppu->mem[0x3f00] = data;
            return ;
        }
    } else {
        if (instance->mirror != INesInstanceMirrorFour) {
            if (instance->mirror == INesInstanceMirrorVertical) {
                if (addr >= 0x2800) {
                    addr -= 0x800;
                }
            } else if (instance->mirror == INesInstanceMirrorHorizontal) {
                if (addr >= 0x2400 && addr < 0x2c00) {
                    addr -= 0x0400;
                } else if (addr >= 0x2c00) {
                    addr -= 0x0800;
                }
            }
        }
    }
    instance->ppu->mem[addr] = data;
}

void INesInstanceTickCPU(INesInstance* instance) {
    CpuTick(instance->cpu, instance);
}

void INesInstanceDestroy(INesInstance* instance) {
    if (instance->file) {
        INesFileDestroy(instance->file);
    }
    if (instance->cpu) {
        free(instance->cpu);
    }
    if (instance->ppu) {
        free(instance->ppu);
    }
    if (instance->pad) {
        free(instance->pad);
    }
    if (instance->apu) {
        free(instance->apu);
    }
    if (instance->mapper) {
        INesMapperDestroy(instance);
    }
    free(instance);
}

void INesInstanceFrame(INesInstance* instance) {
    size_t count = 0;
    for (size_t i = 0; i < 89342; ++i) {
        INesPPUTick(instance);
        if (instance->ppu->tick % 3 == 0) {
            INesInstanceTickCPU(instance);
            if (instance->cpu->tick % 2 == 0) {
                INesAPUTick(instance->apu);
            }
        }
        ++count;
        if (count == 22335) {
            count = 0;
            INesAPUFrame(instance);
        }
    }
}

void INesInstanceOnPPUTick(INesInstance* instance) {
    INesMapperPPUTick(instance);
}
