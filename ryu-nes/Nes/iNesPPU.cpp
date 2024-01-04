#include "iNesPPU.hpp"
#include <assert.h>
#include <string.h>
#include <inttypes.h>

static const uint8_t COLORS[][3] = {
    0x75, 0x75, 0x75,
    0x27, 0x1b, 0x8f,
    0x00, 0x00, 0xab,
    0x47, 0x00, 0x9f,
    0x8f, 0x00, 0x77,
    0xab, 0x00, 0x13,
    0xa7, 0x00, 0x00,
    0x7f, 0x0b, 0x00,
    0x43, 0x2f, 0x00,
    0x00, 0x47, 0x00,
    0x00, 0x51, 0x00,
    0x00, 0x3F, 0x17,
    0x1B, 0x3F, 0x5F,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0xBC, 0xBC, 0xBC,
    0x00, 0x73, 0xEF,
    0x23, 0x3B, 0xEF,
    0x83, 0x00, 0xF3,
    0xBF, 0x00, 0xBF,
    0xE7, 0x00, 0x5B,
    0xDB, 0x2B, 0x00,
    0xCB, 0x4F, 0x0F,
    0x8B, 0x73, 0x00,
    0x00, 0x97, 0x00,
    0x00, 0xAB, 0x00,
    0x00, 0x93, 0x3B,
    0x00, 0x83, 0x8B,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF,
    0x3F, 0xBF, 0xFF,
    0x5F, 0x97, 0xFF,
    0xA7, 0x8B, 0xFD,
    0xF7, 0x7B, 0xFF,
    0xFF, 0x77, 0xB7,
    0xFF, 0x77, 0x63,
    0xFF, 0x9B, 0x3B,
    0xF3, 0xBF, 0x3F,
    0x83, 0xD3, 0x13,
    0x4F, 0xDF, 0x4B,
    0x58, 0xF8, 0x98,
    0x00, 0xEB, 0xDB,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0xff, 0xff, 0xff,
    0xAB, 0xE7, 0xFF,
    0xC7, 0xD7, 0xFF,
    0xD7, 0xCB, 0xFF,
    0xFF, 0xC7, 0xFF,
    0xFF, 0xC7, 0xDB,
    0xFF, 0xBF, 0xB3,
    0xFF, 0xDB, 0xAB,
    0xFF, 0xE7, 0xA3,
    0xE3, 0xFF, 0xA3,
    0xAB, 0xF3, 0xBF,
    0xB3, 0xFF, 0xCF,
    0x9F, 0xFF, 0xF3,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
};

/*
 * 函数: INesPPUCopyHorzTToV
 * -------------------------
 * 帮助函数，将 PPU 中 t 寄存器的水平部分数据拷贝给 v 寄存器。
 * t 和 v 寄存器共 15 位，水平相关位为如下所示:
 * --- -x-- ---x xxxx
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 空
 */
static void INesPPUCopyHorzTToV(INesInstance* instance);

/*
 * 函数: INesPPUCopyVertTToV
 * -------------------------
 * 帮助函数，将 PPU 中 t 寄存器的垂直部分数据拷贝给 v 寄存器。
 * t 和 v 寄存器共 15 位，垂直相关位如下所示:
 * --- x-xx xxx- ----
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 空
 */
static void INesPPUCopyVertTToV(INesInstance* instance);

/*
 * 函数: INesPPUIncreaseFineY
 * --------------------------
 * 帮助函数，将 PPU 的 v 寄存器中的 y 像素累加 1 ，并在越界时会自动切换 nametable 。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 空
 */
static void INesPPUIncreaseFineY(INesInstance* instance);


/*
 * 函数: INesPPUReadCtrl
 * ---------------------
 * CPU 通过 PPU 端口 $2000(PPUCTRL) 进行读取。
 * 由于 PPUCTRL 是一个只写寄存器，所以对寄存器的读取实际返回的值为 iodb 。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回：8 位读取数值。
 */
static uint8_t INesPPUReadCtrl(INesInstance* instance);

/*
 * 函数: INesPPUWriteCtrl
 * ----------------------
 * CPU 通过 PPU 端口 $2000(PPUCTRL) 进行写入。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 *
 * PPUCTRL 写入的 8 位数据结构如下图所示（引用自 nesdev）:
 *
 * 7  bit  0
 * ---- ----
 * VPHB SINN
 * |||| ||||
 * |||| ||++- nametable 基地址 (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
 * |||| |+--- CPU 读写 PPUDATA 时的 VRAM 地址累加数值 (0: 加 1，即往画面右一 tile 增加; 1: 加 32，即往画面下一 tile 增加)
 * |||| +---- 精灵 patterntable 的选择基地址，基于 8x8 精灵 (0: $0000; 1: $1000; ignored in 8x16 mode)
 * |||+------ 背景 patterntable 的选择基地址 (0: $0000; 1: $1000)
 * ||+------- 精灵大小 (0: 8x8; 1: 8x16)
 * |+-------- PPU master/slave select(0: read backdrop from EXT pins; 1: output color on EXT pins)
 * +--------- 是否启用 NMI (0: 关; 1: 开)
 */
static void INesPPUWriteCtrl(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadMask
 * ---------------------
 * CPU 通过 PPU 端口 $2001(PPUMASK) 进行读取。
 * 由于 PPUMASK 是一个只写寄存器，所以对寄存器的读取实际返回的值为 iodb 。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值。
 */
static uint8_t INesPPUReadMask(INesInstance* instance);

/*
 * 函数: INesPPUWriteMask
 * ----------------------
 * CPU 通过 PPU 端口 $2001(PPUMASK) 进行写入。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 *
 * PPUMASK 写入的 8 位数据结构如下图所示（引用自 nesdev）:
 *
 * 7  bit  0
 * ---- ----
 * BGRs bMmG
 * |||| ||||
 * |||| |||+- 灰阶 (0: 普通颜色模式, 1: 灰阶显示)
 * |||| ||+-- 1: 左 8 像素显示背景, 0: 隐藏
 * |||| |+--- 1: 左 8 像素显示精灵, 0: 隐藏
 * |||| +---- 1: 显示背景
 * |||+------ 1: 显示精灵
 * ||+------- 强调红色 (在 PAL/Dendy 为绿色)
 * |+-------- 强调绿色 (在 PAL/Dendy 为红色)
 * +--------- 强调蓝色
 *
 */
static void INesPPUWriteMask(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadStatus
 * -----------------------
 * CPU 通过 PPU 端口 $2002(PPUSTATUS) 进行读取。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值。
 *
 * PPUSTATUS 读取的 8 位数据结构返回值如下图所示（引用自 nesdev）:
 *
 * 7  bit  0
 * ---- ----
 * VSO. ....
 * |||| ||||
 * |||+-++++- PPU 开放式总线。 返回 PPU 总线内容。
 * ||+------- 精灵溢出位。
 * |+-------- 精灵 0 hit 位。
 * +--------- VBlank 启动位。
 *
 */
static uint8_t INesPPUReadStatus(INesInstance* instance);

/*
 * 函数: INesPPUWriteStatus
 * ------------------------
 * CPU 通过 PPU 端口 $2002(PPUSTATUS) 进行写入。
 * 由于 PPUSTATUS 是一个只读寄存器，所以对寄存器的写入实际会写到 iodb ，而不会影响实际值。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 */
static void INesPPUWriteStatus(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadOAMADDR
 * ------------------------
 * CPU 通过 PPU 端口 $2003(PPUOAMADDR) 进行读取。
 * 由于 PPUOAMADDR 是一个只写寄存器，所以对寄存器的读取实际返回的值为 iodb 。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值
 */
static uint8_t INesPPUReadOAMADDR(INesInstance* instance);

/*
 * 函数: INesPPUWriteOAMADDR
 * -------------------------
 * CPU 通过 PPU 端口 $2003(PPUOAMADDR) 进行写入。
 * 写入接下去要访问的 OAM 地址，因为 OAM 的地址空间只有 8 位，所以写入的数据 8 位刚好够用，一次写入即可满足。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 */
static void INesPPUWriteOAMADDR(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadOAMDATA
 * ------------------------
 * CPU 通过 PPU 端口 $2004(PPUOAMDATA) 进行读取。
 * CPU 通过这种方式获取 OAM 内存中已保存的数据，读取的字节地址来自于 INesPPUWriteOAMADDR 所写入的 8 位地址值。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值
 */
static uint8_t INesPPUReadOAMDATA(INesInstance* instance);

/*
 * 函数: INesPPUWriteOAMDATA
 * -------------------------
 * CPU 通过 PPU 端口 $2004(PPUOAMDATA) 进行写入。
 * CPU 通过这种方式往 OAM 内存写入数据，写入的字节地址来自于 INesPPUWriteOAMADDR 所写入的 8 位地址值。
 * 每次写入完成后 OAM 地址会自动增加 1 。
 * 在渲染期间写入会导致硬件 bug，作为模拟器可以忽略该 bug，改为在渲染期间忽略写入即可。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 */
static void INesPPUWriteOAMDATA(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadScroll
 * -----------------------
 * CPU 通过 PPU 端口 $2005(PPUSCROLL) 进行读取。
 * 由于 PPUSCROLL 是一个只写寄存器，所以对寄存器的读取实际返回的值为 iodb 。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值
 */
static uint8_t INesPPUReadScroll(INesInstance* instance);

/*
 * 函数: INesPPUWriteScroll
 * ------------------------
 * CPU 通过 PPU 端口 $2005(PPUSCROLL) 进行写入。
 * 该函数用于为 PPU 设置卷轴的 x 和 y 坐标镜头偏移。
 * 由于需要设置 x 和 y 两个 8 位数值，所以该函数通过两次调用才能完成 x 和 y 的写入。
 * 当 PPU 的 w 为 0 时，写入 x ；当 w 为 1 时，写入 y 。
 * 每次写入都会自动切换 w 。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 */
static void INesPPUWriteScroll(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadADDR
 * ---------------------
 * CPU 通过 PPU 端口 $2006(PPUADDR) 进行读取。
 * 由于 PPUADDR 是一个只写寄存器，所以对寄存器的读取实际返回的值为 iodb 。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值
 */
static uint8_t INesPPUReadADDR(INesInstance* instance);

/*
 * 函数: INesPPUWriteADDR
 * ----------------------
 * CPU 通过 PPU 端口 $2006(PPUADDR) 进行写入。
 * 该端口的作用是为 CPU 访问 PPU 内存设置一个访问地址。
 * 由于 PPU 地址是 16 位（虽说实际上是少于16位），而 CPU 端口写入只允许写入 8 位，所以需要两次写入。
 * 当 PPU 的 w 为 0 时，写入高 8 位 ；当 w 为 1 时，写入低 8 位 。
 * 每次写入都会自动切换 w 。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 */
static void INesPPUWriteADDR(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadDATA
 * ---------------------
 * CPU 通过 PPU 端口 $2007(PPUDATA) 进行读取。
 * 该端口的作用是让 CPU 访问读取 PPU 内存的数值，读取的 PPU 地址为 INesPPUWriteADDR 设置的地址。
 * 访问结束后，VRAM 地址会递增，递增数值取决于 $2000(PPUCTRL) 的 vac 。
 * 访问地址为 0-$3fff 时，访问模式为预缓存读取模式，其它地址为即时读取。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值
 */
static uint8_t INesPPUReadDATA(INesInstance* instance);

/*
 * 函数: INesPPUWriteDATA
 * ----------------------
 * CPU 通过 PPU 端口 $2007(PPUDATA) 进行写入。
 * 该端口的作用是让 CPU 将数值写入 PPU 内存，写入的 PPU 地址为 INesPPUWriteADDR 设置的地址。
 * 访问结束后，VRAM 地址会递增，递增数值取决于 $2000(PPUCTRL) 的 vac 。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数值
 *
 * 返回: 空
 */
static void INesPPUWriteDATA(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUReadOAMDMA
 * -----------------------
 * CPU 通过 PPU 端口 $4014(PPUOAMDMA) 进行读取。
 * 由于 PPUOAMDMA 是一个只写寄存器，所以对寄存器的读取实际返回的值为 iodb 。
 * 该函数供 INesPPURead 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 8 位读取数值
 */
static uint8_t INesPPUReadOAMDMA(INesInstance* instance);

/*
 * 函数: INesPPUWriteOAMDMA
 * ------------------------
 * CPU 通过 PPU 端口 $4014(PPUOAMDMA) 进行写入。
 * 对 $4014 的写入，从操作上等价于：
 * 对于参数 data ，假定其数值为 $xx，函数将读取 CPU 地址 $xx00 到 $xxff 的 256 个数值作为参数，执行 256 次 INesPPUWriteOAMDATA 。
 * 在执行 OAMDMA 过程中，CPU 将挂起 513~514 个时钟周期。
 * 因为执行一次 INesPPUWriteOAMDMA 比用 INesPPUWriteOAMDATA 等价的方案在 NES 上执行效率要高很多，所以 NES 程序比较推荐此方案。
 * 该函数供 INesPPUWrite 内部实现调用。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 data: 8 位写入数据
 *
 * 返回: 空
 */
void INesPPUWriteOAMDMA(INesInstance* instance, uint8_t data);

/*
 * 函数: INesPPUGetNameTableAddr
 * -----------------------------
 * 该函数是一个内部帮助函数，通过 VRAM 地址获得对应 tile 的 nametable 地址。
 * 这样可以通过该函数作为寻址去获得 nametable 中对应的值。
 * 该函数是供内部渲染计算过程中使用。
 *
 * 参数 1 VRAM: 16 位 VRAM 地址
 *
 * 返回: 16 位 nametable 地址
 */
static uint16_t INesPPUGetNameTableAddr(uint16_t VRAM);

/*
 * 函数: INesPPUGetAttributeTableAddr
 * ----------------------------------
 * 该函数是一个内部帮助函数，通过 VRAM 地址获得对应 tile 的 attrtable 地址。
 * 该函数是供内部渲染计算过程使用。
 *
 * 参数 1 VRAM: 16 位 VRAM 地址
 *
 * 返回: 16 位 attributetable 地址
 */
static uint16_t INesPPUGetAttributeTableAddr(uint16_t VRAM);

/*
 * 函数: INesPPUGetVRAMByCoarseXInc
 * --------------------------------
 * 该函数是一个内部帮助函数，通过 VRAM 地址获得水平位置在 nametable 中的下一个 tile 的新 VRAM 地址。
 * 注意，当 tile 抵达当前屏幕最后一个 tile 后（tile 31），会自动切换 nametable 。
 * 另外，tile 的纵向坐标始终保持不变。
 * 该函数用于屏幕渲染计算。
 *
 * 参数 1 VRAM: 16 位 VRAM 地址
 *
 * 返回: 新的 16 位 VRAM 地址
 *
 */

static uint16_t INesPPUGetVRAMByCoarseXInc(uint16_t VRAM);

/*
 * 函数: INesPPUGetFineY
 * ---------------------
 * 该函数是一个内部帮助函数，获取当前 Nes 实例中的 VRAM 地址下，当前 tile 内部的 y 像素偏移量（即 fine y）。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 3 位的 fine y 数值（用 8 位无符号整形保存）
 */
static uint8_t INesPPUGetFineY(INesInstance* instance);

/*
 * 函数: INesPPUGetBgPatternBaseAddr
 * ---------------------------------
 * 该函数是一个内部帮助函数，获取当前 Nes 实例中游戏背景在 patterntable（图案表）中的起始地址。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 16 位的起始地址，只会返回 0x0000 和 0x1000 中的一个。
 */
static uint16_t INesPPUGetBgPatternBaseAddr(INesInstance* instance);

/*
 * 函数: INesPPUGetSprPatternBaseAddr
 * ----------------------------------
 * 该函数是一个内部帮助函数，获取当前 Nes 实例中精灵在 patterntable（图案表）中的起始地址。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 16 位的起始地址，只会返回 0x0000 和 0x1000 中的一个。
 */
static uint16_t INesPPUGetSprPatternBaseAddr(INesInstance* instance);

/*
 * 函数: INesPPUFillBgRegister
 * ---------------------------
 * 该函数用于背景渲染计算过程中写入对应的计算寄存器。包括：
 * 1: 2 个 16 位的移位寄存器：bgs16 。
 * 2: 2 个 8 位的移位寄存器: bgs8 。
 * 3: 2 个 1 位的门闩（latch）寄存器 。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 空
 */
static void INesPPUFillBgRegister(INesInstance* instance);

/*
 * 函数: INesPPUIncreaseVRAM
 * -------------------------
 * 对 PPU 的 VRAM 寄存器进行地址累加，累加的值取决于 PPUCTRL 的第 2 位，即 vac 布尔变量。
 * 当 vac 为 0 时，VRAM 会累加 1，如果 vac 为非 0 值，则 VRAM 会累加 32 。
 * 由于 VRAM 为 15 位宽，所以累加结果按 15 位宽进行溢出循环处理。
 * 该函数仅用于对 PPUDATA($2007) 的读写。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 空
 */
static void INesPPUIncreaseVRAM(INesInstance* instance);

void INesPPUGetColorByPalleteIndex(uint8_t index, uint8_t* r, uint8_t* g, uint8_t* b) {
    //assert(index < 64);
    index = index % 64;
    *r = COLORS[index][0];
    *g = COLORS[index][1];
    *b = COLORS[index][2];
    return ;
}

uint8_t INesPPUReadPort(INesInstance* instance, uint16_t addr) {
    assert((addr >= 0x2000 && addr <= 0x2007) || addr == 0x4014);
    if (addr == 0x2000) {
        return INesPPUReadCtrl(instance);
    }
    
    if (addr == 0x2001) {
        return INesPPUReadMask(instance);
    }
    
    if (addr == 0x2002) {
        return INesPPUReadStatus(instance);
    }
    
    if (addr == 0x2003) {
        return INesPPUReadOAMADDR(instance);
    }
    
    if (addr == 0x2004) {
        return INesPPUReadOAMDATA(instance);
    }
    
    if (addr == 0x2005) {
        return INesPPUReadScroll(instance);
    }
    
    if (addr == 0x2006) {
        return INesPPUReadADDR(instance);
    }
    
    if (addr == 0x2007) {
        return INesPPUReadDATA(instance);
    }
    
    if (addr == 0x4014) {
        return INesPPUReadOAMDMA(instance);
    }
    
    assert(!"INesPPURead unreachable code!");
    return 0;
}

void INesPPUWritePort(INesInstance* instance, uint16_t addr, uint8_t data) {
    assert((addr >= 0x2000 && addr <= 0x2007) || addr == 0x4014);
    if (addr == 0x2000) {
        return INesPPUWriteCtrl(instance, data);
    }
    
    if (addr == 0x2001) {
        return INesPPUWriteMask(instance, data);
    }
    
    if (addr == 0x2002) {
        return INesPPUWriteStatus(instance, data);
    }
    
    if (addr == 0x2003) {
        return INesPPUWriteOAMADDR(instance, data);
    }
    
    if (addr == 0x2004) {
        return INesPPUWriteOAMDATA(instance, data);
    }
    
    if (addr == 0x2005) {
        return INesPPUWriteScroll(instance, data);
    }
    
    if (addr == 0x2006) {
        return INesPPUWriteADDR(instance, data);
    }
    
    if (addr == 0x2007) {
        return INesPPUWriteDATA(instance, data);
    }
    
    if (addr == 0x4014) {
        return INesPPUWriteOAMDMA(instance, data);
    }
    
    assert(!"INesPPUWrite unreachable code!");
}

static uint8_t INesPPUReadCtrl(INesInstance* instance) {
    return instance->ppu->iodb;
}

static void INesPPUWriteCtrl(INesInstance* instance, uint8_t data) {
    instance->ppu->iodb = data;
    instance->ppu->t = (instance->ppu->t & ~(0x8c00)) | (((uint16_t)data & 3) << 10);
    instance->ppu->vac = (data >> 2) & 1;
    instance->ppu->spt = (data >> 3) & 1;
    instance->ppu->bgt = (data >> 4) & 1;
    instance->ppu->sps = (data >> 5) & 1;
    instance->ppu->mss = (data >> 6) & 1;
    instance->ppu->vbi = (data >> 7) & 1;
}

static uint8_t INesPPUReadMask(INesInstance* instance) {
    return instance->ppu->iodb;
}

static void INesPPUWriteMask(INesInstance* instance, uint8_t data) {
    instance->ppu->iodb = data;
    instance->ppu->grs = data & 1;
    instance->ppu->bl8 = (data >> 1) & 1;
    instance->ppu->sl8 = (data >> 2) & 1;
    instance->ppu->bge = (data >> 3) & 1;
    instance->ppu->spe = (data >> 4) & 1;
    instance->ppu->emr = (data >> 5) & 1;
    instance->ppu->emg = (data >> 6) & 1;
    instance->ppu->emb = (data >> 7) & 1;
}

static uint8_t INesPPUReadStatus(INesInstance* instance) {
    uint8_t data = instance->ppu->iodb & 0x1f; // 该 5 位数据读取到的是 iodb 总线的值
    data |= instance->ppu->ovf << 5;
    data |= instance->ppu->s0h << 6;
    data |= instance->ppu->vbs << 7;
    instance->ppu->w = 0; // 每次读取 PPUSTATUS 都会将 PPU 的切换位清零
    instance->ppu->vbs = 0; // 每次读取 PPUSTATUS 都会将第 7 位（VBlank）清零
    instance->ppu->iodb = data; // 因为 PPUSTATUS 是可读寄存器，所以会将读取的值同步给 iodb 总线
    return data;
}

static void INesPPUWriteStatus(INesInstance* instance, uint8_t data) {
    instance->ppu->iodb = data;
}

static uint8_t INesPPUReadOAMADDR(INesInstance* instance) {
    return instance->ppu->iodb;
}

static void INesPPUWriteOAMADDR(INesInstance* instance, uint8_t data) {
    instance->ppu->odr = data;
}

static uint8_t INesPPUReadOAMDATA(INesInstance* instance) {
    uint8_t data = instance->ppu->oam[instance->ppu->odr];
    instance->ppu->iodb = data;
    return data;
}

static void INesPPUWriteOAMDATA(INesInstance* instance, uint8_t data) {
    if (INesPPUIsRendering(instance)) {
        // For emulation purposes, it is probably best to completely ignore writes during rendering. --- nesdev
        return ;
    }
    instance->ppu->iodb = data;
    instance->ppu->oam[instance->ppu->odr] = data;
    ++instance->ppu->odr;
}

static uint8_t INesPPUReadScroll(INesInstance* instance) {
    return instance->ppu->iodb;
}

static void INesPPUWriteScroll(INesInstance* instance, uint8_t data) {
    instance->ppu->iodb = data;
    if (instance->ppu->w) {
        // second write
        instance->ppu->t = (instance->ppu->t & 0xc1f) | (((uint16_t)data & 7) << 12) | (((uint16_t)data >> 3) << 5);
        instance->ppu->w = 0;
    } else {
        // first write
        instance->ppu->x = data & 0x7;
        instance->ppu->t = (instance->ppu->t & 0x7fe0) | ((uint16_t)data >> 3);
        instance->ppu->w = 1;
    }
}

uint8_t INesPPUReadADDR(INesInstance* instance) {
    return instance->ppu->iodb;
}

void INesPPUWriteADDR(INesInstance* instance, uint8_t data) {
    instance->ppu->iodb = data;
    if (instance->ppu->w) {
        // second write
        instance->ppu->t = (instance->ppu->t & 0x7f00) | (uint16_t)data;
        instance->ppu->v = instance->ppu->t;
        instance->ppu->w = 0;
    } else {
        // first write
        instance->ppu->t = (instance->ppu->t & 0xff) | (((uint16_t)data & 0x3f) << 8);
        instance->ppu->w = 1;
    }
}

static uint8_t INesPPUReadDATA(INesInstance* instance) {
    uint8_t data = INesInstancePPURead(instance, instance->ppu->v);
    uint16_t addr = INesInstanceMappingPPUAddr(instance->ppu->v);
    if (addr <= 0x3eff) {
        uint8_t buffer = instance->ppu->rb;
        instance->ppu->rb = data;
        instance->ppu->iodb = buffer;
        INesPPUIncreaseVRAM(instance);
        return buffer;
    }
    instance->ppu->iodb = data;
    INesPPUIncreaseVRAM(instance);
    return data;
}

static void INesPPUWriteDATA(INesInstance* instance, uint8_t data) {
    INesInstancePPUWrite(instance, instance->ppu->v, data);
    instance->ppu->iodb = data;
    INesPPUIncreaseVRAM(instance);
}

static uint8_t INesPPUReadOAMDMA(INesInstance* instance) {
    return instance->ppu->iodb;
}

void INesPPUWriteOAMDMA(INesInstance* instance, uint8_t data) {
    uint16_t base = (uint16_t)data << 8;
    for (uint16_t i = 0; i <= 0xff; ++i) {
        uint8_t v = INesInstanceRead(instance, base + i);
        INesPPUWriteOAMDATA(instance, v);
    }
    instance->ppu->iodb = data;
    if (instance->cpu->tick % 2 == 0) {
        CpuStealCycles(instance->cpu, 513);
    } else {
        CpuStealCycles(instance->cpu, 514);
    }
}

void INesPPUReset(INesInstance* instance) {
    // 将 ppu 结构体所有成员信息清零。
    memset(instance->ppu, 0, sizeof(INesPPU));
    // 将扫描线初始化在预渲染扫描线。
    instance->ppu->ty = 261;
}

void INesPPUTick(INesInstance* instance) {
    INesPPU* ppu = instance->ppu;
    ++ppu->tick;
    // 可视渲染范围
    if (ppu->bge || ppu->spe) {
        ppu->fetchSprite = false;
        if (ppu->ty <= 239 && ppu->tx <= 255) {
            // background
            if (ppu->tx % 8 == 0) {
                INesPPUFillBgRegister(instance);
                instance->ppu->v = INesPPUGetVRAMByCoarseXInc(instance->ppu->v);
            } else {
                ppu->bgs16[0] <<= 1;
                ppu->bgs16[1] <<= 1;
                ppu->bgs8[0] = (ppu->bgs8[0] >> 1) | (ppu->bgb8[0] << 7);
                ppu->bgs8[1] = (ppu->bgs8[1] >> 1) | (ppu->bgb8[1] << 7);
            }
            // 输出背景像素
            uint8_t bit0 = (ppu->bgs16[0] >> (15 - ppu->x)) & 1;
            uint8_t bit1 = ((ppu->bgs16[1] >> (15 - ppu->x)) & 1) << 1;
            uint8_t bit2 = ((ppu->bgs8[0] >> ppu->x) & 1) << 2;
            uint8_t bit3 = ((ppu->bgs8[1] >> ppu->x) & 1) << 3;
            uint8_t palette = bit0 | bit1 | bit2 | bit3;
            
            if (!(palette & 3)) {
                palette = 0;
            }
            assert(palette < 16);
            ppu->bg_p[ppu->ty][ppu->tx] = palette;
            ppu->bg_d[ppu->ty][ppu->tx] = INesInstancePPURead(instance, 0x3f00 + palette) & 0x3f;
            uint8_t spz = (ppu->spr_p[ppu->ty][ppu->tx] >> 5) & 1; // sprite number zero
            uint8_t bgf = (ppu->spr_p[ppu->ty][ppu->tx] >> 4) & 1; // background in front
            ppu->spr_p[ppu->ty][ppu->tx] &= 0x0f;
            
            if (ppu->bge && ppu->spe) {
                ppu->output[ppu->ty][ppu->tx] = ppu->bg_d[ppu->ty][ppu->tx];
                if ((!palette && ppu->spr_p[ppu->ty][ppu->tx]) || (!bgf && palette && ppu->spr_p[ppu->ty][ppu->tx])) {
                    ppu->output[ppu->ty][ppu->tx] = ppu->spr_d[ppu->ty][ppu->tx];
                }
                if (spz && palette && ppu->spr_p[ppu->ty][ppu->tx]) {
                    ppu->s0h = 1;
                }
            } else {
                if (ppu->bge) {
                    ppu->output[ppu->ty][ppu->tx] = ppu->bg_d[ppu->ty][ppu->tx];
                }
                else {
                    ppu->output[ppu->ty][ppu->tx] = ppu->spr_d[ppu->ty][ppu->tx];
                }
            }
            ppu->spr_p[ppu->ty][ppu->tx] = 0;
            ppu->spr_d[ppu->ty][ppu->tx] = INesInstancePPURead(instance, 0x3f00 + 0) & 0x3f;
        } // if (ppu->ty <= 239 && ppu->tx <= 255)
        
        // sprites
        if (ppu->ty <= 239 && ppu->tx == 256) {
            ppu->fetchSprite = true;
            uint16_t spritePatternAddr = INesPPUGetSprPatternBaseAddr(instance);
            int spriteCount = 0;
            size_t spriteHeight = 8 << instance->ppu->sps;
            
            for (int spriteIndex = 0; spriteIndex < 64; ++spriteIndex) {
                uint8_t i = (uint8_t)spriteIndex;
                uint8_t baseIndex = i << 2;
                uint16_t ty = ppu->oam[baseIndex];
                uint16_t tx = ppu->oam[baseIndex + 3];
                
                if (ty >= 239) {
                    continue;
                }
                
                if (ppu->ty < ty || ppu->ty >= ty + spriteHeight) {
                    continue;
                }
                assert(ppu->ty >= ty && ppu->ty < ty + spriteHeight);
                
                uint8_t indexNumber = ppu->oam[baseIndex + 1];
                uint8_t attribute = ppu->oam[baseIndex + 2];
                uint8_t attributeHigherColorBit = attribute & 0x3;
                uint8_t attributeBgPriority = (attribute >> 5) & 1;
                uint8_t attributeFlipHorz = (attribute >> 6) & 1;
                uint8_t attributeFlipVert = (attribute >> 7) & 1;
                uint16_t by = ppu->ty - ty; // offset y in tile
                assert((!instance->ppu->sps && by <= 7) || (instance->ppu->sps && by <= 15));
                
                if (attributeFlipVert) {
                    by = spriteHeight - 1 - by;
                }
                uint16_t readAddr = spritePatternAddr + ((uint16_t)indexNumber << 4) + by;
                if (instance->ppu->sps) {
                    readAddr = ((indexNumber & 1) << 12) + ((uint16_t)(indexNumber >> 1) << 5) + by + (by & 8) ;
                }
                uint8_t byte0 = INesInstancePPURead(instance, readAddr);
                uint8_t byte1 = INesInstancePPURead(instance, readAddr + 8);
                uint8_t findSpr = 0;

                for (uint8_t bx = 0; bx < 8; ++bx) {
                    uint16_t x = (uint16_t)tx + (uint16_t)bx;
                    if (attributeFlipHorz) {
                        x = (uint16_t)tx + (uint16_t)(7 - bx);
                    }
                    if (x >= 256) {
                        continue;
                    }
                    findSpr = 1;
                    
                    uint8_t bit0 = (byte0 >> (7 - bx)) & 1;
                    uint8_t bit1 = (byte1 >> (7 - bx)) & 1;
                    uint8_t attributeLower = bit0 | (bit1 << 1);
                    uint8_t attributeFull = attributeLower | (attributeHigherColorBit << 2);
                    if (attributeLower == 0) {
                        attributeFull = 0;
                    }
                    uint8_t paletteValue = INesInstancePPURead(instance, 0x3f10 + (uint16_t)attributeFull);
                    uint8_t spz = (spriteIndex == 0) ? 1 : 0;
                    if (attributeFull && !(ppu->spr_p[ppu->ty+1][x]&0x0f) && spriteCount < 8) {
                        ppu->spr_p[ppu->ty+1][x] = attributeFull | (attributeBgPriority << 4) | (spz << 5);
                        ppu->spr_d[ppu->ty+1][x] = paletteValue;
                    }
                }
                
                if (findSpr) {
                    ++spriteCount;
                    if (spriteCount == 9) {
                        // overflow
                        ppu->ovf = 1;
                        break;
                    }
                }
            } // end spriteIndex loop
        }

        if (ppu->ty <= 239 || ppu->ty == 261) {
            if (ppu->tx >= 257 && ppu->tx <= 320) {
                // 在可视化扫描线和预渲染扫描线周期内的点 257 到 320 之间，OAMADDR 会被重置为 0 。
                ppu->odr = 0;
            }

            if (ppu->tx == 257) {
                INesPPUCopyHorzTToV(instance);
            } else if (ppu->tx == 256) {
                // 递增 v 的 y 坐标，以获取下一行的数据
                INesPPUIncreaseFineY(instance);
            }
        }

        if (ppu->ty == 261 && ppu->tx >= 280 && ppu->tx <= 304) {
            INesPPUCopyVertTToV(instance);
        }
    } else {
        if (ppu->ty <= 239 && ppu->tx <= 255) {
            ppu->output[ppu->ty][ppu->tx] = INesInstancePPURead(instance, 0x3f00 + 0) & 0x3f;
        }
    }
    
    if (ppu->tx == 1) {
        if (ppu->ty == 241) {
            // 开始 VBlank
            ppu->vbs = 1;
            if (ppu->vbi) {
                instance->cpu->nmi = 1;
            }
        } else if (ppu->ty == 261) {
            // 结束 VBlank
            ppu->ovf = 0;
            ppu->vbs = 0;
            ppu->s0h = 0;
        }
    }
    
    ++ppu->tx;
    if (ppu->tx == 341) {
        ppu->tx = 0;
        ++ppu->ty;
        if (ppu->ty == 262) {
            ppu->ty = 0;
        }
    }
    
    INesInstanceOnPPUTick(instance);
}

uint8_t INesPPUIsRendering(INesInstance* instance) {
    INesPPU* ppu = instance->ppu;
    return (ppu->ty == 261 || (ppu->ty >= 0 && ppu->ty <= 239)) && (ppu->bge || ppu->spe);
}


static void INesPPUCopyHorzTToV(INesInstance* instance) {
    // v: ...|.A..|...B|CDEF <- t: ...|.A..|...B|CDEF
    instance->ppu->v = (instance->ppu->v & 0x7be0) | (instance->ppu->t & 0x041f);
}

static void INesPPUCopyVertTToV(INesInstance* instance) {
    // v: GHI|A.BC|DEF.|.... <- t: GHI|A.BC|DEF.|....
    instance->ppu->v = (instance->ppu->v & 0x041f) | (instance->ppu->t & 0x7be0);
}

static void INesPPUIncreaseFineY(INesInstance* instance) {
    if ((instance->ppu->v & 0x7000) != 0x7000) { // if fine Y < 7
        instance->ppu->v += 0x1000;  // increment fine Y
        instance->ppu->v &= 0x7fff;
    } else {
        instance->ppu->v &= ~0x7000; // fine Y = 0
        uint16_t y = (instance->ppu->v & 0x03e0) >> 5; // let y = coarse Y
        if (y == 29) {
            y = 0; // coarse Y = 0
            instance->ppu->v ^= 0x0800; // switch vertical nametable
        } else if (y == 31) {
            y = 0; // coarse Y = 0, nametable not switch
        } else {
            y += 1; // increment coarse Y
        }
        instance->ppu->v = (instance->ppu->v & ~0x03e0) | (y << 5); // put coarse Y back into v
    }
}

static uint16_t INesPPUGetNameTableAddr(uint16_t VRAM) {
    return 0x2000 | (VRAM & 0x0fff);
}

static uint16_t INesPPUGetAttributeTableAddr(uint16_t VRAM) {
    return 0x23c0 | (VRAM & 0x0c00) | ((VRAM >> 4) & 0x38) | ((VRAM >> 2) & 0x07);
}

static uint16_t INesPPUGetVRAMByCoarseXInc(uint16_t VRAM) {
    if ((VRAM & 0x001f) == 31) { // if coarse X == 31
        VRAM &= ~0x001f; // coarse X = 0
        VRAM ^= 0x0400; // switch horizontal nametable
    } else {
        VRAM += 1; // increment coarse X
    }
    return VRAM;
}

static uint8_t INesPPUGetFineY(INesInstance* instance) {
    return (instance->ppu->v >> 12) & 7;
}

static uint16_t INesPPUGetBgPatternBaseAddr(INesInstance* instance) {
    return instance->ppu->bgt ? 0x1000 : 0x0000;
}

static uint16_t INesPPUGetSprPatternBaseAddr(INesInstance* instance) {
    if (instance->mapper->number == INesMapperTypeMMC5 && instance->ppu->sps) {
        return 0x0000;
    }
    return instance->ppu->spt ? 0x1000 : 0x0000;
}

static void INesPPUFillBgRegister(INesInstance* instance) {
    // 获取 tile 中的 y 像素偏移（fine y）备用
    uint16_t fineY = (uint16_t)INesPPUGetFineY(instance);
    // 获取当前背景在图案表（patterntable）中的基地址，备用
    uint16_t patternSelectAddr = INesPPUGetBgPatternBaseAddr(instance);
    // 写入 16 位的位移寄存器的内容
    // 步骤 1: 获取当前 tile 的 nametable 地址
    uint16_t currTileNameTableAddr = INesPPUGetNameTableAddr(instance->ppu->v);
    // 步骤 2: 通过 nametable 得到对应的值后，该值就是 patterntable 里的 tile 索引。
    // 由于 patterntable 里每个 tile 信息占了 16 字节，所以索引位置要乘以 16 来定位，即左移 4 。
    
    uint16_t currTilePatternAddrOffset = (uint16_t)INesInstancePPURead(instance, currTileNameTableAddr) << 4;
    // 步骤 3: patterntable 索引作为偏移，加上 patterntable 基地址，就是要寻址的 patterntable tile 的起始地址。
    uint16_t currTilePatternAddr = patternSelectAddr + currTilePatternAddrOffset;
    // 步骤 4: 当前 tile 的 patterntable 基地址，加上 fine y 像素偏移，就可以得到 当前 fine y 下 8 个像素的图案值。
    uint8_t patternValueCurr1 = INesInstancePPURead(instance, currTilePatternAddr + fineY);
    // 步骤 5：同 4 步骤，不过这次获得的是像素对应的另外 1 位。
    uint8_t patternValueCurr2 = INesInstancePPURead(instance, currTilePatternAddr + fineY + 8);
    
    // 和上面步骤类似，只是获取的是下一个 tile 的图案数据。
    uint16_t nextv = INesPPUGetVRAMByCoarseXInc(instance->ppu->v);
    uint16_t nextTileNameTableAddr = INesPPUGetNameTableAddr(nextv);
    uint16_t nextTilePatternAddrOffset = (uint16_t)INesInstancePPURead(instance, nextTileNameTableAddr) << 4;
    uint16_t nextTilePatternAddr = patternSelectAddr + nextTilePatternAddrOffset;
    uint8_t patternValueNext1 = INesInstancePPURead(instance, nextTilePatternAddr + fineY);
    uint8_t patternValueNext2 = INesInstancePPURead(instance, nextTilePatternAddr + fineY + 8);
    
    // 组合当前和下一个 tile 的图案数据到两个 16 位位移寄存器。
    instance->ppu->bgs16[0] = (uint16_t)patternValueNext1 | ((uint16_t)patternValueCurr1 << 8);
    instance->ppu->bgs16[1] = (uint16_t)patternValueNext2 | ((uint16_t)patternValueCurr2 << 8);
    
    // 以下是获取属性值的步骤
    // 步骤 1: 获取当前 tile 的属性表地址。
    uint16_t currAttrAddr = INesPPUGetAttributeTableAddr(instance->ppu->v);
    // 步骤 2: 通过属性表地址获得对应的属性数值。
    uint8_t currAttrData = INesInstancePPURead(instance, currAttrAddr);
    // 步骤 3: 重要！计算方格内部偏移量。
    uint8_t currFineTile = ((instance->ppu->v & 2) >> 1) | ((instance->ppu->v & 0x40) >> 5);
    // 步骤 4: 计算属性比特值的低 1 位。
    uint8_t attrBitLo = 1 & (currAttrData >> (currFineTile << 1));
    // 步骤 5: 计算属性比特值的高 1 位。
    uint8_t attrBitHi = 1 & (currAttrData >> ((currFineTile << 1) + 1));
    // 步骤 6: 将 2 个位填充到 8 位寄存器，8 位寄存器的每一位都被填充，0 被填充为 0x00, 1 被填充为 0xff 。
    instance->ppu->bgs8[0] = ~(attrBitLo - 1);
    instance->ppu->bgs8[1] = ~(attrBitHi - 1);
    
    // 用同样的步骤获取下一个 tile 对应的属性值，写到 1 位寄存器上作为候补填充。
    uint16_t nextAttrAddr = INesPPUGetAttributeTableAddr(nextv);
    uint8_t nextAttrData = INesInstancePPURead(instance, nextAttrAddr);
    uint8_t nextFineTile = ((nextv & 2)>> 1) | ((nextv & 0x40) >> 5);
    instance->ppu->bgb8[0] = 1 & (nextAttrData >> (nextFineTile << 1));
    instance->ppu->bgb8[1] = 1 & (nextAttrData >> ((nextFineTile << 1) + 1));
}

static void INesPPUIncreaseVRAM(INesInstance* instance) {
    INesPPU* ppu = instance->ppu;
    ppu->v = (ppu->v + (ppu->vac ? 32 : 1)) & 0x7fff;
}
