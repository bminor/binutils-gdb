/* Target-dependent code for Motorola 68HC11
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.
   Contributed by Stephane Carrez, stcarrez@worldnet.fr

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "value.h"
#include "inferior.h"
#include "dis-asm.h"  
#include "symfile.h"
#include "objfiles.h"
#include "arch-utils.h"

#include "target.h"
#include "opcode/m68hc11.h"

/* Register numbers of various important registers.
   Note that some of these values are "real" register numbers,
   and correspond to the general registers of the machine,
   and some are "phony" register numbers which are too large
   to be actual register numbers as far as the user is concerned
   but do serve to get the desired values when passed to read_register.  */

#define HARD_X_REGNUM 	0
#define HARD_D_REGNUM	1
#define HARD_Y_REGNUM   2
#define HARD_SP_REGNUM 	3
#define HARD_PC_REGNUM 	4

#define HARD_A_REGNUM   5
#define HARD_B_REGNUM   6
#define HARD_CCR_REGNUM 7
#define M68HC11_LAST_HARD_REG (HARD_CCR_REGNUM)

/* Z is replaced by X or Y by gcc during machine reorg.
   ??? There is no way to get it and even know whether
   it's in X or Y or in ZS.  */
#define SOFT_Z_REGNUM        8

/* Soft registers.  These registers are special.  There are treated
   like normal hard registers by gcc and gdb (ie, within dwarf2 info).
   They are physically located in memory.  */
#define SOFT_FP_REGNUM       9
#define SOFT_TMP_REGNUM     10
#define SOFT_ZS_REGNUM      11
#define SOFT_XY_REGNUM      12
#define SOFT_D1_REGNUM      13
#define SOFT_D32_REGNUM     (SOFT_D1_REGNUM+31)
#define M68HC11_MAX_SOFT_REGS 32

#define M68HC11_NUM_REGS        (8)
#define M68HC11_NUM_PSEUDO_REGS (M68HC11_MAX_SOFT_REGS+5)
#define M68HC11_ALL_REGS        (M68HC11_NUM_REGS+M68HC11_NUM_PSEUDO_REGS)

#define M68HC11_REG_SIZE    (2)

struct gdbarch_tdep
  {
    /* from the elf header */
    int elf_flags;
  };

struct frame_extra_info
{
  int frame_reg;
  CORE_ADDR return_pc;
  CORE_ADDR dummy;
  int frameless;
  int size;
};

/* Table of registers for 68HC11.  This includes the hard registers
   and the soft registers used by GCC.  */
static char *
m68hc11_register_names[] =
{
  "x",    "d",    "y",    "sp",   "pc",   "a",    "b",
  "ccr",  "z",    "frame","tmp",  "zs",   "xy",
  "d1",   "d2",   "d3",   "d4",   "d5",   "d6",   "d7",
  "d8",   "d9",   "d10",  "d11",  "d12",  "d13",  "d14",
  "d15",  "d16",  "d17",  "d18",  "d19",  "d20",  "d21",
  "d22",  "d23",  "d24",  "d25",  "d26",  "d27",  "d28",
  "d29",  "d30",  "d31",  "d32"
};

struct m68hc11_soft_reg 
{
  const char *name;
  CORE_ADDR   addr;
};

static struct m68hc11_soft_reg soft_regs[M68HC11_ALL_REGS];

#define M68HC11_FP_ADDR soft_regs[SOFT_FP_REGNUM].addr

static int soft_min_addr;
static int soft_max_addr;
static int soft_reg_initialized = 0;

/* Stack pointer correction value.  For 68hc11, the stack pointer points
   to the next push location.  An offset of 1 must be applied to obtain
   the address where the last value is saved.  For 68hc12, the stack
   pointer points to the last value pushed.  No offset is necessary.  */
static int stack_correction = 1;

/* Look in the symbol table for the address of a pseudo register
   in memory.  If we don't find it, pretend the register is not used
   and not available.  */
static void
m68hc11_get_register_info (struct m68hc11_soft_reg *reg, const char *name)
{
  struct minimal_symbol *msymbol;

  msymbol = lookup_minimal_symbol (name, NULL, NULL);
  if (msymbol)
    {
      reg->addr = SYMBOL_VALUE_ADDRESS (msymbol);
      reg->name = xstrdup (name);

      /* Keep track of the address range for soft registers.  */
      if (reg->addr < (CORE_ADDR) soft_min_addr)
        soft_min_addr = reg->addr;
      if (reg->addr > (CORE_ADDR) soft_max_addr)
        soft_max_addr = reg->addr;
    }
  else
    {
      reg->name = 0;
      reg->addr = 0;
    }
}

/* Initialize the table of soft register addresses according
   to the symbol table.  */
  static void
m68hc11_initialize_register_info (void)
{
  int i;

  if (soft_reg_initialized)
    return;
  
  soft_min_addr = INT_MAX;
  soft_max_addr = 0;
  for (i = 0; i < M68HC11_ALL_REGS; i++)
    {
      soft_regs[i].name = 0;
    }
  
  m68hc11_get_register_info (&soft_regs[SOFT_FP_REGNUM], "_.frame");
  m68hc11_get_register_info (&soft_regs[SOFT_TMP_REGNUM], "_.tmp");
  m68hc11_get_register_info (&soft_regs[SOFT_ZS_REGNUM], "_.z");
  soft_regs[SOFT_Z_REGNUM] = soft_regs[SOFT_ZS_REGNUM];
  m68hc11_get_register_info (&soft_regs[SOFT_XY_REGNUM], "_.xy");

  for (i = SOFT_D1_REGNUM; i < M68HC11_MAX_SOFT_REGS; i++)
    {
      char buf[10];

      sprintf (buf, "_.d%d", i - SOFT_D1_REGNUM + 1);
      m68hc11_get_register_info (&soft_regs[i], buf);
    }

  if (soft_regs[SOFT_FP_REGNUM].name == 0)
    {
      warning ("No frame soft register found in the symbol table.\n");
      warning ("Stack backtrace will not work.\n");
    }
  soft_reg_initialized = 1;
}

/* Given an address in memory, return the soft register number if
   that address corresponds to a soft register.  Returns -1 if not.  */
static int
m68hc11_which_soft_register (CORE_ADDR addr)
{
  int i;
  
  if (addr < soft_min_addr || addr > soft_max_addr)
    return -1;
  
  for (i = SOFT_FP_REGNUM; i < M68HC11_ALL_REGS; i++)
    {
      if (soft_regs[i].name && soft_regs[i].addr == addr)
        return i;
    }
  return -1;
}

/* Fetch a pseudo register.  The 68hc11 soft registers are treated like
   pseudo registers.  They are located in memory.  Translate the register
   fetch into a memory read.  */
void
m68hc11_fetch_pseudo_register (int regno)
{
  char buf[MAX_REGISTER_RAW_SIZE];

  m68hc11_initialize_register_info ();
  
  /* Fetch a soft register: translate into a memory read.  */
  if (soft_regs[regno].name)
    {
      target_read_memory (soft_regs[regno].addr, buf, 2);
    }
  else
    {
      memset (buf, 0, 2);
    }
  supply_register (regno, buf);
}

/* Store a pseudo register.  Translate the register store
   into a memory write.  */
static void
m68hc11_store_pseudo_register (int regno)
{
  m68hc11_initialize_register_info ();

  /* Store a soft register: translate into a memory write.  */
  if (soft_regs[regno].name)
    {
      char buf[MAX_REGISTER_RAW_SIZE];

      read_register_gen (regno, buf);
      target_write_memory (soft_regs[regno].addr, buf, 2);
    }
}

static char *
m68hc11_register_name (int reg_nr)
{
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= M68HC11_ALL_REGS)
    return NULL;

  /* If we don't know the address of a soft register, pretend it
     does not exist.  */
  if (reg_nr > M68HC11_LAST_HARD_REG && soft_regs[reg_nr].name == 0)
    return NULL;
  return m68hc11_register_names[reg_nr];
}

static unsigned char *
m68hc11_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  static unsigned char breakpoint[] = {0x0};
  
  *lenptr = sizeof (breakpoint);
  return breakpoint;
}

/* Immediately after a function call, return the saved pc before the frame
   is setup.  */

static CORE_ADDR
m68hc11_saved_pc_after_call (struct frame_info *frame)
{
  CORE_ADDR addr;
  
  addr = read_register (HARD_SP_REGNUM) + stack_correction;
  addr &= 0x0ffff;
  return read_memory_integer (addr, 2) & 0x0FFFF;
}

static CORE_ADDR
m68hc11_frame_saved_pc (struct frame_info *frame)
{
  return frame->extra_info->return_pc;
}

static CORE_ADDR
m68hc11_frame_args_address (struct frame_info *frame)
{
  return frame->frame;
}

static CORE_ADDR
m68hc11_frame_locals_address (struct frame_info *frame)
{
  return frame->frame;
}

/* Discard from the stack the innermost frame, restoring all saved
   registers.  */

static void
m68hc11_pop_frame (void)
{
  register struct frame_info *frame = get_current_frame ();
  register CORE_ADDR fp, sp;
  register int regnum;

  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    generic_pop_dummy_frame ();
  else
    {
      fp = FRAME_FP (frame);
      FRAME_INIT_SAVED_REGS (frame);

      /* Copy regs from where they were saved in the frame.  */
      for (regnum = 0; regnum < M68HC11_ALL_REGS; regnum++)
	if (frame->saved_regs[regnum])
	  write_register (regnum,
                          read_memory_integer (frame->saved_regs[regnum], 2));

      write_register (HARD_PC_REGNUM, frame->extra_info->return_pc);
      sp = fp + frame->extra_info->size;
      write_register (HARD_SP_REGNUM, sp);
    }
  flush_cached_frames ();
}

/* Analyze the function prologue to find some information
   about the function:
    - the PC of the first line (for m68hc11_skip_prologue)
    - the offset of the previous frame saved address (from current frame)
    - the soft registers which are pushed.  */
static void
m68hc11_guess_from_prologue (CORE_ADDR pc, CORE_ADDR fp,
                             CORE_ADDR *first_line,
                             int *frame_offset, CORE_ADDR *pushed_regs)
{
  CORE_ADDR save_addr;
  CORE_ADDR func_end;
  unsigned char op0, op1, op2;
  int add_sp_mode;
  int sp_adjust = 0;
  int size;
  int found_frame_point;
  int saved_reg;
  CORE_ADDR first_pc;
  
  first_pc = get_pc_function_start (pc);
  size = 0;

  m68hc11_initialize_register_info ();
  if (first_pc == 0)
    {
      *frame_offset = 0;
      *first_line   = pc;
      return;
    }

#define OP_PAGE2 (0x18)
#define OP_LDX  (0xde)
#define OP_LDY  (0xde)
#define OP_PSHX (0x3c)
#define OP_PSHY (0x3c)
#define OP_STS  (0x9f)
#define OP_TSX  (0x30)
#define OP_TSY  (0x30)
#define OP_XGDX (0x8f)
#define OP_XGDY (0x8f)
#define OP_ADDD (0xc3)
#define OP_TXS  (0x35)
#define OP_TYS  (0x35)
#define OP_DES  (0x34)

  /* The 68hc11 stack is as follows:


     |           |
     +-----------+
     |           |
     | args      |
     |           |
     +-----------+
     | PC-return |
     +-----------+
     | Old frame |
     +-----------+
     |           |
     | Locals    |
     |           |
     +-----------+ <--- current frame
     |           |

     With most processors (like 68K) the previous frame can be computed
     easily because it is always at a fixed offset (see link/unlink).
     That is, locals are accessed with negative offsets, arguments are
     accessed with positive ones.  Since 68hc11 only supports offsets
     in the range [0..255], the frame is defined at the bottom of
     locals (see picture).

     The purpose of the analysis made here is to find out the size
     of locals in this function.  An alternative to this is to use
     DWARF2 info.  This would be better but I don't know how to
     access dwarf2 debug from this function.
     
     Walk from the function entry point to the point where we save
     the frame.  While walking instructions, compute the size of bytes
     which are pushed.  This gives us the index to access the previous
     frame.

     We limit the search to 128 bytes so that the algorithm is bounded
     in case of random and wrong code.  We also stop and abort if
     we find an instruction which is not supposed to appear in the
     prologue (as generated by gcc 2.95, 2.96).
  */
  pc = first_pc;
  func_end = pc + 128;
  add_sp_mode = 0;
  found_frame_point = 0;
  while (pc + 2 < func_end)
    {
      op0 = read_memory_unsigned_integer (pc, 1);
      op1 = read_memory_unsigned_integer (pc + 1, 1);
      op2 = read_memory_unsigned_integer (pc + 2, 1);
      
      /* ldx *frame */
      if (op0 == OP_LDX && op1 == M68HC11_FP_ADDR)
        {
          pc += 2;
        }

      /* ldy *frame */
      else if (op0 == OP_PAGE2 && op1 == OP_LDY
               && op2 == M68HC11_FP_ADDR)
        {
          pc += 3;
        }

      /* pshx */
      else if (op0 == OP_PSHX)
        {
          pc += 1;
          size += 2;
        }

      /* pshy */
      else if (op0 == OP_PAGE2 && op1 == OP_PSHX)
        {
          pc += 2;
          size += 2;
        }

      /* sts *frame */
      else if (op0 == OP_STS && op1 == M68HC11_FP_ADDR)
        {
          found_frame_point = 1;
          pc += 2;
          break;
        }
      else if (op0 == OP_TSX && op1 == OP_XGDX)
        {
          add_sp_mode  = 1;
          pc += 2;
        }
      /* des to allocate 1 byte on the stack */
      else if (op0 == OP_DES)
        {
          pc += 1;
          size += 1;
        }
      else if (op0 == OP_PAGE2 && op1 == OP_TSY && op2 == OP_PAGE2)
        {
          op0 = read_memory_unsigned_integer (pc + 3, 1);
          if (op0 != OP_XGDY)
            break;
          
          add_sp_mode  = 2;
          pc += 4;
        }
      else if (add_sp_mode && op0 == OP_ADDD)
        {
          sp_adjust = read_memory_unsigned_integer (pc + 1, 2);
          if (sp_adjust & 0x8000)
            sp_adjust |= 0xffff0000L;

          sp_adjust = -sp_adjust;
          add_sp_mode |= 4;
          pc += 3;
        }
      else if (add_sp_mode == (1 | 4) && op0 == OP_XGDX
               && op1 == OP_TXS)
        {
          size += sp_adjust;
          pc += 2;
          add_sp_mode = 0;
        }
      else if (add_sp_mode == (2 | 4) && op0 == OP_PAGE2
               && op1 == OP_XGDY && op2 == OP_PAGE2)
        {
          op0 = read_memory_unsigned_integer (pc + 3, 1);
          if (op0 != OP_TYS)
            break;

          size += sp_adjust;
          pc += 4;
          add_sp_mode = 0;
        }
      else
        {
          break;
        }
    }

  if (found_frame_point == 0)
    {
      *frame_offset = 0;
    }
  else
    {
      *frame_offset = size;
    }

  /* Now, look forward to see how many registers are pushed on the stack.
     We look only for soft registers so there must be a first LDX *REG
     before a PSHX.  */
  saved_reg = -1;
  save_addr = fp;
  while (pc + 2 < func_end)
    {
      op0 = read_memory_unsigned_integer (pc, 1);
      op1 = read_memory_unsigned_integer (pc + 1, 1);
      op2 = read_memory_unsigned_integer (pc + 2, 1);
      if (op0 == OP_LDX)
        {
          saved_reg = m68hc11_which_soft_register (op1);
          if (saved_reg < 0 || saved_reg == SOFT_FP_REGNUM)
            break;
          
          pc += 2;
        }
      else if (op0 == OP_PAGE2 && op1 == OP_LDY)
        {
          saved_reg = m68hc11_which_soft_register (op2);
          if (saved_reg < 0 || saved_reg == SOFT_FP_REGNUM)
            break;
          
          pc += 3;
        }
      else if (op0 == OP_PSHX)
        {
          /* If there was no load, this is a push for a function call.  */
          if (saved_reg < 0 || saved_reg >= M68HC11_ALL_REGS)
            break;

          /* Keep track of the address where that register is saved
             on the stack.  */
          save_addr -= 2;
          if (pushed_regs)
            pushed_regs[saved_reg] = save_addr;

          pc += 1;
          saved_reg = -1;
        }
      else if (op0 == OP_PAGE2 && op1 == OP_PSHY)
        {
          if (saved_reg < 0 || saved_reg >= M68HC11_ALL_REGS)
            break;
          
          /* Keep track of the address where that register is saved
             on the stack.  */
          save_addr -= 2;
          if (pushed_regs)
            pushed_regs[saved_reg] = save_addr;

          pc += 2;
          saved_reg = -1;
        }
      else
        {
          break;
        }
    }
  *first_line  = pc;
}

static CORE_ADDR
m68hc11_skip_prologue (CORE_ADDR pc)
{
  CORE_ADDR func_addr, func_end;
  struct symtab_and_line sal;
  int frame_offset;

  /* If we have line debugging information, then the end of the
     prologue should be the first assembly instruction of the
     first source line.  */
  if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    {
      sal = find_pc_line (func_addr, 0);
      if (sal.end && sal.end < func_end)
	return sal.end;
    }

  m68hc11_guess_from_prologue (pc, 0, &pc, &frame_offset, 0);
  return pc;
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.
*/

static CORE_ADDR
m68hc11_frame_chain (struct frame_info *frame)
{
  CORE_ADDR addr;

  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    return frame->frame;	/* dummy frame same as caller's frame */

  if (frame->extra_info->return_pc == 0
      || inside_entry_file (frame->extra_info->return_pc))
    return (CORE_ADDR) 0;

  if (frame->frame == 0)
    {
      return (CORE_ADDR) 0;
    }

  addr = frame->frame + frame->extra_info->size + stack_correction - 2;
  addr = read_memory_unsigned_integer (addr, 2) & 0x0FFFF;
  if (addr == 0)
    {
      return (CORE_ADDR) 0;
    }
    
  return addr;
}  

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.   sp is even more special: the address we
   return for it IS the sp for the next frame.  */
static void
m68hc11_frame_init_saved_regs (struct frame_info *fi)
{
  CORE_ADDR pc;
  CORE_ADDR addr;
  
  if (fi->saved_regs == NULL)
    frame_saved_regs_zalloc (fi);
  else
    memset (fi->saved_regs, 0, sizeof (fi->saved_regs));

  pc = fi->pc;
  m68hc11_guess_from_prologue (pc, fi->frame, &pc, &fi->extra_info->size,
                               fi->saved_regs);

  addr = fi->frame + fi->extra_info->size + stack_correction;
  fi->saved_regs[SOFT_FP_REGNUM] = addr - 2;
  fi->saved_regs[HARD_SP_REGNUM] = addr;
  fi->saved_regs[HARD_PC_REGNUM] = fi->saved_regs[HARD_SP_REGNUM];
}

static void
m68hc11_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{
  CORE_ADDR addr;

  fi->extra_info = (struct frame_extra_info *)
    frame_obstack_alloc (sizeof (struct frame_extra_info));
  
  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);
  
  m68hc11_frame_init_saved_regs (fi);

  if (fromleaf)
    {
      fi->extra_info->return_pc = m68hc11_saved_pc_after_call (fi);
    }
  else
    {
      addr = fi->frame + fi->extra_info->size + stack_correction;
      addr = read_memory_unsigned_integer (addr, 2) & 0x0ffff;
      fi->extra_info->return_pc = addr;
#if 0
      printf ("Pc@0x%04x, FR 0x%04x, size %d, read ret @0x%04x -> 0x%04x\n",
              fi->pc,
              fi->frame, fi->size,
              addr & 0x0ffff,
              fi->return_pc);
#endif
    }
}

/* Same as 'info reg' but prints the registers in a different way.  */
static void
show_regs (char *args, int from_tty)
{
  int ccr = read_register (HARD_CCR_REGNUM);
  int i;
  int nr;
  
  printf_filtered ("PC=%04x SP=%04x FP=%04x CCR=%02x %c%c%c%c%c%c%c%c\n",
		   (int) read_register (HARD_PC_REGNUM),
		   (int) read_register (HARD_SP_REGNUM),
		   (int) read_register (SOFT_FP_REGNUM),
		   ccr,
		   ccr & M6811_S_BIT ? 'S' : '-',
		   ccr & M6811_X_BIT ? 'X' : '-',
		   ccr & M6811_H_BIT ? 'H' : '-',
		   ccr & M6811_I_BIT ? 'I' : '-',
		   ccr & M6811_N_BIT ? 'N' : '-',
		   ccr & M6811_Z_BIT ? 'Z' : '-',
		   ccr & M6811_V_BIT ? 'V' : '-',
		   ccr & M6811_C_BIT ? 'C' : '-');

  printf_filtered ("D=%04x IX=%04x IY=%04x\n",
		   (int) read_register (HARD_D_REGNUM),
		   (int) read_register (HARD_X_REGNUM),
		   (int) read_register (HARD_Y_REGNUM));

  nr = 0;
  for (i = SOFT_D1_REGNUM; i < M68HC11_ALL_REGS; i++)
    {
      /* Skip registers which are not defined in the symbol table.  */
      if (soft_regs[i].name == 0)
        continue;
      
      printf_filtered ("D%d=%04x",
                       i - SOFT_D1_REGNUM + 1,
                       (int) read_register (i));
      nr++;
      if ((nr % 8) == 7)
        printf_filtered ("\n");
      else
        printf_filtered (" ");
    }
  if (nr && (nr % 8) != 7)
    printf_filtered ("\n");
}

static CORE_ADDR
m68hc11_push_arguments (int nargs,
                        value_ptr *args,
                        CORE_ADDR sp,
                        int struct_return,
                        CORE_ADDR struct_addr)
{
  int stack_alloc;
  int argnum;
  int first_stack_argnum;
  int stack_offset;
  struct type *type;
  char *val;
  int len;
  
  stack_alloc = 0;
  first_stack_argnum = 0;
  if (struct_return)
    {
      /* The struct is allocated on the stack and gdb used the stack
         pointer for the address of that struct.  We must apply the
         stack offset on the address.  */
      write_register (HARD_D_REGNUM, struct_addr + stack_correction);
    }
  else if (nargs > 0)
    {
      type = VALUE_TYPE (args[0]);
      len = TYPE_LENGTH (type);
      
      /* First argument is passed in D and X registers.  */
      if (len <= 4)
        {
          LONGEST v = extract_unsigned_integer (VALUE_CONTENTS (args[0]), len);
          first_stack_argnum = 1;
          write_register (HARD_D_REGNUM, v);
          if (len > 2)
            {
              v >>= 16;
              write_register (HARD_X_REGNUM, v);
            }
        }
    }
  for (argnum = first_stack_argnum; argnum < nargs; argnum++)
    {
      type = VALUE_TYPE (args[argnum]);
      stack_alloc += (TYPE_LENGTH (type) + 1) & ~2;
    }
  sp -= stack_alloc;

  stack_offset = stack_correction;
  for (argnum = first_stack_argnum; argnum < nargs; argnum++)
    {
      type = VALUE_TYPE (args[argnum]);
      len = TYPE_LENGTH (type);

      val = (char*) VALUE_CONTENTS (args[argnum]);
      write_memory (sp + stack_offset, val, len);
      stack_offset += len;
    }
  return sp;
}


/* Return a location where we can set a breakpoint that will be hit
   when an inferior function call returns.  */
CORE_ADDR
m68hc11_call_dummy_address (void)
{
  return (CORE_ADDR) read_register (HARD_PC_REGNUM);
}

static struct type *
m68hc11_register_virtual_type (int reg_nr)
{
  return builtin_type_uint16;
}

static void
m68hc11_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{
  /* The struct address computed by gdb is on the stack.
     It uses the stack pointer so we must apply the stack
     correction offset.  */
  write_register (HARD_D_REGNUM, addr + stack_correction);
}

static void
m68hc11_store_return_value (struct type *type, char *valbuf)
{
  write_register_bytes (REGISTER_BYTE (HARD_D_REGNUM),
                        valbuf, TYPE_LENGTH (type));
}


/* Given a return value in `regbuf' with a type `type', 
   extract and copy its value into `valbuf'.  */

static void
m68hc11_extract_return_value (struct type *type,
                              char *regbuf,
                              char *valbuf)
{
  int len = TYPE_LENGTH (type);
  
  if (len <= 2)
    {
      memcpy (valbuf, &regbuf[2], len);
    }
  else if (len <= 4)
    {
      memcpy (valbuf, regbuf, len);
    }
  else
    {
      error ("bad size for return value");
    }
}

/* Should call_function allocate stack space for a struct return?  */
static int
m68hc11_use_struct_convention (int gcc_p, struct type *type)
{
  return (TYPE_LENGTH (type) > 4);
}

static int
m68hc11_return_value_on_stack (struct type *type)
{
  return m68hc11_use_struct_convention (1, type);
}

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */
static CORE_ADDR
m68hc11_extract_struct_value_address (char *regbuf)
{
  return extract_address (&regbuf[HARD_D_REGNUM * 2],
                          REGISTER_RAW_SIZE (HARD_D_REGNUM));
}

/* Function: push_return_address (pc)
   Set up the return address for the inferior function call.
   Needed for targets where we don't actually execute a JSR/BSR instruction */

static CORE_ADDR
m68hc11_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
{
  char valbuf[2];
  
  pc = read_register (HARD_PC_REGNUM);
  sp -= 2;
  store_unsigned_integer (valbuf, 2, pc);
  write_memory (sp + stack_correction, valbuf, 2);
  return sp;
}

/* Index within `registers' of the first byte of the space for
   register N.  */
static int
m68hc11_register_byte (int reg_nr)
{
  return (reg_nr * M68HC11_REG_SIZE);
}

static int
m68hc11_register_raw_size (int reg_nr)
{
  return M68HC11_REG_SIZE;
}

static struct gdbarch *
m68hc11_gdbarch_init (struct gdbarch_info info,
                      struct gdbarch_list *arches)
{
  static LONGEST m68hc11_call_dummy_words[] =
  {0};
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  int elf_flags;

  /* Extract the elf_flags if available */
  elf_flags = 0;

  soft_reg_initialized = 0;
  
  /* try to find a pre-existing architecture */
  for (arches = gdbarch_list_lookup_by_info (arches, &info);
       arches != NULL;
       arches = gdbarch_list_lookup_by_info (arches->next, &info))
    {
      /* MIPS needs to be pedantic about which ABI the object is
         using. */
      if (gdbarch_tdep (current_gdbarch)->elf_flags != elf_flags)
	continue;
      return arches->gdbarch;
    }

  /* Need a new architecture. Fill in a target specific vector.  */
  tdep = (struct gdbarch_tdep *) xmalloc (sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);
  tdep->elf_flags = elf_flags;

  /* Initially set everything according to the ABI.  */
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 64);
  set_gdbarch_long_bit (gdbarch, 32);
  set_gdbarch_ptr_bit (gdbarch, 16);
  set_gdbarch_long_long_bit (gdbarch, 64);

  /* Set register info.  */
  set_gdbarch_fp0_regnum (gdbarch, -1);
  set_gdbarch_max_register_raw_size (gdbarch, 2);
  set_gdbarch_max_register_virtual_size (gdbarch, 2);
  set_gdbarch_register_raw_size (gdbarch, m68hc11_register_raw_size);
  set_gdbarch_register_virtual_size (gdbarch, m68hc11_register_raw_size);
  set_gdbarch_register_byte (gdbarch, m68hc11_register_byte);
  set_gdbarch_frame_init_saved_regs (gdbarch, m68hc11_frame_init_saved_regs);
  set_gdbarch_frame_args_skip (gdbarch, 0);

  set_gdbarch_read_pc (gdbarch, generic_target_read_pc);
  set_gdbarch_write_pc (gdbarch, generic_target_write_pc);
  set_gdbarch_read_fp (gdbarch, generic_target_read_fp);
  set_gdbarch_write_fp (gdbarch, generic_target_write_fp);
  set_gdbarch_read_sp (gdbarch, generic_target_read_sp);
  set_gdbarch_write_sp (gdbarch, generic_target_write_sp);

  set_gdbarch_num_regs (gdbarch, M68HC11_NUM_REGS);
  set_gdbarch_num_pseudo_regs (gdbarch, M68HC11_NUM_PSEUDO_REGS);
  set_gdbarch_sp_regnum (gdbarch, HARD_SP_REGNUM);
  set_gdbarch_fp_regnum (gdbarch, SOFT_FP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, HARD_PC_REGNUM);
  set_gdbarch_register_name (gdbarch, m68hc11_register_name);
  set_gdbarch_register_size (gdbarch, 2);
  set_gdbarch_register_bytes (gdbarch, M68HC11_ALL_REGS * 2);
  set_gdbarch_register_virtual_type (gdbarch, m68hc11_register_virtual_type);
  set_gdbarch_fetch_pseudo_register (gdbarch, m68hc11_fetch_pseudo_register);
  set_gdbarch_store_pseudo_register (gdbarch, m68hc11_store_pseudo_register);

  set_gdbarch_use_generic_dummy_frames (gdbarch, 1);
  set_gdbarch_call_dummy_length (gdbarch, 0);
  set_gdbarch_call_dummy_location (gdbarch, AT_ENTRY_POINT);
  set_gdbarch_call_dummy_address (gdbarch, m68hc11_call_dummy_address);
  set_gdbarch_call_dummy_breakpoint_offset_p (gdbarch, 1); /*???*/
  set_gdbarch_call_dummy_breakpoint_offset (gdbarch, 0);
  set_gdbarch_call_dummy_start_offset (gdbarch, 0);
  set_gdbarch_pc_in_call_dummy (gdbarch, generic_pc_in_call_dummy);
  set_gdbarch_call_dummy_words (gdbarch, m68hc11_call_dummy_words);
  set_gdbarch_sizeof_call_dummy_words (gdbarch,
                                       sizeof (m68hc11_call_dummy_words));
  set_gdbarch_call_dummy_p (gdbarch, 1);
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  set_gdbarch_get_saved_register (gdbarch, generic_get_saved_register);
  set_gdbarch_fix_call_dummy (gdbarch, generic_fix_call_dummy);
  set_gdbarch_extract_return_value (gdbarch, m68hc11_extract_return_value);
  set_gdbarch_push_arguments (gdbarch, m68hc11_push_arguments);
  set_gdbarch_push_dummy_frame (gdbarch, generic_push_dummy_frame);
  set_gdbarch_push_return_address (gdbarch, m68hc11_push_return_address);
  set_gdbarch_return_value_on_stack (gdbarch, m68hc11_return_value_on_stack);

  set_gdbarch_store_struct_return (gdbarch, m68hc11_store_struct_return);
  set_gdbarch_store_return_value (gdbarch, m68hc11_store_return_value);
  set_gdbarch_extract_struct_value_address (gdbarch,
                                            m68hc11_extract_struct_value_address);
  set_gdbarch_register_convertible (gdbarch, generic_register_convertible_not);


  set_gdbarch_frame_chain (gdbarch, m68hc11_frame_chain);
  set_gdbarch_frame_chain_valid (gdbarch, generic_file_frame_chain_valid);
  set_gdbarch_frame_saved_pc (gdbarch, m68hc11_frame_saved_pc);
  set_gdbarch_frame_args_address (gdbarch, m68hc11_frame_args_address);
  set_gdbarch_frame_locals_address (gdbarch, m68hc11_frame_locals_address);
  set_gdbarch_saved_pc_after_call (gdbarch, m68hc11_saved_pc_after_call);
  set_gdbarch_frame_num_args (gdbarch, frame_num_args_unknown);

  set_gdbarch_frame_chain_valid (gdbarch, func_frame_chain_valid);
  set_gdbarch_get_saved_register (gdbarch, generic_get_saved_register);

  set_gdbarch_store_struct_return (gdbarch, m68hc11_store_struct_return);
  set_gdbarch_store_return_value (gdbarch, m68hc11_store_return_value);
  set_gdbarch_extract_struct_value_address
    (gdbarch, m68hc11_extract_struct_value_address);
  set_gdbarch_use_struct_convention (gdbarch, m68hc11_use_struct_convention);
  set_gdbarch_init_extra_frame_info (gdbarch, m68hc11_init_extra_frame_info);
  set_gdbarch_pop_frame (gdbarch, m68hc11_pop_frame);
  set_gdbarch_skip_prologue (gdbarch, m68hc11_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_decr_pc_after_break (gdbarch, 0);
  set_gdbarch_function_start_offset (gdbarch, 0);
  set_gdbarch_breakpoint_from_pc (gdbarch, m68hc11_breakpoint_from_pc);

  set_gdbarch_believe_pcc_promotion (gdbarch, 1);
  set_gdbarch_ieee_float (gdbarch, 1);

  return gdbarch;
}

void
_initialize_m68hc11_tdep (void)
{
  register_gdbarch_init (bfd_arch_m68hc11, m68hc11_gdbarch_init);
  if (!tm_print_insn)		/* Someone may have already set it */
    tm_print_insn = print_insn_m68hc11;

  add_com ("regs", class_vars, show_regs, "Print all registers");
} 

