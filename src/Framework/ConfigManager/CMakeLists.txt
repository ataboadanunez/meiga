# Set the path to DetectorProperties.xml as a CMake variable
set(PATH_TO_PROPERTIES "${CMAKE_BINARY_DIR}/Framework/ConfigManager/DetectorProperties.xml")
# Configure the DefaultProperties.h.in file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DefaultProperties.h.in ${CMAKE_CURRENT_SOURCE_DIR}/DefaultProperties.h)

file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(meiga_framework_configmanager STATIC ${sources} ${headers})
set(MEIGA_LIBRARIES meiga_framework_configmanager ${MEIGA_LIBRARIES} PARENT_SCOPE)
target_link_libraries(meiga_framework_configmanager meiga_framework_event)
install(FILES ${headers} DESTINATION include)
install(TARGETS meiga_framework_configmanager DESTINATION lib)

# Copy DetectorProperties.xml to the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/DetectorProperties.xml DESTINATION ${CMAKE_BINARY_DIR}/Framework/ConfigManager)
# Copy ExampleG4AppSimulator.json to the build directory (used in UnitTest)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ExampleG4AppSimulator.json DESTINATION ${CMAKE_BINARY_DIR}/Framework/ConfigManager)
