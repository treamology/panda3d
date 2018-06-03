//
//  P3DGraphicsWindow.h
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#import "P3DGraphicsOutput.h"
#import "P3DGraphicsStateGuardian.h"

#import <Foundation/Foundation.h>

struct IOSGraphicsWindow;

@interface P3DGraphicsWindow : NSObject <P3DGraphicsOutput>

@property (nonatomic) P3DGraphicsStateGuardian *gsg;

- (id)initWithWindow:(IOSGraphicsWindow *)window guardian:(P3DGraphicsStateGuardian *)gsg;
- (IOSGraphicsWindow *)getWrappedOutput;

@end
