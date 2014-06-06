#include "sdlpp.hpp"
using namespace sdlpp;

int main(int argc, const char *argv[])
{
    if (argc != 2) return 1;
    Initializer<> sdl;
    Window window = sdl.createWindow("sdlpp_demo", WindowPosition{},
            WindowShape{640, 480});
    Surface dest = window.getSurface();
    Surface src = Surface::loadBMP(argv[1]);
    dest.blit(src);
    window.update();
    os::delay(2000);
}
