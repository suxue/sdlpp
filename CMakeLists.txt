cmake_minimum_required(VERSION 2.6)
project(sdlpp)

#add_library(libsdlpp sdlpp_main.cpp)
#link_libraries(libsdlpp ${SDL2_LIBRARIES})
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()


add_executable(sdlpp_test test.cpp)
target_link_libraries(sdlpp_test
    ${Boost_TEST_EXEC_MONITOR_LIBRARY})

add_executable(sdldummy sdldummy.cpp)
target_link_libraries(sdldummy ${SDL2_LIBRARIES})

#target_link_libraries(sdl2test ${SDL2_LIBRARIES})
#target_link_libraries(test libsdlpp)
