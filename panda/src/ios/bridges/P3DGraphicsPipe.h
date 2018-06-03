//
//  P3DGraphicsPipe.h
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#import <Foundation/Foundation.h>

struct IOSGraphicsPipe;

@interface P3DGraphicsPipe : NSObject

+ (id)sharedPipe;
- (id)init;

- (IOSGraphicsPipe *)getWrappedPipe;

@end
