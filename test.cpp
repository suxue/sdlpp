#include "sdlpp.hpp"
#define BOOST_TEST_MODULE SdlppTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( init )
{
    using namespace sdlpp;
    typedef Initializer<def::Video, def::Audio> VAInit;
    BOOST_CHECK_EQUAL(VAInit::value, SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}
