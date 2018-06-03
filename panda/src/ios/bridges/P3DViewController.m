//
//  P3DViewController.m
//  Panda3D-CodeCompleteOnly
//
//  Created by Donny Lawrence on 6/2/18.
//  Copyright © 2018 Panda3D. All rights reserved.
//

#import "P3DViewController.h"



@interface P3DViewController ()

@end

@implementation P3DViewController

- (id)init {
    self = [super init];
    return self;
}

- (void)loadView {
    self.engine = [P3DGraphicsEngine sharedEngine];
    
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    GLKView *glView = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:context];
    
    self.window = [self.engine makeWindowWithGLKView:glView];
    
    glView.delegate = self;
    self.view = glView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    [self.engine renderFrame];
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
