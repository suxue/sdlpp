/*!
 * @file
 * @section LICENSE
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Hao Fei <mrfeihao@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 * sdlpp is an incomplete c++ wrapper for SDL2
 */

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

    //! platform related functions
    namespace os {
        /*! wait a specified number of milliseconds before returning
         *  @param ms Number of millisecond
         */
        void delay(std::uint32_t ms);
    }

    namespace error {
        //! retrieve a message about the last error that occurred
        std::string getmsg();

        //! root class for exceptions
        struct RuntimeError : public std::runtime_error {
            RuntimeError(const std::string& m) : std::runtime_error(m) {}
            RuntimeError() : std::runtime_error(getmsg()) {}
        };
    }

    //! Event Processing
    /*!
     */
    namespace event {

        //! closure for keeping inside enum
        struct EventType {
            //! an enum represents all kinds of events
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

        //! Base class for all Event specifilizations
        template<typename T>
        class EventBase {
        protected:
            const T* ptr;
            typedef const T* PointType;
            EventBase(const T* p);
        };

        //! event that expose details, internally hold a reference to EventHandler
        /*!
         *  Event is an interface class through which
         *  actual event data are exposed, users are only allowed to hold
         *  const reference to a Event, the actual data are stored inside
         *  EventHandler. Exceptions will be trigger when trying to access
         *  Event when its underleying data store(EventHandler) is dead, such
         *  as out of scope.
         */
        template<EventType::type T>
        class Event;

        //! window event
        template<>
        class Event<EventType::Window> : protected EventBase<SDL_WindowEvent> {
            Event(PointType p) : EventBase(p) {}
        public:
            typedef Event type;
            static const type extract(const SDL_Event* e);
            SDL_WindowEventID getID() const;
        };

        class EventData;

        //! use this function to poll for currently pending events.
        /*!
         * @return **true** there is pending event
         * @return **false** none events available in queue
         */
        bool poll(EventData& eh);

        //! like poll, but will block until interesting events happen
        void wait(EventData& eh);

        //! event that could be pass around
        /*!
          a unique_ptr like wrapper which is constructed from
          EventData, has move ctor and assignment but noncopyable. Through
          EventHandler user can only access event type and timestamp, more
          specific event data are exposed through Event.
        */
        class EventHandler {
        public:
            class DereferenceFailure : public error::RuntimeError {
                using error::RuntimeError::RuntimeError;
            };
            EventHandler();
            EventHandler(EventHandler&& e);
            EventHandler& operator=(EventHandler&& e);

            /*! should be call by the actuall type got through getType()
             * @warning dangerous, implicit pointer type reinterpretation
             * @return Event represent real event data
            */
            template<EventType::type t>
            const Event<t> acquire();

            EventType::type getType();
            Timestamp getTimeStamp();
        protected:
           std::unique_ptr<SDL_Event> ptr; //!< delegate move semantics to unique_ptr
           static EventHandler create(SDL_Event*e);
        private:
           EventHandler(SDL_Event*e) : ptr(e) {};

           // deleted copy ctor and assignment
           EventHandler(const EventHandler& e);
           EventHandler& operator=(const EventHandler& e);
        };

        //! event that can be polled or waited
        /*!
         *   the storage to store events. It has the same
         *   interface as EventHandler but neither noncopyable nor moveable.
         */
        class EventData : public EventHandler {
            friend bool poll(EventData& eh);
            friend void wait(EventData& eh);
           void initptr();
        public:
            // compact memory
            EventHandler dump();
        };
    } // end namespace event

    //! window position represented by x and y coordinate
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
        WindowMode fullscreen();
        WindowMode fulldesktop();
        WindowMode opengl();
        WindowMode hide();
        WindowMode resizable();
        WindowMode grab();
        WindowMode minimize();
        WindowMode maximize();
        WindowMode borderless();
    };

    //! 4-tuple, x, y coordinate, weight and height
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

    //! general template base class for hold a c pointer
    template<typename T>
    class PointerHolder {
    public:
        ~PointerHolder() {}
        T* get() { return ptr; }
    protected:
        PointerHolder(T* p) : ptr(p) {}
        PointerHolder(PointerHolder<T>&& o);
        T* ptr;
    private:
        //deleted
        PointerHolder(const PointerHolder&);
        PointerHolder& operator=(const PointerHolder&);
    };

    struct Color {
        Color(std::uint8_t r, std::uint8_t g,
              std::uint8_t b, std::uint8_t a = 0xff);
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
        std::uint8_t alpha;
    };

    class Window;
    class Texture;

    // wrap SDL_Renderer
    class Renderer : public PointerHolder<SDL_Renderer> {
        friend Window;
        Renderer(SDL_Renderer* p);
    public:
        ~Renderer();
        Renderer(Renderer&& r);
        //! clear the current rendering target with the drawing color
        void clear();

        /*!
         * copy a portion of the texture to the current rendering target.
         * The texture will be stretched to fill the given rectangle
         */
        void copy(Texture& texture, const Rectangular* srcrect = nullptr,
                const Rectangular* destrect = nullptr);

        //! update the screen with rendering performed
        void present();

        //! set the color used for drawing operations (Rect, Line and
        //! Clear)
        void setDrawColor(const Color& color);

        // fill rectangular with current color
        void fillRect(const Rectangular* rect);
        void drawLine(int x1, int y1, int x2, int y2);
        void drawPoint(int x, int y);
    };

    //! a collection of pixels used in software blitting
    class Surface : public PointerHolder<SDL_Surface> {
        friend Window;
    private:
        bool needDeallocate;

        /*!
         * @parma managed true when no need to call SDL_DestroySurface
         */
        Surface(SDL_Surface *p, bool managed = false);
    public:
        ~Surface();
        Surface(Surface&& s);
        static Surface loadBMP(const std::string& path);

        //! load image through SDL2_image extension library
        static Surface loadIMG(const std::string& path);
        struct LoadFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        struct BlitFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        struct ConvertFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        void blit(const Surface& src,
                  const Rectangular* srcrect = nullptr,
                  const Position* destpos = nullptr);
        void blitScaled(const Surface& src, const Rectangular* srcrect = nullptr,
                  const Rectangular* destrect = nullptr);
        PixelFormat getFormat();
        int getWidth();
        int getHeight();

        //! set the color key (transparent pixel) in a surface
        void setColorKey(const Color& c);

        //! clear the color key (transparent pixel) in a surface
        void unsetColorKey(const Color& c);

        //! copy an existing surface into a new one that is optimized for
        //! blitting to a surface of a specified pixel format
        Surface convert(PixelFormat format);
    };

    class Texture : public PointerHolder<SDL_Texture> {
    public:
        Texture(Renderer& r, Surface& s);
        ~Texture();
    };

    class Window : public PointerHolder<SDL_Window> {
        friend Handler;
        Window(SDL_Window* p);
    public:
        struct CreateFailure : public error::RuntimeError {
            using error::RuntimeError::RuntimeError;
        };
        Window(Window&& w);
        ~Window();
        //! copy the window surface to the screen
        void update();

        Surface getSurface();
        Renderer getRenderer();
    };

    struct Initializer;
    class Handler {
        friend Initializer;
    private:
        Handler() {};
    public:
        ~Handler() { SDL_Quit(); }

        //! create a window
        /*!
         * @parma title the title of the window, in UTF-8 encoding
         * @parma rect position and height, width of window
         * @parma wm window properties
         */
        Window createWindow(const std::string& title,
                            const Rectangular& rect,
                            WindowMode wm = WindowMode());
    };

    //! initalize sdl subsytems
    struct Initializer {
        struct InitFailure : error::RuntimeError { using error::RuntimeError::RuntimeError;};
        const std::uint32_t value;
        Initializer() : value(0) {}

        //! activate timer subsystem
        Initializer timer();
        //! activate audio subsystem
        Initializer audio();
        //! activate video subsystem
        Initializer video();
        //! activate joystick subsystem
        Initializer joyStick();
        //! activate gameconstroller subsystem
        Initializer gameControoler();
        //! activate events subsystem
        Initializer events();
        //! activate eveny subsystem
        Initializer everything();

        //! don't catch fatal signals
        /*!
         * Unless the SDL_INIT_NOPARACHUTE flag is set, it will install
         * cleanup signal handlers for some commonly ignored fatal signals
         * (like SIGSEGV).
         */
        Initializer noParachute();

        //! get a handler has features specified in this Initializer
        Handler acquire();
    private:
        Initializer(std::uint32_t v);
    };
}

//
// Implementations
//
namespace sdlpp {
    namespace os {
        inline void delay(std::uint32_t ms) { SDL_Delay(ms); }
    }

    namespace event {
        template<typename T>
            EventBase<T>::EventBase(const T* p) : ptr(p) {}

        inline EventHandler::EventHandler() : ptr() {}
        inline EventHandler::EventHandler(EventHandler&& e)
            : ptr(std::move(e.ptr)) {}

        inline EventHandler& EventHandler::operator=(EventHandler&& e) {
            ptr = std::move(e.ptr);
            return *this;
        }

        template<EventType::type t>
        const Event<t> EventHandler::acquire() {
            if (ptr) {
                return Event<t>::extract(ptr.get());
            } else {
                throw DereferenceFailure();
            }
        }

        inline EventType::type EventHandler::getType() {
            switch (ptr->type) {
                case SDL_WINDOWEVENT:
                    return EventType::Window;
                case SDL_QUIT:
                    return EventType::Quit;
                default:
                    return EventType::Window;
            }
        }

        inline Timestamp EventHandler::getTimeStamp() {
            if (ptr) {
                return ptr->common.timestamp;
            } else {
                throw DereferenceFailure();
            }
        }

        inline EventHandler EventHandler::create(SDL_Event*e) {
            return EventHandler(e);
        }

        inline SDL_WindowEventID Event<EventType::Window>::getID() const {
            return (SDL_WindowEventID)ptr->event;
        }

        inline const Event<EventType::Window>
        Event<EventType::Window>::extract(const SDL_Event* e) {
            return Event(&e->window);
        }
    }


    inline WindowMode WindowMode::fullscreen() {
        return WindowMode(value | SDL_WINDOW_FULLSCREEN);
    }
    inline WindowMode WindowMode::fulldesktop() {
        return WindowMode(value | SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    inline WindowMode WindowMode::opengl() {
        return WindowMode(value | SDL_WINDOW_OPENGL);
    }
    inline WindowMode WindowMode::hide() {
        return WindowMode(value | SDL_WINDOW_HIDDEN);
    }
    inline WindowMode WindowMode::resizable() {
        return WindowMode(value | SDL_WINDOW_RESIZABLE);
    }
    inline WindowMode WindowMode::grab() {
        return WindowMode(value | SDL_WINDOW_INPUT_GRABBED);
    }
    inline WindowMode WindowMode::minimize() {
        return WindowMode(value | SDL_WINDOW_MINIMIZED);
    }
    inline WindowMode WindowMode::maximize() {
        return WindowMode(value | SDL_WINDOW_MAXIMIZED);
    }
    inline WindowMode WindowMode::borderless() {
        return WindowMode(value | SDL_WINDOW_BORDERLESS);
    }

    template<typename T>
    PointerHolder<T>::PointerHolder(PointerHolder<T>&& o) {
        ptr = o.ptr;
        o.ptr = nullptr;
    }

    inline Renderer::Renderer(SDL_Renderer* p) : PointerHolder(p) {}
    inline Renderer::~Renderer() { SDL_DestroyRenderer(ptr); }
    inline Renderer::Renderer(Renderer&& r) : PointerHolder((PointerHolder&&)r) { }
    inline void Renderer::clear() { if (SDL_RenderClear(ptr)) throw error::RuntimeError(); }
    inline void Renderer::present() { SDL_RenderPresent(ptr); }

    inline Surface::Surface(SDL_Surface *p, bool managed)
        : PointerHolder(p), needDeallocate(!managed) {}
    inline Surface::~Surface() { if (needDeallocate) { SDL_FreeSurface(ptr); } }
    inline Surface::Surface(Surface&& s): PointerHolder((PointerHolder&&)s),
                                 needDeallocate(s.needDeallocate) {}
    inline PixelFormat Surface::getFormat() { return ptr->format; }
    inline Surface Surface::convert(PixelFormat format) {
        SDL_Surface *newsuf = SDL_ConvertSurface(ptr, format, 0);
        if (!newsuf) throw ConvertFailure();
        return Surface(newsuf);
    }


    inline Texture::Texture(Renderer& r, Surface& s) :
        PointerHolder(SDL_CreateTextureFromSurface(r.get(), s.get()))
    {
        if (!ptr) { throw error::RuntimeError(); }
    }

    inline Texture::~Texture() { SDL_DestroyTexture(ptr); }
    inline Window::Window(SDL_Window* p) : PointerHolder(p) {}
    inline Window::Window(Window&& w) : PointerHolder((PointerHolder&&)w) {}
    inline void Window::update() { SDL_UpdateWindowSurface(ptr); }
    inline Surface Window::getSurface() { return Surface(SDL_GetWindowSurface(ptr), true); }

    inline Renderer Window::getRenderer() {
        auto p = SDL_CreateRenderer(this->ptr, -1, 0);
        if (!p) {
            throw error::RuntimeError();
        } else {
            return Renderer(p);
        }
    }


    inline Initializer Initializer::timer() { return Initializer(value | SDL_INIT_TIMER);}
    inline Initializer Initializer::audio() { return Initializer(value | SDL_INIT_AUDIO);}
    inline Initializer Initializer::video() { return Initializer(value | SDL_INIT_VIDEO);}
    inline Initializer Initializer::joyStick() { return Initializer(value | SDL_INIT_JOYSTICK);}
    inline Initializer Initializer::gameControoler() { return Initializer(value | SDL_INIT_GAMECONTROLLER);}
    inline Initializer Initializer::events() { return Initializer(value | SDL_INIT_EVENTS);}
    inline Initializer Initializer::everything() { return Initializer(value | SDL_INIT_EVERYTHING);}
    inline Initializer Initializer::noParachute() { return Initializer(value | SDL_INIT_NOPARACHUTE);}

    inline Handler Initializer::acquire() {
        if (SDL_Init(value) != 0) throw InitFailure();
        return Handler();
    }

    inline Initializer::Initializer(std::uint32_t v) : value(v) {}

    inline void Renderer::setDrawColor(const Color& color) {
        if (SDL_SetRenderDrawColor(ptr, color.red, color.green, color.blue,
                   color.alpha) < 0) {
            throw error::RuntimeError();
        }
    }

    inline void Renderer::fillRect(const Rectangular* rect) {
        if (SDL_RenderFillRect(ptr, rect ? rect->toSdlRect() : nullptr) < 0) {
            throw error::RuntimeError();
        }
    }

    inline void Renderer::drawLine(int x1, int y1, int x2, int y2) {
        if (SDL_RenderDrawLine(ptr, x1, y1, x2, y2) < 0) {
            throw error::RuntimeError();
        }
    }

    inline void Renderer::drawPoint(int x, int y) {
        if (SDL_RenderDrawPoint(ptr, x, y) < 0) {
            throw error::RuntimeError();
        }
    }

    inline int Surface::getWidth() {
        return ptr->w;
    }

    inline int Surface::getHeight() {
        return ptr->h;
    }

    inline void Surface::setColorKey(const Color& c) {
        if (SDL_SetColorKey(ptr, true,
                    SDL_MapRGB(getFormat(), c.red, c.green, c.blue)) < 0) {
            throw error::RuntimeError();
        }
    }

    inline void Surface::unsetColorKey(const Color& c) {
        if (SDL_SetColorKey(ptr, false,
                    SDL_MapRGB(getFormat(), c.red, c.green, c.blue)) < 0) {
            throw error::RuntimeError();
        }
    }


    inline Color::Color(std::uint8_t r, std::uint8_t g,
          std::uint8_t b, std::uint8_t a)
        : red(r), green(g), blue(b), alpha(a) { }

}

#endif
