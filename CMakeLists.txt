# Copyright (c) 2014 Hao Fei <mrfeihao@gmail.com>
#
# This file is part of libsdlpp.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 2.6)
project(sdlpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(sdlpp sdlpp.cpp)
link_libraries(sdlpp ${SDL2_LIBRARY})
link_libraries(sdlpp SDL2_image)
if(MSVC)
    link_libraries(${SDL2_LIBRARY} winmm)
    link_libraries($(SDL2_LIBRARY) user32)
    link_libraries($(SDL2_LIBRARY) gdi32)
    link_libraries($(SDL2_LIBRARY) ole32)
    link_libraries($(SDL2_LIBRARY) shell32)
    link_libraries($(SDL2_LIBRARY) imm32)
    link_libraries($(SDL2_LIBRARY) version)
    link_libraries($(SDL2_LIBRARY) oleaut32)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  /SUBSYSTEM:windows")
endif()

add_executable(sdlpp_test test.cpp)
target_link_libraries(sdlpp_test ${Boost_TEST_EXEC_MONITOR_LIBRARY})
target_link_libraries(sdlpp_test sdlpp)

add_subdirectory(demos)
