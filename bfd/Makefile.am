## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = 1.9 cygnus
ACLOCAL_AMFLAGS = -I . -I .. -I ../config

# Uncomment the following line when doing a release.
# RELEASE=y

INCDIR = $(srcdir)/../include
CSEARCH = -I. -I$(srcdir) -I$(INCDIR)
MKDEP = gcc -MM

SUBDIRS = doc po

bfddocdir = doc
bfdlibdir = @bfdlibdir@
bfdincludedir = @bfdincludedir@

datarootdir = @datarootdir@
docdir = @docdir@
htmldir = @htmldir@
pdfdir = @pdfdir@

bfdlib_LTLIBRARIES = libbfd.la

WARN_CFLAGS = @WARN_CFLAGS@
NO_WERROR = @NO_WERROR@
AM_CFLAGS = $(WARN_CFLAGS)
AM_CPPFLAGS = -DBINDIR='"$(bindir)"'
if PLUGINS
LIBDL = -ldl
endif

# bfd.h goes here, for now
BFD_H = bfd.h

# Jim Kingdon notes:
# Writing S-records should be included in all (or at least most)
# *-*-coff, *-*-aout, etc., configurations, because people will want to
# be able to use objcopy to create S-records.  (S-records are not useful
# for the debugger, so if you are downloading things as S-records you
# need two copies of the executable, one to download and one for the
# debugger).
BFD32_LIBS = \
	archive.lo archures.lo bfd.lo bfdio.lo bfdwin.lo \
	cache.lo coffgen.lo corefile.lo \
	format.lo init.lo libbfd.lo opncls.lo reloc.lo \
	section.lo syms.lo targets.lo hash.lo linker.lo \
	srec.lo binary.lo tekhex.lo ihex.lo stabs.lo stab-syms.lo \
	merge.lo dwarf2.lo simple.lo compress.lo verilog.lo

BFD64_LIBS = archive64.lo

BFD32_LIBS_CFILES = \
	archive.c archures.c bfd.c bfdio.c bfdwin.c \
	cache.c coffgen.c corefile.c \
	format.c init.c libbfd.c opncls.c reloc.c \
	section.c syms.c targets.c hash.c linker.c \
	srec.c binary.c tekhex.c ihex.c stabs.c stab-syms.c \
	merge.c dwarf2.c simple.c compress.c verilog.c

BFD64_LIBS_CFILES = archive64.c

# This list is alphabetized to make it easier to keep in sync
# with the decls and initializer in archures.c.
ALL_MACHINES = \
	cpu-alpha.lo \
	cpu-arc.lo \
	cpu-arm.lo \
	cpu-avr.lo \
	cpu-bfin.lo \
	cpu-cr16.lo \
	cpu-cr16c.lo \
	cpu-cris.lo \
	cpu-crx.lo \
	cpu-d10v.lo \
	cpu-d30v.lo \
	cpu-dlx.lo \
	cpu-fr30.lo \
	cpu-frv.lo \
	cpu-h8300.lo \
	cpu-h8500.lo \
	cpu-hppa.lo \
	cpu-i370.lo \
	cpu-i386.lo \
	cpu-i860.lo \
	cpu-i960.lo \
	cpu-ia64.lo \
	cpu-ip2k.lo \
	cpu-iq2000.lo \
	cpu-lm32.lo \
	cpu-m10200.lo \
	cpu-m10300.lo \
	cpu-m32c.lo \
	cpu-m32r.lo \
	cpu-m68hc11.lo \
	cpu-m68hc12.lo \
	cpu-m68k.lo \
	cpu-m88k.lo \
	cpu-maxq.lo \
	cpu-mcore.lo \
	cpu-mep.lo \
	cpu-mips.lo \
	cpu-mmix.lo \
	cpu-moxie.lo \
	cpu-msp430.lo \
	cpu-mt.lo \
	cpu-ns32k.lo \
	cpu-openrisc.lo \
	cpu-or32.lo \
	cpu-pdp11.lo \
	cpu-pj.lo \
	cpu-plugin.lo \
	cpu-powerpc.lo \
	cpu-rs6000.lo \
	cpu-s390.lo \
	cpu-score.lo \
	cpu-sh.lo \
	cpu-sparc.lo \
	cpu-spu.lo \
	cpu-tic30.lo \
	cpu-tic4x.lo \
	cpu-tic54x.lo \
	cpu-tic80.lo \
	cpu-v850.lo \
	cpu-vax.lo \
	cpu-w65.lo \
	cpu-we32k.lo \
	cpu-xc16x.lo \
	cpu-xstormy16.lo \
	cpu-xtensa.lo \
	cpu-z80.lo \
	cpu-z8k.lo

ALL_MACHINES_CFILES = \
	cpu-alpha.c \
	cpu-arc.c \
	cpu-arm.c \
	cpu-avr.c \
	cpu-bfin.c \
	cpu-cr16.c \
	cpu-cr16c.c \
	cpu-cris.c \
	cpu-crx.c \
	cpu-d10v.c \
	cpu-d30v.c \
	cpu-dlx.c \
	cpu-fr30.c \
	cpu-frv.c \
	cpu-h8300.c \
	cpu-h8500.c \
	cpu-hppa.c \
	cpu-i370.c \
	cpu-i386.c \
	cpu-i860.c \
	cpu-i960.c \
	cpu-ia64.c \
	cpu-ip2k.c \
	cpu-iq2000.c \
	cpu-lm32.c \
	cpu-m10200.c \
	cpu-m10300.c \
	cpu-m32c.c \
	cpu-m32r.c \
	cpu-m68hc11.c \
	cpu-m68hc12.c \
	cpu-m68k.c \
	cpu-m88k.c \
	cpu-maxq.c \
	cpu-mcore.c \
	cpu-mep.c \
	cpu-mips.c \
	cpu-mmix.c \
	cpu-moxie.c \
	cpu-msp430.c \
	cpu-mt.c \
	cpu-ns32k.c \
	cpu-openrisc.c \
	cpu-or32.c \
	cpu-pdp11.c \
	cpu-pj.c \
	cpu-plugin.c \
	cpu-powerpc.c \
	cpu-rs6000.c \
	cpu-s390.c \
	cpu-score.c \
	cpu-sh.c \
	cpu-sparc.c \
	cpu-spu.c \
	cpu-tic30.c \
	cpu-tic4x.c \
	cpu-tic54x.c \
	cpu-tic80.c \
	cpu-v850.c \
	cpu-vax.c \
	cpu-w65.c \
	cpu-we32k.c \
	cpu-xc16x.c \
	cpu-xstormy16.c \
	cpu-xtensa.c \
	cpu-z80.c \
	cpu-z8k.c

# The .o files needed by all of the 32 bit vectors that are configured into
# target_vector in targets.c if configured with --enable-targets=all.
BFD32_BACKENDS = \
	aout-adobe.lo \
	aout-arm.lo \
	aout-cris.lo \
	aout-ns32k.lo \
	aout-sparcle.lo \
	aout-tic30.lo \
	aout0.lo \
	aout32.lo \
	armnetbsd.lo \
	bout.lo \
	cf-i386lynx.lo \
	cf-sparclynx.lo \
	coff-apollo.lo \
	coff-arm.lo \
	coff-aux.lo \
	coff-go32.lo \
	coff-h8300.lo \
	coff-h8500.lo \
	coff-i386.lo \
	coff-i860.lo \
	coff-i960.lo \
	coff-m68k.lo \
	coff-m88k.lo \
	coff-maxq.lo \
	coff-mips.lo \
	coff-or32.lo \
	coff-rs6000.lo \
	coff-sh.lo \
	coff-sparc.lo \
	coff-stgo32.lo \
	coff-svm68k.lo \
	coff-tic30.lo \
	coff-tic4x.lo \
	coff-tic54x.lo \
	coff-tic80.lo \
	coff-u68k.lo \
	coff-w65.lo \
	coff-we32k.lo \
	coff-z80.lo \
	coff-z8k.lo \
	cofflink.lo \
	dwarf1.lo \
	ecoff.lo \
	ecofflink.lo \
	elf-attrs.lo \
	elf-eh-frame.lo \
	elf-m10200.lo \
	elf-m10300.lo \
	elf-strtab.lo \
	elf-vxworks.lo \
	elf.lo \
	elf32-am33lin.lo \
	elf32-arc.lo \
	elf32-arm.lo \
	elf32-avr.lo \
	elf32-bfin.lo \
	elf32-cr16.lo \
	elf32-cr16c.lo \
	elf32-cris.lo \
	elf32-crx.lo \
	elf32-d10v.lo \
	elf32-d30v.lo \
	elf32-dlx.lo \
	elf32-fr30.lo \
	elf32-frv.lo \
	elf32-gen.lo \
	elf32-h8300.lo \
	elf32-hppa.lo \
	elf32-i370.lo \
	elf32-i386.lo \
	elf32-i860.lo \
	elf32-i960.lo \
	elf32-ip2k.lo \
	elf32-iq2000.lo \
	elf32-lm32.lo \
	elf32-m32c.lo \
	elf32-m32r.lo \
	elf32-m68hc11.lo \
	elf32-m68hc12.lo \
	elf32-m68hc1x.lo \
	elf32-m68k.lo \
	elf32-m88k.lo \
	elf32-mcore.lo \
	elf32-mep.lo \
	elf32-mips.lo \
	elf32-moxie.lo \
	elf32-msp430.lo \
	elf32-mt.lo \
	elf32-openrisc.lo \
	elf32-or32.lo \
	elf32-pj.lo \
	elf32-ppc.lo \
	elf32-s390.lo \
	elf32-sh-symbian.lo \
	elf32-sh.lo \
	elf32-sh64-com.lo \
	elf32-sh64.lo \
	elf32-sparc.lo \
	elf32-spu.lo \
	elf32-v850.lo \
	elf32-vax.lo \
	elf32-xc16x.lo \
	elf32-xstormy16.lo \
	elf32-xtensa.lo \
	elf32.lo \
	elflink.lo \
	elfxx-mips.lo \
	elfxx-sparc.lo \
	epoc-pe-arm.lo \
	epoc-pei-arm.lo \
	hp300bsd.lo \
	hp300hpux.lo \
	i386aout.lo \
	i386bsd.lo \
	i386dynix.lo \
	i386freebsd.lo \
	i386linux.lo \
	i386lynx.lo \
	i386mach3.lo \
	i386msdos.lo \
	i386netbsd.lo \
	i386os9k.lo \
	ieee.lo \
	m68k4knetbsd.lo \
	m68klinux.lo \
	m68knetbsd.lo \
	m88kmach3.lo \
	m88kopenbsd.lo \
	mach-o.lo \
	mach-o-i386.lo \
	mipsbsd.lo \
	newsos3.lo \
	nlm.lo \
	nlm32-i386.lo \
	nlm32-ppc.lo \
	nlm32-sparc.lo \
	nlm32.lo \
	ns32knetbsd.lo \
	oasys.lo \
	pc532-mach.lo \
	pdp11.lo \
	pe-arm-wince.lo \
	pe-arm.lo \
	pe-i386.lo \
	pe-mcore.lo \
	pe-mips.lo \
	pe-ppc.lo \
	pe-sh.lo \
	pef.lo \
	pei-arm-wince.lo \
	pei-arm.lo \
	pei-i386.lo \
	pei-mcore.lo \
	pei-mips.lo \
	pei-ppc.lo \
	pei-sh.lo \
	peigen.lo \
	plugin.lo \
	ppcboot.lo \
	reloc16.lo \
	riscix.lo \
	som.lo \
	sparclinux.lo \
	sparclynx.lo \
	sparcnetbsd.lo \
	sunos.lo \
	vax1knetbsd.lo \
	vaxbsd.lo \
	vaxnetbsd.lo \
	versados.lo \
	vms-gsd.lo \
	vms-hdr.lo \
	vms-misc.lo \
	vms-tir.lo \
	vms.lo \
	vmsutil.lo \
	xcofflink.lo \
	xsym.lo \
	xtensa-isa.lo \
	xtensa-modules.lo

BFD32_BACKENDS_CFILES = \
	aout-adobe.c \
	aout-arm.c \
	aout-cris.c \
	aout-ns32k.c \
	aout-sparcle.c \
	aout-tic30.c \
	aout0.c \
	aout32.c \
	armnetbsd.c \
	bout.c \
	cf-i386lynx.c \
	cf-sparclynx.c \
	coff-apollo.c \
	coff-arm.c \
	coff-aux.c \
	coff-go32.c \
	coff-h8300.c \
	coff-h8500.c \
	coff-i386.c \
	coff-i860.c \
	coff-i960.c \
	coff-m68k.c \
	coff-m88k.c \
	coff-maxq.c \
	coff-mips.c \
	coff-or32.c \
	coff-rs6000.c \
	coff-sh.c \
	coff-sparc.c \
	coff-stgo32.c \
	coff-svm68k.c \
	coff-tic30.c \
	coff-tic4x.c \
	coff-tic54x.c \
	coff-tic80.c \
	coff-u68k.c \
	coff-w65.c \
	coff-we32k.c \
	coff-z80.c \
	coff-z8k.c \
	cofflink.c \
	dwarf1.c \
	ecoff.c \
	ecofflink.c \
	elf-attrs.c \
	elf-eh-frame.c \
	elf-m10200.c \
	elf-m10300.c \
	elf-strtab.c \
	elf-vxworks.c \
	elf.c \
	elf32-am33lin.c \
	elf32-arc.c \
	elf32-arm.c \
	elf32-avr.c \
	elf32-bfin.c \
	elf32-cr16.c \
	elf32-cr16c.c \
	elf32-cris.c \
	elf32-crx.c \
	elf32-d10v.c \
	elf32-d30v.c \
	elf32-dlx.c \
	elf32-fr30.c \
	elf32-frv.c \
	elf32-gen.c \
	elf32-h8300.c \
	elf32-hppa.c \
	elf32-i370.c \
	elf32-i386.c \
	elf32-i860.c \
	elf32-i960.c \
	elf32-ip2k.c \
	elf32-iq2000.c \
	elf32-lm32.c \
	elf32-m32c.c \
	elf32-m32r.c \
	elf32-m68hc11.c \
	elf32-m68hc12.c \
	elf32-m68hc1x.c \
	elf32-m68k.c \
	elf32-m88k.c \
	elf32-mcore.c \
	elf32-mep.c \
	elf32-mips.c \
	elf32-moxie.c \
	elf32-msp430.c \
	elf32-mt.c \
	elf32-openrisc.c \
	elf32-or32.c \
	elf32-pj.c \
	elf32-ppc.c \
	elf32-s390.c \
	elf32-sh-symbian.c \
	elf32-sh.c \
	elf32-sh64-com.c \
	elf32-sh64.c \
	elf32-sparc.c \
	elf32-spu.c \
	elf32-v850.c \
	elf32-vax.c \
	elf32-xc16x.c \
	elf32-xstormy16.c \
	elf32-xtensa.c \
	elf32.c \
	elflink.c \
	elfxx-mips.c \
	elfxx-sparc.c \
	epoc-pe-arm.c \
	epoc-pei-arm.c \
	hp300bsd.c \
	hp300hpux.c \
	i386aout.c \
	i386bsd.c \
	i386dynix.c \
	i386freebsd.c \
	i386linux.c \
	i386lynx.c \
	i386mach3.c \
	i386msdos.c \
	i386netbsd.c \
	i386os9k.c \
	ieee.c \
	m68k4knetbsd.c \
	m68klinux.c \
	m68knetbsd.c \
	m88kmach3.c \
	m88kopenbsd.c \
	mach-o.c \
	mach-o-i386.c \
	mipsbsd.c \
	newsos3.c \
	nlm.c \
	nlm32-i386.c \
	nlm32-ppc.c \
	nlm32-sparc.c \
	nlm32.c \
	ns32knetbsd.c \
	oasys.c \
	pc532-mach.c \
	pdp11.c \
	pe-arm-wince.c \
	pe-arm.c \
	pe-i386.c \
	pe-mcore.c \
	pe-mips.c \
	pe-ppc.c \
	pe-sh.c \
	pef.c \
	pei-arm-wince.c \
	pei-arm.c \
	pei-i386.c \
	pei-mcore.c \
	pei-mips.c \
	pei-ppc.c \
	pei-sh.c \
	plugin.c \
	ppcboot.c \
	reloc16.c \
	riscix.c \
	som.c \
	sparclinux.c \
	sparclynx.c \
	sparcnetbsd.c \
	sunos.c \
	vax1knetbsd.c \
	vaxbsd.c \
	vaxnetbsd.c \
	versados.c \
	vms-gsd.c \
	vms-hdr.c \
	vms-misc.c \
	vms-tir.c \
	vms.c \
	vmsutil.c \
	xcofflink.c \
	xsym.c \
	xtensa-isa.c \
	xtensa-modules.c

# The .o files needed by all of the 64 bit vectors that are configured into
# target_vector in targets.c if configured with --enable-targets=all
# and --enable-64-bit-bfd.
# elf32-ia64.c requires a 64-bit bfd_vma, and hence can not be put in
# BFD32_BACKENDS.
BFD64_BACKENDS = \
	aix5ppc-core.lo \
	aout64.lo \
	coff-alpha.lo \
	coff-x86_64.lo \
	coff64-rs6000.lo \
	demo64.lo \
	elf32-ia64.lo \
	elf32-score.lo \
	elf32-score7.lo \
	elf64-alpha.lo \
	elf64-gen.lo \
	elf64-hppa.lo \
	elf64-ia64.lo \
	elf64-mips.lo \
	elf64-mmix.lo \
	elf64-ppc.lo \
	elf64-s390.lo \
	elf64-sh64.lo \
	elf64-sparc.lo \
	elf64-x86-64.lo \
	elf64.lo \
	elfn32-mips.lo \
	mmo.lo \
	nlm32-alpha.lo \
	nlm64.lo \
	pe-x86_64.lo \
	pei-ia64.lo \
	pei-x86_64.lo \
	pepigen.lo \
	pex64igen.lo

BFD64_BACKENDS_CFILES = \
	aix5ppc-core.c \
	aout64.c \
	coff-alpha.c \
	coff-x86_64.c \
	coff64-rs6000.c \
	demo64.c \
	elf32-score.c \
	elf32-score7.c \
	elf64-alpha.c \
	elf64-gen.c \
	elf64-hppa.c \
	elf64-mips.c \
	elf64-mmix.c \
	elf64-ppc.c \
	elf64-s390.c \
	elf64-sh64.c \
	elf64-sparc.c \
	elf64-x86-64.c \
	elf64.c \
	elfn32-mips.c \
	mmo.c \
	nlm32-alpha.c \
	nlm64.c \
	pe-x86_64.c \
	pei-ia64.c \
	pei-x86_64.c

OPTIONAL_BACKENDS = \
	aix386-core.lo \
	cisco-core.lo \
	hpux-core.lo \
	irix-core.lo \
	lynx-core.lo \
	osf-core.lo \
	sco5-core.lo \
	trad-core.lo

OPTIONAL_BACKENDS_CFILES = \
	aix386-core.c \
	cisco-core.c \
	hpux-core.c \
	irix-core.c \
	lynx-core.c \
	osf-core.c \
	sco5-core.c \
	trad-core.c

# We want to rerun configure if configure.in, config.bfd or
# configure.host change. configure.in is needed since the version
# number in Makefile comes from configure.in.
CONFIG_STATUS_DEPENDENCIES = \
	$(srcdir)/configure.in \
	$(srcdir)/config.bfd \
	$(srcdir)/configure.host

# These are defined by configure.in:
WORDSIZE = @wordsize@
ALL_BACKENDS = @all_backends@
BFD_BACKENDS = @bfd_backends@
BFD_MACHINES = @bfd_machines@
TDEFAULTS = @tdefaults@

INCLUDES = @HDEFINES@ @COREFLAG@ @TDEFINES@ $(CSEARCH) $(CSWITCHES) @INCINTL@

# C source files that correspond to .o's.
SOURCE_CFILES = \
	$(BFD32_LIBS_CFILES) \
	$(BFD64_LIBS_CFILES) \
	$(ALL_MACHINES_CFILES) \
	$(BFD32_BACKENDS_CFILES) \
	$(BFD64_BACKENDS_CFILES) \
	$(OPTIONAL_BACKENDS_CFILES)

BUILD_CFILES = \
	elf32-ia64.c elf64-ia64.c peigen.c pepigen.c pex64igen.c

CFILES = $(SOURCE_CFILES) $(BUILD_CFILES)

## This is a list of all .h files which are in the source tree.
SOURCE_HFILES = \
	aout-target.h aoutf1.h aoutx.h coffcode.h coffswap.h ecoffswap.h \
	elf-bfd.h elf-hppa.h elf32-hppa.h \
	elf64-hppa.h elfcode.h elfcore.h \
	freebsd.h genlink.h go32stub.h \
	libaout.h libbfd.h libcoff.h libecoff.h libhppa.h libieee.h \
	libnlm.h liboasys.h libpei.h libxcoff.h mach-o.h \
	netbsd.h nlm-target.h nlmcode.h nlmswap.h ns32k.h \
	pef.h pef-traceback.h peicode.h som.h version.h \
	vms.h xcoff-target.h xsym.h

## ... and all .h files which are in the build tree.
BUILD_HFILES = \
	bfdver.h elf32-target.h elf64-target.h targmatch.h bfd_stdint.h

HFILES = $(SOURCE_HFILES) $(BUILD_HFILES)

SRC_POTFILES = $(SOURCE_CFILES) $(SOURCE_HFILES)
BLD_POTFILES = $(BUILD_CFILES) $(BUILD_HFILES)

po/SRC-POTFILES.in: @MAINT@ Makefile $(SRC_POTFILES)
	for file in $(SRC_POTFILES); do echo $$file; done \
	  | LC_ALL=C sort > tmp.src \
	  && mv tmp.src $(srcdir)/po/SRC-POTFILES.in

po/BLD-POTFILES.in: @MAINT@ Makefile $(BLD_POTFILES)
	for file in $(BLD_POTFILES); do echo $$file; done \
	  | LC_ALL=C sort > tmp.bld \
	  && mv tmp.bld $(srcdir)/po/BLD-POTFILES.in

all diststuff: info

.PHONY: install-pdf install-pdf-am install-pdf-recursive

install-pdf:	install-pdf-recursive

install-pdf-recursive:
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	  || eval $$failcom; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

.PHONY: install-html install-html-am install-html-recursive

install-html:	install-html-recursive

install-html-recursive:
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	  || eval $$failcom; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

# Various kinds of .o files to put in libbfd.a:
# BFD_BACKENDS	Routines the configured targets need.
# BFD_MACHINES	Architecture-specific routines the configured targets need.
# COREFILE	Core file routines for a native configuration
# bfd64_libs	Routines for 64bit support
OFILES = $(BFD_BACKENDS) $(BFD_MACHINES) @COREFILE@ @bfd64_libs@

stamp-ofiles: Makefile
	rm -f tofiles
	f=""; \
	for i in $(OFILES) ; do \
	  case " $$f " in \
	    *" $$i "*) ;; \
	    *) f="$$f $$i" ;; \
	  esac ; \
	done ; \
	echo $$f > tofiles
	$(SHELL) $(srcdir)/../move-if-change tofiles ofiles
	touch stamp-ofiles

ofiles: stamp-ofiles ; @true

# Since BFD64_LIBS is optional and we can't have substitution in
# libbfd_la_SOURCES, we put BFD64_LIBS in OFILES instead.
libbfd_la_SOURCES = $(BFD32_LIBS_CFILES)
libbfd_la_DEPENDENCIES = $(OFILES) ofiles
libbfd_la_LIBADD = `cat ofiles` @SHARED_LIBADD@ $(LIBDL)
libbfd_la_LDFLAGS = -release `cat libtool-soversion` @SHARED_LDFLAGS@

# libtool will build .libs/libbfd.a.  We create libbfd.a in the build
# directory so that we don't have to convert all the programs that use
# libbfd.a simultaneously.  This is a hack which should be removed if
# everything else starts using libtool.  FIXME.

noinst_LIBRARIES = libbfd.a
libbfd_a_SOURCES =

stamp-lib: libbfd.la
	libtooldir=`$(LIBTOOL) --config | sed -n -e 's/^objdir=//p'`; \
	if [ -f $$libtooldir/libbfd.a ]; then \
	  cp $$libtooldir/libbfd.a libbfd.tmp; \
	  $(RANLIB) libbfd.tmp; \
	  $(SHELL) $(srcdir)/../move-if-change libbfd.tmp libbfd.a; \
	else true; fi
	touch stamp-lib

libbfd.a: stamp-lib ; @true

# This file holds an array associating configuration triplets and
# vector names.  It is built from config.bfd.  It is not compiled by
# itself, but is included by targets.c.
targmatch.h: config.bfd targmatch.sed
	rm -f targmatch.h
	sed -f $(srcdir)/targmatch.sed < $(srcdir)/config.bfd > targmatch.new
	mv -f targmatch.new targmatch.h

# When compiling archures.c and targets.c, supply the default target
# info from configure.

targets.lo: targets.c Makefile
	$(LIBTOOL) --mode=compile $(COMPILE) -c $(TDEFAULTS) $(srcdir)/targets.c

archures.lo: archures.c Makefile
	$(LIBTOOL) --mode=compile $(COMPILE) -c $(TDEFAULTS) $(srcdir)/archures.c

dwarf2.lo: dwarf2.c Makefile
	$(LIBTOOL) --mode=compile $(COMPILE) -c -DDEBUGDIR=\"$(DEBUGDIR)\" $(srcdir)/dwarf2.c

elf32-target.h : elfxx-target.h
	rm -f elf32-target.h
	sed -e s/NN/32/g < $(srcdir)/elfxx-target.h > elf32-target.new
	mv -f elf32-target.new elf32-target.h

elf64-target.h : elfxx-target.h
	rm -f elf64-target.h
	sed -e s/NN/64/g < $(srcdir)/elfxx-target.h > elf64-target.new
	mv -f elf64-target.new elf64-target.h

elf32-ia64.c : elfxx-ia64.c
	rm -f elf32-ia64.c
	sed -e s/NN/32/g < $(srcdir)/elfxx-ia64.c > elf32-ia64.new
	mv -f elf32-ia64.new elf32-ia64.c

elf64-ia64.c : elfxx-ia64.c
	rm -f elf64-ia64.c
	sed -e s/NN/64/g < $(srcdir)/elfxx-ia64.c > elf64-ia64.new
	mv -f elf64-ia64.new elf64-ia64.c

peigen.c : peXXigen.c
	rm -f peigen.c
	sed -e s/XX/pe/g < $(srcdir)/peXXigen.c > peigen.new
	mv -f peigen.new peigen.c

pepigen.c : peXXigen.c
	rm -f pepigen.c
	sed -e s/XX/pep/g < $(srcdir)/peXXigen.c > pepigen.new
	mv -f pepigen.new pepigen.c

pex64igen.c: peXXigen.c
	rm -f pex64igen.c
	sed -e s/XX/pex64/g < $(srcdir)/peXXigen.c > pex64igen.new
	mv -f pex64igen.new pex64igen.c

BFD_H_DEPS= $(INCDIR)/ansidecl.h $(INCDIR)/symcat.h
LOCAL_H_DEPS= libbfd.h sysdep.h config.h
$(BFD32_LIBS) \
 $(BFD64_LIBS) \
 $(ALL_MACHINES) \
 $(BFD32_BACKENDS) \
 $(BFD64_BACKENDS) \
 $(OPTIONAL_BACKENDS): $(BFD_H) $(BFD_H_DEPS) $(LOCAL_H_DEPS)

install-bfdlibLTLIBRARIES: @INSTALL_LIBBFD_TRUE@install_libbfd
	@$(NORMAL_INSTALL)

uninstall-bfdlibLTLIBRARIES: @INSTALL_LIBBFD_TRUE@uninstall_libbfd
	@$(NORMAL_UNINSTALL)

.PHONY: install_libbfd uninstall_libbfd
install_libbfd: $(bfdlib_LTLIBRARIES) $(BFD_H)
	$(mkinstalldirs) $(DESTDIR)$(bfdlibdir)
	$(mkinstalldirs) $(DESTDIR)$(bfdincludedir)
	@list='$(bfdlib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(bfdlibdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(bfdlibdir)/$$p; \
	  else :; fi; \
	done
	$(INSTALL_DATA) $(BFD_H) $(DESTDIR)$(bfdincludedir)/bfd.h
	$(INSTALL_DATA) $(INCDIR)/ansidecl.h $(DESTDIR)$(bfdincludedir)/ansidecl.h
	$(INSTALL_DATA) $(INCDIR)/symcat.h $(DESTDIR)$(bfdincludedir)/symcat.h
	$(INSTALL_DATA) $(INCDIR)/bfdlink.h $(DESTDIR)$(bfdincludedir)/bfdlink.h

uninstall_libbfd:
	list='$(bfdlib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(bfdlibdir)/$$p; \
	done
	rm -f $(DESTDIR)$(bfdincludedir)/bfd.h
	rm -f $(DESTDIR)$(bfdincludedir)/ansidecl.h
	rm -f $(DESTDIR)$(bfdincludedir)/symcat.h
	rm -f $(DESTDIR)$(bfdincludedir)/bfdlink.h

# Have to get rid of DEP1 here so that "$?" later includes all of $(CFILES).
DEP: dep.sed $(CFILES) $(HFILES) bfd.h
	rm -f DEP1
	$(MAKE) MKDEP="$(MKDEP)" DEP1
	echo '# IF YOU PUT ANYTHING HERE IT WILL GO AWAY' >> DEP1
	if grep ' /' DEP1 > /dev/null 2> /dev/null; then \
	  echo 'make DEP failed!'; exit 1; \
	else \
	  mv -f DEP1 $@; \
	fi

DEP1: $(CFILES)
	echo '# DO NOT DELETE THIS LINE -- mkdep uses it.' > DEP2
	echo '# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.' >> DEP2
	for f in $?; do \
	  $(MKDEP) $(INCLUDES) $(CFLAGS) $$f > DEPA; \
	  sed -n -e '1s/\.o: .*/.lo: \\/p' -e q < DEPA >> DEP2; \
	  sed -e '1s/.*: //' -f dep.sed < DEPA | \
	    LC_ALL=C sort | LC_ALL=C uniq | \
	    sed -e 's/^[AB]/  /' -e '$$s/ \\$$//' >> DEP2; \
	done
	rm -f DEPA
	mv -f DEP2 $@

dep.sed: dep-in.sed config.status
	sed <$(srcdir)/dep-in.sed >dep.sed	\
		-e 's!@BFD_H@!$(BFD_H)!'	\
		-e 's!@SRCDIR@!$(srcdir)!'	\
		-e 's!@TOPDIR@!'`echo $(srcdir) | sed -e s,/bfd$$,,`'!'

dep: DEP
	sed -e '/^..DO NOT DELETE THIS LINE/,$$d' < Makefile > tmp-Makefile
	cat DEP >> tmp-Makefile
	$(SHELL) $(srcdir)/../move-if-change tmp-Makefile Makefile

dep-in: DEP
	sed -e '/^..DO NOT DELETE THIS LINE/,$$d' < $(srcdir)/Makefile.in > tmp-Makefile.in
	cat DEP >> tmp-Makefile.in
	$(SHELL) $(srcdir)/../move-if-change tmp-Makefile.in $(srcdir)/Makefile.in

dep-am: DEP
	sed -e '/^..DO NOT DELETE THIS LINE/,$$d' < $(srcdir)/Makefile.am > tmp-Makefile.am
	cat DEP >> tmp-Makefile.am
	$(SHELL) $(srcdir)/../move-if-change tmp-Makefile.am $(srcdir)/Makefile.am

host-aout.lo: Makefile

# The following program can be used to generate a simple config file
# which can be folded into an h-XXX file for a new host, with some editing.
aout-params.h: gen-aout
	./gen-aout host > aout-params.h
gen-aout: $(srcdir)/gen-aout.c Makefile
	$(CC) -o gen-aout $(CFLAGS) $(LFLAGS) $(srcdir)/gen-aout.c

$(BFD_H): stmp-bfd-h ; @true

stmp-bfd-h: bfd-in3.h
	rm -f bfd-tmp.h
	cp bfd-in3.h bfd-tmp.h
	$(SHELL) $(srcdir)/../move-if-change bfd-tmp.h $(BFD_H)
	rm -f bfd-tmp.h
	touch stmp-bfd-h

BFD_H_FILES = bfd-in.h init.c opncls.c libbfd.c section.c archures.c \
	reloc.c syms.c bfd.c bfdio.c bfdwin.c \
	archive.c corefile.c targets.c format.c compress.c
BFD64_H_FILES = archive64.c
LIBBFD_H_FILES = libbfd-in.h init.c libbfd.c bfdio.c bfdwin.c \
	cache.c reloc.c archures.c elf.c
LIBCOFF_H_FILES = libcoff-in.h coffcode.h

# Could really use a "copy-if-change"...
headers:
	(cd $(bfddocdir); $(MAKE) protos $(FLAGS_TO_PASS))
	cp $(bfddocdir)/bfd.h bfd-in2.h-new
	$(SHELL) $(srcdir)/../move-if-change bfd-in2.h-new $(srcdir)/bfd-in2.h
	cp $(bfddocdir)/libbfd.h libbfd.h-new
	$(SHELL) $(srcdir)/../move-if-change libbfd.h-new $(srcdir)/libbfd.h
	cp $(bfddocdir)/libcoff.h libcoff.h-new
	$(SHELL) $(srcdir)/../move-if-change libcoff.h-new $(srcdir)/libcoff.h

# We only rebuild the header files automatically if we have been
# configured with --enable-maintainer-mode.

$(srcdir)/bfd-in2.h: @MAINT@ stmp-bin2-h ; @true
stmp-bin2-h: $(BFD_H_FILES) $(BFD64_H_FILES)
	(cd $(bfddocdir); $(MAKE) $(FLAGS_TO_PASS) bfd.h)
	cp $(bfddocdir)/bfd.h bfd-in2.h-new
	$(SHELL) $(srcdir)/../move-if-change bfd-in2.h-new $(srcdir)/bfd-in2.h
	touch stmp-bin2-h

$(srcdir)/libbfd.h: @MAINT@ stmp-lbfd-h ; @true
stmp-lbfd-h: $(LIBBFD_H_FILES)
	(cd $(bfddocdir); $(MAKE) $(FLAGS_TO_PASS) libbfd.h)
	cp $(bfddocdir)/libbfd.h libbfd.h-new
	$(SHELL) $(srcdir)/../move-if-change libbfd.h-new $(srcdir)/libbfd.h
	touch stmp-lbfd-h

$(srcdir)/libcoff.h: @MAINT@ stmp-lcoff-h ; @true
stmp-lcoff-h: $(LIBCOFF_H_FILES)
	(cd $(bfddocdir); $(MAKE) $(FLAGS_TO_PASS) libcoff.h)
	cp $(bfddocdir)/libcoff.h libcoff.h-new
	$(SHELL) $(srcdir)/../move-if-change libcoff.h-new $(srcdir)/libcoff.h
	touch stmp-lcoff-h

MOSTLYCLEANFILES = ofiles stamp-ofiles

CLEANFILES = bfd.h dep.sed stmp-bfd-h DEP DEPA DEP1 DEP2 libbfd.a stamp-lib \
	stmp-bin2-h stmp-lbfd-h stmp-lcoff-h

DISTCLEANFILES = $(BUILD_CFILES) $(BUILD_HFILES) libtool-soversion

bfdver.h: $(srcdir)/version.h $(srcdir)/Makefile.in
	@echo "creating $@"
	@bfd_version=`echo "$(VERSION)" | sed -e 's/\([^\.]*\)\.*\([^\.]*\)\.*\([^\.]*\)\.*\([^\.]*\)\.*\([^\.]*\).*/\1.00\2.00\3.00\4.00\5/' -e 's/\([^\.]*\)\..*\(..\)\..*\(..\)\..*\(..\)\..*\(..\)$$/\1\2\3\4\5/'` ;\
	bfd_version_string="\"$(VERSION)\"" ;\
	bfd_soversion="$(VERSION)" ;\
	bfd_version_package="\"$(PKGVERSION)\"" ;\
	report_bugs_to="\"$(REPORT_BUGS_TO)\"" ;\
	if test "x$(RELEASE)" = x ; then \
	  bfd_version_date=`sed -n -e 's/.*DATE //p' < $(srcdir)/version.h` ;\
	  bfd_version_string="\"$(VERSION).$${bfd_version_date}\"" ;\
	  bfd_soversion="$(VERSION).$${bfd_version_date}" ;\
	fi ;\
	sed -e "s,@bfd_version@,$$bfd_version," \
	    -e "s,@bfd_version_string@,$$bfd_version_string," \
	    -e "s,@bfd_version_package@,$$bfd_version_package," \
	    -e "s,@report_bugs_to@,$$report_bugs_to," \
	    < $(srcdir)/version.h > $@; \
	echo "$${bfd_soversion}" > libtool-soversion

# What appears below is generated by a hacked mkdep using gcc -MM.

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
archive.lo: \
  archive.c \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/ranlib.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h
archures.lo: \
  archures.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h
bfd.lo: \
  bfd.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/demangle.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h \
  bfdver.h \
  elf-bfd.h \
  libcoff.h \
  libecoff.h
bfdio.lo: \
  bfdio.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
bfdwin.lo: \
  bfdwin.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cache.lo: \
  cache.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h
coffgen.lo: \
  coffgen.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libcoff.h
corefile.lo: \
  corefile.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
format.lo: \
  format.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
init.lo: \
  init.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
libbfd.lo: \
  libbfd.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
opncls.lo: \
  opncls.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/objalloc.h
reloc.lo: \
  reloc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
section.lo: \
  section.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
syms.lo: \
  syms.c \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h
targets.lo: \
  targets.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fnmatch.h \
  $(INCDIR)/hashtab.h \
  targmatch.h
hash.lo: \
  hash.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/objalloc.h
linker.lo: \
  linker.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  genlink.h
srec.lo: \
  srec.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h
binary.lo: \
  binary.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h
tekhex.lo: \
  tekhex.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h
ihex.lo: \
  ihex.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h
stabs.lo: \
  stabs.c \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h
stab-syms.lo: \
  stab-syms.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/bfdlink.h \
  libaout.h
merge.lo: \
  merge.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h
dwarf2.lo: \
  dwarf2.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dwarf2.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h
simple.lo: \
  simple.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
compress.lo: \
  compress.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
verilog.lo: \
  verilog.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h
archive64.lo: \
  archive64.c \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-alpha.lo: \
  cpu-alpha.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-arc.lo: \
  cpu-arc.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-arm.lo: \
  cpu-arm.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h
cpu-avr.lo: \
  cpu-avr.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-bfin.lo: \
  cpu-bfin.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-cr16.lo: \
  cpu-cr16.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-cr16c.lo: \
  cpu-cr16c.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-cris.lo: \
  cpu-cris.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-crx.lo: \
  cpu-crx.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-d10v.lo: \
  cpu-d10v.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-d30v.lo: \
  cpu-d30v.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-dlx.lo: \
  cpu-dlx.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-fr30.lo: \
  cpu-fr30.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-frv.lo: \
  cpu-frv.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-h8300.lo: \
  cpu-h8300.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-h8500.lo: \
  cpu-h8500.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-hppa.lo: \
  cpu-hppa.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-i370.lo: \
  cpu-i370.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-i386.lo: \
  cpu-i386.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-i860.lo: \
  cpu-i860.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-i960.lo: \
  cpu-i960.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-ia64.lo: \
  cpu-ia64.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/ia64.h \
  $(srcdir)/../opcodes/ia64-opc.h \
  cpu-ia64-opc.c
cpu-ip2k.lo: \
  cpu-ip2k.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-iq2000.lo: \
  cpu-iq2000.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-lm32.lo: \
  cpu-lm32.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m10200.lo: \
  cpu-m10200.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m10300.lo: \
  cpu-m10300.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m32c.lo: \
  cpu-m32c.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m32r.lo: \
  cpu-m32r.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m68hc11.lo: \
  cpu-m68hc11.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m68hc12.lo: \
  cpu-m68hc12.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-m68k.lo: \
  cpu-m68k.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/m68k.h
cpu-m88k.lo: \
  cpu-m88k.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-maxq.lo: \
  cpu-maxq.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-mcore.lo: \
  cpu-mcore.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-mep.lo: \
  cpu-mep.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-mips.lo: \
  cpu-mips.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-mmix.lo: \
  cpu-mmix.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-moxie.lo: \
  cpu-moxie.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-msp430.lo: \
  cpu-msp430.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-mt.lo: \
  cpu-mt.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-ns32k.lo: \
  cpu-ns32k.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  ns32k.h
cpu-openrisc.lo: \
  cpu-openrisc.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-or32.lo: \
  cpu-or32.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-pdp11.lo: \
  cpu-pdp11.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-pj.lo: \
  cpu-pj.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-plugin.lo: \
  cpu-plugin.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-powerpc.lo: \
  cpu-powerpc.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-rs6000.lo: \
  cpu-rs6000.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-s390.lo: \
  cpu-s390.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-score.lo: \
  cpu-score.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-sh.lo: \
  cpu-sh.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(srcdir)/../opcodes/sh-opc.h
cpu-sparc.lo: \
  cpu-sparc.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-spu.lo: \
  cpu-spu.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-tic30.lo: \
  cpu-tic30.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-tic4x.lo: \
  cpu-tic4x.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-tic54x.lo: \
  cpu-tic54x.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-tic80.lo: \
  cpu-tic80.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-v850.lo: \
  cpu-v850.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h
cpu-vax.lo: \
  cpu-vax.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-w65.lo: \
  cpu-w65.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-we32k.lo: \
  cpu-we32k.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-xc16x.lo: \
  cpu-xc16x.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-xstormy16.lo: \
  cpu-xstormy16.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-xtensa.lo: \
  cpu-xtensa.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-z80.lo: \
  cpu-z80.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
cpu-z8k.lo: \
  cpu-z8k.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
aout-adobe.lo: \
  aout-adobe.c \
  $(INCDIR)/aout/adobe.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libaout.h
aout-arm.lo: \
  aout-arm.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aout-target.h \
  aoutx.h \
  libaout.h
aout-cris.lo: \
  aout-cris.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aout-target.h \
  aout32.c \
  aoutx.h \
  libaout.h
aout-ns32k.lo: \
  aout-ns32k.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aoutx.h \
  libaout.h \
  ns32k.h
aout-sparcle.lo: \
  aout-sparcle.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/aout/sun4.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  aoutf1.h \
  libaout.h
aout-tic30.lo: \
  aout-tic30.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aoutx.h \
  libaout.h
aout0.lo: \
  aout0.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/aout/sun4.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  aoutf1.h \
  libaout.h
aout32.lo: \
  aout32.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aoutx.h \
  libaout.h
armnetbsd.lo: \
  armnetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
bout.lo: \
  bout.c \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/bout.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  genlink.h \
  libaout.h
cf-i386lynx.lo: \
  cf-i386lynx.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-i386.c \
  coffcode.h \
  coffswap.h \
  libcoff.h
cf-sparclynx.lo: \
  cf-sparclynx.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sparc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-sparc.c \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-apollo.lo: \
  coff-apollo.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/apollo.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-arm.lo: \
  coff-arm.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-aux.lo: \
  coff-aux.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/aux-coff.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/m68k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-m68k.c \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-go32.lo: \
  coff-go32.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-i386.c \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-h8300.lo: \
  coff-h8300.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/h8300.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coffcode.h \
  coffswap.h \
  genlink.h \
  libcoff.h
coff-h8500.lo: \
  coff-h8500.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/h8500.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-i386.lo: \
  coff-i386.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-i860.lo: \
  coff-i860.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i860.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-i960.lo: \
  coff-i960.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/i960.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-m68k.lo: \
  coff-m68k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/m68k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-m88k.lo: \
  coff-m88k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/m88k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-maxq.lo: \
  coff-maxq.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/maxq.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-mips.lo: \
  coff-mips.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mips.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffswap.h \
  ecoffswap.h \
  libcoff.h \
  libecoff.h
coff-or32.lo: \
  coff-or32.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/or32.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-rs6000.lo: \
  coff-rs6000.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/rs6000.h \
  $(INCDIR)/coff/xcoff.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h \
  libxcoff.h
coff-sh.lo: \
  coff-sh.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-sparc.lo: \
  coff-sparc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sparc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-stgo32.lo: \
  coff-stgo32.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/go32exe.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-i386.c \
  coffcode.h \
  coffswap.h \
  go32stub.h \
  libcoff.h
coff-svm68k.lo: \
  coff-svm68k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/m68k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-m68k.c \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-tic30.lo: \
  coff-tic30.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/tic30.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-tic4x.lo: \
  coff-tic4x.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/ti.h \
  $(INCDIR)/coff/tic4x.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-tic54x.lo: \
  coff-tic54x.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/ti.h \
  $(INCDIR)/coff/tic54x.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-tic80.lo: \
  coff-tic80.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/tic80.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-u68k.lo: \
  coff-u68k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/m68k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-m68k.c \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-w65.lo: \
  coff-w65.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/w65.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-we32k.lo: \
  coff-we32k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/we32k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-z80.lo: \
  coff-z80.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/z80.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff-z8k.lo: \
  coff-z8k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/z8k.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
cofflink.lo: \
  cofflink.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  libcoff.h
dwarf1.lo: \
  dwarf1.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dwarf.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h
ecoff.lo: \
  ecoff.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/ranlib.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  libaout.h \
  libcoff.h \
  libecoff.h
ecofflink.lo: \
  ecofflink.c \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/objalloc.h \
  libcoff.h \
  libecoff.h
elf-attrs.lo: \
  elf-attrs.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h
elf-eh-frame.lo: \
  elf-eh-frame.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dwarf2.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h
elf-m10200.lo: \
  elf-m10200.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf-m10300.lo: \
  elf-m10300.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mn10300.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf-strtab.lo: \
  elf-strtab.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h
elf-vxworks.lo: \
  elf-vxworks.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/vxworks.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf-vxworks.h
elf.lo: \
  elf.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h \
  elf-bfd.h
elf32-am33lin.lo: \
  elf32-am33lin.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mn10300.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf-m10300.c \
  elf32-target.h
elf32-arc.lo: \
  elf32-arc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/arc.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-arm.lo: \
  elf32-arm.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/arm.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-target.h
elf32-avr.lo: \
  elf32-avr.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/avr.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-avr.h \
  elf32-target.h
elf32-bfin.lo: \
  elf32-bfin.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/bfin.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dwarf2.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-cr16.lo: \
  elf32-cr16.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/cr16.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-cr16c.lo: \
  elf32-cr16c.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/cr16c.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-cris.lo: \
  elf32-cris.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/cris.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-crx.lo: \
  elf32-crx.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/crx.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-d10v.lo: \
  elf32-d10v.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/d10v.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-d30v.lo: \
  elf32-d30v.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/d30v.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-dlx.lo: \
  elf32-dlx.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dlx.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-fr30.lo: \
  elf32-fr30.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/fr30.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-frv.lo: \
  elf32-frv.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dwarf2.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/frv.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-gen.lo: \
  elf32-gen.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-h8300.lo: \
  elf32-h8300.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/h8.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-hppa.lo: \
  elf32-hppa.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/hppa.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf-hppa.h \
  elf32-hppa.h \
  elf32-target.h \
  libhppa.h
elf32-i370.lo: \
  elf32-i370.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/i370.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-i386.lo: \
  elf32-i386.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/i386.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  bfd_stdint.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-target.h
elf32-i860.lo: \
  elf32-i860.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/i860.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-i960.lo: \
  elf32-i960.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/i960.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-ip2k.lo: \
  elf32-ip2k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/ip2k.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-iq2000.lo: \
  elf32-iq2000.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/iq2000.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-lm32.lo: \
  elf32-lm32.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/lm32.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-m32c.lo: \
  elf32-m32c.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/m32c.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-m32r.lo: \
  elf32-m32r.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/m32r.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-m68hc11.lo: \
  elf32-m68hc11.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/m68hc11.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/m68hc11.h \
  elf-bfd.h \
  elf32-m68hc1x.h \
  elf32-target.h
elf32-m68hc12.lo: \
  elf32-m68hc12.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/m68hc11.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/m68hc11.h \
  elf-bfd.h \
  elf32-m68hc1x.h \
  elf32-target.h
elf32-m68hc1x.lo: \
  elf32-m68hc1x.c \
  $(INCDIR)/alloca-conf.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/m68hc11.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/m68hc11.h \
  elf-bfd.h \
  elf32-m68hc1x.h
elf32-m68k.lo: \
  elf32-m68k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/m68k.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/m68k.h \
  elf-bfd.h \
  elf32-target.h
elf32-m88k.lo: \
  elf32-m88k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-mcore.lo: \
  elf32-mcore.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mcore.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-mep.lo: \
  elf32-mep.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mep.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-mips.lo: \
  elf32-mips.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mips.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mips.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  ecoffswap.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-target.h \
  elfxx-mips.h \
  genlink.h
elf32-moxie.lo: \
  elf32-moxie.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/moxie.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-msp430.lo: \
  elf32-msp430.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/msp430.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-mt.lo: \
  elf32-mt.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mt.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-openrisc.lo: \
  elf32-openrisc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/openrisc.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-or32.lo: \
  elf32-or32.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/or32.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-pj.lo: \
  elf32-pj.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/pj.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-ppc.lo: \
  elf32-ppc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/ppc.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-ppc.h \
  elf32-target.h
elf32-s390.lo: \
  elf32-s390.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/s390.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-sh-symbian.lo: \
  elf32-sh-symbian.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(srcdir)/../opcodes/sh-opc.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-sh-relocs.h \
  elf32-sh.c \
  elf32-target.h
elf32-sh.lo: \
  elf32-sh.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(srcdir)/../opcodes/sh-opc.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-sh-relocs.h \
  elf32-target.h
elf32-sh64-com.lo: \
  elf32-sh64-com.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(srcdir)/../opcodes/sh64-opc.h \
  elf-bfd.h \
  elf32-sh64.h
elf32-sh64.lo: \
  elf32-sh64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(srcdir)/../opcodes/sh-opc.h \
  $(srcdir)/../opcodes/sh64-opc.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-sh-relocs.h \
  elf32-sh.c \
  elf32-sh64.h \
  elf32-target.h
elf32-sparc.lo: \
  elf32-sparc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sparc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/sparc.h \
  elf-bfd.h \
  elf-vxworks.h \
  elf32-target.h \
  elfxx-sparc.h
elf32-spu.lo: \
  elf32-spu.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/spu.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-spu.h \
  elf32-target.h
elf32-v850.lo: \
  elf32-v850.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/v850.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-vax.lo: \
  elf32-vax.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/vax.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf32-target.h
elf32-xc16x.lo: \
  elf32-xc16x.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/dwarf2.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/xc16x.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-xstormy16.lo: \
  elf32-xstormy16.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/xstormy16.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-target.h
elf32-xtensa.lo: \
  elf32-xtensa.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/xtensa.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/xtensa-config.h \
  $(INCDIR)/xtensa-isa.h \
  elf-bfd.h \
  elf32-target.h
elf32.lo: \
  elf32.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elfcode.h \
  elfcore.h
elflink.lo: \
  elflink.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/objalloc.h \
  $(INCDIR)/safe-ctype.h \
  elf-bfd.h
elfxx-mips.lo: \
  elfxx-mips.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/mips.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mips.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf-vxworks.h \
  elfxx-mips.h
elfxx-sparc.lo: \
  elfxx-sparc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sparc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/opcode/sparc.h \
  elf-bfd.h \
  elf-vxworks.h \
  elfxx-sparc.h
epoc-pe-arm.lo: \
  epoc-pe-arm.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-arm.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  pe-arm.c \
  peicode.h
epoc-pei-arm.lo: \
  epoc-pei-arm.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-arm.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  pei-arm.c \
  peicode.h
hp300bsd.lo: \
  hp300bsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
hp300hpux.lo: \
  hp300hpux.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/hp300hpux.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aout-target.h \
  aoutx.h \
  libaout.h
i386aout.lo: \
  i386aout.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
i386bsd.lo: \
  i386bsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
i386dynix.lo: \
  i386dynix.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/dynix3.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aout-target.h \
  aoutx.h \
  libaout.h
i386freebsd.lo: \
  i386freebsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  freebsd.h \
  libaout.h
i386linux.lo: \
  i386linux.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
i386lynx.lo: \
  i386lynx.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
i386mach3.lo: \
  i386mach3.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
i386msdos.lo: \
  i386msdos.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libaout.h
i386netbsd.lo: \
  i386netbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
i386os9k.lo: \
  i386os9k.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/os9k.h \
  libaout.h
ieee.lo: \
  ieee.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/ieee.h \
  $(INCDIR)/safe-ctype.h \
  libieee.h
m68k4knetbsd.lo: \
  m68k4knetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
m68klinux.lo: \
  m68klinux.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
m68knetbsd.lo: \
  m68knetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
m88kmach3.lo: \
  m88kmach3.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
m88kopenbsd.lo: \
  m88kopenbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
mach-o.lo: \
  mach-o.c \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  mach-o-target.c \
  mach-o.h
mach-o-i386.lo: \
  mach-o-i386.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  mach-o-target.c \
  mach-o.h
mipsbsd.lo: \
  mipsbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
newsos3.lo: \
  newsos3.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
nlm.lo: \
  nlm.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/internal.h \
  libnlm.h
nlm32-i386.lo: \
  nlm32-i386.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/i386-ext.h \
  $(INCDIR)/nlm/internal.h \
  libnlm.h \
  nlm-target.h \
  nlmswap.h
nlm32-ppc.lo: \
  nlm32-ppc.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/internal.h \
  $(INCDIR)/nlm/ppc-ext.h \
  libnlm.h \
  nlm-target.h \
  nlmswap.h
nlm32-sparc.lo: \
  nlm32-sparc.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/internal.h \
  $(INCDIR)/nlm/sparc32-ext.h \
  libnlm.h \
  nlm-target.h \
  nlmswap.h
nlm32.lo: \
  nlm32.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/internal.h \
  libnlm.h \
  nlmcode.h
ns32knetbsd.lo: \
  ns32knetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
oasys.lo: \
  oasys.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/oasys.h \
  $(INCDIR)/safe-ctype.h \
  liboasys.h
pc532-mach.lo: \
  pc532-mach.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
pdp11.lo: \
  pdp11.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aout-target.h \
  libaout.h
pe-arm-wince.lo: \
  pe-arm-wince.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-arm.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  pe-arm.c \
  peicode.h
pe-arm.lo: \
  pe-arm.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-arm.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pe-i386.lo: \
  pe-i386.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-i386.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pe-mcore.lo: \
  pe-mcore.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mcore.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-mcore.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pe-mips.lo: \
  pe-mips.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mipspe.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pe-ppc.lo: \
  pe-ppc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/powerpc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-ppc.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pe-sh.lo: \
  pe-sh.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coff-sh.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pef.lo: \
  pef.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h \
  pef-traceback.h \
  pef.h
pei-arm-wince.lo: \
  pei-arm-wince.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-arm.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  pei-arm.c \
  peicode.h
pei-arm.lo: \
  pei-arm.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/arm.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-arm.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pei-i386.lo: \
  pei-i386.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-i386.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pei-mcore.lo: \
  pei-mcore.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mcore.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-mcore.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pei-mips.lo: \
  pei-mips.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mipspe.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  libcoff.h \
  libpei.h \
  pe-mips.c \
  peicode.h
pei-ppc.lo: \
  pei-ppc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/powerpc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-ppc.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pei-sh.lo: \
  pei-sh.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coff-sh.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
plugin.lo: \
  plugin.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/plugin-api.h \
  plugin.h
ppcboot.lo: \
  ppcboot.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h
reloc16.lo: \
  reloc16.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  genlink.h \
  libcoff.h
riscix.lo: \
  riscix.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
som.lo: \
  som.c \
  $(INCDIR)/alloca-conf.h \
  $(INCDIR)/filenames.h
sparclinux.lo: \
  sparclinux.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
sparclynx.lo: \
  sparclynx.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/aout/sun4.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
sparcnetbsd.lo: \
  sparcnetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
sunos.lo: \
  sunos.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/aout/sun4.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  aoutf1.h \
  libaout.h
vax1knetbsd.lo: \
  vax1knetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
vaxbsd.lo: \
  vaxbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h
vaxnetbsd.lo: \
  vaxnetbsd.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  libaout.h \
  netbsd.h
versados.lo: \
  versados.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h
vms-gsd.lo: \
  vms-gsd.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  vms.h
vms-hdr.lo: \
  vms-hdr.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  bfdver.h \
  vms.h
vms-misc.lo: \
  vms-misc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  vms.h
vms-tir.lo: \
  vms-tir.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  vms.h
vms.lo: \
  vms.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  vms.h
vmsutil.lo: \
  vmsutil.c \
  vmsutil.h
xcofflink.lo: \
  xcofflink.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/xcoff.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  libcoff.h \
  libxcoff.h
xsym.lo: \
  xsym.c \
  $(INCDIR)/alloca-conf.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  xsym.h
xtensa-isa.lo: \
  xtensa-isa.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/xtensa-isa-internal.h \
  $(INCDIR)/xtensa-isa.h
xtensa-modules.lo: \
  xtensa-modules.c \
  $(INCDIR)/xtensa-isa-internal.h \
  $(INCDIR)/xtensa-isa.h
aix5ppc-core.lo: \
  aix5ppc-core.c \
  $(INCDIR)/filenames.h
aout64.lo: \
  aout64.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/safe-ctype.h \
  aoutx.h \
  libaout.h
coff-alpha.lo: \
  coff-alpha.c \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/alpha.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffswap.h \
  ecoffswap.h \
  libcoff.h \
  libecoff.h
coff-x86_64.lo: \
  coff-x86_64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/x86_64.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coffcode.h \
  coffswap.h \
  libcoff.h
coff64-rs6000.lo: \
  coff64-rs6000.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/rs6k64.h \
  $(INCDIR)/coff/xcoff.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coffcode.h \
  coffswap.h \
  libcoff.h \
  libxcoff.h
demo64.lo: \
  demo64.c \
  $(INCDIR)/aout/aout64.h \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/aout/stab.def \
  $(INCDIR)/aout/stab_gnu.h \
  $(INCDIR)/aout/sun4.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  aout-target.h \
  aoutf1.h \
  libaout.h
elf32-score.lo: \
  elf32-score.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/score.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-score.h \
  elf32-target.h
elf32-score7.lo: \
  elf32-score7.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/score.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elf32-score.h
elf64-alpha.lo: \
  elf64-alpha.c \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/alpha.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/elf/alpha.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  ecoffswap.h \
  elf-bfd.h \
  elf64-target.h \
  libcoff.h \
  libecoff.h
elf64-gen.lo: \
  elf64-gen.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf64-target.h
elf64-hppa.lo: \
  elf64-hppa.c \
  $(INCDIR)/alloca-conf.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/hppa.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf-hppa.h \
  elf64-hppa.h \
  elf64-target.h \
  libhppa.h
elf64-mips.lo: \
  elf64-mips.c \
  $(INCDIR)/aout/ar.h \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/alpha.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mips.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  ecoffswap.h \
  elf-bfd.h \
  elf64-target.h \
  elfxx-mips.h \
  genlink.h
elf64-mmix.lo: \
  elf64-mmix.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mmix.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/mmix.h \
  elf-bfd.h \
  elf64-target.h
elf64-ppc.lo: \
  elf64-ppc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/ppc64.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf64-ppc.h \
  elf64-target.h
elf64-s390.lo: \
  elf64-s390.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/s390.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf64-target.h
elf64-sh64.lo: \
  elf64-sh64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sh.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  elf-bfd.h \
  elf64-target.h
elf64-sparc.lo: \
  elf64-sparc.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/sparc.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/opcode/sparc.h \
  elf-bfd.h \
  elf64-target.h \
  elfxx-sparc.h
elf64-x86-64.lo: \
  elf64-x86-64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/elf/x86-64.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  bfd_stdint.h \
  elf-bfd.h \
  elf64-target.h
elf64.lo: \
  elf64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  elf-bfd.h \
  elfcode.h \
  elfcore.h
elfn32-mips.lo: \
  elfn32-mips.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/ecoff.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/mips.h \
  $(INCDIR)/coff/sym.h \
  $(INCDIR)/coff/symconst.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/mips.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  ecoffswap.h \
  elf-bfd.h \
  elf32-target.h \
  elfxx-mips.h \
  genlink.h
mmo.lo: \
  mmo.c \
  $(INCDIR)/elf/mmix.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/opcode/mmix.h
nlm32-alpha.lo: \
  nlm32-alpha.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/alpha-ext.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/internal.h \
  libnlm.h \
  nlm-target.h \
  nlmswap.h
nlm64.lo: \
  nlm64.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/nlm/common.h \
  $(INCDIR)/nlm/external.h \
  $(INCDIR)/nlm/internal.h \
  libnlm.h \
  nlmcode.h
pe-x86_64.lo: \
  pe-x86_64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/x86_64.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coff-x86_64.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pei-ia64.lo: \
  pei-ia64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/ia64.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  coff-ia64.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
pei-x86_64.lo: \
  pei-x86_64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/x86_64.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/libiberty.h \
  coff-x86_64.c \
  coffcode.h \
  libcoff.h \
  libpei.h \
  peicode.h
aix386-core.lo: \
  aix386-core.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libcoff.h
cisco-core.lo: \
  cisco-core.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
hpux-core.lo: \
  hpux-core.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
irix-core.lo: \
  irix-core.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
lynx-core.lo: \
  lynx-core.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
osf-core.lo: \
  osf-core.c \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h
sco5-core.lo: \
  sco5-core.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libaout.h
trad-core.lo: \
  trad-core.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libaout.h
elf32-ia64.lo: \
  elf32-ia64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/ia64.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/objalloc.h \
  $(INCDIR)/opcode/ia64.h \
  elf-bfd.h \
  elf32-target.h
elf64-ia64.lo: \
  elf64-ia64.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/elf/common.h \
  $(INCDIR)/elf/external.h \
  $(INCDIR)/elf/ia64.h \
  $(INCDIR)/elf/internal.h \
  $(INCDIR)/elf/reloc-macros.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  $(INCDIR)/objalloc.h \
  $(INCDIR)/opcode/ia64.h \
  elf-bfd.h \
  elf64-target.h
peigen.lo: \
  peigen.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/i386.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libcoff.h \
  libpei.h
pepigen.lo: \
  pepigen.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/ia64.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libcoff.h \
  libpei.h
pex64igen.lo: \
  pex64igen.c \
  $(INCDIR)/bfdlink.h \
  $(INCDIR)/coff/external.h \
  $(INCDIR)/coff/internal.h \
  $(INCDIR)/coff/pe.h \
  $(INCDIR)/coff/x86_64.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/hashtab.h \
  libcoff.h \
  libpei.h
# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
