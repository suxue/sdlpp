AC_INIT([sdlpp], [0.1 foreign])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR(../m4)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.14 no-define foreign])
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

AC_ARG_ENABLE([demos],
  AS_HELP_STRING([--disable-demos], [disable building demos]),
  [build_demos=${enableval}],
  [build_demos=yes])

abs_srcdir=`(cd $srcdir && pwd)`
AC_DEFINE_UNQUOTED([SDLPP_DEMO_DATA_DIR], ["$abs_srcdir/demos/data/"])
AH_TEMPLATE([SDLPP_DEMO_DATA_DIR], [sdlpp demo data dir])

AM_CONDITIONAL([BUILD_DEMOS], [test "$build_demos" = yes])
AC_CONFIG_FILES([Makefile demos/Makefile])
AC_OUTPUT
