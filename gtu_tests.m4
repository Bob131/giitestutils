# SYNOPSIS
#
#   GTU_TESTS(GTU-PATH, [ENABLED-DEFAULT = no])
#
# DESCRIPTION
#
#   This macro allows GTU test suites to be enabled or disabled by users at
#   configure time and defines a handful of conditionals and substitutions to
#   abstract away some of the boilerplate building with GTU might otherwise
#   involve. GTU-PATH should be the path to the GTU submodule, recommended to
#   be in your tests directory.
#
#   This macro adds the --enable-tests configuration flag. The ENABLED-DEFAULT
#   argument to this macro allows the default value to be changed; it's
#   recommended to keep this at the default 'no', so users and/or package
#   building infrastructure aren't required to checkout submodules for a
#   successful build.
#
#   This macro defines the ENABLE_GTU_TESTS conditional, intended to be used
#   for effectively commenting out test functionality in Makefiles.
#
#   Substitutions defined by this macro include:
#     - gtu_U_PATH: A path which should be included in SUBDIRS in your test
#       Makefile.
#
#     - gtu_U_VALAFLAGS: Flags required if you intend to use GTU from Vala
#       code.
#
#     - gtu_U_CFLAGS
#
#     - gtu_U_LIBS: Specifies libraries with which a test program relying on
#       GTU should link (including GTU itself).
#
# LICENSE
#
#   Copyright 2018 George Barrett
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice and
#   this notice are preserved.  This file is offered as-is, without any
#   warranty.

AC_DEFUN([GTU_TESTS],
[
AC_REQUIRE([AM_CONDITIONAL])

AC_MSG_CHECKING([whether to build tests])

AC_SUBST([gtu_U_PATH], ["\[$](top_srcdir)/AS_ESCAPE([$1])"])

m4_pushdef([ENABLED_DEFAULT], [no])
m4_pushdef([ENABLED_CHECK], [test "x$enable_tests" = "xyes"])
m4_cond([$2], [yes], [m4_define([ENABLED_DEFAULT], [yes])
                      m4_define([ENABLED_CHECK],
                                [test "x$enable_tests" != "xno"])])

m4_case(ENABLED_DEFAULT,
        [no],  [AC_ARG_ENABLE([tests],
                AS_HELP_STRING([--enable-tests], [enables the test suite]))],
        [yes], [AC_ARG_ENABLE([tests],
                AS_HELP_STRING([--disable-tests], [disables the test suite]))])

AM_CONDITIONAL([ENABLE_GTU_TESTS], ENABLED_CHECK)

# Make gtu.mk (glib-tap.mk) happy
AM_CONDITIONAL([ENABLE_ALWAYS_BUILD_TESTS], [false])
AM_CONDITIONAL([ENABLE_INSTALLED_TESTS], [false])
AC_SUBST(installed_test_metadir, [])
AC_SUBST(installed_testdir, [])

AS_IF(ENABLED_CHECK, [
  AC_MSG_RESULT([yes])
  AC_REQUIRE([PKG_PROG_PKG_CONFIG])
  PKG_PROG_PKG_CONFIG

  AC_CONFIG_SUBDIRS([$1])

  AC_SUBST([gtu_U_VALAFLAGS], ["--vapidir \[$](gtu_U_PATH)/vapi --pkg gtu"])

  m4_pushdef([GLIB_FLAGS], [gtu_glib_U])
  m4_pushdef([GLIB_CFLAGS], m4_join([], $GLIB_FLAGS, [_CFLAGS]))
  m4_pushdef([GLIB_LIBS],   m4_join([], $GLIB_FLAGS, [_LIBS]))

  PKG_CHECK_MODULES(GLIB_FLAGS, [glib-2.0 gobject-2.0])

  AS_VAR_SET([gtu_U_CFLAGS], GLIB_CFLAGS)
  AS_VAR_SET([gtu_U_LIBS], GLIB_LIBS)
  AS_VAR_APPEND([gtu_U_CFLAGS], [" -I\[$](gtu_U_PATH)/include"])
  AS_VAR_APPEND([gtu_U_LIBS], [" \[$](gtu_U_PATH)/src/libgtu.a"])
  AC_SUBST([gtu_U_CFLAGS])
  AC_SUBST([gtu_U_LIBS])

  m4_popdef([GLIB_FLAGS])
  m4_popdef([GLIB_CFLAGS])
  m4_popdef([GLIB_LIBS])

  AC_REQUIRE_AUX_FILE([tap-driver.sh])
], [
  AC_MSG_RESULT([no])
])

m4_popdef([ENABLED_DEFAULT])
m4_popdef([ENABLED_CHECK])
])
