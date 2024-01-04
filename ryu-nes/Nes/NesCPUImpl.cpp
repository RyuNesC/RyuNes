#include "NesCPUImpl.hpp"
#include <stdio.h>
#include <assert.h>
#include <string.h>

const static uint8_t NMI_CLOCK_CYCLE = 7;
const static uint8_t IRQ_CLOCK_CYCLE = 7;

static CPUInstruction g_instructionBook[0x100] = { 0 };

const CPUInstruction* GetCPUInstructionBook(int* pSize) {
    if (pSize) {
        *pSize = (int)sizeof(g_instructionBook)/sizeof(g_instructionBook[0]);
    }
    return g_instructionBook;
}

void CpuInit(CPU2A03* cpu) {
    g_instructionBook[CORE_CODE_ADC_IMMD]           = { 0x69, "adc_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_ADC_ZEROPAGE]       = { 0x65, "adc_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_ADC_ZEROPAGEIX]     = { 0x75, "adc_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_ADC_ABSOLUTE]       = { 0x6d, "adc_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_ADC_ABSOLUTEIX]     = { 0x7d, "adc_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_ADC_ABSOLUTEIY]     = { 0x79, "adc_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_ADC_PREIDX]         = { 0x61, "adc_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_ADC_POSTIDY]        = { 0x71, "adc_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_INC_ZEROPAGE]       = { 0xe6, "inc_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_INC_ZEROPAGE_IDX]   = { 0xf6, "inc_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_INC_ABSOLUTE]       = { 0xee, "inc_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_INC_ABSOLUTE_IDX]   = { 0xfe, "inc_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_INX]                = { 0xe8, "inx",            1, 2, 0 };
    g_instructionBook[CORE_CODE_INY]                = { 0xc8, "iny",            1, 2, 0 };
    g_instructionBook[CORE_CODE_SBC_IMMD]           = { 0xe9, "sbc_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_SBC_ZEROPAGE]       = { 0xe5, "sbc_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_SBC_ZEROPAGE_IDX]   = { 0xf5, "sbc_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_SBC_ABSOLUTE]       = { 0xed, "sbc_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_SBC_ABSOLUTEIX]     = { 0xfd, "sbc_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_SBC_ABSOLUTEIY]     = { 0xf9, "sbc_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_SBC_PREIDX]         = { 0xe1, "sbc_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_SBC_POSTIDY]        = { 0xf1, "sbc_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_DEC_ZEROPAGE]       = { 0xc6, "dec_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_DEC_ZEROPAGE_IDX]   = { 0xd6, "dec_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_DEC_ABSOLUTE]       = { 0xce, "dec_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_DEC_ABSOLUTE_IDX]   = { 0xde, "dec_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_DEX]                = { 0xca, "dex",            1, 2, 0 };
    g_instructionBook[CORE_CODE_DEY]                = { 0x88, "dey",            1, 2, 0 };
    g_instructionBook[CORE_CODE_AND_IMMD]           = { 0x29, "and_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_AND_ZEROPAGE]       = { 0x25, "and_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_AND_ZEROPAGE_IDX]   = { 0x35, "and_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_AND_ABSOLUTE]       = { 0x2d, "and_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_AND_ABSOLUTEIX]     = { 0x3d, "and_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_AND_ABSOLUTEIY]     = { 0x39, "and_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_AND_PREIDX]         = { 0x21, "and_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_AND_POSTIDY]        = { 0x31, "and_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_EOR_IMMD]           = { 0x49, "eor_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_EOR_ZEROPAGE]       = { 0x45, "eor_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_EOR_ZEROPAGE_IDX]   = { 0x55, "eor_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_EOR_ABSOLUTE]       = { 0x4d, "eor_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_EOR_ABSOLUTEIX]     = { 0x5d, "eor_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_EOR_ABSOLUTEIY]     = { 0x59, "eor_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_EOR_PREIDX]         = { 0x41, "eor_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_EOR_POSTIDY]        = { 0x51, "eor_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_ORA_IMMD]           = { 0x09, "ora_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_ORA_ZEROPAGE]       = { 0x05, "ora_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_ORA_ZEROPAGE_IDX]   = { 0x15, "ora_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_ORA_ABSOLUTE]       = { 0x0d, "ora_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_ORA_ABSOLUTEIX]     = { 0x1d, "ora_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_ORA_ABSOLUTEIY]     = { 0x19, "ora_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_ORA_PREIDX]         = { 0x01, "ora_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_ORA_POSTIDY]        = { 0x11, "ora_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_BIT_ZEROPAGE]       = { 0x24, "bit_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_BIT_ABSOLUTE]       = { 0x2c, "bit_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_ASL_REGISTER]       = { 0x0a, "asl_reg",        1, 2, 0 };
    g_instructionBook[CORE_CODE_ASL_ZEROPAGE]       = { 0x06, "asl_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_ASL_ZEROPAGE_IDX]   = { 0x16, "asl_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_ASL_ABSOLUTE]       = { 0x0e, "asl_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_ASL_ABSOLUTEIX]     = { 0x1e, "asl_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_LSR_REGISTER]       = { 0x4a, "lsr_reg",        1, 2, 0 };
    g_instructionBook[CORE_CODE_LSR_ZEROPAGE]       = { 0x46, "lsr_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_LSR_ZEROPAGE_IDX]   = { 0x56, "lsr_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_LSR_ABSOLUTE]       = { 0x4e, "lsr_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_LSR_ABSOLUTEIX]     = { 0x5e, "lsr_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_ROL_REGISTER]       = { 0x2a, "rol_reg",        1, 2, 0 };
    g_instructionBook[CORE_CODE_ROL_ZEROPAGE]       = { 0x26, "rol_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_ROL_ZEROPAGE_IDX]   = { 0x36, "rol_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_ROL_ABSOLUTE]       = { 0x2e, "rol_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_ROL_ABSOLUTEIX]     = { 0x3e, "rol_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_ROR_REGISTER]       = { 0x6a, "ror_reg",        1, 2, 0 };
    g_instructionBook[CORE_CODE_ROR_ZEROPAGE]       = { 0x66, "ror_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_ROR_ZEROPAGE_IDX]   = { 0x76, "ror_zpx",        3, 6, 0 };
    g_instructionBook[CORE_CODE_ROR_ABSOLUTE]       = { 0x6e, "ror_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_ROR_ABSOLUTEIX]     = { 0x7e, "ror_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_BCC]                = { 0x90, "bcc_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BCS]                = { 0xb0, "bcs_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BEQ]                = { 0xf0, "beq_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BMI]                = { 0x30, "bmi_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BNE]                = { 0xd0, "bne_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BPL]                = { 0x10, "bpl_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BVC]                = { 0x50, "bvc_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BVS]                = { 0x70, "bvs_rel",        2, 2, 2 };
    g_instructionBook[CORE_CODE_BRK]                = { 0x00, "brk",            1, 7, 0 };
    g_instructionBook[CORE_CODE_JMP_ABSOLUTE]       = { 0x4c, "jmp_abs",        3, 3, 0 };
    g_instructionBook[CORE_CODE_JMP_INDIRECT]       = { 0x6c, "jmp_ind",        3, 5, 0 };
    g_instructionBook[CORE_CODE_JSR]                = { 0x20, "jsr_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_RTI]                = { 0x40, "rti",            1, 6, 0 };
    g_instructionBook[CORE_CODE_RTS]                = { 0x60, "rts",            1, 6, 0 };
    g_instructionBook[CORE_CODE_CLC]                = { 0x18, "clc",            1, 2, 0 };
    g_instructionBook[CORE_CODE_CLD]                = { 0xd8, "cld",            1, 2, 0 };
    g_instructionBook[CORE_CODE_CLI]                = { 0x58, "cli",            1, 2, 0 };
    g_instructionBook[CORE_CODE_CLV]                = { 0xb8, "clv",            1, 2, 0 };
    g_instructionBook[CORE_CODE_SEC]                = { 0x38, "sec",            1, 2, 0 };
    g_instructionBook[CORE_CODE_SED]                = { 0xf8, "sed",            1, 2, 0 };
    g_instructionBook[CORE_CODE_SEI]                = { 0x78, "sei",            1, 2, 0 };
    g_instructionBook[CORE_CODE_CMP_IMMD]           = { 0xc9, "cmp_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_CMP_ZEROPAGE]       = { 0xc5, "cmp_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_CMP_ZEROPAGEX_IDX]  = { 0xd5, "cmp_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_CMP_ABSOLUTE]       = { 0xcd, "cmp_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_CMP_ABSOLUTEIX]     = { 0xdd, "cmp_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_CMP_ABSOLUTEIY]     = { 0xd9, "cmp_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_CMP_PREIDX]         = { 0xc1, "cmp_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_CMP_POSTIDY]        = { 0xd1, "cmp_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_CPX_IMMD]           = { 0xe0, "cpx_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_CPX_ZEROPAGE]       = { 0xe4, "cpx_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_CPX_ABSOLUTE]       = { 0xec, "cpx_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_CPY_IMMD]           = { 0xc0, "cpy_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_CPY_ZEROPAGE]       = { 0xc4, "cpy_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_CPY_ABSOLUTE]       = { 0xcc, "cpy_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_LDA_IMMD]           = { 0xa9, "lda_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_LDA_ZEROPAGE]       = { 0xa5, "lda_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_LDA_ZEROPAGEIX]     = { 0xb5, "lda_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_LDA_ABSOLUTE]       = { 0xad, "lda_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_LDA_ABSOLUTEIX]     = { 0xbd, "lda_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_LDA_ABSOLUTEIY]     = { 0xb9, "lda_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_LDA_PREIDX]         = { 0xa1, "lda_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_LDA_POSTIDY]        = { 0xb1, "lda_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_LDX_IMMD]           = { 0xa2, "ldx_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_LDX_ZEROPAGE]       = { 0xa6, "ldx_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_LDX_ZEROPAGEIY]     = { 0xb6, "ldx_zpy",        2, 4, 0 };
    g_instructionBook[CORE_CODE_LDX_ABSOLUTE]       = { 0xae, "ldx_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_LDX_ABSOLUTEIY]     = { 0xbe, "ldx_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_LDY_IMMD]           = { 0xa0, "ldy_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_LDY_ZEROPAGE]       = { 0xa4, "ldy_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_LDY_ZEROPAGEIX]     = { 0xb4, "ldy_zpy",        2, 4, 0 };
    g_instructionBook[CORE_CODE_LDY_ABSOLUTE]       = { 0xac, "ldy_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_LDY_ABSOLUTEIX]     = { 0xbc, "ldy_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_STA_ZEROPAGE]       = { 0x85, "sta_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_STA_ZEROPAGEIX]     = { 0x95, "sta_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_STA_ABSOLUTE]       = { 0x8d, "sta_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_STA_ABSOLUTEIX]     = { 0x9d, "sta_absx",       3, 5, 0 };
    g_instructionBook[CORE_CODE_STA_ABSOLUTEIY]     = { 0x99, "sta_absy",       3, 5, 0 };
    g_instructionBook[CORE_CODE_STA_PREIDX]         = { 0x81, "sta_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_STA_POSTIDY]        = { 0x91, "sta_posty",      2, 6, 0 };
    g_instructionBook[CORE_CODE_STX_ZEROPAGE]       = { 0x86, "stx_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_STX_ZEROPAGEIY]     = { 0x96, "stx_zpy",        2, 4, 0 };
    g_instructionBook[CORE_CODE_STX_ABSOLUTE]       = { 0x8e, "stx_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_STY_ZEROPAGE]       = { 0x84, "sty_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_STY_ZEROPAGEIX]     = { 0x94, "sty_zpx",        2, 4, 0 };
    g_instructionBook[CORE_CODE_STY_ABSOLUTE]       = { 0x8c, "sty_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_TAX]                = { 0xaa, "tax",            1, 2, 0 };
    g_instructionBook[CORE_CODE_TAY]                = { 0xa8, "tay",            1, 2, 0 };
    g_instructionBook[CORE_CODE_TSX]                = { 0xba, "tsx",            1, 2, 0 };
    g_instructionBook[CORE_CODE_TXA]                = { 0x8a, "txa",            1, 2, 0 };
    g_instructionBook[CORE_CODE_TXS]                = { 0x9a, "txs",            1, 2, 0 };
    g_instructionBook[CORE_CODE_TYA]                = { 0x98, "tya",            1, 2, 0 };
    g_instructionBook[CORE_CODE_PHA]                = { 0x48, "pha",            1, 3, 0 };
    g_instructionBook[CORE_CODE_PHP]                = { 0x08, "php",            1, 3, 0 };
    g_instructionBook[CORE_CODE_PLA]                = { 0x68, "pla",            1, 4, 0 };
    g_instructionBook[CORE_CODE_PLP]                = { 0x28, "plp",            1, 4, 0 };
    g_instructionBook[CORE_CODE_NOP]                = { 0xea, "nop",            1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_2]              = { 0x1a, "nop_v2",         1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_3]              = { 0x3a, "nop_v3",         1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_4]              = { 0x5a, "nop_v4",         1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_5]              = { 0x7a, "nop_v5",         1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_6]              = { 0xda, "nop_v6",         1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_7]              = { 0xfa, "nop_v7",         1, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGE]       = { 0x04, "nop_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGE_2]     = { 0x44, "nop_zp_v2",      2, 3, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGE_3]     = { 0x64, "nop_zp_v3",      2, 3, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGEIX]     = { 0x14, "nop_zp_x",       2, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGEIX_2]   = { 0x34, "nop_zp_x_v2",    2, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGEIX_3]   = { 0x54, "nop_zp_x_v3",    2, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGEIX_4]   = { 0x74, "nop_zp_x_v4",    2, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGEIX_5]   = { 0xd4, "nop_zp_x_v5",    2, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_ZEROPAGEIX_6]   = { 0xf4, "nop_zp_x_v6",    2, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTE]       = { 0x0c, "nop_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_NOP_IMMD]           = { 0x80, "nop_immd",       2, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_IMMD_2]         = { 0x82, "nop_immd_v2",    2, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_IMMD_3]         = { 0x89, "nop_immd_v3",    2, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_IMMD_4]         = { 0xc2, "nop_immd_v4",    2, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_IMMD_5]         = { 0xe2, "nop_immd_v5",    2, 2, 0 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTEIX]     = { 0x1c, "nop_absx",       3, 4, 1 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTEIX_2]   = { 0x3c, "nop_absx_v2",    3, 4, 1 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTEIX_3]   = { 0x5c, "nop_absx_v3",    3, 4, 1 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTEIX_4]   = { 0x7c, "nop_absx_v4",    3, 4, 1 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTEIX_5]   = { 0xdc, "nop_absx_v5",    3, 4, 1 };
    g_instructionBook[CORE_CODE_NOP_ABSOLUTEIX_6]   = { 0xfc, "nop_absx_v6",    3, 4, 1 };
    g_instructionBook[CORE_CODE_LAX_ZEROPAGE]       = { 0xa7, "lax_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_LAX_ZEROPAGEIY]     = { 0xb7, "lax_zpy",        2, 4, 0 };
    g_instructionBook[CORE_CODE_LAX_ABSOLUTE]       = { 0xaf, "lax_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_LAX_ABSOLUTEIY]     = { 0xbf, "lax_absy",       3, 4, 1 };
    g_instructionBook[CORE_CODE_LAX_PREIDX]         = { 0xa3, "lax_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_LAX_POSTIDY]        = { 0xb3, "lax_posty",      2, 5, 1 };
    g_instructionBook[CORE_CODE_SAX_ZEROPAGE]       = { 0x87, "sax_zp",         2, 3, 0 };
    g_instructionBook[CORE_CODE_SAX_ZEROPAGEIY]     = { 0x97, "sax_zpy",        2, 4, 0 };
    g_instructionBook[CORE_CODE_SAX_PREIDX]         = { 0x83, "sax_prex",       2, 6, 0 };
    g_instructionBook[CORE_CODE_SAX_ABSOLUTE]       = { 0x8f, "sax_abs",        3, 4, 0 };
    g_instructionBook[CORE_CODE_SBC_IMMD_2]         = { 0xeb, "sbc_immd_v2",    2, 2, 0 };
    g_instructionBook[CORE_CODE_DCP_ZEROPAGE]       = { 0xc7, "dcp_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_DCP_ZEROPAGEIX]     = { 0xd7, "dcp_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_DCP_ABSOLUTE]       = { 0xcf, "dcp_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_DCP_ABSOLUTEIX]     = { 0xdf, "dcp_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_DCP_ABSOLUTEIY]     = { 0xdb, "dcp_absy",       3, 7, 0 };
    g_instructionBook[CORE_CODE_DCP_PREIDX]         = { 0xc3, "dcp_prex",       2, 8, 0 };
    g_instructionBook[CORE_CODE_DCP_POSTIDY]        = { 0xd3, "dcp_posty",      2, 8, 0 };
    g_instructionBook[CORE_CODE_ISB_ZEROPAGE]       = { 0xe7, "isb_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_ISB_ZEROPAGEIX]     = { 0xf7, "isb_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_ISB_ABSOLUTE]       = { 0xef, "isb_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_ISB_ABSOLUTEIX]     = { 0xff, "isb_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_ISB_ABSOLUTEIY]     = { 0xfb, "isb_absy",       3, 7, 0 };
    g_instructionBook[CORE_CODE_ISB_PREIDX]         = { 0xe3, "isb_prex",       2, 8, 0 };
    g_instructionBook[CORE_CODE_ISB_POSTIDY]        = { 0xf3, "isb_posty",      2, 8, 0 };
    g_instructionBook[CORE_CODE_SLO_ZEROPAGE]       = { 0x07, "slo_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_SLO_ZEROPAGEIX]     = { 0x17, "slo_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_SLO_ABSOLUTE]       = { 0x0f, "slo_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_SLO_ABSOLUTEIX]     = { 0x1f, "slo_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_SLO_ABSOLUTEIY]     = { 0x1b, "slo_absy",       3, 7, 0 };
    g_instructionBook[CORE_CODE_SLO_PREIDX]         = { 0x03, "slo_prex",       2, 8, 0 };
    g_instructionBook[CORE_CODE_SLO_POSTIDY]        = { 0x13, "slo_posty",      2, 8, 0 };
    g_instructionBook[CORE_CODE_RLA_ZEROPAGE]       = { 0x27, "rla_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_RLA_ZEROPAGEIX]     = { 0x37, "rla_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_RLA_ABSOLUTE]       = { 0x2f, "rla_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_RLA_ABSOLUTEIX]     = { 0x3f, "rla_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_RLA_ABSOLUTEIY]     = { 0x3b, "rla_absy",       3, 7, 0 };
    g_instructionBook[CORE_CODE_RLA_PREIDX]         = { 0x23, "rla_prex",       2, 8, 0 };
    g_instructionBook[CORE_CODE_RLA_POSTIDY]        = { 0x33, "rla_posty",      2, 8, 0 };
    g_instructionBook[CORE_CODE_SRE_ZEROPAGE]       = { 0x47, "sre_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_SRE_ZEROPAGEIX]     = { 0x57, "sre_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_SRE_ABSOLUTE]       = { 0x4f, "sre_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_SRE_ABSOLUTEIX]     = { 0x5f, "sre_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_SRE_ABSOLUTEIY]     = { 0x5b, "sre_absy",       3, 7, 0 };
    g_instructionBook[CORE_CODE_SRE_PREIDX]         = { 0x43, "sre_prex",       2, 8, 0 };
    g_instructionBook[CORE_CODE_SRE_POSTIDY]        = { 0x53, "sre_posty",      2, 8, 0 };
    g_instructionBook[CORE_CODE_RRA_ZEROPAGE]       = { 0x67, "rra_zp",         2, 5, 0 };
    g_instructionBook[CORE_CODE_RRA_ZEROPAGEIX]     = { 0x77, "rra_zpx",        2, 6, 0 };
    g_instructionBook[CORE_CODE_RRA_ABSOLUTE]       = { 0x6f, "rra_abs",        3, 6, 0 };
    g_instructionBook[CORE_CODE_RRA_ABSOLUTEIX]     = { 0x7f, "rra_absx",       3, 7, 0 };
    g_instructionBook[CORE_CODE_RRA_ABSOLUTEIY]     = { 0x7b, "rra_absy",       3, 7, 0 };
    g_instructionBook[CORE_CODE_RRA_PREIDX]         = { 0x63, "rra_prex",       2, 8, 0 };
    g_instructionBook[CORE_CODE_RRA_POSTIDY]        = { 0x73, "rra_posty",      2, 8, 0 };
}

static ADDR AddressingStackPoiner(CPU2A03* cpu) {
    ADDR result = 0x100 | (ADDR)cpu->stackPointer;
    cpu->lastAddressing = result;
    return result;
}

void PushStackWord(CPU2A03* cpu, INesInstance* instance, uint16_t word) {
    ADDR addr = AddressingStackPoiner(cpu);
    INesInstanceWrite(instance, addr, (uint8_t)(word>>8));
    --cpu->stackPointer;
    addr = AddressingStackPoiner(cpu);
    INesInstanceWrite(instance, addr, (uint8_t)(word&0xff));
    --cpu->stackPointer;
}

uint16_t PopStackWord(CPU2A03* cpu, INesInstance* instance) {
    ++cpu->stackPointer;
    ADDR addr = AddressingStackPoiner(cpu);
    uint8_t low = INesInstanceRead(instance, addr);
    ++cpu->stackPointer;
    addr = AddressingStackPoiner(cpu);
    AddressingStackPoiner(cpu);
    uint8_t high = INesInstanceRead(instance, addr);
    return ((uint16_t)high<<8) | (uint16_t)low;
}

static void PushStackByte(CPU2A03* cpu, INesInstance* instance, uint8_t byte) {
    ADDR addr = AddressingStackPoiner(cpu);
    INesInstanceWrite(instance, addr, byte);
    --cpu->stackPointer;
}

static uint8_t PopStackByte(CPU2A03* cpu, INesInstance* instance) {
    ++cpu->stackPointer;
    ADDR addr = AddressingStackPoiner(cpu);
    return INesInstanceRead(instance, addr);
}

void PushStackFlag(CPU2A03* cpu, INesInstance* instance) {
    CPU2A03Flag flag = cpu->flag;
    assert(sizeof(flag) == 1);
    flag.B = 1;
    ADDR addr = AddressingStackPoiner(cpu);
    uint8_t* pByte = (uint8_t*)&flag;
    INesInstanceWrite(instance, addr, *pByte);
    --cpu->stackPointer;
}

void PopStackFlag(CPU2A03* cpu, INesInstance* instance) {
    ++cpu->stackPointer;
    ADDR addr = AddressingStackPoiner(cpu);
    uint8_t byte = INesInstanceRead(instance, addr);
    cpu->flag = *(CPU2A03Flag*)(&byte);
    cpu->flag.B = 0;
    cpu->flag.U = 1;
}

// 立即寻址
static uint8_t LoadImmediately(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR result = INesInstanceRead(instance, addr);
    cpu->lastAddressing = result;
    return result;
}

// 零页寻址
static ADDR AddressingZeroPage(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR result = INesInstanceRead(instance, addr);
    cpu->lastAddressing = result;
    return result;
}

static uint8_t LoadZeroPage(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t targetAddr = AddressingZeroPage(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 零页X变址
static ADDR AddressingZeroPageIndexX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t zeroPage = INesInstanceRead(instance, addr);
    zeroPage += cpu->registerX;
    cpu->lastAddressing = zeroPage;
    return (ADDR)zeroPage;
}

static uint8_t LoadZeroPageIndexX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 零页Y变址
static ADDR AddressingZeroPageIndexY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t zeroPage = INesInstanceRead(instance, addr);
    zeroPage += cpu->registerY;
    cpu->lastAddressing = zeroPage;
    return (ADDR)zeroPage;
}

static uint8_t LoadZeroPageIndexY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingZeroPageIndexY(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 绝对寻址
static ADDR AddressingAbsolute(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t low = INesInstanceRead(instance, addr);
    uint8_t high = INesInstanceRead(instance, addr + 1);
    ADDR result = (ADDR)low | ((ADDR)high << 8);
    cpu->lastAddressing = result;
    return result;
}

// 相对寻址
static ADDR AddressingIndirect(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR jumpAddr = AddressingAbsolute(cpu, instance, addr);
    ADDR jumpAddr2 = (jumpAddr & 0xff00) | ((jumpAddr+1) & 0xff); // 模拟CPU的bug
    uint8_t low = INesInstanceRead(instance, jumpAddr);
    uint8_t high = INesInstanceRead(instance, jumpAddr2);
    ADDR result = (ADDR)low | ((ADDR)high<<8);
    cpu->lastAddressing = result;
    return result;
}

static uint8_t LoadAbsolute(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingAbsolute(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 绝对X变址
static ADDR AddressingAbsoluteOffsetX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t low = INesInstanceRead(instance, addr);
    uint8_t high = INesInstanceRead(instance, addr + 1);
    ADDR targetAddr = (ADDR)low | ((ADDR)high << 8);
    targetAddr += cpu->registerX;
    cpu->crossPageCycle = high != (uint8_t)(targetAddr >> 8);
    cpu->lastAddressing = targetAddr;
    return targetAddr;
}

static uint8_t LoadAbsoluteOffsetX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 绝对Y变址
static ADDR AddressingAbsoluteOffsetY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t low = INesInstanceRead(instance, addr);
    uint8_t high = INesInstanceRead(instance, addr + 1);
    ADDR targetAddr = ((ADDR)high << 8) | (ADDR)low;
    targetAddr += cpu->registerY;
    cpu->crossPageCycle = high != (uint8_t)(targetAddr >> 8);
    cpu->lastAddressing = targetAddr;
    return targetAddr;
}

static uint8_t LoadAbsoluteOffsetY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 变址间接
static ADDR AddressingPreIndirectX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t tmp = INesInstanceRead(instance, addr);
    tmp += cpu->registerX;
    uint8_t low = INesInstanceRead(instance, tmp);
    tmp += 1;
    uint8_t high = INesInstanceRead(instance, tmp);
    ADDR result = ((ADDR)high << 8) | (ADDR)low;
    cpu->lastAddressing = result;
    return result;
}

static uint8_t LoadPreIndirectX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

// 间接变址
static ADDR AddressingPostIndirectY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t tmp = INesInstanceRead(instance, addr);
    uint8_t low = INesInstanceRead(instance, tmp);
    tmp += 1;
    uint8_t high = INesInstanceRead(instance, tmp);
    ADDR targetAddr = (high << 8) | low;
    targetAddr += cpu->registerY;
    cpu->crossPageCycle = high != (targetAddr >> 8);
    cpu->lastAddressing = targetAddr;
    return targetAddr;
}

static uint8_t LoadPostIndirectY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr);
    return INesInstanceRead(instance, targetAddr);
}

static void SetFlagZN(CPU2A03* cpu, int8_t n) {
    cpu->flag.Z = n == 0 ? 1 : 0;
    cpu->flag.N = n < 0 ? 1 : 0;
}

static void CpuExecuteADC(CPU2A03* cpu, uint8_t op) {
    uint8_t ua = cpu->registerA;
    uint8_t ub = op;
    uint8_t uc = cpu->flag.C;
    int8_t sa = (int8_t)ua;
    int8_t sb = (int8_t)ub;
    int8_t sc = (int8_t)uc;
    uint16_t uSum16 = (uint16_t)ua + (uint16_t)ub + (uint16_t)uc;
    int16_t sSum16 = (int16_t)sa + (int16_t)sb + (int16_t)sc;
    int8_t sSum8 = sa + sb + sc;
    uint8_t uSum8 = ua + ub + uc;
    cpu->registerA = uSum8;
    cpu->flag.C = (uSum16 > 0xff) ? 1 : 0;
    cpu->flag.V = ((sSum16 > 0 && sSum8 < 0) || (sSum16 < 0 && sSum8 > 0)) ? 1 : 0;
    SetFlagZN(cpu, (int8_t)uSum8);
}

static void CpuExecuteSBC(CPU2A03* cpu, uint8_t op) {
    uint8_t ua = cpu->registerA;
    uint8_t ub = op;
    uint8_t uc = cpu->flag.C == 1 ? 0 : 1;
    int8_t sa = (int8_t)ua;
    int8_t sb = (int8_t)ub;
    int8_t sc = (int8_t)uc;
    uint16_t uSub16 = (uint16_t)ua - (uint16_t)ub - (uint16_t)uc;
    int16_t sSub16 = (int16_t)sa - (int16_t)sb - (int16_t)sc;
    int8_t sSub8 = sa - sb - sc;
    uint8_t uSub8 = ua - ub - uc;
    cpu->registerA = uSub8;
    cpu->flag.C = (((int16_t)uSub16) < 0) ? 0 : 1;
    cpu->flag.V = ((sSub16 > 0 && sSub8 < 0) || (sSub16 < 0 && sSub8 > 0)) ? 1 : 0;
    SetFlagZN(cpu, (int8_t)uSub8);
}

static void CpuExecuteINC(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    INesInstanceInc(instance, addr);
    int8_t n = (int8_t)INesInstanceRead(instance, addr);
    SetFlagZN(cpu, n);
}

static void CpuExecuteDEC(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    INesInstanceDec(instance, addr);
    int8_t n = (int8_t)INesInstanceRead(instance, addr);
    SetFlagZN(cpu, n);
}

static void CpuExecuteINX(CPU2A03* cpu) {
    cpu->registerX += 1;
    SetFlagZN(cpu, (int8_t)cpu->registerX);
}

static void CpuExecuteINY(CPU2A03* cpu) {
    cpu->registerY += 1;
    SetFlagZN(cpu, (int8_t)cpu->registerY);
}

static void CpuExecuteDEX(CPU2A03* cpu) {
    cpu->registerX -= 1;
    SetFlagZN(cpu, (int8_t)cpu->registerX);
}

static void CpuExecuteDEY(CPU2A03* cpu) {
    cpu->registerY -= 1;
    SetFlagZN(cpu, (int8_t)cpu->registerY);
}

static void CpuExecuteAND(CPU2A03* cpu, uint8_t op) {
    cpu->registerA &= op;
    SetFlagZN(cpu, (int8_t)cpu->registerA);
}

static void CpuExecuteXOR(CPU2A03* cpu, uint8_t op) {
    cpu->registerA ^= op;
    SetFlagZN(cpu, (int8_t)cpu->registerA);
}

static void CpuExecuteOR(CPU2A03* cpu, uint8_t op) {
    cpu->registerA |= op;
    SetFlagZN(cpu, (int8_t)cpu->registerA);
}

static void CpuExecuteBIT(CPU2A03* cpu, uint8_t op) {
    uint8_t val = cpu->registerA & op;
    cpu->flag.N = (op & 0x80) ? 1 : 0;
    cpu->flag.V = (op & 0x40) ? 1 : 0;
    cpu->flag.Z = val == 0 ? 1 : 0;
}

static void CpuExecuteASL(CPU2A03* cpu, INesInstance* instance, ADDR addr, uint8_t* outOp = NULL) {
    uint8_t op = INesInstanceRead(instance, addr);
    cpu->flag.C = (op & 0x80) ? 1 : 0;
    op <<= 1;
    SetFlagZN(cpu, (int8_t)op);
    INesInstanceWrite(instance, addr, op);
    if (outOp) {
        *outOp = op;
    }
}

static void CpuExecuteAccumulatorASL(CPU2A03* cpu) {
    uint8_t op = cpu->registerA;
    cpu->flag.C = (op & 0x80) ? 1 : 0;
    op <<= 1;
    SetFlagZN(cpu, (int8_t)op);
    cpu->registerA = op;
}

static void CpuExecuteLSR(CPU2A03* cpu, INesInstance* instance, ADDR addr, uint8_t* outOp = NULL) {
    uint8_t op = INesInstanceRead(instance, addr);
    cpu->flag.C = (op & 0x01) ? 1 : 0;
    op >>= 1;
    SetFlagZN(cpu, (int8_t)op);
    INesInstanceWrite(instance, addr, op);
    if (outOp) {
        *outOp = op;
    }
}

static void CpuExecuteAccumulatorLSR(CPU2A03* cpu) {
    uint8_t op = cpu->registerA;
    cpu->flag.C = (op & 0x01) ? 1 : 0;
    op >>= 1;
    SetFlagZN(cpu, (int8_t)op);
    cpu->registerA = op;
}

static void CpuExecuteROL(CPU2A03* cpu, INesInstance* instance, ADDR addr, uint8_t* outOp = NULL) {
    uint8_t op = INesInstanceRead(instance, addr);
    uint8_t tmp = (op >> 7);
    op = (op << 1) | cpu->flag.C;
    cpu->flag.C = tmp;
    SetFlagZN(cpu, (int8_t)op);
    INesInstanceWrite(instance, addr, op);
    if (outOp) {
        *outOp = op;
    }
}

static void CpuExecuteAccumulatorROL(CPU2A03* cpu) {
    uint8_t op = cpu->registerA;
    uint8_t tmp = (op >> 7);
    op = (op << 1) | cpu->flag.C;
    cpu->flag.C = tmp;
    SetFlagZN(cpu, (int8_t)op);
    cpu->registerA = op;
}

static void CpuExecuteROR(CPU2A03* cpu, INesInstance* instance, ADDR addr, uint8_t* outOp = NULL) {
    uint8_t op = INesInstanceRead(instance, addr);
    uint8_t tmp = op & 0x01;
    op = (op >> 1) | (cpu->flag.C << 7);
    cpu->flag.C = tmp;
    SetFlagZN(cpu, (int8_t)op);
    INesInstanceWrite(instance, addr, op);
    if (outOp) {
        *outOp = op;
    }
}

static void CpuExecuteAccumulatorROR(CPU2A03* cpu) {
    uint8_t op = cpu->registerA;
    uint8_t tmp = op & 0x01;
    op = (op >> 1) | (cpu->flag.C << 7);
    cpu->flag.C = tmp;
    SetFlagZN(cpu, (int8_t)op);
    cpu->registerA = op;
}

static void CpuExecuteCMP_A_B(CPU2A03* cpu, uint8_t a, uint8_t b) {
    SetFlagZN(cpu, (int8_t)(a - b));
    if (a >= b) {
        cpu->flag.C = 1;
    } else {
        cpu->flag.C = 0;
    }
}

static void CpuExecuteCMP(CPU2A03* cpu, uint8_t op) {
    CpuExecuteCMP_A_B(cpu, cpu->registerA, op);
}

static void CpuExecuteSetRegisterA(CPU2A03* cpu, uint8_t op) {
    cpu->registerA = op;
    SetFlagZN(cpu, (int8_t)op);
}

static void CpuExecuteSetRegisterX(CPU2A03* cpu, uint8_t op) {
    cpu->registerX = op;
    SetFlagZN(cpu, (int8_t)op);
}

static void CpuExecuteSetRegisterY(CPU2A03* cpu, uint8_t op) {
    cpu->registerY = op;
    SetFlagZN(cpu, (int8_t)op);
}

static void CpuExecuteStoreRegisterA(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    INesInstanceWrite(instance, addr, cpu->registerA);
}

static void CpuExecuteStoreRegisterX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    INesInstanceWrite(instance, addr, cpu->registerX);
}

static void CpuExecuteStoreRegisterY(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    INesInstanceWrite(instance, addr, cpu->registerY);
}

static void CpuExecuteLAX(CPU2A03* cpu, uint8_t op) {
    cpu->registerA = op;
    cpu->registerX = op;
    SetFlagZN(cpu, (int8_t)op);
}

static void CpuExecuteSAX(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t r = cpu->registerA & cpu->registerX;
    INesInstanceWrite(instance, addr, r);
}

static void CpuExecuteDCP(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t value = INesInstanceRead(instance, addr) - 1;
    INesInstanceWrite(instance, addr, value);
    CpuExecuteCMP_A_B(cpu, cpu->registerA, value);
}

static void CpuExecuteISB(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t value = INesInstanceRead(instance, addr) + 1;
    INesInstanceWrite(instance, addr, value);
    CpuExecuteSBC(cpu, value);
}

static void CpuExecuteSLO(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t op = 0;
    CpuExecuteASL(cpu, instance, addr, &op);
    CpuExecuteOR(cpu, op);
}

static void CpuExecuteRLA(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t op = 0;
    CpuExecuteROL(cpu, instance, addr, &op);
    CpuExecuteAND(cpu, op);
}

static void CpuExecuteSRE(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t op = 0;
    CpuExecuteLSR(cpu, instance, addr, &op);
    CpuExecuteXOR(cpu, op);
}

static void CpuExecuteRRA(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t op = 0;
    CpuExecuteROR(cpu, instance, addr, &op);
    CpuExecuteADC(cpu, op);
}

void CpuStealCycles(CPU2A03* cpu, int stealCount) {
    assert(stealCount > 0);
    cpu->extraCycle += stealCount;
}

void CpuReset(CPU2A03* cpu, INesInstance* instance) {
    cpu->pc = AddressingAbsolute(cpu, instance, 0xfffc);
    cpu->stackPointer = 0xfd;
    cpu->flag.I = 1;
    cpu->flag.U = 1;
}

void CpuNMI(CPU2A03* cpu, INesInstance* instance) {
    PushStackWord(cpu, instance, cpu->pc);
    PushStackFlag(cpu, instance);
    cpu->flag.I = 1;
    cpu->pc = AddressingAbsolute(cpu, instance, 0xfffa);
    cpu->lastCycle = NMI_CLOCK_CYCLE;
    cpu->totalCycle += NMI_CLOCK_CYCLE;
}

void CpuIRQ(CPU2A03* cpu, INesInstance* instance) {
    PushStackWord(cpu, instance, cpu->pc);
    PushStackFlag(cpu, instance);
    cpu->flag.I = 1;
    cpu->pc = AddressingAbsolute(cpu, instance, BRK_JUMP_ADDRESS);
    cpu->lastCycle = IRQ_CLOCK_CYCLE;
    cpu->totalCycle += IRQ_CLOCK_CYCLE;
}

bool CpuTick(CPU2A03* cpu, INesInstance* instance) {
    uint8_t code = cpu->opcode;
    if (!cpu->cacheFlag) {
        code = INesInstanceRead(instance, cpu->pc);
        cpu->opcode = code;
        cpu->cacheFlag = true;
    }
    CPUInstruction* pInst = g_instructionBook + code;
    cpu->info = pInst;
    ++cpu->tick;
    bool executedCmd = false;
    if (cpu->extraCycle > 0) {
        --cpu->extraCycle;
        return false;
    } else if (cpu->processingNmi || (cpu->nmi && cpu->clockCount == 1)) {
        cpu->processingNmi = true;
        if (cpu->clockCount == NMI_CLOCK_CYCLE) {
            cpu->cacheFlag = false;
            CpuNMI(cpu, instance);
            cpu->processingNmi = false;
            cpu->nmi = false;
            executedCmd = true;
            cpu->clockCount = 1;
        } else {
            ++cpu->clockCount;
        }
    } else if (cpu->processingIRQ || (cpu->irq && cpu->clockCount == 1)) {
        cpu->processingIRQ = true;
        if (cpu->clockCount == IRQ_CLOCK_CYCLE) {
            cpu->cacheFlag = false;
            CpuIRQ(cpu, instance);
            cpu->processingIRQ = false;
            cpu->irq = false;
            executedCmd = true;
            cpu->clockCount = 1;
        } else {
            ++cpu->clockCount;
        }
    } else {
        uint8_t guessCycle = pInst->cycle;
        if (cpu->clockCount == guessCycle) {
            CpuStep(cpu, instance);
            executedCmd = true;
            cpu->clockCount = 1;
            if (cpu->lastCycle != guessCycle) {
                // ryu: in some situation we need to "make up" one or more cycle,
                // this "make up" logic maybe not that accurate.
                // maybe I will change this logic in the future.
                assert(cpu->lastCycle > guessCycle);
                cpu->extraCycle = cpu->lastCycle - guessCycle;
            }
            cpu->cacheFlag = false;
        } else {
            ++cpu->clockCount;
            --cpu->lastCycle;
        }
    }
    
    ++cpu->totalClockCount;
    
    return executedCmd;
}

void CpuStep(CPU2A03* cpu, INesInstance* instance) {
    CpuExecute(cpu, instance, (ADDR)cpu->pc);
}

ADDR CpuExecuteReset(CPU2A03* cpu, INesInstance* instance) {
    return AddressingAbsolute(cpu, instance, 0xfffc);
}

ADDR CpuExecute(CPU2A03* cpu, INesInstance* instance, ADDR addr) {
    uint8_t opCode = cpu->opcode;
    if (!cpu->cacheFlag) {
        opCode = INesInstanceRead(instance, addr);
    }
    cpu->lastOpCode = opCode;
    cpu->pc = addr;
    cpu->crossPageCycle = 0;
    
    switch (opCode) {
        // ADC
        case CORE_CODE_ADC_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ADC_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ADC_ZEROPAGEIX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ADC_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ADC_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ADC_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ADC_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ADC_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteADC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // INC
        case CORE_CODE_INC_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteINC(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_INC_ZEROPAGE_IDX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteINC(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_INC_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteINC(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_INC_ABSOLUTE_IDX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteINC(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // INX
        case CORE_CODE_INX: {
            CpuExecuteINX(cpu);
            cpu->pc = addr + 1;
            break;
        }
        // INY
        case CORE_CODE_INY: {
            CpuExecuteINY(cpu);
            cpu->pc = addr + 1;
            break;
        }
        // SBC
        case CORE_CODE_SBC_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SBC_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SBC_ZEROPAGE_IDX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SBC_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SBC_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SBC_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SBC_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SBC_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // DEC
        case CORE_CODE_DEC_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteDEC(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_DEC_ZEROPAGE_IDX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteDEC(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_DEC_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteDEC(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_DEC_ABSOLUTE_IDX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteDEC(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // DEX
        case CORE_CODE_DEX: {
            CpuExecuteDEX(cpu);
            cpu->pc = addr + 1;
            break;
        }
        // DEY
        case CORE_CODE_DEY: {
            CpuExecuteDEY(cpu);
            cpu->pc = addr + 1;
            break;
        }
        // AND
        case CORE_CODE_AND_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_AND_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_AND_ZEROPAGE_IDX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_AND_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_AND_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_AND_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_AND_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_AND_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteAND(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // EOR
        case CORE_CODE_EOR_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_EOR_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_EOR_ZEROPAGE_IDX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_EOR_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_EOR_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_EOR_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_EOR_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_EOR_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteXOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // ORA
        case CORE_CODE_ORA_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ORA_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ORA_ZEROPAGE_IDX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ORA_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ORA_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ORA_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ORA_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ORA_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteOR(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // BIT
        case CORE_CODE_BIT_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteBIT(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_BIT_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteBIT(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        // ASL
        case CORE_CODE_ASL_REGISTER: {
            CpuExecuteAccumulatorASL(cpu);
            cpu->pc = addr + 1;
            break;
        }
        case CORE_CODE_ASL_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteASL(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ASL_ZEROPAGE_IDX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteASL(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ASL_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteASL(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ASL_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteASL(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // LSR
        case CORE_CODE_LSR_REGISTER: {
            CpuExecuteAccumulatorLSR(cpu);
            cpu->pc = addr + 1;
            break;
        }
        case CORE_CODE_LSR_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteLSR(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LSR_ZEROPAGE_IDX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteLSR(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LSR_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteLSR(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LSR_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteLSR(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // ROL
        case CORE_CODE_ROL_REGISTER: {
            CpuExecuteAccumulatorROL(cpu);
            cpu->pc = addr + 1;
            break;
        }
        case CORE_CODE_ROL_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteROL(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ROL_ZEROPAGE_IDX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteROL(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ROL_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteROL(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ROL_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteROL(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // ROR
        case CORE_CODE_ROR_REGISTER: {
            CpuExecuteAccumulatorROR(cpu);
            cpu->pc = addr + 1;
            break;
        }
        case CORE_CODE_ROR_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteROR(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ROR_ZEROPAGE_IDX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteROR(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ROR_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteROR(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ROR_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteROR(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // BCC
        case CORE_CODE_BCC: {
            if (cpu->flag.C == 0) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BCS
        case CORE_CODE_BCS: {
            if (cpu->flag.C) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BEQ
        case CORE_CODE_BEQ: {
            if (cpu->flag.Z) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BMI
        case CORE_CODE_BMI: {
            if (cpu->flag.N) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BNE
        case CORE_CODE_BNE: {
            if (cpu->flag.Z == 0) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BPL
        case CORE_CODE_BPL: {
            if (cpu->flag.N == 0) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BVC
        case CORE_CODE_BVC: {
            if (cpu->flag.V == 0) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BVS
        case CORE_CODE_BVS: {
            if (cpu->flag.V) {
                auto op = (int8_t)LoadImmediately(cpu, instance, addr + 1);
                cpu->pc = addr + 2 + op;
                cpu->crossPageCycle = (uint8_t)(cpu->pc >> 8) != (uint8_t)((addr + 2) >> 8) ? 2 : 1;
                break;
            } else {
                cpu->pc = addr + 2;
            }
            break;
        }
        // BRK
        case CORE_CODE_BRK: {
            PushStackWord(cpu, instance, addr + 2);
            PushStackFlag(cpu, instance);
            cpu->flag.B = 1;
            cpu->flag.I = 1;
            cpu->pc = AddressingAbsolute(cpu, instance, BRK_JUMP_ADDRESS);
            break;
        }
        // JMP
        case CORE_CODE_JMP_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            cpu->pc = targetAddr;
            break;
        }
        case CORE_CODE_JMP_INDIRECT: {
            ADDR targetAddr = AddressingIndirect(cpu, instance, addr + 1);
            cpu->pc = targetAddr;
            break;
        }
        // JSR
        case CORE_CODE_JSR: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            PushStackWord(cpu, instance, addr + 2);
            cpu->pc = targetAddr;
            break;
        }
        // RTI
        case CORE_CODE_RTI: {
            PopStackFlag(cpu, instance);
            ADDR targetAddr = PopStackWord(cpu, instance);
            cpu->pc = targetAddr;
            break;
        }
        // RTS
        case CORE_CODE_RTS: {
            ADDR targetAddr = PopStackWord(cpu, instance) + 1;
            cpu->pc = targetAddr;
            break;
        }
        // CLC
        case CORE_CODE_CLC: {
            cpu->flag.C = 0;
            cpu->pc = addr + 1;
            break;
        }
        // CLD
        case CORE_CODE_CLD: {
            cpu->flag.D = 0;
            cpu->pc = addr + 1;
            break;
        }
        // CLI
        case CORE_CODE_CLI: {
            cpu->flag.I = 0;
            cpu->pc = addr + 1;
            break;
        }
        // CLV
        case CORE_CODE_CLV: {
            cpu->flag.V = 0;
            cpu->pc = addr + 1;
            break;
        }
        // SEC
        case CORE_CODE_SEC: {
            cpu->flag.C = 1;
            cpu->pc = addr + 1;
            break;
        }
        // SED
        case CORE_CODE_SED: {
            cpu->flag.D = 1;
            cpu->pc = addr + 1;
            break;
        }
        // SEI
        case CORE_CODE_SEI: {
            cpu->flag.I = 1;
            cpu->pc = addr + 1;
            break;
        }
        // CMP
        case CORE_CODE_CMP_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CMP_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CMP_ZEROPAGEX_IDX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CMP_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_CMP_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_CMP_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_CMP_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CMP_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteCMP(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // CPX
        case CORE_CODE_CPX_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteCMP_A_B(cpu, cpu->registerX, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CPX_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteCMP_A_B(cpu, cpu->registerX, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CPX_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteCMP_A_B(cpu, cpu->registerX, op);
            cpu->pc = addr + 3;
            break;
        }
        // CPY
        case CORE_CODE_CPY_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteCMP_A_B(cpu, cpu->registerY, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CPY_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteCMP_A_B(cpu, cpu->registerY, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_CPY_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteCMP_A_B(cpu, cpu->registerY, op);
            cpu->pc = addr + 3;
            break;
        }
        // LDA
        case CORE_CODE_LDA_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDA_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDA_ZEROPAGEIX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDA_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LDA_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LDA_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LDA_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDA_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteSetRegisterA(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // LDX
        case CORE_CODE_LDX_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteSetRegisterX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDX_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteSetRegisterX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDX_ZEROPAGEIY: {
            uint8_t op = LoadZeroPageIndexY(cpu, instance, addr + 1);
            CpuExecuteSetRegisterX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDX_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteSetRegisterX(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LDX_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteSetRegisterX(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        // LDY
        case CORE_CODE_LDY_IMMD: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteSetRegisterY(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDY_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteSetRegisterY(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDY_ZEROPAGEIX: {
            uint8_t op = LoadZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteSetRegisterY(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LDY_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteSetRegisterY(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LDY_ABSOLUTEIX: {
            uint8_t op = LoadAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteSetRegisterY(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        // STA
        case CORE_CODE_STA_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STA_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STA_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_STA_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_STA_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_STA_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STA_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        // STX
        case CORE_CODE_STX_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterX(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STX_ZEROPAGEIY: {
            ADDR targetAddr = AddressingZeroPageIndexY(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterX(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STX_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterX(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // STY
        case CORE_CODE_STY_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterY(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STY_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterY(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_STY_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteStoreRegisterY(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // TAX
        case CORE_CODE_TAX: {
            cpu->registerX = cpu->registerA;
            SetFlagZN(cpu, (int8_t)cpu->registerX);
            cpu->pc = addr + 1;
            break;
        }
        // TAY
        case CORE_CODE_TAY: {
            cpu->registerY = cpu->registerA;
            SetFlagZN(cpu, (int8_t)cpu->registerY);
            cpu->pc = addr + 1;
            break;
        }
        // TSX
        case CORE_CODE_TSX: {
            cpu->registerX = cpu->stackPointer;
            SetFlagZN(cpu, cpu->registerX);
            cpu->pc = addr + 1;
            break;
        }
        // TXA
        case CORE_CODE_TXA: {
            cpu->registerA = cpu->registerX;
            SetFlagZN(cpu, cpu->registerA);
            cpu->pc = addr + 1;
            break;
        }
        // TXS
        case CORE_CODE_TXS: {
            cpu->stackPointer = cpu->registerX;
            cpu->pc = addr + 1;
            break;
        }
        // TYA
        case CORE_CODE_TYA: {
            cpu->registerA = cpu->registerY;
            SetFlagZN(cpu, cpu->registerA);
            cpu->pc = addr + 1;
            break;
        }
        // PHA
        case CORE_CODE_PHA: {
            PushStackByte(cpu, instance, cpu->registerA);
            cpu->pc = addr + 1;
            break;
        }
        // PHP
        case CORE_CODE_PHP: {
            PushStackFlag(cpu, instance);
            cpu->pc = addr + 1;
            break;
        }
        // PLA
        case CORE_CODE_PLA: {
            cpu->registerA = PopStackByte(cpu, instance);
            SetFlagZN(cpu, (int8_t)cpu->registerA);
            cpu->pc = addr + 1;
            break;
        }
        // PLP
        case CORE_CODE_PLP: {
            PopStackFlag(cpu, instance);
            cpu->pc = addr + 1;
            break;
        }
        // NOP s
        case CORE_CODE_NOP:
        case CORE_CODE_NOP_2:
        case CORE_CODE_NOP_3:
        case CORE_CODE_NOP_4:
        case CORE_CODE_NOP_5:
        case CORE_CODE_NOP_6:
        case CORE_CODE_NOP_7: {
            cpu->pc = addr + 1;
            break;
        }
        case CORE_CODE_NOP_ZEROPAGE:
        case CORE_CODE_NOP_ZEROPAGE_2:
        case CORE_CODE_NOP_ZEROPAGE_3: {
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_NOP_ZEROPAGEIX:
        case CORE_CODE_NOP_ZEROPAGEIX_2:
        case CORE_CODE_NOP_ZEROPAGEIX_3:
        case CORE_CODE_NOP_ZEROPAGEIX_4:
        case CORE_CODE_NOP_ZEROPAGEIX_5:
        case CORE_CODE_NOP_ZEROPAGEIX_6: {
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_NOP_ABSOLUTE: {
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_NOP_IMMD:
        case CORE_CODE_NOP_IMMD_2:
        case CORE_CODE_NOP_IMMD_3:
        case CORE_CODE_NOP_IMMD_4:
        case CORE_CODE_NOP_IMMD_5: {
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_NOP_ABSOLUTEIX:
        case CORE_CODE_NOP_ABSOLUTEIX_2:
        case CORE_CODE_NOP_ABSOLUTEIX_3:
        case CORE_CODE_NOP_ABSOLUTEIX_4:
        case CORE_CODE_NOP_ABSOLUTEIX_5:
        case CORE_CODE_NOP_ABSOLUTEIX_6: {
            AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            cpu->pc = addr + 3;
            break;
        }
        // LAX
        case CORE_CODE_LAX_ZEROPAGE: {
            uint8_t op = LoadZeroPage(cpu, instance, addr + 1);
            CpuExecuteLAX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LAX_ZEROPAGEIY: {
            uint8_t op = LoadZeroPageIndexY(cpu, instance, addr + 1);
            CpuExecuteLAX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LAX_ABSOLUTE: {
            uint8_t op = LoadAbsolute(cpu, instance, addr + 1);
            CpuExecuteLAX(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LAX_ABSOLUTEIY: {
            uint8_t op = LoadAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteLAX(cpu, op);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_LAX_PREIDX: {
            uint8_t op = LoadPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteLAX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_LAX_POSTIDY: {
            uint8_t op = LoadPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteLAX(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // SAX
        case CORE_CODE_SAX_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteSAX(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SAX_ZEROPAGEIY: {
            ADDR targetAddr = AddressingZeroPageIndexY(cpu, instance, addr + 1);
            CpuExecuteSAX(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SAX_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteSAX(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
            
        case CORE_CODE_SAX_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteSAX(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        // SBC v2
        case CORE_CODE_SBC_IMMD_2: {
            uint8_t op = LoadImmediately(cpu, instance, addr + 1);
            CpuExecuteSBC(cpu, op);
            cpu->pc = addr + 2;
            break;
        }
        // DCP
        case CORE_CODE_DCP_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_DCP_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_DCP_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_DCP_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_DCP_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_DCP_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_DCP_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteDCP(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        // ISB
        case CORE_CODE_ISB_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ISB_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ISB_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ISB_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ISB_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_ISB_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_ISB_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteISB(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        // SLO
        case CORE_CODE_SLO_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SLO_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SLO_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SLO_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SLO_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SLO_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SLO_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteSLO(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RLA_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RLA_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RLA_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_RLA_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_RLA_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_RLA_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RLA_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteRLA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        // SRE
        case CORE_CODE_SRE_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SRE_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SRE_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SRE_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SRE_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_SRE_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_SRE_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteSRE(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        // RRA
        case CORE_CODE_RRA_ZEROPAGE: {
            ADDR targetAddr = AddressingZeroPage(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RRA_ZEROPAGEIX: {
            ADDR targetAddr = AddressingZeroPageIndexX(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RRA_ABSOLUTE: {
            ADDR targetAddr = AddressingAbsolute(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_RRA_ABSOLUTEIX: {
            ADDR targetAddr = AddressingAbsoluteOffsetX(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_RRA_ABSOLUTEIY: {
            ADDR targetAddr = AddressingAbsoluteOffsetY(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 3;
            break;
        }
        case CORE_CODE_RRA_PREIDX: {
            ADDR targetAddr = AddressingPreIndirectX(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }
        case CORE_CODE_RRA_POSTIDY: {
            ADDR targetAddr = AddressingPostIndirectY(cpu, instance, addr + 1);
            CpuExecuteRRA(cpu, instance, targetAddr);
            cpu->pc = addr + 2;
            break;
        }

        default: {
            cpu->pc = 0;
            assert(false);
            break;
        }
    }

    const CPUInstruction* pInstructionInfo = g_instructionBook + opCode;
    if (pInstructionInfo->crossPageType) {
        cpu->lastCycle = pInstructionInfo->cycle + cpu->crossPageCycle;
    } else {
        cpu->lastCycle = pInstructionInfo->cycle;
    }
    cpu->opcodeCycle = cpu->lastCycle;
    cpu->totalCycle += cpu->lastCycle;
    
    return cpu->pc;
}

