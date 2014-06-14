#include "common.hpp"
#include <iostream>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace sdlpp;
using namespace std;
using namespace sdlpp::event;


int main (int argc, char *argv[])
{
    auto sdl = Initializer().acquire();
    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangular(SCREEN_WIDTH, SCREEN_HEIGHT),
                                    WindowMode().opengl());
    auto renderer = window.getRenderer();
    Color white(0xff, 0xff, 0xff);
    renderer.setDrawColor(white);
    renderer.clear();

    Color red(0xff, 0x00, 0x00);
    auto fillRect = Rectangular(SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                Position(SCREEN_WIDTH/4, SCREEN_HEIGHT/4));
    renderer.setDrawColor(red);
    renderer.fillRect(&fillRect);

    Color blue(0x00, 0x00, 0xFF);
    renderer.setDrawColor(blue);
    renderer.drawLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

    Color green(0x00, 0xff, 0x00);
    renderer.setDrawColor(green);
    for (auto i = 0; i < SCREEN_HEIGHT; i += 4) {
        renderer.drawPoint(SCREEN_WIDTH / 2, i);
    }
    idlewait(renderer);
    return 0;
}
