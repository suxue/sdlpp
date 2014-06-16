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
            Rectangular(800, 600));
    Surface dest(window.getSurface());
    Surface src(Surface::loadIMG(string(SDLPP_DEMO_DATA_DIR) + "arrow.png"));

    PixelFormat format = SDL_PIXELFORMAT_ARGB8888;
    Bpp4Surface canvas(200, 100, format);
    PixelValue value = Color(0, 0x1f, 0xff).mapRGB(canvas.getFormat());
    cout << Color(value, canvas.getFormat()) << endl;
    cout << value << endl;
    for (auto j = 0; j < 100; j++) {
        for (auto i = 0; i < 200; i++) {
            //canvas.setPixel(i, j, value);
            canvas[i][j] = value;
        }
    }

    dest.blitScaled(src);
    dest.blit(canvas);
    window.update();
    idlewait(nullptr, window);
    return 0;
}
