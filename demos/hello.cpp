#include "sdlpp.hpp"
#include "config.h"
#include <string>
#include <iostream>
#define str(s) #s
using namespace sdlpp;


int
main
(int argc, char *argv[])
{
    Handler sdl = Initializer().acquire();
    auto wm = WindowMode();
    auto window = sdl.createWindow("sdlpp_demo", Rectangular(800, 600),
            wm.opengl());
    auto dest = window.getSurface();
    auto path  = std::string(SDLPP_DEMO_DATA_DIR) + "hello.bmp";
    std::cout << path << std::endl;
    auto src = Surface::loadBMP(path);
    auto srcopt = src.convert(dest.format());
    dest.blitScaled(srcopt);
    window.update();
    event::EventHandler e;
    while (true) {
        event::wait(e);
        if (e.getType() == event::EventType::Quit) {
            break;
        }
    }
    return 0;
}
