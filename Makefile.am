## Helder Correia <helder.correia@netcabo.pt>

SUBDIRS = docs src
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = AUTHORS ChangeLog COPYING INSTALL README nload.spec.in

install:
	$(mkinstalldirs) $(prefix)/share/man/man1
	$(INSTALL_DATA) $(top_srcdir)/docs/$(PACKAGE).1 $(prefix)/share/man/man1

uninstall:
	rm $(prefix)/share/man/man1/$(PACKAGE).1
