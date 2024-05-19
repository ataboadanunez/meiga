cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(meiga VERSION 3.0)
# set(CMAKE_BUILD_TYPE Debug)
set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/Config/" CACHE STRING "Path to configuration files")

################################################################################
# External Packages                                                            #
################################################################################

# Find Geant4 package
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIRS})
message(STATUS "Geant4 include directories: ${Geant4_INCLUDE_DIRS}")

# Find boost package
find_package(Boost 1.34 REQUIRED COMPONENTS system filesystem iostreams)

################################################################################
# Subdirectories                                                               #
################################################################################
include_directories(${CMAKE_SOURCE_DIR}/Utilities ${CMAKE_SOURCE_DIR}/Framework/Event ${CMAKE_SOURCE_DIR}/Framework/Detector ${CMAKE_SOURCE_DIR}/Framework/ConfigManager ${CMAKE_SOURCE_DIR}/Framework/SimData ${CMAKE_SOURCE_DIR}/IO ${CMAKE_SOURCE_DIR}/G4Models ${CMAKE_SOURCE_DIR}/Documentation ${CMAKE_SOURCE_DIR}/Externals ${CMAKE_SOURCE_DIR}/UnitTest)
add_subdirectory(Externals)
add_subdirectory(Utilities)
add_subdirectory(IO)
add_subdirectory(Framework)
add_subdirectory(G4Models)
add_subdirectory(Applications)
add_subdirectory(UnitTest)