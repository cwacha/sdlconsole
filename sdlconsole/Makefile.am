LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
SUBDIRS = include src example

EXTRA_DIST = \
	Doxyfile \
	makefile.VC6 \
	makefile.OSX 

#Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

