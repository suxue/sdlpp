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

    auto path  = string(SDLPP_DEMO_DATA_DIR) + "hello.jpg";
    cout << path << endl;
    auto src = Surface::loadIMG(path);
    int height = src.getHeight();
    int width = src.getWidth();


    Handler sdl = Initializer().acquire();
    auto wm = WindowMode();
    auto window = sdl.createWindow("sdlpp_demo", Rectangular(width, height),
            wm.opengl());
    auto dest = window.getSurface();
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
