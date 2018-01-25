DOC_MODULE = gtu

DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

DOC_SOURCE_DIR = $(top_srcdir)/include

SCAN_OPTIONS = --rebuild-types

MKDB_OPTIONS = --xml-mode --output-format=xml

HFILE_GLOB = $(top_srcdir)/include/gtu*.h

content_files = \
	intro.xml \
	getting-started.xml \
	running-tests.xml

if ENABLE_GTK_DOC
include $(top_srcdir)/gtk-doc.make

TESTS_ENVIRONMENT = \
	DOC_MODULE=$(DOC_MODULE) DOC_MAIN_SGML_FILE=$(DOC_MAIN_SGML_FILE) \
	SRCDIR=$(abs_srcdir) BUILDDIR=$(abs_builddir)
TESTS = $(GTKDOC_CHECK)

if GTK_DOC_BUILD_HTML
# we can't use gtkdoc-rebase because it's enormously broken
docs-rebase: html-build.stamp
	-sed -i -e "s|$(shell grep -Poh "(?<=href=\")([^\"]+gtk-doc/html/glib)" html/*.html | head -n1)|https://developer.gnome.org/glib/stable|g" html/*.html
	-sed -i -e "s|$(shell grep -Poh "(?<=href=\")([^\"]+gtk-doc/html/gobject)" html/*.html | head -n1)|https://developer.gnome.org/gobject/stable|g" html/*.html
endif

CLEANFILES += gtu-sections.txt gtu-overrides.txt
endif

-include $(top_srcdir)/git.mk
