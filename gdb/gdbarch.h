/* *INDENT-OFF* */ /* THIS FILE IS GENERATED */

/* Dynamic architecture support for GDB, the GNU debugger.
   Copyright 1998-1999, Free Software Foundation, Inc.

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

/* This file was created with the aid of ``gdbarch.sh''.

   The bourn shell script ``gdbarch.sh'' creates the files
   ``new-gdbarch.c'' and ``new-gdbarch.h and then compares them
   against the existing ``gdbarch.[hc]''.  Any differences found
   being reported.

   If editing this file, please also run gdbarch.sh and merge any
   changes into that script. Conversely, when makeing sweeping changes
   to this file, modifying gdbarch.sh and using its output may prove
   easier. */

#ifndef GDBARCH_H
#define GDBARCH_H

struct frame_info;
struct value;


extern struct gdbarch *current_gdbarch;


/* If any of the following are defined, the target wasn't correctly
   converted. */

#if GDB_MULTI_ARCH
#if defined (EXTRA_FRAME_INFO)
#error "EXTRA_FRAME_INFO: replaced by struct frame_extra_info"
#endif
#endif

#if GDB_MULTI_ARCH
#if defined (FRAME_FIND_SAVED_REGS)
#error "FRAME_FIND_SAVED_REGS: replaced by FRAME_INIT_SAVED_REGS"
#endif
#endif


/* The following are pre-initialized by GDBARCH. */

extern const struct bfd_arch_info * gdbarch_bfd_arch_info (struct gdbarch *gdbarch);
/* set_gdbarch_bfd_arch_info() - not applicable - pre-initialized. */
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_ARCHITECTURE)
#define TARGET_ARCHITECTURE (gdbarch_bfd_arch_info (current_gdbarch))
#endif
#endif

extern int gdbarch_byte_order (struct gdbarch *gdbarch);
/* set_gdbarch_byte_order() - not applicable - pre-initialized. */
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_BYTE_ORDER)
#define TARGET_BYTE_ORDER (gdbarch_byte_order (current_gdbarch))
#endif
#endif


/* The following are initialized by the target dependant code. */

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (TARGET_BFD_VMA_BIT)
#define TARGET_BFD_VMA_BIT (TARGET_ARCHITECTURE->bits_per_address)
#endif

extern int gdbarch_bfd_vma_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_bfd_vma_bit (struct gdbarch *gdbarch, int bfd_vma_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_BFD_VMA_BIT)
#define TARGET_BFD_VMA_BIT (gdbarch_bfd_vma_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_ptr_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_ptr_bit (struct gdbarch *gdbarch, int ptr_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_PTR_BIT)
#define TARGET_PTR_BIT (gdbarch_ptr_bit (current_gdbarch))
#endif
#endif

/*v:1:TARGET_CHAR_BIT:int:char_bit::::8 * sizeof (char):0 */

extern int gdbarch_short_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_short_bit (struct gdbarch *gdbarch, int short_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_SHORT_BIT)
#define TARGET_SHORT_BIT (gdbarch_short_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_int_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_int_bit (struct gdbarch *gdbarch, int int_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_INT_BIT)
#define TARGET_INT_BIT (gdbarch_int_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_long_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_long_bit (struct gdbarch *gdbarch, int long_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_LONG_BIT)
#define TARGET_LONG_BIT (gdbarch_long_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_long_long_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_long_long_bit (struct gdbarch *gdbarch, int long_long_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_LONG_LONG_BIT)
#define TARGET_LONG_LONG_BIT (gdbarch_long_long_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_float_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_float_bit (struct gdbarch *gdbarch, int float_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_FLOAT_BIT)
#define TARGET_FLOAT_BIT (gdbarch_float_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_double_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_double_bit (struct gdbarch *gdbarch, int double_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_DOUBLE_BIT)
#define TARGET_DOUBLE_BIT (gdbarch_double_bit (current_gdbarch))
#endif
#endif

extern int gdbarch_long_double_bit (struct gdbarch *gdbarch);
extern void set_gdbarch_long_double_bit (struct gdbarch *gdbarch, int long_double_bit);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_LONG_DOUBLE_BIT)
#define TARGET_LONG_DOUBLE_BIT (gdbarch_long_double_bit (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (IEEE_FLOAT)
#define IEEE_FLOAT (0)
#endif

extern int gdbarch_ieee_float (struct gdbarch *gdbarch);
extern void set_gdbarch_ieee_float (struct gdbarch *gdbarch, int ieee_float);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (IEEE_FLOAT)
#define IEEE_FLOAT (gdbarch_ieee_float (current_gdbarch))
#endif
#endif

typedef CORE_ADDR (gdbarch_read_pc_ftype) (int pid);
extern CORE_ADDR gdbarch_read_pc (struct gdbarch *gdbarch, int pid);
extern void set_gdbarch_read_pc (struct gdbarch *gdbarch, gdbarch_read_pc_ftype *read_pc);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_READ_PC)
#define TARGET_READ_PC(pid) (gdbarch_read_pc (current_gdbarch, pid))
#endif
#endif

typedef void (gdbarch_write_pc_ftype) (CORE_ADDR val, int pid);
extern void gdbarch_write_pc (struct gdbarch *gdbarch, CORE_ADDR val, int pid);
extern void set_gdbarch_write_pc (struct gdbarch *gdbarch, gdbarch_write_pc_ftype *write_pc);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_WRITE_PC)
#define TARGET_WRITE_PC(val, pid) (gdbarch_write_pc (current_gdbarch, val, pid))
#endif
#endif

typedef CORE_ADDR (gdbarch_read_fp_ftype) (void);
extern CORE_ADDR gdbarch_read_fp (struct gdbarch *gdbarch);
extern void set_gdbarch_read_fp (struct gdbarch *gdbarch, gdbarch_read_fp_ftype *read_fp);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_READ_FP)
#define TARGET_READ_FP() (gdbarch_read_fp (current_gdbarch))
#endif
#endif

typedef void (gdbarch_write_fp_ftype) (CORE_ADDR val);
extern void gdbarch_write_fp (struct gdbarch *gdbarch, CORE_ADDR val);
extern void set_gdbarch_write_fp (struct gdbarch *gdbarch, gdbarch_write_fp_ftype *write_fp);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_WRITE_FP)
#define TARGET_WRITE_FP(val) (gdbarch_write_fp (current_gdbarch, val))
#endif
#endif

typedef CORE_ADDR (gdbarch_read_sp_ftype) (void);
extern CORE_ADDR gdbarch_read_sp (struct gdbarch *gdbarch);
extern void set_gdbarch_read_sp (struct gdbarch *gdbarch, gdbarch_read_sp_ftype *read_sp);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_READ_SP)
#define TARGET_READ_SP() (gdbarch_read_sp (current_gdbarch))
#endif
#endif

typedef void (gdbarch_write_sp_ftype) (CORE_ADDR val);
extern void gdbarch_write_sp (struct gdbarch *gdbarch, CORE_ADDR val);
extern void set_gdbarch_write_sp (struct gdbarch *gdbarch, gdbarch_write_sp_ftype *write_sp);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_WRITE_SP)
#define TARGET_WRITE_SP(val) (gdbarch_write_sp (current_gdbarch, val))
#endif
#endif

extern int gdbarch_num_regs (struct gdbarch *gdbarch);
extern void set_gdbarch_num_regs (struct gdbarch *gdbarch, int num_regs);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (NUM_REGS)
#define NUM_REGS (gdbarch_num_regs (current_gdbarch))
#endif
#endif

/* This macro gives the number of pseudo-registers that live in the
   register namespace but do not get fetched or stored on the target.
   These pseudo-registers may be aliases for other registers,
   combinations of other registers, or they may be computed by GDB. */

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (NUM_PSEUDO_REGS)
#define NUM_PSEUDO_REGS (0)
#endif

extern int gdbarch_num_pseudo_regs (struct gdbarch *gdbarch);
extern void set_gdbarch_num_pseudo_regs (struct gdbarch *gdbarch, int num_pseudo_regs);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (NUM_PSEUDO_REGS)
#define NUM_PSEUDO_REGS (gdbarch_num_pseudo_regs (current_gdbarch))
#endif
#endif

extern int gdbarch_sp_regnum (struct gdbarch *gdbarch);
extern void set_gdbarch_sp_regnum (struct gdbarch *gdbarch, int sp_regnum);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (SP_REGNUM)
#define SP_REGNUM (gdbarch_sp_regnum (current_gdbarch))
#endif
#endif

extern int gdbarch_fp_regnum (struct gdbarch *gdbarch);
extern void set_gdbarch_fp_regnum (struct gdbarch *gdbarch, int fp_regnum);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FP_REGNUM)
#define FP_REGNUM (gdbarch_fp_regnum (current_gdbarch))
#endif
#endif

extern int gdbarch_pc_regnum (struct gdbarch *gdbarch);
extern void set_gdbarch_pc_regnum (struct gdbarch *gdbarch, int pc_regnum);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (PC_REGNUM)
#define PC_REGNUM (gdbarch_pc_regnum (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (FP0_REGNUM)
#define FP0_REGNUM (-1)
#endif

extern int gdbarch_fp0_regnum (struct gdbarch *gdbarch);
extern void set_gdbarch_fp0_regnum (struct gdbarch *gdbarch, int fp0_regnum);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FP0_REGNUM)
#define FP0_REGNUM (gdbarch_fp0_regnum (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (NPC_REGNUM)
#define NPC_REGNUM (-1)
#endif

extern int gdbarch_npc_regnum (struct gdbarch *gdbarch);
extern void set_gdbarch_npc_regnum (struct gdbarch *gdbarch, int npc_regnum);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (NPC_REGNUM)
#define NPC_REGNUM (gdbarch_npc_regnum (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (NNPC_REGNUM)
#define NNPC_REGNUM (-1)
#endif

extern int gdbarch_nnpc_regnum (struct gdbarch *gdbarch);
extern void set_gdbarch_nnpc_regnum (struct gdbarch *gdbarch, int nnpc_regnum);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (NNPC_REGNUM)
#define NNPC_REGNUM (gdbarch_nnpc_regnum (current_gdbarch))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REGISTER_NAME)
#define REGISTER_NAME(regnr) (legacy_register_name (regnr))
#endif

typedef char * (gdbarch_register_name_ftype) (int regnr);
extern char * gdbarch_register_name (struct gdbarch *gdbarch, int regnr);
extern void set_gdbarch_register_name (struct gdbarch *gdbarch, gdbarch_register_name_ftype *register_name);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_NAME)
#define REGISTER_NAME(regnr) (gdbarch_register_name (current_gdbarch, regnr))
#endif
#endif

extern int gdbarch_register_size (struct gdbarch *gdbarch);
extern void set_gdbarch_register_size (struct gdbarch *gdbarch, int register_size);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_SIZE)
#define REGISTER_SIZE (gdbarch_register_size (current_gdbarch))
#endif
#endif

extern int gdbarch_register_bytes (struct gdbarch *gdbarch);
extern void set_gdbarch_register_bytes (struct gdbarch *gdbarch, int register_bytes);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_BYTES)
#define REGISTER_BYTES (gdbarch_register_bytes (current_gdbarch))
#endif
#endif

typedef int (gdbarch_register_byte_ftype) (int reg_nr);
extern int gdbarch_register_byte (struct gdbarch *gdbarch, int reg_nr);
extern void set_gdbarch_register_byte (struct gdbarch *gdbarch, gdbarch_register_byte_ftype *register_byte);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_BYTE)
#define REGISTER_BYTE(reg_nr) (gdbarch_register_byte (current_gdbarch, reg_nr))
#endif
#endif

typedef int (gdbarch_register_raw_size_ftype) (int reg_nr);
extern int gdbarch_register_raw_size (struct gdbarch *gdbarch, int reg_nr);
extern void set_gdbarch_register_raw_size (struct gdbarch *gdbarch, gdbarch_register_raw_size_ftype *register_raw_size);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_RAW_SIZE)
#define REGISTER_RAW_SIZE(reg_nr) (gdbarch_register_raw_size (current_gdbarch, reg_nr))
#endif
#endif

extern int gdbarch_max_register_raw_size (struct gdbarch *gdbarch);
extern void set_gdbarch_max_register_raw_size (struct gdbarch *gdbarch, int max_register_raw_size);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (MAX_REGISTER_RAW_SIZE)
#define MAX_REGISTER_RAW_SIZE (gdbarch_max_register_raw_size (current_gdbarch))
#endif
#endif

typedef int (gdbarch_register_virtual_size_ftype) (int reg_nr);
extern int gdbarch_register_virtual_size (struct gdbarch *gdbarch, int reg_nr);
extern void set_gdbarch_register_virtual_size (struct gdbarch *gdbarch, gdbarch_register_virtual_size_ftype *register_virtual_size);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_VIRTUAL_SIZE)
#define REGISTER_VIRTUAL_SIZE(reg_nr) (gdbarch_register_virtual_size (current_gdbarch, reg_nr))
#endif
#endif

extern int gdbarch_max_register_virtual_size (struct gdbarch *gdbarch);
extern void set_gdbarch_max_register_virtual_size (struct gdbarch *gdbarch, int max_register_virtual_size);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (MAX_REGISTER_VIRTUAL_SIZE)
#define MAX_REGISTER_VIRTUAL_SIZE (gdbarch_max_register_virtual_size (current_gdbarch))
#endif
#endif

typedef struct type * (gdbarch_register_virtual_type_ftype) (int reg_nr);
extern struct type * gdbarch_register_virtual_type (struct gdbarch *gdbarch, int reg_nr);
extern void set_gdbarch_register_virtual_type (struct gdbarch *gdbarch, gdbarch_register_virtual_type_ftype *register_virtual_type);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_VIRTUAL_TYPE)
#define REGISTER_VIRTUAL_TYPE(reg_nr) (gdbarch_register_virtual_type (current_gdbarch, reg_nr))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (DO_REGISTERS_INFO)
#define DO_REGISTERS_INFO(reg_nr, fpregs) (do_registers_info (reg_nr, fpregs))
#endif

typedef void (gdbarch_do_registers_info_ftype) (int reg_nr, int fpregs);
extern void gdbarch_do_registers_info (struct gdbarch *gdbarch, int reg_nr, int fpregs);
extern void set_gdbarch_do_registers_info (struct gdbarch *gdbarch, gdbarch_do_registers_info_ftype *do_registers_info);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (DO_REGISTERS_INFO)
#define DO_REGISTERS_INFO(reg_nr, fpregs) (gdbarch_do_registers_info (current_gdbarch, reg_nr, fpregs))
#endif
#endif

/* MAP a GDB RAW register number onto a simulator register number.  See
   also include/...-sim.h. */

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REGISTER_SIM_REGNO)
#define REGISTER_SIM_REGNO(reg_nr) (default_register_sim_regno (reg_nr))
#endif

typedef int (gdbarch_register_sim_regno_ftype) (int reg_nr);
extern int gdbarch_register_sim_regno (struct gdbarch *gdbarch, int reg_nr);
extern void set_gdbarch_register_sim_regno (struct gdbarch *gdbarch, gdbarch_register_sim_regno_ftype *register_sim_regno);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_SIM_REGNO)
#define REGISTER_SIM_REGNO(reg_nr) (gdbarch_register_sim_regno (current_gdbarch, reg_nr))
#endif
#endif

extern int gdbarch_use_generic_dummy_frames (struct gdbarch *gdbarch);
extern void set_gdbarch_use_generic_dummy_frames (struct gdbarch *gdbarch, int use_generic_dummy_frames);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (USE_GENERIC_DUMMY_FRAMES)
#define USE_GENERIC_DUMMY_FRAMES (gdbarch_use_generic_dummy_frames (current_gdbarch))
#endif
#endif

extern int gdbarch_call_dummy_location (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_location (struct gdbarch *gdbarch, int call_dummy_location);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_LOCATION)
#define CALL_DUMMY_LOCATION (gdbarch_call_dummy_location (current_gdbarch))
#endif
#endif

typedef CORE_ADDR (gdbarch_call_dummy_address_ftype) (void);
extern CORE_ADDR gdbarch_call_dummy_address (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_address (struct gdbarch *gdbarch, gdbarch_call_dummy_address_ftype *call_dummy_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_ADDRESS)
#define CALL_DUMMY_ADDRESS() (gdbarch_call_dummy_address (current_gdbarch))
#endif
#endif

extern CORE_ADDR gdbarch_call_dummy_start_offset (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_start_offset (struct gdbarch *gdbarch, CORE_ADDR call_dummy_start_offset);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_START_OFFSET)
#define CALL_DUMMY_START_OFFSET (gdbarch_call_dummy_start_offset (current_gdbarch))
#endif
#endif

extern CORE_ADDR gdbarch_call_dummy_breakpoint_offset (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_breakpoint_offset (struct gdbarch *gdbarch, CORE_ADDR call_dummy_breakpoint_offset);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_BREAKPOINT_OFFSET)
#define CALL_DUMMY_BREAKPOINT_OFFSET (gdbarch_call_dummy_breakpoint_offset (current_gdbarch))
#endif
#endif

extern int gdbarch_call_dummy_breakpoint_offset_p (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_breakpoint_offset_p (struct gdbarch *gdbarch, int call_dummy_breakpoint_offset_p);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_BREAKPOINT_OFFSET_P)
#define CALL_DUMMY_BREAKPOINT_OFFSET_P (gdbarch_call_dummy_breakpoint_offset_p (current_gdbarch))
#endif
#endif

extern int gdbarch_call_dummy_length (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_length (struct gdbarch *gdbarch, int call_dummy_length);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_LENGTH)
#define CALL_DUMMY_LENGTH (gdbarch_call_dummy_length (current_gdbarch))
#endif
#endif

typedef int (gdbarch_pc_in_call_dummy_ftype) (CORE_ADDR pc, CORE_ADDR sp, CORE_ADDR frame_address);
extern int gdbarch_pc_in_call_dummy (struct gdbarch *gdbarch, CORE_ADDR pc, CORE_ADDR sp, CORE_ADDR frame_address);
extern void set_gdbarch_pc_in_call_dummy (struct gdbarch *gdbarch, gdbarch_pc_in_call_dummy_ftype *pc_in_call_dummy);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (PC_IN_CALL_DUMMY)
#define PC_IN_CALL_DUMMY(pc, sp, frame_address) (gdbarch_pc_in_call_dummy (current_gdbarch, pc, sp, frame_address))
#endif
#endif

extern int gdbarch_call_dummy_p (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_p (struct gdbarch *gdbarch, int call_dummy_p);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_P)
#define CALL_DUMMY_P (gdbarch_call_dummy_p (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (CALL_DUMMY_WORDS)
#define CALL_DUMMY_WORDS (legacy_call_dummy_words)
#endif

extern LONGEST * gdbarch_call_dummy_words (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_words (struct gdbarch *gdbarch, LONGEST * call_dummy_words);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_WORDS)
#define CALL_DUMMY_WORDS (gdbarch_call_dummy_words (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (SIZEOF_CALL_DUMMY_WORDS)
#define SIZEOF_CALL_DUMMY_WORDS (legacy_sizeof_call_dummy_words)
#endif

extern int gdbarch_sizeof_call_dummy_words (struct gdbarch *gdbarch);
extern void set_gdbarch_sizeof_call_dummy_words (struct gdbarch *gdbarch, int sizeof_call_dummy_words);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (SIZEOF_CALL_DUMMY_WORDS)
#define SIZEOF_CALL_DUMMY_WORDS (gdbarch_sizeof_call_dummy_words (current_gdbarch))
#endif
#endif

extern int gdbarch_call_dummy_stack_adjust_p (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_stack_adjust_p (struct gdbarch *gdbarch, int call_dummy_stack_adjust_p);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_STACK_ADJUST_P)
#define CALL_DUMMY_STACK_ADJUST_P (gdbarch_call_dummy_stack_adjust_p (current_gdbarch))
#endif
#endif

extern int gdbarch_call_dummy_stack_adjust (struct gdbarch *gdbarch);
extern void set_gdbarch_call_dummy_stack_adjust (struct gdbarch *gdbarch, int call_dummy_stack_adjust);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (CALL_DUMMY_STACK_ADJUST)
#define CALL_DUMMY_STACK_ADJUST (gdbarch_call_dummy_stack_adjust (current_gdbarch))
#endif
#endif

typedef void (gdbarch_fix_call_dummy_ftype) (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs, struct value **args, struct type *type, int gcc_p);
extern void gdbarch_fix_call_dummy (struct gdbarch *gdbarch, char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs, struct value **args, struct type *type, int gcc_p);
extern void set_gdbarch_fix_call_dummy (struct gdbarch *gdbarch, gdbarch_fix_call_dummy_ftype *fix_call_dummy);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FIX_CALL_DUMMY)
#define FIX_CALL_DUMMY(dummy, pc, fun, nargs, args, type, gcc_p) (gdbarch_fix_call_dummy (current_gdbarch, dummy, pc, fun, nargs, args, type, gcc_p))
#endif
#endif

extern int gdbarch_believe_pcc_promotion (struct gdbarch *gdbarch);
extern void set_gdbarch_believe_pcc_promotion (struct gdbarch *gdbarch, int believe_pcc_promotion);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (BELIEVE_PCC_PROMOTION)
#define BELIEVE_PCC_PROMOTION (gdbarch_believe_pcc_promotion (current_gdbarch))
#endif
#endif

extern int gdbarch_believe_pcc_promotion_type (struct gdbarch *gdbarch);
extern void set_gdbarch_believe_pcc_promotion_type (struct gdbarch *gdbarch, int believe_pcc_promotion_type);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (BELIEVE_PCC_PROMOTION_TYPE)
#define BELIEVE_PCC_PROMOTION_TYPE (gdbarch_believe_pcc_promotion_type (current_gdbarch))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (COERCE_FLOAT_TO_DOUBLE)
#define COERCE_FLOAT_TO_DOUBLE(formal, actual) (default_coerce_float_to_double (formal, actual))
#endif

typedef int (gdbarch_coerce_float_to_double_ftype) (struct type *formal, struct type *actual);
extern int gdbarch_coerce_float_to_double (struct gdbarch *gdbarch, struct type *formal, struct type *actual);
extern void set_gdbarch_coerce_float_to_double (struct gdbarch *gdbarch, gdbarch_coerce_float_to_double_ftype *coerce_float_to_double);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (COERCE_FLOAT_TO_DOUBLE)
#define COERCE_FLOAT_TO_DOUBLE(formal, actual) (gdbarch_coerce_float_to_double (current_gdbarch, formal, actual))
#endif
#endif

typedef void (gdbarch_get_saved_register_ftype) (char *raw_buffer, int *optimized, CORE_ADDR *addrp, struct frame_info *frame, int regnum, enum lval_type *lval);
extern void gdbarch_get_saved_register (struct gdbarch *gdbarch, char *raw_buffer, int *optimized, CORE_ADDR *addrp, struct frame_info *frame, int regnum, enum lval_type *lval);
extern void set_gdbarch_get_saved_register (struct gdbarch *gdbarch, gdbarch_get_saved_register_ftype *get_saved_register);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (GET_SAVED_REGISTER)
#define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) (gdbarch_get_saved_register (current_gdbarch, raw_buffer, optimized, addrp, frame, regnum, lval))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REGISTER_CONVERTIBLE)
#define REGISTER_CONVERTIBLE(nr) (generic_register_convertible_not (nr))
#endif

typedef int (gdbarch_register_convertible_ftype) (int nr);
extern int gdbarch_register_convertible (struct gdbarch *gdbarch, int nr);
extern void set_gdbarch_register_convertible (struct gdbarch *gdbarch, gdbarch_register_convertible_ftype *register_convertible);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_CONVERTIBLE)
#define REGISTER_CONVERTIBLE(nr) (gdbarch_register_convertible (current_gdbarch, nr))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REGISTER_CONVERT_TO_VIRTUAL)
#define REGISTER_CONVERT_TO_VIRTUAL(regnum, type, from, to) (internal_error ("REGISTER_CONVERT_TO_VIRTUAL"), 0)
#endif

typedef void (gdbarch_register_convert_to_virtual_ftype) (int regnum, struct type *type, char *from, char *to);
extern void gdbarch_register_convert_to_virtual (struct gdbarch *gdbarch, int regnum, struct type *type, char *from, char *to);
extern void set_gdbarch_register_convert_to_virtual (struct gdbarch *gdbarch, gdbarch_register_convert_to_virtual_ftype *register_convert_to_virtual);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_CONVERT_TO_VIRTUAL)
#define REGISTER_CONVERT_TO_VIRTUAL(regnum, type, from, to) (gdbarch_register_convert_to_virtual (current_gdbarch, regnum, type, from, to))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REGISTER_CONVERT_TO_RAW)
#define REGISTER_CONVERT_TO_RAW(type, regnum, from, to) (internal_error ("REGISTER_CONVERT_TO_RAW"), 0)
#endif

typedef void (gdbarch_register_convert_to_raw_ftype) (struct type *type, int regnum, char *from, char *to);
extern void gdbarch_register_convert_to_raw (struct gdbarch *gdbarch, struct type *type, int regnum, char *from, char *to);
extern void set_gdbarch_register_convert_to_raw (struct gdbarch *gdbarch, gdbarch_register_convert_to_raw_ftype *register_convert_to_raw);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REGISTER_CONVERT_TO_RAW)
#define REGISTER_CONVERT_TO_RAW(type, regnum, from, to) (gdbarch_register_convert_to_raw (current_gdbarch, type, regnum, from, to))
#endif
#endif

/* This function is called when the value of a pseudo-register needs to
   be updated.  Typically it will be defined on a per-architecture
   basis. */

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (FETCH_PSEUDO_REGISTER)
#define FETCH_PSEUDO_REGISTER(regnum) (internal_error ("FETCH_PSEUDO_REGISTER"), 0)
#endif

typedef void (gdbarch_fetch_pseudo_register_ftype) (int regnum);
extern void gdbarch_fetch_pseudo_register (struct gdbarch *gdbarch, int regnum);
extern void set_gdbarch_fetch_pseudo_register (struct gdbarch *gdbarch, gdbarch_fetch_pseudo_register_ftype *fetch_pseudo_register);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FETCH_PSEUDO_REGISTER)
#define FETCH_PSEUDO_REGISTER(regnum) (gdbarch_fetch_pseudo_register (current_gdbarch, regnum))
#endif
#endif

/* This function is called when the value of a pseudo-register needs to
   be set or stored.  Typically it will be defined on a
   per-architecture basis. */

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (STORE_PSEUDO_REGISTER)
#define STORE_PSEUDO_REGISTER(regnum) (internal_error ("STORE_PSEUDO_REGISTER"), 0)
#endif

typedef void (gdbarch_store_pseudo_register_ftype) (int regnum);
extern void gdbarch_store_pseudo_register (struct gdbarch *gdbarch, int regnum);
extern void set_gdbarch_store_pseudo_register (struct gdbarch *gdbarch, gdbarch_store_pseudo_register_ftype *store_pseudo_register);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (STORE_PSEUDO_REGISTER)
#define STORE_PSEUDO_REGISTER(regnum) (gdbarch_store_pseudo_register (current_gdbarch, regnum))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (POINTER_TO_ADDRESS)
#define POINTER_TO_ADDRESS(type, buf) (unsigned_pointer_to_address (type, buf))
#endif

typedef CORE_ADDR (gdbarch_pointer_to_address_ftype) (struct type *type, void *buf);
extern CORE_ADDR gdbarch_pointer_to_address (struct gdbarch *gdbarch, struct type *type, void *buf);
extern void set_gdbarch_pointer_to_address (struct gdbarch *gdbarch, gdbarch_pointer_to_address_ftype *pointer_to_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (POINTER_TO_ADDRESS)
#define POINTER_TO_ADDRESS(type, buf) (gdbarch_pointer_to_address (current_gdbarch, type, buf))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (ADDRESS_TO_POINTER)
#define ADDRESS_TO_POINTER(type, buf, addr) (unsigned_address_to_pointer (type, buf, addr))
#endif

typedef void (gdbarch_address_to_pointer_ftype) (struct type *type, void *buf, CORE_ADDR addr);
extern void gdbarch_address_to_pointer (struct gdbarch *gdbarch, struct type *type, void *buf, CORE_ADDR addr);
extern void set_gdbarch_address_to_pointer (struct gdbarch *gdbarch, gdbarch_address_to_pointer_ftype *address_to_pointer);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (ADDRESS_TO_POINTER)
#define ADDRESS_TO_POINTER(type, buf, addr) (gdbarch_address_to_pointer (current_gdbarch, type, buf, addr))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (RETURN_VALUE_ON_STACK)
#define RETURN_VALUE_ON_STACK(type) (generic_return_value_on_stack_not (type))
#endif

typedef int (gdbarch_return_value_on_stack_ftype) (struct type *type);
extern int gdbarch_return_value_on_stack (struct gdbarch *gdbarch, struct type *type);
extern void set_gdbarch_return_value_on_stack (struct gdbarch *gdbarch, gdbarch_return_value_on_stack_ftype *return_value_on_stack);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (RETURN_VALUE_ON_STACK)
#define RETURN_VALUE_ON_STACK(type) (gdbarch_return_value_on_stack (current_gdbarch, type))
#endif
#endif

typedef void (gdbarch_extract_return_value_ftype) (struct type *type, char *regbuf, char *valbuf);
extern void gdbarch_extract_return_value (struct gdbarch *gdbarch, struct type *type, char *regbuf, char *valbuf);
extern void set_gdbarch_extract_return_value (struct gdbarch *gdbarch, gdbarch_extract_return_value_ftype *extract_return_value);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (EXTRACT_RETURN_VALUE)
#define EXTRACT_RETURN_VALUE(type, regbuf, valbuf) (gdbarch_extract_return_value (current_gdbarch, type, regbuf, valbuf))
#endif
#endif

typedef CORE_ADDR (gdbarch_push_arguments_ftype) (int nargs, struct value **args, CORE_ADDR sp, int struct_return, CORE_ADDR struct_addr);
extern CORE_ADDR gdbarch_push_arguments (struct gdbarch *gdbarch, int nargs, struct value **args, CORE_ADDR sp, int struct_return, CORE_ADDR struct_addr);
extern void set_gdbarch_push_arguments (struct gdbarch *gdbarch, gdbarch_push_arguments_ftype *push_arguments);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (PUSH_ARGUMENTS)
#define PUSH_ARGUMENTS(nargs, args, sp, struct_return, struct_addr) (gdbarch_push_arguments (current_gdbarch, nargs, args, sp, struct_return, struct_addr))
#endif
#endif

typedef void (gdbarch_push_dummy_frame_ftype) (void);
extern void gdbarch_push_dummy_frame (struct gdbarch *gdbarch);
extern void set_gdbarch_push_dummy_frame (struct gdbarch *gdbarch, gdbarch_push_dummy_frame_ftype *push_dummy_frame);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (PUSH_DUMMY_FRAME)
#define PUSH_DUMMY_FRAME (gdbarch_push_dummy_frame (current_gdbarch))
#endif
#endif

typedef CORE_ADDR (gdbarch_push_return_address_ftype) (CORE_ADDR pc, CORE_ADDR sp);
extern CORE_ADDR gdbarch_push_return_address (struct gdbarch *gdbarch, CORE_ADDR pc, CORE_ADDR sp);
extern void set_gdbarch_push_return_address (struct gdbarch *gdbarch, gdbarch_push_return_address_ftype *push_return_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (PUSH_RETURN_ADDRESS)
#define PUSH_RETURN_ADDRESS(pc, sp) (gdbarch_push_return_address (current_gdbarch, pc, sp))
#endif
#endif

typedef void (gdbarch_pop_frame_ftype) (void);
extern void gdbarch_pop_frame (struct gdbarch *gdbarch);
extern void set_gdbarch_pop_frame (struct gdbarch *gdbarch, gdbarch_pop_frame_ftype *pop_frame);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (POP_FRAME)
#define POP_FRAME (gdbarch_pop_frame (current_gdbarch))
#endif
#endif

/* I wish that these would just go away.... */

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (D10V_MAKE_DADDR)
#define D10V_MAKE_DADDR(x) (internal_error ("D10V_MAKE_DADDR"), 0)
#endif

typedef CORE_ADDR (gdbarch_d10v_make_daddr_ftype) (CORE_ADDR x);
extern CORE_ADDR gdbarch_d10v_make_daddr (struct gdbarch *gdbarch, CORE_ADDR x);
extern void set_gdbarch_d10v_make_daddr (struct gdbarch *gdbarch, gdbarch_d10v_make_daddr_ftype *d10v_make_daddr);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (D10V_MAKE_DADDR)
#define D10V_MAKE_DADDR(x) (gdbarch_d10v_make_daddr (current_gdbarch, x))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (D10V_MAKE_IADDR)
#define D10V_MAKE_IADDR(x) (internal_error ("D10V_MAKE_IADDR"), 0)
#endif

typedef CORE_ADDR (gdbarch_d10v_make_iaddr_ftype) (CORE_ADDR x);
extern CORE_ADDR gdbarch_d10v_make_iaddr (struct gdbarch *gdbarch, CORE_ADDR x);
extern void set_gdbarch_d10v_make_iaddr (struct gdbarch *gdbarch, gdbarch_d10v_make_iaddr_ftype *d10v_make_iaddr);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (D10V_MAKE_IADDR)
#define D10V_MAKE_IADDR(x) (gdbarch_d10v_make_iaddr (current_gdbarch, x))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (D10V_DADDR_P)
#define D10V_DADDR_P(x) (internal_error ("D10V_DADDR_P"), 0)
#endif

typedef int (gdbarch_d10v_daddr_p_ftype) (CORE_ADDR x);
extern int gdbarch_d10v_daddr_p (struct gdbarch *gdbarch, CORE_ADDR x);
extern void set_gdbarch_d10v_daddr_p (struct gdbarch *gdbarch, gdbarch_d10v_daddr_p_ftype *d10v_daddr_p);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (D10V_DADDR_P)
#define D10V_DADDR_P(x) (gdbarch_d10v_daddr_p (current_gdbarch, x))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (D10V_IADDR_P)
#define D10V_IADDR_P(x) (internal_error ("D10V_IADDR_P"), 0)
#endif

typedef int (gdbarch_d10v_iaddr_p_ftype) (CORE_ADDR x);
extern int gdbarch_d10v_iaddr_p (struct gdbarch *gdbarch, CORE_ADDR x);
extern void set_gdbarch_d10v_iaddr_p (struct gdbarch *gdbarch, gdbarch_d10v_iaddr_p_ftype *d10v_iaddr_p);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (D10V_IADDR_P)
#define D10V_IADDR_P(x) (gdbarch_d10v_iaddr_p (current_gdbarch, x))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (D10V_CONVERT_DADDR_TO_RAW)
#define D10V_CONVERT_DADDR_TO_RAW(x) (internal_error ("D10V_CONVERT_DADDR_TO_RAW"), 0)
#endif

typedef CORE_ADDR (gdbarch_d10v_convert_daddr_to_raw_ftype) (CORE_ADDR x);
extern CORE_ADDR gdbarch_d10v_convert_daddr_to_raw (struct gdbarch *gdbarch, CORE_ADDR x);
extern void set_gdbarch_d10v_convert_daddr_to_raw (struct gdbarch *gdbarch, gdbarch_d10v_convert_daddr_to_raw_ftype *d10v_convert_daddr_to_raw);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (D10V_CONVERT_DADDR_TO_RAW)
#define D10V_CONVERT_DADDR_TO_RAW(x) (gdbarch_d10v_convert_daddr_to_raw (current_gdbarch, x))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (D10V_CONVERT_IADDR_TO_RAW)
#define D10V_CONVERT_IADDR_TO_RAW(x) (internal_error ("D10V_CONVERT_IADDR_TO_RAW"), 0)
#endif

typedef CORE_ADDR (gdbarch_d10v_convert_iaddr_to_raw_ftype) (CORE_ADDR x);
extern CORE_ADDR gdbarch_d10v_convert_iaddr_to_raw (struct gdbarch *gdbarch, CORE_ADDR x);
extern void set_gdbarch_d10v_convert_iaddr_to_raw (struct gdbarch *gdbarch, gdbarch_d10v_convert_iaddr_to_raw_ftype *d10v_convert_iaddr_to_raw);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (D10V_CONVERT_IADDR_TO_RAW)
#define D10V_CONVERT_IADDR_TO_RAW(x) (gdbarch_d10v_convert_iaddr_to_raw (current_gdbarch, x))
#endif
#endif

typedef void (gdbarch_store_struct_return_ftype) (CORE_ADDR addr, CORE_ADDR sp);
extern void gdbarch_store_struct_return (struct gdbarch *gdbarch, CORE_ADDR addr, CORE_ADDR sp);
extern void set_gdbarch_store_struct_return (struct gdbarch *gdbarch, gdbarch_store_struct_return_ftype *store_struct_return);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (STORE_STRUCT_RETURN)
#define STORE_STRUCT_RETURN(addr, sp) (gdbarch_store_struct_return (current_gdbarch, addr, sp))
#endif
#endif

typedef void (gdbarch_store_return_value_ftype) (struct type *type, char *valbuf);
extern void gdbarch_store_return_value (struct gdbarch *gdbarch, struct type *type, char *valbuf);
extern void set_gdbarch_store_return_value (struct gdbarch *gdbarch, gdbarch_store_return_value_ftype *store_return_value);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (STORE_RETURN_VALUE)
#define STORE_RETURN_VALUE(type, valbuf) (gdbarch_store_return_value (current_gdbarch, type, valbuf))
#endif
#endif

typedef CORE_ADDR (gdbarch_extract_struct_value_address_ftype) (char *regbuf);
extern CORE_ADDR gdbarch_extract_struct_value_address (struct gdbarch *gdbarch, char *regbuf);
extern void set_gdbarch_extract_struct_value_address (struct gdbarch *gdbarch, gdbarch_extract_struct_value_address_ftype *extract_struct_value_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (EXTRACT_STRUCT_VALUE_ADDRESS)
#define EXTRACT_STRUCT_VALUE_ADDRESS(regbuf) (gdbarch_extract_struct_value_address (current_gdbarch, regbuf))
#endif
#endif

typedef int (gdbarch_use_struct_convention_ftype) (int gcc_p, struct type *value_type);
extern int gdbarch_use_struct_convention (struct gdbarch *gdbarch, int gcc_p, struct type *value_type);
extern void set_gdbarch_use_struct_convention (struct gdbarch *gdbarch, gdbarch_use_struct_convention_ftype *use_struct_convention);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (USE_STRUCT_CONVENTION)
#define USE_STRUCT_CONVENTION(gcc_p, value_type) (gdbarch_use_struct_convention (current_gdbarch, gcc_p, value_type))
#endif
#endif

typedef void (gdbarch_frame_init_saved_regs_ftype) (struct frame_info *frame);
extern void gdbarch_frame_init_saved_regs (struct gdbarch *gdbarch, struct frame_info *frame);
extern void set_gdbarch_frame_init_saved_regs (struct gdbarch *gdbarch, gdbarch_frame_init_saved_regs_ftype *frame_init_saved_regs);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_INIT_SAVED_REGS)
#define FRAME_INIT_SAVED_REGS(frame) (gdbarch_frame_init_saved_regs (current_gdbarch, frame))
#endif
#endif

typedef void (gdbarch_init_extra_frame_info_ftype) (int fromleaf, struct frame_info *frame);
extern void gdbarch_init_extra_frame_info (struct gdbarch *gdbarch, int fromleaf, struct frame_info *frame);
extern void set_gdbarch_init_extra_frame_info (struct gdbarch *gdbarch, gdbarch_init_extra_frame_info_ftype *init_extra_frame_info);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (INIT_EXTRA_FRAME_INFO)
#define INIT_EXTRA_FRAME_INFO(fromleaf, frame) (gdbarch_init_extra_frame_info (current_gdbarch, fromleaf, frame))
#endif
#endif

typedef CORE_ADDR (gdbarch_skip_prologue_ftype) (CORE_ADDR ip);
extern CORE_ADDR gdbarch_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR ip);
extern void set_gdbarch_skip_prologue (struct gdbarch *gdbarch, gdbarch_skip_prologue_ftype *skip_prologue);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (SKIP_PROLOGUE)
#define SKIP_PROLOGUE(ip) (gdbarch_skip_prologue (current_gdbarch, ip))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (PROLOGUE_FRAMELESS_P)
#define PROLOGUE_FRAMELESS_P(ip) (generic_prologue_frameless_p (ip))
#endif

typedef int (gdbarch_prologue_frameless_p_ftype) (CORE_ADDR ip);
extern int gdbarch_prologue_frameless_p (struct gdbarch *gdbarch, CORE_ADDR ip);
extern void set_gdbarch_prologue_frameless_p (struct gdbarch *gdbarch, gdbarch_prologue_frameless_p_ftype *prologue_frameless_p);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (PROLOGUE_FRAMELESS_P)
#define PROLOGUE_FRAMELESS_P(ip) (gdbarch_prologue_frameless_p (current_gdbarch, ip))
#endif
#endif

typedef int (gdbarch_inner_than_ftype) (CORE_ADDR lhs, CORE_ADDR rhs);
extern int gdbarch_inner_than (struct gdbarch *gdbarch, CORE_ADDR lhs, CORE_ADDR rhs);
extern void set_gdbarch_inner_than (struct gdbarch *gdbarch, gdbarch_inner_than_ftype *inner_than);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (INNER_THAN)
#define INNER_THAN(lhs, rhs) (gdbarch_inner_than (current_gdbarch, lhs, rhs))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (BREAKPOINT_FROM_PC)
#define BREAKPOINT_FROM_PC(pcptr, lenptr) (legacy_breakpoint_from_pc (pcptr, lenptr))
#endif

typedef unsigned char * (gdbarch_breakpoint_from_pc_ftype) (CORE_ADDR *pcptr, int *lenptr);
extern unsigned char * gdbarch_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pcptr, int *lenptr);
extern void set_gdbarch_breakpoint_from_pc (struct gdbarch *gdbarch, gdbarch_breakpoint_from_pc_ftype *breakpoint_from_pc);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (BREAKPOINT_FROM_PC)
#define BREAKPOINT_FROM_PC(pcptr, lenptr) (gdbarch_breakpoint_from_pc (current_gdbarch, pcptr, lenptr))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (MEMORY_INSERT_BREAKPOINT)
#define MEMORY_INSERT_BREAKPOINT(addr, contents_cache) (default_memory_insert_breakpoint (addr, contents_cache))
#endif

typedef int (gdbarch_memory_insert_breakpoint_ftype) (CORE_ADDR addr, char *contents_cache);
extern int gdbarch_memory_insert_breakpoint (struct gdbarch *gdbarch, CORE_ADDR addr, char *contents_cache);
extern void set_gdbarch_memory_insert_breakpoint (struct gdbarch *gdbarch, gdbarch_memory_insert_breakpoint_ftype *memory_insert_breakpoint);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (MEMORY_INSERT_BREAKPOINT)
#define MEMORY_INSERT_BREAKPOINT(addr, contents_cache) (gdbarch_memory_insert_breakpoint (current_gdbarch, addr, contents_cache))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (MEMORY_REMOVE_BREAKPOINT)
#define MEMORY_REMOVE_BREAKPOINT(addr, contents_cache) (default_memory_remove_breakpoint (addr, contents_cache))
#endif

typedef int (gdbarch_memory_remove_breakpoint_ftype) (CORE_ADDR addr, char *contents_cache);
extern int gdbarch_memory_remove_breakpoint (struct gdbarch *gdbarch, CORE_ADDR addr, char *contents_cache);
extern void set_gdbarch_memory_remove_breakpoint (struct gdbarch *gdbarch, gdbarch_memory_remove_breakpoint_ftype *memory_remove_breakpoint);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (MEMORY_REMOVE_BREAKPOINT)
#define MEMORY_REMOVE_BREAKPOINT(addr, contents_cache) (gdbarch_memory_remove_breakpoint (current_gdbarch, addr, contents_cache))
#endif
#endif

extern CORE_ADDR gdbarch_decr_pc_after_break (struct gdbarch *gdbarch);
extern void set_gdbarch_decr_pc_after_break (struct gdbarch *gdbarch, CORE_ADDR decr_pc_after_break);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (DECR_PC_AFTER_BREAK)
#define DECR_PC_AFTER_BREAK (gdbarch_decr_pc_after_break (current_gdbarch))
#endif
#endif

extern CORE_ADDR gdbarch_function_start_offset (struct gdbarch *gdbarch);
extern void set_gdbarch_function_start_offset (struct gdbarch *gdbarch, CORE_ADDR function_start_offset);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FUNCTION_START_OFFSET)
#define FUNCTION_START_OFFSET (gdbarch_function_start_offset (current_gdbarch))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REMOTE_TRANSLATE_XFER_ADDRESS)
#define REMOTE_TRANSLATE_XFER_ADDRESS(gdb_addr, gdb_len, rem_addr, rem_len) (generic_remote_translate_xfer_address (gdb_addr, gdb_len, rem_addr, rem_len))
#endif

typedef void (gdbarch_remote_translate_xfer_address_ftype) (CORE_ADDR gdb_addr, int gdb_len, CORE_ADDR *rem_addr, int *rem_len);
extern void gdbarch_remote_translate_xfer_address (struct gdbarch *gdbarch, CORE_ADDR gdb_addr, int gdb_len, CORE_ADDR *rem_addr, int *rem_len);
extern void set_gdbarch_remote_translate_xfer_address (struct gdbarch *gdbarch, gdbarch_remote_translate_xfer_address_ftype *remote_translate_xfer_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REMOTE_TRANSLATE_XFER_ADDRESS)
#define REMOTE_TRANSLATE_XFER_ADDRESS(gdb_addr, gdb_len, rem_addr, rem_len) (gdbarch_remote_translate_xfer_address (current_gdbarch, gdb_addr, gdb_len, rem_addr, rem_len))
#endif
#endif

extern CORE_ADDR gdbarch_frame_args_skip (struct gdbarch *gdbarch);
extern void set_gdbarch_frame_args_skip (struct gdbarch *gdbarch, CORE_ADDR frame_args_skip);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_ARGS_SKIP)
#define FRAME_ARGS_SKIP (gdbarch_frame_args_skip (current_gdbarch))
#endif
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (FRAMELESS_FUNCTION_INVOCATION)
#define FRAMELESS_FUNCTION_INVOCATION(fi) (generic_frameless_function_invocation_not (fi))
#endif

typedef int (gdbarch_frameless_function_invocation_ftype) (struct frame_info *fi);
extern int gdbarch_frameless_function_invocation (struct gdbarch *gdbarch, struct frame_info *fi);
extern void set_gdbarch_frameless_function_invocation (struct gdbarch *gdbarch, gdbarch_frameless_function_invocation_ftype *frameless_function_invocation);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAMELESS_FUNCTION_INVOCATION)
#define FRAMELESS_FUNCTION_INVOCATION(fi) (gdbarch_frameless_function_invocation (current_gdbarch, fi))
#endif
#endif

typedef CORE_ADDR (gdbarch_frame_chain_ftype) (struct frame_info *frame);
extern CORE_ADDR gdbarch_frame_chain (struct gdbarch *gdbarch, struct frame_info *frame);
extern void set_gdbarch_frame_chain (struct gdbarch *gdbarch, gdbarch_frame_chain_ftype *frame_chain);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_CHAIN)
#define FRAME_CHAIN(frame) (gdbarch_frame_chain (current_gdbarch, frame))
#endif
#endif

typedef int (gdbarch_frame_chain_valid_ftype) (CORE_ADDR chain, struct frame_info *thisframe);
extern int gdbarch_frame_chain_valid (struct gdbarch *gdbarch, CORE_ADDR chain, struct frame_info *thisframe);
extern void set_gdbarch_frame_chain_valid (struct gdbarch *gdbarch, gdbarch_frame_chain_valid_ftype *frame_chain_valid);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_CHAIN_VALID)
#define FRAME_CHAIN_VALID(chain, thisframe) (gdbarch_frame_chain_valid (current_gdbarch, chain, thisframe))
#endif
#endif

typedef CORE_ADDR (gdbarch_frame_saved_pc_ftype) (struct frame_info *fi);
extern CORE_ADDR gdbarch_frame_saved_pc (struct gdbarch *gdbarch, struct frame_info *fi);
extern void set_gdbarch_frame_saved_pc (struct gdbarch *gdbarch, gdbarch_frame_saved_pc_ftype *frame_saved_pc);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_SAVED_PC)
#define FRAME_SAVED_PC(fi) (gdbarch_frame_saved_pc (current_gdbarch, fi))
#endif
#endif

typedef CORE_ADDR (gdbarch_frame_args_address_ftype) (struct frame_info *fi);
extern CORE_ADDR gdbarch_frame_args_address (struct gdbarch *gdbarch, struct frame_info *fi);
extern void set_gdbarch_frame_args_address (struct gdbarch *gdbarch, gdbarch_frame_args_address_ftype *frame_args_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_ARGS_ADDRESS)
#define FRAME_ARGS_ADDRESS(fi) (gdbarch_frame_args_address (current_gdbarch, fi))
#endif
#endif

typedef CORE_ADDR (gdbarch_frame_locals_address_ftype) (struct frame_info *fi);
extern CORE_ADDR gdbarch_frame_locals_address (struct gdbarch *gdbarch, struct frame_info *fi);
extern void set_gdbarch_frame_locals_address (struct gdbarch *gdbarch, gdbarch_frame_locals_address_ftype *frame_locals_address);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_LOCALS_ADDRESS)
#define FRAME_LOCALS_ADDRESS(fi) (gdbarch_frame_locals_address (current_gdbarch, fi))
#endif
#endif

typedef CORE_ADDR (gdbarch_saved_pc_after_call_ftype) (struct frame_info *frame);
extern CORE_ADDR gdbarch_saved_pc_after_call (struct gdbarch *gdbarch, struct frame_info *frame);
extern void set_gdbarch_saved_pc_after_call (struct gdbarch *gdbarch, gdbarch_saved_pc_after_call_ftype *saved_pc_after_call);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (SAVED_PC_AFTER_CALL)
#define SAVED_PC_AFTER_CALL(frame) (gdbarch_saved_pc_after_call (current_gdbarch, frame))
#endif
#endif

typedef int (gdbarch_frame_num_args_ftype) (struct frame_info *frame);
extern int gdbarch_frame_num_args (struct gdbarch *gdbarch, struct frame_info *frame);
extern void set_gdbarch_frame_num_args (struct gdbarch *gdbarch, gdbarch_frame_num_args_ftype *frame_num_args);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (FRAME_NUM_ARGS)
#define FRAME_NUM_ARGS(frame) (gdbarch_frame_num_args (current_gdbarch, frame))
#endif
#endif

#if defined (STACK_ALIGN)
/* Legacy for systems yet to multi-arch STACK_ALIGN */
#define STACK_ALIGN_P() (1)
#endif

/* Default predicate for non- multi-arch targets. */
#if (!GDB_MULTI_ARCH) && !defined (STACK_ALIGN_P)
#define STACK_ALIGN_P() (0)
#endif

extern int gdbarch_stack_align_p (struct gdbarch *gdbarch);
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (STACK_ALIGN_P)
#define STACK_ALIGN_P() (gdbarch_stack_align_p (current_gdbarch))
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (STACK_ALIGN)
#define STACK_ALIGN(sp) (internal_error ("STACK_ALIGN"), 0)
#endif

typedef CORE_ADDR (gdbarch_stack_align_ftype) (CORE_ADDR sp);
extern CORE_ADDR gdbarch_stack_align (struct gdbarch *gdbarch, CORE_ADDR sp);
extern void set_gdbarch_stack_align (struct gdbarch *gdbarch, gdbarch_stack_align_ftype *stack_align);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (STACK_ALIGN)
#define STACK_ALIGN(sp) (gdbarch_stack_align (current_gdbarch, sp))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (EXTRA_STACK_ALIGNMENT_NEEDED)
#define EXTRA_STACK_ALIGNMENT_NEEDED (1)
#endif

extern int gdbarch_extra_stack_alignment_needed (struct gdbarch *gdbarch);
extern void set_gdbarch_extra_stack_alignment_needed (struct gdbarch *gdbarch, int extra_stack_alignment_needed);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (EXTRA_STACK_ALIGNMENT_NEEDED)
#define EXTRA_STACK_ALIGNMENT_NEEDED (gdbarch_extra_stack_alignment_needed (current_gdbarch))
#endif
#endif

#if defined (REG_STRUCT_HAS_ADDR)
/* Legacy for systems yet to multi-arch REG_STRUCT_HAS_ADDR */
#define REG_STRUCT_HAS_ADDR_P() (1)
#endif

/* Default predicate for non- multi-arch targets. */
#if (!GDB_MULTI_ARCH) && !defined (REG_STRUCT_HAS_ADDR_P)
#define REG_STRUCT_HAS_ADDR_P() (0)
#endif

extern int gdbarch_reg_struct_has_addr_p (struct gdbarch *gdbarch);
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REG_STRUCT_HAS_ADDR_P)
#define REG_STRUCT_HAS_ADDR_P() (gdbarch_reg_struct_has_addr_p (current_gdbarch))
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (REG_STRUCT_HAS_ADDR)
#define REG_STRUCT_HAS_ADDR(gcc_p, type) (internal_error ("REG_STRUCT_HAS_ADDR"), 0)
#endif

typedef int (gdbarch_reg_struct_has_addr_ftype) (int gcc_p, struct type *type);
extern int gdbarch_reg_struct_has_addr (struct gdbarch *gdbarch, int gcc_p, struct type *type);
extern void set_gdbarch_reg_struct_has_addr (struct gdbarch *gdbarch, gdbarch_reg_struct_has_addr_ftype *reg_struct_has_addr);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (REG_STRUCT_HAS_ADDR)
#define REG_STRUCT_HAS_ADDR(gcc_p, type) (gdbarch_reg_struct_has_addr (current_gdbarch, gcc_p, type))
#endif
#endif

#if defined (SAVE_DUMMY_FRAME_TOS)
/* Legacy for systems yet to multi-arch SAVE_DUMMY_FRAME_TOS */
#define SAVE_DUMMY_FRAME_TOS_P() (1)
#endif

/* Default predicate for non- multi-arch targets. */
#if (!GDB_MULTI_ARCH) && !defined (SAVE_DUMMY_FRAME_TOS_P)
#define SAVE_DUMMY_FRAME_TOS_P() (0)
#endif

extern int gdbarch_save_dummy_frame_tos_p (struct gdbarch *gdbarch);
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (SAVE_DUMMY_FRAME_TOS_P)
#define SAVE_DUMMY_FRAME_TOS_P() (gdbarch_save_dummy_frame_tos_p (current_gdbarch))
#endif

/* Default (function) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (SAVE_DUMMY_FRAME_TOS)
#define SAVE_DUMMY_FRAME_TOS(sp) (internal_error ("SAVE_DUMMY_FRAME_TOS"), 0)
#endif

typedef void (gdbarch_save_dummy_frame_tos_ftype) (CORE_ADDR sp);
extern void gdbarch_save_dummy_frame_tos (struct gdbarch *gdbarch, CORE_ADDR sp);
extern void set_gdbarch_save_dummy_frame_tos (struct gdbarch *gdbarch, gdbarch_save_dummy_frame_tos_ftype *save_dummy_frame_tos);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (SAVE_DUMMY_FRAME_TOS)
#define SAVE_DUMMY_FRAME_TOS(sp) (gdbarch_save_dummy_frame_tos (current_gdbarch, sp))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (TARGET_FLOAT_FORMAT)
#define TARGET_FLOAT_FORMAT (default_float_format (current_gdbarch))
#endif

extern const struct floatformat * gdbarch_float_format (struct gdbarch *gdbarch);
extern void set_gdbarch_float_format (struct gdbarch *gdbarch, const struct floatformat * float_format);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_FLOAT_FORMAT)
#define TARGET_FLOAT_FORMAT (gdbarch_float_format (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (TARGET_DOUBLE_FORMAT)
#define TARGET_DOUBLE_FORMAT (default_double_format (current_gdbarch))
#endif

extern const struct floatformat * gdbarch_double_format (struct gdbarch *gdbarch);
extern void set_gdbarch_double_format (struct gdbarch *gdbarch, const struct floatformat * double_format);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_DOUBLE_FORMAT)
#define TARGET_DOUBLE_FORMAT (gdbarch_double_format (current_gdbarch))
#endif
#endif

/* Default (value) for non- multi-arch platforms. */
#if (!GDB_MULTI_ARCH) && !defined (TARGET_LONG_DOUBLE_FORMAT)
#define TARGET_LONG_DOUBLE_FORMAT (&floatformat_unknown)
#endif

extern const struct floatformat * gdbarch_long_double_format (struct gdbarch *gdbarch);
extern void set_gdbarch_long_double_format (struct gdbarch *gdbarch, const struct floatformat * long_double_format);
#if GDB_MULTI_ARCH
#if (GDB_MULTI_ARCH > GDB_MULTI_ARCH_PARTIAL) || !defined (TARGET_LONG_DOUBLE_FORMAT)
#define TARGET_LONG_DOUBLE_FORMAT (gdbarch_long_double_format (current_gdbarch))
#endif
#endif

extern struct gdbarch_tdep *gdbarch_tdep (struct gdbarch *gdbarch);


/* Mechanism for co-ordinating the selection of a specific
   architecture.

   GDB targets (*-tdep.c) can register an interest in a specific
   architecture.  Other GDB components can register a need to maintain
   per-architecture data.

   The mechanisms below ensures that there is only a loose connection
   between the set-architecture command and the various GDB
   components.  Each component can independantly register their need
   to maintain architecture specific data with gdbarch.

   Pragmatics:

   Previously, a single TARGET_ARCHITECTURE_HOOK was provided.  It
   didn't scale.

   The more traditional mega-struct containing architecture specific
   data for all the various GDB components was also considered.  Since
   GDB is built from a variable number of (fairly independant)
   components it was determined that the global aproach was not
   applicable. */


/* Register a new architectural family with GDB.

   Register support for the specified ARCHITECTURE with GDB.  When
   gdbarch determines that the specified architecture has been
   selected, the corresponding INIT function is called.

   --

   The INIT function takes two parameters: INFO which contains the
   information available to gdbarch about the (possibly new)
   architecture; ARCHES which is a list of the previously created
   ``struct gdbarch'' for this architecture.

   The INIT function parameter INFO shall, as far as possible, be
   pre-initialized with information obtained from INFO.ABFD or
   previously selected architecture (if similar).  INIT shall ensure
   that the INFO.BYTE_ORDER is non-zero.

   The INIT function shall return any of: NULL - indicating that it
   doesn't reconize the selected architecture; an existing ``struct
   gdbarch'' from the ARCHES list - indicating that the new
   architecture is just a synonym for an earlier architecture (see
   gdbarch_list_lookup_by_info()); a newly created ``struct gdbarch''
   - that describes the selected architecture (see gdbarch_alloc()).

   The DUMP_TDEP function shall print out all target specific values.
   Care should be taken to ensure that the function works in both the
   multi-arch and non- multi-arch cases. */

struct gdbarch_list
{
  struct gdbarch *gdbarch;
  struct gdbarch_list *next;
};

struct gdbarch_info
{
  /* Use default: bfd_arch_unknown (ZERO). */
  enum bfd_architecture bfd_architecture;

  /* Use default: NULL (ZERO). */
  const struct bfd_arch_info *bfd_arch_info;

  /* Use default: 0 (ZERO). */
  int byte_order;

  /* Use default: NULL (ZERO). */
  bfd *abfd;

  /* Use default: NULL (ZERO). */
  struct gdbarch_tdep_info *tdep_info;
};

typedef struct gdbarch *(gdbarch_init_ftype) (struct gdbarch_info info, struct gdbarch_list *arches);
typedef void (gdbarch_dump_tdep_ftype) (struct gdbarch *gdbarch, struct ui_file *file);

/* DEPRECATED - use gdbarch_register() */
extern void register_gdbarch_init (enum bfd_architecture architecture, gdbarch_init_ftype *);

extern void gdbarch_register (enum bfd_architecture architecture,
                              gdbarch_init_ftype *,
                              gdbarch_dump_tdep_ftype *);


/* Return a freshly allocated, NULL terminated, array of the valid
   architecture names.  Since architectures are registered during the
   _initialize phase this function only returns useful information
   once initialization has been completed. */

extern const char **gdbarch_printable_names (void);


/* Helper function.  Search the list of ARCHES for a GDBARCH that
   matches the information provided by INFO. */

extern struct gdbarch_list *gdbarch_list_lookup_by_info (struct gdbarch_list *arches,  const struct gdbarch_info *info);


/* Helper function.  Create a preliminary ``struct gdbarch''.  Perform
   basic initialization using values obtained from the INFO andTDEP
   parameters.  set_gdbarch_*() functions are called to complete the
   initialization of the object. */

extern struct gdbarch *gdbarch_alloc (const struct gdbarch_info *info, struct gdbarch_tdep *tdep);


/* Helper function.  Free a partially-constructed ``struct gdbarch''.
   It is assumed that the caller freeds the ``struct
   gdbarch_tdep''. */

extern void gdbarch_free (struct gdbarch *);


/* Helper function. Force an update of the current architecture.  Used
   by legacy targets that have added their own target specific
   architecture manipulation commands.

   The INFO parameter shall be fully initialized (``memset (&INFO,
   sizeof (info), 0)'' set relevant fields) before gdbarch_update_p()
   is called.  gdbarch_update_p() shall initialize any ``default''
   fields using information obtained from the previous architecture or
   INFO.ABFD (if specified) before calling the corresponding
   architectures INIT function.

   Returns non-zero if the update succeeds */

extern int gdbarch_update_p (struct gdbarch_info info);



/* Register per-architecture data-pointer.

   Reserve space for a per-architecture data-pointer.  An identifier
   for the reserved data-pointer is returned.  That identifer should
   be saved in a local static.

   When a new architecture is selected, INIT() is called.  When a
   previous architecture is re-selected, the per-architecture
   data-pointer for that previous architecture is restored (INIT() is
   not called).

   INIT() shall return the initial value for the per-architecture
   data-pointer for the current architecture.

   Multiple registrarants for any architecture are allowed (and
   strongly encouraged).  */

typedef void *(gdbarch_data_ftype) (void);
extern struct gdbarch_data *register_gdbarch_data (gdbarch_data_ftype *init);

/* Return the value of the per-architecture data-pointer for the
   current architecture. */

extern void *gdbarch_data (struct gdbarch_data*);



/* Register per-architecture memory region.

   Provide a memory-region swap mechanism.  Per-architecture memory
   region are created.  These memory regions are swapped whenever the
   architecture is changed.  For a new architecture, the memory region
   is initialized with zero (0) and the INIT function is called.

   Memory regions are swapped / initialized in the order that they are
   registered.  NULL DATA and/or INIT values can be specified.

   New code should use register_gdbarch_data(). */

typedef void (gdbarch_swap_ftype) (void);
extern void register_gdbarch_swap (void *data, unsigned long size, gdbarch_swap_ftype *init);
#define REGISTER_GDBARCH_SWAP(VAR) register_gdbarch_swap (&(VAR), sizeof ((VAR)), NULL)



/* The target-system-dependant byte order is dynamic */

/* TARGET_BYTE_ORDER_SELECTABLE_P determines if the target endianness
   is selectable at runtime.  The user can use the ``set endian''
   command to change it.  TARGET_BYTE_ORDER_AUTO is nonzero when
   target_byte_order should be auto-detected (from the program image
   say). */

#if GDB_MULTI_ARCH
/* Multi-arch GDB is always bi-endian. */
#define TARGET_BYTE_ORDER_SELECTABLE_P 1
#endif

#ifndef TARGET_BYTE_ORDER_SELECTABLE_P
/* compat - Catch old targets that define TARGET_BYTE_ORDER_SLECTABLE
   when they should have defined TARGET_BYTE_ORDER_SELECTABLE_P 1 */
#ifdef TARGET_BYTE_ORDER_SELECTABLE
#define TARGET_BYTE_ORDER_SELECTABLE_P 1
#else
#define TARGET_BYTE_ORDER_SELECTABLE_P 0
#endif
#endif

extern int target_byte_order;
#ifdef TARGET_BYTE_ORDER_SELECTABLE
/* compat - Catch old targets that define TARGET_BYTE_ORDER_SELECTABLE
   and expect defs.h to re-define TARGET_BYTE_ORDER. */
#undef TARGET_BYTE_ORDER
#endif
#ifndef TARGET_BYTE_ORDER
#define TARGET_BYTE_ORDER (target_byte_order + 0)
#endif

extern int target_byte_order_auto;
#ifndef TARGET_BYTE_ORDER_AUTO
#define TARGET_BYTE_ORDER_AUTO (target_byte_order_auto + 0)
#endif



/* The target-system-dependant BFD architecture is dynamic */

extern int target_architecture_auto;
#ifndef TARGET_ARCHITECTURE_AUTO
#define TARGET_ARCHITECTURE_AUTO (target_architecture_auto + 0)
#endif

extern const struct bfd_arch_info *target_architecture;
#ifndef TARGET_ARCHITECTURE
#define TARGET_ARCHITECTURE (target_architecture + 0)
#endif


/* The target-system-dependant disassembler is semi-dynamic */

#include "dis-asm.h"		/* Get defs for disassemble_info */

extern int dis_asm_read_memory (bfd_vma memaddr, bfd_byte *myaddr,
				unsigned int len, disassemble_info *info);

extern void dis_asm_memory_error (int status, bfd_vma memaddr,
				  disassemble_info *info);

extern void dis_asm_print_address (bfd_vma addr,
				   disassemble_info *info);

extern int (*tm_print_insn) (bfd_vma, disassemble_info*);
extern disassemble_info tm_print_insn_info;
#ifndef TARGET_PRINT_INSN
#define TARGET_PRINT_INSN(vma, info) (*tm_print_insn) (vma, info)
#endif
#ifndef TARGET_PRINT_INSN_INFO
#define TARGET_PRINT_INSN_INFO (&tm_print_insn_info)
#endif



/* Explicit test for D10V architecture.
   USE of these macro's is *STRONGLY* discouraged. */

#define GDB_TARGET_IS_D10V (TARGET_ARCHITECTURE->arch == bfd_arch_d10v)


/* Fallback definition for EXTRACT_STRUCT_VALUE_ADDRESS */
#ifndef EXTRACT_STRUCT_VALUE_ADDRESS
#define EXTRACT_STRUCT_VALUE_ADDRESS_P (0)
#define EXTRACT_STRUCT_VALUE_ADDRESS(X) (internal_error ("gdbarch: EXTRACT_STRUCT_VALUE_ADDRESS"), 0)
#else
#ifndef EXTRACT_STRUCT_VALUE_ADDRESS_P
#define EXTRACT_STRUCT_VALUE_ADDRESS_P (1)
#endif
#endif


/* Set the dynamic target-system-dependant parameters (architecture,
   byte-order, ...) using information found in the BFD */

extern void set_gdbarch_from_file (bfd *);


/* Initialize the current architecture to the "first" one we find on
   our list.  */

extern void initialize_current_architecture (void);


/* gdbarch trace variable */
extern int gdbarch_debug;

extern void gdbarch_dump (struct gdbarch *gdbarch, struct ui_file *file);

#endif
