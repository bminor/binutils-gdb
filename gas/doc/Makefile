# Makefile for as.texinfo preprocessing and TeXing
# $Id$
# see pretex.m4 for discussion of preprocessor definitions

# What version of the manual you want (see *.m4); "all" includes everything
CONFIG=all

# Sun/Berkeley m4 doesn't have all the things we need; use GNU or sV
#M4=gm4
M4=/usr/5bin/m4

# Directory for gas source
srcdir=..

# Where to find texinfo.tex to format docn with TeX
TEXIDIR = $(srcdir)/../texinfo/fsf

# There's no index yet; when there is, add texindex call before tex 2nd pass
as.dvi:	as-${CONFIG}.texinfo
	TEXINPUTS=${TEXIDIR}:.:$$TEXINPUTS tex as-${CONFIG}.texinfo
	TEXINPUTS=${TEXIDIR}:.:$$TEXINPUTS tex as-${CONFIG}.texinfo
	mv as-${CONFIG}.dvi as.dvi
	rm as-${CONFIG}.?? as-${CONFIG}.???

as.info: as-${CONFIG}.texinfo
	makeinfo as-${CONFIG}.texinfo

# roff output (-ms)
#edit instances of "ms" to "me" or "mm" to suit your preferences.
# (we don't use a variable because we don't trust all makes to handle
# a var in the target name right).
as.ms: as-${CONFIG}.texinfo
	sed -e '/\\input texinfo/d' \
		-e '/@c TEXI2ROFF-KILL/,/@c END TEXI2ROFF-KILL/d' \
		as-${CONFIG}.texinfo | \
	texi2roff -ms >as.ms 



as-all.texinfo: as.texinfo pretex.m4 none.m4 all.m4
	${M4} pretex.m4 none.m4 all.m4 as.texinfo >as-all.texinfo

as-amd29k.texinfo: as.texinfo pretex.m4 none.m4 amd29k.m4
	${M4} pretex.m4 none.m4 amd29k.m4 as.texinfo >as-amd29k.texinfo

as-gen.texinfo: as.texinfo pretex.m4 none.m4 gen.m4
	${M4} pretex.m4 none.m4 gen.m4 as.texinfo >as-gen.texinfo

as-i80386.texinfo: as.texinfo pretex.m4 none.m4 i80386.m4
	${M4} pretex.m4 none.m4 i80386.m4 as.texinfo >as-i80386.texinfo

as-i960.texinfo: as.texinfo pretex.m4 none.m4 i960.m4
	${M4} pretex.m4 none.m4 i960.m4 as.texinfo >as-i960.texinfo

as-m680x0.texinfo: as.texinfo pretex.m4 none.m4 m680x0.m4
	${M4} pretex.m4 none.m4 m680x0.m4 as.texinfo >as-m680x0.texinfo

as-sparc.texinfo: as.texinfo pretex.m4 none.m4 sparc.m4
	${M4} pretex.m4 none.m4 sparc.m4 as.texinfo >as-sparc.texinfo

as-vax.texinfo: as.texinfo pretex.m4 none.m4 vax.m4
	${M4} pretex.m4 none.m4 vax.m4 as.texinfo >as-vax.texinfo

as-vintage.texinfo: as.texinfo pretex.m4 none.m4 vintage.m4
	${M4} pretex.m4 none.m4 vintage.m4 as.texinfo >as-vintage.texinfo

clean:
	rm -f as-${CONFIG}.* as.dvi
