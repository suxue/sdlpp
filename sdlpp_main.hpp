#ifndef SDLPP_MAIN
#define SDLPP_MAIN

#include "SDL_main.h"
#if defined(__MINGW32__)
#define main SDL_main
#endif

#endif
