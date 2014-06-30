/*!
 * @section LICENSE
 * Copyright (c) 2014 Hao Fei <mrfeihao@gmail.com>
 *
 * This file is part of libsdlpp.
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
 * sdlpp is an incomplete c++ wrapper for SDL2.
 */

#ifndef SDLPP_HPP
#define SDLPP_HPP

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
}
//#include "../util/list.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <cstdlib>
#include <stack>

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
            RuntimeError(const std::string& m);
            RuntimeError();
        };
#ifdef _MSC_VER
#define TO_NUMBER(s) ((_ULonglong)(s))
#else
#define TO_NUMBER(s) (s)
#endif
#define THROW_SDLPP_RUNTIME_ERROR() (throw sdlpp::error::RuntimeError(std::string(__FILE__) + ":" + std::to_string(TO_NUMBER(__LINE__))))
    }

    //! %Event Processing
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

        //! base class for all Event specifilizations
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

        //! %window event
        template<>
        class Event<EventType::Window> : protected EventBase<SDL_WindowEvent> {
            Event(PointType p) : EventBase(p) {}
        public:
            typedef Event type;
            static const type extract(const SDL_Event* e);
            SDL_WindowEventID getID() const;
        };

        //! keyboard event
        template<>
        class Event<EventType::Keyboard> : protected EventBase<SDL_KeyboardEvent> {
            Event(PointType p) : EventBase(p) {}
        public:
            typedef Event type;
            static const type extract(const SDL_Event* e);

            //! @return true key is pressed
            //! @return false key is released
            bool pressed() const;

            //! @return true is a key repeat
            bool repeat() const;

            //! physical key code
            SDL_Scancode scancode() const;

            //! SDL virtual key code
            SDL_Keycode sym() const;
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

        //! %event that could be pass around
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

            /*! should be call by the actuall type got through type()
             * @warning dangerous, implicit pointer type reinterpretation,
             *          make sure use the correct EventType given by
             *          type()
             * @return Event represent real event data
            */
            template<EventType::type t>
            const Event<t> acquire() const;

            EventType::type type() const;
            Timestamp timestamp() const;
        protected:
           std::unique_ptr<SDL_Event> ptr; //!< delegate move semantics to unique_ptr
           static EventHandler create(SDL_Event*e);
        private:
           EventHandler(SDL_Event*e) : ptr(e) {};

           // deleted copy ctor
           EventHandler(const EventHandler& e);

           // deleted copy assignment operator
           EventHandler& operator=(const EventHandler& e);
        };

        //! %event that can be polled or waited
        /*!
         *   the storage to store events. It has the same
         *   interface as EventHandler but neither noncopyable nor moveable.
         */
        class EventData : public EventHandler {
            friend bool poll(EventData& eh);
            friend void wait(EventData& eh);
           void initptr();
        public:
            /*! copy EventData to a EventHandler for storing or passing around.
             *  a EventHandler only stores relavent part in the SDL_Event
             *  union, hence uses less memory than original EventData.
             */
            EventHandler slice();
        };
    } // end namespace event

    struct BlendMode {
        enum type {
            None = SDL_BLENDMODE_NONE,
            Blend = SDL_BLENDMODE_BLEND,
            Add = SDL_BLENDMODE_ADD,
            Mod = SDL_BLENDMODE_MOD
        };
    };

    //! a position/point represented by pair of x and y coordinate
    struct Position : public SDL_Point {
        Position(int xi = SDL_WINDOWPOS_UNDEFINED,
                       int yi = SDL_WINDOWPOS_UNDEFINED)
             {x = xi; y =  yi;}
        Position& setCentered(bool setx, bool sety) {
            if (setx) x = SDL_WINDOWPOS_CENTERED;
            if (sety) y = SDL_WINDOWPOS_CENTERED;
            return *this;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, Position pos) {
        return os << "(" << pos.x << "," << pos.y << ")";
    }

    class Handler;

    //! see Handler::createWindow
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

    //! a quadruple structure, x, y coordinate, weight and height
    struct Rectangle : public SDL_Rect {
        Rectangle(int wi, int hi,
                    const Position& pos = Position()) {
            x = pos.x;
            y = pos.y;
            w = wi;
            h = hi;
        }
        Rectangle() {}
    };

    typedef decltype(SDL_PIXELFORMAT_ARGB8888) PixelFormat;
    const PixelFormat DEFAULT_PIXEL_FORMAT= SDL_PIXELFORMAT_ARGB8888;

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

    //! can hold largest pixel
    typedef std::uint32_t PixelValue;

    //! structure represents color by RGB and alpha value
    struct Color {
        enum ColorEnum {
            Black, White, Red, Lime, Blue, Yellow,
            Cyan, Magenta, Silver, Gray, Maroon, Olive,
            Green, Purple, Teal, Navy
        };
        Color(std::uint8_t r, std::uint8_t g,
              std::uint8_t b, std::uint8_t a = 0xff);
        Color(ColorEnum value);
        Color(PixelValue pixel, const SDL_PixelFormat* format);
        std::uint8_t red;
        std::uint8_t green;
        std::uint8_t blue;
        std::uint8_t alpha;
        PixelValue mapRGB(SDL_PixelFormat *format);
        PixelValue mapRGBA(SDL_PixelFormat *format);
    };

    class Window;
    class Texture;
    class TargetTexture;
    class StaticTexture;
    class StreamingTexture;

    //! A Renderer always associsated with a Window
    class Renderer : public PointerHolder<SDL_Renderer> {
        friend Window;
        Renderer(SDL_Renderer* p);
        template<typename T>
        T createTexture(PixelFormat format, SDL_TextureAccess access, int width, int height);
    public:
        static SDL_RendererFlip horizontalFlip();
        static SDL_RendererFlip verticalFlip();
        ~Renderer();
        Renderer(Renderer&& r);

        //! clear(fill) the current rendering target with the drawing color
        //! wipes out the existing video framebuffer
        void clear();

        /*!
         * copy a portion of the texture to the current rendering target.
         * The texture will be stretched to fill the given rectangle
         */
        void copy(Texture& texture, const Rectangle* srcrect = nullptr,
                const Rectangle* destrect = nullptr);

        /*!
         * @brief copy a portion of the texture to the current rendering
         *        target, optionally rotating it by angle around the given
         *        center and also flipping it top-bottom and/or left-right
         * @param angle an angle in degrees that indicates the rotation
         *              that will be applied to dstrect
         * @param flip horizontalFlip(), verticalFlip() or default(no * flip)
         */
        void copy(Texture& texture, const double angle,
                const Rectangle* srcrect = nullptr,
                const Rectangle* destrect = nullptr,
                const Position* center = nullptr,
                SDL_RendererFlip flip = SDL_FLIP_NONE);

        //! update the screen with rendering performed
        void present();

        //! set a texture as the current rendering target.
        //! @warning avoid dangling pointer
        //! @param texture pass nullptr to restore default target
        void setTarget(TargetTexture* texture = nullptr);

        //! set the color used for drawing operations (Rect, Line and
        //! Clear)
        void setDrawColor(const Color& color);

        //! fill rectangular with current color
        void fillRectangle(const Rectangle& rect);

        //! draw a line on the current rendering target (x1, y1) -> (x2, y2)
        void drawLine(Position a, Position b);

        //! draw a point on the current rendering target (x, y)
        void drawPoint(Position pos);

        //! create a static texture associated with current renderer
        StaticTexture spawnStatic(int width, int height,
                PixelFormat format = DEFAULT_PIXEL_FORMAT);

        //! create a streaming texture associated with current renderer
        StreamingTexture spawnStreaming(int width, int height,
                PixelFormat format = DEFAULT_PIXEL_FORMAT);

        //! create a target texture
        //! @see setTarget()
        TargetTexture spawnTarget(int width, int height,
                PixelFormat format = DEFAULT_PIXEL_FORMAT);
    };

    //! describe the memory layout which depneds on the endianess of hardware
    /*!
     * for example RGBA8888 denotes Red, Green, Blue and Alpha will be
     * represented by a uint8_t[4] array, with the specified order or
     * mask.
    */
    struct PixelMask {
         int bpp;      //!< bit-length per pixel
         std::uint32_t rmask;
         std::uint32_t gmask;
         std::uint32_t bmask;
         std::uint32_t amask;
    public:
         PixelMask(PixelFormat format = DEFAULT_PIXEL_FORMAT);
    };

    //! a collection of pixels used in software blitting
    class Surface: public PointerHolder<SDL_Surface> {
        friend Window;
    private:
        bool needDeallocate;

    protected:
        /*!
         * @param managed true when no need to call SDL_DestroySurface in dtor
         */
        Surface(SDL_Surface *p, bool managed = false);

        //! Constructor that build a empty surface with given format
        Surface(int width, int height, PixelFormat format = DEFAULT_PIXEL_FORMAT);
        Surface(int width, int height, const PixelMask& mask);
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
                  const Rectangle* srcrect = nullptr,
                  const Position* destpos = nullptr);
        void blitScaled(const Surface& src, Rectangle* srcrect = nullptr,
                  Rectangle* destrect = nullptr);

        SDL_PixelFormat *getFormat();
        PixelFormat format();

        int width() const;
        int height() const;
        int pitch() const;

        //! set the color key (transparent pixel) in a surface
        void setColorKey(const Color& c);

        //! clear the color key (transparent pixel) in a surface
        void unsetColorKey(const Color& c);

        //! copy an existing surface into a new one that is optimized for
        //! blitting to a surface of a specified pixel format
        Surface convert(const SDL_PixelFormat *format);

        //! raw pixel data
        void *pixels();

        //! Bytes Per Pixel
        int Bpp();
    };

    template<typename T>
    struct Drawable {
        const static bool value = false;
    };
    class Bpp4Surface;
    template<> struct Drawable<Bpp4Surface> { static const bool value = 1; };

    template<typename Derived>
    class Canvas;

    //! helper class for implementing subscript operator in Canvas
    template<typename Derived>
    class PixelCell {
        Canvas<Derived> *canvas;
        Position pos;
    public:
        PixelCell& operator[](int i);
        operator PixelValue();
        operator Color();
        PixelCell& operator=(Color v);
        PixelCell(Canvas<Derived>* c, int xx);
    };

    //! for CRTP (static polymorphism)
    //! Canvas depend on Derived::setPixel(), Derived::getPixel() and
    //! Dervived::getFormat(), Derived::width(), Derived::height()
    template<typename Derived>
    class Canvas {
        static_assert(Drawable<Derived>::value, "Derived is invalid");
        friend PixelCell<Derived>;
        PixelValue drawColor;
    public:
        PixelValue getPixel(int x, int y); //!< inelined to Dervied::getPixel()
        void setPixel(int x, int y, PixelValue v); //!< inlined to Derived::setPixel()
        Canvas() : drawColor(0) {}
        int getHeight();
        int getWidth();
        void clear();
        void drawPoint(Position pos);
        void drawLine(Position a, Position b);
        void drawEllipse(Position center, Position radius);
        void fillEllipse(Position center, Position radius);
        void drawEllipse(Rectangle rect); //!< ellipse inside a rectangle
        void fillEllipse(Rectangle rect); //!< ellipse inside a rectangle
        void drawRectangle(Rectangle rect);
        void fillRectangle(Rectangle rect);
        void drawCircle(Position center, int radius);
        void fillCircle(Position center, int radius);
        PixelCell<Derived> operator[](int x);
        void setDrawColor(Color color);
        void setDrawPixel(PixelValue pv);

        //! delegate to canvas implementation class
        SDL_PixelFormat *getPixelFormat();
    };

    class Bpp4Surface : public Surface, public Canvas<Bpp4Surface> {
    public:
        Bpp4Surface(int width, int height, PixelFormat format);
        void setPixel(int x, int y, PixelValue value);
        PixelValue getPixel(int x, int y);
    private:
        static PixelFormat check(PixelFormat format);
    };

    //! small object which is moveable and repsent a canvas
    /*! like a Surface but is optimized for GPU renderin */
    class Texture : public PointerHolder<SDL_Texture> {
        friend Renderer;
    protected:
        Texture(SDL_Texture* t);
    public:
        Texture(Renderer& r, Surface& s);
        Texture(Renderer& r, Surface&& s);
        ~Texture();

        //! alpha field is not used
        void setColorMod(const Color& color);
        void setAlphaMod(std::uint8_t alpha);
        void setBlendMode(BlendMode::type m);
        Texture(Texture&& t);
    };

    //! TargetTexture acted as a destination of rendering
    class TargetTexture : public Texture {
        friend Renderer;
        TargetTexture(SDL_Texture* t);
    };

    //! not subjective to changes, mostly used as source texture
    class StaticTexture : public Texture {
        friend Renderer;
        StaticTexture(SDL_Texture* t);
    };

    //! subjective to byte oriented changes in pixel level.
    class StreamingTexture : public Texture {
        friend Renderer;
        StreamingTexture(SDL_Texture* t);
    };

    //! represent a GUI %window instance
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

        //! restore size and position after a minimized or maximized window
        void restore();

        Surface getSurface();
        Renderer getRenderer();
    };

    struct Initializer;

    //! through which can do misc jobs which rely on a successfully
    //! initialization of SDL subsystems
    class Handler {
        friend Initializer;
    private:
        Handler() {};
    public:
        ~Handler() { SDL_Quit(); }

        //! create a window
        /*!
         * @param title the title of the window, in UTF-8 encoding
         * @param rect position and height, width of window
         * @param wm window properties
         */
        Window createWindow(const std::string& title,
                            const Rectangle& rect,
                            WindowMode wm = WindowMode());
    };

    //! initalize sdl subsytems and return a useable Handler
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

    namespace error {
        //! root class for exceptions
        inline RuntimeError::RuntimeError(const std::string& m)
            : std::runtime_error(std::string("(") + m + ") " + getmsg()) {}
        inline RuntimeError::RuntimeError() : std::runtime_error(getmsg()) {}
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
        const Event<t> EventHandler::acquire() const {
            if (ptr) {
                return Event<t>::extract(ptr.get());
            } else {
                throw DereferenceFailure();
            }
        }

        inline EventType::type EventHandler::type() const {
            switch (ptr->type) {
                case SDL_WINDOWEVENT:
                    return EventType::Window;
                case SDL_QUIT:
                    return EventType::Quit;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    return EventType::Keyboard;
                default:
                    return EventType::Window;
            }
        }

        inline Timestamp EventHandler::timestamp() const {
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

        inline bool Event<EventType::Keyboard>::pressed() const {
            return ptr->state == SDL_PRESSED;
        }

        inline bool Event<EventType::Keyboard>::repeat() const {
            return ptr->repeat != 0;
        }

        inline SDL_Scancode Event<EventType::Keyboard>::scancode() const {
            return ptr->keysym.scancode;
        }

        inline SDL_Keycode Event<EventType::Keyboard>::sym() const {
            return ptr->keysym.sym;
        }

        inline const Event<EventType::Window>
        Event<EventType::Window>::extract(const SDL_Event* e) {
            return Event(&e->window);
        }

        inline const Event<EventType::Keyboard>
        Event<EventType::Keyboard>::extract(const SDL_Event* e) {
            return Event(&e->key);
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
    inline void Renderer::clear() { if (SDL_RenderClear(ptr)) THROW_SDLPP_RUNTIME_ERROR(); }
    inline void Renderer::present() { SDL_RenderPresent(ptr); }

    inline Surface::Surface(SDL_Surface *p, bool managed)
        : PointerHolder(p), needDeallocate(!managed) {}
    inline Surface::~Surface() { if (needDeallocate) { SDL_FreeSurface(ptr); } }
    inline Surface::Surface(Surface&& s): PointerHolder((PointerHolder&&)s),
                                 needDeallocate(s.needDeallocate) {}
    inline SDL_PixelFormat *Surface::getFormat() { return ptr->format; }
    inline Surface Surface::convert(const SDL_PixelFormat *format) {
        SDL_Surface *newsuf = SDL_ConvertSurface(ptr,
                const_cast<SDL_PixelFormat*>(format), 0);
        if (!newsuf) throw ConvertFailure();
        return Surface(newsuf);
    }


    inline Texture::Texture(Renderer& r, Surface& s) :
        PointerHolder(SDL_CreateTextureFromSurface(r.get(), s.get()))
    {
        if (!ptr) { THROW_SDLPP_RUNTIME_ERROR(); }
    }

    inline Texture::Texture(Renderer& r, Surface&& s) :
        PointerHolder(SDL_CreateTextureFromSurface(r.get(), s.get()))
    {
        if (!ptr) { THROW_SDLPP_RUNTIME_ERROR(); }
    }

    inline Texture::~Texture() { SDL_DestroyTexture(ptr); }
    inline Window::Window(SDL_Window* p) : PointerHolder(p) {}
    inline Window::Window(Window&& w) : PointerHolder((PointerHolder&&)w) {}
    inline Texture::Texture(Texture&& t) : PointerHolder((PointerHolder&&)t) {}
    inline void Window::update() { SDL_UpdateWindowSurface(ptr); }
    inline Surface Window::getSurface() { return Surface(SDL_GetWindowSurface(ptr), true); }

    inline Renderer Window::getRenderer() {
        auto p = SDL_CreateRenderer(this->ptr, -1, 0);
        if (!p) {
            THROW_SDLPP_RUNTIME_ERROR();
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
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline void Renderer::fillRectangle(const Rectangle& rect) {
        if (SDL_RenderFillRect(ptr, &rect) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline void Renderer::drawLine(Position a, Position b) {
        if (SDL_RenderDrawLine(ptr, a.x, a.y, b.x, b.y) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline void Renderer::drawPoint(Position p) {
        if (SDL_RenderDrawPoint(ptr, p.x, p.y) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline int Surface::width() const {
        return ptr->w;
    }

    inline int Surface::height() const {
        return ptr->h;
    }

    inline void Surface::setColorKey(const Color& c) {
        if (SDL_SetColorKey(ptr, true,
                    SDL_MapRGB(getFormat(), c.red, c.green, c.blue)) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline void Surface::unsetColorKey(const Color& c) {
        if (SDL_SetColorKey(ptr, false,
                    SDL_MapRGB(getFormat(), c.red, c.green, c.blue)) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }


    inline Color::Color(std::uint8_t r, std::uint8_t g,
          std::uint8_t b, std::uint8_t a)
        : red(r), green(g), blue(b), alpha(a) { }

    inline void Texture::setColorMod(const Color& color) {
        if (SDL_SetTextureColorMod(ptr, color.red, color.green, color.blue)
               < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline void Window::restore() { SDL_RestoreWindow(ptr); }
    inline Texture::Texture(SDL_Texture* t) : PointerHolder(t) {}

    inline void Renderer::setTarget(TargetTexture* texture) {
        if (SDL_SetRenderTarget(ptr, texture ? texture->get() : nullptr) <0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline StaticTexture Renderer::spawnStatic(int width, int height,
                                                 PixelFormat format) {
        return createTexture<StaticTexture>(format, SDL_TEXTUREACCESS_STATIC,
                width, height);
    }

    inline StreamingTexture Renderer::spawnStreaming(
            int width, int height, PixelFormat format) {
        return createTexture<StreamingTexture>(format, SDL_TEXTUREACCESS_STREAMING,
                width, height);
    }

    inline TargetTexture Renderer::spawnTarget(
            int width, int height, PixelFormat format) {
        return createTexture<TargetTexture>(format, SDL_TEXTUREACCESS_TARGET,
                width, height);
    }

    template<typename T>
    inline T Renderer::createTexture(PixelFormat format,
            SDL_TextureAccess access, int width, int height) {
        auto tp = SDL_CreateTexture(ptr, format, access, width, height);
        if (!tp) { THROW_SDLPP_RUNTIME_ERROR(); }
        return T(tp);
    }

    inline void Texture::setAlphaMod(std::uint8_t alpha) {
        if (SDL_SetTextureAlphaMod(ptr, alpha) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline void Texture::setBlendMode(BlendMode::type m) {
        if (SDL_SetTextureBlendMode(ptr, (SDL_BlendMode)m) < 0) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline SDL_RendererFlip Renderer::horizontalFlip() {
        return SDL_FLIP_HORIZONTAL;
    }

    inline SDL_RendererFlip Renderer::verticalFlip() {
        return SDL_FLIP_VERTICAL;
    }

    inline void Renderer::copy(Texture& texture, const double angle,
            const Rectangle* srcrect,
            const Rectangle* destrect,
            const Position* center,
            SDL_RendererFlip flip) {
        if (SDL_RenderCopyEx(ptr, texture.get(), srcrect, destrect, angle,
                    center, flip) < 0) {
           THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline TargetTexture::TargetTexture(SDL_Texture*p) : Texture(p) {}
    inline StaticTexture::StaticTexture(SDL_Texture*p) : Texture(p) {}
    inline StreamingTexture::StreamingTexture(SDL_Texture*p) : Texture(p) {}

    inline PixelMask::PixelMask(PixelFormat format) {
        if (SDL_FALSE == SDL_PixelFormatEnumToMasks(format,
                    &bpp, &rmask, &gmask, &bmask, &amask)) {
            THROW_SDLPP_RUNTIME_ERROR();
        }
    }

    inline Surface::Surface(int width, int height, PixelFormat format)
        : PointerHolder(nullptr), needDeallocate(true) {
        PixelMask mask(format);
        auto s = SDL_CreateRGBSurface(0, width, height, mask.bpp,
                mask.rmask, mask.gmask, mask.bmask, mask.amask);
        if (!s) { THROW_SDLPP_RUNTIME_ERROR(); }
        ptr = s;
    }

    inline Surface::Surface(int width, int height, const PixelMask& mask)
        : PointerHolder(nullptr), needDeallocate(true) {
        auto s = SDL_CreateRGBSurface(0, width, height, mask.bpp,
                mask.rmask, mask.gmask, mask.bmask, mask.amask);
        if (!s) { THROW_SDLPP_RUNTIME_ERROR(); }
        ptr = s;
    }

    inline PixelFormat Surface::format() {
        return (PixelFormat)getFormat()->format;
    }

    inline PixelValue Color::mapRGBA(SDL_PixelFormat *format) {
        return SDL_MapRGBA(format, red, green, blue, alpha);
    }

    inline PixelValue Color::mapRGB(SDL_PixelFormat *format) {
        return SDL_MapRGB(format, red, green, blue);
    }

    inline void * Surface::pixels() { return ptr->pixels; }
    inline int Surface::Bpp() { return getFormat()->BytesPerPixel; }
    inline int Surface::pitch() const { return ptr->pitch; }

    template<typename Derived>
    void Canvas<Derived>::drawPoint(Position pos) {
        setPixel(pos.x, pos.y, drawColor);
    }

    template<typename Derived>
    void Canvas<Derived>::setPixel(int x, int y, PixelValue v) {
        static_cast<Derived*>(this)->setPixel(x, y, v);
    }

    template<typename Derived>
    int Canvas<Derived>::getHeight() {
        return static_cast<Derived*>(this)->height();
    }

    template<typename Derived>
    int Canvas<Derived>::getWidth() {
        return static_cast<Derived*>(this)->width();
    }

    template<typename Derived>
    void Canvas<Derived>::setDrawColor(Color c) {
        drawColor = c.mapRGBA(getPixelFormat());
    }

    template<typename Derived>
    void Canvas<Derived>::setDrawPixel(PixelValue v) {
        drawColor = v;
    }

    template<typename Derived>
    SDL_PixelFormat *Canvas<Derived>::getPixelFormat() {
        return static_cast<Derived*>(this)->getFormat();
    }

    template<typename Derived>
    PixelValue Canvas<Derived>::getPixel(int x, int y) {
        return static_cast<Derived*>(this)->getPixel(x, y);
    }

    inline Bpp4Surface::Bpp4Surface(int width, int height, PixelFormat format)
        : Surface(width, height, Bpp4Surface::check(format)), Canvas() {}

    inline PixelFormat Bpp4Surface::check(PixelFormat format) {
        PixelMask m(format);
        if (m.bpp > 24) {
            return format;
        } else {
            throw error::RuntimeError("format does not has 4 byte pixel");
        }
    }

    inline void Bpp4Surface::setPixel(int x, int y, PixelValue value) {
        auto rawbytes = static_cast<std::uint8_t*>(pixels());
        rawbytes += y * pitch();
        auto rawpixels = reinterpret_cast<std::uint32_t*>(rawbytes);
        rawpixels += x;
        *rawpixels = value;
    }

    inline PixelValue Bpp4Surface::getPixel(int x, int y) {
        auto rawbytes = static_cast<std::uint8_t*>(pixels());
        rawbytes += y * pitch();
        auto rawpixels = reinterpret_cast<std::uint32_t*>(rawbytes);
        rawpixels += x;
        return *rawpixels;
    }

    inline Color::Color(PixelValue pixel, const SDL_PixelFormat* format) {
        SDL_GetRGB(pixel, format, &red, &green, &blue);
        alpha = 0xff;
    }

    inline std::ostream& operator<<(std::ostream& out, Color color) {
        return (out << "(" << (int)color.red << "," << (int)color.green <<
            ","<<(int)color.blue << "," <<(int)color.alpha) << ")";
    }

    template<typename Derived>
    PixelCell<Derived>& PixelCell<Derived>::operator[](int i) {
        pos.y = i;
        return *this;
    }

    template<typename Derived>
    PixelCell<Derived>::operator PixelValue() {
        return canvas->getPixel(pos.x, pos.y);
    }

    template<typename Derived>
    PixelCell<Derived>::operator Color() {
        return Color(canvas->getPixel(pos.x, pos.y), canvas->getPixelFormat());
    }

    template<typename Derived>
    PixelCell<Derived>& PixelCell<Derived>::operator=(Color c) {
        canvas->setPixel(pos.x, pos.y, c.mapRGBA(canvas->getPixelFormat()));
        return *this;
    }

    template<typename Derived>
    PixelCell<Derived>::PixelCell(Canvas<Derived>* c, int x) : canvas(c), pos(x,0) {}

    template<typename Derived>
    PixelCell<Derived> Canvas<Derived>::operator[](int x) {
        return PixelCell<Derived>(this, x);
    }

    template<typename Derived>
    void Canvas<Derived>::drawLine(Position a, Position b) {
        int dx = std::abs(b.x - a.x), sx = a.x < b.x ? 1 : -1;
        int dy = -std::abs(b.y - a.y), sy = a.y < b.y ? 1 : -1;
        int err = dx + dy, e2;

        for (; ;) {
            drawPoint(a);
            e2 = 2 * err;
            if (e2 >= dy) {
                if (a.x == b.x) break;
                err += dy;
                a.x += sx;
            }
            if (e2 <= dx) {
                if (a.y == b.y) break;
                err += dx;
                a.y += sy;
            }
        }
    }



    template<typename Derived>
    void Canvas<Derived>::drawEllipse(Position center, Position radius) {
        int a = radius.x, b = radius.y;
        int xm = center.x, ym = center.y;
        int x = -a, y = 0;
        long e2 = (long)b*b, err = x*(2*e2+x)+e2;

        do {
            setPixel(xm-x, ym+y, drawColor);
            setPixel(xm+x, ym+y, drawColor);
            setPixel(xm+x, ym-y, drawColor);
            setPixel(xm-x, ym-y, drawColor);
            e2 = 2*err;
            if (e2 >= (x*2+1)*(long)b*b)
                err += (++x*2+1) * (long)b*b;
            if (e2 <= (y*2+1)*(long)a*a)
                err += (++y*2+1)*(long)a*a;
        } while (x <= 0);

        while (y++ < b) {
            setPixel(xm, ym+y, drawColor);
            setPixel(xm, ym-y, drawColor);
        }
    }

    template<typename Derived>
    void Canvas<Derived>::fillEllipse(Position center, Position radius) {
        int a = radius.x, b = radius.y;
        int xm = center.x, ym = center.y;
        int x = -a, y = 0;
        long e2 = (long)b*b, err = x*(2*e2+x)+e2;

        do {
            drawLine(Position(xm-x, ym+y), Position(xm+x, ym+y));
            drawLine(Position(xm-x, ym-y), Position(xm+x, ym-y));
            e2 = 2*err;
            if (e2 >= (x*2+1)*(long)b*b)
                err += (++x*2+1) * (long)b*b;
            if (e2 <= (y*2+1)*(long)a*a)
                err += (++y*2+1)*(long)a*a;
        } while (x <= 0);

        while (y++ < b) {
            drawLine(Position(xm, ym+y), Position(xm, ym-y));
        }
    }

    template<typename Derived>
    void Canvas<Derived>::drawCircle(Position center, int radius) {
        int xm = center.x, ym = center.y, r = radius;
        int x = -r, y = 0, err = 2-2*r;

        do {
            setPixel(xm-x, ym+y, drawColor);
            setPixel(xm-y, ym-x, drawColor);
            setPixel(xm+x, ym-y, drawColor);
            setPixel(xm+y, ym+x, drawColor);
            r = err;
            if (r <= y) err += ++y*2 + 1;
            if (r > x || err > y)
                err += ++x * 2+1;
        } while (x < 0);
    }

    template<typename Derived>
    void Canvas<Derived>::drawEllipse(Rectangle rect) {
        int x0 = rect.x, y0 = rect.y;
        int x1 = x0 + rect.w, y1 = y0 + rect.h;

        long a = std::abs(x1-x0), b = std::abs(y1-y0), b1 = b&1;
        double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;
        double err = dx+dy+b1*a*a, e2;

        if (x0 > x1) { x0 = x1; x1 += a; }
        if (y0 > y1) y0 = y1;
        y0 += (b+1) / 2;
        y1 = y0 - b1;
        a = 8*a*a;
        b1 = 8*b*b;

        do {
            setPixel(x1, y0, drawColor);
            setPixel(x0, y0, drawColor);
            setPixel(x0, y1, drawColor);
            setPixel(x1, y1, drawColor);
            e2 = 2*err;
            if (e2 <= dy) { y0++; y1-- ; err += dy += a; }
            if (e2 >= dx || 2*err > dy) { x0++; x1-- ; err += dx += b1;}
        } while (x0 <= x1);

        while (y0-y1 <= b) {
            setPixel(x0-1, y0, drawColor);
            setPixel(x1+1, y0++, drawColor);
            setPixel(x0-1, y1, drawColor);
            setPixel(x1+1, y1--, drawColor);
        }
    }


    template<typename Derived>
    void Canvas<Derived>::fillEllipse(Rectangle rect) {
        int x0 = rect.x, y0 = rect.y;
        int x1 = x0 + rect.w, y1 = y0 + rect.h;

        long a = std::abs(x1-x0), b = std::abs(y1-y0), b1 = b&1;
        double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;
        double err = dx+dy+b1*a*a, e2;

        if (x0 > x1) { x0 = x1; x1 += a; }
        if (y0 > y1) y0 = y1;
        y0 += (b+1) / 2;
        y1 = y0 - b1;
        a = 8*a*a;
        b1 = 8*b*b;

        do {
            drawLine(Position(x1, y0), Position(x1, y1));
            drawLine(Position(x0, y0), Position(x0, y1));
            e2 = 2*err;
            if (e2 <= dy) { y0++; y1-- ; err += dy += a; }
            if (e2 >= dx || 2*err > dy) { x0++; x1-- ; err += dx += b1;}
        } while (x0 <= x1);

        while (y0-y1 <= b) {
            drawLine(Position(x0-1, y0), Position(x0-1, y1));
            drawLine(Position(x1+1, y0++), Position(x1+1, y1--));
        }
    }

    template<typename Derived>
    void Canvas<Derived>::drawRectangle(Rectangle rect) {
        // a b
        // c d
        Position a(rect.x, rect.y);
        Position c(rect.x, rect.y+rect.h);
        Position b(rect.x+rect.w, rect.y);
        Position d(rect.x+rect.w, rect.y+rect.h);
        drawLine(a, b);
        drawLine(a, c);
        drawLine(c, d);
        drawLine(b, d);
    }

    template<typename Derived>
    void Canvas<Derived>::fillRectangle(Rectangle rect) {
        for (int i = rect.x; i < rect.x + rect.w; i++) {
            for (int j = rect.y; j < rect.y + rect.h; j++) {
                setPixel(i, j, drawColor);
            }
        }
    }

    template<typename Derived>
    void Canvas<Derived>::clear() {
        const static Position start(0, 0);
        fillRectangle(Rectangle(getWidth(), getHeight(), start));
    }

    template<typename Derived>
    void Canvas<Derived>::fillCircle(Position center, int radius) {
        int xm = center.x, ym = center.y, r = radius;
        int x = -r, y = 0, err = 2-2*r;

        drawLine(Position(xm, ym-r), Position(xm, ym+r));
        do {
            drawLine(Position(xm+x, ym+y), Position(xm+x, ym-y));
            drawLine(Position(xm-x, ym+y), Position(xm-x, ym-y));

            r = err;
            if (r <= y) err += ++y*2 + 1;
            if (r > x || err > y)
                err += ++x * 2+1;
        } while (x < 0);
    }
}

#ifndef SDLPP_PRIVATE
#undef THROW_SDLPP_RUNTIME_ERROR
#undef TO_NUMBER
#endif
#endif
