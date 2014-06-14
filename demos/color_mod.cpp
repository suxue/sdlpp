#include "common.hpp"
#include <iostream>

using namespace sdlpp;
using namespace std;

namespace {
    const int screen_width = 800;
    const int screen_height = 600;
    const int w = screen_width / 2;
    const int h = screen_height / 2;
}

int main(int argc, char *argv[])
{
    auto sdl = Initializer().video().events().acquire();
    auto window = sdl.createWindow("color_mod",
            Rectangular(screen_width, screen_height));
    Renderer renderer(window.getRenderer());

    Texture canvas(renderer.createTargetTexture(screen_width, screen_height));
    renderer.setTarget(&canvas);

    renderer.setDrawColor(Color(0xff, 0xff, 0xff));
    renderer.fillRect(Rectangular(w, h, Position(0, 0)));

    renderer.setDrawColor(Color(0xff, 0, 0));
    renderer.fillRect(Rectangular(w, h, Position(w, 0)));

    renderer.setDrawColor(Color(0, 0xff, 0));
    renderer.fillRect(Rectangular(w, h, Position(0, h)));

    renderer.setDrawColor(Color(0, 0, 0xff));
    renderer.fillRect(Rectangular(w, h, Position(w, h)));

    renderer.setTarget();

    // note, byte % 0xff = byte
    Color mod(0xff, 128, 0xff);
    canvas.setColorMod(mod);
    renderer.copy(canvas);
    renderer.present();

    Callback callback = [&](const event::EventHandler& e) {
        if (e.getType() == event::EventType::Keyboard) {
            bool mod_changed = true;
            auto p = e.acquire<event::EventType::Keyboard>();
            if (p.pressed()) {
                switch (p.sym()) {
                    case SDLK_q:
                        mod.red += 32; break;
                    case SDLK_w:
                        mod.green += 32; break;
                    case SDLK_e:
                        mod.blue += 32; break;
                    case SDLK_a:
                        mod.red -= 32; break;
                    case SDLK_s:
                        mod.green -= 32; break;
                    case SDLK_d:
                        mod.blue -= 32; break;
                    default:
                        mod_changed = false; break;
                }
            }
            if (mod_changed) {
                canvas.setColorMod(mod);
                renderer.copy(canvas);
                renderer.present();
            }
        }
    };

    idlewait(renderer, window, callback);
    return 0;
}
