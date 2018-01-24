DOC_MODULE = gtu

DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

DOC_SOURCE_DIR = $(top_srcdir)/include

HFILE_GLOB = $(top_srcdir)/include/gtu*.h

SCAN_OPTIONS = --rebuild-types

if ENABLE_GTK_DOC
include $(top_srcdir)/gtk-doc.make

TESTS_ENVIRONMENT = \
	DOC_MODULE=$(DOC_MODULE) DOC_MAIN_SGML_FILE=$(DOC_MAIN_SGML_FILE) \
	SRCDIR=$(abs_srcdir) BUILDDIR=$(abs_builddir)
TESTS = $(GTKDOC_CHECK)

CLEANFILES += gtu-sections.txt gtu-overrides.txt
endif

-include $(top_srcdir)/git.mk
