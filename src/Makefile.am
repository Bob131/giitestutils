AM_CPPFLAGS = -Wall -Wextra -std=c99 -Wpedantic

if IS_SUBPROJECT
check_LIBRARIES = libgtu.a
else
noinst_LIBRARIES = libgtu.a
endif

BUILT_SOURCES = log/color-table.c.in

log/color-table.c.in:
	$(AM_V_GEN) log/generate-color-table.py > $@

libgtu_a_SOURCES = \
	init.c \
	flags.c \
	path.c \
	object.c \
	log/tap.c \
	log/color.c \
	log/glib-handlers.c \
	log/glib-format.c \
	test-case/test-case.c \
	test-case/run.c \
	test-suite/test-suite.c \
	test-suite/run.c

libgtu_a_CFLAGS = \
	-I$(top_srcdir)/include \
	-DG_LOG_DOMAIN=GTU_LOG_DOMAIN \
	$(gtu_U_CFLAGS)

-include $(top_srcdir)/git.mk
