# Copyright (C) 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

# This file is part of GDB.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# This file requires GNU make!

# This Makefile updates files in ../regformats from their XML master
# copies.  Because this relies on xsltproc, it is not normally run
# during the build process; it should be run by hand when one of the
# affected XML files is changed, and the results should be kept in the
# GDB repository.

# It can also update the C files in the features directory from their
# XML master copies.  This relies on a GDB linked with expat and
# configured for the correct architecture, so the files are again kept
# in the GDB repository.  To generate C files:
#   make GDB=/path/to/gdb XMLTOC="xml files" cfiles

WHICH = arm-with-iwmmxt arm-with-vfpv2 arm-with-vfpv3 arm-with-neon \
	mips-linux mips64-linux \
	rs6000/powerpc-32l rs6000/powerpc-altivec32l rs6000/powerpc-e500l \
	rs6000/powerpc-64l rs6000/powerpc-altivec64l rs6000/powerpc-vsx32l \
	rs6000/powerpc-vsx64l rs6000/powerpc-cell32l rs6000/powerpc-cell64l \
	s390-linux32 s390-linux64 s390x-linux64

# Record which registers should be sent to GDB by default after stop.
arm-expedite = r11,sp,pc
mips-expedite = r29,pc
mips64-expedite = r29,pc
powerpc-expedite = r1,pc
rs6000/powerpc-cell32l-expedite = r1,pc,r0,orig_r3,r4
rs6000/powerpc-cell64l-expedite = r1,pc,r0,orig_r3,r4
s390-linux32-expedite = r14,r15,pswa
s390-linux64-expedite = r14l,r15l,pswa
s390x-linux64-expedite = r14,r15,pswa


XSLTPROC = xsltproc
outdir = ../regformats
OUTPUTS = $(patsubst %,$(outdir)/%.dat,$(WHICH))

XMLTOC =
CFILES = $(patsubst %.xml,%.c,$(XMLTOC))
GDB = false

all: $(OUTPUTS)

$(outdir)/%.dat: %.xml number-regs.xsl sort-regs.xsl gdbserver-regs.xsl
	echo "# DO NOT EDIT: generated from $<" > $(outdir)/$*.tmp
	echo "name:`echo $(notdir $*) | sed 's/-/_/g'`" >> $(outdir)/$*.tmp
	echo "xmltarget:$(<F)" >> $(outdir)/$*.tmp
	echo "expedite:$(if $($*-expedite),$($*-expedite),$($(firstword $(subst -, ,$(notdir $*)))-expedite))" \
	  >> $(outdir)/$*.tmp
	$(XSLTPROC) --path "$(PWD)" --xinclude number-regs.xsl $< | \
	  $(XSLTPROC) sort-regs.xsl - | \
	  $(XSLTPROC) gdbserver-regs.xsl - >> $(outdir)/$*.tmp
	sh ../../move-if-change $(outdir)/$*.tmp $(outdir)/$*.dat

cfiles: $(CFILES)
%.c: %.xml
	$(GDB) -nx -q -batch \
	  -ex "set tdesc filename $<" -ex 'maint print c-tdesc' > $@.tmp
	sh ../../move-if-change $@.tmp $@

# Other dependencies.
$(outdir)/arm-with-iwmmxt.dat: arm-core.xml xscale-iwmmxt.xml
