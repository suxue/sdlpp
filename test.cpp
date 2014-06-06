#include "sdlpp.hpp"
#include "sdlpp_main.hpp"
#define BOOST_TEST_MODULE SdlppTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( sdlpp_initializer )
{
    using namespace sdlpp;
    typedef Initializer<def::Video, def::Audio> InitType1;
    BOOST_CHECK_EQUAL(InitType1::value, SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    { InitType1 init{}; }

    typedef Initializer<def::Timer> InitType2;
    BOOST_CHECK_EQUAL(InitType2::value, SDL_INIT_TIMER );
    InitType2 sdl{};
    Window w = sdl.createWindow("hello", Rectangular{200, 100});
}
