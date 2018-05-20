/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file config_iosdisplay.h
 * @author drose
 * @date 2009-04-08
 */

#ifndef CONFIG_IOSDISPLAY_H
#define CONFIG_IOSDISPLAY_H

#include "pandabase.h"
#include "notifyCategoryProxy.h"
#include "configVariableBool.h"
#include "configVariableInt.h"

NotifyCategoryDecl(iosdisplay, EXPCL_MISC, EXPTP_MISC);

extern ConfigVariableBool ios_autorotate_view;

extern EXPCL_MISC void init_libiosdisplay();
extern "C" EXPCL_MISC int get_pipe_type_iosdisplay();

#endif  // CONFIG_IOSDISPLAY_H
