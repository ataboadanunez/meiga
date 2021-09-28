#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(G4RockSimulator)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(G4RockSimulator G4RockSimulator.cc ${sources} ${headers})
target_link_libraries(G4RockSimulator ${MEIGA_LIBRARIES} ${Geant4_LIBRARIES})


#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(G4RockSimulator DEPENDS userG4Application)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#

install(TARGETS G4RockSimulator DESTINATION bin)
