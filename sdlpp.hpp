#ifndef SDLPP_HPP
#define SDLPP_HPP

#include "sdlpp_csdl.hpp"
#include "../util/list.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdint>

namespace sdlpp {

    typedef std::uint32_t Timestamp;
    typedef std::uint32_t WindowID;

    namespace os {
        inline void delay(std::uint32_t ms) { SDL_Delay(ms); }
    }

    namespace error {
        std::string getmsg();
        struct RuntimeError : public std::runtime_error {
            RuntimeError(const std::string& m) : std::runtime_error(m) {}
            RuntimeError() : std::runtime_error(getmsg()) {}
        };
    }

    namespace event {
        enum EventType {
            Quit = SDL_QUIT,
            AppTerminating = SDL_APP_TERMINATING,
            AppLowMemory = SDL_APP_LOWMEMORY,
            AppWillEnterBackground = SDL_APP_WILLENTERBACKGROUND,
            AppDidEnterBackground = SDL_APP_DIDENTERBACKGROUND,
            AppWillEnterForeground = SDL_APP_WILLENTERFOREGROUND,
            AppDidEnterForeground = SDL_APP_DIDENTERFOREGROUND,
            Window = SDL_WINDOWEVENT,
            System = SDL_SYSWMEVENT,
            KeyDown = SDL_KEYDOWN,
            KeyUp = SDL_KEYUP,
            TextEditing = SDL_TEXTEDITING,
            TextInput = SDL_TEXTINPUT,
            MouseMotion = SDL_MOUSEMOTION,
            MouseButtonDown = SDL_MOUSEBUTTONDOWN,
            MouseButtonUp = SDL_MOUSEBUTTONUP,
            MouseWheel = SDL_MOUSEWHEEL,
            // TODO Joystick events
            // TODO controller events
            // TODO Touch events
            // TODO Gesture events
            // TODO Clipboard events
            User = SDL_USEREVENT,
        };

        class Event {
        public:
            EventType getType() const { return (EventType)data.type; }
            bool poll() { return !!SDL_PollEvent(&data); }
            bool wait() { return !!SDL_WaitEvent(&data); }
        protected:
            SDL_Event data;
        };

        class WindowEvent : public Event {
            
        };

        class KeyEvent : public Event {
        public:
            typedef SDL_Scancode Scancode;
            typedef SDL_Keycode Keycode;
            typedef SDL_Keymod Keymod;
            Timestamp timestamp() const { return data.key.timestamp;}
            WindowID windowID() const { return data.key.windowID; }
            bool released() const { return data.key.state == SDL_RELEASED; }
            bool pressed() const { return data.key.state == SDL_PRESSED; }
            bool repeat() const{ return data.key.repeat == 0; }
            Scancode scancode() const {return data.key.keysym.scancode; }
            Keymod   mod() const { return (SDL_Keymod)data.key.keysym.mod; }
            Keycode  code() const { return data.key.keysym.sym; }
        };

    }

    struct WindowPosition {
        int x;
        int y;
        WindowPosition(int xi = SDL_WINDOWPOS_UNDEFINED,
                       int yi = SDL_WINDOWPOS_UNDEFINED) : x(xi), y(yi) {}
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
        WindowMode(std::uint32_t v) : value(v) {}
    public:
        WindowMode() : value(0) {}
        WindowMode fullscreen() {
            return WindowMode(value | SDL_WINDOW_FULLSCREEN);
        }
        WindowMode fulldesktop() {
            return WindowMode(value | SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        WindowMode opengl() {
            return WindowMode(value | SDL_WINDOW_OPENGL);
        }
        WindowMode hide() {
            return WindowMode(value | SDL_WINDOW_HIDDEN);
        }
        WindowMode resizable() {
            return WindowMode(value | SDL_WINDOW_RESIZABLE);
        }
        WindowMode grab() {
            return WindowMode(value | SDL_WINDOW_INPUT_GRABBED);
        }
        WindowMode minimize() {
            return WindowMode(value | SDL_WINDOW_MINIMIZED);
        }
        WindowMode maximize() {
            return WindowMode(value | SDL_WINDOW_MAXIMIZED);
        }
        WindowMode borderless() {
            return WindowMode(value | SDL_WINDOW_BORDERLESS);
        }
    };

    struct Rectangular {
        Rectangular(int wi, int hi,
                    const WindowPosition& pos = WindowPosition())
            : x(pos.x), y(pos.y), w(wi), h(hi) {}
        int x;
        int y;
        int w;
        int h;
    };


    template<typename T>
    class PointerHolder {
    public:
        ~PointerHolder() {}

    protected:
        PointerHolder(T* p) : ptr(p) {}
        PointerHolder(PointerHolder<T>&& o) {
            ptr = o.ptr;
        }
        T* ptr;
    };

    class Window;
    class Surface_Base : public PointerHolder<SDL_Surface> {
        friend Window;
    protected:
        Surface_Base(Surface_Base&& s): PointerHolder((PointerHolder&&)s) {}
        Surface_Base(SDL_Surface *p): PointerHolder(p) {}
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
        Surface(SDL_Surface *p) : Surface_Base(p) {}
        ~Surface() { SDL_FreeSurface(ptr); }
        Surface(Surface&& s) : Surface_Base(s.getBase()) {}
        static Surface loadBMP(const std::string& path);
    };

    class InternalSurface : public Surface_Base {
    public:
        InternalSurface(SDL_Surface *p) : Surface_Base(p) {}
        InternalSurface(Surface&& s) : Surface_Base(s.getBase()) {}
    };


    class Window : public PointerHolder<SDL_Window> {
        friend Handler;
        Window(SDL_Window* p) : PointerHolder(p) {};
    public:
        struct CreateFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        Window(Window&& w) : PointerHolder((PointerHolder&&)w) {}
        ~Window();
        void update() { SDL_UpdateWindowSurface(ptr); };
        InternalSurface getSurface();
    };

    struct Initializer;
    class Handler {
        friend Initializer;
    private:
        Handler() {};
    public:
        ~Handler() { SDL_Quit(); }
        Window createWindow(const std::string& title,
                            const Rectangular& rect,
                            WindowMode wm = WindowMode());
    };

    struct Initializer {
        struct InitFailure : error::RuntimeError { using error::RuntimeError::RuntimeError;};
        const std::uint32_t value;
        Initializer() : value(0) {}
        Initializer timer() { return Initializer(value | SDL_INIT_TIMER);}
        Initializer audio() { return Initializer(value | SDL_INIT_AUDIO);}
        Initializer video() { return Initializer(value | SDL_INIT_VIDEO);}
        Initializer joyStick() { return Initializer(value | SDL_INIT_JOYSTICK);}
        Initializer gameControoler() { return Initializer(value | SDL_INIT_GAMECONTROLLER);}
        Initializer events() { return Initializer(value | SDL_INIT_EVENTS);}
        Initializer everything() { return Initializer(value | SDL_INIT_EVERYTHING);}
        Initializer noParachute() { return Initializer(value | SDL_INIT_NOPARACHUTE);}
        Handler acquire() {
            if (SDL_Init(value) != 0) throw InitFailure();
            return Handler();
        }
    private:
        Initializer(std::uint32_t v) : value(v) {}
    };
}

#endif
