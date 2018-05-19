if(NOT HAVE_GLES)
  return()
endif()

set(P3GLESGSG_HEADERS
  config_glesgsg.h glesgsg.h panda_esglext.h
)

set(P3GLESGSG_SOURCES
  config_glesgsg.cxx
  glesgsg.cxx
)

composite_sources(p3glesgsg P3GLESGSG_SOURCES)
add_component_library(p3glesgsg SYMBOL BUILDING_PANDAGLES
  ${P3GLESGSG_HEADERS} ${P3GLESGSG_SOURCES})
target_link_libraries(p3glesgsg p3glstuff panda
  ${OPENGLES_LIBRARY})

install(TARGETS p3glesgsg DESTINATION lib)
install(FILES ${P3GLESGSG_HEADERS} DESTINATION include/panda3d)