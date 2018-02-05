if ENABLE_CHECK_PROGS
noinst_PROGRAMS = testc testvala

testc_SOURCES = \
	testc.c

testc_CFLAGS = \
	-I$(top_srcdir)/include \
	$(gtu_U_CFLAGS)

testc_LDADD = \
	$(top_srcdir)/src/libgtu.a \
	$(gtu_U_LIBS)

testvala_SOURCES = \
	testvala.vala \
	$(top_srcdir)/vapi/gtu.vapi

testvala_CFLAGS = \
	-Wall -Wextra -Wpedantic -std=c99 \
	-I$(top_srcdir)/include \
	$(gtu_U_CFLAGS)

testvala_LDADD = \
	$(top_srcdir)/src/libgtu.a \
	$(gtu_U_LIBS)
endif

CLEANFILES = \
	testvala.c \
	testvala_vala.stamp \
	testvala_vala.stamp-t

-include $(top_srcdir)/git.mk
