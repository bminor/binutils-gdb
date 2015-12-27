/* interp.c -- AArch64 sim interface to GDB.

   Copyright (C) 2015 Free Software Foundation, Inc.

   Contributed by Red Hat.

   This file is part of GDB.

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

#include "config.h"
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "ansidecl.h"
#include "gdb/callback.h"
#include "gdb/remote-sim.h"
#include "gdb/signals.h"
#include "gdb/sim-aarch64.h"

#include "sim-main.h"
#include "sim-options.h"
#include "memory.h"
#include "simulator.h"

#include "dis-asm.h"

static struct disassemble_info  info;
static unsigned long            symcount = 0;
static asymbol **               symtab = NULL;

/* FIXME: 1000 characters should be enough to hold the disassembled
   instruction plus any comments that come after it.  But maybe with
   C++ programs this might not be enough.  Not sure if it is worth
   adding logic to dynamically grow the buffer though.  */
static char opbuf[1000];

static int op_printf (void *, const char *, ...) ATTRIBUTE_FPTR_PRINTF_2;

static int
op_printf (void *stream ATTRIBUTE_UNUSED, const char *fmt, ...)
{
  size_t space_remaining;
  int ret;
  va_list ap;

  space_remaining = sizeof (opbuf) - strlen (opbuf);
  va_start (ap, fmt);
  /* Instead of printing to stream we store the text in opbuf.
     This allows us to use the sim_io_eprintf routine to output
     the text in aarch64_print_insn.  */
  ret = vsnprintf (opbuf + strlen (opbuf), space_remaining, fmt, ap);
  va_end (ap);
  return ret;
}

void
aarch64_print_insn (SIM_DESC sd, uint64_t addr)
{
  int size;

  opbuf[0] = 0;
  size = print_insn_aarch64 (addr, & info);
  sim_io_eprintf (sd, " %*s\n", size, opbuf);
}

static int
sim_dis_read (bfd_vma                     memaddr,
	      bfd_byte *                  ptr,
	      unsigned int                length,
	      struct disassemble_info *   info)
{
  aarch64_get_mem_blk (info->application_data, memaddr, (char *) ptr, length);

  return 0;
}

/* Filter out (in place) symbols that are useless for disassembly.
   COUNT is the number of elements in SYMBOLS.
   Return the number of useful symbols. */

static unsigned long
remove_useless_symbols (asymbol **symbols, unsigned long count)
{
  asymbol **in_ptr  = symbols;
  asymbol **out_ptr = symbols;

  while (count-- > 0)
    {
      asymbol *sym = *in_ptr++;

      if (strstr (sym->name, "gcc2_compiled"))
	continue;
      if (sym->name == NULL || sym->name[0] == '\0')
	continue;
      if (sym->flags & (BSF_DEBUGGING))
	continue;
      if (   bfd_is_und_section (sym->section)
	  || bfd_is_com_section (sym->section))
	continue;
      if (sym->name[0] == '$')
	continue;

      *out_ptr++ = sym;
    }
  return out_ptr - symbols;
}

static signed int
compare_symbols (const void *ap, const void *bp)
{
  const asymbol *a = * (const asymbol **) ap;
  const asymbol *b = * (const asymbol **) bp;

  if (bfd_asymbol_value (a) > bfd_asymbol_value (b))
    return 1;
  if (bfd_asymbol_value (a) < bfd_asymbol_value (b))
    return -1;
  return 0;
}

/* Find the name of the function at ADDR.  */
const char *
aarch64_get_func (uint64_t addr)
{
  int  min, max;

  min = -1;
  max = symcount;
  while (min < max - 1)
    {
      int sym;
      bfd_vma sa;

      sym = (min + max) / 2;
      sa = bfd_asymbol_value (symtab[sym]);

      if (sa > addr)
	max = sym;
      else if (sa < addr)
	min = sym;
      else
	{
	  min = sym;
	  break;
	}
    }

  if (min != -1)
    return bfd_asymbol_name (symtab [min]);

  return "";
}

uint64_t
aarch64_get_sym_value (const char *name)
{
  unsigned long i;

  for (i = 0; i < symcount; i++)
    if (strcmp (bfd_asymbol_name (symtab[i]), name) == 0)
      return bfd_asymbol_value (symtab[i]);

  return 0;
}

SIM_RC
sim_create_inferior (SIM_DESC sd, struct bfd *abfd, char **argv, char **env)
{
  sim_cpu *cpu = STATE_CPU (sd, 0);
  long storage;
  bfd_vma addr = 0;

  if (abfd != NULL)
    addr = bfd_get_start_address (abfd);

  aarch64_set_next_PC (cpu, addr);
  aarch64_update_PC (cpu);

  /* Standalone mode (i.e. `run`) will take care of the argv for us in
     sim_open() -> sim_parse_args().  But in debug mode (i.e. 'target sim'
     with `gdb`), we need to handle it because the user can change the
     argv on the fly via gdb's 'run'.  */
  if (STATE_PROG_ARGV (sd) != argv)
    {
      freeargv (STATE_PROG_ARGV (sd));
      STATE_PROG_ARGV (sd) = dupargv (argv);
    }

  memset (& info, 0, sizeof (info));
  init_disassemble_info (& info, NULL, op_printf);
  info.read_memory_func = sim_dis_read;
  info.arch = bfd_get_arch (abfd);
  info.mach = bfd_get_mach (abfd);
  info.application_data = cpu;
  if (info.mach == 0)
    info.arch = bfd_arch_aarch64;
  disassemble_init_for_target (& info);

  storage = bfd_get_symtab_upper_bound (abfd);
  if (storage > 0)
    {
      symtab = (asymbol **) xmalloc (storage);
      symcount = bfd_canonicalize_symtab (abfd, symtab);
      symcount = remove_useless_symbols (symtab, symcount);
      qsort (symtab, symcount, sizeof (asymbol *), compare_symbols);
    }

  aarch64_init (cpu, bfd_get_start_address (abfd));

  return SIM_RC_OK;
}

/* Read the LENGTH bytes at BUF as a little-endian value.  */

static bfd_vma
get_le (unsigned char *buf, unsigned int length)
{
  bfd_vma acc = 0;

  while (length -- > 0)
    acc = (acc << 8) + buf[length];

  return acc;
}

/* Store VAL as a little-endian value in the LENGTH bytes at BUF.  */

static void
put_le (unsigned char *buf, unsigned int length, bfd_vma val)
{
  int i;

  for (i = 0; i < length; i++)
    {
      buf[i] = val & 0xff;
      val >>= 8;
    }
}

static int
check_regno (int regno)
{
  return 0 <= regno && regno < AARCH64_MAX_REGNO;
}

static size_t
reg_size (int regno)
{
  if (regno == AARCH64_CPSR_REGNO || regno == AARCH64_FPSR_REGNO)
    return 32;
  return 64;
}

static int
aarch64_reg_get (SIM_CPU *cpu, int regno, unsigned char *buf, int length)
{
  size_t size;
  bfd_vma val;

  if (!check_regno (regno))
    return 0;

  size = reg_size (regno);

  if (length != size)
    return 0;

  switch (regno)
    {
    case AARCH64_MIN_GR ... AARCH64_MAX_GR:
      val = aarch64_get_reg_u64 (cpu, regno, 0);
      break;

    case AARCH64_MIN_FR ... AARCH64_MAX_FR:
      val = aarch64_get_FP_double (cpu, regno - 32);
      break;

    case AARCH64_PC_REGNO:
      val = aarch64_get_PC (cpu);
      break;

    case AARCH64_CPSR_REGNO:
      val = aarch64_get_CPSR (cpu);
      break;

    case AARCH64_FPSR_REGNO:
      val = aarch64_get_FPSR (cpu);
      break;

    default:
      sim_io_eprintf (CPU_STATE (cpu),
		      "sim: unrecognized register number: %d\n", regno);
      return -1;
    }

  put_le (buf, length, val);

  return size;
}

static int
aarch64_reg_set (SIM_CPU *cpu, int regno, unsigned char *buf, int length)
{
  size_t size;
  bfd_vma val;

  if (!check_regno (regno))
    return -1;

  size = reg_size (regno);

  if (length != size)
    return -1;

  val = get_le (buf, length);

  switch (regno)
    {
    case AARCH64_MIN_GR ... AARCH64_MAX_GR:
      aarch64_set_reg_u64 (cpu, regno, 1, val);
      break;

    case AARCH64_MIN_FR ... AARCH64_MAX_FR:
      aarch64_set_FP_double (cpu, regno - 32, (double) val);
      break;

    case AARCH64_PC_REGNO:
      aarch64_set_next_PC (cpu, val);
      aarch64_update_PC (cpu);
      break;

    case AARCH64_CPSR_REGNO:
      aarch64_set_CPSR (cpu, val);
      break;

    case AARCH64_FPSR_REGNO:
      aarch64_set_FPSR (cpu, val);
      break;

    default:
      sim_io_eprintf (CPU_STATE (cpu),
		      "sim: unrecognized register number: %d\n", regno);
      return 0;
    }

  return size;
}

static sim_cia
aarch64_pc_get (sim_cpu *cpu)
{
  return aarch64_get_PC (cpu);
}

static void
aarch64_pc_set (sim_cpu *cpu, sim_cia pc)
{
  aarch64_set_next_PC (cpu, pc);
  aarch64_update_PC (cpu);
}

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

enum
{
  OPTION_DISAS = OPTION_START,
};

static SIM_RC
aarch64_option_handler (SIM_DESC  sd ATTRIBUTE_UNUSED,
			sim_cpu * current_cpu ATTRIBUTE_UNUSED,
			int       opt,
			char *    arg ATTRIBUTE_UNUSED,
			int       is_command ATTRIBUTE_UNUSED)
{
  switch (opt)
    {
    case OPTION_DISAS:
      disas = TRUE;
      return SIM_RC_OK;

    default:
      sim_io_eprintf (sd, "Unknown AArch64 option %d\n", opt);
      return SIM_RC_FAIL;
    }
}

static DECLARE_OPTION_HANDLER (aarch64_option_handler);

const OPTION aarch64_options[] =
{
  { {"disas", no_argument, NULL, OPTION_DISAS },
      '\0', NULL, "Enable instruction disassembly",
      aarch64_option_handler, NULL },

  { {NULL, no_argument, NULL, 0}, '\0', NULL, NULL, NULL, NULL }
};

SIM_DESC
sim_open (SIM_OPEN_KIND                  kind,
	  struct host_callback_struct *  callback,
	  struct bfd *                   abfd,
	  char **                        argv)
{
  int i;
  sim_cpu *cpu;
  SIM_DESC sd = sim_state_alloc (kind, callback);

  if (sd == NULL)
    return sd;

  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

  sim_add_option_table (sd, NULL, aarch64_options);

  /* Perform the initialization steps one by one.  */
  if (sim_cpu_alloc_all (sd, 1, 0) != SIM_RC_OK
      || sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK
      || sim_parse_args (sd, argv) != SIM_RC_OK
      || sim_analyze_program (sd,
			      (STATE_PROG_ARGV (sd) != NULL
			       ? *STATE_PROG_ARGV (sd)
			       : NULL), abfd) != SIM_RC_OK
      || sim_config (sd) != SIM_RC_OK
      || sim_post_argv_init (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return NULL;
    }

  aarch64_init_LIT_table ();

  assert (MAX_NR_PROCESSORS == 1);
  cpu = STATE_CPU (sd, 0);
  CPU_PC_FETCH (cpu) = aarch64_pc_get;
  CPU_PC_STORE (cpu) = aarch64_pc_set;
  CPU_REG_FETCH (cpu) = aarch64_reg_get;
  CPU_REG_STORE (cpu) = aarch64_reg_set;

  /* Set SP, FP and PC to 0 and set LR to -1
     so we can detect a top-level return.  */
  aarch64_set_reg_u64 (cpu, SP, 1, 0);
  aarch64_set_reg_u64 (cpu, FP, 1, 0);
  aarch64_set_reg_u64 (cpu, LR, 1, TOP_LEVEL_RETURN_PC);
  aarch64_set_next_PC (cpu, 0);
  aarch64_update_PC (cpu);

  /* Default to a 128 Mbyte (== 2^27) memory space.  */
  sim_do_commandf (sd, "memory-size 0x8000000");

  return sd;
}

void
sim_engine_run (SIM_DESC sd,
		int next_cpu_nr ATTRIBUTE_UNUSED,
		int nr_cpus ATTRIBUTE_UNUSED,
		int siggnal ATTRIBUTE_UNUSED)
{
  aarch64_run (sd);
}
