#ifndef SDLPP_HPP
#define SDLPP_HPP

#include "sdlpp_csdl.hpp"
#include "../util/list.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdint>

namespace sdlpp {

    namespace os {
        inline void delay(std::uint32_t ms) { SDL_Delay(ms); }
    }

    namespace error {
        std::string getmsg();
        struct RuntimeError : public std::runtime_error {
            using std::runtime_error::runtime_error;
            RuntimeError() : RuntimeError{getmsg()} {}
        };
    }

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

    class Handler;
    class WindowMode {
        friend Handler;
        std::uint32_t value;
        WindowMode(std::uint32_t v) : value{v} {}
    public:
        WindowMode() : value{0} {}
        WindowMode fullscreen() {
            return WindowMode{value | SDL_WINDOW_FULLSCREEN};
        }
        WindowMode fulldesktop() {
            return WindowMode{value | SDL_WINDOW_FULLSCREEN_DESKTOP};
        }
        WindowMode opengl() {
            return WindowMode{value | SDL_WINDOW_OPENGL};
        }
        WindowMode hide() {
            return WindowMode{value | SDL_WINDOW_HIDDEN};
        }
        WindowMode resizable() {
            return WindowMode{value | SDL_WINDOW_RESIZABLE};
        }
        WindowMode grab() {
            return WindowMode{value | SDL_WINDOW_INPUT_GRABBED};
        }
        WindowMode minimize() {
            return WindowMode{value | SDL_WINDOW_MINIMIZED};
        }
        WindowMode maximize() {
            return WindowMode{value | SDL_WINDOW_MAXIMIZED};
        }
        WindowMode borderless() {
            return WindowMode{value | SDL_WINDOW_BORDERLESS};
        }
    };

    struct Rectangular {
        Rectangular(int wi, int hi,
                    const WindowPosition& pos = WindowPosition{})
            : x{pos.x}, y{pos.y}, w{wi}, h{hi} {}
        int x;
        int y;
        int w;
        int h;
    };


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

    class Window;
    class Surface_Base : public PointerHolder<SDL_Surface> {
        friend Window;
    protected:
        Surface_Base(Surface_Base&& s): PointerHolder{(PointerHolder&&)s} {}
        Surface_Base(SDL_Surface *p): PointerHolder{p} {}
    public:
        struct LoadFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        struct BlitFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        void blit(const Surface_Base& src);
        Surface_Base&& getBase() { return std::move(*this);}
    };

    class Surface : public Surface_Base {
    public:
        Surface(SDL_Surface *p) : Surface_Base{p} {}
        ~Surface() { SDL_FreeSurface(ptr); }
        Surface(Surface&& s) : Surface_Base{s.getBase()} {}
        static Surface loadBMP(const std::string& path);
    };

    class InternalSurface : public Surface_Base {
    public:
        InternalSurface(SDL_Surface *p) : Surface_Base{p} {}
        InternalSurface(Surface&& s) : Surface_Base{s.getBase()} {}
    };


    class Window : public PointerHolder<SDL_Window> {
        friend Handler;
        Window(SDL_Window* p) : PointerHolder{p} {};
    public:
        struct CreateFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        Window(Window&& w) : PointerHolder{(PointerHolder&&)w} {}
        ~Window();
        void update() { SDL_UpdateWindowSurface(ptr); };
        InternalSurface getSurface();
    };

    class Handler {
    protected:
        Handler() {};
    public:
        Window createWindow(const std::string& title,
                            const Rectangular& rect,
                            WindowMode wm = WindowMode{});
    };

    template<int v>
    class Initializer_Base : public Handler {
    public:
        enum { value = v };
        struct InitError : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        Initializer_Base() {
            if (SDL_Init(value) < 0) {
                throw InitError{error::getmsg()};
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
