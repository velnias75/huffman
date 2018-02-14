bin_PROGRAMS = huffdot
noinst_PROGRAMS = hufftest huffenc
noinst_LIBRARIES = libhufflib.a

pkginclude_HEADERS = huffman.h
noinst_HEADERS = hufflib.h

AM_CXXFLAGS = $(RATIONAL_CFLAGS)
AM_LDFLAGS = -Wl,-as-needed -Wl,--gc-sections

libhufflib_a_CXXFLAGS = $(AM_CXXFLAGS) -ffunction-sections -fdata-sections
libhufflib_a_SOURCES = hufflib.cpp

hufftest_SOURCES = hufftest.cpp
hufftest_LDADD = libhufflib.a $(RATIONAL_LIBS)

huffdot_SOURCES = huffdot.cpp
huffdot_LDADD = libhufflib.a $(RATIONAL_LIBS)

huffenc_SOURCES = huffenc.cpp
huffenc_LDADD = libhufflib.a $(RATIONAL_LIBS)
