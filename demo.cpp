#include "sdlpp.hpp"
#include "sdlpp_main.hpp"
using namespace sdlpp;


int
main
(int argc, char *argv[])
{
    if (argc != 2) return 1;
    Initializer<> sdl;
    auto wm = WindowMode{};
    auto window = sdl.createWindow("sdlpp_demo", Rectangular{800, 600},
            wm.opengl());
    auto dest = window.getSurface();
    auto src = Surface::loadBMP(argv[1]);
    dest.blit(src);
    window.update();
    os::delay(2000);
}
