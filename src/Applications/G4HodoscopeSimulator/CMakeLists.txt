#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(G4HodoscopeSimulator)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(G4HodoscopeSimulator G4HodoscopeSimulator.cc ${sources} ${headers})
target_link_libraries(G4HodoscopeSimulator ${MEIGA_LIBRARIES} ${Geant4_LIBRARIES})


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#

install(TARGETS G4HodoscopeSimulator DESTINATION bin)

#----------------------------------------------------------------------------
# Copy Configuration files to build directory
#
set(config_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../Framework/ConfigManager")
set(copy_source_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(copy_dest_dir "${CMAKE_BINARY_DIR}/Applications/G4HodoscopeSimulator")
set(copy_file_name "G4HodoscopeSimulator.json")
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${copy_dest_dir}
)
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
  COMMENT "Copying ${copy_file_name} to build directory"
)
set(copy_file_name "DetectorList.xml")
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${copy_dest_dir}
)
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
  COMMENT "Copying ${copy_file_name} to build directory"
)
set(copy_file_name "vertical_muon.txt")
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${copy_dest_dir}
)
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
  COMMENT "Copying ${copy_file_name} to build directory"
)
set(copy_file_name "DetectorProperties.xml")
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${copy_dest_dir}
)
add_custom_command(
  TARGET G4HodoscopeSimulator POST_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy "${config_dir}/${copy_file_name}" "${copy_dest_dir}/${copy_file_name}"
  COMMENT "Copying ${copy_file_name} to build directory"
)


