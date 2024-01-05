# RyuNes

## Introduction

Hello everyone, please call me Ryu. This is an Nes emulator I developed independently.

The core part of the code is written in C, although I use the file extensions hpp and cpp.

My emulator has the following advantages:
1. The code is more streamlined and clean. The entire kernel of the emulator is concentrated in the Nes folder, consisting of clean C language code, calling the C Library, without any other dependencies.
2. The kernel's CPU simulation, PPU, and APU sections all provide API calls at the single-cycle level, making it very suitable for embedding into any platform.
3. The entire kernel code is built in pure C, making it highly portable.

I wrote this emulator mainly out of personal interest. From start to finish, I completed the initial version independently, including learning Nes hardware, programming, and debugging.
A so-called completely independent development means that all the code in the kernel (Nes) is developed by myself without using any third-party libraries. This includes the core of the game: CPU instruction set, PPU graphics processing unit, and APU sound processing unit, as well as the Nes-specific mapper mechanism.
The emulator's kernel can currently support most games, and many classic games can be played.

Simple examples include Tank Battle and Super Mario Bros. 1. More advanced examples include Ninja Turtles 1 to 3 (including fighting versions), Zelda, Metal Max, Dragon Quest, and Final Fantasy, among others.
Of course, there are still many areas that need to be improved, such as:
1. Only a few relatively mainstream mappers are supported, such as 0~4, 74. Other mappers have not been supported (these problems should not be significant). The most difficult part is mapper 5, which is in support, but I have encountered some bugs.
2. Some small features of the PPU (the graphics processing unit) have not been implemented, but most games do not use these features, such as Emphasize Color and Edge Clipping (Crop).
3. Due to insufficient debugging, there may be some malignant bugs that I have not discovered (although I played some games without problems).
4. And many more.

## Front-end Part

Because my main development work is focused on the emulator's core (i.e., the backend), I did not invest much time in the front-end. The open-source code provides a demonstration based on Mac's SDL front-end implementation.
Therefore, enthusiasts who want to try running this project need to have a Mac and install SDL and XCode themselves.
I have tried to implement several front-ends for my Nes kernel, such as a Mac OpenGL program, an iOS client demo, etc., all of which run well.

## How to Run

Because the current open-source version is only the code and basic front-end demo, it is not a formal release version. The open-source release is just to announce the birth of the project :)
To run this project, you need a MacOS operating system, and the development environment is XCode. Developers also need to download the SDL runtime library themselves. Then, you can open the project and compile it.
Since running games requires FC (nes) game roms, developers need to download a rom themselves (this should not be difficult to achieve), download it to the local Download folder, and modify the NesWrap2.m file within the runEmulatorLoop method. The fileString variable is changed to the absolute path of the local file.
Follow-up Work
The focus of future work should be to support more mappers first. The front-end development is not the top priority.
I don't know if there are any friends willing to assist me in developing mapper 5, which I currently have bugs in the support process -.- .

## Running Examples

Here is an SDL front-end running Dragon Fighter:

![Dragon Fighter](https://github.com/RyuNesC/RyuNes/blob/main/ryu-nes/DemoAssets/demo001.png)

## Contact Me
If you are interested in the project, you can contact me. This is my email: roof456@qq.com.

If you can help me support Mapper 5, I am willing to pay you $100 :)

You can also add me on WeChat to communicate.

![My WeChat](https://github.com/RyuNesC/RyuNes/blob/main/ryu-nes/DemoAssets/wechat.jpg)

