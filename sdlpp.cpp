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

#include "sdlpp.hpp"
#include <iostream>
#include <functional>
#include <cstring>
namespace sdlpp {

Window
Handler::createWindow(const std::string& title,
                      const Rectangular& rect,
                      WindowMode wm)
{
    SDL_Window *wp = SDL_CreateWindow(title.c_str(),
                         rect.x, rect.y, rect.w, rect.h, wm.value);
    if (!wp) {
        throw Window::CreateFailure();
    }
    return Window(wp);
}

Window::~Window()
{
    SDL_DestroyWindow(ptr);
}

std::string error::getmsg()
{
    const char *msg = SDL_GetError();
    std::string smsg(msg);
    SDL_ClearError();
    return smsg;
}

Surface
Surface::loadBMP(const std::string& path)
{
    auto p = SDL_LoadBMP(path.c_str());
    if (!p) {
        throw LoadFailure();
    }
    return Surface(p);
}

Surface
Surface::loadIMG(const std::string& path)
{
    auto p = IMG_Load(path.c_str());
    if (!p) {
        throw LoadFailure();
    }
    return Surface(p);
}

void
Surface::blit(const Surface& src,
              const Rectangular* srcrect,
        const Position* destpos)
{
    SDL_Rect tmp;
    if (destpos) {
        tmp.x = destpos->x;
        tmp.y = destpos->y;
    }

    if (SDL_BlitSurface(src.ptr, srcrect, ptr,
                destpos == nullptr ? nullptr : &tmp) != 0) {
        throw BlitFailure();
    }
}

void
Surface::blitScaled(const Surface& src, Rectangular* srcrect,
                    Rectangular* destrect)
{
    if (SDL_BlitScaled( src.ptr, srcrect, ptr, destrect) != 0) {
        throw BlitFailure();
    }
}

void
Renderer::copy(Texture& texture, const Rectangular* src, const Rectangular* dest)
{
    if (SDL_RenderCopy(ptr, texture.get(), src, dest)) {
        throw error::RuntimeError();
    }
}

namespace event {

    void EventData::initptr()
    {
        if (!ptr) {
            ptr = std::unique_ptr<SDL_Event>(new SDL_Event());
        }
    }

    bool poll(EventData &eh)
    {
        eh.initptr();
        return !!SDL_PollEvent(eh.ptr.get());
    }

    void wait(EventData &eh)
    {
        eh.initptr();
        SDL_WaitEvent(eh.ptr.get());
    }

    EventHandler EventData::slice()
    {
        size_t size;
        switch (ptr->type) {
            case SDL_WINDOWEVENT:
                size = sizeof(SDL_WindowEvent);
                break;
            case SDL_QUIT:
                size = sizeof(SDL_CommonEvent);
                break;
            default:
                size = sizeof(*ptr.get());
                break;
        }
        char *p = new char[size];
        std::memmove((void*)p, ptr.get(), size);
        return create((SDL_Event*)p);
    }

} // end namespace event
} //end namespace sdlpp
