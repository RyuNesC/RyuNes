#import "NesWrap2.h"
#include "iNesInstance.hpp"
#include <chrono>

#define WS          (256)
#define HS          (240)
#define SC          (2)
#define FREQ        (44100)
#define SAMPLE      (2048)

extern "C" {
#include <SDL2/SDL.h>
}

using namespace::std::chrono;

volatile static int g_isInstanceStop = 0;
volatile static int g_isSDLStop = 0;
static SDL_Window* g_window;
static SDL_Renderer* g_renderer;
static SDL_Texture* g_texture;
static INesInstance* g_instance = NULL;
volatile static double g_sound[16][SAMPLE] = {};
volatile static uint64_t g_sound_i1 = 0;
volatile static uint64_t g_sound_i2 = 0;
volatile static uint8_t g_ppu_output[16][WS*HS] = {};
volatile static uint64_t g_ppu_i1 = 0;
volatile static uint64_t g_ppu_i2 = 0;
volatile static int g_sound_start = 0;

static void AudioCallBack(void* userData, Uint8* stream, int length) {
    if (!g_sound_start) {
        memset(stream, 0, length);
        return ;
    }
    
    if (g_sound_i1 >= g_sound_i2) {
        int count = length / 2;
        size_t si = g_sound_i1 - 1;
        if (g_sound_i1 == 0) {
            si = 15;
        }
        uint16_t* source = (uint16_t*)stream;
        for (int i = 0; i < count; ++i) {
            source[i] = (double)g_sound[si%16][i] * UINT16_MAX;
            assert(g_sound[si%16][i] < 16);
        }
        static int kk = 0;
        ++kk;
    } else {
        int count = length / 2;
        uint16_t* source = (uint16_t*)stream;
        for (int i = 0; i < count; ++i) {
            source[i] = (double)g_sound[g_sound_i1%16][i] * UINT16_MAX;
            assert(g_sound[g_sound_i1%16][i] < 16);
        }
        ++g_sound_i1;
    }
}

@implementation NesWrap2

- (instancetype)init {
    self = [super init];
    return self;
}

+ (instancetype)instance {
    static NesWrap2* obj = nil;
    if (obj == nil) {
        obj = [[NesWrap2 alloc] init];
    }
    return obj;
}

- (void)runEmulatorLoop {
    usleep(1000000);
    
    NSString* fileString = @"/Users/fenghaitongluo/Downloads/Dragon Fighter (USA).nes";
    
    NSURL* filePath = [NSURL fileURLWithPath: fileString];
    NSData* romData = [NSData dataWithContentsOfURL: filePath];

    INesInstance* instance = INesInstanceCreate((const uint8_t*)romData.bytes, (size_t)romData.length);
    if (!instance) {
        assert(!"instance create fail.");
        return ;
    }
    g_instance = instance;
    
    if (INesFileContainsMemoryChip(instance->file)) {
        NSString* fileDir = [fileString stringByDeletingLastPathComponent];
        NSString* fileName = [[fileString lastPathComponent] stringByDeletingPathExtension];
        NSString* saveFileName = [fileName stringByAppendingString: @".sav"];
        NSString* saveString = [fileDir stringByAppendingPathComponent: saveFileName];
        INesInstanceSetFilePath(instance, fileString.UTF8String);
        INesInstanceSetSaveRamFilePath(instance, saveString.UTF8String);
    }
    
    time_point u1 = system_clock::now();
    time_point u2 = system_clock::now();
    size_t apuFrameCount = 0;
    size_t i = 0;
    size_t si = 0;
    double soundCount = 0;
    const double s = (double)89342.0*60.0 / (double)FREQ;
    long double v = 0;
    size_t vc = 0;
    size_t frameCount = 0;
    
    while (!g_isSDLStop) {
        u1 = system_clock::now();
        for (i = 0; i < 89342; ++i) {
            INesPPUTick(instance);
            if (instance->ppu->tick % 3 == 0) {
                INesInstanceTickCPU(instance);
                INesAPUTick(instance->apu);
                if (instance->apu->even) {
                    v += instance->apu->volumn * 1000.0;
                    ++vc;
                }
            }
            ++apuFrameCount;
            if (apuFrameCount == 22335) {
                apuFrameCount = 0;
                INesAPUFrame(instance);
            }
            soundCount = soundCount + 1;
            if (soundCount >= s) {
                soundCount -= s;
                g_sound[g_sound_i2%16][si] = v / (long double)vc / 1000.0;
                v = 0;
                vc = 0;
                ++si;
                if (si == SAMPLE) {
                    si = 0;
                    ++g_sound_i2;
                    g_sound_start = g_sound_i2 >= 2;
                }
            }
        }
        
        memcpy((void*)g_ppu_output[g_ppu_i2%16], (void*)g_instance->ppu->output, HS*WS);
        ++g_ppu_i2;
        
        int i = 0;
        while (1) {
            u2 = system_clock::now();
            size_t lag = 16666;
            if (g_sound_i2 - g_sound_i1 <= 1) {
                lag -= 2000;
            }
            if ((u2 - u1).count() >= lag) {
                if (i == 0) {
                    static int count = 0;
                    ++count;
                    //printf("drop %d\n", count);
                }
                break;
            } else {
                usleep(USEC_PER_SEC*0.001);
            }
            ++i;
        }
        
        ++frameCount;
        if (frameCount >= 60 * 10) {
            INesFileSaveRam(instance->file);
        }
    }
    INesFileSaveRam(instance->file);
    
    g_isInstanceStop = 1;
    INesInstanceDestroy(instance);
    g_instance = NULL;
}

- (void)runSDLLoop {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return ;
    }
    
    SDL_AudioSpec spec {};
    spec.freq = FREQ;
    spec.format = AUDIO_U16SYS;
    spec.channels = 1;
    spec.silence = 0;
    spec.samples = SAMPLE;
    spec.callback = AudioCallBack;
    spec.userdata = NULL;
    SDL_OpenAudio(&spec, NULL);
    SDL_PauseAudio(0);
    
    size_t width = 256*2;
    size_t height = 240*2;
    SDL_Window* window = SDL_CreateWindow("Nes",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          (int)width,
                                          (int)height,
                                          0);
    if (!window) {
        return ;
    }
    
    g_window = window;
    
    SDL_ShowWindow(window);
    SDL_Event event = { 0 };
    
    g_renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC);
    g_texture = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, (int)width, (int)height);
    SDL_RegisterEvents(1);
    
    int loop = 1;
    while (loop) {
        while (SDL_PollEvent(&event) && loop) {
            switch (event.type) {
                case SDL_QUIT: {
                    loop = 0;
                    break;
                }
                
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    if (!g_instance) {
                        break;
                    }
                    
                    INesPadButton button = INesPadButtonA;
                    uint8_t buttonPadPress = 0;
                    
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W: {
                            buttonPadPress = 1;
                            button = INesPadButtonUp;
                            break;
                        }
                        case SDL_SCANCODE_S: {
                            buttonPadPress = 1;
                            button = INesPadButtonDown;
                            break;
                        }
                        case SDL_SCANCODE_A: {
                            buttonPadPress = 1;
                            button = INesPadButtonLeft;
                            break;
                        }
                        case SDL_SCANCODE_D: {
                            buttonPadPress = 1;
                            button = INesPadButtonRight;
                            break;
                        }
                        case SDL_SCANCODE_K: {
                            buttonPadPress = 1;
                            button = INesPadButtonA;
                            break;
                        }
                        case SDL_SCANCODE_J: {
                            buttonPadPress = 1;
                            button = INesPadButtonB;
                            break;
                        }
                        case SDL_SCANCODE_N: {
                            buttonPadPress = 1;
                            button = INesPadButtonStart;
                            break;
                        }
                        case SDL_SCANCODE_V: {
                            buttonPadPress = 1;
                            button = INesPadButtonSelect;
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    
                    if (buttonPadPress) {
                        if (event.type == SDL_KEYDOWN) {
                            INesPadPressButton(g_instance->pad, INesPadPlayer1, button);
                        } else {
                            INesPadReleaseButton(g_instance->pad, INesPadPlayer1, button);
                        }
                    }
                    break;
                }
            }
        };
        
        
        if (g_ppu_i1 < g_ppu_i2) {
            //printf("%d\n", g_ppu_i2 - g_ppu_i1);
            uint32_t* pixels;
            int pitch;
            uint64_t i1 = g_ppu_i2 - 1;
            SDL_LockTexture(g_texture, NULL, (void**)&pixels, &pitch);
            uint8_t r, g, b;
            // 在这里将FC模拟器的像素数据拷贝到pixels中
            for (int y = 0; y < 240; ++y) {
                for (int x = 0; x < 256; ++x) {
                    uint8_t pi = g_ppu_output[i1%16][y*256+x];
                    INesPPUGetColorByPalleteIndex(pi, &r, &g, &b);
                    uint32_t pixel = (r << 16) | (g << 8) | b;
                    pixels[y * 2 * 512 + x * 2] = pixel;
                    pixels[y * 2 * 512 + (x * 2 + 1)] = pixel;
                    pixels[(y * 2 + 1) * 512 + x * 2] = pixel;
                    pixels[(y * 2 + 1) * 512 + (x * 2 + 1)] = pixel;
                }
            }
            SDL_UnlockTexture(g_texture);
            SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
            SDL_RenderPresent(g_renderer);
            g_ppu_i1 = i1 + 1;
        }
    }
    
    g_isSDLStop = 1;
    
    while (!g_isInstanceStop) {
        usleep(1000);
    }
    
    SDL_CloseAudio();
    SDL_Quit();
}

- (void)run {
    __weak NesWrap2* ws = self;
    dispatch_queue_t highPriorityQueue = dispatch_queue_create("com.nes.ryu", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(highPriorityQueue, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0));
    dispatch_async(highPriorityQueue, ^{
        [ws runEmulatorLoop];
    });
    [self runSDLLoop];
}

@end
