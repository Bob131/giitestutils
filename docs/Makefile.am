DOC_MODULE = gtu

DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

DOC_SOURCE_DIR = $(top_srcdir)/include

SCAN_OPTIONS = --rebuild-types

MKDB_OPTIONS = --xml-mode --output-format=xml

HFILE_GLOB = $(top_srcdir)/include/gtu*.h

md2db_files = \
	considerations/gfuncs-okay.xml \
	considerations/gfuncs-avoid.xml \
	considerations/logging.xml

content_files = \
	gtu.css.xml \
	intro.xml \
	getting-started.xml \
	running-tests/main.xml \
	running-tests/mode-list.xml \
	running-tests/args.xml \
	running-tests/make-vars.xml \
	running-tests/env-vars.xml \
	considerations/considerations.xml \
	considerations/memory-leaks.xml \
	$(md2db_files)

expand_content_files = \
	intro.xml \
	getting-started.xml \
	considerations/considerations.xml \
	considerations/memory-leaks.xml

GTKDOC_CFLAGS = -I$(top_srcdir)/include
GTKDOC_LIBS = $(top_srcdir)/src/libgtu.a $(gtu_U_LIBS)


if ENABLE_GTK_DOC
include $(top_srcdir)/gtk-doc.make


TESTS_ENVIRONMENT = \
	DOC_MODULE=$(DOC_MODULE) DOC_MAIN_SGML_FILE=$(DOC_MAIN_SGML_FILE) \
	SRCDIR=$(abs_srcdir) BUILDDIR=$(abs_builddir)
TESTS = $(GTKDOC_CHECK)


# since we don't make versioned releases, we want commit info in the docs
sgml-build.stamp: xml/version.ent

# we add a symbolic link to version.ent since gtkdoc-mkdb blindly copies entity
# declarations to files in different directories without path fixups
xml/version.ent:
	$(AM_V_GEN) if git rev-parse --verify HEAD >/dev/null 2>/dev/null; then \
		( \
			echo "<!ENTITY package_git_version \"$(shell git describe --abbrev=7 --always --dirty)\">"; \
			echo "<!ENTITY package_git_commit \"$(shell git rev-parse --verify HEAD)\">"; \
			echo "<!ENTITY package_git_date \"$(shell git log -1 --format=%cd --date=short)\">"; \
		) > $@; \
	else \
		( \
			echo "<!ENTITY package_git_version \"&package_version;\">"; \
			echo "<!ENTITY package_git_commit \"HEAD\">"; \
			echo "<!ENTITY package_git_date \"$(shell date +%Y-%m-%d)\">"; \
		) > $@; \
	fi; \
	$(LN_S) $@ version.ent 2>/dev/null || true

# TODO: find a way to rebuild only when the git index has changed
.PHONY: xml/version.ent

CLEANFILES += xml/version.ent version.ent


gtkdoc-config.py.tmp:
	$(AM_V_GEN) ( \
		echo '#!/usr/bin/env python3' > $@; \
		echo 'import sys' >> $@; \
		grep sys.path.append $(GTKDOC_CHECK_PATH) >> $@; \
	)

gtkdoc-md2db.py: gtkdoc-md2db.py.in gtkdoc-config.py.tmp
	$(AM_V_GEN) ( \
		cat gtkdoc-config.py.tmp > $@; \
		cat $< >> $@; \
		chmod +x $@; \
	)

$(md2db_files): %.xml: %.md gtkdoc-md2db.py
	$(AM_V_GEN) ./gtkdoc-md2db.py $< > $@; cp $@ xml/

CLEANFILES += $(md2db_files) gtkdoc-md2db.py gtkdoc-config.py.tmp


html-build.stamp: gtk-doc.xsl style

if GTK_DOC_BUILD_HTML
gtkdoc-xslpath.py: gtkdoc-xslpath.py.in gtkdoc-config.py.tmp
	$(AM_V_GEN) ( \
		cat gtkdoc-config.py.tmp > $@; \
		cat $< >> $@; \
		chmod +x $@; \
	)

gtk-doc.xsl: gtk-doc.xsl.in gtkdoc-xslpath.py
	$(AM_V_GEN) sed "s|[@]GTKDOC_XSL[@]|$(shell ./gtkdoc-xslpath.py)/gtk-doc.xsl|g" < $< > $@

style: gtkdoc-xslpath.py
	$(AM_V_GEN) rm -f $@; $(LN_S) $(shell ./gtkdoc-xslpath.py) $@

CLEANFILES += gtkdoc-xslpath.py gtk-doc.xsl style


# we can't use gtkdoc-rebase because it's enormously broken
docs-rebase: html-build.stamp
	-sed -i -e "s|$(shell grep -Poh "(?<=href=\")([^\"]+gtk-doc/html/glib)" html/*.html | head -n1)|https://developer.gnome.org/glib/stable|g" html/*.html
	-sed -i -e "s|$(shell grep -Poh "(?<=href=\")([^\"]+gtk-doc/html/gobject)" html/*.html | head -n1)|https://developer.gnome.org/gobject/stable|g" html/*.html
endif


CLEANFILES += gtu-sections.txt gtu-overrides.txt
endif


-include $(top_srcdir)/git.mk
