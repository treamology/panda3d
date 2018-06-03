/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file iOSGraphicsStateGuardian.mm
 * @author drose
 * @date 2009-04-08
 */

#include "iOSGraphicsStateGuardian.h"
#include "string_utils.h"
#include "config_iosdisplay.h"
#include "depthWriteAttrib.h"
#include "depthTestAttrib.h"
#include "textureAttrib.h"
#include "pnmImage.h"
// #include "glesgsg.h"

TypeHandle IOSGraphicsStateGuardian::_type_handle;

/**
 * Returns the pointer to the GL extension function with the indicated name.
 * It is the responsibility of the caller to ensure that the required
 * extension is defined in the OpenGL runtime prior to calling this; it is an
 * error to call this for a function that is not defined.
 */
void *IOSGraphicsStateGuardian::
get_extension_func(const char *prefix, const char *name) {
  return NULL;
}

/**
 *
 */
IOSGraphicsStateGuardian::
IOSGraphicsStateGuardian(GraphicsEngine *engine, GraphicsPipe *pipe,
                         IOSGraphicsStateGuardian *share_with) :
  GLES2GraphicsStateGuardian(engine, pipe),
  _share_with(share_with)
{
  _shared_buffer = 1011;
  _is_valid = true;
  _needs_reset = false;

  // // Go ahead and create the context.
  // context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  // if (context != nil) {
  //   if (![EAGLContext setCurrentContext:context]) {
  //     printf("Error setting the context");
  //     exit(1);
  //   }
  // } else {
  //   exit(1);
  // }
  
}

/**
 *
 */
IOSGraphicsStateGuardian::
~IOSGraphicsStateGuardian() {
}

/**
 * Resets all internal state as if the gsg were newly created.
 */
void IOSGraphicsStateGuardian::
reset() {
  GLES2GraphicsStateGuardian::reset();
}

/**
 * Fills in the fb_props member with the appropriate values according to the
 * chosen pixel format.
 */
void IOSGraphicsStateGuardian::
props_from_glkview(FrameBufferProperties &fb_props, GLKView *glview) {
    fb_props.clear();
    fb_props.set_all_specified();

    fb_props.set_rgba_bits(8, 8, 8, 8);
    if (glview.drawableColorFormat == GLKViewDrawableColorFormatSRGBA8888) {
        fb_props.set_srgb_color(true);
    } else if (glview.drawableColorFormat == GLKViewDrawableColorFormatRGB565) {
        fb_props.set_rgba_bits(5, 6, 5, 0); // ?
    }

    fb_props.set_depth_bits(0);
    if (glview.drawableDepthFormat == GLKViewDrawableDepthFormat16) {
        fb_props.set_depth_bits(16);
    } else if (glview.drawableDepthFormat == GLKViewDrawableDepthFormat24) {
        fb_props.set_depth_bits(24);
    }

    fb_props.set_stencil_bits(0);
    if (glview.drawableStencilFormat == GLKViewDrawableStencilFormat8) {
        fb_props.set_depth_bits(8);
    }

    fb_props.set_coverage_samples(0);
    if (glview.drawableMultisample == GLKViewDrawableMultisample4X) {
        fb_props.set_coverage_samples(4);
    }

}
