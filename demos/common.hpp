#ifndef SDLPP_DEMOS_COMMON_HPP
#define SDLPP_DEMOS_COMMON_HPP

#include"sdlpp.hpp"
#include "config.h"
#include <functional>

typedef std::function<void(const sdlpp::event::EventHandler& e)> Callback;
extern void default_callback(const sdlpp::event::EventHandler& e);

void idlewait(sdlpp::Renderer& renderer, sdlpp::Window& window,
        Callback cb = default_callback);

#endif