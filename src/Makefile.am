AM_CPPFLAGS = -Wall -Wextra -std=c99 -Wpedantic

check_LIBRARIES = libgtu.a
check_PROGRAMS = gtu-test

libgtu_a_SOURCES = \
	init.c \
	test-case.c

libgtu_a_CFLAGS = \
	-I$(top_srcdir)/include \
	$(gtu_U_CFLAGS)

gtu_test_SOURCES = \
	gtu-test.c

gtu_test_CFLAGS = \
	$(libgtu_a_CFLAGS)

gtu_test_LDADD = \
	libgtu.a \
	$(gtu_U_LIBS)

-include $(top_srcdir)/git.mk
