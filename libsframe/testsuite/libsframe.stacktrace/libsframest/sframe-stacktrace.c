/* sframe-stacktrace.c - The SFrame stacktracer.

   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of libsframest.

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
#include <link.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <execinfo.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ucontext.h>
#include <stdarg.h>
#include "ansidecl.h"
#include "sframe-api.h"
#include "sframe-stacktrace-api.h"
#include "sframe-stacktrace-regs.h"
#include "sframe-state.h"

#define STACK_ELEM_SIZE	  sizeof(uint64_t)

/* Get the 8 bytes at ADDR from file descriptor FD.  */

static int
get_contents_8b (int fd, uint64_t addr, uint64_t *data)
{
  size_t sz = 0;
  int err = 0;

  if (!data)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_ARG);

  if (lseek (fd, addr, SEEK_SET) == -1)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_LSEEK);
  
  sz = read (fd, data, STACK_ELEM_SIZE);
  if (sz != STACK_ELEM_SIZE)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_READ);

  return SFRAME_BT_OK;
}

/* Check if ADDR is valid in SF.

   The address is considered invalid, with regards to SFrame, if it's not in
   any address range of the main module or any of its DSO's.

   Return 1 if valid, 0 otherwise.  */

static bool
sframe_valid_addr_p (struct sframest_ctx *sf, uint64_t addr)
{
  struct sframest_info *sfinfo;

  if (!sf)
    return 0;

  sfinfo = sframe_find_context (sf, addr);
  return sfinfo ? 1 : 0;
}

/* Unwind the stack and collect the stacktrace given SFrame unwind info SF.
   If successful, store the return addresses in RA_LST. The RA_SIZE argument
   specifies the maximum number of return addresses that can be stored in
   RA_LST and contains the number of the addresses collected.  */

static int
sframe_unwind (struct sframest_ctx *sf, void **ra_lst, int *ra_size)
{
  uint64_t cfa, return_addr, ra_stack_loc, rfp_stack_loc;
  sframe_decoder_ctx *dctx;
  int cfa_offset, rfp_offset, ra_offset, errnum, i, count;
  sframe_frame_row_entry fred, *frep = &fred;
  uint64_t pc, rfp, rsp, ra, sframe_vma;
  ucontext_t context, *cp = &context;
  int err = 0;

  if (!sf || !ra_lst || !ra_size)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_ARG);

  /* Get the user context for its registers.  */
  if (getcontext (cp))
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_GETCONTEXT);

  pc = get_context_pc (cp);
  rsp = get_context_rsp (cp);
  rfp = get_context_rfp (cp);
  ra = get_context_ra (cp);

  return_addr = ra;

  /* Load and set up the decoder.  */
  dctx = sframe_load_ctx (sf, pc);
  if (!dctx)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_DECODE);

  sframe_vma = sf->prog_sfinfo.sframe_vma;
  count = *ra_size;

  for (i = 0; i < count; ++i)
    {
      pc -= sframe_vma;
      errnum = sframe_find_fre (dctx, pc, frep);
      if (!errnum)
	{
	  cfa_offset = sframe_fre_get_cfa_offset (dctx, frep, &errnum);
	  if (errnum == SFRAME_ERR_FREOFFSET_NOPRESENT)
	    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_CFA_OFFSET);

	  cfa = ((sframe_fre_get_base_reg_id (frep, &errnum)
		  == SFRAME_BASE_REG_SP) ? rsp : rfp) + cfa_offset;

	  ra_offset = sframe_fre_get_ra_offset (dctx, frep, &errnum);
	  if (!errnum)
	    {
	      ra_stack_loc = cfa + ra_offset;
	      errnum = get_contents_8b (sf->fd, ra_stack_loc, &return_addr);
	      if (sframe_bt_errno (&errnum))
		return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_FRE_INVAL);
	    }

	  /* Validate and add return address to the list.  */
	  if (!sframe_valid_addr_p (sf, return_addr))
	    {
	      i -= 1;
	      goto find_fre_ra_err;
	    }
	  if (i != 0)		/* exclude self.  */
	    ra_lst[i-1] = (void *)return_addr;

	  /* Set up for the next frame.  */
	  rfp_offset = sframe_fre_get_fp_offset (dctx, frep, &errnum);
	  if (!errnum)
	    {
	      rfp_stack_loc = cfa + rfp_offset;
	      errnum = get_contents_8b (sf->fd, rfp_stack_loc, &rfp);
	      if (sframe_bt_errno (&errnum))
		return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_FRE_INVAL);
	    }
	  rsp = cfa;
	  pc = return_addr;

	  /* Check if need to update the decoder context and vma.  */
	  sframe_update_ctx (sf, return_addr, &dctx, &sframe_vma);
	  if (!dctx)
	    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_DECODE);
	}
      else
	{
	  i -= 1;
	  goto find_fre_ra_err;
	}
    }

find_fre_ra_err:
  *ra_size = i;

  return SFRAME_BT_OK;
}

/* Main API that user program calls to get a stacktrace.

   The BUFFER argument provides space for the list of the return addresses
   and the SIZE argument specifies the maximum number of addresses that
   can be stored in the buffer.
   
   Returns the number of return addresses collected or -1 if there is any
   error.  ERRP is set with the error code if any.   */

int
sframe_stacktrace (void **buffer, int size, int *errp)
{
  struct sframest_ctx sf_ctx;

  sframe_unwind_init_debug ();

  memset (&sf_ctx, 0, sizeof (struct sframest_ctx));

  /* Find the .sframe sections and setup the SFrame state for generating stack
     traces.  */
  (void) dl_iterate_phdr (sframe_callback, (void *)&sf_ctx);
  if (!sf_ctx.fd)
    {
      sframe_bt_ret_set_errno (errp, SFRAME_BT_ERR_BAD_SFSTATE);
      return -1;
    }

  /* Do the stack unwinding.  */
  *errp = sframe_unwind (&sf_ctx, buffer, &size);
  if (sframe_bt_errno (errp))
    size = -1;

  sframe_free_cfi (&sf_ctx);

  return size;
}
