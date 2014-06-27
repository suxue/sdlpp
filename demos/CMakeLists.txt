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

# macros in config.h
set(SDLPP_DEMO_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data/)

# config.h
CONFIGURE_FILE(config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(demos_common common.cpp)
target_include_directories(demos_common PRIVATE
    $<TARGET_PROPERTY:sdlpp,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(demos_common PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(hello hello.cpp)
target_link_libraries(hello sdlpp demos_common)

add_executable(basic_draw basic_draw.cpp)
target_link_libraries(basic_draw sdlpp demos_common)

add_executable(color_key color_key.cpp)
target_link_libraries(color_key sdlpp demos_common)

add_executable(sprite sprite.cpp)
target_link_libraries(sprite sdlpp demos_common)

add_executable(color_mod color_mod.cpp)
target_link_libraries(color_mod sdlpp demos_common)

add_executable(canvas canvas.cpp)
target_link_libraries(canvas sdlpp demos_common)

if(LUA51_FOUND)
add_executable(luasdl luasdl.cpp)
target_link_libraries(luasdl luamm sdlpp demos_common)
target_include_directories(luasdl PRIVATE
    $<TARGET_PROPERTY:luamm,INTERFACE_INCLUDE_DIRECTORIES>)
endif()
