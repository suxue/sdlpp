#include "sdlpp.hpp"
#include <iostream>
#include <functional>
namespace sdlpp {

Window
Handler::createWindow(const std::string& title,
                             const WindowPosition& pos,
                             const WindowShape& shape)
{
    return Window{
        SDL_CreateWindow(title.c_str(), pos.x, pos.y, shape.w, shape.h,0)};
}

Window::~Window()
{
    SDL_DestroyWindow(ptr);
}

} //end namespace sdlpp
