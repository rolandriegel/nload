## Helder Correia <helder.correia@netcabo.pt>

SUBDIRS = docs src
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = AUTHORS ChangeLog COPYING INSTALL README nload.spec.in

install:
	$(mkinstalldirs) $(DESTDIR)$(prefix)/man/man1
	$(INSTALL_DATA) $(top_srcdir)/docs/$(PACKAGE).1 $(DESTDIR)$(prefix)/man/man1
	gzip -f $(DESTDIR)$(prefix)/man/man1/$(PACKAGE).1

uninstall:
	rm $(DESTDIR)$(prefix)/man/man1/$(PACKAGE).1.gz
