#ifndef iNesAPU_hpp
#define iNesAPU_hpp

#include "iNesInstance.hpp"

#include <stdio.h>
#include <stdint.h>

struct INesInstance;

struct INesAPUPulse {
    uint8_t enable;                                         // 1 bit
    // $4000, $4004
    uint8_t envelope;                                       // 4 bit
    uint8_t constants;                                      // 1 bit
    union { uint8_t envelopeLoop; uint8_t lengthHalt; };    // 1 bit
    uint8_t duty;                                           // 2 bit
    // $4001, $4005
    uint8_t sweepShift;                                     // 3 bit
    uint8_t sweepNegative;                                  // 1 bit
    uint8_t sweepDividerPeriod;                             // 3 bit
    uint8_t sweepEnable;                                    // 1 bit
    uint8_t sweepReloadFlag;                                // 1 bit
    uint8_t sweepDividerCounter;
    uint16_t sweepTargetPeriod;
    // $4002, $4006 -> $4003, $4007
    uint16_t timer;                                         // 11 bit
    uint8_t lengthLoad;                                     // 5 bit
    // inner var
    uint8_t dutyCounter;                                    // 3 bit
    uint8_t lengthCounter;
    uint8_t envelopeCounter;
    uint8_t envelopeVolume;
    uint16_t internalTimer;
    uint8_t volumn;                                         // 0 ... 15
};

struct INesAPUTriangle {
    uint8_t enable;                                         // 1 bit
    union { uint8_t linearControl; uint8_t lengthHalt; };   // 1 bit
    uint8_t linearCounterLoad;                              // 7 bit
    int16_t timer;                                          // 11 bit
    uint8_t lengthLoad;                                     // 5 bit
    
    uint8_t reloadCounter;                                  // 1 bit
    int16_t internalTimer;
    uint8_t lengthCounter;                                  // 8 bit
    uint8_t linearCounter;                                  // 7 bit
    uint8_t duty;
    uint8_t volume;                                         // 0 ... 15
};

struct INesAPUNoise {
    uint8_t enable;                                         // 1 bit
    union { uint8_t envelopeLoop; uint8_t lengthHalt; };    // 1 bit
    uint8_t constantVolume;                                 // 1 bit
    uint8_t envelope;                                       // 4 bit
    uint8_t loop;                                           // 1 bit
    uint8_t period;                                         // 4 bit
    uint8_t lengthLoad;                                     // 5 bit
    
    uint8_t lengthCounter;                                  // 8 bit
    uint8_t volume;                                         // 0 ... 15
    uint8_t envelopeVolume;
    
    int16_t timer;
    int16_t internalTimer;
    uint8_t envelopeCounter;
    uint16_t shiftRegister;
};

struct INesAPUDMC {
    uint8_t enable;                                         // 1 bit
    uint8_t IRQEnable;                                      // 1 bit
    uint8_t loop;                                           // 1 bit
    uint8_t frequency;                                      // 4 bit
    uint8_t loadCounter;                                    // 7 bit
    uint8_t sampleAddress;                                  // 8 bit
    uint8_t sampleLength;                                   // 8 bit
    uint8_t lengthCounter;                                  // 8 bit
    uint8_t timer;                                          // 8 bit
    uint8_t internalTimer;
    uint16_t address;
    uint16_t length;
    uint16_t currAddress;
    uint16_t currLength;
    uint8_t bitCount;
    uint16_t shiftRegister;
    uint8_t volume;                                         // 0 ... 15
};

enum INesAPUStepMode {
    INesAPUStepMode4Step = 0,
    INesAPUStepMode5Step = 1
};

struct INesAPU {
    INesAPUPulse pulses[2];
    INesAPUTriangle triangle;
    INesAPUNoise noise;
    INesAPUDMC DMC;
    uint8_t frameIRQ;                                       // 1 bit
    uint8_t DMCIRQ;                                         // 1 bit
    double volumn;
    
    // $4015
    // write
    uint8_t DMCEnable;                                      // 1 bit
    // read
    uint8_t frameInterruptEnable;                           // 1 bit
    uint8_t DMCInterruptEnable;                             // 1 bit
    // $4017
    enum INesAPUStepMode stepMode;
    uint8_t IRQDisable;                                     // 1 bit
    
    // help
    uint8_t step;
    uint8_t even;                                           // 1 bit
    
    INesInstance* instance;
};

void INesAPUReset(INesAPU* apu);
void INesAPUWrite(INesAPU* apu, uint16_t addr, uint8_t data);
uint8_t INesAPURead(INesAPU* apu, uint16_t addr);
void INesAPUTick(INesAPU* apu);
void INesAPUFrame(INesInstance* instance);

#endif /* iNesAPU_hpp */
