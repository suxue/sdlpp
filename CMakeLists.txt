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

add_library(sdlpp sdlpp.cpp)
target_link_libraries(sdlpp ${SDL2_LIBRARY})
target_link_libraries(sdlpp SDL2_image)
if(MSVC)
    target_link_libraries(sdlpp winmm)
    target_link_libraries(sdlpp user32)
    target_link_libraries(sdlpp gdi32)
    target_link_libraries(sdlpp ole32)
    target_link_libraries(sdlpp shell32)
    target_link_libraries(sdlpp imm32)
    target_link_libraries(sdlpp version)
    target_link_libraries(sdlpp oleaut32)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  /SUBSYSTEM:windows")
endif()

add_executable(sdlpp_test test.cpp)
target_link_libraries(sdlpp_test ${Boost_TEST_EXEC_MONITOR_LIBRARY})
target_link_libraries(sdlpp_test sdlpp)

if (BUILD_DEMOS)
    add_subdirectory(demos)
endif()


add_test (NAME sdlpp_test COMMAND sdlpp_test -o=report)
