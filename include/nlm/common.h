/* NLM (NetWare Loadable Module) support for BFD.
   Copyright (C) 1993 Free Software Foundation, Inc.

   Written by Fred Fish @ Cygnus Support

This file is part of BFD, the Binary File Descriptor library.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* This file is part of NLM support for BFD, and contains the portions
   that are common to both the internal and external representations. */

#define NLM_MAX_DESCRIPTION_LENGTH		127
#define NLM_MAX_SCREEN_NAME_LENGTH		71
#define NLM_MAX_THREAD_NAME_LENGTH		71
#define NLM_MAX_COPYRIGHT_MESSAGE_LENGTH	255
#define NLM_OTHER_DATA_LENGTH 			400		/* FIXME */
#define NLM_OLD_THREAD_NAME_LENGTH		5
#define NLM_SIGNATURE_SIZE			24
#define NLM_SIGNATURE				"NetWare Loadable Module\032"
#define NLM_MODULE_NAME_SIZE			14
#define NLM_DEFAULT_STACKSIZE			(8 * 1024)
