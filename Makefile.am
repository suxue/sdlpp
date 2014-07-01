ACLOCAL_AMFLAGS = -I ../m4

if BUILD_DEMOS
SUBDIRS = demos/
endif

lib_LIBRARIES = libsdlpp.a
libsdlpp_a_SOURCES = sdlpp.cpp sdlpp.hpp

bin_PROGRAMS = sdlpp_test$(EXEEXT)
sdlpp_test_SOURCES = test.cpp sdlpp.hpp
sdlpp_test_LDADD = libsdlpp.a -lboost_test_exec_monitor

test: sdlpp_test$(EXEEXT)
	./sdlpp_test$(EXEEXT)
