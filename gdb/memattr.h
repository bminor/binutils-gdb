/* Memory attributes support, for GDB.

   Copyright (C) 2001, 2006 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#ifndef MEMATTR_H
#define MEMATTR_H

#include "vec.h"

enum mem_access_mode
{
  MEM_RW,			/* read/write */
  MEM_RO,			/* read only */
  MEM_WO,			/* write only */

  /* Read/write, but special steps are required to write to it.  */
  MEM_FLASH
};

enum mem_access_width
{
  MEM_WIDTH_UNSPECIFIED,
  MEM_WIDTH_8,			/*  8 bit accesses */
  MEM_WIDTH_16,			/* 16  "      "    */
  MEM_WIDTH_32,			/* 32  "      "    */
  MEM_WIDTH_64			/* 64  "      "    */
};

/* The set of all attributes that can be set for a memory region.
  
   This structure was created so that memory attributes can be passed
   to target_ functions without exposing the details of memory region
   list, which would be necessary if these fields were simply added to
   the mem_region structure.

   FIXME: It would be useful if there was a mechanism for targets to
   add their own attributes.  For example, the number of wait states. */
 
struct mem_attrib 
{
  /* read/write, read-only, or write-only */
  enum mem_access_mode mode;

  enum mem_access_width width;

  /* enables hardware breakpoints */
  int hwbreak;
  
  /* enables host-side caching of memory region data */
  int cache;
  
  /* enables memory verification.  after a write, memory is re-read
     to verify that the write was successful. */
  int verify; 

  /* Block size.  Only valid if mode == MEM_FLASH.  */
  int blocksize;
};

struct mem_region 
{
  CORE_ADDR lo;
  CORE_ADDR hi;

  /* Item number of this memory region. */
  int number;

  /* Status of this memory region (enabled if non-zero, otherwise disabled) */
  int enabled_p;

  /* Attributes for this region */
  struct mem_attrib attrib;
};

/* Declare a vector type for a group of mem_region structures.  The
   typedef is necessary because vec.h can not handle a struct tag.
   Except during construction, these vectors are kept sorted.  */
typedef struct mem_region mem_region_s;
DEF_VEC_O(mem_region_s);

extern struct mem_region *lookup_mem_region(CORE_ADDR);

void invalidate_target_mem_regions (void);

void mem_region_init (struct mem_region *);

int mem_region_cmp (const void *, const void *);

#endif	/* MEMATTR_H */
