AM_CPPFLAGS = -Wall -Wextra -std=c99 -Wpedantic

check_LIBRARIES = libgtu.a

libgtu_a_SOURCES = \
	init.c \
	object.c \
	test-case.c \
	g-test-case.c \
	test-suite.c \
	flags.c

libgtu_a_CFLAGS = \
	-I$(top_srcdir)/include \
	$(gtu_U_CFLAGS)

if IS_SUBPROJECT
else
check_PROGRAMS = gtu-test

gtu_test_SOURCES = \
	gtu-test.c

gtu_test_CFLAGS = \
	$(libgtu_a_CFLAGS)

gtu_test_LDADD = \
	libgtu.a \
	$(gtu_U_LIBS)
endif

-include $(top_srcdir)/git.mk
