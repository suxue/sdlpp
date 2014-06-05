cmake_minimum_required(VERSION 2.6)
project(sdlpp)

# add_library(libsdlpp csdldecl.hpp sdlpp.hpp)
add_executable(sdlpp_test test.cpp)

add_executable(sdldummy sdldummy.cpp)
target_link_libraries(sdldummy ${SDL2_LIBRARIES})

target_link_libraries(sdlpp_test
    ${Boost_TEST_EXEC_MONITOR_LIBRARY})

#target_link_libraries(sdl2test ${SDL2_LIBRARIES})
#target_link_libraries(test libsdlpp)
