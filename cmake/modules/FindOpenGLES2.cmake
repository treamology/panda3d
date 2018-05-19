# Filename: FindOpenGLES2.cmake
# Author: Donny Lawrence (19 May, 2018)
#
# Usage:
#   find_package(OPENGLES2 [REQUIRED] [QUIET])
#
# Once done this will define:
#   OPENGLES2_FOUND       - true if fftw is found on the system
#   OPENGLES2_INCLUDE_DIR - the fftw include directory
#   OPENGLES2_LIBRARY_DIR - the fftw library directory
#   OPENGLES2_LIBRARY     - the path to the library binary
#
# The following variables will be checked by the function
#   OPENGLES2_ROOT - if set, the libraries are exclusively searched under this path
#

if(NOT OPENGLES2_INCLUDE_DIR)
  # Check if we can use PkgConfig
  find_package(PkgConfig QUIET)

  if(PKG_CONFIG_FOUND AND NOT OPENGLES2_ROOT)
    pkg_check_modules(PKG_OPENGLES2 QUIET "OpenGLES2")
  endif()

  # Find the gles2 include files
  find_path(OPENGLES2_INCLUDE_DIR
    NAMES "OpenGLES/ES2/gl.h"
    PATH_SUFFIXES "Headers"
  )

  mark_as_advanced(OPENGLES2_INCLUDE_DIR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPENGLES2 DEFAULT_MSG OPENGLES2_INCLUDE_DIR)