#ifndef SDLPP_HPP
#define SDLPP_HPP

#include "sdlpp_csdl.hpp"
#include "../util/list.hpp"
#include <stdexcept>
#include <string>

namespace sdlpp {

    namespace def {
        enum Subsystem {
            Video,
            Audio,
            Timer,
            Joystick,
            Haptic,
            GameController,
            Events,
            Everything,
            NoParachute
        };

        using util::list::Prepend;
        using util::list::Create;
#       define Prepend util::list::Prepend
#       define Create util::list::Create
        typedef Prepend<Video, SDL_INIT_VIDEO, Prepend<
                        Audio, SDL_INIT_AUDIO, Prepend<
                        Timer, SDL_INIT_TIMER, Prepend<
                        Joystick, SDL_INIT_JOYSTICK, Prepend<
                        Haptic, SDL_INIT_HAPTIC, Prepend<
                        GameController, SDL_INIT_GAMECONTROLLER, Prepend<
                        Events, SDL_INIT_EVENTS, Prepend<
                        Everything, SDL_INIT_EVERYTHING, Create<
                        NoParachute, SDL_INIT_NOPARACHUTE>>>>>>>>>
                SubsystemMap;
#       undef Prepend
#       undef Create
    };

    template<int v>
    class Initializer_Base {
    public:
        enum { value = v };
        struct InitError : public std::runtime_error {
            InitError(const std::string& msg) : std::runtime_error(msg) {};
        };
        Initializer_Base() {
            if (SDL_Init(value) < 0) {
                std::string msg = std::string(SDL_GetError());
                SDL_ClearError();
                throw InitError(msg);
            }
        }
        ~Initializer_Base() { SDL_Quit(); }
    };

    template<def::Subsystem... s>
    class Initializer;

    template<def::Subsystem s, def::Subsystem... sl>
    class Initializer<s, sl...> : public Initializer_Base<
            util::list::Find<s, def::SubsystemMap>::value
                | Initializer<sl...>::value> {
                public: Initializer() {};
            };

    template<def::Subsystem s>
    class Initializer<s> : public
        Initializer_Base<util::list::Find<s, def::SubsystemMap>::value> {
            public: Initializer()  {};
    };
}

#endif
