/* Copyright (C) 2006, 2007 Free Software Foundation, Inc.

   Free Software Foundation, Inc.

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

extern int rs6000_software_single_step (struct frame_info *frame);

/* Hook in rs6000-tdep.c for determining the TOC address when
   calling functions in the inferior.  */

extern CORE_ADDR (*rs6000_find_toc_address_hook) (CORE_ADDR);

