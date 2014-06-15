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

using namespace sdlpp;
using namespace sdlpp::event;
using namespace std;

static const int screen_width = 800;
static const int screen_height = 600;

int main(int argc, char *argv[])
{
    auto sdl = Initializer().acquire();
    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangular(screen_width, screen_height),
                                    WindowMode().opengl());

    auto renderer = window.getRenderer();
    auto path  = string(SDLPP_DEMO_DATA_DIR) + "sprites.png";
    Surface spriteSuf = Surface::loadIMG(path);
    Color cyan(0, 0xff, 0xff);
    spriteSuf.setColorKey(cyan);

    Texture sprite(renderer, spriteSuf);
    auto height = spriteSuf.height();
    auto width = spriteSuf.width();
    auto sh = height/2;
    auto sw = width/2;

    Color white = Color(0xff, 0xff, 0xff);
    renderer.setDrawColor(white);
    renderer.clear();

    int clips[][2][2] = {
        // {{source position}, {destination position}}
        { {0, 0}, {0, 0} }, // top-left
        { {sw, 0}, {screen_width-sw, 0} }, // top-right
        { {0, sh}, {0, screen_height - sh}}, // bottom-left
        { {sw, sh}, {screen_width - sw, screen_height - sh} }}; // bottom-right

    Rectangular srcrect(sw, sh);
    Rectangular destrect(sw, sh);
    for (unsigned i = 0; i < sizeof(clips) / sizeof(clips[0]); i++) {
        auto p = clips[i];
        srcrect.x = p[0][0];
        srcrect.y = p[0][1];
        destrect.x = p[1][0];
        destrect.y = p[1][1];
        renderer.copy(sprite, &srcrect, &destrect);
    }
    renderer.present();

    idlewait(renderer, window);
    return 0;
}
