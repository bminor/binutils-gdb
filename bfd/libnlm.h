/* BFD back-end data structures for NLM (NetWare Loadable Modules) files.
   Copyright (C) 1993 Free Software Foundation, Inc.
   Written by Cygnus Support.

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

#ifndef _LIBNLM_H_
#define _LIBNLM_H_ 1

#include "nlm/common.h"
#include "nlm/internal.h"
#include "nlm/external.h"

/* If size isn't specified as 64 or 32, NAME macro should fail.  */
#ifndef NAME
#  if ARCH_SIZE==64
#    define NAME(x,y) CAT4(x,64,_,y)
#  endif
#  if ARCH_SIZE==32
#    define NAME(x,y) CAT4(x,32,_,y)
#  endif
#endif

#define NlmNAME(X)	NAME(Nlm,X)
#define nlmNAME(X)	NAME(nlm,X)

typedef struct
{
  asymbol symbol;
} nlm32_symbol_type;

typedef struct
{
  asymbol symbol;
} nlm64_symbol_type;

#define bfd_nlm32_mkobject		bfd_nlm_mkobject
#define bfd_nlm64_mkobject		bfd_nlm_mkobject
#define nlm_mkobject			bfd_nlm_mkobject
extern boolean bfd_nlm_mkobject PARAMS ((bfd *));

extern void bfd_nlm32_get_symbol_info
  PARAMS ((bfd *, asymbol *, symbol_info *));
extern unsigned int bfd_nlm32_get_symtab_upper_bound PARAMS ((bfd *));
extern unsigned int bfd_nlm64_get_symtab_upper_bound PARAMS ((bfd *));
extern unsigned int bfd_nlm32_get_symtab PARAMS ((bfd *, asymbol **));
extern unsigned int bfd_nlm64_get_symtab PARAMS ((bfd *, asymbol **));
extern asymbol *bfd_nlm32_make_empty_symbol PARAMS ((bfd *));
extern asymbol *bfd_nlm64_make_empty_symbol PARAMS ((bfd *));
extern bfd_target *bfd_nlm32_object_p PARAMS ((bfd *));
extern bfd_target *bfd_nlm64_object_p PARAMS ((bfd *));
extern boolean bfd_nlm32_set_arch_mach
  PARAMS ((bfd *, enum bfd_architecture, unsigned long));
extern boolean bfd_nlm64_set_arch_mach
  PARAMS ((bfd *, enum bfd_architecture, unsigned long));

/* Some private data is stashed away for future use using the tdata pointer
   in the bfd structure.  */

struct nlm_obj_tdata
{
  /* Actual data, but ref like ptr */
  Nlm_Internal_Fixed_Header	nlm_fixed_hdr[1];
  Nlm_Internal_Variable_Header	nlm_variable_hdr[1];
  Nlm_Internal_Version_Header	nlm_version_hdr[1];
  Nlm_Internal_Copyright_Header	nlm_copyright_hdr[1];
  Nlm_Internal_Extended_Header	nlm_extended_hdr[1];
  Nlm_Internal_Custom_Header	nlm_custom_hdr[1];
};

#define nlm_tdata(bfd)			((bfd) -> tdata.nlm_obj_data)
#define nlm_fixed_header(bfd)		(nlm_tdata(bfd) -> nlm_fixed_hdr)
#define nlm_variable_header(bfd)	(nlm_tdata(bfd) -> nlm_variable_hdr)
#define nlm_version_header(bfd)		(nlm_tdata(bfd) -> nlm_version_hdr)
#define nlm_copyright_header(bfd)	(nlm_tdata(bfd) -> nlm_copyright_hdr)
#define nlm_extended_header(bfd)	(nlm_tdata(bfd) -> nlm_extended_hdr)
#define nlm_custom_header(bfd)		(nlm_tdata(bfd) -> nlm_custom_hdr)

/* The NLM code, data, and uninitialized sections have no names defined
   in the NLM, but bfd wants to give them names, so use the traditional
   UNIX names.  */

#define NLM_CODE_NAME			".text"
#define NLM_INITIALIZED_DATA_NAME	".data"
#define NLM_UNINITIALIZED_DATA_NAME	".bss"

#endif /* _LIBNLM_H_ */
