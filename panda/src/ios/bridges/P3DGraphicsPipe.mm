//
//  P3DGraphicsPipe.m
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#include "iOSGraphicsPipe.h"

#import "P3DGraphicsPipe.h"

@interface P3DGraphicsPipe () {
    IOSGraphicsPipe *_wrappedPipe;
}
@end

@implementation P3DGraphicsPipe

+ (id)sharedPipe {
    static P3DGraphicsPipe *sharedPipe = nil;
    static dispatch_once_t token;
    dispatch_once(&token, ^{
        sharedPipe = [[P3DGraphicsPipe alloc] init];
    });
    return sharedPipe;
}

- (id)init {
    if (self = [super init]) {
        _wrappedPipe = new IOSGraphicsPipe();
        if (!_wrappedPipe) self = nil;
    }
    return self;
}

- (void)dealloc {
    delete _wrappedPipe;
}

- (IOSGraphicsPipe *)getWrappedPipe {
    return _wrappedPipe;
}

@end
