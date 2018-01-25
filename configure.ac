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
"])

PKG_CHECK_MODULES(gtu_U, $GTU_REQUIRED_PACKAGES)
AC_SUBST(gtu_U_REQUIRES, $GTU_REQUIRED_PACKAGES)

m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.9], [--flavour no-tmpl])
AM_COND_IF([ENABLE_GTK_DOC], [AC_PROG_LN_S])
], [
AM_CONDITIONAL([ENABLE_GTK_DOC], [false])
])

AM_COND_IF([ENABLE_GTK_DOC], [AC_CONFIG_FILES([docs/Makefile])])

dnl ###########################################################################
dnl Files to generate
dnl ###########################################################################

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
