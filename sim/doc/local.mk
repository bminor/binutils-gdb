## See sim/Makefile.am.
##
## Copyright (C) 1997-2023 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

TEXINFO_TEX = %D%/sim.texi
info_TEXINFOS = $(TEXINFO_TEX)
%C%_sim_TEXINFOS = \
	%D%/abi-newlib-errno.texi \
	%D%/abi-newlib-open.texi \
	%D%/abi-newlib-signal.texi \
	%D%/abi-newlib-sys.texi \
	%D%/arch-aarch64.texi \
	%D%/arch-arm.texi \
	%D%/arch-avr.texi \
	%D%/arch-bfin.texi \
	%D%/arch-bpf.texi \
	%D%/arch-cr16.texi \
	%D%/arch-cris.texi \
	%D%/arch-d10v.texi \
	%D%/arch-erc32.texi \
	%D%/arch-frv.texi \
	%D%/arch-ft32.texi \
	%D%/arch-h8300.texi \
	%D%/arch-iq2000.texi \
	%D%/arch-lm32.texi \
	%D%/arch-m32c.texi \
	%D%/arch-m32r.texi \
	%D%/arch-m68hc11.texi \
	%D%/arch-mcore.texi \
	%D%/arch-microblaze.texi \
	%D%/arch-mips.texi \
	%D%/arch-mn10300.texi \
	%D%/arch-moxie.texi \
	%D%/arch-msp430.texi \
	%D%/arch-or1k.texi \
	%D%/arch-ppc.texi \
	%D%/arch-pru.texi \
	%D%/arch-riscv.texi \
	%D%/arch-rl78.texi \
	%D%/arch-rx.texi \
	%D%/arch-sh.texi \
	%D%/arch-v850.texi \
	%D%/fdl.texi \
	%D%/simver.texi

AM_MAKEINFOFLAGS = -I %D% --no-split --css-include=sim.css
TEXI2DVI = texi2dvi -I %D%

%D%/simver.texi: %D%/simver.texi-stamp ; @true
%D%/simver.texi-stamp: $(srcroot)/gdb/version.in $(srcroot)/bfd/version.h %D%/create-version.sh
	$(AM_V_at)$(MKDIR_P) %D%
	$(AM_V_GEN)$(SHELL) $(srcdir)/%D%/create-version.sh $(srcroot)/gdb $@.tmp \
		"$(PACKAGE_URL)" "$(REPORT_BUGS_TEXI)"
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change $@.tmp $(@:-stamp=)
	$(AM_V_at)touch $@

$(HTMLS): %D%/sim.css

html-local: %D%/sim/index.html
%D%/sim/index.html: $(TEXINFO_TEX) $(%C%_sim_TEXINFOS) %D%/sim.css
	$(AM_V_MAKEINFO)$(MAKEINFOHTML) $(AM_MAKEINFOHTMLFLAGS) $(MAKEINFOFLAGS) \
		--output=%D%/sim --split=node -I$(srcdir) $<

MAINTAINERCLEANFILES += %D%/sim.info
