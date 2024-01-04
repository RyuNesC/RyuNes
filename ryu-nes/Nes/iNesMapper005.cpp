#include "iNesMapper005.hpp"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

static const uint8_t CHR_PAGE_8KB_FLAG = 0;
static const uint8_t CHR_PAGE_4KB_FLAG = 1;
static const uint8_t CHR_PAGE_2KB_FLAG = 2;
static const uint8_t CHR_PAGE_1KB_FLAG = 3;


enum INesMapper005VerticalSplitSide {
    INesMapper005VerticalSplitSideLeft = 0,
    INesMapper005VerticalSplitSideRight = 1,
};

struct INesMapper005 {
    uint8_t PRGBankMode;
    uint8_t CHRBankMode;
    uint8_t PRGRamProtect1;
    uint8_t PRGRamProtect2;
    uint8_t extendRamMode;
    uint8_t nametableMapping;
    uint8_t fillModeTile;
    uint8_t fillModeColor;
    uint8_t PRGSelectBanks[5];
    uint8_t upperCHRBankBits;   // no game use it
    uint8_t CHRSelectBanks[12];
    bool enableVerticalSplitMode;
    INesMapper005VerticalSplitSide verticalSplitSide;
    uint8_t verticalSplitTile;
    uint8_t verticalSplitScroll;
    uint8_t verticalSplitBank;
    uint8_t IRQScanlineCmpVal;
    bool IRQScanlineEnable;
    bool IRQScanlinePending;
    bool IRQScanlineInFrame;
    uint8_t mult0;
    uint8_t mult1;
    uint8_t extendRAM[1024];
    uint8_t scanlineCounter;
};

static uint8_t INesMapper005ReadPRG(INesInstance* instance, size_t range0, size_t range1, size_t reg, size_t addr) {
    assert(reg >= 0x5113 && reg <= 0x5117);
    assert(addr >= range0 && addr <= range1);
    assert(((range1 - range0) & 0xfff) == 0xfff);
    INesMapper005* mapper = (INesMapper005*)instance->mapper->data;
    
    size_t regValue = mapper->PRGSelectBanks[reg - 0x5113];
    size_t bank = regValue;
    
    if (range1 - range0 == 32767) {
        bank &= 0xfc;
    } else if (range1 - range0 == 16383) {
        bank &= 0xfe;
    } else {
        assert(range1 - range0 == 8191);
    }
    
    if (reg >= 0x5114 && reg <= 0x5116) {
        bank &= 0x7f;
    }
    
    if (reg >= 0x5114 && reg <= 0x5116 && !(regValue >> 7)) {
        bank &= 0xf;
        return INesFileReadRam(instance->file, addr - range0 + (bank << 13));
    }
    
    bank %= instance->file->PRGBankCount;
    return INesFileReadRom(instance->file, addr - range0 + (bank << 13));
}

static uint8_t INesMapper005ReadCHR(INesInstance* instance, size_t range0, size_t range1, size_t reg, size_t addr) {
    assert(addr >= range0 && addr <= range1);
    assert(reg >= 0x5120 && addr <= 0x512b);
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    
    uint8_t bank = mapper005->CHRSelectBanks[reg - 0x5120];
    size_t base = bank * (range1 - range0 + 1);
    size_t offset = addr - range0;
    return INesFileReadChr(instance->file, base + offset);
}

static uint16_t INesMapper005MappingNameTableAddr(INesInstance* instance, uint16_t addr, uint8_t* page) {
    assert(addr >= 0x2000 && addr <= 0x2fff);
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    uint8_t p = 0;
    uint16_t cvt = 0;
    
    if (addr <= 0x23ff) {
        // $2000 -> $23ff
        p = mapper005->nametableMapping & 3;
        cvt = p == 0 ? addr : addr + 0x400;
    } else if (addr <= 0x27ff) {
        // $2400 -> $27ff
        p = (mapper005->nametableMapping >> 2) & 3;
        cvt = p == 0 ? addr - 0x400 : addr;
    } else if (addr <= 0x2bff) {
        // $2800 -> $2bff
        p = (mapper005->nametableMapping >> 4) & 3;
        cvt = p == 0 ? addr - 0x800 : addr - 0x400;
    } else {
        // $2c00 -> $2fff
        p = (mapper005->nametableMapping >> 6) & 3;
        cvt = p == 0 ? addr - 0xc00 : addr - 0x800;
    }
    
    if (page) {
        *page = p;
    }
    
    return cvt;
}

bool INesMapper005Init(INesInstance* instance) {
    if (instance->mapper->data) {
        free(instance->mapper->data);
    }
    instance->mapper->data = NULL;
    
    if (instance->mapper->number != INesMapperTypeMMC5) {
        return false;
    }
    
    instance->mapper->data = malloc(sizeof(INesMapper005));
    memset(instance->mapper->data, 0, sizeof(INesMapper005));
    
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    mapper005->mult0 = 0xff;
    mapper005->mult1 = 0xff;
    mapper005->PRGSelectBanks[4] = 0x7f;
    mapper005->scanlineCounter = 0xff;
    
    return true;
}

void INesMapper005Write(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert(addr >= 0x4020 || addr < 0x2000);
    
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    if (addr == 0x5100) {
        mapper005->PRGBankMode = data & 3;
    } else if (addr == 0x5101) {
        mapper005->CHRBankMode = data & 3;
    } else if (addr == 0x5102) {
        mapper005->PRGRamProtect1 = data & 3;
    } else if (addr == 0x5103) {
        mapper005->PRGRamProtect2 = data & 3;
    } else if (addr == 0x5104) {
        mapper005->extendRamMode = data & 3;
    } else if (addr == 0x5105) {
        mapper005->nametableMapping = data;
    } else if (addr == 0x5106) {
        mapper005->fillModeTile = data;
    } else if (addr == 0x5107) {
        data = data & 3;
        data = data | (data << 2) | (data << 4) | (data << 6);
        mapper005->fillModeColor = data;
    } else if (addr >= 0x5113 && addr <= 0x5117) {
        if (addr == 0x5113) {
            data = data & 0xf;
        } else if (addr == 0x5117) {
            data = data & 0x7f;
        }
        mapper005->PRGSelectBanks[addr-0x5113] = data;
    } else if (addr >= 0x5120 && addr <= 0x512b) {
        mapper005->CHRSelectBanks[addr - 0x5120] = data;
    } else if (addr == 0x5130) {
        mapper005->upperCHRBankBits = data & 3;
    } else if (addr == 0x5200) {
        mapper005->enableVerticalSplitMode = (data >> 7) & 1;
        mapper005->verticalSplitSide = (INesMapper005VerticalSplitSide)((data >> 6) & 1);
        mapper005->verticalSplitTile = data & 0x1f;
    } else if (addr == 0x5201) {
        mapper005->verticalSplitScroll = data;
    } else if (addr == 0x5202) {
        mapper005->verticalSplitBank = data;
    } else if (addr == 0x5203) {
        mapper005->IRQScanlineCmpVal = data;
    } else if (addr == 0x5204) {
        mapper005->IRQScanlineEnable = (data >> 7) & 1;
    } else if (addr == 0x5205) {
        mapper005->mult0 = data;
    } else if (addr == 0x5206) {
        mapper005->mult1 = data;
    } else if (addr >= 0x5c00 && addr <= 0x5fff) {
        if (mapper005->extendRamMode == 2) {
            mapper005->extendRAM[addr - 0x5c00] = data;
        } else if (mapper005->extendRamMode <= 1) {
            // Writes are only allowed when the PPU is rendering, otherwise a $00 is written.
            if (INesPPUIsRendering(instance)) {
                mapper005->extendRAM[addr - 0x5c00] = data;
            } else {
                mapper005->extendRAM[addr - 0x5c00] = 0;
            }
        }
    } else if (addr >= 0x6000 && addr <= 0x7fff) {
        uint16_t offset = addr - 0x6000;
        size_t addr = ((size_t)mapper005->PRGSelectBanks[0] << 13) + (size_t)offset;
        INesFileWriteRam(instance->file, addr, data);
    }
}

uint8_t INesMapper005Read(INesInstance* instance, uint16_t addr) {
    assert(addr >= 0x4020 || addr < 0x2000);
    
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    uint8_t data = 0;
    
    if (addr == 0x5204) {
        data = (mapper005->IRQScanlinePending << 7) | (mapper005->IRQScanlineInFrame << 6);
        mapper005->IRQScanlinePending = false;
    } else if (addr == 0x5205) {
        data = (mapper005->mult0 * mapper005->mult1) & 0xff;
    } else if (addr == 0x5206) {
        data = (uint8_t)(((uint16_t)mapper005->mult0 * (uint16_t)mapper005->mult1) >> 8);
    } else if (addr >= 0x5c00 && addr <= 0x5fff) {
        if (mapper005->extendRamMode >= 2) {
            data = mapper005->extendRAM[addr - 0x5c00];
        }
    } else if (addr >= 0x6000 && addr <= 0x7fff) {
        uint16_t offset = addr - 0x6000;
        size_t addr = ((size_t)mapper005->PRGSelectBanks[0] << 13) + (size_t)offset;
        data = INesFileReadRam(instance->file, addr);
    } else if (addr >= 0x8000 && addr <= 0xffff) {
        if (mapper005->PRGBankMode == 0) {
            data = INesMapper005ReadPRG(instance, 0x8000, 0xffff, 0x5117, addr);
        } else if (mapper005->PRGBankMode == 1) {
            if (addr >= 0x8000 && addr <= 0xbfff) {
                data = INesMapper005ReadPRG(instance, 0x8000, 0xbfff, 0x5115, addr);
            } else {
                assert(addr >= 0xc000 && addr <= 0xffff);
                data = INesMapper005ReadPRG(instance, 0xc000, 0xffff, 0x5117, addr);
            }
        } else if (mapper005->PRGBankMode == 2) {
            if (addr >= 0x8000 && addr <= 0xbfff) {
                data = INesMapper005ReadPRG(instance, 0x8000, 0xbfff, 0x5115, addr);
            } else if (addr >= 0xc000 && addr <= 0xdfff) {
                data = INesMapper005ReadPRG(instance, 0xc000, 0xdfff, 0x5116, addr);
            } else {
                assert(addr >= 0xe000 && addr <= 0xffff);
                data = INesMapper005ReadPRG(instance, 0xe000, 0xffff, 0x5117, addr);
            }
        } else {
            assert(mapper005->PRGBankMode == 3);
            if (addr >= 0x8000 && addr <= 0x9fff) {
                data = INesMapper005ReadPRG(instance, 0x8000, 0x9fff, 0x5114, addr);
            } else if (addr >= 0xa000 && addr <= 0xbfff) {
                data = INesMapper005ReadPRG(instance, 0xa000, 0xbfff, 0x5115, addr);
            } else if (addr >= 0xc000 && addr <= 0xdfff) {
                data = INesMapper005ReadPRG(instance, 0xc000, 0xdfff, 0x5116, addr);
            } else {
                assert(addr >= 0xe000 && addr <= 0xffff);
                data = INesMapper005ReadPRG(instance, 0xe000, 0xffff, 0x5117, addr);
            }
        }
    } else if (addr >= 0 && addr <= 0x1fff) {
        if (instance->ppu->sps == 0 || instance->ppu->fetchSprite) {
            // 8x8
            if (mapper005->CHRBankMode == CHR_PAGE_8KB_FLAG) {
                data = INesMapper005ReadCHR(instance, 0x0000, 0x1fff, 0x5127, addr);
            } else if (mapper005->CHRBankMode == CHR_PAGE_4KB_FLAG) {
                if (addr <= 0xfff) {
                    data = INesMapper005ReadCHR(instance, 0x0000, 0x0fff, 0x5123, addr);
                } else {
                    data = INesMapper005ReadCHR(instance, 0x1000, 0x1fff, 0x5127, addr);
                }
            } else if (mapper005->CHRBankMode == CHR_PAGE_2KB_FLAG) {
                if (addr <= 0x07ff) {
                    data = INesMapper005ReadCHR(instance, 0x0000, 0x07ff, 0x5121, addr);
                } else if (addr <= 0x0fff) {
                    data = INesMapper005ReadCHR(instance, 0x0800, 0x0fff, 0x5123, addr);
                } else if (addr <= 0x17ff) {
                    data = INesMapper005ReadCHR(instance, 0x1000, 0x17ff, 0x5125, addr);
                } else if (addr <= 0x1fff) {
                    data = INesMapper005ReadCHR(instance, 0x1800, 0x1fff, 0x5127, addr);
                }
            } else if (mapper005->CHRBankMode == CHR_PAGE_1KB_FLAG) {
                if (addr <= 0x03ff) {
                    data = INesMapper005ReadCHR(instance, 0x0000, 0x03ff, 0x5120, addr);
                } else if (addr <= 0x07ff) {
                    data = INesMapper005ReadCHR(instance, 0x0400, 0x07ff, 0x5121, addr);
                } else if (addr <= 0x0bff) {
                    data = INesMapper005ReadCHR(instance, 0x0800, 0x0bff, 0x5122, addr);
                } else if (addr <= 0x0fff) {
                    data = INesMapper005ReadCHR(instance, 0x0c00, 0x0fff, 0x5123, addr);
                } else if (addr <= 0x13ff) {
                    data = INesMapper005ReadCHR(instance, 0x1000, 0x13ff, 0x5124, addr);
                } else if (addr <= 0x17ff) {
                    data = INesMapper005ReadCHR(instance, 0x1400, 0x17ff, 0x5125, addr);
                } else if (addr <= 0x1bff) {
                    data = INesMapper005ReadCHR(instance, 0x1800, 0x1bff, 0x5126, addr);
                } else if (addr <= 0x1fff) {
                    data = INesMapper005ReadCHR(instance, 0x1c00, 0x1fff, 0x5127, addr);
                }
            }
        } else {
            // 8x16
            if (mapper005->CHRBankMode == CHR_PAGE_8KB_FLAG) {
                data = INesMapper005ReadCHR(instance, 0x0000, 0x1fff, 0x512b, addr);
            } else if (mapper005->CHRBankMode == CHR_PAGE_4KB_FLAG) {
                if (addr >= 0x0000 && addr <= 0x0fff) {
                    data = INesMapper005ReadCHR(instance, 0x0000, 0x0fff, 0x512b, addr);
                } else {
                    assert(addr >= 0x1000 && addr <= 0x1fff);
                    data = INesMapper005ReadCHR(instance, 0x1000, 0x1fff, 0x512b, addr);
                }
            } else if (mapper005->CHRBankMode == CHR_PAGE_2KB_FLAG) {
                if (addr >= 0x0000 && addr <= 0x07ff) {
                    data = INesMapper005ReadCHR(instance, 0x0000, 0x07ff, 0x5129, addr);
                } else if (addr >= 0x1000 && addr <= 0x17ff) {
                    data = INesMapper005ReadCHR(instance, 0x1000, 0x17ff, 0x5129, addr);
                } else if (addr >= 0x0800 && addr <= 0x0fff) {
                    data = INesMapper005ReadCHR(instance, 0x0800, 0x0fff, 0x512b, addr);
                } else {
                    assert(addr >= 0x1800 && addr <= 0x18ff);
                    data = INesMapper005ReadCHR(instance, 0x1800, 0x18ff, 0x512b, addr);
                }
            } else {
                assert(mapper005->CHRBankMode == CHR_PAGE_1KB_FLAG);
                if (addr >= 0x0000 && addr <= 0x03ff) {
                    data = INesMapper005ReadCHR(instance, 0x0000, 0x03ff, 0x5128, addr);
                } else if (addr >= 0x1000 && addr <= 0x13ff) {
                    data = INesMapper005ReadCHR(instance, 0x1000, 0x13ff, 0x5128, addr);
                } else if (addr >= 0x0400 && addr <= 0x07ff) {
                    data = INesMapper005ReadCHR(instance, 0x0400, 0x07ff, 0x5129, addr);
                } else if (addr >= 0x1400 && addr <= 0x17ff) {
                    data = INesMapper005ReadCHR(instance, 0x1400, 0x17ff, 0x5129, addr);
                } else if (addr >= 0x0800 && addr <= 0x0bff) {
                    data = INesMapper005ReadCHR(instance, 0x0800, 0x0bff, 0x512a, addr);
                } else if (addr >= 0x1800 && addr <= 0x1bff) {
                    data = INesMapper005ReadCHR(instance, 0x1800, 0x1bff, 0x512a, addr);
                } else if (addr >= 0x0c00 && addr <= 0x0fff) {
                    data = INesMapper005ReadCHR(instance, 0x0c00, 0x0fff, 0x512b, addr);
                } else {
                    assert(addr >= 0x1c00 && addr <= 0x1fff);
                    data = INesMapper005ReadCHR(instance, 0x1c00, 0x1fff, 0x512b, addr);
                }
            }
        }
    }
    
    return data;
}

void INesMapper005WriteNameTable(INesInstance* instance, uint16_t addr, uint8_t data) {
    uint8_t page = 0;
    uint16_t cvt = INesMapper005MappingNameTableAddr(instance, addr, &page);
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    
    if (page <= 1) {
        instance->mem[cvt] = data;
    } else if (page == 2) {
        mapper005->extendRAM[addr & 0x3ff] = data;
    }
    
    return;
}

uint8_t INesMapper005ReadNameTable(INesInstance* instance, uint16_t addr) {
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    uint8_t page = 0;
    uint16_t cvt = INesMapper005MappingNameTableAddr(instance, addr, &page);
    
    if (page <= 1) {
        return instance->mem[cvt];
    } else if (page == 2) {
        if (mapper005->extendRamMode <= 1) {
            return mapper005->extendRAM[addr & 0x3ff];
        }
    } else {
        assert(page == 3);
        if ((cvt & 0x3ff) < 0x3c0) {
            return mapper005->fillModeTile;
        } else {
            return mapper005->fillModeColor;
        }
    }
    
    return 0;
}

void INesMapper005PPUTick(INesInstance* instance) {
    INesMapper005* mapper005 = (INesMapper005*)instance->mapper->data;
    INesPPU* ppu = instance->ppu;
    
    if ((ppu->ty == 261 || (ppu->ty >= 0 && ppu->ty <= 239)) && (ppu->spe || ppu->bge)) {
        mapper005->IRQScanlineInFrame = true;
    } else {
        mapper005->IRQScanlineInFrame = false;
    }
    
    if (instance->ppu->tx == 0 && mapper005->IRQScanlineInFrame) {
        if (mapper005->scanlineCounter == mapper005->IRQScanlineCmpVal && mapper005->IRQScanlineCmpVal) {
            mapper005->IRQScanlinePending = true;
            if (!instance->cpu->flag.I && mapper005->IRQScanlineEnable) {
                instance->cpu->irq = 1;
            }
        }
        mapper005->scanlineCounter++;
    }
    
    if (!mapper005->IRQScanlineInFrame) {
        mapper005->scanlineCounter = 0xff;
    }
}
