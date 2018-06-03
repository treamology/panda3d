if(NOT APPLE OR NOT IOS)
  return()
endif()

set(P3IOSBRIDGES_HEADERS
    P3DGraphicsEngine.h
    P3DGraphicsPipe.h
    P3DViewController.h
    P3DGraphicsWindow.h
    P3DGraphicsStateGuardian.h
    P3DGraphicsOutput.h)

set(P3IOSBRIDGES_SOURCES
    P3DGraphicsEngine.mm
    P3DGraphicsPipe.mm
    P3DViewController.m
    P3DGraphicsWindow.mm
    P3DGraphicsStateGuardian.mm)

composite_sources(p3iosbridges P3IOSBRIDGES_SOURCES)
add_component_library(p3iosbridges ${P3IOSBRIDGES_HEADERS} ${P3IOSBRIDGES_SOURCES})
set_target_properties(p3iosbridges PROPERTIES DEFINE_SYMBOL BUILDING_PANDAGLES COMPILE_FLAGS "-ObjC++ -fobjc-arc")
target_link_libraries(p3iosbridges p3iosdisplay)

# Frameworks:
# find_library(APPLICATIONSERVICES_LIBRARY ApplicationServices)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(UIKIT_LIBRARY UIKit)
find_library(GLKIT_LIBRARY GLKit)
target_link_libraries(p3iosbridges ${UIKIT_LIBRARY} ${QUARTZCORE_LIBRARY} ${GLKIT_LIBRARY})

install(TARGETS p3iosbridges DESTINATION lib)
install(FILES ${P3IOSBRIDGES_HEADERS} DESTINATION include/panda3d)
