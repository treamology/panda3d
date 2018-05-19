# Filename: FindOpenGLES.cmake
# Author: Donny Lawrence (19 May, 2018)
#
# Usage:
#   find_package(OPENGLES [REQUIRED] [QUIET])
#
# Once done this will define:
#   OPENGLES_FOUND       - true if OpenGLES is found on the system
#   OPENGLES_INCLUDE_DIR - the OpenGLES include directory
#   OPENGLES_LIBRARY     - the OpenGLES library
#
# The following variables will be checked by the function
#   OPENGLES_ROOT - if set, the libraries are exclusively searched under this path
#

if(NOT OPENGLES_INCLUDE_DIR)
  # Check if we can use PkgConfig
  find_package(PkgConfig QUIET)

  if(PKG_CONFIG_FOUND AND NOT OPENGLES_ROOT)
    pkg_check_modules(PKG_OPENGLES QUIET "OpenGLES")
  endif()

  # This really is only meant for iOS right now, it needs to be expanded for
  # Android.
  find_path(OPENGLES_INCLUDE_DIR
    NAMES "OpenGLES/ES1/gl.h"
    PATH_SUFFIXES "Headers"
  )
  set(OPENGLES_LIBRARY ${OPENGLES_INCLUDE_DIR} CACHE PATH "")

  mark_as_advanced(OPENGLES_INCLUDE_DIR)
  mark_as_advanced(OPENGLES_LIBRARY)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPENGLES DEFAULT_MSG OPENGLES_INCLUDE_DIR
  OPENGLES_LIBRARY)