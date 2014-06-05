#ifndef SDLPP_HPP
#define SDLPP_HPP

#include "sdlpp_csdl.hpp"
#include "../util/list.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

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
    };

    struct WindowPosition {
        int x;
        int y;
        WindowPosition(int xi = SDL_WINDOWPOS_UNDEFINED,
                       int yi = SDL_WINDOWPOS_UNDEFINED) : x{xi}, y{yi} {}
        WindowPosition& setCentered(bool setx, bool sety) {
            if (setx) x = SDL_WINDOWPOS_CENTERED;
            if (sety) y = SDL_WINDOWPOS_CENTERED;
            return *this;
        }
    };

    struct WindowShape {
        int w;
        int h;
    };

    class Handler;

    template<typename T>
    class PointerHolder {
    public:
        PointerHolder(const PointerHolder<T>& o) = delete;
        ~PointerHolder() {}
        PointerHolder<T>& operator=(const PointerHolder<T>& o) = delete;
        PointerHolder<T>& operator=(PointerHolder<T>&& o) = delete;
    protected:
        PointerHolder(T* p) : ptr{p} {}
        PointerHolder(PointerHolder<T>&& o) {
            ptr = o.ptr;
        }
        T* ptr;
    };

    class Window : public PointerHolder<SDL_Window> {
        friend Handler;
        Window(SDL_Window* p) : PointerHolder{p} {};
    public:
        Window(Window&& w) : PointerHolder{(PointerHolder&&)w} {}
        ~Window();
    };

    class Handler {
    protected:
        Handler() {};
    public:
        Window createWindow(const std::string& title,
                                 const WindowPosition& pos,
                                 const WindowShape& shape);
    };

    template<int v>
    class Initializer_Base : public Handler {
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

    template<>
    class Initializer<> : public Initializer_Base<0> {
        public: Initializer()  {};
    };
}

#endif
