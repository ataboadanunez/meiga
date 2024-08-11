cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
#Adds our organic, home-grown search modules and etc to the module path of CMake
# set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_BUILD_TYPE Debug)

project(meiga VERSION 2.1.1)
set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/Config/" CACHE STRING "Path to configuration files")

################################################################################
# External Packages                                                            #
################################################################################

# Find Geant4 package
set(REQUIRED_GEANT4_VERSION "10.7.2")
find_package(Geant4 ${REQUIRED_GEANT4_VERSION} REQUIRED)
if(NOT Geant4_VERSION VERSION_EQUAL REQUIRED_GEANT4_VERSION)
  message(FATAL_ERROR "Geant4 version ${REQUIRED_GEANT4_VERSION} is required. Found version: ${Geant4_VERSION}. Please use the correct version.")
endif()
include(${Geant4_USE_FILE})

# Find boost package
#find_package(Boost 1.34 ${Boost_is_REQUIRED})
find_package(Boost 1.34 REQUIRED COMPONENTS system filesystem iostreams)

# Find nlohman for json parser
# find_package(nlohmann_json 3.2.0 REQUIRED)

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
#add_subdirectory(Documentation)
# Define a variable for the path to the configuration file
#set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/Framework/ConfigManager/" CACHE STRING "Path to configuration files")
