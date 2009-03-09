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
#include <string.h>

#include "defs.h"
#include "osabi.h"
#include "frame.h"
#include "regcache.h"
#include "gdb_assert.h"
#include "inferior.h"
#include "gdbcmd.h"
#include "reggroups.h"

#include "arc-tdep.h"
#include "arc-jtag.h"


#ifdef ARC4_JTAG
/* brk */
unsigned int a4_jtag_breakpoint_size = 4;
unsigned char a4_jtag_breakpoint_insn[4] = { 0x00, 0xfe, 0xff, 0x1f };
#define A4_HALT_VALUE 0x02000000
#else
/* brk_s */
unsigned int arc700_jtag_breakpoint_size = 2;
unsigned char arc700_jtag_breakpoint_insn[2] = { 0xff, 0x7f };
#endif



struct arc_reg_info 
{
  char *name ;
  int hw_regno;
  char *description;
#ifdef ARC4_JTAG
  enum arc4_jtag_regnums gdbregno;
#else
  enum arc700_jtag_regnums gdbregno;
#endif
  enum ARCProcessorVersion arcVersionSupported;
};



static const char *
arc_jtag_register_name (int regno)
{
  static char jtag_names[][30] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6",
    "r7", "r8", "r9", "r10", "r11", "r12", "r13",
    "r14", "r15", "r16", "r17", "r18", "r19", "r20",
    "r21", "r22", "r23", "r24", "r25", "r26",
    
    "fp",
    "sp",
    "ilink1",
    "ilink2",
    "blink",
    
    /* Extension core regs are 32..59 inclusive.  */
    "r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39",
    "r40", "r41", "r42", "r43", "r44", "r45", "r46", "r47", "r48", "r49",
    "r50", "r51", "r52", "r53", "r54", "r55", "r56", "r57", "r58", "r59",
       
    "lp_count",

    /* 61 is reserved, 62 is not a real register.  */
    /*FIXMEA: The following 3 are supposed to be registers
     that are used only to encode immediate values in A4*/
    "r61",
    "r62",
    
    "pcl",

    /* Now the aux registers.  */
    
    "status",
    "semaphore",
    "lp_start",
    "lp_end",
    "identity",
    "debug",
    
#ifndef ARC4_JTAG
    "pc",
    "status32",
    "status32_l1",
    "status32_l2",

    "count0",
    "control0",
    "limit0",
    "int_vector_base",
    "aux_macmode",
    "aux_irq_lv12",


    "count1",
    "control1",
    "limit1",
    "aux_irq_lev",
    "aux_irq_hint",
    "eret",
    "erbta",
    "erstatus",
    "ecr",
    "efa",
    "icause1",
    "icause2",
    "aux_ienable",
    "aux_itrigger",
    "xpu",
    "bta",
    "bta_l1",
    "bta_l2",
    "aux_irq_pulse_cancel",
    "aux_irq_pending",

    /* Build configuration registers.  */
    "bcr_0",
    "dccm_base_build",
    "crc_base_build",
    "bta_link_build",
    "dvbf_build",
    "tel_instr_build",
    "bcr_6",
    "memsubsys",
    "vecbase_ac_build",
    "p_base_address",
    "bcr_a",
    "bcr_b",
    "bcr_c",
    "bcr_d",
    "bcr_e",
    "mmu_build",
    "arcangel_build",
    "bcr_11",
    "d_cache_build",
    "madi_build",
    "dccm_build",
    "timer_build",
    "ap_build",
    "icache_build",
    "iccm_build",
    "dspram_build",
    "mac_build",
    "multiply_build",
    "swap_build",
    "norm_build",
    "minmax_build",
    "barrel_build",
#endif
    
  };
  
  gdb_assert(ARRAY_SIZE (jtag_names) == NUM_REGS + NUM_PSEUDO_REGS);
  gdb_assert(regno >=0 && regno < NUM_REGS + NUM_PSEUDO_REGS);

  return jtag_names[regno];
}

int
arc_jtag_register_reggroup_p (int regnum, struct reggroup *group)
{
  /* These registers don't exist, so they are not in any reggroup.  */
  if ((regnum >= 32 && regnum <= 59) || (regnum == 61) || (regnum == 62))
    return 0;

  /* Which regs to save/restore ? */
  if ((group == save_reggroup || group == restore_reggroup))
    {
      /* Save/restore:
	 1. all core regs, except PCL (PCL is not writable)
	 2. aux regs LP_START..LP_END (IDENTITY is not writable)
	 3. aux regs PC_REGNUM..STATUS32_L2
	 3. aux regs ERET..EFA */
      return ( ( regnum >= 0 && regnum < ARC_PCL_REGNUM)
	       || ( regnum >= ARC_LP_START_REGNUM && regnum<= ARC_LP_END_REGNUM)
#ifdef ARC4_JTAG
	       || ( regnum == ARC_STATUS_REGNUM)
#else	       
	       || ( regnum >= ARC_PC_REGNUM && regnum <= ARC_STATUS32_L2_REGNUM)
	       || ( regnum >= ARC_ERET_REGNUM && regnum <= ARC_EFA_REGNUM)
#endif
	       );
    }
  
  return -1;
}

static void
arc_jtag_print_registers_info (struct gdbarch *gdbarch, struct ui_file *file,
			       struct frame_info *frame, int regnum, int all)
{
  int i;

  if (regnum >= 0 )
    {
      default_print_registers_info (gdbarch, file, frame, regnum, all);
      return;
    }

  /* if regnum < 0 , print all registers */

  for (i=0; i <= 26; ++i)
    default_print_registers_info (gdbarch, file, frame, i, all);
  default_print_registers_info (gdbarch, file, frame, 
				ARC_FP_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, 
				ARC_SP_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, 
				ARC_ILINK1_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, 
				ARC_ILINK2_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, 
				ARC_BLINK_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, 
				ARC_LP_COUNT_REGNUM, all);
  
  /* now the aux registers */
   if (!all)
    {
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_LP_START_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_LP_END_REGNUM, all);
      
#ifndef ARC4_JTAG 
      default_print_registers_info (gdbarch, file,frame,
				    ARC_STATUS32_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_BTA_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_EFA_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_ERET_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_STATUS32_L1_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_STATUS32_L2_REGNUM, all);
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_ERSTATUS_REGNUM, all);
      
      
      /* PC */
      default_print_registers_info (gdbarch, file, frame, 
				    ARC_PC_REGNUM, all);
#endif
    }
   else
     {
       /* This part needs cleaning up. */
       for (i = ARC_STATUS_REGNUM;
#ifndef ARC4_JTAG
	    i <= ARC_AUX_IRQ_PENDING_REGNUM;
#else /*FIXMEA*/
	    i <=  ARC_DEBUG_REGNUM;
#endif
	    i ++ )
	 default_print_registers_info (gdbarch, file, frame , 
				       i, all);

       
       for (i = ARC_STATUS_REGNUM ; 
#ifndef ARC4_JTAG
	    i <= ARC_AUX_IRQ_PENDING_REGNUM;
#else /*FIXMEA*/
	    i <=  ARC_DEBUG_REGNUM;
#endif
	    i ++ )
	 default_print_registers_info (gdbarch, file, frame, 
				       i, all);
       
#ifndef ARC4_JTAG
       for (i = ARC_BCR_1_REGNUM ; 
	    i <= ARC_BCR_5_REGNUM ;
	    i ++ )
	 default_print_registers_info (gdbarch, file , frame, 
				       i , all);
       
       for (i = ARC_BCR_7_REGNUM ;
	    i <= ARC_BCR_9_REGNUM;
	    i ++ )
	 default_print_registers_info (gdbarch, file, frame, 
				       i , all);

       for (i = ARC_BCR_F_REGNUM;
	    i <= ARC_BCR_10_REGNUM;
	    i ++ )
	 default_print_registers_info (gdbarch, file, frame , 
				       i , all);

       for (i = ARC_BCR_12_REGNUM;
	    i <= ARC_BCR_1F_REGNUM;
	    i ++)
	 default_print_registers_info (gdbarch, file, frame , 
				       i , all);
#endif //if no ARC4_JTAG


     }
}

/* Command: aux-read <from> <to>

   Read and display a range of aux registers.  Some of the aux registers
   (pc, debug, etc.) are part of the register set, but this is a more
   general interface.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */
static void
arc_jtag_aux_read_command (char *arg, int from_tty)
{
  char *arg2 = 0;
  struct expression *expr;
  struct value *val;
  struct cleanup *old_chain = 0;
  int auxregno, auxregno2 = 0, nrregs;
  unsigned int *buf;
  int i, nrtransfered;

  if (!arg)
    {
      printf_filtered ("aux-read <REG-FROM> [<REG-TO>]\n");
      return;
    }
  
  /* strip leading spaces */
  while(*arg == ' ')
    arg++;

  /* two arguments ?  */
  /* This assumes that the first arg cannot have spaces.  (The disas command
     also seems to work this way.)  */
  arg2 = strchr (arg, ' ');

  /* get the second one */
  if (arg2)
    {
      struct expression *expr2;
      struct value *val2;

      arg2[0] = 0;
      arg2++;
     
      expr2 = parse_expression (arg2);
      val2 = evaluate_expression (expr2);
      xfree (expr2);

      auxregno2 = *(int *)(VALUE_CONTENTS (val2));
    }

  /* first arg */
  expr = parse_expression (arg);
  val = evaluate_expression (expr);
  old_chain = make_cleanup (free_current_contents, &expr);
  
  auxregno = *(int *)(VALUE_CONTENTS (val));

  /* so, how many regs do we want ? */
  if (arg2)
    {
      if (auxregno2 < auxregno)
	{
	  warning ("aux-read: %s < %s, showing one register", arg2, arg);
	  nrregs = 1;
	}
      else
	nrregs = auxregno2 - auxregno + 1;
    }
  else
    nrregs = 1;

  buf = xcalloc (nrregs, sizeof(int));
  make_cleanup (free_current_contents, &buf);
  
  /* Go get 'em !  */
  nrtransfered = target_read_aux_reg (buf, auxregno, nrregs);
  if (nrtransfered <= 0)
    {
      do_cleanups (old_chain);
      error ("aux-read: couldn't read any registers.");
    }
  else if (nrtransfered < nrregs)
    {
      warning ("aux-read: could only read %d registers", nrtransfered);
    }
  
  gdb_assert (nrtransfered <= nrregs);

  /* Show them.  */
  for (i = auxregno; i - auxregno < nrtransfered; ++i)
    {
      if ((i - auxregno) % 4 == 0)
	printf_filtered("%s%08x: ", ((i - auxregno) ? "\n" : ""), i);
      
      printf_filtered ("%08x ", buf[i - auxregno]);
    }
  printf_filtered ("\n");
  
  do_cleanups (old_chain);
}

/* aux-write <regnum> = <value>
   Write VALUE to aux register REGNUM. */
static void
arc_jtag_aux_write_command (char *arg, int from_tty)
{
  char *value_arg = 0;
  struct expression *regnum_expr, *value_expr;
  struct value *regnum_val, *value_val;
  struct cleanup *old_chain = 0;
  unsigned int regnum, value;
  int err;
  
  if (!arg)
    {
      printf_filtered ("aux-write <regnum> = <value>\n");
      return;
    }

  value_arg = strchr(arg, '=');
  if (!value_arg)
    {
      error ("aux-write: can't find second argument\n\
Usage: aux-write <regnum> = <value>");
      return;
    }
  value_arg[0] = 0;
  value_arg++;

  /* Regnum expression */
  regnum_expr = parse_expression (arg);
  regnum_val = evaluate_expression (regnum_expr);
  old_chain = make_cleanup (free_current_contents, &regnum_expr);
  regnum = *(unsigned int *)(VALUE_CONTENTS (regnum_val));

  /* Value expression */
  value_expr = parse_expression (value_arg);
  value_val = evaluate_expression (value_expr);
  make_cleanup (free_current_contents, &value_expr);
  value = *(unsigned int *)(VALUE_CONTENTS (value_val));

  /* Write it.  */
  err = target_write_aux_reg (&value, regnum, 1);
  if (err != 1)
    {
      do_cleanups (old_chain);
      error ("aux-write: couldn't write to register 0x%x", regnum);
    }

  do_cleanups (old_chain);
}

#ifdef ARC4_JTAG
//  gdbarch_write_pc_ftype *write_pc;
/*
  Write PC
  Arguments:
  1.CORE_ADDR val : Contains the value to be written into PC.
  2.ptid_t ptid: Process id of the process.

  Returns: void
  Description: FIXMEA: Update
      Reads the status register
      Inserts the value (upper 24 bit) into the bits
      0-23 in the status register
      Write the status register
 */
void
a4_jtag_write_pc (CORE_ADDR val, ptid_t ptid)
{
  CORE_ADDR insert_val = val >> 2;
  unsigned int buffer;


  if(debug_arc_jtag_target_message)
      printf_filtered ("\n -----***------------ a4_jtag_write_pc Entered ---*%%*#\n");

  
  target_read_aux_reg (&buffer, ARC_HW_STATUS_REGNUM, 1);

  if (!(buffer & A4_HALT_VALUE))
    {
      if(debug_arc_jtag_target_message)
	printf_filtered ("\n***** Halting Processor... *********\n");
      
      buffer = buffer | A4_HALT_VALUE ;
      target_write_aux_reg (&buffer, ARC_HW_STATUS_REGNUM, 1);
      /* Now the A4 processor has halted*/
    }
  
  if(debug_arc_jtag_target_message)
    printf_filtered (" \nWriting value %u to PC\n", val);


  target_read_aux_reg (&buffer, ARC_HW_STATUS_REGNUM, 1);
  if(debug_arc_jtag_target_message)
    printf_filtered (" \nValue of Status Register before writing %d\
                     \n Value of PC: 0x%x\n", buffer, buffer & 0x00ffffff);

  buffer = buffer & 0xff000000;
  insert_val    = insert_val & 0x00ffffff;
  buffer = buffer | insert_val ;
  
  if(debug_arc_jtag_target_message)
    printf_filtered (" \nValue of Status Register to be written %d\
                       \n Value of PC: 0x%x\n", buffer, buffer & 0x00ffffff);

  //  jtag_ops.jtag_write_aux_reg (ARC_STATUS_REGNUM, buffer);
  target_write_aux_reg (&buffer, ARC_HW_STATUS_REGNUM, 1);

  if(debug_arc_jtag_target_message)
    {
      target_read_aux_reg (&buffer, ARC_HW_STATUS_REGNUM, 1);
      printf_filtered (" \nValue of Status Register after reading again %d\
                        \n Value of PC: 0x%x\n", buffer, buffer & 0x00ffffff);
    }

    if(debug_arc_jtag_target_message)
      printf_filtered ("\n -----***------------ a4_jtag_write_pc Leaving ---*%%*#\n");

}


/*
  Read PC
  Arguments:
  1.ptid_t ptid: Process id of the process.

  Returns: CORE_ADDR 
  Description:
      Reads the status register
      Extracts the PC value from it.
      Right shift twice to get correct value of PC
      return PC
*/
CORE_ADDR
a4_jtag_read_pc (ptid_t ptid)
{
  unsigned int buffer;

  if (debug_arc_jtag_target_message)
    printf_filtered ("\n Entering a4_jtag_read_pc ()");
  buffer = 0;
  target_read_aux_reg (&buffer, ARC_HW_STATUS_REGNUM, 1);
  if (debug_arc_jtag_target_message)
    printf_filtered ("\n Value of Status Reg: 0x%x",buffer);
  buffer = buffer & 0x00ffffff;
  buffer = buffer << 2;

  if (debug_arc_jtag_target_message)
    printf_filtered ("\n Leaving a4_jtag_read_pc ()\
                      \n Value of Pc: 0x%x\n", buffer);
  
  return buffer;
}

#endif // ARC4_JTAG

ARCVariantsInfo arc_debug_processor_information;

struct gdbarch *
arc_jtag_init (struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

#ifndef ARC4_JTAG
  tdep->arc_breakpoint_size = arc700_jtag_breakpoint_size;
  tdep->arc_breakpoint_insn = arc700_jtag_breakpoint_insn;
#else
  tdep->arc_breakpoint_size = a4_jtag_breakpoint_size;
  tdep->arc_breakpoint_insn = a4_jtag_breakpoint_insn;
#endif

  set_gdbarch_num_regs (gdbarch, ARC_NR_REGS);
#ifndef ARC4_JTAG
  set_gdbarch_pc_regnum (gdbarch, ARC_PC_REGNUM);
#else
  //  set_gdbarch_pc_regnum (gdbarch, ARC_STATUS_REGNUM);
  set_gdbarch_write_pc (gdbarch, a4_jtag_write_pc);
  set_gdbarch_read_pc (gdbarch, a4_jtag_read_pc);
#endif
  set_gdbarch_register_name (gdbarch, arc_jtag_register_name);
  
  set_gdbarch_print_registers_info (gdbarch, arc_jtag_print_registers_info);

  tdep->register_reggroup_p = arc_jtag_register_reggroup_p;

  tdep->lowest_pc = 0;
  
  tdep->sigtramp_p = NULL;

  tdep->arc_processor_variant_info = &arc_debug_processor_information;

  /* Auxillary register commands.  */
  add_cmd ("arc-aux-read", class_vars, arc_jtag_aux_read_command,
	   "Read and show a range of auxillary registers.\n\
Usage: arc-aux-read <REG-FROM> [<REG-TO>]\n\
REG-FROM and REG-TO can be any expressions that evaluate to integers.\n\
If REG-TO is not specified, one register is displayed.",
	   &cmdlist);

  add_cmd ("arc-aux-write", class_vars, arc_jtag_aux_write_command,
	   "Write to an auxillary register.\n\
Usage: arc-aux-write <REG> = <VALUE>\n\
REG and VALUE can be any expressions that evaluate to integers.",
	   &cmdlist);

  return gdbarch;
}

