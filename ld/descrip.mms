# mms file to build vms-i960 ld
# sac@cygnus.com

.FIRST
	@[-.deci960]define-gnu


CC	= gcc
CFLAGS	= /define=(VMS, LNK960_LITTLE) /cc1_options="-mvaxc-alignment"  /inc=([],[-.include],[-.bfd], [-.include.aout])

all  : ld.exe
	copy descrip.mms all

BFD_OBJ	= [-.bfd]archive.obj, -
	[-.bfd]coff-i960.obj, -
	[-.bfd]format.obj, -
	[-.bfd]reloc.obj, -
	[-.bfd]archures.obj, -
	[-.bfd]core.obj, -
	[-.bfd]init.obj, -
	[-.bfd]section.obj, -
	[-.bfd]bfd.obj, -
	[-.bfd]cpu-i960.obj, -
	[-.bfd]libbfd.obj, -
	[-.bfd]srec.obj, -
	[-.bfd]cache.obj, -
	[-.bfd]ctor.obj, -
	[-.bfd]opncls.obj, -
	[-.bfd]targets.obj, -
	[-.bfd]syms.obj,-
	[-.bfd]seclet.obj

LIBIBERTY_OBJ = -
[]bzero.obj, getopt1.obj,obstack.obj,[-.libiberty]bcopy.obj,-
[-.libiberty]getopt.obj,[-.libiberty]index.obj,[-.libiberty]strdup.obj,alloca.obj

LD_OBJ = -
cplus-dem.obj, -
ld__a29k.obj, -
ld__ebmon29k.obj, -
ld__gld960.obj, -
ld__go32.obj, -
ld__h8300hms.obj, -
ld__h8300xray.obj, -
ld__hp300bsd.obj, -
ld__i386aout.obj, -
ld__lnk960.obj, -
ld__m88kbcs.obj, -
ld__news.obj, -
ld__st2000.obj, -
ld__sun3.obj, -
ld__sun4.obj, -
ld__vanilla.obj, -
ldctor.obj, -
ldemul.obj, -
lderror.obj, -
ldexp.obj, -
ldfile.obj, -
ldgram.obj, -
ldindr.obj, -
ldlang.obj, -
ldlex.obj, -
ldmain.obj, -
ldsym.obj, -
ldver.obj, -
ldwarn.obj, -
ldmisc.obj,-
ldwrite.obj, -
lexsup.obj, -
mri.obj, -
relax.obj

	
LIBS=$(BFD_OBJ), $(LIBIBERTY_OBJ)

ld.exe : $(LD_OBJ)
	copy [-.libiberty]bzero.obj []
	copy [-.libiberty]getopt1.obj []
	copy [-.libiberty]obstack.obj []
	$(LINK)/exe=ld.exe  $(LD_OBJ), $(LIBS)



