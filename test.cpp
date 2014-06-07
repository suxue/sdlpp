#define SDL_MAIN_HANDLED 1
#include "sdlpp.hpp"
#define BOOST_TEST_MODULE SdlppTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( sdlpp_initializer )
{
    using namespace sdlpp;
    Initializer i1 = Initializer().video().audio();
    BOOST_CHECK_EQUAL(i1.value, SDL_INIT_AUDIO | SDL_INIT_VIDEO);
}

int main()
{}
