if(NOT HAVE_GLES2)
  return()
endif()

set(P3GLES2GSG_HEADERS
  config_gles2gsg.h
  gles2gsg.h
)

set(P3GLES2GSG_SOURCES
  config_gles2gsg.cxx
  gles2gsg.cxx
)

composite_sources(p3gles2gsg P3GLES2GSG_SOURCES)
add_component_library(p3gles2gsg SYMBOL BUILDING_PANDAGLES2
  ${P3GLE2SGSG_HEADERS} ${P3GLES2GSG_SOURCES})
target_link_libraries(p3gles2gsg p3glstuff panda
  PKG::GLES2)

if(NOT BUILD_METALIBS)
  install(TARGETS p3gles2gsg
    EXPORT GLES COMPONENT GLES
    DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE COMPONENT GLESDevel)
endif()
install(FILES ${P3GLES2GSG_HEADERS} COMPONENT GLESDevel DESTINATION include/panda3d)
