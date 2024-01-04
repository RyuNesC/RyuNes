#include "iNesMapper004.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct INesMapper004 {
    uint8_t bankData[8];
    uint8_t mirroring;
    uint8_t PRGRAMProtect;
    uint8_t IRQLatch;
    uint8_t IRQCounter;
    bool IRQEnable;
    
    uint8_t RValue;
    bool CHRA12Invention;
    bool PRGROMBankMode;
    
    INesInstance* instance;
};

static uint32_t switchCHRBankAddr(INesMapper004* mapper004, uint8_t R, uint32_t addr, uint32_t base) {
    assert(R < 6);
    uint32_t bank = (uint32_t)mapper004->bankData[R];
    if (R <= 1) {
        bank &= 0xfe;
    }
    return (bank << 10) + (addr - base);
}

static uint32_t switchPRGBankAddr(INesMapper004* mapper004, uint8_t R, uint32_t addr, uint32_t base) {
    assert(R == 6 || R == 7);
    uint32_t bank = (uint32_t)mapper004->bankData[R];
    return (bank << 13) + (addr - base);
}


static uint32_t INesMapper004MappingCHRAddr(INesInstance* instance, uint16_t addr) {
    assert(addr < 0x2000);
    INesMapper004* mapper004 = (INesMapper004*)instance->mapper->data;
    uint32_t cvt = 0;
    
    uint8_t R = 0;
    uint32_t B = 0;
    
    if (addr >= 0x1c00) {
        if (mapper004->CHRA12Invention) {
            R = 1;
            B = 0x1800;
        } else {
            R = 5;
            B = 0x1c00;
        }
    } else if (addr >= 0x1800) {
        if (mapper004->CHRA12Invention) {
            R = 1;
            B = 0x1800;
        } else {
            R = 4;
            B = 0x1800;
        }
    } else if (addr >= 0x1400) {
        if (mapper004->CHRA12Invention) {
            R = 0;
            B = 0x1000;
        } else {
            R = 3;
            B = 0x1400;
        }
    } else if (addr >= 0x1000) {
        if (mapper004->CHRA12Invention) {
            R = 0;
            B = 0x1000;
        } else {
            R = 2;
            B = 0x1000;
        }
    } else if (addr >= 0x0c00) {
        if (mapper004->CHRA12Invention) {
            R = 5;
            B = 0x0c00;
        } else {
            R = 1;
            B = 0x0800;
        }
    } else if (addr >= 0x0800) {
        if (mapper004->CHRA12Invention) {
            R = 4;
            B = 0x0800;
        } else {
            R = 1;
            B = 0x0800;
        }
    } else if (addr >= 0x0400) {
        if (mapper004->CHRA12Invention) {
            R = 3;
            B = 0x0400;
        } else {
            R = 0;
            B = 0x0000;
        }
    } else {
        if (mapper004->CHRA12Invention) {
            R = 2;
            B = 0x0000;
        } else {
            R = 0;
            B = 0x0000;
        }
    }
    
    cvt = switchCHRBankAddr(mapper004, R, addr, B);
    
    if (cvt >= instance->file->CHRRomSize) {
        return 0;
    }
    
    return cvt;
}


static uint8_t INesMapper004CHRRead(INesInstance* instance, uint16_t addr) {
    assert(addr < 0x2000);
    uint32_t cvt = INesMapper004MappingCHRAddr(instance, addr);
    return INesFileReadChr(instance->file, cvt);
}

static void INesMapper004CHRWrite(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr < 0x2000);
    uint32_t cvt = INesMapper004MappingCHRAddr(instance, addr);
    instance->file->CHRRom[cvt] = data;
}

static uint8_t INesMapper004PRGRead(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x8000);
    
    INesMapper004* mapper004 = (INesMapper004*)instance->mapper->data;
    uint32_t cvt = 0;
    uint8_t R = 0;
    uint32_t B = 0;
    
    if (addr >= 0xe000) {
        // last bank
        return INesFileReadRom(instance->file, instance->file->PRGRomSize - 0x2000 + addr - 0xe000);
    } else if (addr >= 0xc000) {
        if (mapper004->PRGROMBankMode) {
            R = 6;
            B = 0xc000;
        } else {
            // penultimate
            return instance->file->PRGRom[instance->file->PRGRomSize - 0x4000 + addr - 0xc000];
        }
    } else if (addr >= 0xa000) {
        R = 7;
        B = 0xa000;
    } else {
        assert(addr >= 0x8000);
        if (mapper004->PRGROMBankMode) {
            // penultimate
            return instance->file->PRGRom[instance->file->PRGRomSize - 0x4000 + addr - 0x8000];
        } else {
            R = 6;
            B = 0x8000;
        }
    }
    cvt = switchPRGBankAddr(mapper004, R, addr, B);
    if (cvt >= instance->file->PRGRomSize) {
        return 0;
    }
    
    return instance->file->PRGRom[cvt];
}

bool INesMapper004Init(INesInstance* instance) {
    if (instance->mapper->data) {
        free(instance->mapper->data);
    }
    instance->mapper->data = NULL;
    
    if (instance->mapper->number != INesMapperTypeMMC3) {
        return false;
    }
    
    if (instance->file->PRGRomSize > KB512) {
        return false;
    }
    
    if (instance->file->PRGRomSize < KB8) {
        return false;
    }
    
    instance->mapper->data = malloc(sizeof(INesMapper004));
    memset(instance->mapper->data, 0, sizeof(INesMapper004));
    ((INesMapper004*)instance->mapper->data)->instance = instance;
    
    return true;
}

void INesMapper004Write(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr >= 0x4020 || addr < 0x2000);
    
    bool odd = addr & 1;
    INesMapper004* mapper004 = (INesMapper004*)instance->mapper->data;
    
    if (addr >= 0xe000) {
        if (odd) {
            mapper004->IRQEnable = true;
        } else {
            mapper004->IRQEnable = false;
        }
    } else if (addr >= 0xc000) {
        if (odd) {
            mapper004->IRQCounter = 0;
        } else {
            mapper004->IRQLatch = data;
        }
    } else if (addr >= 0xa000) {
        if (odd) {
            mapper004->PRGRAMProtect = data;
        } else {
            mapper004->mirroring = data & 1;
            if (mapper004->mirroring) {
                instance->mirror = INesInstanceMirrorHorizontal;
            } else {
                instance->mirror = INesInstanceMirrorVertical;
            }
        }
    } else if (addr >= 0x8000) {
        if (odd) {
            if (mapper004->RValue == 6 || mapper004->RValue == 7) {
                data &= 0x3f;
                data %= mapper004->instance->file->PRGBankCount;
            } else {
                if (mapper004->RValue == 0 || mapper004->RValue == 1) {
                    data &= 0xfe;
                }
                data %= mapper004->instance->file->CHRBankCount;
            }
            assert(mapper004->RValue <= 7);
            mapper004->bankData[mapper004->RValue] = data;
        } else {
            mapper004->CHRA12Invention = data & 0x80;
            mapper004->PRGROMBankMode = data & 0x40;
            mapper004->RValue = data & 7;
        }
    } else if (addr >= 0x6000) {
        INesFileWriteRam(instance->file, addr - 0x6000, data);
    } else if (addr < 0x2000) {
        INesMapper004CHRWrite(instance, addr, data);
    }
}

uint8_t INesMapper004Read(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x4020 || addr < 0x2000);
    if (addr >= 0x8000) {
        return INesMapper004PRGRead(instance, addr);
    } else if (addr >= 0x6000) {
        return INesFileReadRam(instance->file, addr - 0x6000);
    } else if (addr < 0x2000) {
        return INesMapper004CHRRead(instance, addr);
    }
    return 0;
}

void INesMapper004PPUTick(INesInstance* instance) {
    if (instance->ppu->tx != 280) {
        return;
    }
    
    if (instance->ppu->ty >= 240 && instance->ppu->ty <= 260) {
        return;
    }
    
    if (!instance->ppu->bge && !instance->ppu->spe) {
        return;
    }
    
    INesMapper004* mapper004 = (INesMapper004*)instance->mapper->data;
    if (mapper004->IRQCounter == 0) {
        mapper004->IRQCounter = mapper004->IRQLatch;
    } else {
        --mapper004->IRQCounter;
        if (mapper004->IRQCounter == 0 && mapper004->IRQEnable) {
            if (!instance->cpu->flag.I) {
                instance->cpu->irq = 1;
            }
        }
    }
}
