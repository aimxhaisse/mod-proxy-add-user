AC_PREREQ(2.59)
AC_INIT([proxy-add-user], [0.1], [mxs@buffout.org])
AM_INIT_AUTOMAKE([1.9 gnu])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
if pkg-config --cflags apr-1 apr-util-1 >/dev/null 2>&1
then
  CPPFLAGS="$CPPFLAGS `pkg-config --cflags apr-1 apr-util-1`"
  LIBS="$LIBS `pkg-config --libs apr-1 apr-util-1`"
else
  AC_ERROR([Cannot find libapr configuration])
fi

AC_PROG_LIBTOOL
AC_OUTPUT
