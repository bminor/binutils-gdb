/* sframe-backtrace.c - The SFrame backtracer.

   Copyright (C) 2022 Free Software Foundation, Inc.

   This file is part of libsframebt.

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
#include "sframe-backtrace-api.h"

#ifndef PT_SFRAME
#define PT_SFRAME 0x6474e554		/* FIXME.  */
#endif

#define _sf_printflike_(string_index, first_to_check) ATTRIBUTE_PRINTF (1, 2)

static bool _sframe_unwind_debug;	/* Control for printing out debug info.  */
static int no_of_entries = NUM_OF_DSOS;

/* SFrame decode data for the main module or a DSO.  */
struct sframe_decode_data
{
  char *sfdd_data;			/* SFrame decode data.  */
  int sfdd_data_size;			/* SFrame decode data size.  */
  uint64_t sfdd_text_vma;		/* Text segment's virtual address.  */
  int sfdd_text_size;			/* Text segment's size.  */
  uint64_t sfdd_sframe_vma;		/* SFrame segment's virtual address.  */
  sframe_decoder_ctx *sfdd_sframe_ctx;	/* SFrame decoder context.  */
};

/* List that holds SFrame info for the shared libraries.  */
struct dso_cfi_list
{
  int alloced;				/* Entries allocated.  */
  int used;				/* Entries used.  */
  struct sframe_decode_data *entry;	/* DSO's decode data.  */
};

/* Data that's passed through sframe_callback.  */
struct sframe_unwind_info
{
  int sui_fd;				/* File descriptor.  */
  struct sframe_decode_data sui_ctx;	/* The decode data.  */
  struct dso_cfi_list sui_dsos;		/* The DSO list.  */
};

static void
sframe_unwind_init_debug (void)
{
  static int inited;

  if (!inited)
    {
      _sframe_unwind_debug = getenv ("SFRAME_UNWIND_DEBUG") != NULL;
      inited = 1;
    }
}

_sf_printflike_ (1, 2)
static void
debug_printf (const char *format, ...)
{
  if (_sframe_unwind_debug == true)
    {
      va_list args;

      va_start (args, format);
      __builtin_vprintf (format, args);
      va_end (args);
    }
}

/* sframe_bt_errno - Check if there is error code in ERRP.  */

static int
sframe_bt_errno (const int *errp)
{
  if (errp == NULL)
    return 0;

  return (*errp != SFRAME_BT_OK);
}

/* sframe_bt_set_errno - Store the specified error code ERROR into ERRP if
   it is non-NULL.  */

static void
sframe_bt_set_errno (int *errp, int error)
{
  if (errp != NULL)
    *errp = error;
}

/* sframe_add_dso - Add .sframe info in D_DATA, which is associated with
   a dynamic shared object, to D_LIST.  */

static void
sframe_add_dso (struct dso_cfi_list *d_list,
		struct sframe_decode_data d_data,
		int *errp)
{
  if (d_list->alloced == 0)
    {
      d_list->entry = malloc (no_of_entries * sizeof (struct sframe_decode_data));
      if (d_list->entry == NULL)
	{
	  sframe_bt_set_errno (errp, SFRAME_BT_ERR_MALLOC);
	  return;
	}
      memset (d_list->entry, 0,
	      no_of_entries * sizeof (struct sframe_decode_data));
      d_list->alloced = no_of_entries;
    }
  else if (d_list->used == d_list->alloced)
    {
      d_list->entry = realloc (d_list->entry,
			       ((d_list->alloced + no_of_entries)
				* sizeof (struct sframe_decode_data)));
      if (d_list->entry == NULL)
	{
	  sframe_bt_set_errno (errp, SFRAME_BT_ERR_REALLOC);
	  return;
	}

      memset (&d_list->entry[d_list->alloced], 0,
	      no_of_entries * sizeof (struct sframe_decode_data));
      d_list->alloced += no_of_entries;
    }

  sframe_bt_set_errno (errp, SFRAME_BT_OK);
  d_list->entry[d_list->used++] = d_data;
}

/* sframe_free_cfi - Free up space allocated for .sframe info for CF.  */

static void
sframe_free_cfi (struct sframe_unwind_info *sf)
{
  struct dso_cfi_list *d_list;
  int i;

  if (sf == NULL)
    return;

  free (sf->sui_ctx.sfdd_data);
  sframe_decoder_free (&sf->sui_ctx.sfdd_sframe_ctx);
  close (sf->sui_fd);

  d_list = &sf-> sui_dsos;
  if (d_list == NULL)
    return;

  for (i = 0; i < d_list->used; ++i)
    {
      free (d_list->entry[i].sfdd_data);
      sframe_decoder_free (&d_list->entry[i].sfdd_sframe_ctx);
    }

  free (d_list->entry);
}

/* sframe_find_context - Find the decode data that contains ADDR from CF.
   Return the pointer to the decode data or NULL.  */

static struct sframe_decode_data *
sframe_find_context (struct sframe_unwind_info *sf, uint64_t addr)
{
  struct dso_cfi_list *d_list;
  struct sframe_decode_data sdec_data;
  int i;

  if (sf == NULL)
    return NULL;

  if (sf->sui_ctx.sfdd_text_vma < addr
      && sf->sui_ctx.sfdd_text_vma + sf->sui_ctx.sfdd_text_size > addr)
    return &sf->sui_ctx;

  d_list = &sf->sui_dsos;
  for (i = 0; i < sf->sui_dsos.used; ++i)
    {
      sdec_data = d_list->entry[i];
      if ((sdec_data.sfdd_text_vma <= addr)
	  && (sdec_data.sfdd_text_vma + sdec_data.sfdd_text_size >= addr))
	return &d_list->entry[i];
    }

  return NULL;
}

/* sframe_valid_addr - Check if ADDR is valid in CF. The address is considered
   invalid, with regards to SFrame, if it's not in any address range of the
   main module or any of its DSO's. Return 1 if valid, 0 otherwise.  */

static int
sframe_valid_addr (struct sframe_unwind_info *sf, uint64_t addr)
{
  struct sframe_decode_data *cdp;

  if (sf == NULL)
    return 0;

  cdp = sframe_find_context (sf, addr);
  return cdp ? 1 : 0;
}

/* sframe_load_ctx - Call decoder to create and set up the SFrame info for
   either the main module or one of the DSOs from CF, based on the input
   RADDR argument.  Return the newly created decode context or NULL.  */

static sframe_decoder_ctx *
sframe_load_ctx (struct sframe_unwind_info *sf, uint64_t raddr)
{
  sframe_decoder_ctx *nctx;
  struct sframe_decode_data *cdp;

  if (sf == NULL)
    return NULL;

  cdp = sframe_find_context (sf, raddr);
  if (cdp == NULL)
    return NULL;

  if (cdp->sfdd_sframe_ctx == NULL)
    {
      int err;
      nctx = sframe_decode (cdp->sfdd_data, cdp->sfdd_data_size, &err);
      if (nctx == NULL)
	return NULL;
      cdp->sfdd_sframe_ctx = nctx;
      return nctx;
    }

  return NULL;
}

/* sframe_update_ctx - Check if need to do a decode context switch, based on
   the input RADDR argument, from CF. A new decode context will be created
   and set up if it isn't already done so. Return the new decode context in
   CTX and vma in CFI_VMA.  */

static void
sframe_update_ctx (struct sframe_unwind_info *sf, uint64_t raddr,
		   sframe_decoder_ctx **ctx, uint64_t *cfi_vma)
{
  sframe_decoder_ctx *nctx;
  struct sframe_decode_data *cdp;

  cdp = sframe_find_context (sf, raddr);
  if (cdp != NULL)
    {
      if (cdp->sfdd_sframe_ctx == NULL)
	{
	  int err;
	  nctx = sframe_decode (cdp->sfdd_data, cdp->sfdd_data_size, &err);
	  if (nctx == NULL)
	    {
	      *ctx = NULL;
	      return;
	    }
	  cdp->sfdd_sframe_ctx = nctx;
	}
	*ctx = cdp->sfdd_sframe_ctx;
	*cfi_vma = cdp->sfdd_sframe_vma;
    }
}

/* get_contents - Return contents at ADDR from file descriptor FD.  */

static uint64_t
get_contents (int fd, uint64_t addr, int *errp)
{
  uint64_t data;
  size_t sz;

  sframe_bt_set_errno (errp, SFRAME_BT_OK);
  if (lseek (fd, addr, SEEK_SET) == -1)
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_LSEEK);
      return 0;
    }
  sz = read (fd, &data, sizeof (uint64_t));
  if (sz != sizeof (uint64_t))
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_READ);
      return 0;
    }

  return data;
}

/* sframe_fd_open - Open /proc image associated with the process id and return
   the file descriptor.  */

static int
sframe_fd_open (int *errp)
{
  int fd;

  if ((fd = open ("/proc/self/mem", O_CLOEXEC)) == -1)
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_OPEN);
      return -1;
    }

  return fd;
}

/* sframe_callback - The callback from dl_iterate_phdr with header info
   in INFO.
   Return SFrame info for either the main module or a DSO in DATA.  */

static int
sframe_callback (struct dl_phdr_info *info,
		 size_t size ATTRIBUTE_UNUSED,
		 void *data)
{
  struct sframe_unwind_info *sf = (struct sframe_unwind_info *) data;
  int p_type, i, fd, sframe_err;
  ssize_t len;
  uint64_t text_vma = 0;
  int text_size = 0;

  if (data == NULL || info == NULL)
    return 1;

  debug_printf ("-- name: %s %14p\n", info->dlpi_name, (void *)info->dlpi_addr);

  for (i = 0; i < info->dlpi_phnum; i++)
    {
      debug_printf ("  %2d: [%" PRIu64 "; memsz %" PRIu64 "] flags: 0x%x; \n", i,
		   (uint64_t) info->dlpi_phdr[i].p_vaddr,
		   (uint64_t) info->dlpi_phdr[i].p_memsz,
		   info->dlpi_phdr[i].p_flags);

      p_type = info->dlpi_phdr[i].p_type;
      if (p_type == PT_LOAD && info->dlpi_phdr[i].p_flags & PF_X)
	{
	  text_vma = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
	  text_size = info->dlpi_phdr[i].p_memsz;
	  continue;
	}
      if (p_type != PT_SFRAME)
	continue;

      if (info->dlpi_name[0] == '\0')		/* the main module.  */
	{
	  fd = sframe_fd_open (&sframe_err);
	  if (fd == -1)
	    return 1;
	  if (lseek (fd, info->dlpi_addr + info->dlpi_phdr[i].p_vaddr,
		     SEEK_SET) == -1)
	    {
	      sframe_bt_set_errno (&sframe_err, SFRAME_BT_ERR_LSEEK);
	      return 1;
	    }

	  sf->sui_ctx.sfdd_data = malloc (info->dlpi_phdr[i].p_memsz);
	  if (sf->sui_ctx.sfdd_data == NULL)
	    {
	      sframe_bt_set_errno (&sframe_err, SFRAME_BT_ERR_MALLOC);
	      return 1;
	    }

	  len = read (fd, sf->sui_ctx.sfdd_data, info->dlpi_phdr[i].p_memsz);
	  if (len == -1 || len != (ssize_t) info->dlpi_phdr[i].p_memsz)
	    {
	      sframe_bt_set_errno (&sframe_err, SFRAME_BT_ERR_READ);
	      return 1;
	    }

	  assert (text_vma);
	  sf->sui_ctx.sfdd_data_size = len;
	  sf->sui_ctx.sfdd_sframe_vma = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
	  sf->sui_fd = fd;
	  sf->sui_ctx.sfdd_text_vma = text_vma;
	  sf->sui_ctx.sfdd_text_size = text_size;
	  text_vma = 0;
	  return 0;
	}
      else
	{					/* a dynamic shared object.  */
	  struct sframe_decode_data dt;
	  memset (&dt, 0, sizeof (struct sframe_decode_data));
	  assert (sf->sui_fd);
	  if (lseek (sf->sui_fd, info->dlpi_addr + info->dlpi_phdr[i].p_vaddr,
		     SEEK_SET) == -1)
	    {
	      sframe_bt_set_errno (&sframe_err, SFRAME_BT_ERR_LSEEK);
	      return 1;
	    }

	  dt.sfdd_data = malloc (info->dlpi_phdr[i].p_memsz);
	  if (dt.sfdd_data == NULL)
	    {
	      sframe_bt_set_errno (&sframe_err, SFRAME_BT_ERR_MALLOC);
	      return 1;
	    }

	  len = read (sf->sui_fd, dt.sfdd_data, info->dlpi_phdr[i].p_memsz);
	  if (len == -1 || len != (ssize_t) info->dlpi_phdr[i].p_memsz)
	    {
	      sframe_bt_set_errno (&sframe_err, SFRAME_BT_ERR_READ);
	      return 1;
	    }

	  assert (text_vma);
	  dt.sfdd_data_size = len;
	  dt.sfdd_sframe_vma = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
	  dt.sfdd_text_vma = text_vma;
	  dt.sfdd_text_size = text_size;
	  text_vma = 0;
	  sframe_add_dso (&sf->sui_dsos, dt, &sframe_err);
	  if (sframe_err != SFRAME_BT_OK)
	    return 1;
	  return 0;
	}
    }

    return 0;
}

/* sframe_unwind - Unwind the stack backtrace for CF. If successful,
   store the return addresses in RA_LST. The RA_SIZE argument specifies
   the maximum number of return addresses that can be stored in RA_LST
   and contains the number of the addresses collected.  */

static void
sframe_unwind (struct sframe_unwind_info *sf, void **ra_lst,
	       int *ra_size, int *errp)
{
  uint64_t cfa, return_addr, ra_stack_loc, rfp_stack_loc;
  int8_t fixed_ra_offset;
  sframe_decoder_ctx *ctx;
  int cfa_offset, rfp_offset, errnum, i, count;
  sframe_frame_row_entry fred, *frep = &fred;
  uint64_t pc, rfp, rsp, cfi_vma;
  ucontext_t context, *cp = &context;

  if (sf == NULL || ra_lst == NULL || ra_size == NULL)
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_ARG);
      return;
    }

  /* Get the user context for its registers.  */
  if (getcontext (cp) != 0)
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_GETCONTEXT);
      return;
    }
  sframe_bt_set_errno (errp, SFRAME_BT_OK);

#if defined (__x86_64__)
  pc = cp->uc_mcontext.gregs[REG_RIP];
  rsp = cp->uc_mcontext.gregs[REG_RSP];
  rfp = cp->uc_mcontext.gregs[REG_RBP];
#elif defined (__aarch64__)
#define UNWIND_AARCH64_X29		29	/* 64-bit frame pointer.  */
#define UNWIND_AARCH64_X30		30	/* 64-bit link pointer.  */
  pc = cp->uc_mcontext.pc;
  rsp = cp->uc_mcontext.sp;
  rfp = cp->uc_mcontext.regs[UNWIND_AARCH64_X29];
  uint64_t ra = cp->uc_mcontext.regs[UNWIND_AARCH64_X30];
#endif

  /* Load and set up the decoder.  */
  ctx = sframe_load_ctx (sf, pc);
  if (ctx == NULL)
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_DECODE);
      return;
    }
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
	    {
	      sframe_bt_set_errno (errp, SFRAME_BT_ERR_CFA_OFFSET);
	      return;
	    }

	  cfa = ((frep->fre_info & 0x1) == SFRAME_BASE_REG_SP
	    ? rsp : rfp) + cfa_offset;

#ifdef __x86_64__
	  /* For x86, read the return address from the fixed RA offset from
	     the SFrame header.  RA must be at location CFA - 8.  */
	  fixed_ra_offset = sframe_decoder_get_fixed_ra_offset (ctx);
	  if (fixed_ra_offset == SFRAME_CFA_FIXED_RA_INVALID)
	   return;

	  ra_stack_loc = cfa + fixed_ra_offset;
	  return_addr = get_contents (sf->sui_fd, ra_stack_loc, errp);
	  if (sframe_bt_errno (errp))
	    return;
#else
#ifdef __aarch64__
	  int ra_offset = sframe_fre_get_ra_offset (ctx, frep, &errnum);
	  if (errnum == 0)
	    {
	      ra_stack_loc = cfa + ra_offset;
	      return_addr = get_contents (sf->sui_fd, ra_stack_loc, errp);
	      if (sframe_bt_errno (errp))
		return;
	    }
	  else
	    return_addr = ra;
#endif
#endif

	  /* Validate and add return address to the list.  */
	  if (sframe_valid_addr (sf, return_addr) == 0)
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
	      rfp = get_contents (sf->sui_fd, rfp_stack_loc, errp);
	      if (sframe_bt_errno (errp))
		return;
	    }
	  rsp = cfa;
	  pc = return_addr;

	  /* Check if need to update the decoder context and vma.  */
	  sframe_update_ctx (sf, return_addr, &ctx, &cfi_vma);
	  if (ctx == NULL)
	    {
	      sframe_bt_set_errno (errp, SFRAME_BT_ERR_DECODE);
	      return;
	    }
	}
      else
	{
	  i -= 1;
	  goto find_fre_ra_err;
	}
    }

find_fre_ra_err:
  *ra_size = i;
}

/* sframe_backtrace - Main API that user program calls to get a backtrace.
   The BUFFER argument provides space for the list of the return addresses
   and the SIZE argument specifies the maximum number of addresses that
   can be stored in the buffer.  Return the number of return addresses
   collected or -1 if there is any error.  */

int
sframe_backtrace (void **buffer, int size, int *errp)
{
  struct sframe_unwind_info sframeinfo;

  sframe_unwind_init_debug ();

  memset (&sframeinfo, 0, sizeof (struct sframe_unwind_info));

  /* find and set up the .sframe sections.  */
  (void) dl_iterate_phdr (sframe_callback, (void *)&sframeinfo);
  if (sframeinfo.sui_fd == 0)
    {
      sframe_bt_set_errno (errp, SFRAME_BT_ERR_NOTPRESENT);
      return -1;
    }

  /* Do the stack unwinding.  */
  sframe_unwind (&sframeinfo, buffer, &size, errp);
  if (sframe_bt_errno (errp))
    size = -1;

  sframe_free_cfi (&sframeinfo);

  return size;
}
