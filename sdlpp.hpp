#ifndef SDLPP_HPP
#define SDLPP_HPP

#include "sdlpp_csdl.hpp"
#include "../util/list.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdint>
#include <boost/utility.hpp>
#include <memory>

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

        struct EventType {
            enum type {
                Window,
                Keyboard,
                TextEditing,
                MouseMotion,
                MouseButton,
                MouseWheel,
                Quit,
                User,
            };
        };

        template<typename T>
        class EventBase {
        protected:
            const T* ptr;
            typedef const T* PointType;
            EventBase(const T* p) : ptr(p) {}
        };

        template<EventType::type T>
        class Event;

        template<>
        class Event<EventType::Window> : protected EventBase<SDL_WindowEvent> {
            Event(PointType p) : EventBase(p) {}
        public:
            typedef Event type;
            static const type extract(const SDL_Event* e) {
                return Event(&e->window);
            }
        };

        class EventHandler;
        bool poll(EventHandler& eh);
        void wait(EventHandler& eh);

        class EventHandler {
            friend bool poll(EventHandler& eh);
            friend void wait(EventHandler& eh);
        public:
            class DereferenceFailure : public error::RuntimeError {
                using error::RuntimeError::RuntimeError;
            };
            EventHandler() : ptr() {}
            EventHandler(EventHandler&& e) : ptr(std::move(e.ptr)) {}
            EventHandler& operator=(EventHandler&& e) {
                ptr = std::move(e.ptr);
                return *this;
            }
            template<EventType::type t>
            const Event<t> acquire() {
                if (ptr) {
                    return Event<t>::extract(ptr.get());
                } else {
                    throw DereferenceFailure();
                }
            }

            EventType::type getType() {
                switch (ptr->type) {
                    case SDL_WINDOWEVENT:
                        return EventType::Window;
                    case SDL_QUIT:
                        return EventType::Quit;
                    default:
                        return EventType::Window;
                }
            }

            const Event<EventType::Window> getWindowEvent() {
                return acquire<EventType::Window>();
            }

            virtual ~EventHandler() {}
        private:
           std::unique_ptr<SDL_Event> ptr;
           // deleted
           EventHandler(const EventHandler& e);
           EventHandler& operator=(const EventHandler& e);
           void initptr();
        };

    }


    struct Position {
        int x;
        int y;
        Position(int xi = SDL_WINDOWPOS_UNDEFINED,
                       int yi = SDL_WINDOWPOS_UNDEFINED) : x(xi), y(yi) {}
        Position& setCentered(bool setx, bool sety) {
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
                    const Position& pos = Position())
            : x(pos.x), y(pos.y), w(wi), h(hi) {}
        int x;
        int y;
        int w;
        int h;
        SDL_Rect *toSdlRect() const { return (SDL_Rect*)this; }
    };

    typedef const SDL_PixelFormat* PixelFormat;

    template<typename T>
    class PointerHolder {
    public:
        ~PointerHolder() {}

    protected:
        PointerHolder(T* p) : ptr(p) {}
        PointerHolder(PointerHolder<T>&& o) {
            ptr = o.ptr;
            o.ptr = nullptr;
        }
        T* ptr;
    };

    class Window;
    class Surface : public PointerHolder<SDL_Surface> {
        friend Window;
    private:
        bool needDeallocate;
        // default is unmanaged surface, need to be freed
        Surface(SDL_Surface *p, bool managed = false)
            : PointerHolder(p), needDeallocate(!managed) {}
    public:
        ~Surface() { if (needDeallocate) { SDL_FreeSurface(ptr); } }
        Surface(Surface&& s): PointerHolder((PointerHolder&&)s)
                              ,needDeallocate(s.needDeallocate) {}
        static Surface loadBMP(const std::string& path);
        struct LoadFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        struct BlitFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        struct ConvertFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        void blit(const Surface& src);
        void blit(const Position& destpos, const Surface& src,
                  const Rectangular& srcrect);
        void blitScaled(const Surface& src);
        void blitScaled(const Rectangular& destrect, const Surface& src,
                  const Rectangular& srcrect);
        PixelFormat format() { return ptr->format; }
        Surface convert(PixelFormat format) {
            SDL_Surface *newsuf = SDL_ConvertSurface(ptr, format, 0);
            if (!newsuf) throw ConvertFailure();
            return Surface(newsuf);
        }
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
        Surface getSurface() { return Surface(SDL_GetWindowSurface(ptr), true); }
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
