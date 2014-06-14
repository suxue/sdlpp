#include "common.hpp"
#include <iostream>

using namespace sdlpp;
using namespace sdlpp::event;
using namespace std;

bool maxmin = false;

void idlewait(Renderer& renderer, Window& window)
{
    EventData e;
    while (true) {
        event::wait(e);
        if (e.getType() == EventType::Quit) {
            break;
        } else if (e.getType() == EventType::Window) {
            auto p = e.acquire<EventType::Window>();
            switch (p.getID()) {
                case SDL_WINDOWEVENT_EXPOSED:
                    renderer.present();
                    if (maxmin) {
                        maxmin = true;
                        window.restore();
                    }
                    break;
                case SDL_WINDOWEVENT_MAXIMIZED:
                case SDL_WINDOWEVENT_MINIMIZED:
                    maxmin = true;
                    break;
                default:
                    break;
            }
        }
    }
}
