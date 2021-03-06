if(CMAKE_SYSTEM_NAME STREQUAL iOS)
  cmake_minimum_required(VERSION 3.14)
else()
  cmake_minimum_required(VERSION 3.0.2)
endif()

set(CMAKE_DISABLE_SOURCE_CHANGES ON) # Must go before project() below
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON) # Must go before project() below

get_property(IS_MULTICONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${CMAKE_VERSION} VERSION_GREATER 3.8.0)
  # When using the Xcode generator, don't append the platform name to the
  # intermediate configuration directory.
  set_property(GLOBAL PROPERTY XCODE_EMIT_EFFECTIVE_PLATFORM_NAME OFF)
endif()

# Figure out the version
file(STRINGS "setup.cfg" _version REGEX "^version = ")
string(REGEX REPLACE "^.*= " "" _version "${_version}")
project(Panda3D VERSION ${_version})
unset(_version)

enable_testing()

# Create some convenience variables for the build configuration intdir. One uses
# the build system's representation, while the other uses a generator expression.
string(REPLACE "$(EFFECTIVE_PLATFORM_NAME)" "" PANDA_CFG_INTDIR "${CMAKE_CFG_INTDIR}")
if(PANDA_CFG_INTDIR STREQUAL ".")
  set(PANDA_CFG_INTDIR_GEN ".")
else()
  set(PANDA_CFG_INTDIR_GEN "$<CONFIG>")
endif()

# Add generic modules to cmake module path,
# and add Panda3D specific modules to cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/macros/")


# Include global modules needed for configure scripts
include(PackageConfig)      # Defines package_option

# Configure Panda3D
include(dtool/CompilerFlags.cmake)
include(dtool/PandaVersion.cmake)
include(dtool/Package.cmake)
include(dtool/Config.cmake)

# Include global modules
include(AddBisonTarget)     # Defines add_bison_target function
include(AddFlexTarget)      # Defines add_flex_target function
include(BuildMetalib)       # Defines add_component_library AND add_metalib
include(CompositeSources)   # Defines composite_sources function
include(Python)             # Defines add_python_target AND install_python_package
include(Interrogate)        # Defines target_interrogate AND add_python_module
include(RunPzip)            # Defines run_pzip function
include(Versioning)         # Hooks 'add_library' to apply VERSION/SOVERSION

# Determine which trees to build.
option(BUILD_DTOOL  "Build the dtool source tree." ON)
option(BUILD_PANDA  "Build the panda source tree." ON)
option(BUILD_DIRECT "Build the direct source tree." ON)
option(BUILD_PANDATOOL "Build the pandatool source tree." ON)
option(BUILD_CONTRIB "Build the contrib source tree." ON)
option(BUILD_MODELS "Build/install the built-in models." ON)

if(CMAKE_SYSTEM_NAME STREQUAL iOS)
  set(BUILD_PANDATOOL OFF)
endif()

# Include Panda3D packages
if(BUILD_DTOOL)
  add_subdirectory(dtool "${CMAKE_BINARY_DIR}/dtool")
endif()

if(BUILD_PANDA)
  add_subdirectory(panda "${CMAKE_BINARY_DIR}/panda")
endif()

if(BUILD_DIRECT)
  add_subdirectory(direct "${CMAKE_BINARY_DIR}/direct")
endif()

if(BUILD_PANDATOOL)
  add_subdirectory(pandatool "${CMAKE_BINARY_DIR}/pandatool")
endif()

if(BUILD_CONTRIB)
  add_subdirectory(contrib "${CMAKE_BINARY_DIR}/contrib")
endif()

if(BUILD_MODELS)
  run_pzip(models
    "${CMAKE_CURRENT_SOURCE_DIR}/models/"
    "${PROJECT_BINARY_DIR}/${PANDA_CFG_INTDIR}/models"
    *.egg)
  run_pzip(dmodels
    "${CMAKE_CURRENT_SOURCE_DIR}/dmodels/src/"
    "${PROJECT_BINARY_DIR}/${PANDA_CFG_INTDIR}/models"
    *.egg)

  add_custom_command(TARGET models
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                             -DSOURCE="${CMAKE_CURRENT_SOURCE_DIR}/models/maps/"
                             -DDESTINATION="${PROJECT_BINARY_DIR}/${PANDA_CFG_INTDIR}/models/maps"
                             -P ${PROJECT_SOURCE_DIR}/cmake/scripts/CopyPattern.cmake
                     COMMENT "Copying models/maps")
  add_custom_command(TARGET dmodels
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                             -DSOURCE="${CMAKE_CURRENT_SOURCE_DIR}/dmodels/src/"
                             -DDESTINATION="${PROJECT_BINARY_DIR}/${PANDA_CFG_INTDIR}/models"
                             -DFILES_MATCHING="PATTERN;*.rgb;PATTERN;*.png;PATTERN;*.jpg;PATTERN;*.wav"
                             -P ${PROJECT_SOURCE_DIR}/cmake/scripts/CopyPattern.cmake
                     COMMENT "Copying dmodels' assets")

  install(DIRECTORY "${PROJECT_BINARY_DIR}/${PANDA_CFG_INTDIR_GEN}/models"
    COMPONENT Models DESTINATION share/panda3d)
endif()

if(INTERROGATE_PYTHON_INTERFACE)
  # If we built the Python interface, run the test suite.  Note, we do NOT test
  # for pytest before adding this test.  If the user doesn't have pytest, we'd
  # like for the tests to fail.

  if(PANDA_CFG_INTDIR STREQUAL ".")
    set(_workdir "${PROJECT_BINARY_DIR}")
  else()
    set(_workdir "${PROJECT_BINARY_DIR}/$<CONFIG>")
  endif()

  add_test(NAME pytest
    COMMAND "${PYTHON_EXECUTABLE}" -m pytest "${PROJECT_SOURCE_DIR}/tests"
    WORKING_DIRECTORY "${_workdir}")
endif()

# Generate the Panda3DConfig.cmake file so find_package(Panda3D) works, and
# also register the build directory with CMake's package registry.

file(COPY "${PROJECT_SOURCE_DIR}/cmake/install/Panda3DConfig.cmake"
  DESTINATION "${PROJECT_BINARY_DIR}")
install(FILES "${PROJECT_SOURCE_DIR}/cmake/install/Panda3DConfig.cmake"
  DESTINATION "lib/cmake/Panda3D")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/Panda3DConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion)
install(FILES "${PROJECT_BINARY_DIR}/Panda3DConfigVersion.cmake"
  DESTINATION "lib/cmake/Panda3D")

if(NOT CMAKE_CROSSCOMPILING)
  export(PACKAGE Panda3D)
endif()
