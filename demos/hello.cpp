#include "common.hpp"
#include <string>
#include <iostream>

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
    idlewait(renderer, window);
    return 0;
}
