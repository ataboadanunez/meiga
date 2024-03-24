cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
#Adds our organic, home-grown search modules and etc to the module path of CMake
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

project(meiga VERSION 1.0)

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
set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/Framework/ConfigManager/" CACHE STRING "Path to configuration files")
