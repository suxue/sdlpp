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
#include <string>
#include <iostream>

using namespace sdlpp;
using namespace std;
using namespace sdlpp::event;


int
main
(int argc, char *argv[])
{
    auto path  = string(SDLPP_DEMO_DATA_DIR) + "hello.jpg";
    cout << path << endl;
    auto src = Surface::loadIMG(path);
    int height = src.getHeight();
    int width = src.getWidth();


    Handler sdl = Initializer().acquire();
    auto wm = WindowMode();
    auto window = sdl.createWindow("sdlpp_demo", Rectangular(width, height),
            wm.opengl());
    auto dest = window.getSurface();
    auto srcopt = src.convert(dest.getFormat());
    auto renderer = window.getRenderer();
    Texture texture(renderer, srcopt);
    renderer.copy(texture);
    renderer.present();

    // dest.blitScaled(srcopt);
    // window.update();
    idlewait(renderer, window);
    return 0;
}
