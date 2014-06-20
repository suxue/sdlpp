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

#include "luamm.hpp"
#include "common.hpp"
#include <cstdint>
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace sdlpp;
using namespace std;
using namespace sdlpp::event;
using namespace luamm;

int main (int argc, char *argv[])
{
    auto sdl = Initializer().acquire();

    // setup canvas
    PixelFormat format = SDL_PIXELFORMAT_ARGB8888;
    Bpp4Surface canvas(SCREEN_WIDTH, SCREEN_HEIGHT, format);
    canvas.setDrawColor(Color::White);
    canvas.clear();

    // setup lua
    NewState lua;
    lua.openlibs();
    lua["color"] = [&canvas](State& st) -> int {
        // accept 3 value, rgb, return the Color object
        Number r = st[-3];
        Number g = st[-2];
        Number b = st[-1];
        st.pop(3);
        Color c(r, g, b);
        st.pushUserData(c);
        return 1;
    };

    lua["set"] = [&canvas](State& st) -> int {
        // accept 3 value, rgb, return the Color object
        UserData ud = st[-1];
        auto c = ud.get<Color>();
        canvas.setDrawColor(*c);
        st.pop();
        return 0;
    };

    lua["line"] = [&canvas](State& st) -> int {
        Table s = st[-2];
        Table e = st[-1];
        Number x1 = s[1], y1 = s[2];
        Number x2 = e[1], y2 = e[2];
        st.pop(2);
        canvas.drawLine(Position(x1, y1), Position(x2, y2));
        return 0;
    };

    lua["fillRect"] = [&canvas](State& st) -> int {
        Table s = st[-2];
        Table e = st[-1];
        Number x1 = s[1], y1 = s[2];
        Number x2 = e[1], y2 = e[2];
        st.pop(2);
        canvas.fillRectangle(Rectangle(x2, y2, Position(x1, y1)));
        return 0;
    };

    lua.loadstring("set(color(255, 0, 255)); line({1, 1}, {100, 100})");
    if (lua.pcall(0, 0) != LUA_OK) {
        const char *msg = lua[1];
        fprintf(stderr, "%s\n", msg);
        return 1;
    }

    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangle(SCREEN_WIDTH, SCREEN_HEIGHT),
                                    WindowMode());
    Surface dest(window.getSurface());
    dest.blit(canvas);
    window.update();
    idlewait(nullptr, window);
}
