#include "iNesFile.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

static void INesFileLoadSaveRam(INesFile* iNesFile) {
    if (!iNesFile->saveRamFilePath || iNesFile->isSaveRamLoad) {
        return;
    }
    iNesFile->isSaveRamLoad = true;
    
    FILE* fp = fopen(iNesFile->saveRamFilePath, "rb");
    if (!fp) {
        return;
    }
    
    fread(iNesFile->PRGRam, iNesFile->PRGRamSize, 1, fp);
    fclose(fp);
}

INesFile* INesFileCreate(const uint8_t* data, size_t size) {
    INesFile* iNesFile = (INesFile*)malloc(sizeof(INesFile));
    memset(iNesFile, 0, sizeof(INesFile));
    if (size < 16) {
        INesFileDestroy(iNesFile);
        return NULL;
    }
    memcpy(iNesFile->header, data, 16);
    const uint8_t HeaderChecker[] = { 0x4e, 0x45, 0x53, 0x1a };
    if (memcmp(HeaderChecker, iNesFile->header, 4) != 0) {
        INesFileDestroy(iNesFile);
        return NULL;
    }
    
    size_t offset = 16;
    bool containsTrainer = INesFileContainsTrainer(iNesFile);
    if (containsTrainer) {
        iNesFile->trainer = (uint8_t*)malloc(512);
        memcpy(iNesFile->trainer, data + offset, 512);
        offset += 512;
    }
    
    size_t PRGRomSize = INesFileGetPRGRomSizeBytes(iNesFile);
    if (PRGRomSize == 0 || size < offset + PRGRomSize) {
        INesFileDestroy(iNesFile);
        return NULL;
    }
    
    iNesFile->PRGRom = (uint8_t*)malloc(PRGRomSize);
    iNesFile->PRGRomSize = PRGRomSize;
    iNesFile->PRGBankCount = PRGRomSize / 8192;
    memcpy(iNesFile->PRGRom, data + offset, PRGRomSize);
    offset += PRGRomSize;
    
    size_t CHRRomSize = INesFileGetCHRRomSizeBytes(iNesFile);
    if (CHRRomSize != 0) {
        if (size < offset + CHRRomSize) {
            INesFileDestroy(iNesFile);
            return NULL;
        }
        iNesFile->CHRRom = (uint8_t*)malloc(CHRRomSize);
        iNesFile->CHRRomSize = CHRRomSize;
        iNesFile->CHRBankCount = PRGRomSize / 1024;
        memcpy(iNesFile->CHRRom, data + offset, CHRRomSize);
        offset += CHRRomSize;
    } else {
        CHRRomSize = 0x2000;
        iNesFile->CHRRom = (uint8_t*)malloc(CHRRomSize);
        iNesFile->CHRRomSize = CHRRomSize;
        iNesFile->onlyCHRRam = true;
        memcpy(iNesFile->CHRRom, data + offset, CHRRomSize);
    }
    
    bool containsPlayChoiceInstRom = INesFileContainsPlayChoince10(iNesFile);
    if (containsPlayChoiceInstRom) {
        if (size < offset + 8192) {
            INesFileDestroy(iNesFile);
            return NULL;
        }
        iNesFile->INSTRom = (uint8_t*)malloc(8192);
        memcpy(iNesFile->INSTRom, data + offset, 8192);
        offset += 8192;
        
        if (size < offset + 16) {
            INesFileDestroy(iNesFile);
            return NULL;
        }
        iNesFile->PROMData = (uint8_t*)malloc(16);
        memcpy(iNesFile->PROMData, data + offset, 16);
        offset += 16;
        
        if (size < offset + 16) {
            INesFileDestroy(iNesFile);
            return NULL;
        }
        iNesFile->PROMCounterOut = (uint8_t*)malloc(16);
        memcpy(iNesFile->PROMCounterOut, data + offset, 16);
        offset += 16;
    }
    
    iNesFile->PRGRamSize = INesFileGetPRGRamSizeBytes(iNesFile);
    iNesFile->PRGRam = (uint8_t*)malloc(iNesFile->PRGRamSize);
    memset(iNesFile->PRGRam, 0, iNesFile->PRGRamSize);
    
    return iNesFile;
}

INesFileType INesFileGetFileType(const INesFile* iNesFile) {
    if ((iNesFile->header[7] & 0xc) == 0x08) {
        return INesFileTypeINes2;
    }
    
    if ((iNesFile->header[7] & 0xc) == 0x04) {
        return INesFileTypeArchaicINes;
    }
    
    if ((iNesFile->header[7] & 0xc) == 0x00 && iNesFile->header[12] == 0 &&
        iNesFile->header[13] == 0 && iNesFile->header[14] == 0 && iNesFile->header[15] == 0) {
        return INesFileTypeINes;
    }
    
    return INesFileTypeArchaicINes;
}

size_t INesFileGetPRGRomSizeUnitCount(const INesFile* iNesFile) {
    return (size_t)iNesFile->header[4];
}

size_t INesFileGetPRGRomSizeBytes(const INesFile* iNesFile) {
    return (size_t)iNesFile->header[4] * 16384;
}

size_t INesFileGetCHRRomSizeUnitCount(const INesFile* iNesFile) {
    return (size_t)iNesFile->header[5];
}

size_t INesFileGetCHRRomSizeBytes(const INesFile* iNesFile) {
    return (size_t)iNesFile->header[5] * 8192;
}

uint8_t INesFileGetFlag6(const INesFile* iNesFile) {
    return iNesFile->header[6];
}

uint8_t INesFileGetFlag7(const INesFile* iNesFile) {
    return iNesFile->header[7];
}

uint8_t INesFileGetFlag8(const INesFile* iNesFile) {
    return iNesFile->header[8];
}

uint8_t INesFileGetFlag9(const INesFile* iNesFile) {
    return iNesFile->header[9];
}

uint8_t INesFileGetFlag10(const INesFile* iNesFile) {
    return iNesFile->header[10];
}

INesFileMirroring INesFileGetMirroring(const INesFile* iNesFile) {
    return (INesFileMirroring)(iNesFile->header[6] & 1);
}

bool INesFileContainsMemoryChip(const INesFile* iNesFile) {
    return iNesFile->header[6] & 2;
}

bool INesFileContainsTrainer(const INesFile* iNesFile) {
    return iNesFile->header[6] & 4;
}

bool INesFileUseFourScreenVRAM(const INesFile* iNesFile) {
    return iNesFile->header[6] & 8;
}

uint8_t INesFileGetLowerNybbleMapperNumber(const INesFile* iNesFile) {
    return (iNesFile->header[6] >> 4) & 0xf;
}

bool INesFileContainsVSSystem(const INesFile* iNesFile) {
    return iNesFile->header[7] & 1;
}

bool INesFileContainsPlayChoince10(const INesFile* iNesFile) {
    return iNesFile->header[7] & 2;
}

bool INesFileContainsFlags8To15InNes2(const INesFile* iNesFile) {
    return (iNesFile->header[7] & 0xc) == 0x8;
}

uint8_t INesFileGetHigherNybbleMapperNumber(const INesFile* iNesFile) {
    return (iNesFile->header[7] >> 4) & 0xf;
}

uint8_t INesFileGetMapperNumber(const INesFile* iNesFile) {
    return ((iNesFile->header[6] >> 4) & 0xf) | (iNesFile->header[7] & 0xf0);
}

size_t INesFileGetPRGRamSizeBytes(const INesFile* iNesFile) {
    if (iNesFile->header[8] == 0) {
        return 8192;
    }
    return (size_t)iNesFile->header[8] * 8192;
}

INesFileFlag9TVSystem INesFileGetFlag9TVSystem(const INesFile* iNesFile) {
    return (INesFileFlag9TVSystem)(iNesFile->header[9] & 1);
}

INesFileFlag10TVSystem INesFileGetFlag10TVSystem(const INesFile* iNesFile) {
    return (INesFileFlag10TVSystem)(iNesFile->header[10] & 3);
}

bool INesFileFlag10PRGRamPresent(const INesFile* iNesFile) {
    return !(iNesFile->header[10] & 0x10);
}

bool INesFileFlag10BoardConflics(const INesFile* iNesFile) {
    return (iNesFile->header[10] & 0x20);
}

void INesFileWriteRam(INesFile* iNesFile, size_t addr, uint8_t data) {
    if (addr >= iNesFile->PRGRamSize) {
        return;
    }
    INesFileLoadSaveRam(iNesFile);
    iNesFile->PRGRam[addr] = data;
}

uint8_t INesFileReadRam(INesFile* iNesFile, size_t addr) {
    if (addr >= iNesFile->PRGRamSize) {
        return 0;
    }
    INesFileLoadSaveRam(iNesFile);
    return iNesFile->PRGRam[addr];
}

uint8_t INesFileReadRom(INesFile* iNesFile, size_t addr) {
    if (addr >= iNesFile->PRGRomSize) {
        return 0;
    }
    return iNesFile->PRGRom[addr];
}

uint8_t INesFileReadChr(INesFile* iNesFile, size_t addr) {
    if (addr >= iNesFile->CHRRomSize) {
        return 0;
    }
    return iNesFile->CHRRom[addr];
}

void INesFileSaveRam(INesFile* iNesFile) {
    if (!iNesFile->saveRamFilePath || !INesFileContainsMemoryChip(iNesFile)) {
        return;
    }
    INesFileLoadSaveRam(iNesFile);
    FILE* fp = fopen(iNesFile->saveRamFilePath, "wb");
    if (!fp) {
        return;
    }
    fwrite(iNesFile->PRGRam, iNesFile->PRGRamSize, 1, fp);
    fclose(fp);
}

void INesFileDestroy(const INesFile* iNesFile) {
    if (iNesFile->filePath) {
        free(iNesFile->filePath);
    }
    
    if (iNesFile->saveRamFilePath) {
        free(iNesFile->saveRamFilePath);
    }
    
    if (iNesFile->trainer) {
        free(iNesFile->trainer);
    }
    
    if (iNesFile->PRGRom) {
        free(iNesFile->PRGRom);
    }
    
    if (iNesFile->CHRRom) {
        free(iNesFile->CHRRom);
    }
    
    if (iNesFile->INSTRom) {
        free(iNesFile->INSTRom);
    }
    
    if (iNesFile->PROMData) {
        free(iNesFile->PROMData);
    }
    
    if (iNesFile->PROMCounterOut) {
        free(iNesFile->PROMCounterOut);
    }
    
    if (iNesFile->PRGRam) {
        free(iNesFile->PRGRam);
    }
}
