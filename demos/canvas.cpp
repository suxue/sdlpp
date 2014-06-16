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

using namespace sdlpp;
using namespace sdlpp::event;
using namespace std;

int main(int argc, char *argv[])
{
    auto sdl = Initializer().video().events().acquire();
    auto window = sdl.createWindow("software rendering",
            Rectangular(600, 600));
    Surface dest(window.getSurface());
    Color white(0xff, 0xff, 0xff);
    Color red(0xff, 0, 0);
    Color green(0, 0xff, 0);
    Color blue(0, 0, 0xff);

    /*
     * rendering pixels
     */
    PixelFormat format = SDL_PIXELFORMAT_ARGB8888;
    Bpp4Surface canvas(10, 10, format);
    for (auto j = 0; j < 10; j++) {
        for (auto i = 0; i < 10; i++) {
            canvas[i][j] = white;
        }
    }

    canvas.line(Position(9, 0), Position(0, 6), green);

    /*
     * rendering pixel boundaries
     */
    Bpp4Surface network(dest.width(), dest.height(), format);
    for (auto i = 0; i < 600; i+=60) {
        network.line(Position(0, i), Position(599, i), blue);
        network.line(Position(i, 0), Position(i, 599), blue);
    }

    network.line(Position(599, 0), Position(0, 419), red);

    dest.blitScaled(canvas);
    dest.blit(network);
    window.update();
    idlewait(nullptr, window);
    return 0;
}
