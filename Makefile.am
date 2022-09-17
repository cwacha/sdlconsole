LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
SUBDIRS = include src example

EXTRA_DIST = \
	makefile.VC6 \
	Makefile.OSX \
	autogen.sh \
	autoclean.sh \
	README_WIN.txt \
	docs \
	devcpp \
	.vscode

#Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

