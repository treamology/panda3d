function(add_library target_name)
  _add_library("${target_name}" ${ARGN})
  get_target_property(type "${target_name}" TYPE)
  if(type STREQUAL "SHARED_LIBRARY")
    set_target_properties("${target_name}" PROPERTIES
      VERSION "${PROJECT_VERSION}")
  endif()
endfunction(add_library)
