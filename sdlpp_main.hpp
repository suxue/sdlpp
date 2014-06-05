#ifndef SDLPP_MAIN
#define SDLPP_MAIN

#ifdef __MINGW32__
extern "C" {
    extern int main(int argc, char *argv[]);
    int SDL_main(int argc, char *argv[])
    {
        return main(argc, argv);
    }
}
#endif

#endif
