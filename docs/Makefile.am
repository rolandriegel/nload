## Helder Correia <helder.correia@netcabo.pt>

man1_MAN = nload.1

install:
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) $(top_srcdir)/docs/$(PACKAGE).1 $(DESTDIR)$(mandir)/man1
	gzip -f $(DESTDIR)$(mandir)/man1/$(PACKAGE).1

uninstall:
	rm $(DESTDIR)$(mandir)/man1/$(PACKAGE).1.gz
