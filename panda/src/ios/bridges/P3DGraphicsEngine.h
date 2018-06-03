//
//  P3DGraphicsEngine.h
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#import "P3DGraphicsPipe.h"
#import "P3DGraphicsOutput.h"
#import "P3DGraphicsWindow.h"

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <GLKit/GLKit.h>

struct GraphicsEngine;

@interface P3DGraphicsEngine : NSObject

@property (nonatomic, readonly) P3DGraphicsPipe *currentPipe;

+ (id)sharedEngine;
- (id)init;

- (GraphicsEngine *)getWrappedEngine;
- (P3DGraphicsWindow *)makeWindowWithGLKView:(GLKView *)view;

- (void)renderFrame;

@end
