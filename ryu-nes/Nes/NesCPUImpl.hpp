#pragma once

#include <cstdint>
#include "iNesInstance.hpp"

struct INesInstance;

#define Core_PrintANZCV(tag) \
    printf("[%s:%d] ANZCV = [A: 0x%02x, N: %d, Z: %d, C: %d, V: %d]\n", tag, (int)__LINE__, \
            (int)s_cpu->registerA, s_cpu->flag.N, s_cpu->flag.Z, s_cpu->flag.C, s_cpu->flag.V)

#define FRAMES_PER_SECOND_NTSC          60
#define SCANLINE_PER_FRAME_NTSC         262
#define CPU_CYCLES_PER_SCANLINE_NTSC    113.33
#define RESOLUTION_WIDTH_NTSC           256
#define RESOLUTION_HEIGHT_NTSC          224
#define CORE_CPU_FREQUENCY_NTSC         1790000

#define CORE_CYCLES_DMA                 512

#define BRK_JUMP_ADDRESS (0xfffe)

#define CORE_CODE_ADC_IMMD              0x69
#define CORE_CODE_ADC_ZEROPAGE          0x65
#define CORE_CODE_ADC_ZEROPAGEIX        0x75
#define CORE_CODE_ADC_ABSOLUTE          0x6d
#define CORE_CODE_ADC_ABSOLUTEIX        0x7d
#define CORE_CODE_ADC_ABSOLUTEIY        0x79
#define CORE_CODE_ADC_PREIDX            0x61
#define CORE_CODE_ADC_POSTIDY           0x71
#define CORE_CODE_INC_ZEROPAGE          0xe6
#define CORE_CODE_INC_ZEROPAGE_IDX      0xf6
#define CORE_CODE_INC_ABSOLUTE          0xee
#define CORE_CODE_INC_ABSOLUTE_IDX      0xfe
#define CORE_CODE_INX                   0xe8
#define CORE_CODE_INY                   0xc8
#define CORE_CODE_SBC_IMMD              0xe9
#define CORE_CODE_SBC_ZEROPAGE          0xe5
#define CORE_CODE_SBC_ZEROPAGE_IDX      0xf5
#define CORE_CODE_SBC_ABSOLUTE          0xed
#define CORE_CODE_SBC_ABSOLUTEIX        0xfd
#define CORE_CODE_SBC_ABSOLUTEIY        0xf9
#define CORE_CODE_SBC_PREIDX            0xe1
#define CORE_CODE_SBC_POSTIDY           0xf1
#define CORE_CODE_DEC_ZEROPAGE          0xc6
#define CORE_CODE_DEC_ZEROPAGE_IDX      0xd6
#define CORE_CODE_DEC_ABSOLUTE          0xce
#define CORE_CODE_DEC_ABSOLUTE_IDX      0xde
#define CORE_CODE_DEX                   0xca
#define CORE_CODE_DEY                   0x88
#define CORE_CODE_AND_IMMD              0x29
#define CORE_CODE_AND_ZEROPAGE          0x25
#define CORE_CODE_AND_ZEROPAGE_IDX      0x35
#define CORE_CODE_AND_ABSOLUTE          0x2d
#define CORE_CODE_AND_ABSOLUTEIX        0x3d
#define CORE_CODE_AND_ABSOLUTEIY        0x39
#define CORE_CODE_AND_PREIDX            0x21
#define CORE_CODE_AND_POSTIDY           0x31
#define CORE_CODE_EOR_IMMD              0x49
#define CORE_CODE_EOR_ZEROPAGE          0x45
#define CORE_CODE_EOR_ZEROPAGE_IDX      0x55
#define CORE_CODE_EOR_ABSOLUTE          0x4d
#define CORE_CODE_EOR_ABSOLUTEIX        0x5d
#define CORE_CODE_EOR_ABSOLUTEIY        0x59
#define CORE_CODE_EOR_PREIDX            0x41
#define CORE_CODE_EOR_POSTIDY           0x51
#define CORE_CODE_ORA_IMMD              0x09
#define CORE_CODE_ORA_ZEROPAGE          0x05
#define CORE_CODE_ORA_ZEROPAGE_IDX      0x15
#define CORE_CODE_ORA_ABSOLUTE          0x0d
#define CORE_CODE_ORA_ABSOLUTEIX        0x1d
#define CORE_CODE_ORA_ABSOLUTEIY        0x19
#define CORE_CODE_ORA_PREIDX            0x01
#define CORE_CODE_ORA_POSTIDY           0x11
#define CORE_CODE_BIT_ZEROPAGE          0x24
#define CORE_CODE_BIT_ABSOLUTE          0x2c
#define CORE_CODE_ASL_REGISTER          0x0a
#define CORE_CODE_ASL_ZEROPAGE          0x06
#define CORE_CODE_ASL_ZEROPAGE_IDX      0x16
#define CORE_CODE_ASL_ABSOLUTE          0x0e
#define CORE_CODE_ASL_ABSOLUTEIX        0x1e
#define CORE_CODE_LSR_REGISTER          0x4a
#define CORE_CODE_LSR_ZEROPAGE          0x46
#define CORE_CODE_LSR_ZEROPAGE_IDX      0x56
#define CORE_CODE_LSR_ABSOLUTE          0x4e
#define CORE_CODE_LSR_ABSOLUTEIX        0x5e
#define CORE_CODE_ROL_REGISTER          0x2a
#define CORE_CODE_ROL_ZEROPAGE          0x26
#define CORE_CODE_ROL_ZEROPAGE_IDX      0x36
#define CORE_CODE_ROL_ABSOLUTE          0x2e
#define CORE_CODE_ROL_ABSOLUTEIX        0x3e
#define CORE_CODE_ROR_REGISTER          0x6a
#define CORE_CODE_ROR_ZEROPAGE          0x66
#define CORE_CODE_ROR_ZEROPAGE_IDX      0x76
#define CORE_CODE_ROR_ABSOLUTE          0x6e
#define CORE_CODE_ROR_ABSOLUTEIX        0x7e
#define CORE_CODE_BCC                   0x90
#define CORE_CODE_BCS                   0xb0
#define CORE_CODE_BEQ                   0xf0
#define CORE_CODE_BMI                   0x30
#define CORE_CODE_BNE                   0xd0
#define CORE_CODE_BPL                   0x10
#define CORE_CODE_BVC                   0x50
#define CORE_CODE_BVS                   0x70
#define CORE_CODE_BRK                   0x00
#define CORE_CODE_JMP_ABSOLUTE          0x4c
#define CORE_CODE_JMP_INDIRECT          0x6c
#define CORE_CODE_JSR                   0x20
#define CORE_CODE_RTI                   0x40
#define CORE_CODE_RTS                   0x60
#define CORE_CODE_CLC                   0x18
#define CORE_CODE_CLD                   0xd8
#define CORE_CODE_CLI                   0x58
#define CORE_CODE_CLV                   0xb8
#define CORE_CODE_SEC                   0x38
#define CORE_CODE_SED                   0xf8
#define CORE_CODE_SEI                   0x78
#define CORE_CODE_CMP_IMMD              0xc9
#define CORE_CODE_CMP_ZEROPAGE          0xc5
#define CORE_CODE_CMP_ZEROPAGEX_IDX     0xd5
#define CORE_CODE_CMP_ABSOLUTE          0xcd
#define CORE_CODE_CMP_ABSOLUTEIX        0xdd
#define CORE_CODE_CMP_ABSOLUTEIY        0xd9
#define CORE_CODE_CMP_PREIDX            0xc1
#define CORE_CODE_CMP_POSTIDY           0xd1
#define CORE_CODE_CPX_IMMD              0xe0
#define CORE_CODE_CPX_ZEROPAGE          0xe4
#define CORE_CODE_CPX_ABSOLUTE          0xec
#define CORE_CODE_CPY_IMMD              0xc0
#define CORE_CODE_CPY_ZEROPAGE          0xc4
#define CORE_CODE_CPY_ABSOLUTE          0xcc
#define CORE_CODE_LDA_IMMD              0xa9
#define CORE_CODE_LDA_ZEROPAGE          0xa5
#define CORE_CODE_LDA_ZEROPAGEIX        0xb5
#define CORE_CODE_LDA_ABSOLUTE          0xad
#define CORE_CODE_LDA_ABSOLUTEIX        0xbd
#define CORE_CODE_LDA_ABSOLUTEIY        0xb9
#define CORE_CODE_LDA_PREIDX            0xa1
#define CORE_CODE_LDA_POSTIDY           0xb1
#define CORE_CODE_LDX_IMMD              0xa2
#define CORE_CODE_LDX_ZEROPAGE          0xa6
#define CORE_CODE_LDX_ZEROPAGEIY        0xb6
#define CORE_CODE_LDX_ABSOLUTE          0xae
#define CORE_CODE_LDX_ABSOLUTEIY        0xbe
#define CORE_CODE_LDY_IMMD              0xa0
#define CORE_CODE_LDY_ZEROPAGE          0xa4
#define CORE_CODE_LDY_ZEROPAGEIX        0xb4
#define CORE_CODE_LDY_ABSOLUTE          0xac
#define CORE_CODE_LDY_ABSOLUTEIX        0xbc
#define CORE_CODE_STA_ZEROPAGE          0x85
#define CORE_CODE_STA_ZEROPAGEIX        0x95
#define CORE_CODE_STA_ABSOLUTE          0x8d
#define CORE_CODE_STA_ABSOLUTEIX        0x9d
#define CORE_CODE_STA_ABSOLUTEIY        0x99
#define CORE_CODE_STA_PREIDX            0x81
#define CORE_CODE_STA_POSTIDY           0x91
#define CORE_CODE_STX_ZEROPAGE          0x86
#define CORE_CODE_STX_ZEROPAGEIY        0x96
#define CORE_CODE_STX_ABSOLUTE          0x8e
#define CORE_CODE_STY_ZEROPAGE          0x84
#define CORE_CODE_STY_ZEROPAGEIX        0x94
#define CORE_CODE_STY_ABSOLUTE          0x8c
#define CORE_CODE_TAX                   0xaa
#define CORE_CODE_TAY                   0xa8
#define CORE_CODE_TSX                   0xba
#define CORE_CODE_TXA                   0x8a
#define CORE_CODE_TXS                   0x9a
#define CORE_CODE_TYA                   0x98
#define CORE_CODE_PHA                   0x48
#define CORE_CODE_PHP                   0x08
#define CORE_CODE_PLA                   0x68
#define CORE_CODE_PLP                   0x28
#define CORE_CODE_NOP                   0xea

// undocument instruction code
#define CORE_CODE_NOP_2                 0x1a
#define CORE_CODE_NOP_3                 0x3a
#define CORE_CODE_NOP_4                 0x5a
#define CORE_CODE_NOP_5                 0x7a
#define CORE_CODE_NOP_6                 0xda
#define CORE_CODE_NOP_7                 0xfa
#define CORE_CODE_NOP_ZEROPAGE          0x04
#define CORE_CODE_NOP_ZEROPAGE_2        0x44
#define CORE_CODE_NOP_ZEROPAGE_3        0x64
#define CORE_CODE_NOP_ZEROPAGEIX        0x14
#define CORE_CODE_NOP_ZEROPAGEIX_2      0x34
#define CORE_CODE_NOP_ZEROPAGEIX_3      0x54
#define CORE_CODE_NOP_ZEROPAGEIX_4      0x74
#define CORE_CODE_NOP_ZEROPAGEIX_5      0xd4
#define CORE_CODE_NOP_ZEROPAGEIX_6      0xf4
#define CORE_CODE_NOP_ABSOLUTE          0x0c
#define CORE_CODE_NOP_IMMD              0x80
#define CORE_CODE_NOP_IMMD_2            0x82
#define CORE_CODE_NOP_IMMD_3            0x89
#define CORE_CODE_NOP_IMMD_4            0xc2
#define CORE_CODE_NOP_IMMD_5            0xe2
#define CORE_CODE_NOP_ABSOLUTEIX        0x1c
#define CORE_CODE_NOP_ABSOLUTEIX_2      0x3c
#define CORE_CODE_NOP_ABSOLUTEIX_3      0x5c
#define CORE_CODE_NOP_ABSOLUTEIX_4      0x7c
#define CORE_CODE_NOP_ABSOLUTEIX_5      0xdc
#define CORE_CODE_NOP_ABSOLUTEIX_6      0xfc
#define CORE_CODE_LAX_ZEROPAGE          0xa7
#define CORE_CODE_LAX_ZEROPAGEIY        0xb7
#define CORE_CODE_LAX_ABSOLUTE          0xaf
#define CORE_CODE_LAX_ABSOLUTEIY        0xbf
#define CORE_CODE_LAX_PREIDX            0xa3
#define CORE_CODE_LAX_POSTIDY           0xb3
#define CORE_CODE_SAX_ZEROPAGE          0x87
#define CORE_CODE_SAX_ZEROPAGEIY        0x97
#define CORE_CODE_SAX_PREIDX            0x83
#define CORE_CODE_SAX_ABSOLUTE          0x8f
#define CORE_CODE_SBC_IMMD_2            0xeb
#define CORE_CODE_DCP_ZEROPAGE          0xc7
#define CORE_CODE_DCP_ZEROPAGEIX        0xd7
#define CORE_CODE_DCP_ABSOLUTE          0xcf
#define CORE_CODE_DCP_ABSOLUTEIX        0xdf
#define CORE_CODE_DCP_ABSOLUTEIY        0xdb
#define CORE_CODE_DCP_PREIDX            0xc3
#define CORE_CODE_DCP_POSTIDY           0xd3
#define CORE_CODE_ISB_ZEROPAGE          0xe7
#define CORE_CODE_ISB_ZEROPAGEIX        0xf7
#define CORE_CODE_ISB_ABSOLUTE          0xef
#define CORE_CODE_ISB_ABSOLUTEIX        0xff
#define CORE_CODE_ISB_ABSOLUTEIY        0xfb
#define CORE_CODE_ISB_PREIDX            0xe3
#define CORE_CODE_ISB_POSTIDY           0xf3
#define CORE_CODE_SLO_ZEROPAGE          0x07
#define CORE_CODE_SLO_ZEROPAGEIX        0x17
#define CORE_CODE_SLO_ABSOLUTE          0x0f
#define CORE_CODE_SLO_ABSOLUTEIX        0x1f
#define CORE_CODE_SLO_ABSOLUTEIY        0x1b
#define CORE_CODE_SLO_PREIDX            0x03
#define CORE_CODE_SLO_POSTIDY           0x13
#define CORE_CODE_RLA_ZEROPAGE          0x27
#define CORE_CODE_RLA_ZEROPAGEIX        0x37
#define CORE_CODE_RLA_ABSOLUTE          0x2f
#define CORE_CODE_RLA_ABSOLUTEIX        0x3f
#define CORE_CODE_RLA_ABSOLUTEIY        0x3b
#define CORE_CODE_RLA_PREIDX            0x23
#define CORE_CODE_RLA_POSTIDY           0x33
#define CORE_CODE_SRE_ZEROPAGE          0x47
#define CORE_CODE_SRE_ZEROPAGEIX        0x57
#define CORE_CODE_SRE_ABSOLUTE          0x4f
#define CORE_CODE_SRE_ABSOLUTEIX        0x5f
#define CORE_CODE_SRE_ABSOLUTEIY        0x5b
#define CORE_CODE_SRE_PREIDX            0x43
#define CORE_CODE_SRE_POSTIDY           0x53
#define CORE_CODE_RRA_ZEROPAGE          0x67
#define CORE_CODE_RRA_ZEROPAGEIX        0x77
#define CORE_CODE_RRA_ABSOLUTE          0x6f
#define CORE_CODE_RRA_ABSOLUTEIX        0x7f
#define CORE_CODE_RRA_ABSOLUTEIY        0x7b
#define CORE_CODE_RRA_PREIDX            0x63
#define CORE_CODE_RRA_POSTIDY           0x73

typedef uint16_t ADDR;

struct CPUInstruction {
    uint8_t code;
    const char* name;
    uint8_t size;
    uint8_t cycle;
    uint8_t crossPageType; // 0: no cross; 1: add one when cross; 2: same page add one and other add two
};

struct CPUClock {
    uint64_t totalClockCount;
    uint32_t clockCount;
};

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif
struct CPU2A03Flag {
    uint8_t C: 1; // Carry
    uint8_t Z: 1; // Zero
    uint8_t I: 1; // Interrupt Disable
    uint8_t D: 1; // Decimal, but BCD mode is banned in nes
    uint8_t B: 1; // Break
    uint8_t U: 1; // Unused
    uint8_t V: 1; // Overflow
    uint8_t N: 1; // Negative
};
struct CPU2A03 {
    // physical simulation
    uint8_t registerA;
    uint8_t registerX;
    uint8_t registerY;
    uint8_t stackPointer;
    union {
        CPU2A03Flag flag;
        uint8_t p;
    };
    uint16_t pc;

    // helper variables
    bool nmi;
    bool processingNmi;
    
    bool irq;
    bool processingIRQ;
    
    long long tick;
    uint8_t lastOpCode;
    long extraCycle;
    int lastCycle;
    long totalCycle;
    long clockCount;
    long totalClockCount;
    int crossPageCycle;
    uint8_t opcodeCycle;
    ADDR lastAddressing;
    CPUInstruction* info;
    bool cacheFlag;
    uint8_t opcode;
};
struct CPUInfo {
    uint8_t lastCycle;
    uint64_t totalCycles;
    uint16_t lastAddressing;
};
#ifdef _WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

void CpuInit(CPU2A03* cpu);
void CpuStealCycles(CPU2A03* cpu, int stealCount);
void CpuReset(CPU2A03* cpu, INesInstance* instance);
void CpuNMI(CPU2A03* cpu, INesInstance* instance);
bool CpuTick(CPU2A03* cpu, INesInstance* instance);
void CpuStep(CPU2A03* cpu, INesInstance* instance);

ADDR CpuExecuteReset(CPU2A03* cpu, INesInstance* instance);
ADDR CpuExecute(CPU2A03* cpu, INesInstance* instance, ADDR addr);

void PushStackWord(CPU2A03* cpu, INesInstance* instance, uint16_t word);
uint16_t PopStackWord(CPU2A03* cpu, INesInstance* instance);

void PushStackFlag(CPU2A03* cpu, INesInstance* instance);
void PopStackFlag(CPU2A03* cpu, INesInstance* instance);

