#include "iNesAPU.hpp"
#include <assert.h>

static const uint8_t LengthTable[32] = {
    10,  254, 20,  2,  40,  4,  80, 6,
    160, 8,   60,  10, 14,  12, 26, 14,
    12,  16,  24,  18, 48,  20, 96, 22,
    192, 24,  72,  26, 16,  28, 32, 30,
};

static const uint8_t PulseDutyTable[4][8] = {
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 0, 0, 0,
    1, 0, 0, 1, 1, 1, 1, 1,
};

static const uint8_t TriangleTable[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
};

static const int16_t NoiseTable[16] = {
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

static const uint8_t DMCTable[16] = {
    214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27
};

static const double PulseVolumeTable[32] = {
    0.000000, 0.007520, 0.015040, 0.022560, 0.030080, 0.037600, 0.045120, 0.052640,
    0.060160, 0.067680, 0.075200, 0.082720, 0.090240, 0.097760, 0.105280, 0.112800,
    0.120320, 0.127840, 0.135360, 0.142880, 0.150400, 0.157920, 0.165440, 0.172960,
    0.180480, 0.188000, 0.195520, 0.203040, 0.210560, 0.218080, 0.225600, 0.233120,
};

static const double TriangleVolumeTable[16] = {
    0.000000, 0.008510, 0.017020, 0.025530, 0.034040, 0.042550, 0.051060, 0.059570,
    0.068080, 0.076590, 0.085100, 0.093610, 0.102120, 0.110630, 0.119140, 0.127650
};

static const double NoiseVolumeTable[16] = {
    0.000000, 0.004940, 0.009880, 0.014820, 0.019760, 0.024700, 0.029640, 0.034580,
    0.039520, 0.044460, 0.049400, 0.054340, 0.059280, 0.064220, 0.069160, 0.074100,
};

static const double DMCVolumeTable[128] = {
    0.000000, 0.003300, 0.006600, 0.009900, 0.013200, 0.016500, 0.019800, 0.023100,
    0.026400, 0.029700, 0.033000, 0.036300, 0.039600, 0.042900, 0.046200, 0.049500,
    0.052800, 0.056100, 0.059400, 0.062700, 0.066000, 0.069300, 0.072600, 0.075900,
    0.079200, 0.082500, 0.085800, 0.089100, 0.092400, 0.095700, 0.099000, 0.102300,
    0.105600, 0.108900, 0.112200, 0.115500, 0.118800, 0.122100, 0.125400, 0.128700,
    0.132000, 0.135300, 0.138600, 0.141900, 0.145200, 0.148500, 0.151800, 0.155100,
    0.158400, 0.161700, 0.165000, 0.168300, 0.171600, 0.174900, 0.178200, 0.181500,
    0.184800, 0.188100, 0.191400, 0.194700, 0.198000, 0.201300, 0.204600, 0.207900,
    0.211200, 0.214500, 0.217800, 0.221100, 0.224400, 0.227700, 0.231000, 0.234300,
    0.237600, 0.240900, 0.244200, 0.247500, 0.250800, 0.254100, 0.257400, 0.260700,
    0.264000, 0.267300, 0.270600, 0.273900, 0.277200, 0.280500, 0.283800, 0.287100,
    0.290400, 0.293700, 0.297000, 0.300300, 0.303600, 0.306900, 0.310200, 0.313500,
    0.316800, 0.320100, 0.323400, 0.326700, 0.330000, 0.333300, 0.336600, 0.339900,
    0.343200, 0.346500, 0.349800, 0.353100, 0.356400, 0.359700, 0.363000, 0.366300,
    0.369600, 0.372900, 0.376200, 0.379500, 0.382800, 0.386100, 0.389400, 0.392700,
    0.396000, 0.399300, 0.402600, 0.405900, 0.409200, 0.412500, 0.415800, 0.419100, 
};

//MARK: static func declaration
static void INesAPUProcessEnvelope(INesAPU* apu);
static void INesAPUProcessLinearCounter(INesAPU* apu);
static void INesAPUProcessLengthCounter(INesAPU* apu);
static void INesAPUProcessSweep(INesAPU* apu);

static void INesAPUPulseTick(INesAPU* apu, size_t pid);
static void INesAPUProcessPulseEnvelope(INesAPU* apu, size_t pid);
static void INesAPUProcessPulseLengthCounter(INesAPU* apu, size_t pid);
static void INesAPUProcessPulseSweep(INesAPU* apu, size_t pid);

static void INesAPUTriangleTick(INesAPU* apu);
static void INesAPUProcessTriangleLengthCounter(INesAPU* apu);

static void INesAPUNoiseTick(INesAPU* apu);
static void INesAPUProcessNoiseEnvelope(INesAPU* apu);
static void INesAPUProcessNoiseLengthCounter(INesAPU* apu);

static void INesAPUDMCTick(INesAPU* apu);
static void INesAPUDMCProcessReader(INesAPU* apu);
static void INesAPUDMCProcessShifter(INesAPU* apu);
static void INesAPUDMCRestart(INesAPU* apu);


//MARK: interface
void INesAPUReset(INesAPU* apu) {
    apu->noise.shiftRegister = 1;
}

void INesAPUWrite(INesAPU* apu, uint16_t addr, uint8_t data) {
    assert((addr >= 0x4000 && addr <= 0x400C) || (addr >= 0x400E && addr <= 0x4015) || addr == 0x4017);
    if (addr >= 0x4000 && addr <= 0x4007) {
        assert((addr - 0x4000) >> 2 <= 1);
        INesAPUPulse* pulse = apu->pulses + ((addr - 0x4000) >> 2);
        uint8_t i = addr & 3;
        if (i == 0) {
            assert(addr == 0x4000 || addr == 0x4004);
            pulse->envelope = data & 0xf;
            pulse->constants = (data >> 4) & 1;
            pulse->envelopeLoop = (data >> 5) & 1;
            pulse->duty = (data >> 6) & 3;
        } else if (i == 1) {
            assert(addr == 0x4001 || addr == 0x4005);
            pulse->sweepShift = data & 7;
            pulse->sweepNegative = (data >> 3) & 1;
            pulse->sweepDividerPeriod = ((data >> 4) & 7);
            pulse->sweepEnable = data >> 7;
            pulse->sweepReloadFlag = 1;
            pulse->sweepDividerCounter = 0; //? pulse->sweepDividerPeriod;
            // ?
            if (pulse->sweepDividerPeriod == 0) {
                //pulse->lengthCounter = 0;
            }
        } else if (i == 2) {
            assert(addr == 0x4002 || addr == 0x4006);
            pulse->timer &= 0x700;
            pulse->timer |= data;
        } else if (i == 3) {
            assert(addr == 0x4003 || addr == 0x4007);
            pulse->timer &= 0xff;
            pulse->timer |= (int16_t)(data & 7) << 8;
            pulse->lengthLoad = data >> 3;
            pulse->lengthCounter = LengthTable[pulse->lengthLoad];
            pulse->internalTimer = 0;
            pulse->dutyCounter = 0;
            pulse->envelopeCounter = 0;
            pulse->envelopeVolume = 15;
        }
    } else if (addr == 0x4015) {
        apu->pulses[0].enable = data & 1;
        if (!apu->pulses[0].enable) {
            apu->pulses[0].lengthCounter = 0;
        }
        
        apu->pulses[1].enable = (data >> 1) & 1;
        if (!apu->pulses[1].enable) {
            apu->pulses[1].lengthCounter = 0;
        }
        
        apu->triangle.enable = (data >> 2) & 1;
        if (!apu->triangle.enable) {
            apu->triangle.lengthCounter = 0;
        }
        
        apu->noise.enable = (data >> 3) & 1;
        if (!apu->noise.enable) {
            apu->noise.lengthCounter = 0;
        }
        
        apu->DMC.enable = (data >> 4) & 1;
        if (!apu->DMC.enable) {
            apu->DMC.lengthCounter = 0;
        } else {
            if (!apu->DMC.currLength) {
                INesAPUDMCRestart(apu);
            }
        }
    } else if (addr == 0x4017) {
        apu->stepMode = (INesAPUStepMode)((data >> 7) & 1);
        apu->IRQDisable = (data >> 6) & 1;
        apu->step = 0;
    } else if (addr == 0x4008) {
        apu->triangle.linearControl = data >> 7;
        apu->triangle.linearCounterLoad = data & 0x7f;
    } else if (addr == 0x400a) {
        apu->triangle.timer &= 0x700;
        apu->triangle.timer |= data;
        apu->triangle.internalTimer = apu->triangle.timer;
    } else if (addr == 0x400b) {
        apu->triangle.timer &= 0xff;
        apu->triangle.timer |= (int16_t)(data & 7) << 8;
        apu->triangle.lengthLoad = data >> 3;
        apu->triangle.lengthCounter = LengthTable[apu->triangle.lengthLoad];
        apu->triangle.internalTimer = 0;
        apu->triangle.reloadCounter = 1;
    } else if (addr == 0x400c) {
        apu->noise.envelope = data & 0xf;
        apu->noise.constantVolume = 1 & (data >> 4);
        apu->noise.lengthHalt = 1 & (data >> 5);
    } else if (addr == 0x400e) {
        apu->noise.loop = 1 & (data >> 7);
        apu->noise.period = data & 0xf;
        apu->noise.timer = NoiseTable[apu->noise.period];
        apu->noise.internalTimer = 0;
    } else if (addr == 0x400f) {
        apu->noise.lengthLoad = (data >> 3) & 0x1f;
        apu->noise.lengthCounter = LengthTable[apu->noise.lengthLoad];
        apu->noise.envelopeCounter = 0;
        apu->noise.envelopeVolume = 15;
    } else if (addr == 0x4010) {
        apu->DMC.IRQEnable = 1 & (data >> 7);
        apu->DMC.loop = 1 & (data >> 6);
        apu->DMC.frequency = data & 0xf;
        apu->DMC.timer = DMCTable[apu->DMC.frequency];
        apu->DMC.internalTimer = 0;
    } else if (addr == 0x4011) {
        apu->DMC.loadCounter = data & 0x7f;
    } else if (addr == 0x4012) {
        apu->DMC.sampleAddress = data;
        apu->DMC.address = 0xc000 | ((uint16_t)data << 6);
    } else if (addr == 0x4013) {
        apu->DMC.sampleLength = data;
        apu->DMC.length = ((uint16_t)data << 4) | 1;
    }
}

uint8_t INesAPURead(INesAPU* apu, uint16_t addr) {
    assert((addr >= 0x4000 && addr <= 0x400C) || (addr >= 0x400E && addr <= 0x4015));
    uint8_t data = 0;
    if (addr == 0x4015) {
        data |= !!apu->pulses[0].lengthCounter;
        data |= (!!apu->pulses[1].lengthCounter) << 1;
        data |= (!!apu->triangle.lengthCounter) << 2;
        data |= (!!apu->noise.lengthCounter) << 3;
        data |= (!!apu->DMC.lengthCounter) << 4;
        data |= apu->frameIRQ << 6;
        data |= apu->DMCIRQ << 7;
        apu->frameIRQ = 0;
    }
    return data;
}

void INesAPUTick(INesAPU* apu) {
    if (apu->even) {
        INesAPUPulseTick(apu, 0);
        INesAPUPulseTick(apu, 1);
        INesAPUNoiseTick(apu);
        INesAPUDMCTick(apu);
        apu->volumn = PulseVolumeTable[apu->pulses[0].volumn + apu->pulses[1].volumn];
        apu->volumn += TriangleVolumeTable[apu->triangle.volume];
        apu->volumn += NoiseVolumeTable[apu->noise.volume];
        apu->volumn += DMCVolumeTable[apu->DMC.volume];
        //printf("v = %d\n", apu->noise.volume);
    }
    apu->even = !apu->even;
    
    INesAPUTriangleTick(apu);
}

void INesAPUFrame(INesInstance* instance) {
    INesAPU* apu = instance->apu;
    if (apu->stepMode == INesAPUStepMode4Step) {
        switch (apu->step) {
            case 0:
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            case 1:
                INesAPUProcessLengthCounter(apu);
                INesAPUProcessSweep(apu);
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            case 2:
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            case 3:
                INesAPUProcessLengthCounter(apu);
                INesAPUProcessSweep(apu);
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                if (!instance->apu->IRQDisable) {
                    instance->apu->frameIRQ = 1;
                    if (!instance->cpu->flag.I) {
                        instance->cpu->irq = 1;
                    }
                }
                break;
            default:
                assert(!"unreachable!");
                break;
        }
        apu->step = (apu->step + 1) % 4;
    } else {
        assert(apu->stepMode == INesAPUStepMode5Step);
        switch (apu->step) {
            case 0:
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            case 1:
                INesAPUProcessLengthCounter(apu);
                INesAPUProcessSweep(apu);
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            case 2:
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            case 3:
                // do nothing
                break;
            case 4:
                INesAPUProcessLengthCounter(apu);
                INesAPUProcessSweep(apu);
                INesAPUProcessEnvelope(apu);
                INesAPUProcessLinearCounter(apu);
                break;
            default:
                assert(!"unreachable!");
                break;
        }
        apu->step = (apu->step + 1) % 5;
    }
}

//MARK: static func implementation
static void INesAPUProcessEnvelope(INesAPU* apu) {
    INesAPUProcessPulseEnvelope(apu, 0);
    INesAPUProcessPulseEnvelope(apu, 1);
    INesAPUProcessNoiseEnvelope(apu);
}

static void INesAPUProcessLinearCounter(INesAPU* apu) {
    if (apu->triangle.reloadCounter) {
        apu->triangle.linearCounter = apu->triangle.linearCounterLoad;
    } else if (apu->triangle.linearCounter > 0) {
        --apu->triangle.linearCounter;
    }
    if (!apu->triangle.linearControl) {
        apu->triangle.reloadCounter = 0;
    }
}

static void INesAPUProcessLengthCounter(INesAPU* apu) {
    INesAPUProcessPulseLengthCounter(apu, 0);
    INesAPUProcessPulseLengthCounter(apu, 1);
    INesAPUProcessTriangleLengthCounter(apu);
    INesAPUProcessNoiseLengthCounter(apu);
}

static void INesAPUProcessSweep(INesAPU* apu) {
    INesAPUProcessPulseSweep(apu, 0);
    INesAPUProcessPulseSweep(apu, 1);
}

static void INesAPUPulseTick(INesAPU* apu, size_t pid) {
    assert(pid == 0 || pid == 1);
    
    INesAPUPulse* pulse = apu->pulses + pid;
    assert(pulse->duty < 4);
    assert(pulse->dutyCounter < 8);
    
    if (pulse->internalTimer <= 0) {
        uint8_t env = pulse->constants ? pulse->envelope : pulse->envelopeVolume;
        pulse->volumn = env * PulseDutyTable[pulse->duty][pulse->dutyCounter];
        pulse->dutyCounter = (pulse->dutyCounter + 1) & 7;
        pulse->internalTimer = pulse->timer;
    } else {
        --pulse->internalTimer;
    }
    
    if (!pulse->enable || pulse->lengthCounter == 0 || pulse->timer < 8 || pulse->timer > 0x7ff || pulse->sweepTargetPeriod > 0x7ff) {
        pulse->volumn = 0;
    }
}

static void INesAPUProcessPulseEnvelope(INesAPU* apu, size_t pid) {
    assert(pid == 0 || pid == 1);
    INesAPUPulse* pulse = apu->pulses + pid;
    
    if (pulse->envelopeCounter == pulse->envelope) {
        pulse->envelopeCounter = 0;
        if (pulse->envelopeVolume == 0) {
            pulse->envelopeVolume = (16 - pulse->envelopeLoop) & 15;
        } else {
            --pulse->envelopeVolume;
        }
    }
    
    ++pulse->envelopeCounter;
}

static void INesAPUProcessPulseLengthCounter(INesAPU* apu, size_t pid) {
    assert(pid == 0 || pid == 1);
    INesAPUPulse* pulse = apu->pulses + pid;
    if (!pulse->enable) {
        return;
    }
    
    if (!pulse->lengthHalt && pulse->lengthCounter) {
        --pulse->lengthCounter;
    }
}

static void INesAPUProcessPulseSweep(INesAPU* apu, size_t pid) {
    assert(pid == 0 || pid == 1);
    INesAPUPulse* pulse = apu->pulses + pid;
    
    uint16_t op = pulse->timer >> pulse->sweepShift;
    if (pulse->sweepNegative) {
        pulse->sweepTargetPeriod = pulse->timer - op;
        if (0 == pid && pulse->sweepTargetPeriod > 0) {
            --pulse->sweepTargetPeriod;
        }
    } else {
        pulse->sweepTargetPeriod = pulse->timer + op;
    }
    
    if (pulse->sweepEnable && pulse->sweepDividerCounter == 0 && pulse->sweepTargetPeriod <= 0x7ff && pulse->timer >= 8 && pulse->sweepShift) {
        if (pulse->sweepShift || !pulse->sweepNegative) {
            pulse->timer = pulse->sweepTargetPeriod;
        }
    }
    
    if (pulse->sweepDividerCounter == 0 || pulse->sweepReloadFlag) {
        pulse->sweepDividerCounter = pulse->sweepDividerPeriod;
        pulse->sweepReloadFlag = 0;
    } else {
        pulse->sweepDividerCounter--;
    }
}

static void INesAPUProcessTriangleLengthCounter(INesAPU* apu) {
    if (!apu->triangle.enable) {
        return;
    }
    
    if (!apu->triangle.lengthHalt && apu->triangle.lengthCounter) {
        --apu->triangle.lengthCounter;
    }
}

static void INesAPUTriangleTick(INesAPU* apu) {
    if (apu->triangle.internalTimer <= 0) {
        apu->triangle.internalTimer = apu->triangle.timer;
        if (apu->triangle.linearCounter && apu->triangle.lengthCounter) {
            if (apu->triangle.timer >= 2) {
                apu->triangle.volume = TriangleTable[apu->triangle.duty];
            }
            apu->triangle.duty = (apu->triangle.duty + 1) & 0x1f;
        }
    } else {
        --apu->triangle.internalTimer;
    }
    apu->triangle.volume = apu->triangle.volume * apu->triangle.enable;
}

static void INesAPUNoiseTick(INesAPU* apu) {
    if (apu->noise.internalTimer == apu->noise.timer) {
        apu->noise.internalTimer = 0;
        uint16_t shift = apu->noise.loop ? 6 : 1;
        uint16_t b1 = apu->noise.shiftRegister & 1;
        uint16_t b2 = (apu->noise.shiftRegister >> shift) & 1;
        apu->noise.shiftRegister >>= 1;
        apu->noise.shiftRegister |= (b1 ^ b2) << 14;
    } else {
        ++apu->noise.internalTimer;
    }
    
    if (apu->noise.enable && apu->noise.lengthCounter && !(apu->noise.shiftRegister & 1)) {
        apu->noise.volume = apu->noise.constantVolume ? apu->noise.envelope : apu->noise.envelopeVolume;
    } else {
        apu->noise.volume = 0;
    }
}

static void INesAPUProcessNoiseEnvelope(INesAPU* apu) {
    if (apu->noise.envelopeCounter == apu->noise.envelope) {
        apu->noise.envelopeCounter = 0;
        if (apu->noise.envelopeVolume == 0) {
            apu->noise.envelopeVolume = (16 - apu->noise.envelopeLoop) & 15;
        } else {
            --apu->noise.envelopeVolume;
        }
    }
    
    ++apu->noise.envelopeCounter;
}

static void INesAPUProcessNoiseLengthCounter(INesAPU* apu) {
    INesAPUNoise* noise = &apu->noise;
    if (!noise->enable) {
        return;
    }

    if (!noise->lengthHalt && noise->lengthCounter) {
        --noise->lengthCounter;
    }
}

static void INesAPUDMCTick(INesAPU* apu) {
    INesAPUDMCProcessReader(apu);
    if (apu->DMC.internalTimer == apu->DMC.timer) {
        apu->DMC.internalTimer = 0;
        INesAPUDMCProcessShifter(apu);
    } else {
        apu->DMC.internalTimer++;
    }
    
    apu->DMC.volume = apu->DMC.loadCounter;
}

static void INesAPUDMCProcessReader(INesAPU* apu) {
    if (apu->DMC.currLength && !apu->DMC.bitCount) {
        CpuStealCycles(apu->instance->cpu, 4); // TODO:? d.cpu.stall += 4
        apu->DMC.shiftRegister = INesInstanceRead(apu->instance, apu->DMC.currAddress);
        apu->DMC.bitCount = 8;
        ++apu->DMC.currAddress;
        if (apu->DMC.currAddress == 0) {
            apu->DMC.currAddress = 0x8000;
        }
        --apu->DMC.currLength;
        if (apu->DMC.currLength == 0 && apu->DMC.loop) {
            INesAPUDMCRestart(apu);
        }
    }
}

static void INesAPUDMCProcessShifter(INesAPU* apu) {
    if (apu->DMC.bitCount == 0) {
        return ;
    }
    
    if ((apu->DMC.shiftRegister & 1) == 1) {
        if (apu->DMC.loadCounter <= 125) {
            apu->DMC.loadCounter += 2;
        }
    } else {
        if (apu->DMC.loadCounter >= 2) {
            apu->DMC.loadCounter -= 2;
        }
    }
    
    apu->DMC.shiftRegister >>= 1;
    apu->DMC.bitCount--;
}

static void INesAPUDMCRestart(INesAPU* apu) {
    apu->DMC.currLength = apu->DMC.length;
    apu->DMC.currAddress = apu->DMC.address;
}
