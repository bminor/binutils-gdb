/* sframe-state.c - The SFrame state for stacktracing.

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

#define _sf_printflike_(string_index, first_to_check) ATTRIBUTE_PRINTF (1, 2)

static bool _sframe_unwind_debug;	/* Control for printing out debug info.  */
static const int no_of_entries = NUM_OF_DSOS;

void
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

#if 0
/* sframe_bt_set_errno - Store the specified error code ERROR into ERRP if
   it is non-NULL.  */

static void
sframe_bt_set_errno (int *errp, int error)
{
  if (errp != NULL)
    *errp = error;
}

#endif

static void sframest_sfinfo_init(struct sframest_info *sfinfo,
				 const char *buf, int64_t buflen,
				 uint64_t sframe_vma,
				 uint64_t text_vma, int64_t text_size)
{
  if (!sfinfo)
    return;

  sfinfo->buf = buf;
  sfinfo->buflen = buflen;
  sfinfo->sframe_vma = sframe_vma;
  sfinfo->text_vma = text_vma;
  sfinfo->text_size = text_size;
}

/* Return whether the given SFrame stack trace info object SFINFO has (stack
   trace) information corresponding to addr.  */

bool sframest_sfinfo_addr_range_p (struct sframest_info *sfinfo,
				   uint64_t addr)
{
  if (!sfinfo || !addr)
    return false;

  return (sfinfo->text_vma <= addr
	  && sfinfo->text_vma + sfinfo->text_size > addr);
}

/* Add .sframe info in D_DATA, which is associated with
   a dynamic shared object, to D_LIST.  */

static int
sframe_add_dso (struct sframest_info_list *d_list,
		struct sframest_info d_data)
{
  int err = 0;

  if (!d_list->alloced)
    {
      d_list->entry = malloc (no_of_entries * sizeof (struct sframest_info));
      if (!d_list->entry)
	return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_MALLOC);

      memset (d_list->entry, 0,
	      no_of_entries * sizeof (struct sframest_info));
      d_list->alloced = no_of_entries;
    }
  else if (d_list->used == d_list->alloced)
    {
      d_list->entry = realloc (d_list->entry,
			       ((d_list->alloced + no_of_entries)
				* sizeof (struct sframest_info)));
      if (!d_list->entry)
	return sframe_bt_ret_set_errno (&err, SFRAME_BT_ERR_REALLOC);

      memset (&d_list->entry[d_list->alloced], 0,
	      no_of_entries * sizeof (struct sframest_info));
      d_list->alloced += no_of_entries;
    }

  sframe_bt_ret_set_errno (&err, SFRAME_BT_OK);
  d_list->entry[d_list->used++] = d_data;

  return SFRAME_BT_OK;
}

/* Free up space allocated for .sframe info for CF.  */

void
sframe_free_cfi (struct sframest_ctx *sf)
{
  struct sframest_info_list *d_list;
  int i;

  if (!sf)
    return;

  // free (sf->sui_ctx.sfdd_data);
  sframe_decoder_free (&sf->prog_sfinfo.dctx);
  close (sf->fd);

  d_list = &sf->dsos_sfinfo;
  if (!d_list->alloced)
    return;

  for (i = 0; i < d_list->used; ++i)
    {
      // free (d_list->entry[i].sfdd_data);
      sframe_decoder_free (&d_list->entry[i].dctx);
    }

  free (d_list->entry);
}

/* Find the decode data that contains ADDR from CF.
   Return the pointer to the decode data or NULL.  */

struct sframest_info *
sframe_find_context (struct sframest_ctx *sf, uint64_t addr)
{
  struct sframest_info_list *d_list;
  struct sframest_info sfinfo;
  int i;

  if (!sf)
    return NULL;

  if (sframest_sfinfo_addr_range_p (&sf->prog_sfinfo, addr))
    return &sf->prog_sfinfo;

  d_list = &sf->dsos_sfinfo;
  for (i = 0; i < sf->dsos_sfinfo.used; ++i)
    {
      sfinfo = d_list->entry[i];
      if (sframest_sfinfo_addr_range_p (&sfinfo, addr))
	return &d_list->entry[i];
    }

  return NULL;
}

/* Call decoder to create and set up the SFrame info for either the main module
   or one of the DSOs from CF, based on the input RADDR argument.  Return the
   newly created decode context or NULL.  */

struct sframest_info *
sframest_get_sfinfo (struct sframest_ctx *sf, uint64_t raddr)
{
  struct sframest_info *sfinfo = NULL;
  int err = 0;

  if (!sf)
    return NULL;

  sfinfo = sframe_find_context (sf, raddr);
  if (!sfinfo)
    return NULL;

  /* Decode the SFrame section the first time.  */
  if (!sfinfo->dctx)
    sfinfo->dctx = sframe_decode (sfinfo->buf, sfinfo->buflen, &err);

  return sfinfo;
}

/* Open /proc image associated with the process id and return the file
   descriptor.  */

static int
get_proc_mem_fd (int *errp)
{
  int fd;

  if ((fd = open ("/proc/self/mem", O_CLOEXEC)) == -1)
    {
      sframe_bt_ret_set_errno (errp, SFRAME_BT_ERR_OPEN);
      return -1;
    }

  return fd;
}

/* The callback from dl_iterate_phdr with header info in INFO.
   Return SFrame info for either the main module or a DSO in DATA.  */

int
sframe_callback (struct dl_phdr_info *info,
		 size_t size ATTRIBUTE_UNUSED,
		 void *data)
{
  struct sframest_ctx *sf = (struct sframest_ctx *) data;
  int p_type, i, fd, sframe_err;
  uint64_t text_vma = 0;
  uint64_t text_size = 0;

  if (!data || !info)
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

      if (info->dlpi_name[0] == '\0')
	{
	  /* the main module.  */
	  fd = get_proc_mem_fd (&sframe_err);
	  if (fd == -1)
	    return 1;

	  assert (text_vma);
	  sframest_sfinfo_init (&sf->prog_sfinfo,
				(char *)(info->dlpi_addr
					 + info->dlpi_phdr[i].p_vaddr),
				info->dlpi_phdr[i].p_memsz,
				info->dlpi_addr + info->dlpi_phdr[i].p_vaddr,
				text_vma, text_size);
	  sf->fd = fd;
	  text_vma = 0;
	  return 0;
	}
      else
	{
	  /* a dynamic shared object.  */
	  struct sframest_info sfinfo;
	  memset (&sfinfo, 0, sizeof (struct sframest_info));
	  assert (sf->fd);

	  assert (text_vma);
	  sframest_sfinfo_init (&sfinfo,
				(char *)(info->dlpi_addr
					 + info->dlpi_phdr[i].p_vaddr),
				info->dlpi_phdr[i].p_memsz,
				info->dlpi_addr + info->dlpi_phdr[i].p_vaddr,
				text_vma, text_size);

	  text_vma = 0;
	  
	  sframe_err = sframe_add_dso (&sf->dsos_sfinfo, sfinfo);
	  // FIXME TODO
	  if (sframe_err != SFRAME_BT_OK)
	    return 1;
	  return 0;
	}
    }

    return 0;
}
