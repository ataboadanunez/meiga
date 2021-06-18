cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
#Adds our organic, home-grown search modules and etc to the module path of CMake
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

project(musim VERSION 1.0)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR}/Utilities ${CMAKE_SOURCE_DIR}/Framework)
#include_directories(${CMAKE_SOURCE_DIR}/Framework)
add_subdirectory(Utilities)
add_subdirectory(Framework)
add_subdirectory(Applications)

#add_executable(musim)
#target_link_libraries(musim PUBLIC Applications)
