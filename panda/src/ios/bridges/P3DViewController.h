//
//  P3DViewController.h
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#import "P3DGraphicsEngine.h"
#import "P3DGraphicsWindow.h"

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface P3DViewController : UIViewController <GLKViewDelegate>

@property (nonatomic, weak) P3DGraphicsEngine *engine;
@property (nonatomic) P3DGraphicsWindow *window;

- (id)init;

@end
