/* Target-dependent code for the HP PA architecture, for GDB.

   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993, 1994, 1995,
   1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software
   Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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

#include "defs.h"
#include "frame.h"
#include "bfd.h"
#include "inferior.h"
#include "value.h"
#include "regcache.h"
#include "completer.h"
#include "language.h"
#include "osabi.h"
#include "gdb_assert.h"
#include "infttrace.h"
#include "arch-utils.h"
/* For argument passing to the inferior */
#include "symtab.h"
#include "infcall.h"
#include "dis-asm.h"
#include "trad-frame.h"
#include "frame-unwind.h"
#include "frame-base.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <dl.h>
#include <sys/param.h>
#include <signal.h>

#include <sys/ptrace.h>
#include <machine/save_state.h>

#ifdef COFF_ENCAPSULATE
#include "a.out.encap.h"
#else
#endif

/*#include <sys/user.h>         After a.out.h  */
#include <sys/file.h>
#include "gdb_stat.h"
#include "gdb_wait.h"

#include "gdbcore.h"
#include "gdbcmd.h"
#include "target.h"
#include "symfile.h"
#include "objfiles.h"
#include "hppa-tdep.h"

/* Some local constants.  */
static const int hppa32_num_regs = 128;
static const int hppa64_num_regs = 96;

/* Get at various relevent fields of an instruction word. */
#define MASK_5 0x1f
#define MASK_11 0x7ff
#define MASK_14 0x3fff
#define MASK_21 0x1fffff

/* Define offsets into the call dummy for the _sr4export address.
   See comments related to CALL_DUMMY for more info.  */
#define SR4EXPORT_LDIL_OFFSET (INSTRUCTION_SIZE * 12)
#define SR4EXPORT_LDO_OFFSET (INSTRUCTION_SIZE * 13)

/* To support detection of the pseudo-initial frame
   that threads have. */
#define THREAD_INITIAL_FRAME_SYMBOL  "__pthread_exit"
#define THREAD_INITIAL_FRAME_SYM_LEN  sizeof(THREAD_INITIAL_FRAME_SYMBOL)

/* Sizes (in bytes) of the native unwind entries.  */
#define UNWIND_ENTRY_SIZE 16
#define STUB_UNWIND_ENTRY_SIZE 8

static int get_field (unsigned word, int from, int to);

static int extract_5_load (unsigned int);

static unsigned extract_5R_store (unsigned int);

static unsigned extract_5r_store (unsigned int);

struct unwind_table_entry *find_unwind_entry (CORE_ADDR);

static int extract_17 (unsigned int);

static int extract_21 (unsigned);

static int extract_14 (unsigned);

static void unwind_command (char *, int);

static int low_sign_extend (unsigned int, unsigned int);

static int sign_extend (unsigned int, unsigned int);

static int hppa_alignof (struct type *);

static int prologue_inst_adjust_sp (unsigned long);

static int is_branch (unsigned long);

static int inst_saves_gr (unsigned long);

static int inst_saves_fr (unsigned long);

static int compare_unwind_entries (const void *, const void *);

static void read_unwind_info (struct objfile *);

static void internalize_unwinds (struct objfile *,
				 struct unwind_table_entry *,
				 asection *, unsigned int,
				 unsigned int, CORE_ADDR);
static void record_text_segment_lowaddr (bfd *, asection *, void *);
/* FIXME: brobecker 2002-11-07: We will likely be able to make the
   following functions static, once we hppa is partially multiarched.  */
int hppa_reg_struct_has_addr (int gcc_p, struct type *type);
CORE_ADDR hppa_skip_prologue (CORE_ADDR pc);
CORE_ADDR hppa_skip_trampoline_code (CORE_ADDR pc);
int hppa_in_solib_call_trampoline (CORE_ADDR pc, char *name);
int hppa_in_solib_return_trampoline (CORE_ADDR pc, char *name);
int hppa_inner_than (CORE_ADDR lhs, CORE_ADDR rhs);
int hppa_pc_requires_run_before_use (CORE_ADDR pc);
int hppa_instruction_nullified (void);
int hppa_cannot_store_register (int regnum);
CORE_ADDR hppa_smash_text_address (CORE_ADDR addr);
CORE_ADDR hppa_target_read_pc (ptid_t ptid);
void hppa_target_write_pc (CORE_ADDR v, ptid_t ptid);

typedef struct
  {
    struct minimal_symbol *msym;
    CORE_ADDR solib_handle;
    CORE_ADDR return_val;
  }
args_for_find_stub;

static int cover_find_stub_with_shl_get (void *);

static int is_pa_2 = 0;		/* False */

/* This is declared in symtab.c; set to 1 in hp-symtab-read.c */
extern int hp_som_som_object_present;

/* In breakpoint.c */
extern int exception_catchpoints_are_fragile;

/* Handle 32/64-bit struct return conventions.  */

static enum return_value_convention
hppa32_return_value (struct gdbarch *gdbarch,
		     struct type *type, struct regcache *regcache,
		     void *readbuf, const void *writebuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      if (readbuf != NULL)
	regcache_cooked_read_part (regcache, FP4_REGNUM, 0,
				   TYPE_LENGTH (type), readbuf);
      if (writebuf != NULL)
	regcache_cooked_write_part (regcache, FP4_REGNUM, 0,
				    TYPE_LENGTH (type), writebuf);
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
  if (TYPE_LENGTH (type) <= 2 * 4)
    {
      /* The value always lives in the right hand end of the register
	 (or register pair)?  */
      int b;
      int reg = 28;
      int part = TYPE_LENGTH (type) % 4;
      /* The left hand register contains only part of the value,
	 transfer that first so that the rest can be xfered as entire
	 4-byte registers.  */
      if (part > 0)
	{
	  if (readbuf != NULL)
	    regcache_cooked_read_part (regcache, reg, 4 - part,
				       part, readbuf);
	  if (writebuf != NULL)
	    regcache_cooked_write_part (regcache, reg, 4 - part,
					part, writebuf);
	  reg++;
	}
      /* Now transfer the remaining register values.  */
      for (b = part; b < TYPE_LENGTH (type); b += 4)
	{
	  if (readbuf != NULL)
	    regcache_cooked_read (regcache, reg, (char *) readbuf + b);
	  if (writebuf != NULL)
	    regcache_cooked_write (regcache, reg, (const char *) writebuf + b);
	  reg++;
	}
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
  else
    return RETURN_VALUE_STRUCT_CONVENTION;
}

static enum return_value_convention
hppa64_return_value (struct gdbarch *gdbarch,
		     struct type *type, struct regcache *regcache,
		     void *readbuf, const void *writebuf)
{
  /* RM: Floats are returned in FR4R, doubles in FR4.  Integral values
     are in r28, padded on the left.  Aggregates less that 65 bits are
     in r28, right padded.  Aggregates upto 128 bits are in r28 and
     r29, right padded.  */ 
  if (TYPE_CODE (type) == TYPE_CODE_FLT
      && TYPE_LENGTH (type) <= 8)
    {
      /* Floats are right aligned?  */
      int offset = register_size (gdbarch, FP4_REGNUM) - TYPE_LENGTH (type);
      if (readbuf != NULL)
	regcache_cooked_read_part (regcache, FP4_REGNUM, offset,
				   TYPE_LENGTH (type), readbuf);
      if (writebuf != NULL)
	regcache_cooked_write_part (regcache, FP4_REGNUM, offset,
				    TYPE_LENGTH (type), writebuf);
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
  else if (TYPE_LENGTH (type) <= 8 && is_integral_type (type))
    {
      /* Integrals are right aligned.  */
      int offset = register_size (gdbarch, FP4_REGNUM) - TYPE_LENGTH (type);
      if (readbuf != NULL)
	regcache_cooked_read_part (regcache, 28, offset,
				   TYPE_LENGTH (type), readbuf);
      if (writebuf != NULL)
	regcache_cooked_write_part (regcache, 28, offset,
				    TYPE_LENGTH (type), writebuf);
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
  else if (TYPE_LENGTH (type) <= 2 * 8)
    {
      /* Composite values are left aligned.  */
      int b;
      for (b = 0; b < TYPE_LENGTH (type); b += 8)
	{
	  int part = min (8, TYPE_LENGTH (type) - b);
	  if (readbuf != NULL)
	    regcache_cooked_read_part (regcache, 28 + b / 8, 0, part,
				       (char *) readbuf + b);
	  if (writebuf != NULL)
	    regcache_cooked_write_part (regcache, 28 + b / 8, 0, part,
					(const char *) writebuf + b);
	}
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
  else
    return RETURN_VALUE_STRUCT_CONVENTION;
}

/* Routines to extract various sized constants out of hppa 
   instructions. */

/* This assumes that no garbage lies outside of the lower bits of 
   value. */

static int
sign_extend (unsigned val, unsigned bits)
{
  return (int) (val >> (bits - 1) ? (-1 << bits) | val : val);
}

/* For many immediate values the sign bit is the low bit! */

static int
low_sign_extend (unsigned val, unsigned bits)
{
  return (int) ((val & 0x1 ? (-1 << (bits - 1)) : 0) | val >> 1);
}

/* Extract the bits at positions between FROM and TO, using HP's numbering
   (MSB = 0). */

static int
get_field (unsigned word, int from, int to)
{
  return ((word) >> (31 - (to)) & ((1 << ((to) - (from) + 1)) - 1));
}

/* extract the immediate field from a ld{bhw}s instruction */

static int
extract_5_load (unsigned word)
{
  return low_sign_extend (word >> 16 & MASK_5, 5);
}

/* extract the immediate field from a break instruction */

static unsigned
extract_5r_store (unsigned word)
{
  return (word & MASK_5);
}

/* extract the immediate field from a {sr}sm instruction */

static unsigned
extract_5R_store (unsigned word)
{
  return (word >> 16 & MASK_5);
}

/* extract a 14 bit immediate field */

static int
extract_14 (unsigned word)
{
  return low_sign_extend (word & MASK_14, 14);
}

/* extract a 21 bit constant */

static int
extract_21 (unsigned word)
{
  int val;

  word &= MASK_21;
  word <<= 11;
  val = get_field (word, 20, 20);
  val <<= 11;
  val |= get_field (word, 9, 19);
  val <<= 2;
  val |= get_field (word, 5, 6);
  val <<= 5;
  val |= get_field (word, 0, 4);
  val <<= 2;
  val |= get_field (word, 7, 8);
  return sign_extend (val, 21) << 11;
}

/* extract a 17 bit constant from branch instructions, returning the
   19 bit signed value. */

static int
extract_17 (unsigned word)
{
  return sign_extend (get_field (word, 19, 28) |
		      get_field (word, 29, 29) << 10 |
		      get_field (word, 11, 15) << 11 |
		      (word & 0x1) << 16, 17) << 2;
}


/* Compare the start address for two unwind entries returning 1 if 
   the first address is larger than the second, -1 if the second is
   larger than the first, and zero if they are equal.  */

static int
compare_unwind_entries (const void *arg1, const void *arg2)
{
  const struct unwind_table_entry *a = arg1;
  const struct unwind_table_entry *b = arg2;

  if (a->region_start > b->region_start)
    return 1;
  else if (a->region_start < b->region_start)
    return -1;
  else
    return 0;
}

static CORE_ADDR low_text_segment_address;

static void
record_text_segment_lowaddr (bfd *abfd, asection *section, void *ignored)
{
  if (((section->flags & (SEC_ALLOC | SEC_LOAD | SEC_READONLY))
       == (SEC_ALLOC | SEC_LOAD | SEC_READONLY))
      && section->vma < low_text_segment_address)
    low_text_segment_address = section->vma;
}

static void
internalize_unwinds (struct objfile *objfile, struct unwind_table_entry *table,
		     asection *section, unsigned int entries, unsigned int size,
		     CORE_ADDR text_offset)
{
  /* We will read the unwind entries into temporary memory, then
     fill in the actual unwind table.  */
  if (size > 0)
    {
      unsigned long tmp;
      unsigned i;
      char *buf = alloca (size);

      low_text_segment_address = -1;

      /* If addresses are 64 bits wide, then unwinds are supposed to
	 be segment relative offsets instead of absolute addresses. 

	 Note that when loading a shared library (text_offset != 0) the
	 unwinds are already relative to the text_offset that will be
	 passed in.  */
      if (TARGET_PTR_BIT == 64 && text_offset == 0)
	{
	  bfd_map_over_sections (objfile->obfd,
				 record_text_segment_lowaddr, NULL);

	  /* ?!? Mask off some low bits.  Should this instead subtract
	     out the lowest section's filepos or something like that?
	     This looks very hokey to me.  */
	  low_text_segment_address &= ~0xfff;
	  text_offset += low_text_segment_address;
	}

      bfd_get_section_contents (objfile->obfd, section, buf, 0, size);

      /* Now internalize the information being careful to handle host/target
         endian issues.  */
      for (i = 0; i < entries; i++)
	{
	  table[i].region_start = bfd_get_32 (objfile->obfd,
					      (bfd_byte *) buf);
	  table[i].region_start += text_offset;
	  buf += 4;
	  table[i].region_end = bfd_get_32 (objfile->obfd, (bfd_byte *) buf);
	  table[i].region_end += text_offset;
	  buf += 4;
	  tmp = bfd_get_32 (objfile->obfd, (bfd_byte *) buf);
	  buf += 4;
	  table[i].Cannot_unwind = (tmp >> 31) & 0x1;
	  table[i].Millicode = (tmp >> 30) & 0x1;
	  table[i].Millicode_save_sr0 = (tmp >> 29) & 0x1;
	  table[i].Region_description = (tmp >> 27) & 0x3;
	  table[i].reserved1 = (tmp >> 26) & 0x1;
	  table[i].Entry_SR = (tmp >> 25) & 0x1;
	  table[i].Entry_FR = (tmp >> 21) & 0xf;
	  table[i].Entry_GR = (tmp >> 16) & 0x1f;
	  table[i].Args_stored = (tmp >> 15) & 0x1;
	  table[i].Variable_Frame = (tmp >> 14) & 0x1;
	  table[i].Separate_Package_Body = (tmp >> 13) & 0x1;
	  table[i].Frame_Extension_Millicode = (tmp >> 12) & 0x1;
	  table[i].Stack_Overflow_Check = (tmp >> 11) & 0x1;
	  table[i].Two_Instruction_SP_Increment = (tmp >> 10) & 0x1;
	  table[i].Ada_Region = (tmp >> 9) & 0x1;
	  table[i].cxx_info = (tmp >> 8) & 0x1;
	  table[i].cxx_try_catch = (tmp >> 7) & 0x1;
	  table[i].sched_entry_seq = (tmp >> 6) & 0x1;
	  table[i].reserved2 = (tmp >> 5) & 0x1;
	  table[i].Save_SP = (tmp >> 4) & 0x1;
	  table[i].Save_RP = (tmp >> 3) & 0x1;
	  table[i].Save_MRP_in_frame = (tmp >> 2) & 0x1;
	  table[i].extn_ptr_defined = (tmp >> 1) & 0x1;
	  table[i].Cleanup_defined = tmp & 0x1;
	  tmp = bfd_get_32 (objfile->obfd, (bfd_byte *) buf);
	  buf += 4;
	  table[i].MPE_XL_interrupt_marker = (tmp >> 31) & 0x1;
	  table[i].HP_UX_interrupt_marker = (tmp >> 30) & 0x1;
	  table[i].Large_frame = (tmp >> 29) & 0x1;
	  table[i].Pseudo_SP_Set = (tmp >> 28) & 0x1;
	  table[i].reserved4 = (tmp >> 27) & 0x1;
	  table[i].Total_frame_size = tmp & 0x7ffffff;

	  /* Stub unwinds are handled elsewhere. */
	  table[i].stub_unwind.stub_type = 0;
	  table[i].stub_unwind.padding = 0;
	}
    }
}

/* Read in the backtrace information stored in the `$UNWIND_START$' section of
   the object file.  This info is used mainly by find_unwind_entry() to find
   out the stack frame size and frame pointer used by procedures.  We put
   everything on the psymbol obstack in the objfile so that it automatically
   gets freed when the objfile is destroyed.  */

static void
read_unwind_info (struct objfile *objfile)
{
  asection *unwind_sec, *stub_unwind_sec;
  unsigned unwind_size, stub_unwind_size, total_size;
  unsigned index, unwind_entries;
  unsigned stub_entries, total_entries;
  CORE_ADDR text_offset;
  struct obj_unwind_info *ui;
  obj_private_data_t *obj_private;

  text_offset = ANOFFSET (objfile->section_offsets, 0);
  ui = (struct obj_unwind_info *) obstack_alloc (&objfile->objfile_obstack,
					   sizeof (struct obj_unwind_info));

  ui->table = NULL;
  ui->cache = NULL;
  ui->last = -1;

  /* For reasons unknown the HP PA64 tools generate multiple unwinder
     sections in a single executable.  So we just iterate over every
     section in the BFD looking for unwinder sections intead of trying
     to do a lookup with bfd_get_section_by_name. 

     First determine the total size of the unwind tables so that we
     can allocate memory in a nice big hunk.  */
  total_entries = 0;
  for (unwind_sec = objfile->obfd->sections;
       unwind_sec;
       unwind_sec = unwind_sec->next)
    {
      if (strcmp (unwind_sec->name, "$UNWIND_START$") == 0
	  || strcmp (unwind_sec->name, ".PARISC.unwind") == 0)
	{
	  unwind_size = bfd_section_size (objfile->obfd, unwind_sec);
	  unwind_entries = unwind_size / UNWIND_ENTRY_SIZE;

	  total_entries += unwind_entries;
	}
    }

  /* Now compute the size of the stub unwinds.  Note the ELF tools do not
     use stub unwinds at the curren time.  */
  stub_unwind_sec = bfd_get_section_by_name (objfile->obfd, "$UNWIND_END$");

  if (stub_unwind_sec)
    {
      stub_unwind_size = bfd_section_size (objfile->obfd, stub_unwind_sec);
      stub_entries = stub_unwind_size / STUB_UNWIND_ENTRY_SIZE;
    }
  else
    {
      stub_unwind_size = 0;
      stub_entries = 0;
    }

  /* Compute total number of unwind entries and their total size.  */
  total_entries += stub_entries;
  total_size = total_entries * sizeof (struct unwind_table_entry);

  /* Allocate memory for the unwind table.  */
  ui->table = (struct unwind_table_entry *)
    obstack_alloc (&objfile->objfile_obstack, total_size);
  ui->last = total_entries - 1;

  /* Now read in each unwind section and internalize the standard unwind
     entries.  */
  index = 0;
  for (unwind_sec = objfile->obfd->sections;
       unwind_sec;
       unwind_sec = unwind_sec->next)
    {
      if (strcmp (unwind_sec->name, "$UNWIND_START$") == 0
	  || strcmp (unwind_sec->name, ".PARISC.unwind") == 0)
	{
	  unwind_size = bfd_section_size (objfile->obfd, unwind_sec);
	  unwind_entries = unwind_size / UNWIND_ENTRY_SIZE;

	  internalize_unwinds (objfile, &ui->table[index], unwind_sec,
			       unwind_entries, unwind_size, text_offset);
	  index += unwind_entries;
	}
    }

  /* Now read in and internalize the stub unwind entries.  */
  if (stub_unwind_size > 0)
    {
      unsigned int i;
      char *buf = alloca (stub_unwind_size);

      /* Read in the stub unwind entries.  */
      bfd_get_section_contents (objfile->obfd, stub_unwind_sec, buf,
				0, stub_unwind_size);

      /* Now convert them into regular unwind entries.  */
      for (i = 0; i < stub_entries; i++, index++)
	{
	  /* Clear out the next unwind entry.  */
	  memset (&ui->table[index], 0, sizeof (struct unwind_table_entry));

	  /* Convert offset & size into region_start and region_end.  
	     Stuff away the stub type into "reserved" fields.  */
	  ui->table[index].region_start = bfd_get_32 (objfile->obfd,
						      (bfd_byte *) buf);
	  ui->table[index].region_start += text_offset;
	  buf += 4;
	  ui->table[index].stub_unwind.stub_type = bfd_get_8 (objfile->obfd,
							  (bfd_byte *) buf);
	  buf += 2;
	  ui->table[index].region_end
	    = ui->table[index].region_start + 4 *
	    (bfd_get_16 (objfile->obfd, (bfd_byte *) buf) - 1);
	  buf += 2;
	}

    }

  /* Unwind table needs to be kept sorted.  */
  qsort (ui->table, total_entries, sizeof (struct unwind_table_entry),
	 compare_unwind_entries);

  /* Keep a pointer to the unwind information.  */
  if (objfile->obj_private == NULL)
    {
      obj_private = (obj_private_data_t *)
	obstack_alloc (&objfile->objfile_obstack,
		       sizeof (obj_private_data_t));
      obj_private->unwind_info = NULL;
      obj_private->so_info = NULL;
      obj_private->dp = 0;

      objfile->obj_private = obj_private;
    }
  obj_private = (obj_private_data_t *) objfile->obj_private;
  obj_private->unwind_info = ui;
}

/* Lookup the unwind (stack backtrace) info for the given PC.  We search all
   of the objfiles seeking the unwind table entry for this PC.  Each objfile
   contains a sorted list of struct unwind_table_entry.  Since we do a binary
   search of the unwind tables, we depend upon them to be sorted.  */

struct unwind_table_entry *
find_unwind_entry (CORE_ADDR pc)
{
  int first, middle, last;
  struct objfile *objfile;

  /* A function at address 0?  Not in HP-UX! */
  if (pc == (CORE_ADDR) 0)
    return NULL;

  ALL_OBJFILES (objfile)
  {
    struct obj_unwind_info *ui;
    ui = NULL;
    if (objfile->obj_private)
      ui = ((obj_private_data_t *) (objfile->obj_private))->unwind_info;

    if (!ui)
      {
	read_unwind_info (objfile);
	if (objfile->obj_private == NULL)
	  error ("Internal error reading unwind information.");
	ui = ((obj_private_data_t *) (objfile->obj_private))->unwind_info;
      }

    /* First, check the cache */

    if (ui->cache
	&& pc >= ui->cache->region_start
	&& pc <= ui->cache->region_end)
      return ui->cache;

    /* Not in the cache, do a binary search */

    first = 0;
    last = ui->last;

    while (first <= last)
      {
	middle = (first + last) / 2;
	if (pc >= ui->table[middle].region_start
	    && pc <= ui->table[middle].region_end)
	  {
	    ui->cache = &ui->table[middle];
	    return &ui->table[middle];
	  }

	if (pc < ui->table[middle].region_start)
	  last = middle - 1;
	else
	  first = middle + 1;
      }
  }				/* ALL_OBJFILES() */
  return NULL;
}

const unsigned char *
hppa_breakpoint_from_pc (CORE_ADDR *pc, int *len)
{
  static const unsigned char breakpoint[] = {0x00, 0x01, 0x00, 0x04};
  (*len) = sizeof (breakpoint);
  return breakpoint;
}

/* Return the name of a register.  */

const char *
hppa32_register_name (int i)
{
  static char *names[] = {
    "flags",  "r1",      "rp",     "r3",
    "r4",     "r5",      "r6",     "r7",
    "r8",     "r9",      "r10",    "r11",
    "r12",    "r13",     "r14",    "r15",
    "r16",    "r17",     "r18",    "r19",
    "r20",    "r21",     "r22",    "r23",
    "r24",    "r25",     "r26",    "dp",
    "ret0",   "ret1",    "sp",     "r31",
    "sar",    "pcoqh",   "pcsqh",  "pcoqt",
    "pcsqt",  "eiem",    "iir",    "isr",
    "ior",    "ipsw",    "goto",   "sr4",
    "sr0",    "sr1",     "sr2",    "sr3",
    "sr5",    "sr6",     "sr7",    "cr0",
    "cr8",    "cr9",     "ccr",    "cr12",
    "cr13",   "cr24",    "cr25",   "cr26",
    "mpsfu_high","mpsfu_low","mpsfu_ovflo","pad",
    "fpsr",    "fpe1",   "fpe2",   "fpe3",
    "fpe4",   "fpe5",    "fpe6",   "fpe7",
    "fr4",     "fr4R",   "fr5",    "fr5R",
    "fr6",    "fr6R",    "fr7",    "fr7R",
    "fr8",     "fr8R",   "fr9",    "fr9R",
    "fr10",   "fr10R",   "fr11",   "fr11R",
    "fr12",    "fr12R",  "fr13",   "fr13R",
    "fr14",   "fr14R",   "fr15",   "fr15R",
    "fr16",    "fr16R",  "fr17",   "fr17R",
    "fr18",   "fr18R",   "fr19",   "fr19R",
    "fr20",    "fr20R",  "fr21",   "fr21R",
    "fr22",   "fr22R",   "fr23",   "fr23R",
    "fr24",    "fr24R",  "fr25",   "fr25R",
    "fr26",   "fr26R",   "fr27",   "fr27R",
    "fr28",    "fr28R",  "fr29",   "fr29R",
    "fr30",   "fr30R",   "fr31",   "fr31R"
  };
  if (i < 0 || i >= (sizeof (names) / sizeof (*names)))
    return NULL;
  else
    return names[i];
}

const char *
hppa64_register_name (int i)
{
  static char *names[] = {
    "flags",  "r1",      "rp",     "r3",
    "r4",     "r5",      "r6",     "r7",
    "r8",     "r9",      "r10",    "r11",
    "r12",    "r13",     "r14",    "r15",
    "r16",    "r17",     "r18",    "r19",
    "r20",    "r21",     "r22",    "r23",
    "r24",    "r25",     "r26",    "dp",
    "ret0",   "ret1",    "sp",     "r31",
    "sar",    "pcoqh",   "pcsqh",  "pcoqt",
    "pcsqt",  "eiem",    "iir",    "isr",
    "ior",    "ipsw",    "goto",   "sr4",
    "sr0",    "sr1",     "sr2",    "sr3",
    "sr5",    "sr6",     "sr7",    "cr0",
    "cr8",    "cr9",     "ccr",    "cr12",
    "cr13",   "cr24",    "cr25",   "cr26",
    "mpsfu_high","mpsfu_low","mpsfu_ovflo","pad",
    "fpsr",    "fpe1",   "fpe2",   "fpe3",
    "fr4",    "fr5",     "fr6",    "fr7",
    "fr8",     "fr9",    "fr10",   "fr11",
    "fr12",   "fr13",    "fr14",   "fr15",
    "fr16",    "fr17",   "fr18",   "fr19",
    "fr20",   "fr21",    "fr22",   "fr23",
    "fr24",    "fr25",   "fr26",   "fr27",
    "fr28",  "fr29",    "fr30",   "fr31"
  };
  if (i < 0 || i >= (sizeof (names) / sizeof (*names)))
    return NULL;
  else
    return names[i];
}



/* Return the adjustment necessary to make for addresses on the stack
   as presented by hpread.c.

   This is necessary because of the stack direction on the PA and the
   bizarre way in which someone (?) decided they wanted to handle
   frame pointerless code in GDB.  */
int
hpread_adjust_stack_address (CORE_ADDR func_addr)
{
  struct unwind_table_entry *u;

  u = find_unwind_entry (func_addr);
  if (!u)
    return 0;
  else
    return u->Total_frame_size << 3;
}

/* This function pushes a stack frame with arguments as part of the
   inferior function calling mechanism.

   This is the version of the function for the 32-bit PA machines, in
   which later arguments appear at lower addresses.  (The stack always
   grows towards higher addresses.)

   We simply allocate the appropriate amount of stack space and put
   arguments into their proper slots.  */
   
CORE_ADDR
hppa32_push_dummy_call (struct gdbarch *gdbarch, CORE_ADDR func_addr,
			struct regcache *regcache, CORE_ADDR bp_addr,
			int nargs, struct value **args, CORE_ADDR sp,
			int struct_return, CORE_ADDR struct_addr)
{
  /* NOTE: cagney/2004-02-27: This is a guess - its implemented by
     reverse engineering testsuite failures.  */

  /* Stack base address at which any pass-by-reference parameters are
     stored.  */
  CORE_ADDR struct_end = 0;
  /* Stack base address at which the first parameter is stored.  */
  CORE_ADDR param_end = 0;

  /* The inner most end of the stack after all the parameters have
     been pushed.  */
  CORE_ADDR new_sp = 0;

  /* Two passes.  First pass computes the location of everything,
     second pass writes the bytes out.  */
  int write_pass;
  for (write_pass = 0; write_pass < 2; write_pass++)
    {
      CORE_ADDR struct_ptr = 0;
      CORE_ADDR param_ptr = 0;
      int reg = 27;	      /* NOTE: Registers go down.  */
      int i;
      for (i = 0; i < nargs; i++)
	{
	  struct value *arg = args[i];
	  struct type *type = check_typedef (VALUE_TYPE (arg));
	  /* The corresponding parameter that is pushed onto the
	     stack, and [possibly] passed in a register.  */
	  char param_val[8];
	  int param_len;
	  memset (param_val, 0, sizeof param_val);
	  if (TYPE_LENGTH (type) > 8)
	    {
	      /* Large parameter, pass by reference.  Store the value
		 in "struct" area and then pass its address.  */
	      param_len = 4;
	      struct_ptr += align_up (TYPE_LENGTH (type), 8);
	      if (write_pass)
		write_memory (struct_end - struct_ptr, VALUE_CONTENTS (arg),
			      TYPE_LENGTH (type));
	      store_unsigned_integer (param_val, 4, struct_end - struct_ptr);
	    }
	  else if (TYPE_CODE (type) == TYPE_CODE_INT
		   || TYPE_CODE (type) == TYPE_CODE_ENUM)
	    {
	      /* Integer value store, right aligned.  "unpack_long"
		 takes care of any sign-extension problems.  */
	      param_len = align_up (TYPE_LENGTH (type), 4);
	      store_unsigned_integer (param_val, param_len,
				      unpack_long (type,
						   VALUE_CONTENTS (arg)));
	    }
	  else
	    {
	      /* Small struct value, store right aligned?  */
	      param_len = align_up (TYPE_LENGTH (type), 4);
	      memcpy (param_val + param_len - TYPE_LENGTH (type),
		      VALUE_CONTENTS (arg), TYPE_LENGTH (type));
	    }
	  param_ptr += param_len;
	  reg -= param_len / 4;
	  if (write_pass)
	    {
	      write_memory (param_end - param_ptr, param_val, param_len);
	      if (reg >= 23)
		{
		  regcache_cooked_write (regcache, reg, param_val);
		  if (param_len > 4)
		    regcache_cooked_write (regcache, reg + 1, param_val + 4);
		}
	    }
	}

      /* Update the various stack pointers.  */
      if (!write_pass)
	{
	  struct_end = sp + struct_ptr;
	  /* PARAM_PTR already accounts for all the arguments passed
	     by the user.  However, the ABI mandates minimum stack
	     space allocations for outgoing arguments.  The ABI also
	     mandates minimum stack alignments which we must
	     preserve.  */
	  param_end = struct_end + max (align_up (param_ptr, 8), 16);
	}
    }

  /* If a structure has to be returned, set up register 28 to hold its
     address */
  if (struct_return)
    write_register (28, struct_addr);

  /* Set the return address.  */
  regcache_cooked_write_unsigned (regcache, RP_REGNUM, bp_addr);

  /* Update the Stack Pointer.  */
  regcache_cooked_write_unsigned (regcache, SP_REGNUM, param_end + 32);

  /* The stack will have 32 bytes of additional space for a frame marker.  */
  return param_end + 32;
}

/* This function pushes a stack frame with arguments as part of the
   inferior function calling mechanism.

   This is the version for the PA64, in which later arguments appear
   at higher addresses.  (The stack always grows towards higher
   addresses.)

   We simply allocate the appropriate amount of stack space and put
   arguments into their proper slots.

   This ABI also requires that the caller provide an argument pointer
   to the callee, so we do that too.  */
   
CORE_ADDR
hppa64_push_dummy_call (struct gdbarch *gdbarch, CORE_ADDR func_addr,
			struct regcache *regcache, CORE_ADDR bp_addr,
			int nargs, struct value **args, CORE_ADDR sp,
			int struct_return, CORE_ADDR struct_addr)
{
  /* NOTE: cagney/2004-02-27: This is a guess - its implemented by
     reverse engineering testsuite failures.  */

  /* Stack base address at which any pass-by-reference parameters are
     stored.  */
  CORE_ADDR struct_end = 0;
  /* Stack base address at which the first parameter is stored.  */
  CORE_ADDR param_end = 0;

  /* The inner most end of the stack after all the parameters have
     been pushed.  */
  CORE_ADDR new_sp = 0;

  /* Two passes.  First pass computes the location of everything,
     second pass writes the bytes out.  */
  int write_pass;
  for (write_pass = 0; write_pass < 2; write_pass++)
    {
      CORE_ADDR struct_ptr = 0;
      CORE_ADDR param_ptr = 0;
      int i;
      for (i = 0; i < nargs; i++)
	{
	  struct value *arg = args[i];
	  struct type *type = check_typedef (VALUE_TYPE (arg));
	  if ((TYPE_CODE (type) == TYPE_CODE_INT
	       || TYPE_CODE (type) == TYPE_CODE_ENUM)
	      && TYPE_LENGTH (type) <= 8)
	    {
	      /* Integer value store, right aligned.  "unpack_long"
		 takes care of any sign-extension problems.  */
	      param_ptr += 8;
	      if (write_pass)
		{
		  ULONGEST val = unpack_long (type, VALUE_CONTENTS (arg));
		  int reg = 27 - param_ptr / 8;
		  write_memory_unsigned_integer (param_end - param_ptr,
						 val, 8);
		  if (reg >= 19)
		    regcache_cooked_write_unsigned (regcache, reg, val);
		}
	    }
	  else
	    {
	      /* Small struct value, store left aligned?  */
	      int reg;
	      if (TYPE_LENGTH (type) > 8)
		{
		  param_ptr = align_up (param_ptr, 16);
		  reg = 26 - param_ptr / 8;
		  param_ptr += align_up (TYPE_LENGTH (type), 16);
		}
	      else
		{
		  param_ptr = align_up (param_ptr, 8);
		  reg = 26 - param_ptr / 8;
		  param_ptr += align_up (TYPE_LENGTH (type), 8);
		}
	      if (write_pass)
		{
		  int byte;
		  write_memory (param_end - param_ptr, VALUE_CONTENTS (arg),
				TYPE_LENGTH (type));
		  for (byte = 0; byte < TYPE_LENGTH (type); byte += 8)
		    {
		      if (reg >= 19)
			{
			  int len = min (8, TYPE_LENGTH (type) - byte);
			  regcache_cooked_write_part (regcache, reg, 0, len,
						      VALUE_CONTENTS (arg) + byte);
			}
		      reg--;
		    }
		}
	    }
	}
      /* Update the various stack pointers.  */
      if (!write_pass)
	{
	  struct_end = sp + struct_ptr;
	  /* PARAM_PTR already accounts for all the arguments passed
	     by the user.  However, the ABI mandates minimum stack
	     space allocations for outgoing arguments.  The ABI also
	     mandates minimum stack alignments which we must
	     preserve.  */
	  param_end = struct_end + max (align_up (param_ptr, 16), 64);
	}
    }

  /* If a structure has to be returned, set up register 28 to hold its
     address */
  if (struct_return)
    write_register (28, struct_addr);

  /* Set the return address.  */
  regcache_cooked_write_unsigned (regcache, RP_REGNUM, bp_addr);

  /* Update the Stack Pointer.  */
  regcache_cooked_write_unsigned (regcache, SP_REGNUM, param_end + 64);

  /* The stack will have 32 bytes of additional space for a frame marker.  */
  return param_end + 64;
}

static CORE_ADDR
hppa32_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  /* HP frames are 64-byte (or cache line) aligned (yes that's _byte_
     and not _bit_)!  */
  return align_up (addr, 64);
}

/* Force all frames to 16-byte alignment.  Better safe than sorry.  */

static CORE_ADDR
hppa64_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  /* Just always 16-byte align.  */
  return align_up (addr, 16);
}


/* elz: Used to lookup a symbol in the shared libraries.
   This function calls shl_findsym, indirectly through a
   call to __d_shl_get. __d_shl_get is in end.c, which is always
   linked in by the hp compilers/linkers. 
   The call to shl_findsym cannot be made directly because it needs
   to be active in target address space. 
   inputs: - minimal symbol pointer for the function we want to look up
   - address in target space of the descriptor for the library
   where we want to look the symbol up.
   This address is retrieved using the 
   som_solib_get_solib_by_pc function (somsolib.c). 
   output: - real address in the library of the function.          
   note: the handle can be null, in which case shl_findsym will look for
   the symbol in all the loaded shared libraries.
   files to look at if you need reference on this stuff:
   dld.c, dld_shl_findsym.c
   end.c
   man entry for shl_findsym */

CORE_ADDR
find_stub_with_shl_get (struct minimal_symbol *function, CORE_ADDR handle)
{
  struct symbol *get_sym, *symbol2;
  struct minimal_symbol *buff_minsym, *msymbol;
  struct type *ftype;
  struct value **args;
  struct value *funcval;
  struct value *val;

  int x, namelen, err_value, tmp = -1;
  CORE_ADDR endo_buff_addr, value_return_addr, errno_return_addr;
  CORE_ADDR stub_addr;


  args = alloca (sizeof (struct value *) * 8);		/* 6 for the arguments and one null one??? */
  funcval = find_function_in_inferior ("__d_shl_get");
  get_sym = lookup_symbol ("__d_shl_get", NULL, VAR_DOMAIN, NULL, NULL);
  buff_minsym = lookup_minimal_symbol ("__buffer", NULL, NULL);
  msymbol = lookup_minimal_symbol ("__shldp", NULL, NULL);
  symbol2 = lookup_symbol ("__shldp", NULL, VAR_DOMAIN, NULL, NULL);
  endo_buff_addr = SYMBOL_VALUE_ADDRESS (buff_minsym);
  namelen = strlen (DEPRECATED_SYMBOL_NAME (function));
  value_return_addr = endo_buff_addr + namelen;
  ftype = check_typedef (SYMBOL_TYPE (get_sym));

  /* do alignment */
  if ((x = value_return_addr % 64) != 0)
    value_return_addr = value_return_addr + 64 - x;

  errno_return_addr = value_return_addr + 64;


  /* set up stuff needed by __d_shl_get in buffer in end.o */

  target_write_memory (endo_buff_addr, DEPRECATED_SYMBOL_NAME (function), namelen);

  target_write_memory (value_return_addr, (char *) &tmp, 4);

  target_write_memory (errno_return_addr, (char *) &tmp, 4);

  target_write_memory (SYMBOL_VALUE_ADDRESS (msymbol),
		       (char *) &handle, 4);

  /* now prepare the arguments for the call */

  args[0] = value_from_longest (TYPE_FIELD_TYPE (ftype, 0), 12);
  args[1] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 1), SYMBOL_VALUE_ADDRESS (msymbol));
  args[2] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 2), endo_buff_addr);
  args[3] = value_from_longest (TYPE_FIELD_TYPE (ftype, 3), TYPE_PROCEDURE);
  args[4] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 4), value_return_addr);
  args[5] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 5), errno_return_addr);

  /* now call the function */

  val = call_function_by_hand (funcval, 6, args);

  /* now get the results */

  target_read_memory (errno_return_addr, (char *) &err_value, sizeof (err_value));

  target_read_memory (value_return_addr, (char *) &stub_addr, sizeof (stub_addr));
  if (stub_addr <= 0)
    error ("call to __d_shl_get failed, error code is %d", err_value);

  return (stub_addr);
}

/* Cover routine for find_stub_with_shl_get to pass to catch_errors */
static int
cover_find_stub_with_shl_get (void *args_untyped)
{
  args_for_find_stub *args = args_untyped;
  args->return_val = find_stub_with_shl_get (args->msym, args->solib_handle);
  return 0;
}

/* Get the PC from %r31 if currently in a syscall.  Also mask out privilege
   bits.  */

CORE_ADDR
hppa_target_read_pc (ptid_t ptid)
{
  int flags = read_register_pid (FLAGS_REGNUM, ptid);

  /* The following test does not belong here.  It is OS-specific, and belongs
     in native code.  */
  /* Test SS_INSYSCALL */
  if (flags & 2)
    return read_register_pid (31, ptid) & ~0x3;

  return read_register_pid (PCOQ_HEAD_REGNUM, ptid) & ~0x3;
}

/* Write out the PC.  If currently in a syscall, then also write the new
   PC value into %r31.  */

void
hppa_target_write_pc (CORE_ADDR v, ptid_t ptid)
{
  int flags = read_register_pid (FLAGS_REGNUM, ptid);

  /* The following test does not belong here.  It is OS-specific, and belongs
     in native code.  */
  /* If in a syscall, then set %r31.  Also make sure to get the 
     privilege bits set correctly.  */
  /* Test SS_INSYSCALL */
  if (flags & 2)
    write_register_pid (31, v | 0x3, ptid);

  write_register_pid (PCOQ_HEAD_REGNUM, v, ptid);
  write_register_pid (PCOQ_TAIL_REGNUM, v + 4, ptid);
}

/* return the alignment of a type in bytes. Structures have the maximum
   alignment required by their fields. */

static int
hppa_alignof (struct type *type)
{
  int max_align, align, i;
  CHECK_TYPEDEF (type);
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_PTR:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
      return TYPE_LENGTH (type);
    case TYPE_CODE_ARRAY:
      return hppa_alignof (TYPE_FIELD_TYPE (type, 0));
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      max_align = 1;
      for (i = 0; i < TYPE_NFIELDS (type); i++)
	{
	  /* Bit fields have no real alignment. */
	  /* if (!TYPE_FIELD_BITPOS (type, i)) */
	  if (!TYPE_FIELD_BITSIZE (type, i))	/* elz: this should be bitsize */
	    {
	      align = hppa_alignof (TYPE_FIELD_TYPE (type, i));
	      max_align = max (max_align, align);
	    }
	}
      return max_align;
    default:
      return 4;
    }
}

/* Return one if PC is in the call path of a trampoline, else return zero.

   Note we return one for *any* call trampoline (long-call, arg-reloc), not
   just shared library trampolines (import, export).  */

int
hppa_in_solib_call_trampoline (CORE_ADDR pc, char *name)
{
  struct minimal_symbol *minsym;
  struct unwind_table_entry *u;
  static CORE_ADDR dyncall = 0;
  static CORE_ADDR sr4export = 0;

#ifdef GDB_TARGET_IS_HPPA_20W
  /* PA64 has a completely different stub/trampoline scheme.  Is it
     better?  Maybe.  It's certainly harder to determine with any
     certainty that we are in a stub because we can not refer to the
     unwinders to help. 

     The heuristic is simple.  Try to lookup the current PC value in th
     minimal symbol table.  If that fails, then assume we are not in a
     stub and return.

     Then see if the PC value falls within the section bounds for the
     section containing the minimal symbol we found in the first
     step.  If it does, then assume we are not in a stub and return.

     Finally peek at the instructions to see if they look like a stub.  */
  {
    struct minimal_symbol *minsym;
    asection *sec;
    CORE_ADDR addr;
    int insn, i;

    minsym = lookup_minimal_symbol_by_pc (pc);
    if (! minsym)
      return 0;

    sec = SYMBOL_BFD_SECTION (minsym);

    if (bfd_get_section_vma (sec->owner, sec) <= pc
	&& pc < (bfd_get_section_vma (sec->owner, sec)
		 + bfd_section_size (sec->owner, sec)))
      return 0;

    /* We might be in a stub.  Peek at the instructions.  Stubs are 3
       instructions long. */
    insn = read_memory_integer (pc, 4);

    /* Find out where we think we are within the stub.  */
    if ((insn & 0xffffc00e) == 0x53610000)
      addr = pc;
    else if ((insn & 0xffffffff) == 0xe820d000)
      addr = pc - 4;
    else if ((insn & 0xffffc00e) == 0x537b0000)
      addr = pc - 8;
    else
      return 0;

    /* Now verify each insn in the range looks like a stub instruction.  */
    insn = read_memory_integer (addr, 4);
    if ((insn & 0xffffc00e) != 0x53610000)
      return 0;
	
    /* Now verify each insn in the range looks like a stub instruction.  */
    insn = read_memory_integer (addr + 4, 4);
    if ((insn & 0xffffffff) != 0xe820d000)
      return 0;
    
    /* Now verify each insn in the range looks like a stub instruction.  */
    insn = read_memory_integer (addr + 8, 4);
    if ((insn & 0xffffc00e) != 0x537b0000)
      return 0;

    /* Looks like a stub.  */
    return 1;
  }
#endif

  /* FIXME XXX - dyncall and sr4export must be initialized whenever we get a
     new exec file */

  /* First see if PC is in one of the two C-library trampolines.  */
  if (!dyncall)
    {
      minsym = lookup_minimal_symbol ("$$dyncall", NULL, NULL);
      if (minsym)
	dyncall = SYMBOL_VALUE_ADDRESS (minsym);
      else
	dyncall = -1;
    }

  if (!sr4export)
    {
      minsym = lookup_minimal_symbol ("_sr4export", NULL, NULL);
      if (minsym)
	sr4export = SYMBOL_VALUE_ADDRESS (minsym);
      else
	sr4export = -1;
    }

  if (pc == dyncall || pc == sr4export)
    return 1;

  minsym = lookup_minimal_symbol_by_pc (pc);
  if (minsym && strcmp (DEPRECATED_SYMBOL_NAME (minsym), ".stub") == 0)
    return 1;

  /* Get the unwind descriptor corresponding to PC, return zero
     if no unwind was found.  */
  u = find_unwind_entry (pc);
  if (!u)
    return 0;

  /* If this isn't a linker stub, then return now.  */
  if (u->stub_unwind.stub_type == 0)
    return 0;

  /* By definition a long-branch stub is a call stub.  */
  if (u->stub_unwind.stub_type == LONG_BRANCH)
    return 1;

  /* The call and return path execute the same instructions within
     an IMPORT stub!  So an IMPORT stub is both a call and return
     trampoline.  */
  if (u->stub_unwind.stub_type == IMPORT)
    return 1;

  /* Parameter relocation stubs always have a call path and may have a
     return path.  */
  if (u->stub_unwind.stub_type == PARAMETER_RELOCATION
      || u->stub_unwind.stub_type == EXPORT)
    {
      CORE_ADDR addr;

      /* Search forward from the current PC until we hit a branch
         or the end of the stub.  */
      for (addr = pc; addr <= u->region_end; addr += 4)
	{
	  unsigned long insn;

	  insn = read_memory_integer (addr, 4);

	  /* Does it look like a bl?  If so then it's the call path, if
	     we find a bv or be first, then we're on the return path.  */
	  if ((insn & 0xfc00e000) == 0xe8000000)
	    return 1;
	  else if ((insn & 0xfc00e001) == 0xe800c000
		   || (insn & 0xfc000000) == 0xe0000000)
	    return 0;
	}

      /* Should never happen.  */
      warning ("Unable to find branch in parameter relocation stub.\n");
      return 0;
    }

  /* Unknown stub type.  For now, just return zero.  */
  return 0;
}

/* Return one if PC is in the return path of a trampoline, else return zero.

   Note we return one for *any* call trampoline (long-call, arg-reloc), not
   just shared library trampolines (import, export).  */

int
hppa_in_solib_return_trampoline (CORE_ADDR pc, char *name)
{
  struct unwind_table_entry *u;

  /* Get the unwind descriptor corresponding to PC, return zero
     if no unwind was found.  */
  u = find_unwind_entry (pc);
  if (!u)
    return 0;

  /* If this isn't a linker stub or it's just a long branch stub, then
     return zero.  */
  if (u->stub_unwind.stub_type == 0 || u->stub_unwind.stub_type == LONG_BRANCH)
    return 0;

  /* The call and return path execute the same instructions within
     an IMPORT stub!  So an IMPORT stub is both a call and return
     trampoline.  */
  if (u->stub_unwind.stub_type == IMPORT)
    return 1;

  /* Parameter relocation stubs always have a call path and may have a
     return path.  */
  if (u->stub_unwind.stub_type == PARAMETER_RELOCATION
      || u->stub_unwind.stub_type == EXPORT)
    {
      CORE_ADDR addr;

      /* Search forward from the current PC until we hit a branch
         or the end of the stub.  */
      for (addr = pc; addr <= u->region_end; addr += 4)
	{
	  unsigned long insn;

	  insn = read_memory_integer (addr, 4);

	  /* Does it look like a bl?  If so then it's the call path, if
	     we find a bv or be first, then we're on the return path.  */
	  if ((insn & 0xfc00e000) == 0xe8000000)
	    return 0;
	  else if ((insn & 0xfc00e001) == 0xe800c000
		   || (insn & 0xfc000000) == 0xe0000000)
	    return 1;
	}

      /* Should never happen.  */
      warning ("Unable to find branch in parameter relocation stub.\n");
      return 0;
    }

  /* Unknown stub type.  For now, just return zero.  */
  return 0;

}

/* Figure out if PC is in a trampoline, and if so find out where
   the trampoline will jump to.  If not in a trampoline, return zero.

   Simple code examination probably is not a good idea since the code
   sequences in trampolines can also appear in user code.

   We use unwinds and information from the minimal symbol table to
   determine when we're in a trampoline.  This won't work for ELF
   (yet) since it doesn't create stub unwind entries.  Whether or
   not ELF will create stub unwinds or normal unwinds for linker
   stubs is still being debated.

   This should handle simple calls through dyncall or sr4export,
   long calls, argument relocation stubs, and dyncall/sr4export
   calling an argument relocation stub.  It even handles some stubs
   used in dynamic executables.  */

CORE_ADDR
hppa_skip_trampoline_code (CORE_ADDR pc)
{
  long orig_pc = pc;
  long prev_inst, curr_inst, loc;
  static CORE_ADDR dyncall = 0;
  static CORE_ADDR dyncall_external = 0;
  static CORE_ADDR sr4export = 0;
  struct minimal_symbol *msym;
  struct unwind_table_entry *u;

  /* FIXME XXX - dyncall and sr4export must be initialized whenever we get a
     new exec file */

  if (!dyncall)
    {
      msym = lookup_minimal_symbol ("$$dyncall", NULL, NULL);
      if (msym)
	dyncall = SYMBOL_VALUE_ADDRESS (msym);
      else
	dyncall = -1;
    }

  if (!dyncall_external)
    {
      msym = lookup_minimal_symbol ("$$dyncall_external", NULL, NULL);
      if (msym)
	dyncall_external = SYMBOL_VALUE_ADDRESS (msym);
      else
	dyncall_external = -1;
    }

  if (!sr4export)
    {
      msym = lookup_minimal_symbol ("_sr4export", NULL, NULL);
      if (msym)
	sr4export = SYMBOL_VALUE_ADDRESS (msym);
      else
	sr4export = -1;
    }

  /* Addresses passed to dyncall may *NOT* be the actual address
     of the function.  So we may have to do something special.  */
  if (pc == dyncall)
    {
      pc = (CORE_ADDR) read_register (22);

      /* If bit 30 (counting from the left) is on, then pc is the address of
         the PLT entry for this function, not the address of the function
         itself.  Bit 31 has meaning too, but only for MPE.  */
      if (pc & 0x2)
	pc = (CORE_ADDR) read_memory_integer (pc & ~0x3, TARGET_PTR_BIT / 8);
    }
  if (pc == dyncall_external)
    {
      pc = (CORE_ADDR) read_register (22);
      pc = (CORE_ADDR) read_memory_integer (pc & ~0x3, TARGET_PTR_BIT / 8);
    }
  else if (pc == sr4export)
    pc = (CORE_ADDR) (read_register (22));

  /* Get the unwind descriptor corresponding to PC, return zero
     if no unwind was found.  */
  u = find_unwind_entry (pc);
  if (!u)
    return 0;

  /* If this isn't a linker stub, then return now.  */
  /* elz: attention here! (FIXME) because of a compiler/linker 
     error, some stubs which should have a non zero stub_unwind.stub_type 
     have unfortunately a value of zero. So this function would return here
     as if we were not in a trampoline. To fix this, we go look at the partial
     symbol information, which reports this guy as a stub.
     (FIXME): Unfortunately, we are not that lucky: it turns out that the 
     partial symbol information is also wrong sometimes. This is because 
     when it is entered (somread.c::som_symtab_read()) it can happen that
     if the type of the symbol (from the som) is Entry, and the symbol is
     in a shared library, then it can also be a trampoline.  This would
     be OK, except that I believe the way they decide if we are ina shared library
     does not work. SOOOO..., even if we have a regular function w/o trampolines
     its minimal symbol can be assigned type mst_solib_trampoline.
     Also, if we find that the symbol is a real stub, then we fix the unwind
     descriptor, and define the stub type to be EXPORT.
     Hopefully this is correct most of the times. */
  if (u->stub_unwind.stub_type == 0)
    {

/* elz: NOTE (FIXME!) once the problem with the unwind information is fixed
   we can delete all the code which appears between the lines */
/*--------------------------------------------------------------------------*/
      msym = lookup_minimal_symbol_by_pc (pc);

      if (msym == NULL || MSYMBOL_TYPE (msym) != mst_solib_trampoline)
	return orig_pc == pc ? 0 : pc & ~0x3;

      else if (msym != NULL && MSYMBOL_TYPE (msym) == mst_solib_trampoline)
	{
	  struct objfile *objfile;
	  struct minimal_symbol *msymbol;
	  int function_found = 0;

	  /* go look if there is another minimal symbol with the same name as 
	     this one, but with type mst_text. This would happen if the msym
	     is an actual trampoline, in which case there would be another
	     symbol with the same name corresponding to the real function */

	  ALL_MSYMBOLS (objfile, msymbol)
	  {
	    if (MSYMBOL_TYPE (msymbol) == mst_text
		&& DEPRECATED_STREQ (DEPRECATED_SYMBOL_NAME (msymbol), DEPRECATED_SYMBOL_NAME (msym)))
	      {
		function_found = 1;
		break;
	      }
	  }

	  if (function_found)
	    /* the type of msym is correct (mst_solib_trampoline), but
	       the unwind info is wrong, so set it to the correct value */
	    u->stub_unwind.stub_type = EXPORT;
	  else
	    /* the stub type info in the unwind is correct (this is not a
	       trampoline), but the msym type information is wrong, it
	       should be mst_text. So we need to fix the msym, and also
	       get out of this function */
	    {
	      MSYMBOL_TYPE (msym) = mst_text;
	      return orig_pc == pc ? 0 : pc & ~0x3;
	    }
	}

/*--------------------------------------------------------------------------*/
    }

  /* It's a stub.  Search for a branch and figure out where it goes.
     Note we have to handle multi insn branch sequences like ldil;ble.
     Most (all?) other branches can be determined by examining the contents
     of certain registers and the stack.  */

  loc = pc;
  curr_inst = 0;
  prev_inst = 0;
  while (1)
    {
      /* Make sure we haven't walked outside the range of this stub.  */
      if (u != find_unwind_entry (loc))
	{
	  warning ("Unable to find branch in linker stub");
	  return orig_pc == pc ? 0 : pc & ~0x3;
	}

      prev_inst = curr_inst;
      curr_inst = read_memory_integer (loc, 4);

      /* Does it look like a branch external using %r1?  Then it's the
         branch from the stub to the actual function.  */
      if ((curr_inst & 0xffe0e000) == 0xe0202000)
	{
	  /* Yup.  See if the previous instruction loaded
	     a value into %r1.  If so compute and return the jump address.  */
	  if ((prev_inst & 0xffe00000) == 0x20200000)
	    return (extract_21 (prev_inst) + extract_17 (curr_inst)) & ~0x3;
	  else
	    {
	      warning ("Unable to find ldil X,%%r1 before ble Y(%%sr4,%%r1).");
	      return orig_pc == pc ? 0 : pc & ~0x3;
	    }
	}

      /* Does it look like a be 0(sr0,%r21)? OR 
         Does it look like a be, n 0(sr0,%r21)? OR 
         Does it look like a bve (r21)? (this is on PA2.0)
         Does it look like a bve, n(r21)? (this is also on PA2.0)
         That's the branch from an
         import stub to an export stub.

         It is impossible to determine the target of the branch via
         simple examination of instructions and/or data (consider
         that the address in the plabel may be the address of the
         bind-on-reference routine in the dynamic loader).

         So we have try an alternative approach.

         Get the name of the symbol at our current location; it should
         be a stub symbol with the same name as the symbol in the
         shared library.

         Then lookup a minimal symbol with the same name; we should
         get the minimal symbol for the target routine in the shared
         library as those take precedence of import/export stubs.  */
      if ((curr_inst == 0xe2a00000) ||
	  (curr_inst == 0xe2a00002) ||
	  (curr_inst == 0xeaa0d000) ||
	  (curr_inst == 0xeaa0d002))
	{
	  struct minimal_symbol *stubsym, *libsym;

	  stubsym = lookup_minimal_symbol_by_pc (loc);
	  if (stubsym == NULL)
	    {
	      warning ("Unable to find symbol for 0x%lx", loc);
	      return orig_pc == pc ? 0 : pc & ~0x3;
	    }

	  libsym = lookup_minimal_symbol (DEPRECATED_SYMBOL_NAME (stubsym), NULL, NULL);
	  if (libsym == NULL)
	    {
	      warning ("Unable to find library symbol for %s\n",
		       DEPRECATED_SYMBOL_NAME (stubsym));
	      return orig_pc == pc ? 0 : pc & ~0x3;
	    }

	  return SYMBOL_VALUE (libsym);
	}

      /* Does it look like bl X,%rp or bl X,%r0?  Another way to do a
         branch from the stub to the actual function.  */
      /*elz */
      else if ((curr_inst & 0xffe0e000) == 0xe8400000
	       || (curr_inst & 0xffe0e000) == 0xe8000000
	       || (curr_inst & 0xffe0e000) == 0xe800A000)
	return (loc + extract_17 (curr_inst) + 8) & ~0x3;

      /* Does it look like bv (rp)?   Note this depends on the
         current stack pointer being the same as the stack
         pointer in the stub itself!  This is a branch on from the
         stub back to the original caller.  */
      /*else if ((curr_inst & 0xffe0e000) == 0xe840c000) */
      else if ((curr_inst & 0xffe0f000) == 0xe840c000)
	{
	  /* Yup.  See if the previous instruction loaded
	     rp from sp - 8.  */
	  if (prev_inst == 0x4bc23ff1)
	    return (read_memory_integer
		    (read_register (HPPA_SP_REGNUM) - 8, 4)) & ~0x3;
	  else
	    {
	      warning ("Unable to find restore of %%rp before bv (%%rp).");
	      return orig_pc == pc ? 0 : pc & ~0x3;
	    }
	}

      /* elz: added this case to capture the new instruction
         at the end of the return part of an export stub used by
         the PA2.0: BVE, n (rp) */
      else if ((curr_inst & 0xffe0f000) == 0xe840d000)
	{
	  return (read_memory_integer
		  (read_register (HPPA_SP_REGNUM) - 24, TARGET_PTR_BIT / 8)) & ~0x3;
	}

      /* What about be,n 0(sr0,%rp)?  It's just another way we return to
         the original caller from the stub.  Used in dynamic executables.  */
      else if (curr_inst == 0xe0400002)
	{
	  /* The value we jump to is sitting in sp - 24.  But that's
	     loaded several instructions before the be instruction.
	     I guess we could check for the previous instruction being
	     mtsp %r1,%sr0 if we want to do sanity checking.  */
	  return (read_memory_integer
		  (read_register (HPPA_SP_REGNUM) - 24, TARGET_PTR_BIT / 8)) & ~0x3;
	}

      /* Haven't found the branch yet, but we're still in the stub.
         Keep looking.  */
      loc += 4;
    }
}


/* For the given instruction (INST), return any adjustment it makes
   to the stack pointer or zero for no adjustment. 

   This only handles instructions commonly found in prologues.  */

static int
prologue_inst_adjust_sp (unsigned long inst)
{
  /* This must persist across calls.  */
  static int save_high21;

  /* The most common way to perform a stack adjustment ldo X(sp),sp */
  if ((inst & 0xffffc000) == 0x37de0000)
    return extract_14 (inst);

  /* stwm X,D(sp) */
  if ((inst & 0xffe00000) == 0x6fc00000)
    return extract_14 (inst);

  /* std,ma X,D(sp) */
  if ((inst & 0xffe00008) == 0x73c00008)
    return (inst & 0x1 ? -1 << 13 : 0) | (((inst >> 4) & 0x3ff) << 3);

  /* addil high21,%r1; ldo low11,(%r1),%r30)
     save high bits in save_high21 for later use.  */
  if ((inst & 0xffe00000) == 0x28200000)
    {
      save_high21 = extract_21 (inst);
      return 0;
    }

  if ((inst & 0xffff0000) == 0x343e0000)
    return save_high21 + extract_14 (inst);

  /* fstws as used by the HP compilers.  */
  if ((inst & 0xffffffe0) == 0x2fd01220)
    return extract_5_load (inst);

  /* No adjustment.  */
  return 0;
}

/* Return nonzero if INST is a branch of some kind, else return zero.  */

static int
is_branch (unsigned long inst)
{
  switch (inst >> 26)
    {
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
      return 1;

    default:
      return 0;
    }
}

/* Return the register number for a GR which is saved by INST or
   zero it INST does not save a GR.  */

static int
inst_saves_gr (unsigned long inst)
{
  /* Does it look like a stw?  */
  if ((inst >> 26) == 0x1a || (inst >> 26) == 0x1b
      || (inst >> 26) == 0x1f
      || ((inst >> 26) == 0x1f
	  && ((inst >> 6) == 0xa)))
    return extract_5R_store (inst);

  /* Does it look like a std?  */
  if ((inst >> 26) == 0x1c
      || ((inst >> 26) == 0x03
	  && ((inst >> 6) & 0xf) == 0xb))
    return extract_5R_store (inst);

  /* Does it look like a stwm?  GCC & HPC may use this in prologues. */
  if ((inst >> 26) == 0x1b)
    return extract_5R_store (inst);

  /* Does it look like sth or stb?  HPC versions 9.0 and later use these
     too.  */
  if ((inst >> 26) == 0x19 || (inst >> 26) == 0x18
      || ((inst >> 26) == 0x3
	  && (((inst >> 6) & 0xf) == 0x8
	      || (inst >> 6) & 0xf) == 0x9))
    return extract_5R_store (inst);

  return 0;
}

/* Return the register number for a FR which is saved by INST or
   zero it INST does not save a FR.

   Note we only care about full 64bit register stores (that's the only
   kind of stores the prologue will use).

   FIXME: What about argument stores with the HP compiler in ANSI mode? */

static int
inst_saves_fr (unsigned long inst)
{
  /* is this an FSTD ? */
  if ((inst & 0xfc00dfc0) == 0x2c001200)
    return extract_5r_store (inst);
  if ((inst & 0xfc000002) == 0x70000002)
    return extract_5R_store (inst);
  /* is this an FSTW ? */
  if ((inst & 0xfc00df80) == 0x24001200)
    return extract_5r_store (inst);
  if ((inst & 0xfc000002) == 0x7c000000)
    return extract_5R_store (inst);
  return 0;
}

/* Advance PC across any function entry prologue instructions
   to reach some "real" code. 

   Use information in the unwind table to determine what exactly should
   be in the prologue.  */


CORE_ADDR
skip_prologue_hard_way (CORE_ADDR pc)
{
  char buf[4];
  CORE_ADDR orig_pc = pc;
  unsigned long inst, stack_remaining, save_gr, save_fr, save_rp, save_sp;
  unsigned long args_stored, status, i, restart_gr, restart_fr;
  struct unwind_table_entry *u;

  restart_gr = 0;
  restart_fr = 0;

restart:
  u = find_unwind_entry (pc);
  if (!u)
    return pc;

  /* If we are not at the beginning of a function, then return now. */
  if ((pc & ~0x3) != u->region_start)
    return pc;

  /* This is how much of a frame adjustment we need to account for.  */
  stack_remaining = u->Total_frame_size << 3;

  /* Magic register saves we want to know about.  */
  save_rp = u->Save_RP;
  save_sp = u->Save_SP;

  /* An indication that args may be stored into the stack.  Unfortunately
     the HPUX compilers tend to set this in cases where no args were
     stored too!.  */
  args_stored = 1;

  /* Turn the Entry_GR field into a bitmask.  */
  save_gr = 0;
  for (i = 3; i < u->Entry_GR + 3; i++)
    {
      /* Frame pointer gets saved into a special location.  */
      if (u->Save_SP && i == HPPA_FP_REGNUM)
	continue;

      save_gr |= (1 << i);
    }
  save_gr &= ~restart_gr;

  /* Turn the Entry_FR field into a bitmask too.  */
  save_fr = 0;
  for (i = 12; i < u->Entry_FR + 12; i++)
    save_fr |= (1 << i);
  save_fr &= ~restart_fr;

  /* Loop until we find everything of interest or hit a branch.

     For unoptimized GCC code and for any HP CC code this will never ever
     examine any user instructions.

     For optimzied GCC code we're faced with problems.  GCC will schedule
     its prologue and make prologue instructions available for delay slot
     filling.  The end result is user code gets mixed in with the prologue
     and a prologue instruction may be in the delay slot of the first branch
     or call.

     Some unexpected things are expected with debugging optimized code, so
     we allow this routine to walk past user instructions in optimized
     GCC code.  */
  while (save_gr || save_fr || save_rp || save_sp || stack_remaining > 0
	 || args_stored)
    {
      unsigned int reg_num;
      unsigned long old_stack_remaining, old_save_gr, old_save_fr;
      unsigned long old_save_rp, old_save_sp, next_inst;

      /* Save copies of all the triggers so we can compare them later
         (only for HPC).  */
      old_save_gr = save_gr;
      old_save_fr = save_fr;
      old_save_rp = save_rp;
      old_save_sp = save_sp;
      old_stack_remaining = stack_remaining;

      status = target_read_memory (pc, buf, 4);
      inst = extract_unsigned_integer (buf, 4);

      /* Yow! */
      if (status != 0)
	return pc;

      /* Note the interesting effects of this instruction.  */
      stack_remaining -= prologue_inst_adjust_sp (inst);

      /* There are limited ways to store the return pointer into the
	 stack.  */
      if (inst == 0x6bc23fd9 || inst == 0x0fc212c1)
	save_rp = 0;

      /* These are the only ways we save SP into the stack.  At this time
         the HP compilers never bother to save SP into the stack.  */
      if ((inst & 0xffffc000) == 0x6fc10000
	  || (inst & 0xffffc00c) == 0x73c10008)
	save_sp = 0;

      /* Are we loading some register with an offset from the argument
         pointer?  */
      if ((inst & 0xffe00000) == 0x37a00000
	  || (inst & 0xffffffe0) == 0x081d0240)
	{
	  pc += 4;
	  continue;
	}

      /* Account for general and floating-point register saves.  */
      reg_num = inst_saves_gr (inst);
      save_gr &= ~(1 << reg_num);

      /* Ugh.  Also account for argument stores into the stack.
         Unfortunately args_stored only tells us that some arguments
         where stored into the stack.  Not how many or what kind!

         This is a kludge as on the HP compiler sets this bit and it
         never does prologue scheduling.  So once we see one, skip past
         all of them.   We have similar code for the fp arg stores below.

         FIXME.  Can still die if we have a mix of GR and FR argument
         stores!  */
      if (reg_num >= (TARGET_PTR_BIT == 64 ? 19 : 23) && reg_num <= 26)
	{
	  while (reg_num >= (TARGET_PTR_BIT == 64 ? 19 : 23) && reg_num <= 26)
	    {
	      pc += 4;
	      status = target_read_memory (pc, buf, 4);
	      inst = extract_unsigned_integer (buf, 4);
	      if (status != 0)
		return pc;
	      reg_num = inst_saves_gr (inst);
	    }
	  args_stored = 0;
	  continue;
	}

      reg_num = inst_saves_fr (inst);
      save_fr &= ~(1 << reg_num);

      status = target_read_memory (pc + 4, buf, 4);
      next_inst = extract_unsigned_integer (buf, 4);

      /* Yow! */
      if (status != 0)
	return pc;

      /* We've got to be read to handle the ldo before the fp register
         save.  */
      if ((inst & 0xfc000000) == 0x34000000
	  && inst_saves_fr (next_inst) >= 4
	  && inst_saves_fr (next_inst) <= (TARGET_PTR_BIT == 64 ? 11 : 7))
	{
	  /* So we drop into the code below in a reasonable state.  */
	  reg_num = inst_saves_fr (next_inst);
	  pc -= 4;
	}

      /* Ugh.  Also account for argument stores into the stack.
         This is a kludge as on the HP compiler sets this bit and it
         never does prologue scheduling.  So once we see one, skip past
         all of them.  */
      if (reg_num >= 4 && reg_num <= (TARGET_PTR_BIT == 64 ? 11 : 7))
	{
	  while (reg_num >= 4 && reg_num <= (TARGET_PTR_BIT == 64 ? 11 : 7))
	    {
	      pc += 8;
	      status = target_read_memory (pc, buf, 4);
	      inst = extract_unsigned_integer (buf, 4);
	      if (status != 0)
		return pc;
	      if ((inst & 0xfc000000) != 0x34000000)
		break;
	      status = target_read_memory (pc + 4, buf, 4);
	      next_inst = extract_unsigned_integer (buf, 4);
	      if (status != 0)
		return pc;
	      reg_num = inst_saves_fr (next_inst);
	    }
	  args_stored = 0;
	  continue;
	}

      /* Quit if we hit any kind of branch.  This can happen if a prologue
         instruction is in the delay slot of the first call/branch.  */
      if (is_branch (inst))
	break;

      /* What a crock.  The HP compilers set args_stored even if no
         arguments were stored into the stack (boo hiss).  This could
         cause this code to then skip a bunch of user insns (up to the
         first branch).

         To combat this we try to identify when args_stored was bogusly
         set and clear it.   We only do this when args_stored is nonzero,
         all other resources are accounted for, and nothing changed on
         this pass.  */
      if (args_stored
       && !(save_gr || save_fr || save_rp || save_sp || stack_remaining > 0)
	  && old_save_gr == save_gr && old_save_fr == save_fr
	  && old_save_rp == save_rp && old_save_sp == save_sp
	  && old_stack_remaining == stack_remaining)
	break;

      /* Bump the PC.  */
      pc += 4;
    }

  /* We've got a tenative location for the end of the prologue.  However
     because of limitations in the unwind descriptor mechanism we may
     have went too far into user code looking for the save of a register
     that does not exist.  So, if there registers we expected to be saved
     but never were, mask them out and restart.

     This should only happen in optimized code, and should be very rare.  */
  if (save_gr || (save_fr && !(restart_fr || restart_gr)))
    {
      pc = orig_pc;
      restart_gr = save_gr;
      restart_fr = save_fr;
      goto restart;
    }

  return pc;
}


/* Return the address of the PC after the last prologue instruction if
   we can determine it from the debug symbols.  Else return zero.  */

static CORE_ADDR
after_prologue (CORE_ADDR pc)
{
  struct symtab_and_line sal;
  CORE_ADDR func_addr, func_end;
  struct symbol *f;

  /* If we can not find the symbol in the partial symbol table, then
     there is no hope we can determine the function's start address
     with this code.  */
  if (!find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    return 0;

  /* Get the line associated with FUNC_ADDR.  */
  sal = find_pc_line (func_addr, 0);

  /* There are only two cases to consider.  First, the end of the source line
     is within the function bounds.  In that case we return the end of the
     source line.  Second is the end of the source line extends beyond the
     bounds of the current function.  We need to use the slow code to
     examine instructions in that case. 

     Anything else is simply a bug elsewhere.  Fixing it here is absolutely
     the wrong thing to do.  In fact, it should be entirely possible for this
     function to always return zero since the slow instruction scanning code
     is supposed to *always* work.  If it does not, then it is a bug.  */
  if (sal.end < func_end)
    return sal.end;
  else
    return 0;
}

/* To skip prologues, I use this predicate.  Returns either PC itself
   if the code at PC does not look like a function prologue; otherwise
   returns an address that (if we're lucky) follows the prologue.  If
   LENIENT, then we must skip everything which is involved in setting
   up the frame (it's OK to skip more, just so long as we don't skip
   anything which might clobber the registers which are being saved.
   Currently we must not skip more on the alpha, but we might the lenient
   stuff some day.  */

CORE_ADDR
hppa_skip_prologue (CORE_ADDR pc)
{
  unsigned long inst;
  int offset;
  CORE_ADDR post_prologue_pc;
  char buf[4];

  /* See if we can determine the end of the prologue via the symbol table.
     If so, then return either PC, or the PC after the prologue, whichever
     is greater.  */

  post_prologue_pc = after_prologue (pc);

  /* If after_prologue returned a useful address, then use it.  Else
     fall back on the instruction skipping code.

     Some folks have claimed this causes problems because the breakpoint
     may be the first instruction of the prologue.  If that happens, then
     the instruction skipping code has a bug that needs to be fixed.  */
  if (post_prologue_pc != 0)
    return max (pc, post_prologue_pc);
  else
    return (skip_prologue_hard_way (pc));
}

struct hppa_frame_cache
{
  CORE_ADDR base;
  struct trad_frame_saved_reg *saved_regs;
};

static struct hppa_frame_cache *
hppa_frame_cache (struct frame_info *next_frame, void **this_cache)
{
  struct hppa_frame_cache *cache;
  long saved_gr_mask;
  long saved_fr_mask;
  CORE_ADDR this_sp;
  long frame_size;
  struct unwind_table_entry *u;
  int i;

  if ((*this_cache) != NULL)
    return (*this_cache);
  cache = FRAME_OBSTACK_ZALLOC (struct hppa_frame_cache);
  (*this_cache) = cache;
  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  /* Yow! */
  u = find_unwind_entry (frame_func_unwind (next_frame));
  if (!u)
    return (*this_cache);

  /* Turn the Entry_GR field into a bitmask.  */
  saved_gr_mask = 0;
  for (i = 3; i < u->Entry_GR + 3; i++)
    {
      /* Frame pointer gets saved into a special location.  */
      if (u->Save_SP && i == HPPA_FP_REGNUM)
	continue;
	
      saved_gr_mask |= (1 << i);
    }

  /* Turn the Entry_FR field into a bitmask too.  */
  saved_fr_mask = 0;
  for (i = 12; i < u->Entry_FR + 12; i++)
    saved_fr_mask |= (1 << i);

  /* Loop until we find everything of interest or hit a branch.

     For unoptimized GCC code and for any HP CC code this will never ever
     examine any user instructions.

     For optimized GCC code we're faced with problems.  GCC will schedule
     its prologue and make prologue instructions available for delay slot
     filling.  The end result is user code gets mixed in with the prologue
     and a prologue instruction may be in the delay slot of the first branch
     or call.

     Some unexpected things are expected with debugging optimized code, so
     we allow this routine to walk past user instructions in optimized
     GCC code.  */
  {
    int final_iteration = 0;
    CORE_ADDR pc;
    CORE_ADDR end_pc = skip_prologue_using_sal (pc);
    int looking_for_sp = u->Save_SP;
    int looking_for_rp = u->Save_RP;
    int fp_loc = -1;
    if (end_pc == 0)
      end_pc = frame_pc_unwind (next_frame);
    frame_size = 0;
    for (pc = frame_func_unwind (next_frame);
	 ((saved_gr_mask || saved_fr_mask
	   || looking_for_sp || looking_for_rp
	   || frame_size < (u->Total_frame_size << 3))
	  && pc <= end_pc);
	 pc += 4)
      {
	int reg;
	char buf4[4];
	long status = target_read_memory (pc, buf4, sizeof buf4);
	long inst = extract_unsigned_integer (buf4, sizeof buf4);
	
	/* Note the interesting effects of this instruction.  */
	frame_size += prologue_inst_adjust_sp (inst);
	
	/* There are limited ways to store the return pointer into the
	   stack.  */
	if (inst == 0x6bc23fd9) /* stw rp,-0x14(sr0,sp) */
	  {
	    looking_for_rp = 0;
	    cache->saved_regs[RP_REGNUM].addr = -20;
	  }
	else if (inst == 0x0fc212c1) /* std rp,-0x10(sr0,sp) */
	  {
	    looking_for_rp = 0;
	    cache->saved_regs[RP_REGNUM].addr = -16;
	  }
	
	/* Check to see if we saved SP into the stack.  This also
	   happens to indicate the location of the saved frame
	   pointer.  */
	if ((inst & 0xffffc000) == 0x6fc10000  /* stw,ma r1,N(sr0,sp) */
	    || (inst & 0xffffc00c) == 0x73c10008) /* std,ma r1,N(sr0,sp) */
	  {
	    looking_for_sp = 0;
	    cache->saved_regs[HPPA_FP_REGNUM].addr = 0;
	  }
	
	/* Account for general and floating-point register saves.  */
	reg = inst_saves_gr (inst);
	if (reg >= 3 && reg <= 18
	    && (!u->Save_SP || reg != HPPA_FP_REGNUM))
	  {
	    saved_gr_mask &= ~(1 << reg);
	    if ((inst >> 26) == 0x1b && extract_14 (inst) >= 0)
	      /* stwm with a positive displacement is a _post_
		 _modify_.  */
	      cache->saved_regs[reg].addr = 0;
	    else if ((inst & 0xfc00000c) == 0x70000008)
	      /* A std has explicit post_modify forms.  */
	      cache->saved_regs[reg].addr = 0;
	    else
	      {
		CORE_ADDR offset;
		
		if ((inst >> 26) == 0x1c)
		  offset = (inst & 0x1 ? -1 << 13 : 0) | (((inst >> 4) & 0x3ff) << 3);
		else if ((inst >> 26) == 0x03)
		  offset = low_sign_extend (inst & 0x1f, 5);
		else
		  offset = extract_14 (inst);
		
		/* Handle code with and without frame pointers.  */
		if (u->Save_SP)
		  cache->saved_regs[reg].addr = offset;
		else
		  cache->saved_regs[reg].addr = (u->Total_frame_size << 3) + offset;
	      }
	  }

	/* GCC handles callee saved FP regs a little differently.  
	   
	   It emits an instruction to put the value of the start of
	   the FP store area into %r1.  It then uses fstds,ma with a
	   basereg of %r1 for the stores.

	   HP CC emits them at the current stack pointer modifying the
	   stack pointer as it stores each register.  */
	
	/* ldo X(%r3),%r1 or ldo X(%r30),%r1.  */
	if ((inst & 0xffffc000) == 0x34610000
	    || (inst & 0xffffc000) == 0x37c10000)
	  fp_loc = extract_14 (inst);
	
	reg = inst_saves_fr (inst);
	if (reg >= 12 && reg <= 21)
	  {
	    /* Note +4 braindamage below is necessary because the FP
	       status registers are internally 8 registers rather than
	       the expected 4 registers.  */
	    saved_fr_mask &= ~(1 << reg);
	    if (fp_loc == -1)
	      {
		/* 1st HP CC FP register store.  After this
		   instruction we've set enough state that the GCC and
		   HPCC code are both handled in the same manner.  */
		cache->saved_regs[reg + FP4_REGNUM + 4].addr = 0;
		fp_loc = 8;
	      }
	    else
	      {
		cache->saved_regs[reg + HPPA_FP0_REGNUM + 4].addr = fp_loc;
		fp_loc += 8;
	      }
	  }
	
	/* Quit if we hit any kind of branch the previous iteration. */
	if (final_iteration)
	  break;
	/* We want to look precisely one instruction beyond the branch
	   if we have not found everything yet.  */
	if (is_branch (inst))
	  final_iteration = 1;
      }
  }

  {
    /* The frame base always represents the value of %sp at entry to
       the current function (and is thus equivalent to the "saved"
       stack pointer.  */
    CORE_ADDR this_sp = frame_unwind_register_unsigned (next_frame, HPPA_SP_REGNUM);
    /* FIXME: cagney/2004-02-22: This assumes that the frame has been
       created.  If it hasn't everything will be out-of-wack.  */
    if (u->Save_SP && trad_frame_addr_p (cache->saved_regs, HPPA_SP_REGNUM))
      /* Both we're expecting the SP to be saved and the SP has been
	 saved.  The entry SP value is saved at this frame's SP
	 address.  */
      cache->base = read_memory_integer (this_sp, TARGET_PTR_BIT / 8);
    else
      /* The prologue has been slowly allocating stack space.  Adjust
	 the SP back.  */
      cache->base = this_sp - frame_size;
    trad_frame_set_value (cache->saved_regs, HPPA_SP_REGNUM, cache->base);
  }

  /* The PC is found in the "return register", "Millicode" uses "r31"
     as the return register while normal code uses "rp".  */
  if (u->Millicode)
    cache->saved_regs[PCOQ_HEAD_REGNUM] = cache->saved_regs[31];
  else
    cache->saved_regs[PCOQ_HEAD_REGNUM] = cache->saved_regs[RP_REGNUM];

  {
    /* Convert all the offsets into addresses.  */
    int reg;
    for (reg = 0; reg < NUM_REGS; reg++)
      {
	if (trad_frame_addr_p (cache->saved_regs, reg))
	  cache->saved_regs[reg].addr += cache->base;
      }
  }

  return (*this_cache);
}

static void
hppa_frame_this_id (struct frame_info *next_frame, void **this_cache,
			   struct frame_id *this_id)
{
  struct hppa_frame_cache *info = hppa_frame_cache (next_frame, this_cache);
  (*this_id) = frame_id_build (info->base, frame_func_unwind (next_frame));
}

static void
hppa_frame_prev_register (struct frame_info *next_frame,
				 void **this_cache,
				 int regnum, int *optimizedp,
				 enum lval_type *lvalp, CORE_ADDR *addrp,
				 int *realnump, void *valuep)
{
  struct hppa_frame_cache *info = hppa_frame_cache (next_frame, this_cache);
  struct gdbarch *gdbarch = get_frame_arch (next_frame);
  if (regnum == PCOQ_TAIL_REGNUM)
    {
      /* The PCOQ TAIL, or NPC, needs to be computed from the unwound
	 PC register.  */
      *optimizedp = 0;
      *lvalp = not_lval;
      *addrp = 0;
      *realnump = 0;
      if (valuep)
	{
	  int regsize = register_size (gdbarch, PCOQ_HEAD_REGNUM);
	  CORE_ADDR pc;
	  int optimized;
	  enum lval_type lval;
	  CORE_ADDR addr;
	  int realnum;
	  bfd_byte value[MAX_REGISTER_SIZE];
	  trad_frame_prev_register (next_frame, info->saved_regs,
				    PCOQ_HEAD_REGNUM, &optimized, &lval, &addr,
				    &realnum, &value);
	  pc = extract_unsigned_integer (&value, regsize);
	  store_unsigned_integer (valuep, regsize, pc + 4);
	}
    }
  else
    {
      trad_frame_prev_register (next_frame, info->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, valuep);
    }
}

static const struct frame_unwind hppa_frame_unwind =
{
  NORMAL_FRAME,
  hppa_frame_this_id,
  hppa_frame_prev_register
};

static const struct frame_unwind *
hppa_frame_unwind_sniffer (struct frame_info *next_frame)
{
  return &hppa_frame_unwind;
}

static CORE_ADDR
hppa_frame_base_address (struct frame_info *next_frame,
				void **this_cache)
{
  struct hppa_frame_cache *info = hppa_frame_cache (next_frame,
							   this_cache);
  return info->base;
}

static const struct frame_base hppa_frame_base = {
  &hppa_frame_unwind,
  hppa_frame_base_address,
  hppa_frame_base_address,
  hppa_frame_base_address
};

static const struct frame_base *
hppa_frame_base_sniffer (struct frame_info *next_frame)
{
  return &hppa_frame_base;
}

static struct frame_id
hppa_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_id_build (frame_unwind_register_unsigned (next_frame,
							 HPPA_SP_REGNUM),
			 frame_pc_unwind (next_frame));
}

static CORE_ADDR
hppa_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_signed (next_frame, PCOQ_HEAD_REGNUM) & ~3;
}

/* Exception handling support for the HP-UX ANSI C++ compiler.
   The compiler (aCC) provides a callback for exception events;
   GDB can set a breakpoint on this callback and find out what
   exception event has occurred. */

/* The name of the hook to be set to point to the callback function */
static char HP_ACC_EH_notify_hook[] = "__eh_notify_hook";
/* The name of the function to be used to set the hook value */
static char HP_ACC_EH_set_hook_value[] = "__eh_set_hook_value";
/* The name of the callback function in end.o */
static char HP_ACC_EH_notify_callback[] = "__d_eh_notify_callback";
/* Name of function in end.o on which a break is set (called by above) */
static char HP_ACC_EH_break[] = "__d_eh_break";
/* Name of flag (in end.o) that enables catching throws */
static char HP_ACC_EH_catch_throw[] = "__d_eh_catch_throw";
/* Name of flag (in end.o) that enables catching catching */
static char HP_ACC_EH_catch_catch[] = "__d_eh_catch_catch";
/* The enum used by aCC */
typedef enum
  {
    __EH_NOTIFY_THROW,
    __EH_NOTIFY_CATCH
  }
__eh_notification;

/* Is exception-handling support available with this executable? */
static int hp_cxx_exception_support = 0;
/* Has the initialize function been run? */
int hp_cxx_exception_support_initialized = 0;
/* Similar to above, but imported from breakpoint.c -- non-target-specific */
extern int exception_support_initialized;
/* Address of __eh_notify_hook */
static CORE_ADDR eh_notify_hook_addr = 0;
/* Address of __d_eh_notify_callback */
static CORE_ADDR eh_notify_callback_addr = 0;
/* Address of __d_eh_break */
static CORE_ADDR eh_break_addr = 0;
/* Address of __d_eh_catch_catch */
static CORE_ADDR eh_catch_catch_addr = 0;
/* Address of __d_eh_catch_throw */
static CORE_ADDR eh_catch_throw_addr = 0;
/* Sal for __d_eh_break */
static struct symtab_and_line *break_callback_sal = 0;

/* Code in end.c expects __d_pid to be set in the inferior,
   otherwise __d_eh_notify_callback doesn't bother to call
   __d_eh_break!  So we poke the pid into this symbol
   ourselves.
   0 => success
   1 => failure  */
int
setup_d_pid_in_inferior (void)
{
  CORE_ADDR anaddr;
  struct minimal_symbol *msymbol;
  char buf[4];			/* FIXME 32x64? */

  /* Slam the pid of the process into __d_pid; failing is only a warning!  */
  msymbol = lookup_minimal_symbol ("__d_pid", NULL, symfile_objfile);
  if (msymbol == NULL)
    {
      warning ("Unable to find __d_pid symbol in object file.");
      warning ("Suggest linking executable with -g (links in /opt/langtools/lib/end.o).");
      return 1;
    }

  anaddr = SYMBOL_VALUE_ADDRESS (msymbol);
  store_unsigned_integer (buf, 4, PIDGET (inferior_ptid)); /* FIXME 32x64? */
  if (target_write_memory (anaddr, buf, 4))	/* FIXME 32x64? */
    {
      warning ("Unable to write __d_pid");
      warning ("Suggest linking executable with -g (links in /opt/langtools/lib/end.o).");
      return 1;
    }
  return 0;
}

/* Initialize exception catchpoint support by looking for the
   necessary hooks/callbacks in end.o, etc., and set the hook value to
   point to the required debug function

   Return 0 => failure
   1 => success          */

static int
initialize_hp_cxx_exception_support (void)
{
  struct symtabs_and_lines sals;
  struct cleanup *old_chain;
  struct cleanup *canonical_strings_chain = NULL;
  int i;
  char *addr_start;
  char *addr_end = NULL;
  char **canonical = (char **) NULL;
  int thread = -1;
  struct symbol *sym = NULL;
  struct minimal_symbol *msym = NULL;
  struct objfile *objfile;
  asection *shlib_info;

  /* Detect and disallow recursion.  On HP-UX with aCC, infinite
     recursion is a possibility because finding the hook for exception
     callbacks involves making a call in the inferior, which means
     re-inserting breakpoints which can re-invoke this code */

  static int recurse = 0;
  if (recurse > 0)
    {
      hp_cxx_exception_support_initialized = 0;
      exception_support_initialized = 0;
      return 0;
    }

  hp_cxx_exception_support = 0;

  /* First check if we have seen any HP compiled objects; if not,
     it is very unlikely that HP's idiosyncratic callback mechanism
     for exception handling debug support will be available!
     This will percolate back up to breakpoint.c, where our callers
     will decide to try the g++ exception-handling support instead. */
  if (!hp_som_som_object_present)
    return 0;

  /* We have a SOM executable with SOM debug info; find the hooks */

  /* First look for the notify hook provided by aCC runtime libs */
  /* If we find this symbol, we conclude that the executable must
     have HP aCC exception support built in.  If this symbol is not
     found, even though we're a HP SOM-SOM file, we may have been
     built with some other compiler (not aCC).  This results percolates
     back up to our callers in breakpoint.c which can decide to
     try the g++ style of exception support instead.
     If this symbol is found but the other symbols we require are
     not found, there is something weird going on, and g++ support
     should *not* be tried as an alternative.

     ASSUMPTION: Only HP aCC code will have __eh_notify_hook defined.  
     ASSUMPTION: HP aCC and g++ modules cannot be linked together. */

  /* libCsup has this hook; it'll usually be non-debuggable */
  msym = lookup_minimal_symbol (HP_ACC_EH_notify_hook, NULL, NULL);
  if (msym)
    {
      eh_notify_hook_addr = SYMBOL_VALUE_ADDRESS (msym);
      hp_cxx_exception_support = 1;
    }
  else
    {
      warning ("Unable to find exception callback hook (%s).", HP_ACC_EH_notify_hook);
      warning ("Executable may not have been compiled debuggable with HP aCC.");
      warning ("GDB will be unable to intercept exception events.");
      eh_notify_hook_addr = 0;
      hp_cxx_exception_support = 0;
      return 0;
    }

  /* Next look for the notify callback routine in end.o */
  /* This is always available in the SOM symbol dictionary if end.o is linked in */
  msym = lookup_minimal_symbol (HP_ACC_EH_notify_callback, NULL, NULL);
  if (msym)
    {
      eh_notify_callback_addr = SYMBOL_VALUE_ADDRESS (msym);
      hp_cxx_exception_support = 1;
    }
  else
    {
      warning ("Unable to find exception callback routine (%s).", HP_ACC_EH_notify_callback);
      warning ("Suggest linking executable with -g (links in /opt/langtools/lib/end.o).");
      warning ("GDB will be unable to intercept exception events.");
      eh_notify_callback_addr = 0;
      return 0;
    }

#ifndef GDB_TARGET_IS_HPPA_20W
  /* Check whether the executable is dynamically linked or archive bound */
  /* With an archive-bound executable we can use the raw addresses we find
     for the callback function, etc. without modification. For an executable
     with shared libraries, we have to do more work to find the plabel, which
     can be the target of a call through $$dyncall from the aCC runtime support
     library (libCsup) which is linked shared by default by aCC. */
  /* This test below was copied from somsolib.c/somread.c.  It may not be a very
     reliable one to test that an executable is linked shared. pai/1997-07-18 */
  shlib_info = bfd_get_section_by_name (symfile_objfile->obfd, "$SHLIB_INFO$");
  if (shlib_info && (bfd_section_size (symfile_objfile->obfd, shlib_info) != 0))
    {
      /* The minsym we have has the local code address, but that's not the
         plabel that can be used by an inter-load-module call. */
      /* Find solib handle for main image (which has end.o), and use that
         and the min sym as arguments to __d_shl_get() (which does the equivalent
         of shl_findsym()) to find the plabel. */

      args_for_find_stub args;
      static char message[] = "Error while finding exception callback hook:\n";

      args.solib_handle = som_solib_get_solib_by_pc (eh_notify_callback_addr);
      args.msym = msym;
      args.return_val = 0;

      recurse++;
      catch_errors (cover_find_stub_with_shl_get, &args, message,
		    RETURN_MASK_ALL);
      eh_notify_callback_addr = args.return_val;
      recurse--;

      exception_catchpoints_are_fragile = 1;

      if (!eh_notify_callback_addr)
	{
	  /* We can get here either if there is no plabel in the export list
	     for the main image, or if something strange happened (?) */
	  warning ("Couldn't find a plabel (indirect function label) for the exception callback.");
	  warning ("GDB will not be able to intercept exception events.");
	  return 0;
	}
    }
  else
    exception_catchpoints_are_fragile = 0;
#endif

  /* Now, look for the breakpointable routine in end.o */
  /* This should also be available in the SOM symbol dict. if end.o linked in */
  msym = lookup_minimal_symbol (HP_ACC_EH_break, NULL, NULL);
  if (msym)
    {
      eh_break_addr = SYMBOL_VALUE_ADDRESS (msym);
      hp_cxx_exception_support = 1;
    }
  else
    {
      warning ("Unable to find exception callback routine to set breakpoint (%s).", HP_ACC_EH_break);
      warning ("Suggest linking executable with -g (link in /opt/langtools/lib/end.o).");
      warning ("GDB will be unable to intercept exception events.");
      eh_break_addr = 0;
      return 0;
    }

  /* Next look for the catch enable flag provided in end.o */
  sym = lookup_symbol (HP_ACC_EH_catch_catch, (struct block *) NULL,
		       VAR_DOMAIN, 0, (struct symtab **) NULL);
  if (sym)			/* sometimes present in debug info */
    {
      eh_catch_catch_addr = SYMBOL_VALUE_ADDRESS (sym);
      hp_cxx_exception_support = 1;
    }
  else
    /* otherwise look in SOM symbol dict. */
    {
      msym = lookup_minimal_symbol (HP_ACC_EH_catch_catch, NULL, NULL);
      if (msym)
	{
	  eh_catch_catch_addr = SYMBOL_VALUE_ADDRESS (msym);
	  hp_cxx_exception_support = 1;
	}
      else
	{
	  warning ("Unable to enable interception of exception catches.");
	  warning ("Executable may not have been compiled debuggable with HP aCC.");
	  warning ("Suggest linking executable with -g (link in /opt/langtools/lib/end.o).");
	  return 0;
	}
    }

  /* Next look for the catch enable flag provided end.o */
  sym = lookup_symbol (HP_ACC_EH_catch_catch, (struct block *) NULL,
		       VAR_DOMAIN, 0, (struct symtab **) NULL);
  if (sym)			/* sometimes present in debug info */
    {
      eh_catch_throw_addr = SYMBOL_VALUE_ADDRESS (sym);
      hp_cxx_exception_support = 1;
    }
  else
    /* otherwise look in SOM symbol dict. */
    {
      msym = lookup_minimal_symbol (HP_ACC_EH_catch_throw, NULL, NULL);
      if (msym)
	{
	  eh_catch_throw_addr = SYMBOL_VALUE_ADDRESS (msym);
	  hp_cxx_exception_support = 1;
	}
      else
	{
	  warning ("Unable to enable interception of exception throws.");
	  warning ("Executable may not have been compiled debuggable with HP aCC.");
	  warning ("Suggest linking executable with -g (link in /opt/langtools/lib/end.o).");
	  return 0;
	}
    }

  /* Set the flags */
  hp_cxx_exception_support = 2;	/* everything worked so far */
  hp_cxx_exception_support_initialized = 1;
  exception_support_initialized = 1;

  return 1;
}

/* Target operation for enabling or disabling interception of
   exception events.
   KIND is either EX_EVENT_THROW or EX_EVENT_CATCH
   ENABLE is either 0 (disable) or 1 (enable).
   Return value is NULL if no support found;
   -1 if something went wrong,
   or a pointer to a symtab/line struct if the breakpointable
   address was found. */

struct symtab_and_line *
child_enable_exception_callback (enum exception_event_kind kind, int enable)
{
  char buf[4];

  if (!exception_support_initialized || !hp_cxx_exception_support_initialized)
    if (!initialize_hp_cxx_exception_support ())
      return NULL;

  switch (hp_cxx_exception_support)
    {
    case 0:
      /* Assuming no HP support at all */
      return NULL;
    case 1:
      /* HP support should be present, but something went wrong */
      return (struct symtab_and_line *) -1;	/* yuck! */
      /* there may be other cases in the future */
    }

  /* Set the EH hook to point to the callback routine */
  store_unsigned_integer (buf, 4, enable ? eh_notify_callback_addr : 0);	/* FIXME 32x64 problem */
  /* pai: (temp) FIXME should there be a pack operation first? */
  if (target_write_memory (eh_notify_hook_addr, buf, 4))	/* FIXME 32x64 problem */
    {
      warning ("Could not write to target memory for exception event callback.");
      warning ("Interception of exception events may not work.");
      return (struct symtab_and_line *) -1;
    }
  if (enable)
    {
      /* Ensure that __d_pid is set up correctly -- end.c code checks this. :-( */
      if (PIDGET (inferior_ptid) > 0)
	{
	  if (setup_d_pid_in_inferior ())
	    return (struct symtab_and_line *) -1;
	}
      else
	{
	  warning ("Internal error: Invalid inferior pid?  Cannot intercept exception events.");
	  return (struct symtab_and_line *) -1;
	}
    }

  switch (kind)
    {
    case EX_EVENT_THROW:
      store_unsigned_integer (buf, 4, enable ? 1 : 0);
      if (target_write_memory (eh_catch_throw_addr, buf, 4))	/* FIXME 32x64? */
	{
	  warning ("Couldn't enable exception throw interception.");
	  return (struct symtab_and_line *) -1;
	}
      break;
    case EX_EVENT_CATCH:
      store_unsigned_integer (buf, 4, enable ? 1 : 0);
      if (target_write_memory (eh_catch_catch_addr, buf, 4))	/* FIXME 32x64? */
	{
	  warning ("Couldn't enable exception catch interception.");
	  return (struct symtab_and_line *) -1;
	}
      break;
    default:
      error ("Request to enable unknown or unsupported exception event.");
    }

  /* Copy break address into new sal struct, malloc'ing if needed. */
  if (!break_callback_sal)
    {
      break_callback_sal = (struct symtab_and_line *) xmalloc (sizeof (struct symtab_and_line));
    }
  init_sal (break_callback_sal);
  break_callback_sal->symtab = NULL;
  break_callback_sal->pc = eh_break_addr;
  break_callback_sal->line = 0;
  break_callback_sal->end = eh_break_addr;

  return break_callback_sal;
}

/* Record some information about the current exception event */
static struct exception_event_record current_ex_event;
/* Convenience struct */
static struct symtab_and_line null_symtab_and_line =
{NULL, 0, 0, 0};

/* Report current exception event.  Returns a pointer to a record
   that describes the kind of the event, where it was thrown from,
   and where it will be caught.  More information may be reported
   in the future */
struct exception_event_record *
child_get_current_exception_event (void)
{
  CORE_ADDR event_kind;
  CORE_ADDR throw_addr;
  CORE_ADDR catch_addr;
  struct frame_info *fi, *curr_frame;
  int level = 1;

  curr_frame = get_current_frame ();
  if (!curr_frame)
    return (struct exception_event_record *) NULL;

  /* Go up one frame to __d_eh_notify_callback, because at the
     point when this code is executed, there's garbage in the
     arguments of __d_eh_break. */
  fi = find_relative_frame (curr_frame, &level);
  if (level != 0)
    return (struct exception_event_record *) NULL;

  select_frame (fi);

  /* Read in the arguments */
  /* __d_eh_notify_callback() is called with 3 arguments:
     1. event kind catch or throw
     2. the target address if known
     3. a flag -- not sure what this is. pai/1997-07-17 */
  event_kind = read_register (ARG0_REGNUM);
  catch_addr = read_register (ARG1_REGNUM);

  /* Now go down to a user frame */
  /* For a throw, __d_eh_break is called by
     __d_eh_notify_callback which is called by
     __notify_throw which is called
     from user code.
     For a catch, __d_eh_break is called by
     __d_eh_notify_callback which is called by
     <stackwalking stuff> which is called by
     __throw__<stuff> or __rethrow_<stuff> which is called
     from user code. */
  /* FIXME: Don't use such magic numbers; search for the frames */
  level = (event_kind == EX_EVENT_THROW) ? 3 : 4;
  fi = find_relative_frame (curr_frame, &level);
  if (level != 0)
    return (struct exception_event_record *) NULL;

  select_frame (fi);
  throw_addr = get_frame_pc (fi);

  /* Go back to original (top) frame */
  select_frame (curr_frame);

  current_ex_event.kind = (enum exception_event_kind) event_kind;
  current_ex_event.throw_sal = find_pc_line (throw_addr, 1);
  current_ex_event.catch_sal = find_pc_line (catch_addr, 1);

  return &current_ex_event;
}

/* Instead of this nasty cast, add a method pvoid() that prints out a
   host VOID data type (remember %p isn't portable).  */

static CORE_ADDR
hppa_pointer_to_address_hack (void *ptr)
{
  gdb_assert (sizeof (ptr) == TYPE_LENGTH (builtin_type_void_data_ptr));
  return POINTER_TO_ADDRESS (builtin_type_void_data_ptr, &ptr);
}

static void
unwind_command (char *exp, int from_tty)
{
  CORE_ADDR address;
  struct unwind_table_entry *u;

  /* If we have an expression, evaluate it and use it as the address.  */

  if (exp != 0 && *exp != 0)
    address = parse_and_eval_address (exp);
  else
    return;

  u = find_unwind_entry (address);

  if (!u)
    {
      printf_unfiltered ("Can't find unwind table entry for %s\n", exp);
      return;
    }

  printf_unfiltered ("unwind_table_entry (0x%s):\n",
		     paddr_nz (hppa_pointer_to_address_hack (u)));

  printf_unfiltered ("\tregion_start = ");
  print_address (u->region_start, gdb_stdout);

  printf_unfiltered ("\n\tregion_end = ");
  print_address (u->region_end, gdb_stdout);

#define pif(FLD) if (u->FLD) printf_unfiltered (" "#FLD);

  printf_unfiltered ("\n\tflags =");
  pif (Cannot_unwind);
  pif (Millicode);
  pif (Millicode_save_sr0);
  pif (Entry_SR);
  pif (Args_stored);
  pif (Variable_Frame);
  pif (Separate_Package_Body);
  pif (Frame_Extension_Millicode);
  pif (Stack_Overflow_Check);
  pif (Two_Instruction_SP_Increment);
  pif (Ada_Region);
  pif (Save_SP);
  pif (Save_RP);
  pif (Save_MRP_in_frame);
  pif (extn_ptr_defined);
  pif (Cleanup_defined);
  pif (MPE_XL_interrupt_marker);
  pif (HP_UX_interrupt_marker);
  pif (Large_frame);

  putchar_unfiltered ('\n');

#define pin(FLD) printf_unfiltered ("\t"#FLD" = 0x%x\n", u->FLD);

  pin (Region_description);
  pin (Entry_FR);
  pin (Entry_GR);
  pin (Total_frame_size);
}

void
hppa_skip_permanent_breakpoint (void)
{
  /* To step over a breakpoint instruction on the PA takes some
     fiddling with the instruction address queue.

     When we stop at a breakpoint, the IA queue front (the instruction
     we're executing now) points at the breakpoint instruction, and
     the IA queue back (the next instruction to execute) points to
     whatever instruction we would execute after the breakpoint, if it
     were an ordinary instruction.  This is the case even if the
     breakpoint is in the delay slot of a branch instruction.

     Clearly, to step past the breakpoint, we need to set the queue
     front to the back.  But what do we put in the back?  What
     instruction comes after that one?  Because of the branch delay
     slot, the next insn is always at the back + 4.  */
  write_register (PCOQ_HEAD_REGNUM, read_register (PCOQ_TAIL_REGNUM));
  write_register (PCSQ_HEAD_REGNUM, read_register (PCSQ_TAIL_REGNUM));

  write_register (PCOQ_TAIL_REGNUM, read_register (PCOQ_TAIL_REGNUM) + 4);
  /* We can leave the tail's space the same, since there's no jump.  */
}

int
hppa_reg_struct_has_addr (int gcc_p, struct type *type)
{
  /* On the PA, any pass-by-value structure > 8 bytes is actually passed
     via a pointer regardless of its type or the compiler used.  */
  return (TYPE_LENGTH (type) > 8);
}

int
hppa_inner_than (CORE_ADDR lhs, CORE_ADDR rhs)
{
  /* Stack grows upward */
  return (lhs > rhs);
}

int
hppa_pc_requires_run_before_use (CORE_ADDR pc)
{
  /* Sometimes we may pluck out a minimal symbol that has a negative address.
  
     An example of this occurs when an a.out is linked against a foo.sl.
     The foo.sl defines a global bar(), and the a.out declares a signature
     for bar().  However, the a.out doesn't directly call bar(), but passes
     its address in another call.
  
     If you have this scenario and attempt to "break bar" before running,
     gdb will find a minimal symbol for bar() in the a.out.  But that
     symbol's address will be negative.  What this appears to denote is
     an index backwards from the base of the procedure linkage table (PLT)
     into the data linkage table (DLT), the end of which is contiguous
     with the start of the PLT.  This is clearly not a valid address for
     us to set a breakpoint on.
  
     Note that one must be careful in how one checks for a negative address.
     0xc0000000 is a legitimate address of something in a shared text
     segment, for example.  Since I don't know what the possible range
     is of these "really, truly negative" addresses that come from the
     minimal symbols, I'm resorting to the gross hack of checking the
     top byte of the address for all 1's.  Sigh.  */

  return (!target_has_stack && (pc & 0xFF000000));
}

int
hppa_instruction_nullified (void)
{
  /* brobecker 2002/11/07: Couldn't we use a ULONGEST here? It would
     avoid the type cast.  I'm leaving it as is for now as I'm doing
     semi-mechanical multiarching-related changes.  */
  const int ipsw = (int) read_register (IPSW_REGNUM);
  const int flags = (int) read_register (FLAGS_REGNUM);

  return ((ipsw & 0x00200000) && !(flags & 0x2));
}

/* Return the GDB type object for the "standard" data type of data
   in register N.  */

static struct type *
hppa32_register_type (struct gdbarch *gdbarch, int reg_nr)
{
   if (reg_nr < FP4_REGNUM)
     return builtin_type_uint32;
   else
     return builtin_type_ieee_single_big;
}

/* Return the GDB type object for the "standard" data type of data
   in register N.  hppa64 version.  */

static struct type *
hppa64_register_type (struct gdbarch *gdbarch, int reg_nr)
{
   if (reg_nr < FP4_REGNUM)
     return builtin_type_uint64;
   else
     return builtin_type_ieee_double_big;
}

/* Return True if REGNUM is not a register available to the user
   through ptrace().  */

int
hppa_cannot_store_register (int regnum)
{
  return (regnum == 0
          || regnum == PCSQ_HEAD_REGNUM
          || (regnum >= PCSQ_TAIL_REGNUM && regnum < IPSW_REGNUM)
          || (regnum > IPSW_REGNUM && regnum < FP4_REGNUM));

}

CORE_ADDR
hppa_smash_text_address (CORE_ADDR addr)
{
  /* The low two bits of the PC on the PA contain the privilege level.
     Some genius implementing a (non-GCC) compiler apparently decided
     this means that "addresses" in a text section therefore include a
     privilege level, and thus symbol tables should contain these bits.
     This seems like a bonehead thing to do--anyway, it seems to work
     for our purposes to just ignore those bits.  */

  return (addr &= ~0x3);
}

/* Get the ith function argument for the current function.  */
CORE_ADDR
hppa_fetch_pointer_argument (struct frame_info *frame, int argi, 
			     struct type *type)
{
  CORE_ADDR addr;
  get_frame_register (frame, R0_REGNUM + 26 - argi, &addr);
  return addr;
}

/* Here is a table of C type sizes on hppa with various compiles
   and options.  I measured this on PA 9000/800 with HP-UX 11.11
   and these compilers:

     /usr/ccs/bin/cc    HP92453-01 A.11.01.21
     /opt/ansic/bin/cc  HP92453-01 B.11.11.28706.GP
     /opt/aCC/bin/aCC   B3910B A.03.45
     gcc                gcc 3.3.2 native hppa2.0w-hp-hpux11.11

     cc            : 1 2 4 4 8 : 4 8 -- : 4 4
     ansic +DA1.1  : 1 2 4 4 8 : 4 8 16 : 4 4
     ansic +DA2.0  : 1 2 4 4 8 : 4 8 16 : 4 4
     ansic +DA2.0W : 1 2 4 8 8 : 4 8 16 : 8 8
     acc   +DA1.1  : 1 2 4 4 8 : 4 8 16 : 4 4
     acc   +DA2.0  : 1 2 4 4 8 : 4 8 16 : 4 4
     acc   +DA2.0W : 1 2 4 8 8 : 4 8 16 : 8 8
     gcc           : 1 2 4 4 8 : 4 8 16 : 4 4

   Each line is:

     compiler and options
     char, short, int, long, long long
     float, double, long double
     char *, void (*)()

   So all these compilers use either ILP32 or LP64 model.
   TODO: gcc has more options so it needs more investigation.

   For floating point types, see:

     http://docs.hp.com/hpux/pdf/B3906-90006.pdf
     HP-UX floating-point guide, hpux 11.00

   -- chastain 2003-12-18  */

static struct gdbarch *
hppa_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch_tdep *tdep;
  struct gdbarch *gdbarch;
  
  /* Try to determine the ABI of the object we are loading.  */
  if (info.abfd != NULL && info.osabi == GDB_OSABI_UNKNOWN)
    {
      /* If it's a SOM file, assume it's HP/UX SOM.  */
      if (bfd_get_flavour (info.abfd) == bfd_target_som_flavour)
	info.osabi = GDB_OSABI_HPUX_SOM;
    }

  /* find a candidate among the list of pre-declared architectures.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return (arches->gdbarch);

  /* If none found, then allocate and initialize one.  */
  tdep = XMALLOC (struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (&info, tdep);

  /* Determine from the bfd_arch_info structure if we are dealing with
     a 32 or 64 bits architecture.  If the bfd_arch_info is not available,
     then default to a 32bit machine.  */
  if (info.bfd_arch_info != NULL)
    tdep->bytes_per_address =
      info.bfd_arch_info->bits_per_address / info.bfd_arch_info->bits_per_byte;
  else
    tdep->bytes_per_address = 4;

  /* Some parts of the gdbarch vector depend on whether we are running
     on a 32 bits or 64 bits target.  */
  switch (tdep->bytes_per_address)
    {
      case 4:
        set_gdbarch_num_regs (gdbarch, hppa32_num_regs);
        set_gdbarch_register_name (gdbarch, hppa32_register_name);
        set_gdbarch_register_type (gdbarch, hppa32_register_type);
        break;
      case 8:
        set_gdbarch_num_regs (gdbarch, hppa64_num_regs);
        set_gdbarch_register_name (gdbarch, hppa64_register_name);
        set_gdbarch_register_type (gdbarch, hppa64_register_type);
        break;
      default:
        internal_error (__FILE__, __LINE__, "Unsupported address size: %d",
                        tdep->bytes_per_address);
    }

  set_gdbarch_long_bit (gdbarch, tdep->bytes_per_address * TARGET_CHAR_BIT);
  set_gdbarch_ptr_bit (gdbarch, tdep->bytes_per_address * TARGET_CHAR_BIT);

  /* The following gdbarch vector elements are the same in both ILP32
     and LP64, but might show differences some day.  */
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 128);
  set_gdbarch_long_double_format (gdbarch, &floatformat_ia64_quad_big);

  /* The following gdbarch vector elements do not depend on the address
     size, or in any other gdbarch element previously set.  */
  set_gdbarch_skip_prologue (gdbarch, hppa_skip_prologue);
  set_gdbarch_skip_trampoline_code (gdbarch, hppa_skip_trampoline_code);
  set_gdbarch_in_solib_call_trampoline (gdbarch, hppa_in_solib_call_trampoline);
  set_gdbarch_in_solib_return_trampoline (gdbarch,
                                          hppa_in_solib_return_trampoline);
  set_gdbarch_inner_than (gdbarch, hppa_inner_than);
  set_gdbarch_sp_regnum (gdbarch, HPPA_SP_REGNUM);
  set_gdbarch_fp0_regnum (gdbarch, HPPA_FP0_REGNUM);
  set_gdbarch_cannot_store_register (gdbarch, hppa_cannot_store_register);
  set_gdbarch_addr_bits_remove (gdbarch, hppa_smash_text_address);
  set_gdbarch_smash_text_address (gdbarch, hppa_smash_text_address);
  set_gdbarch_believe_pcc_promotion (gdbarch, 1);
  set_gdbarch_read_pc (gdbarch, hppa_target_read_pc);
  set_gdbarch_write_pc (gdbarch, hppa_target_write_pc);

  /* Helper for function argument information.  */
  set_gdbarch_fetch_pointer_argument (gdbarch, hppa_fetch_pointer_argument);

  set_gdbarch_print_insn (gdbarch, print_insn_hppa);

  /* When a hardware watchpoint triggers, we'll move the inferior past
     it by removing all eventpoints; stepping past the instruction
     that caused the trigger; reinserting eventpoints; and checking
     whether any watched location changed.  */
  set_gdbarch_have_nonsteppable_watchpoint (gdbarch, 1);

  /* Inferior function call methods.  */
  switch (tdep->bytes_per_address)
    {
    case 4:
      set_gdbarch_push_dummy_call (gdbarch, hppa32_push_dummy_call);
      set_gdbarch_frame_align (gdbarch, hppa32_frame_align);
      break;
    case 8:
      set_gdbarch_push_dummy_call (gdbarch, hppa64_push_dummy_call);
      set_gdbarch_frame_align (gdbarch, hppa64_frame_align);
      break;
    default:
      internal_error (__FILE__, __LINE__, "bad switch");
    }
      
  /* Struct return methods.  */
  switch (tdep->bytes_per_address)
    {
    case 4:
      set_gdbarch_return_value (gdbarch, hppa32_return_value);
      break;
    case 8:
      set_gdbarch_return_value (gdbarch, hppa64_return_value);
      break;
    default:
      internal_error (__FILE__, __LINE__, "bad switch");
    }
      
  /* Frame unwind methods.  */
  set_gdbarch_unwind_dummy_id (gdbarch, hppa_unwind_dummy_id);
  set_gdbarch_unwind_pc (gdbarch, hppa_unwind_pc);
  frame_unwind_append_sniffer (gdbarch, hppa_frame_unwind_sniffer);
  frame_base_append_sniffer (gdbarch, hppa_frame_base_sniffer);

  /* Hook in ABI-specific overrides, if they have been registered.  */
  gdbarch_init_osabi (info, gdbarch);

  return gdbarch;
}

static void
hppa_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
   /* Nothing to print for the moment.  */
}

void
_initialize_hppa_tdep (void)
{
  struct cmd_list_element *c;
  void break_at_finish_command (char *arg, int from_tty);
  void tbreak_at_finish_command (char *arg, int from_tty);
  void break_at_finish_at_depth_command (char *arg, int from_tty);

  gdbarch_register (bfd_arch_hppa, hppa_gdbarch_init, hppa_dump_tdep);

  add_cmd ("unwind", class_maintenance, unwind_command,
	   "Print unwind table entry at given address.",
	   &maintenanceprintlist);

  deprecate_cmd (add_com ("xbreak", class_breakpoint, 
			  break_at_finish_command,
			  concat ("Set breakpoint at procedure exit. \n\
Argument may be function name, or \"*\" and an address.\n\
If function is specified, break at end of code for that function.\n\
If an address is specified, break at the end of the function that contains \n\
that exact address.\n",
		   "With no arg, uses current execution address of selected stack frame.\n\
This is useful for breaking on return to a stack frame.\n\
\n\
Multiple breakpoints at one place are permitted, and useful if conditional.\n\
\n\
Do \"help breakpoints\" for info on other commands dealing with breakpoints.", NULL)), NULL);
  deprecate_cmd (add_com_alias ("xb", "xbreak", class_breakpoint, 1), NULL);
  deprecate_cmd (add_com_alias ("xbr", "xbreak", class_breakpoint, 1), NULL);
  deprecate_cmd (add_com_alias ("xbre", "xbreak", class_breakpoint, 1), NULL);
  deprecate_cmd (add_com_alias ("xbrea", "xbreak", class_breakpoint, 1), NULL);

  deprecate_cmd (c = add_com ("txbreak", class_breakpoint, 
			      tbreak_at_finish_command,
"Set temporary breakpoint at procedure exit.  Either there should\n\
be no argument or the argument must be a depth.\n"), NULL);
  set_cmd_completer (c, location_completer);
  
  if (xdb_commands)
    deprecate_cmd (add_com ("bx", class_breakpoint, 
			    break_at_finish_at_depth_command,
"Set breakpoint at procedure exit.  Either there should\n\
be no argument or the argument must be a depth.\n"), NULL);
}

