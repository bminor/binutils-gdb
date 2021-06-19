dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

SIM_AC_OPTION_DEFAULT_MODEL(bf537)

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(SDL, sdl, [
	AC_CHECK_LIB(dl, dlopen, [
		SDL_CFLAGS="${SDL_CFLAGS} -DHAVE_SDL"
		SDL_LIBS="-ldl"
		], [SDL_CFLAGS= SDL_LIBS=])
	], [:])
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

SIM_AC_OUTPUT
