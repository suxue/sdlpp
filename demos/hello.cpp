#include "sdlpp.hpp"
#include "config.h"
#include <string>
#include <iostream>
#define str(s) #s

using namespace sdlpp;
using namespace std;
using namespace sdlpp::event;


int
main
(int argc, char *argv[])
{
    Handler sdl = Initializer().acquire();
    auto wm = WindowMode();
    auto window = sdl.createWindow("sdlpp_demo", Rectangular(800, 600),
            wm.opengl());
    auto dest = window.getSurface();
    auto path  = string(SDLPP_DEMO_DATA_DIR) + "hello.bmp";
    cout << path << endl;
    auto src = Surface::loadBMP(path);
    auto srcopt = src.convert(dest.getFormat());
    auto renderer = window.getRenderer();
    Texture texture(renderer, srcopt);
    renderer.copy(texture);
    renderer.present();

    // dest.blitScaled(srcopt);
    // window.update();
    EventData e;
    while (true) {
        event::wait(e);
        if (e.getType() == EventType::Quit) {
            break;
        } else if (e.getType() == EventType::Window) {
            auto p = e.acquire<EventType::Window>();
            if (p.getID() == SDL_WINDOWEVENT_EXPOSED) { // redraw
                renderer.present();
            }
        }
    }
    return 0;
}
