AC_INIT([giitestutils], [0.0])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects no-dist])
AM_SILENT_RULES([yes])

AM_PROG_CC_C_O
AC_PROG_RANLIB

dnl ###########################################################################
dnl Dependencies
dnl ###########################################################################

AC_FUNC_ALLOCA

GLIB_REQUIRED=2.40.0

GTU_REQUIRED_PACKAGES=m4_flatten(["
  glib-2.0 >= $GLIB_REQUIRED
  gobject-2.0 >= $GLIB_REQUIRED
  libunwind
"])

PKG_CHECK_MODULES(gtu_U, $GTU_REQUIRED_PACKAGES)
AC_SUBST(gtu_U_REQUIRES, $GTU_REQUIRED_PACKAGES)

AC_ARG_ENABLE([tests],
              AS_HELP_STRING([--enable-tests],
                             [used for testing subproject builds. Do not use]))

dnl We can't make this work if tests are enabled by default. We just hope that
dnl no user
dnl     (a) enables tests by default; and
dnl     (b) uses gtkdoc/can't afford to build gtu-test
dnl at the same time.
AM_CONDITIONAL([IS_SUBPROJECT], [test "x$enable_tests" = "xyes"])

AM_COND_IF([IS_SUBPROJECT], [
dnl Only consider building documentation if we're not part of a superproject.
  AM_CONDITIONAL([ENABLE_GTK_DOC],      [false])
  AM_CONDITIONAL([HAVE_GTK_DOC],        [false])
  AM_CONDITIONAL([GTK_DOC_BUILD_HTML],  [false])
  AM_CONDITIONAL([GTK_DOC_BUILD_PDF],   [false])
  AM_CONDITIONAL([GTK_DOC_USE_LIBTOOL], [false])
  AM_CONDITIONAL([GTK_DOC_USE_REBASE],  [false])

dnl Ditto for check programs
  AM_CONDITIONAL([ENABLE_CHECK_PROGS], [false])
], [
  m4_ifdef([GTK_DOC_CHECK], [GTK_DOC_CHECK([1.9], [--flavour no-tmpl])])
  AM_COND_IF([ENABLE_GTK_DOC], [AC_PROG_LN_S])

  AC_ARG_ENABLE([check-progs],
                AS_HELP_STRING([--disable-check-progs]),
                ["don't build sanity check binaries"])
  AM_CONDITIONAL([ENABLE_CHECK_PROGS], [test "x$enable_check_progs" != "xno"])
  AM_COND_IF([ENABLE_CHECK_PROGS], [AM_PROG_VALAC])
])

dnl ###########################################################################
dnl Files to generate
dnl ###########################################################################

AC_CONFIG_FILES([
Makefile
src/Makefile
src/gtu-test/Makefile
docs/Makefile
])
AC_OUTPUT
