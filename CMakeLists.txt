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

cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

find_package(C11)
Find_package(MyBoost)

find_package(SDL2)
include_directories( ${SDL2_INCLUDE_DIR} )
MESSAGE(STATUS "SDL2_FOUND = ${SDL2_FOUND}")
MESSAGE(STATUS "SDL_INCLUDE_DIR = ${SDL2_INCLUDE_DIR}")
MESSAGE(STATUS "SDL_LIBRARY = ${SDL2_LIBRARY}")

add_library(sdlpp sdlpp.cpp)
target_link_libraries(sdlpp ${SDL2_LIBRARY})
target_link_libraries(sdlpp SDL2_image)

target_include_directories(sdlpp INTERFACE ./)
target_include_directories(sdlpp PRIVATE ${SDL2_INCLUDE_DIR})

add_executable(sdlpp_test test.cpp)
target_link_libraries(sdlpp_test ${Boost_TEST_EXEC_MONITOR_LIBRARY})
target_link_libraries(sdlpp_test sdlpp)

if (BUILD_DEMOS)
    add_subdirectory(demos)
endif()

enable_testing ()
ADD_TEST(NAME testdep/sdlpp  COMMAND "${CMAKE_COMMAND}" --build
    ${CMAKE_BINARY_DIR} --target sdlpp_test)
add_test (NAME test/sdlpp COMMAND sdlpp_test -o=report)
SET_TESTS_PROPERTIES (test/sdlpp PROPERTIES DEPENDS testdep/sdlpp )
