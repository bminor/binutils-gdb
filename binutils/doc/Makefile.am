## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = cygnus

# What version of the manual you want; "all" includes everything
CONFIG=all

# Options to extract the man page from as.texinfo
MANCONF = -Dman

TEXI2POD = perl $(top_srcdir)/../etc/texi2pod.pl

POD2MAN = pod2man --center="GNU" --release="binutils-$(VERSION)" --section=1

# List of man pages generated from binutils.texi
man_MANS = \
	addr2line.1 \
	ar.1 \
	dlltool.1 \
	nlmconv.1 \
	nm.1 \
	objcopy.1 \
	objdump.1 \
	ranlib.1 \
	readelf.1 \
	size.1 \
	strings.1 \
	strip.1 \
	windres.1 \
	$(DEMANGLER_NAME).1

info_TEXINFOS = binutils.texi

config.texi: Makefile
	rm -f config.texi
	echo '@set VERSION $(VERSION)' > config.texi

binutils_TEXI = $(srcdir)/binutils.texi

binutils.dvi:  $(binutils_TEXI) config.texi

binutils.info: $(binutils_TEXI) config.texi

# Man page generation from texinfo
addr2line.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Daddr2line < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

ar.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dar < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

dlltool.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Ddlltool < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

nlmconv.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dnlmconv < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

nm.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dnm < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

objcopy.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dobjcopy < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

objdump.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dobjdump < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

ranlib.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dranlib < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

readelf.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dreadelf < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

size.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dsize < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

strings.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dstrings < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

strip.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dstrip < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

windres.1:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dwindres < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

cxxfilt.man:	$(binutils_TEXI)
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dcxxfilt < $< > $@.pod
	-($(POD2MAN) $@.pod | sed -e '/^.if n .na/d' > $@.T$$$$ && \
		mv -f $@.T$$$$ $@) || (rm -f $@.T$$$$ && exit 1)
	rm -f $@.pod

MAINTAINERCLEANFILES = config.texi
DISTCLEANFILES       = config.texi
MOSTLYCLEANFILES     = $(DEMANGLER_NAME).1

$(DEMANGLER_NAME).1: cxxfilt.man Makefile
	sed -e 's/@PROGRAM@/$(DEMANGLER_NAME)/' < cxxfilt.man \
		> $(DEMANGLER_NAME).1

# Maintenance

# We need it for the taz target in ../../Makefile.in.
info: $(MANS)

# Build the man page from the texinfo file
# The sed command removes the no-adjust Nroff command so that
# the man output looks standard.
as.1: $(srcdir)/as.texinfo
	touch $@
	-$(TEXI2POD) $(MANCONF) < $(srcdir)/as.texinfo > as.pod
	-($(POD2MAN) as.pod | \
	        sed -e '/^.if n .na/d' > $@.T$$$$ && \
	        mv -f $@.T$$$$ $@) || \
	        (rm -f $@.T$$$$ && exit 1)
	rm -f as.pod
