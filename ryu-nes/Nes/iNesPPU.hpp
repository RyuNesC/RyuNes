#ifndef iNesPPU_hpp
#define iNesPPU_hpp
#include "iNesInstance.hpp"

#include <stdio.h>
#include <stdint.h>

struct INesInstance;

struct INesPPU {
    uint8_t iodb;   // internal data bus for communication
    
    // in 2000 PPUCTRL
    uint8_t vac;    // VRAM address increment bit
    uint8_t spt;    // sprite pattern table bit
    uint8_t bgt;    // background pattern table bit
    uint8_t sps;    // sprite size bit. 0: 8x8 1: 8x16
    uint8_t mss;    // master/slave select
    uint8_t vbi;    // vertical blank interval, generate an NMI at the start of the vertical blanking interval
    
    // in 2001 PPUMASK
    uint8_t grs;    // gray scale bit
    uint8_t bl8;    // background in leftmost 8 pixels of screen bit
    uint8_t sl8;    // sprite in leftmost 8 pixels of screen bit
    uint8_t bge;    // background enable bit
    uint8_t spe;    // sprite enable bit
    uint8_t emr;    // emphasize red
    uint8_t emg;    // emphasize green
    uint8_t emb;    // emphasize blue
    
    // in 2002 PPUSTATUS
    uint8_t ovf;    // sprite overflow flag bit
    uint8_t s0h;    // sprite 0 hit bit
    uint8_t vbs;    // vertical blank has start bit
    
    // in 2003 OAMADDR
    uint8_t odr;    // OAMADDR
    
    // in 2007  DATA
    uint8_t rb;     //  read buffer byte
    
    // internal
    uint16_t t;     // temporary VRAM 15 bits
    uint16_t v;     // VRAM 15 bits
    uint8_t w;      // first second write toggle bit
    uint8_t x;      // fine x scroll, 3 bits, 0-7
    
    // tick for scanlines
    uint16_t tx;
    uint16_t ty;
    uint16_t bgs16[2];  // 2 pair of background register 16
    uint8_t bgs8[2];    // 2 pair of background register 8
    uint8_t bgb8[2];    // 2 pair of background latch bit
    uint8_t sps1[8];    // 8 pair of shift register for sprite part 1
    uint8_t sps2[8];    // 8 pair of shift register for sprite part 2
    uint8_t spl[8];     // 8 latches for sprite
    uint8_t spc[8];     // 8 counters for sprite
    
    // mem
    uint8_t oam[256];       // OAM memory
    uint8_t oam2[32];       // secondary OAM memory
    uint8_t mem[0x4000];    // PPU memory
    
    // output
    uint8_t bg_p[240][256];
    uint8_t bg_d[240][256];
    uint8_t spr_p[240][256];
    uint8_t spr_d[240][256];
    uint8_t output[240][256];
    
    // help
    long long tick;
    bool fetchSprite;
};

/*
 * 函数: INesPPUGetColorByPalleteIndex
 * -----------------------------------
 * 帮助函数，通过调色板索引获得像素 rgb 值。
 *
 * 参数 1 index: 调色板索引
 * 参数 2 r: 输出，颜色值 r
 * 参数 3 g: 输出，颜色值 g
 * 参数 4 b: 输出，颜色值 b
 *
 * 返回: 空
 */
void INesPPUGetColorByPalleteIndex(uint8_t index, uint8_t* r, uint8_t* g, uint8_t* b);

/*
 * 函数: INesPPUReadPort
 * -----------------
 * CPU 通过 PPU 寄存器端口与 PPU 进行读取。
 * CPU 的 PPU 寄存器端口地址分别为 $2000~$2007 以及 $4014 。
 * 
 * 参数 1 instance: Nes 实例
 * 参数 2 addr: CPU 的 16 位地址端口，限定为 0x2000~0x2007 或 0x4014 中的一个值。
 *
 * 返回：如果 addr 范围正确，则返回 PPU 寄存器读取的 8 位返回值。否则返回 0 。
 */
uint8_t INesPPUReadPort(INesInstance* instance, uint16_t addr);

/*
 * 函数: INesPPUWritePort
 * ------------------
 * CPU 通过 PPU 寄存器端口与 PPU 进行写入。
 * CPU 的 PPU 寄存器端口地址分别为 $2000~$2007 以及 $4014 。
 *
 * 参数 1 instance: Nes 实例
 * 参数 2 addr: CPU 的 16 位地址端口，限定为 0x2000~0x2007 或 0x4014 中的一个值。
 * 参数 3 data: 写入的 8 位数据。
 *
 * 返回: 空
 */
void INesPPUWritePort(INesInstance* instance, uint16_t addr, uint8_t data);

/*
 * 函数: INesPPUReset
 * ------------------
 * 重置 PPU 内部状态进行初始化
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 空
 */
void INesPPUReset(INesInstance* instance);

/*
 * 函数: INesPPUTick
 * -----------------
 * 执行一次 PPU 时钟
 *
 * 返回: 空
 */
void INesPPUTick(INesInstance* instance);

/*
 * 函数: INesPPUIsRendering
 * ------------------------
 * 帮助函数，判断 PPU 当前是否处于渲染状态。
 * PPU 处于渲染状态的判断条件取决于两方面:
 * 1. PPU 扫描线在可视扫描线或预渲染扫描线。
 * 2. PPUCTRL 的背景显示位和精灵显示位至少要有一个处于开启状态。
 *
 * 参数 1 instance: Nes 实例
 *
 * 返回: 1（是）或者 0（否）
 */
uint8_t INesPPUIsRendering(INesInstance* instance);

#endif /* iNesPPU_hpp */
