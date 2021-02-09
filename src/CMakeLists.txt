list(APPEND SRC_LIST main.cpp)
add_executable(Snake ${SRC_LIST})
list(APPEND THIRD_INC_DIRS ${SDL2_INCLUDE_DIRS})
list(APPEND THIRD_LINK_DIRS ${SDL2_LIBRARY_DIRS})
list(APPEND THIRD_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_ttf_LIBRARIES} ${SDL2_mixer_LIBRARIES})

target_include_directories(Snake PRIVATE ${THIRD_INC_DIRS})
target_link_directories(Snake PUBLIC ${THIRD_LINK_DIRS})
target_link_libraries(Snake ${THIRD_LIBRARIES} res_lib)

install(TARGETS Snake DESTINATION .)
install(CODE "include(BundleUtilities)
fixup_bundle(\${CMAKE_INSTALL_PREFIX}/Snake.exe \"\" \"\")"
)
