/* This testcase is part of GDB, the GNU debugger.

   Copyright 2014 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

	.text
	.globl	func
func:
	.long	0x7c642e98	/* <+0>:   lxvd2x  vs3,r4,r5         */
	.long	0x7c642ed8	/* <+4>:   lxvd2ux vs3,r4,r5         */
	.long	0x7d642e99	/* <+8>:   lxvd2x  vs43,r4,r5        */
	.long	0x7d642ed9	/* <+12>:  lxvd2ux vs43,r4,r5        */
	.long	0x7c642f98	/* <+16>:  stxvd2x vs3,r4,r5         */
	.long	0x7c642fd8	/* <+20>:  stxvd2ux vs3,r4,r5        */
	.long	0x7d642f99	/* <+24>:  stxvd2x vs43,r4,r5        */
	.long	0x7d642fd9	/* <+28>:  stxvd2ux vs43,r4,r5       */
	.long	0xf0642850	/* <+32>:  xxmrghd vs3,vs4,vs5       */
	.long	0xf16c6857	/* <+36>:  xxmrghd vs43,vs44,vs45    */
	.long	0xf0642b50	/* <+40>:  xxmrgld vs3,vs4,vs5       */
	.long	0xf16c6b57	/* <+44>:  xxmrgld vs43,vs44,vs45    */
	.long	0xf0642850	/* <+48>:  xxmrghd vs3,vs4,vs5       */
	.long	0xf16c6857	/* <+52>:  xxmrghd vs43,vs44,vs45    */
	.long	0xf0642b50	/* <+56>:  xxmrgld vs3,vs4,vs5       */
	.long	0xf16c6b57	/* <+60>:  xxmrgld vs43,vs44,vs45    */
	.long	0xf0642950	/* <+64>:  xxpermdi vs3,vs4,vs5,1    */
	.long	0xf16c6957	/* <+68>:  xxpermdi vs43,vs44,vs45,1 */
	.long	0xf0642a50	/* <+72>:  xxpermdi vs3,vs4,vs5,2    */
	.long	0xf16c6a57	/* <+76>:  xxpermdi vs43,vs44,vs45,2 */
	.long	0xf0642780	/* <+80>:  xvmovdp vs3,vs4           */
	.long	0xf16c6787	/* <+84>:  xvmovdp vs43,vs44         */
	.long	0xf0642780	/* <+88>:  xvmovdp vs3,vs4           */
	.long	0xf16c6787	/* <+92>:  xvmovdp vs43,vs44         */
	.long	0xf0642f80	/* <+96>:  xvcpsgndp vs3,vs4,vs5     */
	.long	0xf16c6f87	/* <+100>: xvcpsgndp vs43,vs44,vs45  */
	.long	0x7c00007c	/* <+104>: wait                      */
	.long	0x7c00007c	/* <+108>: wait                      */
	.long	0x7c20007c	/* <+112>: waitrsv                   */
	.long	0x7c20007c	/* <+116>: waitrsv                   */
	.long	0x7c40007c	/* <+120>: waitimpl                  */
	.long	0x7c40007c	/* <+124>: waitimpl                  */
	.long	0x4c000324	/* <+128>: doze                      */
	.long	0x4c000364	/* <+132>: nap                       */
	.long	0x4c0003a4	/* <+136>: sleep                     */
	.long	0x4c0003e4	/* <+140>: rvwinkle                  */
	.long	0x7c830134	/* <+144>: prtyw   r3,r4             */
	.long	0x7dcd0174	/* <+148>: prtyd   r13,r14           */
	.long	0x7d5c02a6	/* <+152>: mfcfar  r10               */
	.long	0x7d7c03a6	/* <+156>: mtcfar  r11               */
	.long	0x7c832bf8	/* <+160>: cmpb    r3,r4,r5          */
	.long	0x7d4b662a	/* <+164>: lwzcix  r10,r11,r12       */
	.long	0xee119004	/* <+168>: dadd    f16,f17,f18       */
	.long	0xfe96c004	/* <+172>: daddq   f20,f22,f24       */
	.long	0x7c60066c	/* <+176>: dss     3                 */
	.long	0x7e00066c	/* <+180>: dssall                    */
	.long	0x7c2522ac	/* <+184>: dst     r5,r4,1           */
	.long	0x7e083aac	/* <+188>: dstt    r8,r7,0           */
	.long	0x7c6532ec	/* <+192>: dstst   r5,r6,3           */
	.long	0x7e442aec	/* <+196>: dststt  r4,r5,2           */
	.long	0x7d4b6356	/* <+200>: divwe   r10,r11,r12       */
	.long	0x7d6c6b57	/* <+204>: divwe.  r11,r12,r13       */
	.long	0x7d8d7756	/* <+208>: divweo  r12,r13,r14       */
	.long	0x7dae7f57	/* <+212>: divweo. r13,r14,r15       */
	.long	0x7d4b6316	/* <+216>: divweu  r10,r11,r12       */
	.long	0x7d6c6b17	/* <+220>: divweu. r11,r12,r13       */
	.long	0x7d8d7716	/* <+224>: divweuo r12,r13,r14       */
	.long	0x7dae7f17	/* <+228>: divweuo. r13,r14,r15      */
	.long	0x7e27d9f8	/* <+232>: bpermd  r7,r17,r27        */
	.long	0x7e8a02f4	/* <+236>: popcntw r10,r20           */
	.long	0x7e8a03f4	/* <+240>: popcntd r10,r20           */
	.long	0x7e95b428	/* <+244>: ldbrx   r20,r21,r22       */
	.long	0x7e95b528	/* <+248>: stdbrx  r20,r21,r22       */
	.long	0x7d4056ee	/* <+252>: lfiwzx  f10,0,r10         */
	.long	0x7d4956ee	/* <+256>: lfiwzx  f10,r9,r10        */
	.long	0xec802e9c	/* <+260>: fcfids  f4,f5             */
	.long	0xec802e9d	/* <+264>: fcfids. f4,f5             */
	.long	0xec802f9c	/* <+268>: fcfidus f4,f5             */
	.long	0xec802f9d	/* <+272>: fcfidus. f4,f5            */
	.long	0xfc80291c	/* <+276>: fctiwu  f4,f5             */
	.long	0xfc80291d	/* <+280>: fctiwu. f4,f5             */
	.long	0xfc80291e	/* <+284>: fctiwuz f4,f5             */
	.long	0xfc80291f	/* <+288>: fctiwuz. f4,f5            */
	.long	0xfc802f5c	/* <+292>: fctidu  f4,f5             */
	.long	0xfc802f5d	/* <+296>: fctidu. f4,f5             */
	.long	0xfc802f5e	/* <+300>: fctiduz f4,f5             */
	.long	0xfc802f5f	/* <+304>: fctiduz. f4,f5            */
	.long	0xfc802f9c	/* <+308>: fcfidu  f4,f5             */
	.long	0xfc802f9d	/* <+312>: fcfidu. f4,f5             */
	.long	0xfc0a5900	/* <+316>: ftdiv   cr0,f10,f11       */
	.long	0xff8a5900	/* <+320>: ftdiv   cr7,f10,f11       */
	.long	0xfc005140	/* <+324>: ftsqrt  cr0,f10           */
	.long	0xff805140	/* <+328>: ftsqrt  cr7,f10           */
	.long	0x7e084a2c	/* <+332>: dcbtt   r8,r9             */
	.long	0x7e0849ec	/* <+336>: dcbtstt r8,r9             */
	.long	0xed406644	/* <+340>: dcffix  f10,f12           */
	.long	0xee80b645	/* <+344>: dcffix. f20,f22           */
	.long	0x7d4b6068	/* <+348>: lbarx   r10,r11,r12       */
	.long	0x7d4b6068	/* <+352>: lbarx   r10,r11,r12       */
	.long	0x7d4b6069	/* <+356>: lbarx   r10,r11,r12,1     */
	.long	0x7e95b0e8	/* <+360>: lharx   r20,r21,r22       */
	.long	0x7e95b0e8	/* <+364>: lharx   r20,r21,r22       */
	.long	0x7e95b0e9	/* <+368>: lharx   r20,r21,r22,1     */
	.long	0x7d4b656d	/* <+372>: stbcx.  r10,r11,r12       */
	.long	0x7d4b65ad	/* <+376>: sthcx.  r10,r11,r12       */
	.long	0xfdc07830	/* <+380>: fre     f14,f15           */
	.long	0xfdc07831	/* <+384>: fre.    f14,f15           */
	.long	0xedc07830	/* <+388>: fres    f14,f15           */
	.long	0xedc07831	/* <+392>: fres.   f14,f15           */
	.long	0xfdc07834	/* <+396>: frsqrte f14,f15           */
	.long	0xfdc07835	/* <+400>: frsqrte. f14,f15          */
	.long	0xedc07834	/* <+404>: frsqrtes f14,f15          */
	.long	0xedc07835	/* <+408>: frsqrtes. f14,f15         */
	.long	0x7c43271e	/* <+412>: isel    r2,r3,r4,28       */
	.long	0x7c284fec	/* <+416>: dcbzl   r8,r9             */
	.long	0xed405834	/* <+420>: frsqrtes f10,f11          */
	.long	0xec220804	/* <+424>: dadd    f1,f2,f1          */
	.long	0xfc020004	/* <+428>: daddq   f0,f2,f0          */
	.long	0xec220c04	/* <+432>: dsub    f1,f2,f1          */
	.long	0xfc020404	/* <+436>: dsubq   f0,f2,f0          */
	.long	0xec220844	/* <+440>: dmul    f1,f2,f1          */
	.long	0xfc020044	/* <+444>: dmulq   f0,f2,f0          */
	.long	0xec220c44	/* <+448>: ddiv    f1,f2,f1          */
	.long	0xfc020444	/* <+452>: ddivq   f0,f2,f0          */
	.long	0xec820d04	/* <+456>: dcmpu   cr1,f2,f1         */
	.long	0xfc820504	/* <+460>: dcmpuq  cr1,f2,f0         */
