bin_PROGRAMS = basic_draw canvas color_key color_mod hello sprite luasdl

LIBS = -Wl,--whole-archive -lSDL2main -Wl,--no-whole-archive -lSDL2 -lSDL2_image 
AM_CPPFLAGS = -I$(srcdir)/.. -I..
LDADD = ../libsdlpp.a

basic_draw_SOURCES = basic_draw.cpp common.cpp common.hpp
canvas_SOURCES = canvas.cpp common.cpp common.hpp
color_key_SOURCES = color_key.cpp common.cpp common.hpp
color_mod_SOURCES = color_mod.cpp common.cpp common.hpp
hello_SOURCES = hello.cpp common.cpp common.hpp
sprite_SOURCES = sprite.cpp common.cpp common.hpp
luasdl_SOURCES = luasdl.cpp common.cpp common.hpp
luasdl_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/../luamm
luasdl_LDADD = ../libsdlpp.a -llua

../libsdlpp.a:
	@cd .. && $(MAKE) libsdlpp.a
