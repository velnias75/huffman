DISTCLEANFILES = vowellish.dot
EXTRA_DIST = README.md

doc_DATA = AUTHORS $(EXTRA_DIST) $(DISTCLEANFILES)

SUBDIRS = src

vowellish.dot:
	echo -n "aaaaaaaaaaaaeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeiiiiiiiiioooooooooooooooooooooooooooooouuuuuuu" \
		| $(top_builddir)/src/huffdot > $(top_srcdir)/$@
