#ifndef iNesFile_hpp
#define iNesFile_hpp

#include <stdio.h>
#include <stdint.h>

enum INesFileMirroring {
    INesFileMirroringHorizontal = 0,
    INesFileMirroringVertical = 1
};

enum INesFileFlag9TVSystem {
    INesFileFlag9TVSystemNTSC = 0,
    INesFileFlag9TVSystemPAL = 1
};

enum INesFileFlag10TVSystem {
    INesFileFlag10TVSystemNTSC = 0,
    INesFileFlag10TVSystemPAL = 2,
    INesFileFlag10TVSystemDUALCompact1 = 1,
    INesFileFlag10TVSystemDUALCompact3 = 3
};

enum INesFileType {
    INesFileTypeUnknown = 0,
    INesFileTypeArchaicINes = 1,
    INesFileTypeINes = 2,
    INesFileTypeINes2 = 3
};

struct INesFile {
    char* filePath;
    size_t filePathLength;
    char* saveRamFilePath;
    size_t saveRamFilePathLength;
    uint8_t header[16];
    uint8_t* trainer;
    uint8_t* PRGRom;
    size_t PRGRomSize;
    size_t PRGBankCount;
    uint8_t* CHRRom;
    size_t CHRRomSize;
    size_t CHRBankCount;
    uint8_t* INSTRom;
    uint8_t* PROMData;
    uint8_t* PROMCounterOut;
    uint8_t* PRGRam;
    size_t PRGRamSize;
    bool isSaveRamLoad;
    bool onlyCHRRam;
};

INesFile* INesFileCreate(const uint8_t* data, size_t size);
INesFileType INesFileGetFileType(const INesFile* iNesFile);
size_t INesFileGetPRGRomSizeUnitCount(const INesFile* iNesFile);
size_t INesFileGetPRGRomSizeBytes(const INesFile* iNesFile);
size_t INesFileGetCHRRomSizeUnitCount(const INesFile* iNesFile);
size_t INesFileGetCHRRomSizeBytes(const INesFile* iNesFile);
uint8_t INesFileGetFlag6(const INesFile* iNesFile);
uint8_t INesFileGetFlag7(const INesFile* iNesFile);
uint8_t INesFileGetFlag8(const INesFile* iNesFile);
uint8_t INesFileGetFlag9(const INesFile* iNesFile);
uint8_t INesFileGetFlag10(const INesFile* iNesFile);
INesFileMirroring INesFileGetMirroring(const INesFile* iNesFile);
bool INesFileContainsMemoryChip(const INesFile* iNesFile);
bool INesFileContainsTrainer(const INesFile* iNesFile);
bool INesFileUseFourScreenVRAM(const INesFile* iNesFile);
uint8_t INesFileGetLowerNybbleMapperNumber(const INesFile* iNesFile);
bool INesFileContainsVSSystem(const INesFile* iNesFile);
bool INesFileContainsPlayChoince10(const INesFile* iNesFile);
bool INesFileContainsFlags8To15InNes2(const INesFile* iNesFile);
uint8_t INesFileGetHigherNybbleMapperNumber(const INesFile* iNesFile);
uint8_t INesFileGetMapperNumber(const INesFile* iNesFile);
size_t INesFileGetPRGRamSizeBytes(const INesFile* iNesFile);
INesFileFlag9TVSystem INesFileGetFlag9TVSystem(const INesFile* iNesFile);
INesFileFlag10TVSystem INesFileGetFlag10TVSystem(const INesFile* iNesFile);
bool INesFileFlag10PRGRamPresent(const INesFile* iNesFile);
bool INesFileFlag10BoardConflics(const INesFile* iNesFile);
void INesFileWriteRam(INesFile* iNesFile, size_t addr, uint8_t data);
uint8_t INesFileReadRam(INesFile* iNesFile, size_t addr);
uint8_t INesFileReadRom(INesFile* iNesFile, size_t addr);
uint8_t INesFileReadChr(INesFile* iNesFile, size_t addr);
void INesFileSaveRam(INesFile* iNesFile);
void INesFileDestroy(const INesFile* iNesFile);

#endif /* iNesFile_hpp */
