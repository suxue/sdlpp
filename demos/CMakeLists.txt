add_executable(hello hello.cpp)
target_link_libraries(hello sdlpp)

add_executable(basic_draw basic_draw.cpp)
target_link_libraries(basic_draw sdlpp)

add_executable(color_key color_key.cpp)
target_link_libraries(color_key sdlpp)

add_executable(sprite sprite.cpp)
target_link_libraries(sprite sdlpp)
