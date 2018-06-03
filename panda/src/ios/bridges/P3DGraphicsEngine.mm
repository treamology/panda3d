//
//  P3DGraphicsEngine.m
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#include "graphicsEngine.h"
#include "iOSGraphicsPipe.h"
#include "iOSGraphicsStateGuardian.h"

#import "P3DGraphicsEngine.h"
#import "P3DGraphicsStateGuardian.h"

@interface P3DGraphicsEngine () {
    GraphicsEngine *_wrappedEngine;
}
@end

@implementation P3DGraphicsEngine

+ (id)sharedEngine {
    static P3DGraphicsEngine *sharedGraphicsEngine = nil;
    static dispatch_once_t token;
    dispatch_once(&token, ^{
        sharedGraphicsEngine = [[P3DGraphicsEngine alloc] init];
    });
    return sharedGraphicsEngine;
}

- (id)init {
    if (self = [super init]) {
        _wrappedEngine = GraphicsEngine::get_global_ptr();
        if (!_wrappedEngine) self = nil;
    }
    return self;
}

- (void)dealloc {
    delete _wrappedEngine;
}

- (GraphicsEngine *)getWrappedEngine {
    return _wrappedEngine;
}

- (id<P3DGraphicsOutput>)makeOutputWithGraphicsPipe:(P3DGraphicsPipe *)pipe
                                                 name:(NSString *)name
                                              fbProps:(FrameBufferProperties)fbProps
                                          windowProps:(WindowProperties)winProps
                                                flags:(int)flags
                                graphicsStateGuardian:(P3DGraphicsStateGuardian *)gsg
                                           hostOutput:(id<P3DGraphicsOutput> *)host {
    IOSGraphicsPipe *_pipe = [pipe getWrappedPipe];
    std::string _name = std::string([name UTF8String]);
    IOSGraphicsStateGuardian *_gsg = [gsg getWrappedGuardian];
    
    IOSGraphicsWindow *window = (IOSGraphicsWindow *)_wrappedEngine->make_output(_pipe, _name, 0, fbProps, winProps, flags, _gsg, NULL);
    return [[P3DGraphicsWindow alloc] initWithWindow:window guardian:gsg];
}

- (P3DGraphicsWindow *)makeWindowWithGLKView:(GLKView *)view {
    P3DGraphicsPipe *pipe = [P3DGraphicsPipe sharedPipe];
    P3DGraphicsStateGuardian *gsg = [[P3DGraphicsStateGuardian alloc] initWithContext:view.context engine:self pipe:pipe];
    
    FrameBufferProperties fbProps = FrameBufferProperties::get_default();
    //[gsg getWrappedGuardian]->props_from_glkview(*fbProps, view);
    
    WindowProperties winProps = WindowProperties::size(view.bounds.size.width, view.bounds.size.height);
    
    int flags = GraphicsPipe::BF_require_window;
    
    return (P3DGraphicsWindow *)[self makeOutputWithGraphicsPipe:pipe name:@"Panda" fbProps:fbProps windowProps:winProps flags:flags graphicsStateGuardian:gsg hostOutput:nil];
}

- (void)renderFrame {
    _wrappedEngine->render_frame();
}
@end
