#include "iNesMapper001.hpp"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

enum INesMapper001Type {
    INesMapper001TypeOthers = 0,
    INesMapper001TypeSN = 1,
    INesMapper001TypeSOSUSX = 2,
};

struct INesMapper001 {
    uint8_t loadRegister;
    uint8_t controlRegister;
    uint8_t chrBank0;
    uint8_t chrBank1;
    uint8_t prgBank;
    bool mode4kb;
    bool useCHRRAM;
    enum INesMapper001Type type;
    uint8_t PRGRAM[KB32];
    uint8_t PRGRAMBank;
    uint8_t PRG256ROMBank;
};

bool INesMapper001Init(INesInstance* instance) {
    if (instance->mapper->data) {
        free(instance->mapper->data);
    }
    instance->mapper->data = NULL;
    
    if ((INesMapperType)instance->mapper->number != INesMapperTypeMMC1) {
        return false;
    }
    
    if (instance->file->PRGRomSize < 16384) {
        return false;
    }
    
    instance->mapper->data = malloc(sizeof(INesMapper001));
    memset(instance->mapper->data, 0, sizeof(INesMapper001));
    INesMapper001* mapper001 = (INesMapper001*)instance->mapper->data;
    mapper001->loadRegister = 0x10;
    mapper001->controlRegister = 0x0c;
    mapper001->mode4kb = false;
    
    size_t PRGRAM = INesFileGetPRGRamSizeBytes(instance->file);
    size_t PRGROM = INesFileGetPRGRomSizeBytes(instance->file);
    mapper001->useCHRRAM = INesFileGetCHRRomSizeBytes(instance->file) == 0;
    
    if (PRGROM == KB256 && PRGRAM == KB8 && mapper001->useCHRRAM) {
        mapper001->type = INesMapper001TypeSN;
    } else if (PRGROM == KB512 && mapper001->useCHRRAM) {
        mapper001->type = INesMapper001TypeSOSUSX;
    }
    
    return true;
}

void INesMapper001Write(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr >= 0x4020 || addr < 0x2000);
    assert(instance->mapper->number == (uint8_t)INesMapperTypeMMC1);
    INesMapper001* mapper001 = (INesMapper001*)instance->mapper->data;
    
    if (addr >= 0x8000) {
        if ((data >> 7) == 1) {
            mapper001->loadRegister = 0x10;
            mapper001->controlRegister = 0x0c;
            mapper001->mode4kb = (mapper001->controlRegister >> 4) & 1;
        } else {
            uint8_t complete = mapper001->loadRegister & 1;
            mapper001->loadRegister >>= 1;
            mapper001->loadRegister |= (data & 1) << 4;
            if (complete) {
                if (addr <= 0x9fff) {
                    // control
                    mapper001->controlRegister = mapper001->loadRegister & 0x1f;
                    mapper001->mode4kb = (mapper001->controlRegister >> 4) & 1;
                    uint8_t mirror = mapper001->controlRegister & 3;
                    if (mirror <= 1) {
                        instance->mirror = INesInstanceMirrorOneScreen;
                    } else if (mirror == 2) {
                        instance->mirror = INesInstanceMirrorVertical;
                    } else {
                        assert(mirror == 3);
                        instance->mirror = INesInstanceMirrorHorizontal;
                    }
                } else if (addr <= 0xbfff) {
                    // chr bank 0
                    mapper001->chrBank0 = mapper001->loadRegister & 0x1f;
                    if (mapper001->type == INesMapper001TypeSOSUSX) {
                        mapper001->PRGRAMBank = (mapper001->chrBank0 >> 2) & 3;
                        mapper001->PRG256ROMBank = (mapper001->chrBank0 >> 4) & 1;
                    }
                } else if (addr <= 0xdfff) {
                    // chr bank 1
                    mapper001->chrBank1 = mapper001->loadRegister & 0x1f;
                    if (mapper001->mode4kb && mapper001->type == INesMapper001TypeSOSUSX) {
                        mapper001->PRGRAMBank = (mapper001->chrBank1 >> 2) & 3;
                        mapper001->PRG256ROMBank = (mapper001->chrBank1 >> 4) & 1;
                    }
                } else {
                    assert(addr <= 0xffff);
                    // prg bank
                    mapper001->prgBank = mapper001->loadRegister & 0x0f;
                }
                mapper001->loadRegister = 0x10;
            }
        }
    } else if (addr >= 0x6000) {
        uint16_t offset = addr - 0x6000;
        uint16_t bankSize = mapper001->mode4kb ? KB4 : KB8;
        INesFileWriteRam(instance->file, offset + bankSize * mapper001->PRGRAMBank, data);
    } else if (addr < 0x2000) {
        // CHR
        instance->ppu->mem[addr] = data;
    }
}

uint8_t INesMapper001Read(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x4020 || addr < 0x2000);
    INesMapper001* mapper001 = (INesMapper001*)instance->mapper->data;
    
    if (addr >= 0x8000) {
        size_t startOffset = 0;
        size_t spaceSize = instance->file->PRGRomSize;
        uint32_t pbank = mapper001->prgBank;
        size_t prgAddr = 0;
        
        if (mapper001->type == INesMapper001TypeSOSUSX) {
            if (mapper001->PRG256ROMBank) {
                startOffset = 262144;
            }
            spaceSize = 262144;
        }
        uint8_t prgBankMode = (mapper001->controlRegister >> 2) & 3;
        if (prgBankMode <= 1) {
            return instance->file->PRGRom[startOffset + ((pbank & 0xe) << 14) + (uint32_t)(addr - 0x8000)];
        } else if (prgBankMode == 2) {
            if (addr < 0xc000) {
                // fix first bank at $8000
                return instance->file->PRGRom[startOffset + addr - 0x8000];
            } else {
                assert(addr >= 0xc000);
                return instance->file->PRGRom[startOffset + (pbank << 14) + (uint32_t)(addr - 0xc000)];
            }
        } else if (prgBankMode == 3) {
            if (addr >= 0xc000) {
                // fix last bank at $C000
                prgAddr = startOffset + spaceSize - 0x4000 + ((uint32_t)addr - 0xc000);
                if (prgAddr >= instance->file->PRGRomSize) {
                    assert(!"prg size error!");
                    return 0;
                }
                return instance->file->PRGRom[prgAddr];
            } else {
                assert(addr < 0xc000);
                return instance->file->PRGRom[startOffset + (pbank << 14) + (uint32_t)(addr - 0x8000)];
            }
        }
    } else if (addr >= 0x6000) {
        uint16_t offset = addr - 0x6000;
        uint16_t bankSize = mapper001->mode4kb ? KB4 : KB8;
        return INesFileReadRam(instance->file, offset + bankSize * mapper001->PRGRAMBank);
    } else if (addr < 0x2000) {
        if (instance->file->onlyCHRRam) {
            return instance->ppu->mem[addr];
        }
        
        if (mapper001->mode4kb) {
            // 4 kb mode, switch two separate 4 KB banks
            if (addr < 0x1000) {
                // lower bank
                uint32_t cvtaddr = ((uint32_t)mapper001->chrBank0 << 12) + (uint32_t)addr;
                if (cvtaddr >= instance->file->CHRRomSize) {
                    assert(!"chr size error!");
                    return 0;
                }
                return instance->file->CHRRom[cvtaddr];
            } else {
                // upper bank
                uint32_t cvtaddr = ((uint32_t)mapper001->chrBank1 << 12) + ((uint32_t)addr - (uint32_t)0x1000);
                if (cvtaddr >= instance->file->CHRRomSize) {
                    assert(!"chr size error!");
                    return 0;
                }
                return instance->file->CHRRom[cvtaddr];
            }
        } else {
            // 8 kb mode, switch 8 KB at a time
            uint32_t cvtaddr = ((uint32_t)(mapper001->chrBank0 & 0xfe) << 12) + (uint32_t)addr;
            if (cvtaddr >= instance->file->CHRRomSize) {
                assert(!"chr size error!");
                return 0;
            }
            return instance->file->CHRRom[cvtaddr];
        }
    }
    return 0;
}
