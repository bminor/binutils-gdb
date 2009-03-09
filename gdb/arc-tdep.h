/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Soam Vasani <soam.vasani@codito.com>
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com> 

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/
enum arc700_api_regnums
  {
    ARC_ARG0_REGNUM     =        0,
    ARC_ARG1_REGNUM     =        1,
    ARC_ARG2_REGNUM     =        2,
    ARC_ARG3_REGNUM     =        3,
    ARC_ARG4_REGNUM     =        4,
    ARC_ARG5_REGNUM     =        5,
    ARC_ARG6_REGNUM     =        6,
    ARC_ARG7_REGNUM     =        7,

    /* When a return value is stored in registers, is in either r0 or in
       (r1,r0).  Used in arc_extract_return_value */
    ARC_RETURN1_REGNUM	=	 0,
    ARC_RETURN2_REGNUM 	=	 1
  };



enum ARCProcessorVersion
  {
    UNSUPPORTED,
    ARCompact,
    ARC600,
    ARC700,
    A5,
    A4,
  };


enum ARCExtensionsSupportedInformation
  {
    ARC700_MMU
  };



typedef struct ARCProcessorInformation
{
  enum ARCProcessorVersion arcprocessorversion;
  enum ARCExtensionsSupportedInformation extensionsSupported;

}ARCVariantsInfo;

struct gdbarch_tdep
{
  /* Detect sigtramp.  */
  int (*sigtramp_p) (struct frame_info *);
  
  /* Get address of sigcontext for sigtramp.  */
  CORE_ADDR (*sigcontext_addr) (struct frame_info *);

  /* Offset of registers in `struct sigcontext'.  */
  int *sc_reg_offset;
  int sc_num_regs;

  /* In our linux target, gdbarch_pc_regnum points to stop_pc, which is a
     register that's made-up by the kernel and does not actually exist.
     stop_pc is NOT saved in the sigcontext; what is saved is the ret
     "register".  Since ret is a linux-only register, it's regnum is visible
     only in arc-linux-tdep.c; hence initialize pc_regnum_in_sigcontext in
     arc-linux-tdep.c.  */
  int pc_regnum_in_sigcontext;

  /* Returns false, true, or -1; -1 means the tdep has nothing to say about this
     register and group.  */
  int (*register_reggroup_p) (int, struct reggroup *);
  
  /* Breakpoint instruction to be used */
  unsigned char * arc_breakpoint_insn;
  int arc_breakpoint_size;

  /* For stopping backtraces.  */
  CORE_ADDR lowest_pc;
  
  /* ARC Processor variant information.  */
  ARCVariantsInfo * arc_processor_variant_info ;

};


void arc_software_single_step(enum target_signal ignore, int insert_breakpoints_p);

