#include "sdlpp.hpp"
#include "config.h"
#include <iostream>

using namespace sdlpp;
using namespace std;
using namespace sdlpp::event;

int main (int argc, char *argv[])
{
    auto path  = string(SDLPP_DEMO_DATA_DIR) + "background.png";
    auto backgroundSuf = Surface::loadIMG(path);
    int height = backgroundSuf.getHeight();
    int width = backgroundSuf.getWidth();

    auto sdl = Initializer().acquire();
    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangular(width, height),
                                    WindowMode().opengl());
    auto renderer = window.getRenderer();

    Texture background(renderer, backgroundSuf);
    renderer.copy(background);

    Surface fooSuf = Surface::loadIMG(string(SDLPP_DEMO_DATA_DIR)+"foo.png");
    Color cyan = Color(0, 0xff, 0xff);
    fooSuf.setColorKey(cyan);
    Texture foo(renderer, fooSuf);
    Rectangular destRect(fooSuf.getWidth(), fooSuf.getHeight(),
            Position(width/2, height/2));
    renderer.copy(foo, nullptr, &destRect);

    renderer.present();
#   include"ewait.cpp"
    return 0;
}
