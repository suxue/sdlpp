#include "sdlpp.hpp"
#include <iostream>
#include <functional>
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

InternalSurface
Window::getSurface()
{
    return InternalSurface(SDL_GetWindowSurface(ptr));
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

void
Surface_Base::blit(const Surface_Base& src)
{
    if (SDL_BlitSurface(src.ptr, NULL, ptr, NULL) != 0) {
        throw BlitFailure();
    }
}

} //end namespace sdlpp
