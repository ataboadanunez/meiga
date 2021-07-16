#set(MUSIM_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Framework/SimData ${CMAKE_SOURCE_DIR}/Framework/ConfigManager ${CMAKE_SOURCE_DIR}/Framework/Detector  ${CMAKE_SOURCE_DIR}/Framework/Event)
add_subdirectory(SimData)
add_subdirectory(ConfigManager)
add_subdirectory(Detector)
add_subdirectory(Event)
set(MUSIM_LIBRARIES ${MUSIM_LIBRARIES} PARENT_SCOPE)

#add_executable(musim)
#target_link_libraries(musim PUBLIC Applications)
