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


DOCFILES = syms.texi  bfd.texi  cache.texi \
	 format.texi  section.texi  archive.texi \
	 core.texi  libbfd.texi  archures.texi \
	 reloc.texi  opncls.texi  targets.texi \
	 aoutx.texi  coffcode.texi cache.texi

PROTOS = opncls.p archures.p libbfd.p \
	section.p syms.p bfd.p \
	archive.p reloc.p targets.p \
	format.p coffcode.p core.p

IPROTOS = cache.ip libbfd.ip reloc.ip

docs: protos bfd.info bfd.dvi bfd.ps

protos: $(PROTOS) $(IPROTOS)
	sed -f intobfd bfd-in.h  > bfd.h
	sed -f tolibbfd libbfd-in.h  > libbfd.h
	sed -f tolibcoff libcoff-in.h  > libcoff.h



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
