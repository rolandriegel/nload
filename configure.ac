# Helder Correia <helder.correia@netcabo.pt>

# Process this file with autoconf to produce a configure script.
AC_INIT(nload, 0.3.1, support@roland-riegel.de)
AM_INIT_AUTOMAKE(nload, 0.3.1)
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h sys/time.h])

# Use -Wall if we have gcc.
if test "x$GCC" = "xyes" ; then
	CFLAGS="$CFLAGS -Wall"
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gettimeofday memset])

AC_OUTPUT([
Makefile
docs/Makefile
docs/nload.1
nload.spec
src/Makefile
])
