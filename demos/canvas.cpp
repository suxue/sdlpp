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
            Rectangle(600, 600));
    Surface dest(window.getSurface());

    /*
     * rendering pixels
     */
    PixelFormat format = SDL_PIXELFORMAT_ARGB8888;
    Bpp4Surface canvas(10, 10, format);
    canvas.setDrawColor(Color::White);
    canvas.clear();


    canvas.setDrawColor(Color::Lime);
    canvas.drawLine(Position(9, 0), Position(0, 6));

    /*
     * rendering pixel boundaries
     */
    Bpp4Surface network(dest.width(), dest.height(), format);
    network.setDrawColor(Color::Blue);
    for (auto i = 0; i < 600; i+=60) {
        network.drawLine(Position(0, i), Position(599, i));
        network.drawLine(Position(i, 0), Position(i, 599));
    }

    network.setDrawColor(Color::Red);
    network.drawLine(Position(599, 0), Position(0, 419));

    network.setDrawColor(Color::Black);
    network.drawEllipse(Position(200, 100), Position(100, 50));
    network.drawPoint(Position(200, 100)); // ellipse center
    network.drawLine(Position(200, 50), Position(200, 150)); // horizontal
    network.drawLine(Position(100, 100), Position(300, 100)); // vertical

    network.setDrawColor(Color::Magenta);
    network.drawCircle(Position(400, 400), 150);

    Rectangle bound(100, 200, Position(300, 300));
    network.setDrawColor(Color::Olive);
    network.drawEllipse(bound);
    network.drawRectangle(bound);


    network.setDrawColor(Color::Purple);
    network.fillCircle(Position(200, 150), 75);

    bound = Rectangle(100, 200, Position(400, 200));
    network.setDrawColor(Color::Yellow);
    network.fillEllipse(bound);

    dest.blitScaled(canvas);
    dest.blit(network);
    window.update();
    idlewait(nullptr, window);
    return 0;
}
