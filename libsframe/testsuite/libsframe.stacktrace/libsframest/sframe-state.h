/* sframe-state.h - The SFrame state for stacktracing.

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

#ifndef SFRAME_STATE_H
#define SFRAME_STATE_H

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

#ifndef PT_SFRAME
#define PT_SFRAME 0x6474e554		/* FIXME.  */
#endif

/* SFrame decode data for the main module or a DSO.  */
struct sframe_stinfo
{
  char *sfdd_data;			/* SFrame decode data.  */
  int sfdd_data_size;			/* SFrame decode data size.  */
  uint64_t sfdd_text_vma;		/* Text segment's virtual address.  */
  int sfdd_text_size;			/* Text segment's size.  */
  uint64_t sfdd_sframe_vma;		/* SFrame segment's virtual address.  */
  sframe_decoder_ctx *sfdd_sframe_ctx;	/* SFrame decoder context.  */
};

/* List that holds SFrame info for the shared libraries.  */
struct sframe_stinfo_list
{
  int alloced;				/* Entries allocated.  */
  int used;				/* Entries used.  */
  struct sframe_stinfo *entry;	/* DSO's decode data.  */
};

/* Data that's passed through sframe_callback.  */
struct sframe_state
{
  int sui_fd;				/* File descriptor.  */
  struct sframe_stinfo sui_ctx;	/* The decode data.  */
  struct sframe_stinfo_list sui_dsos;		/* The DSO list.  */
};


void sframe_unwind_init_debug (void);

int sframe_callback (struct dl_phdr_info *info,
		     size_t size ATTRIBUTE_UNUSED,
		     void *data);

void sframe_update_ctx (struct sframe_state *sf, uint64_t raddr,
			sframe_decoder_ctx **ctx, uint64_t *cfi_vma);

sframe_decoder_ctx *sframe_load_ctx (struct sframe_state *sf, uint64_t raddr);

struct sframe_stinfo *
sframe_find_context (struct sframe_state *sf, uint64_t addr);

void sframe_free_cfi (struct sframe_state *sf);

#endif /* SFRAME_STATE_H.  */
