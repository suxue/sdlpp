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

    if (SDL_BlitSurface(src.ptr,
            srcrect == nullptr ? nullptr : srcrect->toSdlRect(),
            ptr, destpos == nullptr ? nullptr : &tmp) != 0) {
        throw BlitFailure();
    }
}

void
Surface::blitScaled(const Surface& src, const Rectangular* srcrect,
                    const Rectangular* destrect)
{
    if (SDL_BlitScaled(
                src.ptr, srcrect == nullptr ? nullptr : srcrect->toSdlRect(),
                ptr, destrect == nullptr ? nullptr : destrect->toSdlRect())
            != 0) {
        throw BlitFailure();
    }
}

void
Renderer::copy(Texture& texture, const Rectangular* src, const Rectangular* dest)
{
    if (SDL_RenderCopy(ptr, texture.get(),
                src == nullptr ? nullptr : src->toSdlRect(),
                dest == nullptr ? nullptr : dest->toSdlRect())) {
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

    EventHandler EventData::dump()
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
