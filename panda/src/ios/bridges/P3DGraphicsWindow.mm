//
//  P3DGraphicsWindow.m
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#include "iOSGraphicsWindow.h"

#import "P3DGraphicsWindow.h"

@interface P3DGraphicsWindow () {
    IOSGraphicsWindow *_wrappedWindow;
}
@end

@implementation P3DGraphicsWindow

- (id)initWithWindow:(IOSGraphicsWindow *)window guardian:(P3DGraphicsStateGuardian *)gsg {
    if (self = [super init]) {
        _wrappedWindow = window;
        self.gsg = gsg;
    }
    return self;
}

- (IOSGraphicsWindow *)getWrappedOutput {
    return _wrappedWindow;
}

@end
