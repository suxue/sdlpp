add_library(demos_common common.cpp)

add_executable(hello hello.cpp)
target_link_libraries(hello sdlpp)
target_link_libraries(hello demos_common)

add_executable(basic_draw basic_draw.cpp)
target_link_libraries(basic_draw sdlpp)
target_link_libraries(basic_draw demos_common)

add_executable(color_key color_key.cpp)
target_link_libraries(color_key sdlpp)
target_link_libraries(color_key demos_common)

add_executable(sprite sprite.cpp)
target_link_libraries(sprite sdlpp)
target_link_libraries(sprite demos_common)

add_executable(color_mod color_mod.cpp)
target_link_libraries(color_mod sdlpp)
target_link_libraries(color_mod demos_common)

add_executable(flip flip.cpp)
target_link_libraries(flip sdlpp)
target_link_libraries(flip demos_common)
