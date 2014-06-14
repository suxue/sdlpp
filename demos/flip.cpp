#include "common.hpp"
#include <iostream>

using namespace sdlpp;
using namespace sdlpp::event;
using namespace std;

int main(int argc, char *argv[])
{
    auto sdl = Initializer().video().events().acquire();
    auto window = sdl.createWindow("flip and rotation",
            Rectangular(800, 600));
    Renderer renderer(window.getRenderer());
    Texture arrow(renderer, Surface::loadIMG(DEMO_PATH("arrow.png")));
    renderer.copy(arrow);
    renderer.present();

    idlewait(renderer, window);
    return 0;
}
