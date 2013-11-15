/* tracereg.s Test file for AArch64 trace unit registers.

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

	rw_sys_reg sys_reg=trcprgctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcprocselr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcstatr xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcconfigr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcauxctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trceventctl0r xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trceventctl1r xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcstallctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trctsctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsyncpr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcccctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcbbctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trctraceidr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcqctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvictlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcviiectlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvissctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvipcssctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvdctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvdsacctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvdarcctlr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcseqevr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcseqevr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcseqevr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcseqrstevr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcseqstr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcextinselr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntrldvr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntrldvr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntrldvr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntrldvr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntctlr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntctlr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntctlr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntctlr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntvr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntvr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntvr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccntvr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcidr8 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr9 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr10 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr11 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr12 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr13 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcimspec0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcimspec7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcidr0 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr1 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr2 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr3 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr4 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr5 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr6 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcidr7 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcrsctlr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr8 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr9 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr10 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr11 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr12 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr13 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr14 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr15 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr16 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr17 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr18 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr19 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr20 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr21 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr22 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr23 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr24 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr25 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr26 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr27 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr28 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr29 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr30 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcrsctlr31 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcssccr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsscsr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcsspcicr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcoslar xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trcoslsr xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpdcr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcpdsr xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcacvr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr8 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr9 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr10 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr11 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr12 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr13 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr14 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacvr15 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr8 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr9 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr10 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr11 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr12 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr13 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr14 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcacatr15 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcvr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdvcmr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcvr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr2 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr3 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr4 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr5 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr6 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcvr7 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcctlr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trccidcctlr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcctlr0 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcvmidcctlr1 xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcitctrl xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcclaimset xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcclaimclr xreg=x7 r=1 w=1
	rw_sys_reg sys_reg=trcdevaff0 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcdevaff1 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trclar xreg=x7 r=0 w=1
	rw_sys_reg sys_reg=trclsr xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcauthstatus xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcdevarch xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcdevid xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcdevtype xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr4 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr5 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr6 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr7 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr0 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr1 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr2 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trcpidr3 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trccidr0 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trccidr1 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trccidr2 xreg=x7 r=1 w=0
	rw_sys_reg sys_reg=trccidr3 xreg=x7 r=1 w=0
