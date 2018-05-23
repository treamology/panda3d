if(NOT HAVE_GLES2)
  return()
endif()

set(P3GLES2GSG_HEADERS
  config_gles2gsg.h gles2gsg.h panda_esgl2ext.h
)

set(P3GLES2GSG_SOURCES
  config_gles2gsg.cxx
  gles2gsg.cxx
)

composite_sources(p3gles2gsg P3GLES2GSG_SOURCES)
add_component_library(p3gles2gsg SYMBOL BUILDING_PANDAGLES2
  ${P3GLES2GSG_HEADERS} ${P3GLES2GSG_SOURCES})
target_link_libraries(p3gles2gsg p3glstuff panda
  ${OPENGLES_LIBRARY})
target_use_packages(p3gles2gsg CG)

install(TARGETS p3gles2gsg DESTINATION lib)
install(FILES ${P3GLES2GSG_HEADERS} DESTINATION include/panda3d)