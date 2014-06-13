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
Surface::blit(const Surface& src)
{
    if (SDL_BlitSurface(src.ptr, NULL, ptr, NULL) != 0) {
        throw BlitFailure();
    }
}


void
Surface::blitScaled(const Surface& src)
{
    if (SDL_BlitScaled(src.ptr, nullptr, ptr, nullptr) != 0) {
        throw BlitFailure();
    }
}

void
Surface::blit(const Position& destpos, const Surface& src,
              const Rectangular& srcrect)
{
    Rectangular dest = Rectangular(0, 0, destpos);
    if (SDL_BlitSurface(src.ptr, srcrect.toSdlRect(), ptr, dest.toSdlRect()) != 0) {
        throw BlitFailure();
    }
}

void
Surface::blitScaled(const Rectangular& destrect, const Surface& src,
              const Rectangular& srcrect)
{
    if (SDL_BlitScaled(src.ptr, srcrect.toSdlRect(), ptr, destrect.toSdlRect()) != 0) {
        throw BlitFailure();
    }
}

namespace event {

    void EventHandler::initptr()
    {
        if (!ptr) {
            ptr = std::unique_ptr<SDL_Event>(new SDL_Event());
        }
    }

    bool poll(EventHandler &eh)
    {
        eh.initptr();
        return !!SDL_PollEvent(eh.ptr.get());
    }

    void wait(EventHandler &eh)
    {
        eh.initptr();
        SDL_WaitEvent(eh.ptr.get());
    }

} // end namespace event
} //end namespace sdlpp
