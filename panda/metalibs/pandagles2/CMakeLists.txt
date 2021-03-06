if(NOT HAVE_GLES2 OR NOT HAVE_EAGL)
  return()
endif()

set(PANDAGLES2_LINK_TARGETS p3gles2gsg p3glstuff)

if(HAVE_EGL)
  list(APPEND PANDAGLES2_LINK_TARGETS p3egldisplay_gles2 p3x11display)
  set(PANDAGLES2_PIPE_TYPE "eglGraphicsPipe")
  set(PANDAGLES2_PIPE_INCLUDE "eglGraphicsPipe.h")
elseif(HAVE_EAGL)
  list(APPEND PANDAGLES2_LINK_TARGETS p3eagldisplay)
  set(PANDAGLES2_PIPE_TYPE "EAGLGraphicsPipe")
  set(PANDAGLES2_PIPE_INCLUDE "eaglGraphicsPipe.h")
endif()

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "GLESDevel")
add_metalib(pandagles2 ${MODULE_TYPE}
  INCLUDE ${PANDAGLES2_PIPE_INCLUDE}
  INIT init_libpandagles2 pandagles2.h
  EXPORT int get_pipe_type_pandagles2 "${PANDAGLES2_PIPE_TYPE}::get_class_type().get_index()"
  COMPONENTS ${PANDAGLES2_LINK_TARGETS})
unset(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME)

if(CMAKE_SYSTEM_NAME STREQUAL iOS)
  # We won't be linking with libpython until the wrapper app itself is built,
  # so we'll quelch the errors that get spit out when trying to build with
  # missing symbols.
  target_link_options(pandagles2 PRIVATE -undefined dynamic_lookup)
endif()

install(TARGETS pandagles2
  EXPORT GLES COMPONENT GLES
  DESTINATION ${MODULE_DESTINATION}
  ARCHIVE COMPONENT GLESDevel)

export_targets(GLES COMPONENT GLESDevel)
