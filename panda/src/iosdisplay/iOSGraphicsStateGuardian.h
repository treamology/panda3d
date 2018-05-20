/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file iOSGraphicsStateGuardian.h
 * @author drose
 * @date 2009-04-08
 */

#ifndef IOSGRAPHICSSTATEGUARDIAN_H
#define IOSGRAPHICSSTATEGUARDIAN_H

#include "pandabase.h"
#include "glesgsg.h"

#include "iPhoneGraphicsWindow.h"

class IPhoneGraphicsWindow;

/**
 *
 */
class IOSGraphicsStateGuardian : public GLESGraphicsStateGuardian {
public:
  IOSGraphicsStateGuardian(GraphicsEngine *engine, GraphicsPipe *pipe,
                              IOSGraphicsStateGuardian *share_with);
  virtual ~IOSGraphicsStateGuardian();
  virtual void reset();

protected:
  virtual void *get_extension_func(const char *prefix, const char *name);

private:
  void describe_pixel_format(FrameBufferProperties &fb_props);

  // We have to save a pointer to the GSG we intend to share texture context
  // with, since we don't create our own context in the constructor.
  PT(IOSGraphicsStateGuardian) _share_with;

public:
  GLint _shared_buffer;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    GLESGraphicsStateGuardian::init_type();
    register_type(_type_handle, "IOSGraphicsStateGuardian",
                  GLESGraphicsStateGuardian::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;

  friend class IPhoneGraphicsBuffer;
};


#endif
