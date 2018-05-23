if(NOT APPLE OR NOT IOS)
  return()
endif()

set(P3IOSDISPLAY_HEADERS
    config_iosdisplay.h
    iOSGraphicsPipe.h
    iOSGraphicsWindow.h iOSGraphicsWindow.I
    iOSGraphicsStateGuardian.h
    viewController.h
    eaglView.h)

set(P3IOSDISPLAY_SOURCES
    config_iosdisplay.mm
    iOSGraphicsPipe.mm
    iOSGraphicsWindow.mm
    iOSGraphicsStateGuardian.mm
    viewController.mm
    eaglView.mm)

composite_sources(p3iosdisplay P3IOSDISPLAY_SOURCES)
add_component_library(p3iosdisplay ${P3IOSDISPLAY_HEADERS} ${P3IOSDISPLAY_SOURCES})
set_target_properties(p3iosdisplay PROPERTIES DEFINE_SYMBOL BUILDING_PANDAGLES COMPILE_FLAGS "-ObjC++")
target_link_libraries(p3iosdisplay p3display p3putil p3gles2gsg)

# Frameworks:
# find_library(APPLICATIONSERVICES_LIBRARY ApplicationServices)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(UIKIT_LIBRARY UIKit)
target_link_libraries(p3iosdisplay ${UIKIT_LIBRARY} ${QUARTZCORE_LIBRARY})

install(TARGETS p3iosdisplay DESTINATION lib)
install(FILES ${P3IOSDISPLAY_HEADERS} DESTINATION include/panda3d)
