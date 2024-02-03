AC_PREREQ([2.69])
AC_INIT([sfe], [0.1], [angelo@kernel-space.org], [sfe], [http://www.kernel-space.org/])
AM_INIT_AUTOMAKE(subdir-objects)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CHECK_TOOL(CC, gcc, gcc)
AC_CHECK_TOOL(CXX, g++, g++)

AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test "x$PKG_CONFIG" = "xno"; then
	AC_MSG_ERROR([You need to install pkg-config])
fi

AC_MSG_NOTICE(found $PKG_CONFIG)

CPPFLAGS="-O3 --pipe -Wall -Wno-deprecated-declarations"

# defualts

# Substiture and create Makefile
AC_SUBST(GTK_LIBS)
AC_CONFIG_FILES([Makefile])

# Checks for libraries.
PKG_CHECK_MODULES([X11], [x11 >= 1.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.6.0])

GTK_CPPFLAGS=`$PKG_CONFIG --cflags gtk4`
GTK_LIBS=`$PKG_CONFIG --libs gtk4`

# updating CFLAGS with gtk4 include dir
LDFLAGS="$LDFLAGS $GTK_LIBS"
CPPFLAGS="$CPPFLAGS $GTK_CPPFLAGS"
LIBS="$LDFLAGS"

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_OUTPUT
