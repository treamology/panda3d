if(NOT HAVE_EGG)
  return()
endif()

add_executable(egg-palettize eggPalettize.cxx eggPalettize.h)
target_link_libraries(egg-palettize p3palettizer p3eggbase p3progbase)
install(TARGETS egg-palettize EXPORT Tools COMPONENT Tools DESTINATION bin)

add_library(p3txafile txaFileFilter.cxx txaFileFilter.h txaFileFilter.I)
set_target_properties(p3txafile PROPERTIES DEFINE_SYMBOL BUILDING_MISC)
target_link_libraries(p3txafile PRIVATE p3palettizer)
install(TARGETS p3txafile EXPORT Tools COMPONENT Tools DESTINATION lib RUNTIME DESTINATION bin)
