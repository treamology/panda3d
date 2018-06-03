//
//  P3DGraphicsStateGuardian.m
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#include "iOSGraphicsStateGuardian.h"

#import "P3DGraphicsStateGuardian.h"

@interface P3DGraphicsStateGuardian () {
    IOSGraphicsStateGuardian *_wrappedGuardian;
}
@end

@implementation P3DGraphicsStateGuardian


- (id)initWithContext:(EAGLContext *)context engine:(P3DGraphicsEngine *)engine pipe:(P3DGraphicsPipe *)pipe {
    if (self = [super init]) {
        self.context = context;
        if (self.context) {
            [EAGLContext setCurrentContext:self.context];
        }
        
        _wrappedGuardian = new IOSGraphicsStateGuardian([engine getWrappedEngine], [pipe getWrappedPipe], NULL);
        if (!_wrappedGuardian) self = nil;
    }
    
    return self;
}

- (void)dealloc {
    delete _wrappedGuardian;
}

- (IOSGraphicsStateGuardian *)getWrappedGuardian {
    return _wrappedGuardian;
}

@end
