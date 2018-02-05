if ENABLE_CHECK_PROGS
noinst_PROGRAMS = testc

testc_SOURCES = \
	testc.c

testc_CFLAGS = \
	-I$(top_srcdir)/include \
	$(gtu_U_CFLAGS)

testc_LDADD = \
	$(top_srcdir)/src/libgtu.a \
	$(gtu_U_LIBS)
endif

-include $(top_srcdir)/git.mk
