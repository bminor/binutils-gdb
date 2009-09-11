/* Main simulator entry points specific to the ARC.
   Copyright (C) 1996, 1997, 1998, 1999, 2003, 2004, 2005, 2006, 2007, 2008,
   2009 Free Software Foundation, Inc.

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


/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module provides operations to:                                    */
/*                                                                            */
/*        1) create an "instance" of the ARC simulator                        */
/*        2) destroy an "instance" of the ARC simulator                       */
/*        3) create an "inferior" to run on the simulator                     */
/*        4) execute a simulator-related command                              */
/*                                                                            */
/*     N.B. there can be only one simulator instance at a time because of the */
/*          use of the current_state global variable.                         */
/*                                                                            */
/*     This module is also responsible for setting up the "command line"      */
/*     arguments and environment (name/value pairs) to be passed to the       */
/*     program that is to be debugged. This data is passed on the stack above */
/*     the top of stack as it is known to the program.                        */
/*                                                                            */
/*     E.g. if we are passing 4 arguments to main, we must place the data on  */
/*          the stack as:                                                     */
/*                                                                            */
/*             .                                                              */
/*             .                                                              */
/*          stack[top + A3] <== <arg_3>                                       */
/*             .                                                              */
/*             .                                                              */
/*          stack[top + A2] <== <arg_2>                                       */
/*             .                                                              */
/*             .                                                              */
/*          stack[top + A1] <== <arg_1>                                       */
/*             .                                                              */
/*             .                                                              */
/*          stack[top + A0] <== <arg_0>                                       */
/*          stack[top + 28] <== 0x0           # ? NULL terminator             */
/*          stack[top + 24] <== 0x0           # envp NULL terminator          */
/*          stack[top + 20] <== 0x0           # argv NULL terminator          */
/*          stack[top + 16] <== TOP + A3      # argv[3]                       */
/*          stack[top + 12] <== TOP + A2      # argv[2]                       */
/*          stack[top +  8] <== TOP + A1      # argv[1]                       */
/*          stack[top +  4] <== TOP + A0      # argv[0]                       */
/*          stack[top     ] <== 0x4           # argc                          */
/*                                                                            */
/*     where TOP = &stack[top]                                                */
/*       and A0 .. A3 are the offsets of the stored arguments from the stack  */
/*       top.                                                                 */
/*                                                                            */
/*                                                                            */
/* Notes:                                                                     */
/*     The interface to this module is somewhat muddled: both sim_open and    */
/*     sim_create_inferior have a BFD parameter which denotes the executable  */
/*     file which is to be run; but for sim_open this parameter may be NULL.  */
/*                                                                            */
/*     Also, both sim_open and sim_create_inferior have an argv parameter,    */
/*     but for sim_open *some* of the strings in this array parameter may be  */
/*     command-line arguments to be passed to the program to be executed,     */
/*     whilst the others are arguments for the simulator itself; but for      */
/*     sim_create_inferior, *all* of the strings (if any) in this parameter   */
/*     are such command-line arguments.                                       */
/*                                                                            */
/*     Finally, sim_create_inferior has an envp parameter which holds a set   */
/*     of name/value pairs to be passed as the environment of the executed    */
/*     program, whereas sim_open does not; but this is not (currently) done.  */
/*                                                                            */
/*                                                                            */
/*     This complexity arises from the variety of ways in which a simulator   */
/*     instance may be created and used:                                      */
/*                                                                            */
/*       a) the instance may be created by gdb before the executable file is  */
/*          known; e.g. the user may issue the commands                       */
/*                                                                            */
/*              set endian big | little                                       */           
/*              target sim                                                    */
/*              file <executable>                                             */
/*              load                                                          */
/*              run                                                           */
/*                                                                            */
/*          and in this case the BFD parameter to sim_open is NULL, and its   */
/*          argv parameter does not hold the executable's arguments;          */
/*                                                                            */
/*       b) the instance may be created by gdb after the executable file is   */
/*          known; e.g. the user may issue the commands                       */
/*                                                                            */
/*              file <executable>                                             */
/*              target sim                                                    */
/*              load                                                          */
/*              run                                                           */
/*                                                                            */
/*          and in this case the BFD parameter to sim_open is non-NULL, and   */
/*          its argv parameter does not hold the executable's arguments;      */
/*                                                                            */
/*       c) the instance may be created by a standalone executable; e.g.      */
/*          known; e.g. the user may issue the command                        */
/*                                                                            */
/*              run <executable> [ <arg1> .. <argN> ]                         */
/*                                                                            */
/*          and in this case the BFD parameter to sim_open is non-NULL, and   */
/*          its argv parameter does hold the executable's arguments.          */
/*                                                                            */
/*     Note that in each case, the argv parameter to sim_create_inferior      */
/*     holds the command-line arguments for the executable (so, there is      */
/*     apparently no need for them to be passed in the sim_open argv also!).  */
/*                                                                            */
/*     The arguments to the executed program may also be specified in several */
/*     ways; e.g by use of the gdb 'set args' command, or as arguments to the */
/*     gdb 'run' command; or, in the case of a standalone executable, on the  */
/*     command line.                                                          */
/*                                                                            */
/*     Note that the executable program can be run (by gdb) repeatedly with   */
/*     different argument lists, i.e. there may be multiple calls of          */
/*     sim_create_inferior after a call of sim_open; and each of these lists  */
/*     may require a different amount of memory to hold them on the stack.    */
/*                                                                            */
/*                                                                            */
/*     It is necessary to define a memory region with a particular start      */
/*     address and size in the simulator memory (all accesses outside this    */
/*     address range by the executed program are erroneous, thus allowing     */
/*     invalid accesses to be detected); this region must include the program */
/*     text, (un)initialized data, heap and stack.  This must also include    */
/*     the argument/environment data which is passed above the top of the     */
/*     stack.                                                                 */
/*                                                                            */
/*     If the executable file is known (i.e. we have a non-NULL BFD),         */
/*     the bounds of the memory region required for the program may be        */
/*     extracted from the section header information in the file; if the      */
/*     arguments/environment are known, the space required to hold them on    */
/*     the stack may be computed. This allows the total space to be computed, */
/*     and so a memory region holding it may be defined.                      */
/*                                                                            */
/*     However, if a subsequent execution of the program should require a     */
/*     greater total space (because its arguments have changed) it is then    */
/*     necessary to define an additional memory region to provide the extra   */
/*     space; it is not possible to redefine the existing region, or a new    */
/*     region of the new size (as memory regions may not overlap).            */
/*                                                                            */
/******************************************************************************/

/* system header files */

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

/* simulator / binutils header files */

#include "sim-main.h"
#include "sim-options.h"
#include "libiberty.h"
#include "bfd.h"


/* -------------------------------------------------------------------------- */
/*                         conditional compilation flags                      */
/* -------------------------------------------------------------------------- */

//#define LOGGING
//#define PUSH_ENVIRONMENT_ONTO_STACK


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* Records simulator descriptor so utilities like arc_dump_regs can be called
   from gdb.  */
SIM_DESC current_state;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define DEFAULT_ENVIRONMENT_SPACE     2048

#define TARGET_POINTER_SIZE    4
#define TARGET_INT_SIZE        4


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

#define IS_LITTLE_ENDIAN(abfd)   (abfd) ? bfd_little_endian (abfd) \
                                        : (CURRENT_TARGET_BYTE_ORDER == LITTLE_ENDIAN)


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* Cover function of sim_state_free to free the cpu buffers as well.  */

static void
free_state (SIM_DESC sd)
{
  if (STATE_MODULES (sd) != NULL)
    sim_module_uninstall (sd);
  sim_cpu_free_all (sd);
  sim_state_free (sd);
}


/* PROFILE_CPU_CALLBACK */

static void
print_arc_misc_cpu (SIM_CPU *cpu, int verbose)
{
  if (CPU_PROFILE_FLAGS (cpu) [PROFILE_INSN_IDX])
    {
      SIM_DESC sd = CPU_STATE (cpu);
      char     buf[20];

      sim_io_printf (sd, "Miscellaneous Statistics\n\n");
      sim_io_printf (sd, "  %-*s %s\n\n",
		     PROFILE_LABEL_WIDTH, "Fill nops:",
		     sim_add_commas (buf, sizeof (buf),
				     CPU_ARC_MISC_PROFILE (cpu)->fillnop_count));
    }
}


#ifdef LOGGING
static void
dump (SIM_DESC sd, char** argv, char** envp)
{
  char **cpp, **rpp;
  char* tag;
  int   cnt;

  for (cpp = argv, tag = "argv", cnt = 2; cnt--; cpp = envp, tag = "env")
    {
      if (cpp)
        {
           int i = 0;

           for (rpp = cpp; *rpp; rpp++, i++)
               sim_io_eprintf(sd, "%s[%d] = %s\n", tag, i, *rpp);
        }
    }
}
#endif

/* Setup copy arguments / environment to the stack area of SD according
   to argv and envp.  Either or both of argv and envp may be NULL.
   Return 0 on failure, nonzero on success.  */

static int
setup_stack (SIM_DESC sd, int little_endian_p, char** argv, char** envp)
{
  SIM_CPU* cpu = STATE_CPU (sd, 0);
  int      wpp = sd->memory.stack_top + TARGET_INT_SIZE;
  int      cp  = sd->memory.argument_data_start;
  int      count;
  char**   cpp;

#ifdef LOGGING
  sim_io_eprintf(sd, "setup stack (%c/E) %d args\n",
                     (little_endian_p) ? 'L' : 'B',
                     sd->memory.num_arguments);
  dump(sd, argv, envp);
#endif

  if (sd->memory.stack_top <= sd->memory.stack_start)
    {
      host_callback *callback = STATE_CALLBACK (sd);

      (*callback->printf_filtered) (callback, "stack overflow\n");
      return 0;
    }

  /* Can't use sim_core_write_unaligned_4 without everything
     initialized when tracing, and then these writes would get into
     the trace.  */
#define write_dword(addr, data)                                              \
 do                                                                          \
   {                                                                         \
     bfd_byte buf[4];                                                        \
     USI data_ = data;                                                       \
     USI addr_ = addr;                                                       \
     if (little_endian_p)                                                    \
       bfd_putl32 (data_, buf);                                              \
     else                                                                    \
       bfd_putb32 (data_, buf);                                              \
     if (sim_core_write_buffer (sd, cpu, 0, buf, addr_, 4) != 4)             \
        {                                                                    \
          sim_io_eprintf(sd, "failed to write word to address 0x%x\n", addr);\
          return 0;                                                          \
        }                                                                    \
   }                                                                         \
 while (0)


 /* Push the arguments and environment onto the stack.  */

  write_dword (sd->memory.stack_top, sd->memory.num_arguments);
  for (cpp = argv, count = 2; count--; cpp = envp)
    {
      if (cpp)
        {
          char*  argument;
          char** rpp;

          for (rpp = cpp; (argument = *cpp); cpp++)
            {
              size_t len = strlen(argument) + 1;

#ifdef LOGGING
              sim_io_eprintf(sd, "pushing argument '%s' onto program stack at address 0x%x\n", argument, cp);
#endif

              if (sim_core_write_buffer (sd, cpu, 0, argument, cp, len) != len)
                {
                  sim_io_eprintf(sd, "could not push argument '%s' onto program stack at address 0x%x\n", argument, cp);
                  return 0;
                }
              write_dword (wpp, cp);
              cp += len;
              wpp += TARGET_POINTER_SIZE;
            }
        }

      write_dword (wpp, 0);         // NULL array terminator
      wpp += TARGET_POINTER_SIZE;

    }
  write_dword (wpp, 0); /* uClibc aux_dat NULL terminator.  */

  /* success */
  return 1;
}


/* Find out heap and stack end boundaries, and calculate required memory size;
   if this cannot be done, use the default memory size.
   ABFD, ARGV and/or ENVP may be NULL.
   If this calculation fails, return 0.  */

static int
determine_memory_requirements (SIM_DESC sd, struct bfd *abfd, char **argv,
			       char **envp, struct sim_memory* memory)
{
  USI    stack_start           = 0, stack_top = 0, heap_start = 0, heap_end = 0;
  USI    program_end           = 0;
  USI    memory_size;
  USI    data_start;
  int    argc                  = 0;
  size_t total_argument_length = 0;
  int    num_pointers          = 0;

  /* N.B. the *_end variables actually denote the next byte after the end of
   the sections to which they refer; however, they are used consistently
   in this way, so that is not a problem!  */

  /* If we have an executable file to look at.  */
  if (abfd)
    {
      asection* section;

      /* Look at each section in the file.  */
      for (section = abfd->sections; section; section = section->next)
        {
          if (strcmp (bfd_get_section_name (abfd, section), ".stack") == 0)
            {
              stack_start = bfd_get_section_vma (abfd, section);
              stack_top   = stack_start + bfd_section_size (abfd, section);
              stack_top  &= -TARGET_POINTER_SIZE;

              /* N.B. this assumes that the target memory region to be
		 created has the range 0 .. stack_top - 1, i.e. that there
		 is nothing in the program that has to be loaded at
		 addresses above the stack top.  */
              memory_size = stack_top;
            }
          else if (strcmp (bfd_get_section_name (abfd, section), ".heap") == 0)
            {
              heap_start = bfd_get_section_vma (abfd, section);
              heap_end   = heap_start + bfd_section_size (abfd, section);
            }
          else if (bfd_get_section_flags(abfd, section) & SEC_LOAD)
            {
              USI section_end = bfd_get_section_vma (abfd, section) +
                                bfd_section_size    (abfd, section);

              /* Keep track of the end address of whatever else is to be
		 loaded.  */
              if (program_end < section_end)
                program_end = section_end;
            }
        }

       /* If we know the start of the stack, check for an overlap.  */ 
      if (stack_start > 0 && program_end > stack_start)
	{
	  sim_io_eprintf(sd,
			 "program data overlaps program stack at 0x%x (0x%x)\n",
			 stack_start, program_end);
	  return 0;
	}

       /* No heap section found?  */
      if (heap_end == 0)
        {
          /* Assume the heap lies between the program data and the stack.  */
          heap_start = program_end;
          heap_end   = stack_start;
        }
    }


  /* If we have arguments or environment variables to pass to the program.  */

  if (argv)
    {
      char **rpp;

      for (rpp = argv; *rpp; rpp++, num_pointers++)
         total_argument_length += strlen (*rpp) + 1; 

       argc = num_pointers;
    }
  num_pointers++; /* For NULL terminator.  */

  if (envp)
    {
      char** rpp;

      for (rpp = envp; *rpp; rpp++, num_pointers++)
         total_argument_length += strlen (*rpp) + 1;   
    }
  num_pointers++;   /* For NULL terminator.  */

  num_pointers++;   /* For uclibc aux_dat.  */

  /* If we could not get the stack bounds from the executable file (and hence
     do not know the memory size), use the default memory size and take the
     top of the stack as being at the end of that memory range (aligned to
     the size of a target pointer).  */
  if (!stack_top)
    stack_top = (USI) ((ARC_DEFAULT_MEM_SIZE + 3) & -TARGET_POINTER_SIZE);

  data_start = (USI) (stack_top                          +
                      TARGET_INT_SIZE                    +  /* For argc.  */
                      num_pointers * TARGET_POINTER_SIZE);

  /* We need enough memory to hold the program stack, plus the data to be passed
     above the top of the stack.  */
  memory_size = (USI) (data_start + total_argument_length);

#ifdef PUSH_ENVIRONMENT_ONTO_STACK
  if (!envp)
     memory_size += DEFAULT_ENVIRONMENT_SPACE;
#endif

  /* Round up to multiple of 32: strlen expects memory to come in chunks
     that are at least cache-line (32 bytes) sized.
     FIXME: is that true?  */
  memory_size += 31;
  memory_size &= -32;

  memory->heap_start          = heap_start;
  memory->heap_end            = heap_end;
  memory->stack_top           = stack_top;
  memory->stack_start         = stack_start;
  memory->total_size          = memory_size;
  memory->argument_data_start = data_start;
  memory->num_arguments       = (USI) argc;

#ifdef LOGGING
  sim_io_eprintf(sd, "heap  start   : 0x%x\n", heap_start);
  sim_io_eprintf(sd, "heap  end     : 0x%x\n", heap_end);
  sim_io_eprintf(sd, "stack start   : 0x%x\n", stack_start);
  sim_io_eprintf(sd, "stack top     : 0x%x\n", stack_top);
  sim_io_eprintf(sd, "memory size   : 0x%x\n", memory_size);
  sim_io_eprintf(sd, "arg data start: 0x%x\n", data_start);
  sim_io_eprintf(sd, "%d args\n", argc);
#endif

  /* Success.  */
  return 1;
}


static void
define_memory_region (SIM_DESC sd, USI start, USI size)
{ 
  /* Allocate core managed memory if none specified by user.  */
  if (!sd->memory_regions_defined_by_user)
    {
#ifdef LOGGING
      sim_io_printf   (sd, "memory region 0x%x,0x%x\n", start, size);
#endif
      sim_do_commandf (sd, "memory region 0x%x,0x%x",   start, size);
    }
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Create an instance of the simulator.  */

SIM_DESC
sim_open (SIM_OPEN_KIND  kind,
          host_callback *callback,
          struct bfd    *abfd,      /* May be NULL.  */
          char         **argv)
{
  SIM_DESC sd = sim_state_alloc (kind, callback);
  int      little_endian_p;
  char**   prog_argv;
  char     c;

  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);

#ifdef LOGGING
  sim_io_eprintf (sd, "sim_open: %p\n", abfd);
  dump(sd, argv, NULL);
#endif

  /* The cpu data is kept in a separately-allocated chunk of memory.  */
  if (sim_cpu_alloc_all (sd, 1, cgen_cpu_max_extra_bytes ()) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

#if 0 /* FIXME: pc is in mach-specific struct.  */
  /* FIXME: watchpoints code shouldn't need this.  */
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

#ifdef HAVE_DV_SOCKSER /* FIXME: was done differently before.  */
  if (dv_sockser_install (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }
#endif

#if 0 /* FIXME: 'twould be nice if we could do this.  */
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

#ifdef LOGGING
  /* sim_parse_args may set up STATE_PROG_ARGV(sd), in the case that kind == SIM_OPEN_STANDALONE.  */
  dump(sd, STATE_PROG_ARGV (sd), NULL);
#endif

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

  /* If we have not been given an executable file.  */
  if (!abfd)
    {
      /* If the test below fails, we will use ARC_DEFAULT_MEM_SIZE. */
      if (prog_argv != NULL && *prog_argv != NULL)
	{
	  char* name = *prog_argv;

	  abfd = bfd_openr (name, 0);

	  if (abfd == NULL || !bfd_check_format (abfd, bfd_object))
	    {
	      free_state (sd);
	      return 0;
	    }
	}
    }

  /* Establish any remaining configuration options.  */
  if (sim_config (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  little_endian_p = IS_LITTLE_ENDIAN(abfd);

  /* Check whether core managed memory has been specified by user.
     Use address 4 here in case the user wanted address 0 unmapped.
     N.B. this is really not a very good check - we want to know whether the
     user has explicitly specified the target's memory regions, so that we
     don't define a region ourself (which might overlap), and we try to
     find that out by seeing if we can read the memory at address 0x4 !!!!  */
  sd->memory_regions_defined_by_user = 
    sim_core_read_buffer (sd, NULL, read_map, &c, 4, 1) != 0;

  /* Calculate the memory size required without actually initializing the stack.  */
  if (!determine_memory_requirements(sd, abfd, prog_argv, NULL, &sd->memory))
    {
      free_state (sd);
      return 0;
    }

  define_memory_region (sd, 0, sd->memory.total_size);

  /* If the simulator is being used stand-alone, we know that the program 
     arguments we have been given here are not going to change later on (when
     sim_create_inferior is called) - so we can go ahead and set up the stack
     with those arguments right now.  */
  if (STATE_OPEN_KIND (sd) == SIM_OPEN_STANDALONE)
    {
      /* Now that we do know the memory size, initialize the stack.  */
      if (!setup_stack (sd, little_endian_p, prog_argv, NULL))
        {
          free_state (sd);
          return 0;
        }
    }

  if (sim_post_argv_init (sd) != SIM_RC_OK)
    {
      free_state (sd);
      return 0;
    }

  /* Open a copy of the cpu descriptor table.  */
  {
    CGEN_CPU_DESC cd = arc_cgen_cpu_open_1
                          (STATE_ARCHITECTURE (sd)->printable_name,
			   (little_endian_p ? CGEN_ENDIAN_LITTLE : CGEN_ENDIAN_BIG));
    int i;

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

  for (c = 0; c < MAX_NR_PROCESSORS; c++)
    {
      /* Only needed for profiling, but the structure member is small.  */
      memset (CPU_ARC_MISC_PROFILE (STATE_CPU (sd, i)), 0,
	      sizeof (* CPU_ARC_MISC_PROFILE (STATE_CPU (sd, i))));

      /* Hook in callback for reporting these stats.  */
      PROFILE_INFO_CPU_CALLBACK (CPU_PROFILE_DATA (STATE_CPU (sd, i))) =
         print_arc_misc_cpu;
    }

  /* Store in a global so things like arc32_dump_regs can be invoked
     from the gdb command line.  */
  current_state = sd;

  return sd;
}


void
sim_close (SIM_DESC sd, int quitting)
{
  arc_cgen_cpu_close (CPU_CPU_DESC (STATE_CPU (sd, 0)));
  sim_module_uninstall (sd);
}


SIM_RC
sim_create_inferior (SIM_DESC sd, struct bfd *abfd, char **argv, char **envp)
{
  SIM_CPU *current_cpu = STATE_CPU (sd, 0);
  SIM_ADDR addr;

#ifndef PUSH_ENVIRONMENT_ONTO_STACK
  envp = NULL;
#endif
#ifdef LOGGING
  sim_io_eprintf (sd, "sim_create_inferior: %p\n", abfd);
  dump (sd, argv, envp);
#endif

  if (abfd != NULL)
    {
      int little_endian_p = bfd_little_endian (abfd);

      if (little_endian_p)
        {
          if (CURRENT_TARGET_BYTE_ORDER == BIG_ENDIAN)
            {
              sim_io_eprintf (sd, "Target is big-endian but executable file %s is little-endian.\n",
			      bfd_get_filename (abfd));
              return SIM_RC_FAIL;
            }
        }
      else
        {
          if (CURRENT_TARGET_BYTE_ORDER == LITTLE_ENDIAN)
            {
              sim_io_eprintf (sd, "Target is little-endian but executable file %s is big-endian.\n",
			      bfd_get_filename(abfd));
              return SIM_RC_FAIL;
            }
        }

      addr = bfd_get_start_address (abfd);
    }
  else
    addr = 0;

  sim_pc_set (current_cpu, addr);

  /* If the simulator is being used stand-alone, we have already set up the
     stack (when sim_open was called); but if the simulator is being used by
     gdb we must set up the stack each time that sim_create_inferior is
     called, in case the program arguments have changed since last time.  */
  if (STATE_OPEN_KIND(sd) == SIM_OPEN_DEBUG)
    {
      struct sim_memory memory;

      if (determine_memory_requirements (sd, abfd, argv, envp, &memory))
        {
          USI current_size = sd->memory.total_size;

          if (memory.total_size > current_size)
            {
#ifdef LOGGING
              sim_io_eprintf (sd, "program requires memory size 0x%x bytes"
                                  " but simulator currently has memory size 0x%x bytes\n",
                                   memory.total_size, current_size);
#endif

              /* Define another memory region to provide the extra memory
		 required.  */
              define_memory_region (sd, current_size,
				    memory.total_size - current_size);

              /* Update all the memory details.  */
              sd->memory = memory;
            }
          else
            {
              /* Update all the memory details *except* the current size.  */
              sd->memory = memory;
              sd->memory.total_size = current_size;
            }

	  if (!setup_stack (sd, IS_LITTLE_ENDIAN (abfd), argv, envp))
	    return SIM_RC_FAIL;
	}
      else
	return SIM_RC_FAIL;
    }

  current_cpu->endbrk = sd->memory.heap_start;

  /* Set r28 (SP) to the stack top, and set r0/r1 to argc/argv. */
  a5f_h_cr_set (current_cpu, 0,  sd->memory.num_arguments);
  a5f_h_cr_set (current_cpu, 1,  sd->memory.stack_top + 4);
  a5f_h_cr_set (current_cpu, 28, sd->memory.stack_top);

#ifdef LOGGING
  sim_io_eprintf (sd, "R0 = 0x%08X\n", sd->memory.num_arguments);
  sim_io_eprintf (sd, "R1 = 0x%08X\n", sd->memory.stack_top + 4);
  sim_io_eprintf (sd, "SP = 0x%08X\n", sd->memory.stack_top);
#endif

#if 0
  STATE_ARGV (sd) = sim_copy_argv (argv);
  STATE_ENVP (sd) = sim_copy_argv (envp);
#endif

  return SIM_RC_OK;
}


void
sim_do_command (SIM_DESC sd, char* cmd)
{
  char **argv;

  if (cmd == NULL)
    return;

  argv = buildargv (cmd);

  /* Is the command 'info reg[ister] <name>' ?  */

  if (argv[0] != NULL
      && strcasecmp (argv[0], "info") == 0
      && argv[1] != NULL
      && strncasecmp (argv[1], "reg", 3) == 0)
    {
      SI val;

      if (argv[2] == NULL)
        sim_io_eprintf (sd, "Missing register in `%s'\n", cmd);
      else if (argv[3] != NULL)
        sim_io_eprintf (sd, "Too many arguments in `%s'\n", cmd);

      /* Should we recognize some/all of the ARC register names here?  */

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

/******************************************************************************/
