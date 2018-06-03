//
//  P3DGraphicsStateGuardian.h
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//


#import "P3DGraphicsPipe.h"

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

#import "P3DGraphicsEngine.h"

struct IOSGraphicsStateGuardian;

@class P3DGraphicsEngine;

@interface P3DGraphicsStateGuardian : NSObject

@property (nonatomic) EAGLContext *context;

- (id)initWithContext:(EAGLContext *)context engine:(P3DGraphicsEngine *)engine pipe:(P3DGraphicsPipe *)pipe;

- (IOSGraphicsStateGuardian *)getWrappedGuardian;

@end
