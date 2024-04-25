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

  if (data == NULL)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_ARG);

  if (lseek (fd, addr, SEEK_SET) == -1)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_LSEEK);
  
  sz = read (fd, data, STACK_ELEM_SIZE);
  if (sz != STACK_ELEM_SIZE)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_READ);

  return SFRAME_BT_OK;
}

/* sframe_valid_addr_p - Check if ADDR is valid in CF. The address is considered
   invalid, with regards to SFrame, if it's not in any address range of the
   main module or any of its DSO's. Return 1 if valid, 0 otherwise.  */

static int
sframe_valid_addr_p (struct sframe_state *sf, uint64_t addr)
{
  struct sframe_stinfo *cdp;

  if (sf == NULL)
    return 0;

  cdp = sframe_find_context (sf, addr);
  return cdp ? 1 : 0;
}

/* Unwind the stack and collect the stacktrace given SFrame unwind info SF.
   If successful, store the return addresses in RA_LST. The RA_SIZE argument
   specifies the maximum number of return addresses that can be stored in
   RA_LST and contains the number of the addresses collected.  */

static int
sframe_unwind (struct sframe_state *sf, void **ra_lst,
	       int *ra_size)
{
  uint64_t cfa, return_addr, ra_stack_loc, rfp_stack_loc;
  int8_t fixed_ra_offset;
  sframe_decoder_ctx *ctx;
  int cfa_offset, rfp_offset, errnum, i, count;
  sframe_frame_row_entry fred, *frep = &fred;
  uint64_t pc, rfp, rsp, ra, cfi_vma;
  ucontext_t context, *cp = &context;
  int err = 0;

  if (sf == NULL || ra_lst == NULL || ra_size == NULL)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_ARG);

  /* Get the user context for its registers.  */
  if (getcontext (cp) != 0)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_GETCONTEXT);

  pc = get_context_pc (cp);
  rsp = get_context_rsp (cp);
  rfp = get_context_rfp (cp);
  ra = get_context_ra (cp);

  /* Load and set up the decoder.  */
  ctx = sframe_load_ctx (sf, pc);
  if (ctx == NULL)
    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_DECODE);

  cfi_vma = sf->sui_ctx.sfdd_sframe_vma;
  count = *ra_size;

  for (i = 0; i < count; ++i)
    {
      pc -= cfi_vma;
      errnum = sframe_find_fre (ctx, pc, frep);
      if (errnum == 0)
	{
	  cfa_offset = sframe_fre_get_cfa_offset (ctx, frep, &errnum);
	  if (errnum == SFRAME_ERR_FREOFFSET_NOPRESENT)
	    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_CFA_OFFSET);

	  cfa = (((frep->fre_info & 0x1) == SFRAME_BASE_REG_SP)
		 ? rsp : rfp) + cfa_offset;

#ifdef __x86_64__
	  /* For x86, read the return address from the fixed RA offset from
	     the SFrame header.  RA must be at location CFA - 8.  */
	  fixed_ra_offset = sframe_decoder_get_fixed_ra_offset (ctx);
	  if (fixed_ra_offset == SFRAME_CFA_FIXED_RA_INVALID)
	   return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_FRE_INVAL);

	  ra_stack_loc = cfa + fixed_ra_offset;
	  errnum = get_contents_8b (sf->sui_fd, ra_stack_loc, &return_addr);
	  if (sframe_bt_errno (&errnum))
	    return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_FRE_INVAL);
#else
#ifdef __aarch64__
	  int ra_offset = sframe_fre_get_ra_offset (ctx, frep, &errnum);
	  if (errnum == 0)
	    {
	      ra_stack_loc = cfa + ra_offset;
	      errnum = get_contents_8b (sf->sui_fd, ra_stack_loc, &return_addr);
	      if (sframe_bt_errno (&errnum))
		return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_FRE_INVAL);
	    }
	  else
	    return_addr = ra;
#endif
#endif

	  /* Validate and add return address to the list.  */
	  if (sframe_valid_addr_p (sf, return_addr) == 0)
	    {
	      i -= 1;
	      goto find_fre_ra_err;
	    }
	  if (i != 0)		/* exclude self.  */
	    ra_lst[i-1] = (void *)return_addr;

	  /* Set up for the next frame.  */
	  rfp_offset = sframe_fre_get_fp_offset (ctx, frep, &errnum);
	  if (errnum == 0)
	    {
	      rfp_stack_loc = cfa + rfp_offset;
	      errnum = get_contents_8b (sf->sui_fd, rfp_stack_loc, &rfp);
	      if (sframe_bt_errno (&errnum))
		return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_FRE_INVAL);
	    }
	  rsp = cfa;
	  pc = return_addr;

	  /* Check if need to update the decoder context and vma.  */
	  sframe_update_ctx (sf, return_addr, &ctx, &cfi_vma);
	  if (ctx == NULL)
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
  struct sframe_state sframeinfo;

  sframe_unwind_init_debug ();

  memset (&sframeinfo, 0, sizeof (struct sframe_state));

  /* Find the .sframe sections and setup the SFrame state for generating stack
     traces.  */
  (void) dl_iterate_phdr (sframe_callback, (void *)&sframeinfo);
  if (sframeinfo.sui_fd == 0)
    {
      sframe_bt_ret_set_errno (errp, SFRAME_BT_ERR_BAD_SFSTATE);
      return -1;
    }

  /* Do the stack unwinding.  */
  *errp = sframe_unwind (&sframeinfo, buffer, &size);
  if (sframe_bt_errno (errp))
    size = -1;

  sframe_free_cfi (&sframeinfo);

  return size;
}
