.SUFFIXES: .texi .o .c .h .p .ip
VPATH=..
.c.texi:
	scanit $< $@

.h.texi:
	scanit $< $@

.c.p:
	scanph $< $@

.h.p:
	scanph $< $@

.c.ip:
	scanph -i $< $@


DOCFILES = aoutx.texi  archive.texi archures.texi \
	bfd.texi  cache.texi coffcode.texi \
	core.texi  format.texi  libbfd.texi  \
	opncls.texi  reloc.texi  section.texi  \
	syms.texi  targets.texi init.texi
	

PROTOS = archive.p archures.p bfd.p \
	coffcode.p core.p format.p \
	libbfd.p opncls.p reloc.p \
	section.p syms.p targets.p  \
	format.p coffcode.p core.p howto.p machines.p init.p

IPROTOS = cache.ip libbfd.ip reloc.ip init.ip archures.ip

# SRCDOC, SRCPROT, SRCIPROT only used to sidestep Sun Make bug in interaction 
# between VPATH and suffix rules.  If you use GNU Make, perhaps other Makes,
# you don't need these three:
SRCDOC = aoutx.h  archive.c archures.c \
	bfd.c  cache.c coffcode.h \
	core.c  format.c  libbfd.c  \
	opncls.c  reloc.c  section.c  \
	syms.c  targets.c init.c

SRCPROT = archive.c archures.c bfd.c \
	coffcode.h core.c format.c \
	libbfd.c opncls.c reloc.c \
	section.c syms.c targets.c init.c

SRCIPROT = cache.c libbfd.c reloc.c howto.c cpu-h8300.c  cpu-i960.c archures.c init.c


docs: protos bfd.info bfd.dvi bfd.ps

protos: $(PROTOS) $(IPROTOS)
	sed -f intobfd bfd-in.h  > bfd.h
	sed -f tolibbfd libbfd-in.h  > libbfd.h
	sed -f tolibcoff libcoff-in.h  > libcoff.h

# Following three rules only for the benefit of Sun Make; see comment above
$(DOCFILES) : $(SRCDOC)
$(PROTOS) : $(SRCPROT)
$(IPROTOS) : $(SRCIPROT)

clean:
	rm -f $(PROTOS) *.p *.ip *.h bfd.?? $(DOCFILES) bfd.dvi bfd.ps *~* *# bfd.??? 

bfd.info: $(DOCFILES) bfd.texinfo
	makeinfo +no-validate bfd.texinfo

bfd.dvi: $(DOCFILES) bfd.texinfo
	tex bfd.texinfo
	texindex bfd.??
	tex bfd.texinfo

bfd.ps: bfd.dvi
	dvips bfd -o
	
quickdoc: $(DSRC) docs
	tex bfd.texinfo
