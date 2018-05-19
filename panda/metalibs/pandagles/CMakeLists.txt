if(HAVE_GLES)
  if(NOT IOS)
    message("") # Add extra line before error
    message(SEND_ERROR
      "When compiling with OpenGLES (HAVE_GLES), at least one of:
    IOS must be defined.")
  endif()

  set(PANDAGLES_LINK_TARGETS p3glesgsg)

  if(IOS)
    set(PANDAGLES_LINK_TARGETS ${PANDAGLES_LINK_TARGETS} p3iphonedisplay)
  endif()

  add_metalib(pandagles pandagles.cxx COMPONENTS ${PANDAGLES_LINK_TARGETS})
  set_target_properties(pandagles PROPERTIES DEFINE_SYMBOL BUILDING_PANDAGLES)

  if(APPLE)
    set_target_properties(pandagles PROPERTIES COMPILE_FLAGS "-ObjC++")
  endif()

  install(TARGETS pandagles DESTINATION lib)
endif()