if(HAVE_GLES2)
  if(NOT IOS)
    message("") # Add extra line before error
    message(SEND_ERROR
      "When compiling with OpenGLES2 (HAVE_GLES2), at least one of:
    IOS must be defined.")
  endif()

  set(PANDAGLES2_LINK_TARGETS p3gles2gsg)

  if(IOS)
    set(PANDAGLES2_LINK_TARGETS ${PANDAGLES2_LINK_TARGETS} p3iphonedisplay)
  endif()

  add_metalib(pandagles2 pandagles2.cxx COMPONENTS ${PANDAGLES2_LINK_TARGETS})
  set_target_properties(pandagles2 PROPERTIES DEFINE_SYMBOL BUILDING_PANDAGLES2)

  install(TARGETS pandagles2 DESTINATION lib)
endif()