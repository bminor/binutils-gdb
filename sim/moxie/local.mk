## See sim/Makefile.am
##
## Copyright (C) 1993-2022 Free Software Foundation, Inc.
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

dtbdir = $(datadir)/gdb/dtb

dtb_DATA = %D%/moxie-gdb.dtb

%D%/moxie-gdb.dtb: @MAINT@ %D%/moxie-gdb.dts %D%/$(am__dirstamp)
	$(AM_V_GEN) \
	if test "x$(DTC)" != x; then \
	  $(DTC) -O dtb -o $@.tmp ${srcdir}/%D%/moxie-gdb.dts || exit 1; \
	  $(SHELL) $(srcroot)/move-if-change $@.tmp ${srcdir}/%D%/moxie-gdb.dtb || exit 1; \
	  touch ${srcdir}/%D%/moxie-gdb.dtb; \
	else \
	  echo "Could not update the moxie-gdb.dtb file because the device "; \
	  echo "tree compiler tool (dtc) is missing.  Install the tool to "; \
	  echo "update the device tree blob."; \
	fi
