file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(meiga_framework_simdata STATIC ${sources} ${headers})
target_link_libraries(meiga_framework_simdata meiga_framework_detector meiga_utilities)
set(MEIGA_LIBRARIES meiga_framework_simdata ${MEIGA_LIBRARIES} PARENT_SCOPE)
install(FILES ${headers} DESTINATION include)
install(TARGETS meiga_framework_simdata DESTINATION lib)
