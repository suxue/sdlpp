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
    lua["color"] = lua.newCallable([&canvas](State& st,
                Number r, Number g, Number b) {
        return st.newUserData<Color>(r, g, b);
    });

    lua["set"] = lua.newCallable([&canvas](State& st, UserData&& color)  {
        canvas.setDrawColor(* color.to<Color>());
        return Nil();
    });

    lua["line"] = lua.newCallable([&canvas](State& st,
                Table&& from, Table&& to) {
        canvas.drawLine(
                Position( Number(from[1]), Number(from[2]) ),
                Position( Number(to[1]), Number(to[2]) ));
        return Nil();
    });

    lua["fillRect"] = lua.newCallable([&canvas](State& st,
                Table&& center, Table&& geo)  {
        Number x = center[1];
        Number y = center[2];
        Number w = geo[1];
        Number h = geo[2];
        canvas.fillRectangle(Rectangle(w, h, Position(x, y)));
        return Nil();
    });

    // (center{x, y}, radius(x, y))
    lua["fillEllipse"] = lua.newCallable([&canvas](State& st,
                Table&& center, Table&& radius) {
        Number x = center[1];
        Number y = center[2];
        Number rx = radius[1];
        Number ry = radius[2];
        canvas.fillEllipse(Position(x, y), Position(rx, ry));
        return Nil();
    });

    // (center{x, y}, radius)
    lua["drawCircle"] = lua.newCallable([&canvas](State& st,
                Table&& center, Number radius) {
        Number x = center[1];
        Number y = center[2];
        canvas.drawCircle(Position(x, y), radius);
        return Nil();
    });

    lua["fillCircle"] = lua.newCallable([&canvas](State& st,
                Table&& center, Number radius) {
        Number x = center[1];
        Number y = center[2];
        canvas.fillCircle(Position(x, y), radius);
        return Nil();
    });

    string filename;
    if (argc == 2) {
        filename = argv[1];
    } else {
        filename = string(SDLPP_DEMO_DATA_DIR) + "luasdl.lua";
    }
    Closure cl = lua.newFile(filename);
    cl();

    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangle(SCREEN_WIDTH, SCREEN_HEIGHT),
                                    WindowMode());
    Surface dest(window.getSurface());
    dest.blit(canvas);
    window.update();
    idlewait(nullptr, window);
}
