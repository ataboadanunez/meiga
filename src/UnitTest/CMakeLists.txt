#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(runTests)

set(CONFIG_FILE_PATH "${CMAKE_SOURCE_DIR}/UnitTest/files/" CACHE STRING "Path to configuration files")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(runTests runTests.cc ${sources} ${headers})
target_link_libraries(runTests ${MEIGA_LIBRARIES} ${Geant4_LIBRARIES})

# Add custom target for running tests
add_custom_target(test
    COMMAND runTests
    DEPENDS runTests
    COMMENT "Running unit tests"
)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS runTests DESTINATION bin)
target_compile_definitions(runTests PRIVATE CONFIG_FILE_PATH="${CONFIG_FILE_PATH}")