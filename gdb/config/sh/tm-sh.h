/* Target-specific definition for a Hitachi Super-H.
   Copyright (C) 1993, 2000 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Contributed by Steve Chamberlain sac@cygnus.com */

#define GDB_MULTI_ARCH 1

/* Information that is dependent on the processor variant. */
struct gdbarch_tdep
  {
    int FPUL_REGNUM;  /*                       sh3e, sh4 */
    int FPSCR_REGNUM; /*                       sh3e, sh4 */
    int DSR_REGNUM;   /* sh-dsp,      sh3-dsp            */
    int FP15_REGNUM;  /*                       sh3e, sh4 */
    int A0G_REGNUM;   /* sh-dsp,      sh3-dsp            */
    int A0_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int A1G_REGNUM;   /* sh-dsp,      sh3-dsp            */
    int A1_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int M0_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int M1_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int X0_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int X1_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int Y0_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int Y1_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int MOD_REGNUM;   /* sh-dsp,      sh3-dsp            */
    int SSR_REGNUM;   /*         sh3, sh3-dsp, sh3e, sh4 */
    int SPC_REGNUM;   /*         sh3, sh3-dsp, sh3e, sh4 */
    int RS_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int RE_REGNUM;    /* sh-dsp,      sh3-dsp            */
    int DR0_REGNUM;   /*                             sh4 */
    int DR2_REGNUM;   /*                             sh4 */
    int DR4_REGNUM;   /*                             sh4 */
    int DR6_REGNUM;   /*                             sh4 */
    int DR8_REGNUM;   /*                             sh4 */
    int DR10_REGNUM;  /*                             sh4 */
    int DR12_REGNUM;  /*                             sh4 */
    int DR14_REGNUM;  /*                             sh4 */
    int FV0_REGNUM;   /*                             sh4 */
    int FV4_REGNUM;   /*                             sh4 */
    int FV8_REGNUM;   /*                             sh4 */
    int FV12_REGNUM;  /*                             sh4 */
  };

/* Registers common to all the SH variants. */
enum
  {
    R0_REGNUM = 0,
    STRUCT_RETURN_REGNUM = 2,
    ARG0_REGNUM = 4,
    ARGLAST_REGNUM = 7,
    PR_REGNUM = 17,
    GBR_REGNUM = 18,
    VBR_REGNUM = 19,
    MACH_REGNUM = 20,
    MACL_REGNUM = 21,
    SR_REGNUM = 22
  };

/* Define DO_REGISTERS_INFO() to do machine-specific formatting
   of register dumps. */
extern void sh_do_registers_info (int regnum, int fpregs);
#undef  DO_REGISTERS_INFO
#define DO_REGISTERS_INFO(REGNUM, FP) sh_do_registers_info(REGNUM, FP)

#define NUM_REALREGS 59 /* used in remote-e7000.c which is not multiarched. */

#define REGISTER_TYPE  long /* used in standalone.c */

#define BIG_REMOTE_BREAKPOINT    { 0xc3, 0x20 } /* Used in remote.c */
#define LITTLE_REMOTE_BREAKPOINT { 0x20, 0xc3 } /* Used in remote.c */

/*#define NOP   {0x20, 0x0b}*/ /* Who uses this???*/

