#include "sdlpp.hpp"
using namespace sdlpp;

int main()
{
    Initializer<> sdl;
    Window w = sdl.createWindow("hello", WindowPosition{},
            WindowShape{200, 100});
}
