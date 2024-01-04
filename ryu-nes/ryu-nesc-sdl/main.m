#import <Foundation/Foundation.h>
#import "NesWrap2.h"

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        [NesWrap2.instance run];
    }
    return 0;
}
