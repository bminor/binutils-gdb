/* Definitions to target GDB to GNU/Linux on 386.
   Copyright 1992, 1993 Free Software Foundation, Inc.

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

#ifndef TM_LINUX_H
#define TM_LINUX_H

#define I386_GNULINUX_TARGET

#include "i386/tm-i386.h"

/* Size of sigcontext, from <asm/sigcontext.h>.  */
#define LINUX_SIGCONTEXT_SIZE (88)

/* Offset to saved PC in sigcontext, from <asm/sigcontext.h>.  */
#define LINUX_SIGCONTEXT_PC_OFFSET (56)

/* Offset to saved SP in sigcontext, from <asm/sigcontext.h>.  */
#define LINUX_SIGCONTEXT_SP_OFFSET (28)

/* We need this file for the SOLIB_TRAMPOLINE stuff. */

#include "tm-sysv4.h"

/* copy of tm-cygwin32.h */
#undef REGISTER_RAW_SIZE
#undef REGISTER_VIRTUAL_SIZE
#undef REGISTER_VIRTUAL_TYPE
#undef REGISTER_NAMES
#undef REGISTER_BYTES
#undef REGISTER_BYTE
#undef MAX_REGISTER_VIRTUAL_SIZE
#undef NUM_REGS
#undef NUM_FREGS

/* Number of machine registers */

#define NUM_REGS 31
#define NUM_FREGS 15 

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.  */

/* the order of the first 8 registers must match the compiler's 
 * numbering scheme (which is the same as the 386 scheme)
 * also, this table must match regmap in i386-pinsn.c.
 */

#define REGISTER_NAMES { "eax",  "ecx",  "edx",  "ebx",  \
			 "esp",  "ebp",  "esi",  "edi",  \
			 "eip",  "eflags","cs",  "ss",   \
			 "ds",   "es",   "fs",   "gs",   \
			 "cwd",  "swd",  "twd",  "fip",  \
			 "fcs",  "fopo", "fos",          \
			 "st",   "st1",  "st2",  "st3",  \
                         "st4",  "st5",  "st6",  "st7",}

#define LOW_RETURN_REGNUM 0	/* holds low four bytes of result */
#define HIGH_RETURN_REGNUM 2	/* holds high four bytes of result */

#define FPSTART_REGNUM	  16    /* start of FPU registers */
#define FPCONTROL_REGNUM  16	/* FPU control register */
#define FPSTATUS_REGNUM   17	/* FPU status register */
#define FPTAG_REGNUM	  18    /* FPU tag register */
#define FPDATA_REGNUM     23	/* actual floating-point values */
#define FPEND_REGNUM	  (FPSTART_REGNUM + 14)	/* last FPU register */

#define FPENV_BYTES (7 * 4)

#define FPREG_RAW_SIZE (10)

/* Total amount of space needed to store our copies of the machine's
   FPU state.  */

#define FPREG_BYTES (FPENV_BYTES + 8 * FPREG_RAW_SIZE)

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */

#define REGISTER_BYTES (FPSTART_REGNUM * 4 + FPREG_BYTES)

/* Index within `registers' of the first byte of the space for
   register N.  */

#define REGISTER_BYTE(N) (((N) < FPDATA_REGNUM) ? \
			  (N) * 4 : \
			  (((N) - FPDATA_REGNUM) * FPREG_RAW_SIZE) \
			  + (FPDATA_REGNUM * 4))

/* Number of bytes of storage in the actual machine representation
   for register N.  */

#define REGISTER_RAW_SIZE(N) (((N) < FPDATA_REGNUM) ? 4 : FPREG_RAW_SIZE)

/* Number of bytes of storage in the program's representation
   for register N. */

#define REGISTER_VIRTUAL_SIZE(N) (((N) < FPDATA_REGNUM) ? 4 : FPREG_RAW_SIZE)

/* Largest value REGISTER_RAW_SIZE can have.  */

#undef MAX_REGISTER_RAW_SIZE
#define MAX_REGISTER_RAW_SIZE FPREG_RAW_SIZE

/* Largest value REGISTER_VIRTUAL_SIZE can have.  */

#define MAX_REGISTER_VIRTUAL_SIZE FPREG_RAW_SIZE

#if defined(HAVE_LONG_DOUBLE) && defined(HOST_I386)
/* The host and target are i386 machines and the compiler supports
   long doubles. Long doubles on the host therefore have the same
   layout as a 387 FPU stack register. */
#define LD_I387
#endif

#define TARGET_LONG_DOUBLE_BIT 80

#ifdef LD_I387
extern int i387_extract_floating (PTR addr, int len, long double *dretptr);
extern int i387_store_floating   (PTR addr, int len, long double val);

#define TARGET_EXTRACT_FLOATING i387_extract_floating
#define TARGET_STORE_FLOATING   i387_store_floating

#define TARGET_ANALYZE_FLOATING					\
  do								\
    {								\
      unsigned expon;						\
								\
      low = extract_unsigned_integer (valaddr, 4);		\
      high = extract_unsigned_integer (valaddr + 4, 4);		\
      expon = extract_unsigned_integer (valaddr + 8, 2);	\
								\
      nonnegative = ((expon & 0x8000) == 0);			\
      is_nan = ((expon & 0x7fff) == 0x7fff)			\
	&& ((high & 0x80000000) == 0x80000000)			\
	&& (((high & 0x7fffffff) | low) != 0);			\
    }								\
  while (0)
#endif

#ifndef LD_I387
/* Nonzero if register N requires conversion
   from raw format to virtual format.  */
#define REGISTER_CONVERTIBLE(N) \
  ((N < FPDATA_REGNUM) ? 0 : 1)
#endif

#ifdef LD_I387
#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
{ \
  long double val = *((long double *)FROM); \
  store_floating ((TO), TYPE_LENGTH (TYPE), val); \
}
#else
/* Convert data from raw format for register REGNUM in buffer FROM
   to virtual format with type TYPE in buffer TO.  */
extern void
i387_to_double PARAMS ((char *, char *));

#define REGISTER_CONVERT_TO_VIRTUAL(REGNUM,TYPE,FROM,TO) \
{ \
  double val; \
  i387_to_double ((FROM), (char *)&val); \
  store_floating ((TO), TYPE_LENGTH (TYPE), val); \
}
#endif

#ifdef LD_I387
#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
{ \
  long double val = extract_floating ((FROM), TYPE_LENGTH (TYPE)); \
  *((long double *)TO) = val; \
}
#else
extern void
double_to_i387 PARAMS ((char *, char *));

#define REGISTER_CONVERT_TO_RAW(TYPE,REGNUM,FROM,TO) \
{ \
  double val = extract_floating ((FROM), TYPE_LENGTH (TYPE)); \
  double_to_i387((char *)&val, (TO)); \
}
#endif

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

#ifdef LD_I387
#define REGISTER_VIRTUAL_TYPE(N) \
  ((N < FPDATA_REGNUM) ? builtin_type_int : \
   builtin_type_long_double)
#else
#define REGISTER_VIRTUAL_TYPE(N) \
  ((N < FPDATA_REGNUM) ? builtin_type_int : \
   builtin_type_double)
#endif

/* end of copy */

extern void i387_float_info(void);
#define FLOAT_INFO { i387_float_info (); }

/* The following works around a problem with /usr/include/sys/procfs.h  */
#define sys_quotactl 1

/* Define DO_REGISTERS_INFO() to do machine-specific formatting
   of register dumps. */

#define DO_REGISTERS_INFO(_regnum, fp) i386_do_registers_info(_regnum, fp)
extern void i386_do_registers_info PARAMS ((int, int));

extern void i387_print_register PARAMS ((char *, int));

/* When the i386 Linux kernel calls a signal handler, the return
   address points to a bit of code on the stack.  These definitions
   are used to identify this bit of code as a signal trampoline in
   order to support backtracing through calls to signal handlers.  */

#define I386_LINUX_SIGTRAMP
#define IN_SIGTRAMP(pc, name) ((name) == NULL && i386_linux_sigtramp (pc))

extern int i386_linux_sigtramp PARAMS ((CORE_ADDR));

/* We need our own version of sigtramp_saved_pc to get the saved PC in
   a sigtramp routine.  */

#define sigtramp_saved_pc i386_linux_sigtramp_saved_pc
extern CORE_ADDR i386_linux_sigtramp_saved_pc PARAMS ((struct frame_info *));

/* Signal trampolines don't have a meaningful frame.  As in tm-i386.h,
   the frame pointer value we use is actually the frame pointer of the
   calling frame--that is, the frame which was in progress when the
   signal trampoline was entered.  gdb mostly treats this frame
   pointer value as a magic cookie.  We detect the case of a signal
   trampoline by looking at the SIGNAL_HANDLER_CALLER field, which is
   set based on IN_SIGTRAMP.

   When a signal trampoline is invoked from a frameless function, we
   essentially have two frameless functions in a row.  In this case,
   we use the same magic cookie for three frames in a row.  We detect
   this case by seeing whether the next frame has
   SIGNAL_HANDLER_CALLER set, and, if it does, checking whether the
   current frame is actually frameless.  In this case, we need to get
   the PC by looking at the SP register value stored in the signal
   context.

   This should work in most cases except in horrible situations where
   a signal occurs just as we enter a function but before the frame
   has been set up.  */

#define FRAMELESS_SIGNAL(FRAME)					\
  ((FRAME)->next != NULL					\
   && (FRAME)->next->signal_handler_caller			\
   && frameless_look_for_prologue (FRAME))

#undef FRAME_CHAIN
#define FRAME_CHAIN(FRAME)					\
  ((FRAME)->signal_handler_caller				\
   ? (FRAME)->frame						\
    : (FRAMELESS_SIGNAL (FRAME)					\
       ? (FRAME)->frame						\
       : (!inside_entry_file ((FRAME)->pc)			\
	  ? read_memory_integer ((FRAME)->frame, 4)		\
	  : 0)))

#undef FRAME_SAVED_PC
#define FRAME_SAVED_PC(FRAME)					\
  ((FRAME)->signal_handler_caller				\
   ? sigtramp_saved_pc (FRAME)					\
   : (FRAMELESS_SIGNAL (FRAME)					\
      ? read_memory_integer (i386_linux_sigtramp_saved_sp ((FRAME)->next), 4) \
      : read_memory_integer ((FRAME)->frame + 4, 4)))

extern CORE_ADDR i386_linux_sigtramp_saved_sp PARAMS ((struct frame_info *));

/* Some versions of Linux have real-time signal support in the C library, and
   some don't.  We have to include this file to find out.  */
#include <signal.h>

#ifdef __SIGRTMIN
#define REALTIME_LO __SIGRTMIN
#define REALTIME_HI (__SIGRTMAX + 1)
#else
#define REALTIME_LO 32
#define REALTIME_HI 64
#endif

/* When we call a function in a shared library, and the PLT sends us
   into the dynamic linker to find the function's real address, we
   need to skip over the dynamic linker call.  This function decides
   when to skip, and where to skip to.  See the comments for
   SKIP_SOLIB_RESOLVER at the top of infrun.c.  */
#define SKIP_SOLIB_RESOLVER i386_linux_skip_solib_resolver
extern CORE_ADDR i386_linux_skip_solib_resolver (CORE_ADDR pc);

/* N_FUN symbols in shared libaries have 0 for their values and need
   to be relocated. */
#define SOFUN_ADDRESS_MAYBE_MISSING

#endif /* #ifndef TM_LINUX_H */

