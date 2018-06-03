//
//  P3DGraphicsOutput.h
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#include <graphicsOutput.h>
#import <Foundation/Foundation.h>

@protocol P3DGraphicsOutput <NSObject>

- (GraphicsOutput *)getWrappedOutput;

@end
