##Copyright (C) 1991 Free Software Foundation, Inc.

# This file is part of GDB.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# main GDB source directory
srcdir = ..

# Documentation (gdb.dvi) needs either GNU m4 or SysV m4; 
# Berkeley/Sun don't have quite enough. 
#M4=/usr/5bin/m4
M4=gm4

# where to find texinfo; GDB dist should include a recent one
TEXIDIR=${srcdir}/../texinfo/fsf

# where to find makeinfo, preferably one designed for texinfo-2
MAKEINFO=makeinfo

# Where is the source dir for the READLINE library?  Traditionally in .. or .
# (For the binary library built from it, we use ${READLINE_DIR}${subdir}.)
READLINE_DIR = ${srcdir}/../readline

# Main GDB manual's source files
SFILES_DOCDIR = \
	gdb.texinfo pretex.m4 none.m4 all.m4 gdbinv-m.m4 gdbinv-s.m4   

all: gdb.info gdb.dvi refcard.dvi gdb-internals gdbint.dvi

clean:
	rm -f gdb.dvi rdl-apps.texi gdb-all* gdb.info* 
	rm -f gdb-internals gdbint.?? gdbint.???
	rm -f refcard.ps refcard.dvi rcfonts.tex refcard.log *~

# GDB QUICK REFERENCE (TeX dvi file, CM fonts)
refcard.dvi : refcard.tex
	rm -f rcfonts.tex; cp rc-cm.tex rcfonts.tex
	TEXINPUTS=.:$$TEXINPUTS tex refcard.tex; rm -f refcard.log 

# GDB QUICK REFERENCE (PostScript output, common PS fonts)
refcard.ps : refcard.tex
	rm -f rcfonts.tex; cp rc-ps.tex rcfonts.tex
	TEXINPUTS=.:$$TEXINPUTS tex refcard.tex
	dvips -t landscape refcard -o; rm -f refcard.dvi refcard.log

# Cover file for "Readline" appendices
rdl-apps.texi: ${READLINE_DIR}/inc-readline.texinfo \
			${READLINE_DIR}/inc-history.texinfo
	rm -f rdl-apps.texi
	echo "@include ${READLINE_DIR}/inc-readline.texinfo" >rdl-apps.texi
	echo "@include ${READLINE_DIR}/inc-history.texinfo" >>rdl-apps.texi

# GDB MANUAL: texinfo source, created by preprocessing w/m4
# Be sure to not create a bad gdb-all.texi if ${M4} is missing or aborts...
gdb-all.texi: ${SFILES_DOCDIR}
	rm -f foobus.texinfo
	${M4} pretex.m4 none.m4 all.m4 gdb.texinfo  >foobus.texinfo
	rm -f gdb-all.texi
	mv foobus.texinfo gdb-all.texi

# GDB MANUAL: TeX dvi file
gdb.dvi : gdb-all.texi rdl-apps.texi
	TEXINPUTS=${TEXIDIR}:$$TEXINPUTS tex gdb-all.texi
	texindex gdb-all.??
	TEXINPUTS=${TEXIDIR}:$$TEXINPUTS tex gdb-all.texi
	mv gdb-all.dvi gdb.dvi
	rm -f gdb-all.?? gdb-all.???

# GDB MANUAL: info file
# We're using texinfo2, and older makeinfo's may not be able to
# cope with all the markup.  In the meantime, we distribute the info
# files 
gdb.info: gdb-all.texi
	${MAKEINFO} gdb-all.texi

# GDB INTERNALS MANUAL: TeX dvi file
gdbint.dvi : gdbint.texinfo
	TEXINPUTS=${TEXIDIR}:$$TEXINPUTS tex gdbint.texinfo
	texindex gdbint.??
	TEXINPUTS=${TEXIDIR}:$$TEXINPUTS tex gdbint.texinfo
	rm -f gdbint.?? gdbint.aux gdbint.cps gdbint.fns gdbint.kys \
		gdbint.log gdbint.pgs gdbint.toc gdbint.tps gdbint.vrs

# GDB INTERNALS MANUAL: info file
gdb-internals : gdbint.texinfo
	${MAKEINFO} gdbint.texinfo




