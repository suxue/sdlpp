#include "sdlpp.hpp"
#include "config.h"
#include <string>

using namespace sdlpp;
using namespace sdlpp::event;
using namespace std;

static const int screen_width = 800;
static const int screen_height = 600;

int main(int argc, char *argv[])
{
    auto sdl = Initializer().acquire();
    auto window = sdl.createWindow("sdlpp_demo",
                                    Rectangular(screen_width, screen_height),
                                    WindowMode().opengl());

    auto renderer = window.getRenderer();
    auto path  = string(SDLPP_DEMO_DATA_DIR) + "sprites.png";
    Surface spriteSuf = Surface::loadIMG(path);
    Color cyan(0, 0xff, 0xff);
    spriteSuf.setColorKey(cyan);

    Texture sprite(renderer, spriteSuf);
    auto height = spriteSuf.getHeight();
    auto width = spriteSuf.getWidth();
    auto sh = height/2;
    auto sw = width/2;

    Color white = Color(0xff, 0xff, 0xff);
    renderer.setDrawColor(white);
    renderer.clear();

    int clips[][2][2] = {
        // {{source position}, {destination position}}
        { {0, 0}, {0, 0} }, // top-left
        { {sw, 0}, {screen_width-sw, 0} }, // top-right
        { {0, sh}, {0, screen_height - sh}}, // bottom-left
        { {sw, sh}, {screen_width - sw, screen_height - sh} }}; // bottom-right

    Rectangular srcrect(sw, sh);
    Rectangular destrect(sw, sh);
    for (unsigned i = 0; i < sizeof(clips) / sizeof(clips[0]); i++) {
        auto p = clips[i];
        srcrect.x = p[0][0];
        srcrect.y = p[0][1];
        destrect.x = p[1][0];
        destrect.y = p[1][1];
        renderer.copy(sprite, &srcrect, &destrect);
    }
    renderer.present();


#   include"ewait.cpp"
    return 0;
}
