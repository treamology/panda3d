/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file config_iosdisplay.mm
 * @author drose
 * @date 2009-04-08
 */

#include "config_iosdisplay.h"
#include "iOSGraphicsPipe.h"
#include "iPhoneGraphicsStateGuardian.h"
#include "iPhoneGraphicsWindow.h"

#include "graphicsPipeSelection.h"
#include "dconfig.h"
#include "pandaSystem.h"


Configure(config_iosdisplay);

NotifyCategoryDef(iosdisplay, "display");

ConfigureFn(config_iosdisplay) {
  init_libiosdisplay();
}

ConfigVariableBool ios_autorotate_view
("ios-autorotate-view", true,
 PRC_DESC("Set this true to enable the iOS application to rotate its "
          "view automatically according to the phone's orientation, or "
          "false for its view to remain fixed."));

/**
 * Initializes the library.  This must be called at least once before any of
 * the functions or classes in this library can be used.  Normally it will be
 * called by the static initializers and need not be called explicitly, but
 * special cases exist.
 */
void
init_libiosdisplay() {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  IOSGraphicsPipe::init_type();
  IPhoneGraphicsWindow::init_type();
  IPhoneGraphicsStateGuardian::init_type();

  GraphicsPipeSelection *selection = GraphicsPipeSelection::get_global_ptr();
  selection->add_pipe_type(IOSGraphicsPipe::get_class_type(), IOSGraphicsPipe::pipe_constructor);

  PandaSystem *ps = PandaSystem::get_global_ptr();
  ps->set_system_tag("OpenGL", "window_system", "iOS");
  ps->set_system_tag("OpenGL ES", "window_system", "iOS");

  GLESinit_classes();
}

/**
 * Returns the TypeHandle index of the recommended graphics pipe type defined
 * by this module.
 */
int
get_pipe_type_iosdisplay() {
  return IOSGraphicsPipe::get_class_type().get_index();
}
