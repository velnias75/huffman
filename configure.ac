#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([huffman], [0.1], [heiko@rangun.de], , [https://github.com/velnias75/huffman])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG([0.22])

# Checks for libraries.
PKG_CHECK_MODULES([RATIONAL], [rational >= 2.7], [ AC_DEFINE([HAVE_RATIONAL_H], [1],
	[Define to 1 if you have the <rational/rational.h> header file.]) ], [ true ])

# Checks for header files.
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T

# Checks for library functions.

AC_OUTPUT([
	Makefile
	src/Makefile
])
