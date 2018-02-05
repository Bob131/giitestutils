AM_CPPFLAGS = -Wall -Wextra -std=c99 -Wpedantic

SUBDIRS = . gtu-test

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

-include $(top_srcdir)/git.mk
