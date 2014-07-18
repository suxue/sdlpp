AC_INIT([sdlpp], [0.1 foreign])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR(autoconf-common/m4/)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11
AM_PROG_AR
AC_PROG_RANLIB

AC_LANG_PUSH([C])
AC_CHECK_HEADERS([SDL2/SDL.h], [],
                     [AC_MSG_ERROR(You need the SDL C headers.)])
AC_LANG_POP([C])

MY_COMMON

abs_srcdir=`(cd $srcdir && pwd)`
AC_DEFINE_UNQUOTED([SDLPP_DEMO_DATA_DIR], ["$abs_srcdir/demos/data/"])
AH_TEMPLATE([SDLPP_DEMO_DATA_DIR], [sdlpp demo data dir])

AC_CONFIG_FILES([Makefile demos/Makefile])
AC_OUTPUT
