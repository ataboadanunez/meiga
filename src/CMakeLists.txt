cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(meiga VERSION 3.0)
set(MEIGA_VERSION "${PROJECT_VERSION}" CACHE INTERNAL "Meiga project version")
set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_BUILD_TYPE Debug)
set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/Config/" CACHE STRING "Path to configuration files")

################################################################################
# External Packages                                                            #
################################################################################

# Find Geant4 package
# Set the required Geant4 version
set(MINIMUM_GEANT4_VERSION "11.2.0")

# Find the Geant4 package
find_package(Geant4 ${REQUIRED_GEANT4_VERSION} REQUIRED)

# Check if the found version of Geant4 matches the required version exactly
if(Geant4_VERSION VERSION_LESS MINIMUM_GEANT4_VERSION)
    message(FATAL_ERROR "Geant4 version ${MINIMUM_GEANT4_VERSION} or higher is required. Found version: ${Geant4_VERSION}.")
endif()
include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIRS})
message(STATUS "Geant4 include directories: ${Geant4_INCLUDE_DIRS}")

# Find boost package
find_package(Boost 1.34 REQUIRED COMPONENTS system filesystem iostreams)
# fixes boost/bind.hpp annoying warning during compilation
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

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