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
using namespace std;
using namespace sdlpp::event;

int main (int argc, char *argv[])
{
    auto path  = string(SDLPP_DEMO_DATA_DIR) + "background.png";
    auto backgroundSuf = Surface::loadIMG(path);
    int height = backgroundSuf.height();
    int width = backgroundSuf.width();

    auto sdl = Initializer().acquire();
    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangular(width, height),
                                    WindowMode().opengl());
    auto renderer = window.getRenderer();

    Texture background(renderer, backgroundSuf);
    renderer.copy(background);

    Surface fooSuf = Surface::loadIMG(string(SDLPP_DEMO_DATA_DIR)+"foo.png");
    Color cyan = Color(0, 0xff, 0xff);
    fooSuf.setColorKey(cyan);
    Texture foo(renderer, fooSuf);
    Rectangular destRect(fooSuf.width(), fooSuf.height(),
            Position(width/2, height/2));
    renderer.copy(foo, nullptr, &destRect);

    renderer.present();
    idlewait(renderer, window);
    return 0;
}
