/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file viewController.h
 * @author drose
 * @date 2009-04-10
 */

#include "pandabase.h"

#import <UIKit/UIKit.h>
#import <UIKit/UITextView.h>

class IOSGraphicsPipe;

@interface ControllerDemoViewController : UIViewController {

@private
  IOSGraphicsPipe *_pipe;
}

- (id)initWithPipe: (IOSGraphicsPipe *)pipe;
@end
