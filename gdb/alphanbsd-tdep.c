/* Target-dependent code for NetBSD/Alpha.
   Copyright 2002 Free Software Foundation, Inc.
   Contributed by Wasabi Systems, Inc.

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
#include "gdbcore.h"
#include "value.h"
#include "solib-svr4.h"

#include "alpha-tdep.h"

/* Fetch (and possibly build) an appropriate link_map_offsets
   structure for NetBSD/alpha targets using the struct offsets
   defined in <link.h> (but without actual reference to that file).

   This makes it possible to access NetBSD/alpha shared libraries
   from a GDB that was not built on a NetBSD/alpha host (for cross
   debugging).  */

static struct link_map_offsets *
alphanbsd_solib_svr4_fetch_link_map_offsets (void)
{
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = NULL;

  if (lmp == NULL)
    {
      lmp = &lmo;

      lmo.r_debug_size = 32;

      lmo.r_map_offset = 8;
      lmo.r_map_size   = 8;

      lmo.link_map_size = 40;

      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 8;

      lmo.l_name_offset = 8;
      lmo.l_name_size   = 8;

      lmo.l_next_offset = 24;
      lmo.l_next_size   = 8;

      lmo.l_prev_offset = 32;
      lmo.l_prev_size   = 8;
    }

  return lmp;
}

/* Under NetBSD/alpha, signal handler invocations can be identified by the
   designated code sequence that is used to return from a signal handler.
   In particular, the return address of a signal handler points to the
   following code sequence:

	ldq	a0, 0(sp)
	lda	sp, 16(sp)
	lda	v0, 295(zero)	# __sigreturn14
	call_pal callsys

   Each instruction has a unique encoding, so we simply attempt to match
   the instruction the PC is pointing to with any of the above instructions.
   If there is a hit, we know the offset to the start of the designated
   sequence and can then check whether we really are executing in the
   signal trampoline.  If not, -1 is returned, otherwise the offset from the
   start of the return sequence is returned.  */
static const unsigned int sigtramp_retcode[] =
{
  0xa61e0000,		/* ldq a0, 0(sp) */
  0x23de0010,		/* lda sp, 16(sp) */
  0x201f0127,		/* lda v0, 295(zero) */
  0x00000083,		/* call_pal callsys */
};
#define	RETCODE_NWORDS \
  (sizeof (sigtramp_retcode) / sizeof (sigtramp_retcode[0]))

LONGEST
alphanbsd_sigtramp_offset (CORE_ADDR pc)
{
  unsigned int ret[4], w;
  LONGEST off;
  int i;

  if (read_memory_nobpt (pc, (char *) &w, 4) != 0)
    return -1;

  for (i = 0; i < RETCODE_NWORDS; i++)
    {
      if (w == sigtramp_retcode[i])
	break;
    }
  if (i == RETCODE_NWORDS)
    return (-1);

  off = i * 4;
  pc -= off;

  if (read_memory_nobpt (pc, (char *) ret, sizeof (ret)) != 0)
    return -1;

  if (memcmp (ret, sigtramp_retcode, sizeof (sigtramp_retcode)) == 0)
    return off;

  return -1;
}

static int
alphanbsd_pc_in_sigtramp (CORE_ADDR pc, char *func_name)
{
  return (alphanbsd_sigtramp_offset (pc) >= 0);
}

static void
alphanbsd_init_abi (struct gdbarch_info info,
                    struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  set_gdbarch_pc_in_sigtramp (gdbarch, alphanbsd_pc_in_sigtramp);

  /* NetBSD/alpha does not provide single step support via ptrace(2); we
     must use software single-stepping.  */
  set_gdbarch_software_single_step (gdbarch, alpha_software_single_step);

  set_solib_svr4_fetch_link_map_offsets (gdbarch,
                                  alphanbsd_solib_svr4_fetch_link_map_offsets);

  tdep->dynamic_sigtramp_offset = alphanbsd_sigtramp_offset;
}

void
_initialize_alphanbsd_tdep (void)
{
  alpha_gdbarch_register_os_abi (ALPHA_ABI_NETBSD, alphanbsd_init_abi);
}
