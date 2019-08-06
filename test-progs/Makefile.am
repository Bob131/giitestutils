if ENABLE_CHECK_PROGS
noinst_PROGRAMS = testc testvala testempty testemptysuite

testc_SOURCES = \
	testc.c

testc_CFLAGS = \
	-Wall -Wextra -Wpedantic -std=c99 \
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

testempty_SOURCES = \
	testempty.c

testempty_CFLAGS = $(testc_CFLAGS)

testempty_LDADD = $(testc_LDADD)

testemptysuite_SOURCES = \
	testemptysuite.c

testemptysuite_CFLAGS = $(testc_CFLAGS)

testemptysuite_LDADD = $(testc_LDADD)
endif

CLEANFILES = \
	testvala.c \
	testvala_vala.stamp \
	testvala_vala.stamp-t

-include $(top_srcdir)/git.mk
