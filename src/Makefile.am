AM_CPPFLAGS = -Wall -Wextra -std=c99 -Wpedantic

if IS_SUBPROJECT
check_LIBRARIES = libgtu.a
else
noinst_LIBRARIES = libgtu.a
endif

libgtu_a_SOURCES = \
	init.c \
	log.c \
	flags.c \
	path.c \
	object.c \
	test-case/test-case.c \
	test-case/run.c \
	test-suite/test-suite.c \
	test-suite/run.c

libgtu_a_CFLAGS = \
	-I$(top_srcdir)/include \
	$(gtu_U_CFLAGS)

if IS_SUBPROJECT
else
noinst_PROGRAMS = gtu-test

gtu_test_SOURCES = \
	gtu-test.c

gtu_test_CFLAGS = \
	$(libgtu_a_CFLAGS)

gtu_test_LDADD = \
	libgtu.a \
	$(gtu_U_LIBS)
endif

-include $(top_srcdir)/git.mk
