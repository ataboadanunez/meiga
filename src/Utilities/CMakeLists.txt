file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set(MEIGA_LIBRARIES meiga_utilities)
add_library(meiga_utilities STATIC ${sources} ${headers})
set(MEIGA_LIBRARIES meiga_utilities PARENT_SCOPE)
install(FILES ${headers} DESTINATION include)
install(TARGETS ${MEIGA_LIBRARIES} DESTINATION lib)
