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

/* glibc's elf.h will bring this in future.  */
#ifndef PT_SFRAME
#define PT_SFRAME 0x6474e554
#endif

/* SFrame stacktrace data.  */

struct sframest_info
{
  /* Reference to the SFrame section in process memory.  */
  const char *buf;
  /* Length in bytes of the SFrame section in memory.  */
  uint64_t buflen;
  /* Text segment's virtual address.  */
  uint64_t text_vma;
  /* Text segment's length in bytes.  */
  uint64_t text_size;
  /* SFrame segment's virtual address.  */
  uint64_t sframe_vma;
  /* SFrame decoder context.  For access to decoded SFrame information.  */
  sframe_decoder_ctx *dctx;
};

/* List of SFrame stacktrace info objects.
   Typically used to represent SFrame stacktrace info for set of shared
   libraries of a program.  */

struct sframest_info_list
{
  /* Number of entries allocated.  */
  int alloced;
  /* Number of entries used.  */
  int used;
  /* (Array) List of SFrame stacktrace info objects.  */
  struct sframest_info *entry;
};

/* SFrame stacktracing context.  */

struct sframest_ctx
{
  /* File descriptor for the process memory.  */
  int fd;
  /* SFrame stacktrace info for program.  */
  struct sframest_info prog_sfinfo;
  /* SFrame stacktrace info for its DSOs.  */
  struct sframest_info_list dsos_sfinfo;
};

void sframe_unwind_init_debug (void);

int sframe_callback (struct dl_phdr_info *info,
		     size_t size ATTRIBUTE_UNUSED,
		     void *data);

bool sframest_sfinfo_addr_range_p (struct sframest_info *sfinfo,
				   uint64_t addr);

struct sframest_info *sframest_get_sfinfo (struct sframest_ctx *sfctx,
					   uint64_t raddr);

struct sframest_info *sframe_find_context (struct sframest_ctx *sfctx,
					   uint64_t addr);

void sframest_ctx_free (struct sframest_ctx *sfctx);

#endif /* SFRAME_STATE_H.  */
