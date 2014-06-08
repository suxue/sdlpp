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
    dest.blit(src);
    window.update();
    event::Event e;
    while (true) {
        if (e.poll()) {
            if (e.getType() == event::Quit) break;
            else if (e.getType() == event::KeyDown) {
                const event::KeyEvent* ke = (event::KeyEvent*)&e;
                switch (ke->code()) {
                    case SDLK_UP:
                        std::cout << "up" << std::endl;
                        break;
                    case SDLK_DOWN:
                        std::cout << "down" << std::endl;
                        break;
                    case SDLK_LEFT:
                        std::cout << "left" << std::endl;
                        break;
                    case SDLK_RIGHT:
                        std::cout << "right" << std::endl;
                        break;
                }
            }
        }
    }
    return 0;
}
