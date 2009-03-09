/* Main simulator entry points specific to the ARC.
   Copyright (C) 1996, 1997, 1998, 1999, 2003, 2004, 2005, 2006, 2007, 2008
   Free Software Foundation, Inc.

   This file is part of GDB, the GNU debugger.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sim-main.h"
#include "sim-options.h"
#include "libiberty.h"
#include "bfd.h"

#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

static void free_state (SIM_DESC);
static void print_arc_misc_cpu (SIM_CPU *cpu, int verbose);

/* Records simulator descriptor so utilities like arc_dump_regs can be
   called from gdb.  */
SIM_DESC current_state;

/* Cover function of sim_state_free to free the cpu buffers as well.  */

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}

/* Find out heap and stack end boundaries, and return required memory
   size; if this cannot be calcualted, return DEFAULT_MEMSIZE.
   If INIT_P is nonzero, initialize memory above the stack with argv and envp.
   If this initialization fails, return 0.  */
static int
init_stack (struct bfd *abfd, char **argv,
	    SIM_DESC sd, int default_memsize, int init_p)
{
  USI stack_start = 0, stack_top = 0, heap_start = 0, heap_end = 0;
  USI prog_end = 0;
  char *null_env = NULL;
  char **envp = &null_env;
  int argc;
  char **cpp, **rpp, *str;
  size_t len;
  int cp, wpp;
  int n_ptr = 0, cnt;
  /* All CPUs have the same memory map, apparently.  */
  SIM_CPU *cpu = STATE_CPU (sd, 0);
  bfd_byte buf[4];

  if (abfd)
    {
      asection *s;

      for (s = abfd->sections; s; s = s->next)
	if (strcmp (bfd_get_section_name (abfd, s), ".stack") == 0)
	  {
	    stack_start = bfd_get_section_vma (abfd, s);
	    stack_top = stack_start + bfd_section_size (abfd, s);
	    stack_top &= -4; /* 4 == target pointer size */
	    default_memsize = stack_top;
	  }
	else if (strcmp (bfd_get_section_name (abfd, s), ".heap") == 0)
	  {
	    heap_start = bfd_get_section_vma (abfd, s);
	    heap_end = heap_start + bfd_section_size (abfd, s);
	  }
	else
	  {
	    USI s_end
	      = bfd_get_section_vma (abfd, s) + bfd_section_size (abfd, s);

	    if (prog_end < s_end)
	      prog_end = s_end;
	  }
      if (heap_end == 0)
	{
	  if (prog_end > stack_start)
	    return 0;
	  heap_start = prog_end;
	  heap_end = stack_start;
	}
      if (!argv)
	n_ptr == 1;
      else
	{
	  for (cpp = envp, len = 0, cnt = 2; cnt--; cpp = argv)
	    {
	      argc = 0;
	      for (rpp = cpp; *rpp; rpp++)
		argc++, len += strlen (*rpp) + 1;
	      n_ptr += argc + 1;
	    }
	}
      n_ptr ++; //* For uclibc aux_dat */
      if (!stack_top)
	stack_top = (default_memsize + 3) & -4; /* 4 == target pointer size */
      wpp = stack_top + 4;
      cp = wpp + n_ptr * 4;
      default_memsize = cp + len;
      /* Round up to multiple of 32.  strlen expects memory to come in chunks
	 that are at least cache-line (32 bytes) sized.  */
      default_memsize += 31;
      default_memsize &= -32;
    }
  if (abfd && init_p)
    {
      int little_endian_p = bfd_little_endian (abfd);

      if (stack_top <= stack_start)
	{
	  host_callback *callback = STATE_CALLBACK (sd);

	  (*callback->printf_filtered) (callback, "stack overflow\n");
	  return 0;
	}

      /* Can't use sim_core_write_unaligned_4 without everything
         initialized when tracing, and then these writes would get into
         the trace.  */
#define write_dword(addr, data)						\
 do									\
   {									\
     USI data_ = data;							\
     USI addr_ = addr;							\
     if (little_endian_p)						\
       bfd_putl32 (data_, buf); 					\
     else								\
       bfd_putb32 (data_, buf); 					\
     if (sim_core_write_buffer (sd, cpu, 0, buf, addr_, 4) != 4)	\
        return 0;							\
   }									\
 while (0)

      write_dword (stack_top, argc);
      for (cpp = argv, cnt = 2; cnt--; cpp = envp)
	{
	  for (rpp = cpp; str = *cpp++;)
	    {
	      len = strlen (str) + 1;
	      if (sim_core_write_buffer (sd, cpu, 0, str, cp, len) != len)
		return 0;
	      write_dword (wpp, cp);
	      cp += len;
	      wpp += 4;
	    }
	  write_dword (wpp, 0);
	  wpp += 4;
	}
      write_dword (wpp, 0);
      sd->heap_start = heap_start;
      sd->heap_end = heap_end;
      sd->stack_top = stack_top;
    }
  return default_memsize;
}

/* Create an instance of the simulator.  */

SIM_DESC
sim_open (kind, callback, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *callback;
     struct bfd *abfd;
     char **argv;
{
  SIM_DESC sd = sim_state_alloc (kind, callback);
  char c;
  int i;
  int default_memsize;
  char ** prog_argv;

  /* The cpu data is kept in a separately allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, cgen_cpu_max_extra_bytes ()) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

#if 0 /* FIXME: pc is in mach-specific struct */
  /* FIXME: watchpoints code shouldn't need this */
  {
    SIM_CPU *current_cpu = STATE_CPU (sd, 0);
    STATE_WATCHPOINTS (sd)->pc = &(PC);
    STATE_WATCHPOINTS (sd)->sizeof_pc = sizeof (PC);
  }
#endif

  if (sim_pre_argv_init (sd, argv[0]) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

#ifdef HAVE_DV_SOCKSER /* FIXME: was done differently before */
  if (dv_sockser_install (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }
#endif

#if 0 /* FIXME: 'twould be nice if we could do this */
  /* These options override any module options.
     Obviously ambiguity should be avoided, however the caller may wish to
     augment the meaning of an option.  */
  if (extra_options != NULL)
    sim_add_option_table (sd, extra_options);
#endif

  /* getopt will print the error message so we just have to exit if this fails.
     FIXME: Hmmm...  in the case of gdb we need getopt to call
     print_filtered.  */
  if (sim_parse_args (sd, argv) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Check for/establish the reference program image, and set arch info.  */
  if (sim_analyze_program (sd,
			   (STATE_PROG_ARGV (sd) != NULL
			    ? *STATE_PROG_ARGV (sd)
			    : NULL),
			   abfd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  prog_argv = STATE_PROG_ARGV (sd);
  if (!abfd)
    {
      char *name;

      /* FIXME: If the test below fails, we will use ARC_DEFAULT_MEMSIZE.
	 We should have some way to remember this, so that we can
	 emit an error in sim_create_inferior if the required memory
	 size is larger.  */
      if (prog_argv != NULL && *prog_argv != NULL)
	{
	  name = *prog_argv;
	  abfd = bfd_openr (name, 0);
	  if (abfd == NULL || !bfd_check_format (abfd, bfd_object))
	    {
	      free_state (sd);
	      return 0;
	    }
	}
    }

  default_memsize
    = init_stack (abfd, prog_argv, sd, ARC_DEFAULT_MEM_SIZE, 0);
  /* Allocate core managed memory if none specified by user.
     Use address 4 here in case the user wanted address 0 unmapped.  */
  if (sim_core_read_buffer (sd, NULL, read_map, &c, 4, 1) == 0)
    sim_do_commandf (sd, "memory region 0,0x%x", default_memsize);

  if (!init_stack (abfd, prog_argv, sd, default_memsize, 1)
      /* Establish any remaining configuration options.  */
      || sim_config (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Open a copy of the cpu descriptor table.  */
  {
    CGEN_CPU_DESC cd
      = arc_cgen_cpu_open_1 (STATE_ARCHITECTURE (sd)->printable_name,
			     (abfd && bfd_little_endian (abfd)
			      ? CGEN_ENDIAN_LITTLE: CGEN_ENDIAN_BIG));
    for (i = 0; i < MAX_NR_PROCESSORS; ++i)
      {
	SIM_CPU *cpu = STATE_CPU (sd, i);
	CPU_CPU_DESC (cpu) = cd;
	CPU_DISASSEMBLER (cpu) = sim_cgen_disassemble_insn;
      }
    arc_cgen_init_dis (cd);
  }

  /* Initialize various cgen things not done by common framework.
     Must be done after arc_cgen_cpu_open.  */
  cgen_init (sd);

  for (c = 0; c < MAX_NR_PROCESSORS; ++c)
    {
      /* Only needed for profiling, but the structure member is small.  */
      memset (CPU_ARC_MISC_PROFILE (STATE_CPU (sd, i)), 0,
	      sizeof (* CPU_ARC_MISC_PROFILE (STATE_CPU (sd, i))));
      /* Hook in callback for reporting these stats */
      PROFILE_INFO_CPU_CALLBACK (CPU_PROFILE_DATA (STATE_CPU (sd, i)))
	= print_arc_misc_cpu;
    }

  /* Store in a global so things like sparc32_dump_regs can be invoked
     from the gdb command line.  */
  current_state = sd;

  return sd;
}

void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  arc_cgen_cpu_close (CPU_CPU_DESC (STATE_CPU (sd, 0)));
  sim_module_uninstall (sd);
}

SIM_RC
sim_create_inferior (sd, abfd, argv, envp)
     SIM_DESC sd;
     struct bfd *abfd;
     char **argv;
     char **envp;
{
  SIM_CPU *current_cpu = STATE_CPU (sd, 0);
  SIM_ADDR addr;

  if (abfd != NULL)
    addr = bfd_get_start_address (abfd);
  else
    addr = 0;
  sim_pc_set (current_cpu, addr);
  /* Initialize stack pointer.  */
  if (!sd->stack_top
      && !init_stack (abfd, argv, sd, 1, 1))
    return 0;
  current_cpu->endbrk = sd->heap_start;

  a5f_h_cr_set (current_cpu, 28, (sd)->stack_top);
  /* Set r0/r1 to argc / argv */
  a5f_h_cr_set (current_cpu, 0, GETMEMSI (current_cpu, 0, sd->stack_top));
  a5f_h_cr_set (current_cpu, 1, sd->stack_top+4);

#ifdef ARC_LINUX
  m32rbf_h_cr_set (current_cpu,
                    arc_decode_gdb_ctrl_regnum(SPI_REGNUM), 0x1f00000);
  m32rbf_h_cr_set (current_cpu,
                    arc_decode_gdb_ctrl_regnum(SPU_REGNUM), 0x1f00000);
#endif

#if 0
  STATE_ARGV (sd) = sim_copy_argv (argv);
  STATE_ENVP (sd) = sim_copy_argv (envp);
#endif

  return SIM_RC_OK;
}

/* PROFILE_CPU_CALLBACK */

static void
print_arc_misc_cpu (SIM_CPU *cpu, int verbose)
{
  SIM_DESC sd = CPU_STATE (cpu);
  char buf[20];

  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_INSN_IDX])
    {
      sim_io_printf (sd, "Miscellaneous Statistics\n\n");
      sim_io_printf (sd, "  %-*s %s\n\n",
		     PROFILE_LABEL_WIDTH, "Fill nops:",
		     sim_add_commas (buf, sizeof (buf),
				     CPU_ARC_MISC_PROFILE (cpu)->fillnop_count));
      if (STATE_ARCHITECTURE (sd)->mach == bfd_mach_m32rx)
	sim_io_printf (sd, "  %-*s %s\n\n",
		       PROFILE_LABEL_WIDTH, "Parallel insns:",
		       sim_add_commas (buf, sizeof (buf),
				       CPU_ARC_MISC_PROFILE (cpu)->parallel_count));
      if (STATE_ARCHITECTURE (sd)->mach == bfd_mach_m32r2)
	sim_io_printf (sd, "  %-*s %s\n\n",
		       PROFILE_LABEL_WIDTH, "Parallel insns:",
		       sim_add_commas (buf, sizeof (buf),
				       CPU_ARC_MISC_PROFILE (cpu)->parallel_count));
    }
}

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{ 
  char **argv;

  if (cmd == NULL)
    return;

  argv = buildargv (cmd);

  if (argv[0] != NULL
      && strcasecmp (argv[0], "info") == 0
      && argv[1] != NULL
      && strncasecmp (argv[1], "reg", 3) == 0)
    {
      SI val;

      /* We only support printing bbpsw,bbpc here as there is no equivalent
	 functionality in gdb.  */
      if (argv[2] == NULL)
	sim_io_eprintf (sd, "Missing register in `%s'\n", cmd);
      else if (argv[3] != NULL)
	sim_io_eprintf (sd, "Too many arguments in `%s'\n", cmd);
#if 0
      else if (strcasecmp (argv[2], "bbpsw") == 0)
	{
	  val = m32rbf_h_cr_get (STATE_CPU (sd, 0), H_CR_BBPSW);
	  sim_io_printf (sd, "bbpsw 0x%x %d\n", val, val);
	}
      else if (strcasecmp (argv[2], "bbpc") == 0)
	{
	  val = m32rbf_h_cr_get (STATE_CPU (sd, 0), H_CR_BBPC);
	  sim_io_printf (sd, "bbpc 0x%x %d\n", val, val);
	}
#endif
      else
	sim_io_eprintf (sd, "Printing of register `%s' not supported with `sim info'\n",
			argv[2]);
    }
  else
    {
      if (sim_args_command (sd, cmd) != SIM_RC_OK)
	sim_io_eprintf (sd, "Unknown sim command `%s'\n", cmd);
    }

  freeargv (argv);
}
