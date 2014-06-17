/*!
 * @section LICENSE
 * Copyright (c) 2014 Hao Fei <mrfeihao@gmail.com>
 *
 * This file is part of libsdlpp.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "common.hpp"
#include <iostream>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace sdlpp;
using namespace std;
using namespace sdlpp::event;


int main (int argc, char *argv[])
{
    auto sdl = Initializer().acquire();
    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangle(SCREEN_WIDTH, SCREEN_HEIGHT),
                                    WindowMode().opengl());
    auto renderer = window.getRenderer();
    Color white(0xff, 0xff, 0xff);
    renderer.setDrawColor(white);
    renderer.clear();

    Color red(0xff, 0x00, 0x00);
    auto fillRect = Rectangle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                Position(SCREEN_WIDTH/4, SCREEN_HEIGHT/4));
    renderer.setDrawColor(red);
    renderer.fillRect(fillRect);

    Color blue(0x00, 0x00, 0xFF);
    renderer.setDrawColor(blue);
    renderer.drawLine(Position(0, SCREEN_HEIGHT / 2),
                      Position(SCREEN_WIDTH, SCREEN_HEIGHT / 2));

    Color green(0x00, 0xff, 0x00);
    renderer.setDrawColor(green);
    for (auto i = 0; i < SCREEN_HEIGHT; i += 4) {
        renderer.drawPoint(Position(SCREEN_WIDTH / 2, i));
    }
    idlewait(&renderer, window);
    return 0;
}
