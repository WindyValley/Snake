add_resources(resources bk.mp3 tada.wav body.bmp button.bmp food.bmp simkai.ttf)
add_library(res_lib STATIC res_lib.cpp "${resources}")
target_include_directories(res_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")