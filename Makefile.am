## Helder Correia <helder.correia@netcabo.pt>

SUBDIRS = docs src
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = AUTHORS ChangeLog COPYING INSTALL README nload.spec.in

install:
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) $(top_srcdir)/docs/$(PACKAGE).1 $(DESTDIR)$(mandir)/man1
	gzip -f $(DESTDIR)$(mandir)/man1/$(PACKAGE).1

uninstall:
	rm $(DESTDIR)$(mandir)/man1/$(PACKAGE).1.gz
