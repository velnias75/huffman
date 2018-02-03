#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([huffman], [0.0], [heiko@rangun.de])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_TYPE_SIZE_T

# Checks for library functions.

AC_OUTPUT([
	Makefile
	src/Makefile
])
