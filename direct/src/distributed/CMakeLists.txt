if(NOT HAVE_PYTHON)
  return()
endif()

set(P3DISTRIBUTED_HEADERS
  config_distributed.h
  cConnectionRepository.I
  cConnectionRepository.h
  cDistributedSmoothNodeBase.I
  cDistributedSmoothNodeBase.h
)

set(P3DISTRIBUTED_SOURCES
  config_distributed.cxx
  cConnectionRepository.cxx
  cDistributedSmoothNodeBase.cxx
)

add_component_library(p3distributed NOINIT SYMBOL BUILDING_DIRECT_DISTRIBUTED
  ${P3DISTRIBUTED_HEADERS})
target_link_libraries(p3distributed p3directbase p3dcparser panda)
target_interrogate(p3distributed ALL EXTENSIONS ${P3DISTRIBUTED_SOURCES})

if(NOT BUILD_METALIBS)
  install(TARGETS p3distributed
    EXPORT Direct COMPONENT Direct
    DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE COMPONENT DirectDevel)
endif()
install(FILES ${P3DISTRIBUTED_HEADERS} COMPONENT DirectDevel DESTINATION include/panda3d)
