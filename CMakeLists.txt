cmake_minimum_required(VERSION 2.6)
project(sdlpp)

add_library(sdlpp sdlpp.cpp)
link_libraries(sdlpp ${SDL2_LIBRARIES})

if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

add_executable(sdlpp_test test.cpp)
target_link_libraries(sdlpp_test
    ${Boost_TEST_EXEC_MONITOR_LIBRARY})
target_link_libraries(sdlpp_test sdlpp)

add_executable(demo demo.cpp)
target_link_libraries(demo sdlpp)

#target_link_libraries(sdl2test ${SDL2_LIBRARIES})
#target_link_libraries(test libsdlpp)
