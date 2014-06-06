#include "sdlpp.hpp"
#include <iostream>
#include <functional>
namespace sdlpp {

Window
Handler::createWindow(const std::string& title,
                             const WindowPosition& pos,
                             const WindowShape& shape)
{
    SDL_Window *wp = SDL_CreateWindow(title.c_str(), pos.x, pos.y, shape.w, shape.h,0);
    if (!wp) {
        throw Window::CreateFailure{error::getmsg()};
    }
    return Window{wp};
}

Window::~Window()
{
    SDL_DestroyWindow(ptr);
}

std::string error::getmsg()
{
    const char *msg = SDL_GetError();
    std::string smsg{msg};
    SDL_ClearError();
    return smsg;
}

Surface
Window::getSurface()
{
    return Surface{SDL_GetWindowSurface(ptr)};
}

struct SurfaceCleaner {
    void operator()(SDL_Surface* s) {
        SDL_FreeSurface(s);
    }
};

Surface
Surface::loadBMP(const std::string& path)
{
    auto p = SDL_LoadBMP(path.c_str());
    if (!p) {
        throw LoadFailure{error::getmsg()};
    }
    return Surface{p, SurfaceCleaner{}};
}

void
Surface::blit(const Surface& src)
{
    if (SDL_BlitSurface(src.ptr, NULL, ptr, NULL) != 0) {
        throw BlitFailure{error::getmsg()};
    }
}

} //end namespace sdlpp
