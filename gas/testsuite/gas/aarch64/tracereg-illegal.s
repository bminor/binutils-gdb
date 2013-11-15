/* tracereg-illegal.s Test file for AArch64 trace unit registers.
   Reject writing to registers that are read-only and reading from
   registers that are write-only.

   Copyright 2013 Free Software Foundation, Inc.
   Contributed by ARM Ltd.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the license, or
   (at your option) any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

	.macro rw_sys_reg sys_reg xreg r w
	.ifc \w, 1
	msr \sys_reg, \xreg
	.endif
	.ifc \r, 1
	mrs \xreg, \sys_reg
	.endif
	.endm

	.text

	rw_sys_reg sys_reg=trcstatr xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr8 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr9 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr10 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr11 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr12 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr13 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr0 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr1 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr2 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr3 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr4 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr5 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr6 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcidr7 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcoslar xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcoslsr xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpdsr xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcdevaff0 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcdevaff1 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trclar xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trclsr xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcauthstatus xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcdevarch xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcdevid xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcdevtype xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr4 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr5 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr6 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr7 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr0 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr1 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr2 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcpidr3 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trccidr0 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trccidr1 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trccidr2 xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trccidr3 xreg=x7 r=0 w=1
