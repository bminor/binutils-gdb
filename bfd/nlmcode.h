/* NLM (NetWare Loadable Module) executable support for BFD.
   Copyright (C) 1993 Free Software Foundation, Inc.

   Written by Fred Fish @ Cygnus Support, using ELF support as the
   template.

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

#include <string.h>		/* For strrchr and friends */
#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libnlm.h"

/* The functions in this file do not use the names they appear to use.
   This file is actually compiled multiple times, once for each size
   of NLM target we are using.  At each size we use a different name,
   constructed by the macro nlmNAME.  For example, the function which
   is named nlm_symbol_type below is actually named nlm32_symbol_type
   in the final executable.  */

#define Nlm_External_Fixed_Header	NlmNAME(External_Fixed_Header)
#define Nlm_External_Version_Header	NlmNAME(External_Version_Header)
#define Nlm_External_Copyright_Header	NlmNAME(External_Copyright_Header)
#define Nlm_External_Extended_Header	NlmNAME(External_Extended_Header)
#define Nlm_External_Custom_Header	NlmNAME(External_Custom_Header)

#define nlm_symbol_type			nlmNAME(symbol_type)
#define nlm_get_symtab_upper_bound	nlmNAME(get_symtab_upper_bound)
#define nlm_get_symtab			nlmNAME(get_symtab)
#define nlm_make_empty_symbol		nlmNAME(make_empty_symbol)
#define nlm_print_symbol		nlmNAME(print_symbol)
#define nlm_get_symbol_info		nlmNAME(get_symbol_info)
#define nlm_get_reloc_upper_bound	nlmNAME(get_reloc_upper_bound)
#define nlm_canonicalize_reloc		nlmNAME(canonicalize_reloc)
#define nlm_object_p			nlmNAME(object_p)
#define nlm_set_section_contents	nlmNAME(set_section_contents)
#define nlm_write_object_contents	nlmNAME(write_object_contents)

/* Forward declarations of static functions */

static boolean add_bfd_section
  PARAMS ((bfd *, char *, file_ptr, bfd_size_type, flagword));
static void nlm_swap_fixed_header_in
  PARAMS ((bfd *, Nlm_External_Fixed_Header *, Nlm_Internal_Fixed_Header *));
static void nlm_swap_fixed_header_out
  PARAMS ((bfd *, Nlm_Internal_Fixed_Header *, Nlm_External_Fixed_Header *));
static boolean nlm_swap_variable_header_in
  PARAMS ((bfd *));
static boolean nlm_swap_variable_header_out
  PARAMS ((bfd *));
static boolean find_nonzero
  PARAMS ((PTR, size_t));
static boolean nlm_swap_auxiliary_headers_in
  PARAMS ((bfd *));
static boolean nlm_swap_auxiliary_headers_out
  PARAMS ((bfd *));
static boolean nlm_slurp_symbol_table
  PARAMS ((bfd *));
static boolean nlm_slurp_reloc_fixups
  PARAMS ((bfd *));
static boolean nlm_compute_section_file_positions
  PARAMS ((bfd *));
static int nlm_external_reloc_compare
  PARAMS ((const void *, const void *));

/* Should perhaps use put_offset, put_word, etc.  For now, the two versions
   can be handled by explicitly specifying 32 bits or "the long type".  */
#if ARCH_SIZE == 64
#define put_word	bfd_h_put_64
#define get_word	bfd_h_get_64
#endif
#if ARCH_SIZE == 32
#define put_word	bfd_h_put_32
#define get_word	bfd_h_get_32
#endif

bfd_target *
DEFUN (nlm_object_p, (abfd), bfd * abfd)
{
  Nlm_External_Fixed_Header x_fxdhdr;	/* Nlm file header, external form */
  Nlm_Internal_Fixed_Header *i_fxdhdrp;	/* Nlm file header, internal form */
  enum bfd_architecture arch;

  /* Read in the fixed length portion of the NLM header in external format.  */

  if (bfd_read ((PTR) &x_fxdhdr, sizeof (x_fxdhdr), 1, abfd) !=
      sizeof (x_fxdhdr))
    {
      bfd_error = system_call_error;
      return (NULL);
    }

  /* Check to see if we have an NLM file by matching the NLM signature. */

  if (strncmp (x_fxdhdr.signature, NLM_SIGNATURE, NLM_SIGNATURE_SIZE) != 0)
    {
    wrong:
      bfd_error = wrong_format;
      return (NULL);
    }

  /* There's no supported way to discover the endianess of an NLM, so test for
     a sane version number after doing byte swapping appropriate for this
     XVEC.  (Hack alert!) */

  if (get_word (abfd, (bfd_byte *) x_fxdhdr.version) > 0xFFFF)
    {
      goto wrong;
    }

  /* There's no supported way to check for 32 bit versus 64 bit addresses,
     so ignore this distinction for now.  (FIXME) */

  /* Allocate an instance of the nlm_obj_tdata structure and hook it up to
     the tdata pointer in the bfd. */

  nlm_tdata (abfd) = (struct nlm_obj_tdata *)
    bfd_zalloc (abfd, sizeof (struct nlm_obj_tdata));
  if (nlm_tdata (abfd) == NULL)
    {
      bfd_error = no_memory;
      return (NULL);
    }

  /* FIXME:  Any `wrong' exits below here will leak memory (tdata).  */

  /* Swap in the rest of the fixed length header. */

  i_fxdhdrp = nlm_fixed_header (abfd);
  nlm_swap_fixed_header_in (abfd, &x_fxdhdr, i_fxdhdrp);

  if (!nlm_swap_variable_header_in (abfd)
      || !nlm_swap_auxiliary_headers_in (abfd)
      || !add_bfd_section (abfd, NLM_CODE_NAME,
			   i_fxdhdrp -> codeImageOffset,
			   i_fxdhdrp -> codeImageSize,
			   (SEC_CODE | SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
			    | SEC_RELOC))
      || !add_bfd_section (abfd, NLM_INITIALIZED_DATA_NAME,
			   i_fxdhdrp -> dataImageOffset,
			   i_fxdhdrp -> dataImageSize,
			   (SEC_DATA | SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
			    | SEC_RELOC))
      || !add_bfd_section (abfd, NLM_UNINITIALIZED_DATA_NAME,
			   (file_ptr) 0,
			   i_fxdhdrp -> uninitializedDataSize,
			   SEC_ALLOC))
    {
      return (NULL);
    }

  if (nlm_fixed_header (abfd)->numberOfRelocationFixups != 0
      || nlm_fixed_header (abfd)->numberOfExternalReferences != 0)
    abfd->flags |= HAS_RELOC;
  if (nlm_fixed_header (abfd)->numberOfPublics != 0
      || nlm_fixed_header (abfd)->numberOfDebugRecords != 0)
    abfd->flags |= HAS_SYMS;

  arch = nlm_architecture (abfd);
  if (arch != bfd_arch_unknown)
    bfd_default_set_arch_mach (abfd, arch, (unsigned long) 0);

  return (abfd -> xvec);
}

/* Add a section to the bfd. */

static boolean
DEFUN (add_bfd_section, (abfd, name, offset, size, flags),
       bfd *abfd AND
       char *name AND
       file_ptr offset AND
       bfd_size_type size AND
       flagword flags)
{
  asection *newsect;

  newsect = bfd_make_section (abfd, name);
  if (newsect == NULL)
    {
      return (false);
    }
  newsect -> vma = 0;				/* NLM's are relocatable. */
  newsect -> _raw_size = size;
  newsect -> filepos = offset;
  newsect -> flags = flags;
  newsect -> alignment_power = bfd_log2 (0);	/* FIXME */
  return (true);
}

/* Translate an NLM fixed length file header in external format into an NLM
   file header in internal format. */

static void
DEFUN (nlm_swap_fixed_header_in, (abfd, src, dst),
       bfd * abfd AND
       Nlm_External_Fixed_Header * src AND
       Nlm_Internal_Fixed_Header * dst)
{
  memcpy (dst -> signature, src -> signature, NLM_SIGNATURE_SIZE);
  memcpy (dst -> moduleName, src -> moduleName, NLM_MODULE_NAME_SIZE);
  dst -> version =
    get_word (abfd, (bfd_byte *) src -> version);
  dst -> codeImageOffset =
    get_word (abfd, (bfd_byte *) src -> codeImageOffset);
  dst -> codeImageSize =
    get_word (abfd, (bfd_byte *) src -> codeImageSize);
  dst -> dataImageOffset =
    get_word (abfd, (bfd_byte *) src -> dataImageOffset);
  dst -> dataImageSize =
    get_word (abfd, (bfd_byte *) src -> dataImageSize);
  dst -> uninitializedDataSize =
    get_word (abfd, (bfd_byte *) src -> uninitializedDataSize);
  dst -> customDataOffset =
    get_word (abfd, (bfd_byte *) src -> customDataOffset);
  dst -> customDataSize =
    get_word (abfd, (bfd_byte *) src -> customDataSize);
  dst -> moduleDependencyOffset =
    get_word (abfd, (bfd_byte *) src -> moduleDependencyOffset);
  dst -> numberOfModuleDependencies =
    get_word (abfd, (bfd_byte *) src -> numberOfModuleDependencies);
  dst -> relocationFixupOffset =
    get_word (abfd, (bfd_byte *) src -> relocationFixupOffset);
  dst -> numberOfRelocationFixups =
    get_word (abfd, (bfd_byte *) src -> numberOfRelocationFixups);
  dst -> externalReferencesOffset =
    get_word (abfd, (bfd_byte *) src -> externalReferencesOffset);
  dst -> numberOfExternalReferences =
    get_word (abfd, (bfd_byte *) src -> numberOfExternalReferences);
  dst -> publicsOffset =
    get_word (abfd, (bfd_byte *) src -> publicsOffset);
  dst -> numberOfPublics =
    get_word (abfd, (bfd_byte *) src -> numberOfPublics);
  dst -> debugInfoOffset =
    get_word (abfd, (bfd_byte *) src -> debugInfoOffset);
  dst -> numberOfDebugRecords =
    get_word (abfd, (bfd_byte *) src -> numberOfDebugRecords);
  dst -> codeStartOffset =
    get_word (abfd, (bfd_byte *) src -> codeStartOffset);
  dst -> exitProcedureOffset =
    get_word (abfd, (bfd_byte *) src -> exitProcedureOffset);
  dst -> checkUnloadProcedureOffset =
    get_word (abfd, (bfd_byte *) src -> checkUnloadProcedureOffset);
  dst -> moduleType =
    get_word (abfd, (bfd_byte *) src -> moduleType);
  dst -> flags = 
    get_word (abfd, (bfd_byte *) src -> flags);
}

/* Translate an NLM fixed length file header in internal format into
   an NLM file header in external format. */

static void
DEFUN (nlm_swap_fixed_header_out, (abfd, src, dst),
       bfd * abfd AND
       Nlm_Internal_Fixed_Header * src AND
       Nlm_External_Fixed_Header * dst)
{
  memcpy (dst -> signature, src -> signature, NLM_SIGNATURE_SIZE);
  memcpy (dst -> moduleName, src -> moduleName, NLM_MODULE_NAME_SIZE);
  put_word (abfd, (bfd_vma) src -> version,
	    (bfd_byte *) dst -> version);
  put_word (abfd, (bfd_vma) src -> codeImageOffset,
	    (bfd_byte *) dst -> codeImageOffset);
  put_word (abfd, (bfd_vma) src -> codeImageSize,
	    (bfd_byte *) dst -> codeImageSize);
  put_word (abfd, (bfd_vma) src -> dataImageOffset,
	    (bfd_byte *) dst -> dataImageOffset);
  put_word (abfd, (bfd_vma) src -> dataImageSize,
	    (bfd_byte *) dst -> dataImageSize);
  put_word (abfd, (bfd_vma) src -> uninitializedDataSize,
	    (bfd_byte *) dst -> uninitializedDataSize);
  put_word (abfd, (bfd_vma) src -> customDataOffset,
	    (bfd_byte *) dst -> customDataOffset);
  put_word (abfd, (bfd_vma) src -> customDataSize,
	    (bfd_byte *) dst -> customDataSize);
  put_word (abfd, (bfd_vma) src -> moduleDependencyOffset,
	    (bfd_byte *) dst -> moduleDependencyOffset);
  put_word (abfd, (bfd_vma) src -> numberOfModuleDependencies,
	    (bfd_byte *) dst -> numberOfModuleDependencies);
  put_word (abfd, (bfd_vma) src -> relocationFixupOffset,
	    (bfd_byte *) dst -> relocationFixupOffset);
  put_word (abfd, (bfd_vma) src -> numberOfRelocationFixups,
	    (bfd_byte *) dst -> numberOfRelocationFixups);
  put_word (abfd, (bfd_vma) src -> externalReferencesOffset,
	    (bfd_byte *) dst -> externalReferencesOffset);
  put_word (abfd, (bfd_vma) src -> numberOfExternalReferences,
	    (bfd_byte *) dst -> numberOfExternalReferences);
  put_word (abfd, (bfd_vma) src -> publicsOffset,
	    (bfd_byte *) dst -> publicsOffset);
  put_word (abfd, (bfd_vma) src -> numberOfPublics,
	    (bfd_byte *) dst -> numberOfPublics);
  put_word (abfd, (bfd_vma) src -> debugInfoOffset,
	    (bfd_byte *) dst -> debugInfoOffset);
  put_word (abfd, (bfd_vma) src -> numberOfDebugRecords,
	    (bfd_byte *) dst -> numberOfDebugRecords);
  put_word (abfd, (bfd_vma) src -> codeStartOffset,
	    (bfd_byte *) dst -> codeStartOffset);
  put_word (abfd, (bfd_vma) src -> exitProcedureOffset,
	    (bfd_byte *) dst -> exitProcedureOffset);
  put_word (abfd, (bfd_vma) src -> checkUnloadProcedureOffset,
	    (bfd_byte *) dst -> checkUnloadProcedureOffset);
  put_word (abfd, (bfd_vma) src -> moduleType,
	    (bfd_byte *) dst -> moduleType);
  put_word (abfd, (bfd_vma) src -> flags,
	    (bfd_byte *) dst -> flags);
}

/* Read and swap in the variable length header.  All the fields must
   exist in the NLM, and must exist in the order they are read here. */

static boolean
DEFUN (nlm_swap_variable_header_in, (abfd),
       bfd * abfd)
{
  unsigned char temp [NLM_TARGET_LONG_SIZE];

  /* Read the description length and text members. */

  if (bfd_read ((PTR) &nlm_variable_header (abfd) -> descriptionLength,
		sizeof (nlm_variable_header (abfd) -> descriptionLength),
		1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> descriptionLength))
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_read ((PTR) nlm_variable_header (abfd) -> descriptionText,
		nlm_variable_header (abfd) -> descriptionLength + 1,
		1, abfd) !=
      nlm_variable_header (abfd) -> descriptionLength + 1)
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Read and convert the stackSize field. */

  if (bfd_read ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
    {
      bfd_error = system_call_error;
      return (false);
    }
  nlm_variable_header (abfd) -> stackSize = get_word (abfd, (bfd_byte *) temp);

  /* Read and convert the reserved field. */

  if (bfd_read ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
    {
      bfd_error = system_call_error;
      return (false);
    }
  nlm_variable_header (abfd) -> reserved = get_word (abfd, (bfd_byte *) temp);

  /* Read the oldThreadName field.  This field is a fixed length string. */

  if (bfd_read ((PTR) nlm_variable_header (abfd) -> oldThreadName,
		sizeof (nlm_variable_header (abfd) -> oldThreadName),
		1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> oldThreadName))
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Read the screen name length and text members. */

  if (bfd_read ((PTR) &nlm_variable_header (abfd) -> screenNameLength,
		sizeof (nlm_variable_header (abfd) -> screenNameLength),
		1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> screenNameLength))
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_read ((PTR) nlm_variable_header (abfd) -> screenName,
		nlm_variable_header (abfd) -> screenNameLength + 1,
		1, abfd) !=
      nlm_variable_header (abfd) -> screenNameLength + 1)
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Read the thread name length and text members. */

  if (bfd_read ((PTR) &nlm_variable_header (abfd) -> threadNameLength,
		sizeof (nlm_variable_header (abfd) -> threadNameLength),
		1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> threadNameLength))
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_read ((PTR) nlm_variable_header (abfd) -> threadName,
		nlm_variable_header (abfd) -> threadNameLength + 1,
		1, abfd) !=
      nlm_variable_header (abfd) -> threadNameLength + 1)
    {
      bfd_error = system_call_error;
      return (false);
    }
  return (true);
}

/* Swap and write out the variable length header.  All the fields must
   exist in the NLM, and must exist in this order.  */

static boolean
DEFUN (nlm_swap_variable_header_out, (abfd),
       bfd * abfd)
{
  unsigned char temp [NLM_TARGET_LONG_SIZE];

  /* Write the description length and text members. */

  if (bfd_write ((PTR) &nlm_variable_header (abfd) -> descriptionLength,
		 sizeof (nlm_variable_header (abfd) -> descriptionLength),
		 1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> descriptionLength))
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_write ((PTR) nlm_variable_header (abfd) -> descriptionText,
		 nlm_variable_header (abfd) -> descriptionLength + 1,
		 1, abfd) !=
      nlm_variable_header (abfd) -> descriptionLength + 1)
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Convert and write the stackSize field. */

  put_word (abfd, (bfd_vma) nlm_variable_header (abfd) -> stackSize,
	    (bfd_byte *) temp);
  if (bfd_write ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Convert and write the reserved field. */

  put_word (abfd, (bfd_vma) nlm_variable_header (abfd) -> reserved,
	    (bfd_byte *) temp);
  if (bfd_write ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Write the oldThreadName field.  This field is a fixed length string. */

  if (bfd_write ((PTR) nlm_variable_header (abfd) -> oldThreadName,
		 sizeof (nlm_variable_header (abfd) -> oldThreadName),
		 1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> oldThreadName))
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Write the screen name length and text members. */

  if (bfd_write ((PTR) &nlm_variable_header (abfd) -> screenNameLength,
		 sizeof (nlm_variable_header (abfd) -> screenNameLength),
		 1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> screenNameLength))
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_write ((PTR) nlm_variable_header (abfd) -> screenName,
		 nlm_variable_header (abfd) -> screenNameLength + 1,
		 1, abfd) !=
      nlm_variable_header (abfd) -> screenNameLength + 1)
    {
      bfd_error = system_call_error;
      return (false);
    }

  /* Write the thread name length and text members. */

  if (bfd_write ((PTR) &nlm_variable_header (abfd) -> threadNameLength,
		 sizeof (nlm_variable_header (abfd) -> threadNameLength),
		 1, abfd) !=
      sizeof (nlm_variable_header (abfd) -> threadNameLength))
    {
      bfd_error = system_call_error;
      return (false);
    }
  if (bfd_write ((PTR) nlm_variable_header (abfd) -> threadName,
		 nlm_variable_header (abfd) -> threadNameLength + 1,
		 1, abfd) !=
      nlm_variable_header (abfd) -> threadNameLength + 1)
    {
      bfd_error = system_call_error;
      return (false);
    }
  return (true);
}

/* Read and swap in the contents of all the auxiliary headers.  Because of
   the braindead design, we have to do strcmps on strings of indeterminate
   length to figure out what each auxiliary header is.  Even worse, we have
   no way of knowing how many auxiliary headers there are or where the end
   of the auxiliary headers are, except by finding something that doesn't
   look like a known auxiliary header.  This means that the first new type
   of auxiliary header added will break all existing tools that don't
   recognize it. */

static boolean
DEFUN (nlm_swap_auxiliary_headers_in, (abfd),
       bfd * abfd)
{
  unsigned char tempstr [16];
  long position;

  for (;;)
    {
      position = bfd_tell (abfd);
      if (bfd_read ((PTR) tempstr, sizeof (tempstr), 1, abfd) !=
	  sizeof (tempstr))
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      if (bfd_seek (abfd, position, SEEK_SET) == -1)
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      if (strncmp (tempstr, "VeRsIoN#", 8) == 0)
	{
	  Nlm_External_Version_Header thdr;
	  if (bfd_read ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  memcpy (nlm_version_header (abfd) -> stamp, thdr.stamp,
		  sizeof (thdr.stamp));
	  nlm_version_header (abfd) -> majorVersion =
	    get_word (abfd, (bfd_byte *) thdr.majorVersion);
	  nlm_version_header (abfd) -> minorVersion =
	    get_word (abfd, (bfd_byte *) thdr.minorVersion);
	  nlm_version_header (abfd) -> revision =
	    get_word (abfd, (bfd_byte *) thdr.revision);
	  nlm_version_header (abfd) -> year =
	    get_word (abfd, (bfd_byte *) thdr.year);
	  nlm_version_header (abfd) -> month =
	    get_word (abfd, (bfd_byte *) thdr.month);
	  nlm_version_header (abfd) -> day =
	    get_word (abfd, (bfd_byte *) thdr.day);
	}
      else if (strncmp (tempstr, "MeSsAgEs", 8) == 0)
	{
	  Nlm_External_Extended_Header thdr;
	  if (bfd_read ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  memcpy (nlm_extended_header (abfd) -> stamp, thdr.stamp,
		  sizeof (thdr.stamp));
	  nlm_extended_header (abfd) -> languageID =
	    get_word (abfd, (bfd_byte *) thdr.languageID);
	  nlm_extended_header (abfd) -> messageFileOffset =
	    get_word (abfd, (bfd_byte *) thdr.messageFileOffset);
	  nlm_extended_header (abfd) -> messageFileLength =
	    get_word (abfd, (bfd_byte *) thdr.messageFileLength);
	  nlm_extended_header (abfd) -> messageCount =
	    get_word (abfd, (bfd_byte *) thdr.messageCount);
	  nlm_extended_header (abfd) -> helpFileOffset =
	    get_word (abfd, (bfd_byte *) thdr.helpFileOffset);
	  nlm_extended_header (abfd) -> helpFileLength =
	    get_word (abfd, (bfd_byte *) thdr.helpFileLength);
	  nlm_extended_header (abfd) -> RPCDataOffset =
	    get_word (abfd, (bfd_byte *) thdr.RPCDataOffset);
	  nlm_extended_header (abfd) -> RPCDataLength =
	    get_word (abfd, (bfd_byte *) thdr.RPCDataLength);
	  nlm_extended_header (abfd) -> sharedCodeOffset =
	    get_word (abfd, (bfd_byte *) thdr.sharedCodeOffset);
	  nlm_extended_header (abfd) -> sharedCodeLength =
	    get_word (abfd, (bfd_byte *) thdr.sharedCodeLength);
	  nlm_extended_header (abfd) -> sharedDataOffset =
	    get_word (abfd, (bfd_byte *) thdr.sharedDataOffset);
	  nlm_extended_header (abfd) -> sharedDataLength =
	    get_word (abfd, (bfd_byte *) thdr.sharedDataLength);
	  nlm_extended_header (abfd) -> sharedRelocationFixupOffset =
	    get_word (abfd, (bfd_byte *) thdr.sharedRelocationFixupOffset);
	  nlm_extended_header (abfd) -> sharedRelocationFixupCount =
	    get_word (abfd, (bfd_byte *) thdr.sharedRelocationFixupCount);
	  nlm_extended_header (abfd) -> sharedExternalReferenceOffset =
	    get_word (abfd, (bfd_byte *) thdr.sharedExternalReferenceOffset);
	  nlm_extended_header (abfd) -> sharedExternalReferenceCount =
	    get_word (abfd, (bfd_byte *) thdr.sharedExternalReferenceCount);
	  nlm_extended_header (abfd) -> sharedPublicsOffset =
	    get_word (abfd, (bfd_byte *) thdr.sharedPublicsOffset);
	  nlm_extended_header (abfd) -> sharedPublicsCount =
	    get_word (abfd, (bfd_byte *) thdr.sharedPublicsCount);
	  nlm_extended_header (abfd) -> SharedInitializationOffset =
	    get_word (abfd, (bfd_byte *) thdr.sharedInitializationOffset);
	  nlm_extended_header (abfd) -> SharedExitProcedureOffset =
	    get_word (abfd, (bfd_byte *) thdr.SharedExitProcedureOffset);
	  nlm_extended_header (abfd) -> productID =
	    get_word (abfd, (bfd_byte *) thdr.productID);
	  nlm_extended_header (abfd) -> reserved0 =
	    get_word (abfd, (bfd_byte *) thdr.reserved0);
	  nlm_extended_header (abfd) -> reserved1 =
	    get_word (abfd, (bfd_byte *) thdr.reserved1);
	  nlm_extended_header (abfd) -> reserved2 =
	    get_word (abfd, (bfd_byte *) thdr.reserved2);
	  nlm_extended_header (abfd) -> reserved3 =
	    get_word (abfd, (bfd_byte *) thdr.reserved3);
	  nlm_extended_header (abfd) -> reserved4 =
	    get_word (abfd, (bfd_byte *) thdr.reserved4);
	  nlm_extended_header (abfd) -> reserved5 =
	    get_word (abfd, (bfd_byte *) thdr.reserved5);
	}
      else if (strncmp (tempstr, "CuStHeAd", 8) == 0)
	{
	  Nlm_External_Custom_Header thdr;
	  if (bfd_read ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  memcpy (nlm_custom_header (abfd) -> stamp, thdr.stamp,
		  sizeof (thdr.stamp));
	  nlm_custom_header (abfd) -> dataLength =
	    get_word (abfd, (bfd_byte *) thdr.dataLength);
	  nlm_custom_header (abfd) -> debugRecOffset =
	    get_word (abfd, (bfd_byte *) thdr.debugRecOffset);
	  nlm_custom_header (abfd) -> debugRecLength =
	    get_word (abfd, (bfd_byte *) thdr.debugRecLength);
	}
      else if (strncmp (tempstr, "CoPyRiGhT=", 10) == 0)
	{
	  Nlm_External_Copyright_Header thdr;
	  if (bfd_read ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  memcpy (nlm_copyright_header (abfd) -> stamp, thdr.stamp,
		  sizeof (thdr.stamp));
	  nlm_copyright_header (abfd) -> copyrightMessageLength =
	    get_word (abfd, (bfd_byte *) thdr.copyrightMessageLength);
	  /* The copyright message is a variable length string. */
	  if (bfd_read ((PTR) nlm_copyright_header (abfd) -> copyrightMessage,
			nlm_copyright_header (abfd) -> copyrightMessageLength + 1,
			1, abfd) !=
	      nlm_copyright_header (abfd) -> copyrightMessageLength + 1)
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	}
      else
	{
	  break;
	}
    }
  return (true);
}

/* Return whether there is a non-zero byte in a memory block.  */

static boolean
find_nonzero (buf, size)
     PTR buf;
     size_t size;
{
  char *p = (char *) buf;

  while (size-- != 0)
    if (*p++ != 0)
      return true;
  return false;
}

/* Swap out the contents of the auxiliary headers.  We create those
   auxiliary headers which have been set non-zero.  We do not require
   the caller to set up the stamp fields.  */

static boolean
nlm_swap_auxiliary_headers_out (abfd)
     bfd *abfd;
{
  /* Write out the version header if there is one.  */
  if (find_nonzero ((PTR) nlm_version_header (abfd),
		    sizeof (Nlm_Internal_Version_Header)))
    {
      Nlm_External_Version_Header thdr;

      memcpy (thdr.stamp, "VeRsIoN#", 8);
      put_word (abfd, (bfd_vma) nlm_version_header (abfd) -> majorVersion,
		(bfd_byte *) thdr.majorVersion);
      put_word (abfd, (bfd_vma) nlm_version_header (abfd) -> minorVersion,
		(bfd_byte *) thdr.minorVersion);
      put_word (abfd, (bfd_vma) nlm_version_header (abfd) -> revision,
		(bfd_byte *) thdr.revision);
      put_word (abfd, (bfd_vma) nlm_version_header (abfd) -> year,
		(bfd_byte *) thdr.year);
      put_word (abfd, (bfd_vma) nlm_version_header (abfd) -> month,
		(bfd_byte *) thdr.month);
      put_word (abfd, (bfd_vma) nlm_version_header (abfd) -> day,
		(bfd_byte *) thdr.day);
      if (bfd_write ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	  {
	    bfd_error = system_call_error;
	    return false;
	  }
    }

  /* Write out the extended header if there is one.  */
  if (find_nonzero ((PTR) nlm_extended_header (abfd),
		    sizeof (Nlm_Internal_Extended_Header)))
    {
      Nlm_External_Extended_Header thdr;

      memcpy (thdr.stamp, "MeSsAgEs", 8);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> languageID,
		(bfd_byte *) thdr.languageID);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> messageFileOffset,
		(bfd_byte *) thdr.messageFileOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> messageFileLength,
		(bfd_byte *) thdr.messageFileLength);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> messageCount,
		(bfd_byte *) thdr.messageCount);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> helpFileOffset,
		(bfd_byte *) thdr.helpFileOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> helpFileLength,
		(bfd_byte *) thdr.helpFileLength);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> RPCDataOffset,
		(bfd_byte *) thdr.RPCDataOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> RPCDataLength,
		(bfd_byte *) thdr.RPCDataLength);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedCodeOffset,
		(bfd_byte *) thdr.sharedCodeOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedCodeLength,
		(bfd_byte *) thdr.sharedCodeLength);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedDataOffset,
		(bfd_byte *) thdr.sharedDataOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedDataLength,
		(bfd_byte *) thdr.sharedDataLength);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedRelocationFixupOffset,
		(bfd_byte *) thdr.sharedRelocationFixupOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedRelocationFixupCount,
		(bfd_byte *) thdr.sharedRelocationFixupCount);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedExternalReferenceOffset,
		(bfd_byte *) thdr.sharedExternalReferenceOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedExternalReferenceCount,
		(bfd_byte *) thdr.sharedExternalReferenceCount);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedPublicsOffset,
		(bfd_byte *) thdr.sharedPublicsOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> sharedPublicsCount,
		(bfd_byte *) thdr.sharedPublicsCount);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> SharedInitializationOffset,
		(bfd_byte *) thdr.sharedInitializationOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> SharedExitProcedureOffset,
		(bfd_byte *) thdr.SharedExitProcedureOffset);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> productID,
		(bfd_byte *) thdr.productID);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> reserved0,
		(bfd_byte *) thdr.reserved0);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> reserved1,
		(bfd_byte *) thdr.reserved1);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> reserved2,
		(bfd_byte *) thdr.reserved2);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> reserved3,
		(bfd_byte *) thdr.reserved3);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> reserved4,
		(bfd_byte *) thdr.reserved4);
      put_word (abfd,
		(bfd_vma) nlm_extended_header (abfd) -> reserved5,
		(bfd_byte *) thdr.reserved5);
      if (bfd_write ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	  {
	    bfd_error = system_call_error;
	    return false;
	  }
    }

  /* Write out the custom header if there is one.   */
  if (find_nonzero ((PTR) nlm_custom_header (abfd),
		    sizeof (Nlm_Internal_Custom_Header)))
    {
      Nlm_External_Custom_Header thdr;

      /* Right now we assume the custom header is always the suggested
	 format for alternate debugging records.  */
      BFD_ASSERT (nlm_custom_header (abfd) -> dataLength == 8);

      memcpy (thdr.stamp, "CuStHeAd", 8);
      put_word (abfd, (bfd_vma) nlm_custom_header (abfd) -> dataLength,
		(bfd_byte *) thdr.dataLength);
      put_word (abfd, (bfd_vma) nlm_custom_header (abfd) -> debugRecOffset,
		(bfd_byte *) thdr.debugRecOffset);
      put_word (abfd, (bfd_vma) nlm_custom_header (abfd) -> debugRecLength,
		(bfd_byte *) thdr.debugRecLength);
      if (bfd_write ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	  {
	    bfd_error = system_call_error;
	    return false;
	  }
    }

  /* Write out the copyright header if there is one.  */
  if (find_nonzero ((PTR) nlm_copyright_header (abfd),
		    sizeof (Nlm_Internal_Copyright_Header)))
    {
      Nlm_External_Copyright_Header thdr;

      memcpy (thdr.stamp, "CoPyRiGhT=", 10);
      put_word (abfd,
		(bfd_vma) nlm_copyright_header (abfd) -> copyrightMessageLength,
		(bfd_byte *) thdr.copyrightMessageLength);
      if (bfd_write ((PTR) &thdr, sizeof (thdr), 1, abfd) != sizeof (thdr))
	  {
	    bfd_error = system_call_error;
	    return false;
	  }
      /* The copyright message is a variable length string. */
      if (bfd_write ((PTR) nlm_copyright_header (abfd) -> copyrightMessage,
		     nlm_copyright_header (abfd) -> copyrightMessageLength + 1,
		     1, abfd) !=
	  nlm_copyright_header (abfd) -> copyrightMessageLength + 1)
	{
	  bfd_error = system_call_error;
	  return false;
	}
    }

  return true;
}

/* We read the NLM's public symbols and use it to generate a bfd symbol
   table (hey, it's better than nothing) on a one-for-one basis.  Thus
   use the number of public symbols as the number of bfd symbols we will
   have once we actually get around to reading them in.

   Return the number of bytes required to hold the symtab vector, based on
   the count plus 1, since we will NULL terminate the vector allocated based
   on this size. */

unsigned int
DEFUN (nlm_get_symtab_upper_bound, (abfd), bfd * abfd)
{
  Nlm_Internal_Fixed_Header *i_fxdhdrp;	/* Nlm file header, internal form */
  unsigned int symcount;
  unsigned int symtab_size = 0;

  i_fxdhdrp = nlm_fixed_header (abfd);
  symcount = (i_fxdhdrp -> numberOfPublics
	      + i_fxdhdrp -> numberOfExternalReferences);
  symtab_size = (symcount + 1) * (sizeof (asymbol));
  return (symtab_size);
}

/* Note that bfd_get_symcount is guaranteed to be zero if slurping the
   symbol table fails. */

unsigned int
nlm_get_symtab (abfd, alocation)
     bfd *abfd;
     asymbol **alocation;
{
  nlm_symbol_type *symbase;
  bfd_size_type counter = 0;

  if (nlm_slurp_symbol_table (abfd) == false)
    return 0;
  symbase = nlm_get_symbols (abfd);
  while (counter < bfd_get_symcount (abfd))
    {
      *alocation++ = &symbase->symbol;
      symbase++;
      counter++;
    }
  *alocation = (asymbol *) NULL;
  return bfd_get_symcount (abfd);
}

/* Make an NLM symbol.  There is nothing special to do here.  */

asymbol *
nlm_make_empty_symbol (abfd)
     bfd * abfd;
{
  nlm_symbol_type *new;

  new = (nlm_symbol_type *) bfd_zalloc (abfd, sizeof (nlm_symbol_type));
  new->symbol.the_bfd = abfd;
  return &new->symbol;
}

/* Get symbol information.  */

void
nlm_get_symbol_info (ignore_abfd, symbol, ret)
     bfd * ignore_abfd;
     asymbol * symbol;
     symbol_info * ret;
{
  bfd_symbol_info (symbol, ret);
}

/* Print symbol information.  */

void
nlm_print_symbol (abfd, afile, symbol, how)
     bfd *abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *) afile;

  switch (how)
    {
    case bfd_print_symbol_name:
    case bfd_print_symbol_more:
      if (symbol->name)
	fprintf (file,"%s", symbol->name);
      break;
    case bfd_print_symbol_all:
      bfd_print_symbol_vandf ((PTR) file, symbol);
      fprintf (file, " %-5s", symbol->section->name);
      if (symbol->name)
	fprintf (file," %s", symbol->name);
      break;
    }
}

/* Slurp in nlm symbol table.

   In the external (in-file) form, NLM export records are variable length,
   with the following form:

	1 byte		length of the symbol name (N)
	N bytes		the symbol name
	4 bytes		the symbol offset from start of it's section

   Note that we currently ignore the internal debug records.  There is
   a lot of duplication between the export records and the internal debug
   records.  We may in the future, want to merge the information from the
   debug records with the information from the export records to produce
   a more complete symbol table, treating additional information from the
   debug records as static symbols. (FIXME)

   We do read in the import records.  These are treated as undefined
   symbols.  As we read them in we also read in the associated reloc
   information, which is attached to the symbol.

   The bfd symbols are copied to SYMPTRS.

   When we return, the bfd symcount is either zero or contains the correct
   number of symbols.
*/

static boolean
nlm_slurp_symbol_table (abfd)
     bfd *abfd;
{
  Nlm_Internal_Fixed_Header *i_fxdhdrp;	/* Nlm file header, internal form */
  bfd_size_type totsymcount;		/* Number of NLM symbols */
  bfd_size_type symcount;		/* Counter of NLM symbols */
  nlm_symbol_type *sym;			/* Pointer to current bfd symbol */
  char symlength;			/* Symbol length read into here */
  bfd_size_type rcount;			/* Number of relocs */
  bfd_byte temp[NLM_TARGET_LONG_SIZE];	/* Symbol offsets read into here */
  boolean (*read_reloc_func) PARAMS ((bfd *, nlm_symbol_type *, asection **,
				      arelent *));

  if (nlm_get_symbols (abfd) != NULL)
    return (true);

  /* Read each raw NLM symbol, using the information to create a canonical bfd
     symbol table entry.

     Note that we allocate the initial bfd canonical symbol buffer based on a
     one-to-one mapping of the NLM symbols to canonical symbols.  We actually
     use all the NLM symbols, so there will be no space left over at the end.
     When we have all the symbols, we build the caller's pointer vector. */

  abfd -> symcount = 0;
  i_fxdhdrp = nlm_fixed_header (abfd);
  totsymcount = (i_fxdhdrp -> numberOfPublics
		 + i_fxdhdrp -> numberOfExternalReferences);
  if (totsymcount == 0)
    {
      return (true);
    }

  if (bfd_seek (abfd, i_fxdhdrp -> publicsOffset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (false);
    }

  sym = ((nlm_symbol_type *)
	 bfd_zalloc (abfd, totsymcount * sizeof (nlm_symbol_type)));
  nlm_set_symbols (abfd, sym);

  /* We use the bfd's symcount directly as the control count, so that early
     termination of the loop leaves the symcount correct for the symbols that
     were read. */

  symcount = i_fxdhdrp -> numberOfPublics;
  while (abfd -> symcount < symcount)
    {
      if (bfd_read ((PTR) &symlength, sizeof (symlength), 1, abfd)
	  != sizeof (symlength))
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      sym -> symbol.the_bfd = abfd;
      sym -> symbol.name = bfd_alloc (abfd, symlength + 1);
      if (bfd_read ((PTR) sym -> symbol.name, symlength, 1, abfd)
	  != symlength)
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      if (bfd_read ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      sym -> symbol.flags = BSF_GLOBAL | BSF_EXPORT;
      sym -> symbol.value = get_word (abfd, temp);
      if (sym -> symbol.value & NLM_HIBIT)
	{
	  sym -> symbol.value &= ~NLM_HIBIT;
	  sym -> symbol.flags |= BSF_FUNCTION;
	  sym -> symbol.section =
	    bfd_get_section_by_name (abfd, NLM_CODE_NAME);
	}
      else
	{
	  sym -> symbol.section =
	    bfd_get_section_by_name (abfd, NLM_INITIALIZED_DATA_NAME);
	}
      sym -> rcnt = 0;
      abfd -> symcount++;
      sym++;
    }

  /* Read in the import records.  We can only do this if we know how
     to read relocs for this target.  */

  read_reloc_func = nlm_read_reloc_func (abfd);
  if (read_reloc_func != NULL)
    {
      if (bfd_seek (abfd, i_fxdhdrp -> externalReferencesOffset, SEEK_SET)
	  == -1)
	{
	  bfd_error = system_call_error;
	  return (false);
	}
  
      symcount += i_fxdhdrp -> numberOfExternalReferences;
      while (abfd -> symcount < symcount)
	{
	  struct nlm_relent *nlm_relocs;

	  if (bfd_read ((PTR) &symlength, sizeof (symlength), 1, abfd)
	      != sizeof (symlength))
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  sym -> symbol.the_bfd = abfd;
	  sym -> symbol.name = bfd_alloc (abfd, symlength + 1);
	  if (bfd_read ((PTR) sym -> symbol.name, symlength, 1, abfd)
	      != symlength)
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  sym -> symbol.flags = 0;
	  sym -> symbol.value = 0;
	  sym -> symbol.section = &bfd_und_section;
	  if (bfd_read ((PTR) temp, sizeof (temp), 1, abfd) != sizeof (temp))
	    {
	      bfd_error = system_call_error;
	      return (false);
	    }
	  rcount = get_word (abfd, temp);
	  nlm_relocs = ((struct nlm_relent *)
			bfd_alloc (abfd, rcount * sizeof (struct nlm_relent)));
	  sym -> relocs = nlm_relocs;
	  sym -> rcnt = 0;
	  while (sym -> rcnt < rcount)
	    {
	      asection *section;

	      if ((*read_reloc_func) (abfd, sym, &section,
				      &nlm_relocs -> reloc)
		  == false)
		return false;
	      nlm_relocs -> section = section;
	      nlm_relocs++;
	      sym -> rcnt++;
	    }

	  abfd -> symcount++;
	  sym++;
	}
    }

  return (true);
}

/* Get the relocs for an NLM file.  There are two types of relocs.
   Imports are relocs against symbols defined in other NLM files.  We
   treat these as relocs against global symbols.  Relocation fixups
   are internal relocs.

   The actual format used to store the relocs is machine specific.  */

/* Read in the relocation fixup information.  This is stored in
   nlm_relocation_fixups, an array of arelent structures, and
   nlm_relocation_fixup_secs, an array of section pointers.  The
   section pointers are needed because the relocs are not sorted by
   section.  */

static boolean
nlm_slurp_reloc_fixups (abfd)
     bfd *abfd;
{
  boolean (*read_func) PARAMS ((bfd *, nlm_symbol_type *, asection **,
				arelent *));
  bfd_size_type count;
  arelent *rels;
  asection **secs;

  if (nlm_relocation_fixups (abfd) != NULL)
    return true;
  read_func = nlm_read_reloc_func (abfd);
  if (read_func == NULL)
    return true;

  if (bfd_seek (abfd, nlm_fixed_header (abfd)->relocationFixupOffset,
		SEEK_SET) != 0)
    {
      bfd_error = system_call_error;
      return false;
    }

  count = nlm_fixed_header (abfd)->numberOfRelocationFixups;
  rels = (arelent *) bfd_alloc (abfd, count * sizeof (arelent));
  secs = (asection **) bfd_alloc (abfd, count * sizeof (asection *));
  if (rels == NULL || secs == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  nlm_relocation_fixups (abfd) = rels;
  nlm_relocation_fixup_secs (abfd) = secs;

  /* We have to read piece by piece, because we don't know how large
     the machine specific reloc information is.  */
  while (count-- != 0)
    {
      if ((*read_func) (abfd, (nlm_symbol_type *) NULL, secs, rels) == false)
	{
	  nlm_relocation_fixups (abfd) = NULL;
	  nlm_relocation_fixup_secs (abfd) = NULL;
	  return false;
	}
      ++secs;
      ++rels;
    }

  return true;
}

/* Get the number of relocs.  This really just returns an upper bound,
   since it does not attempt to distinguish them based on the section.
   That will be handled when they are actually read.  */

unsigned int
nlm_get_reloc_upper_bound (abfd, sec)
     bfd *abfd;
     asection *sec;
{
  nlm_symbol_type *syms;
  bfd_size_type count;
  unsigned int ret;

  /* If we don't know how to read relocs, just return 0.  */
  if (nlm_read_reloc_func (abfd) == NULL)
    return 0;
  /* Make sure we have either the code or the data section.  */
  if ((bfd_get_section_flags (abfd, sec) & (SEC_CODE | SEC_DATA)) == 0)
    return 0;

  syms = nlm_get_symbols (abfd);
  if (syms == NULL)
    {
      if (nlm_slurp_symbol_table (abfd) == NULL)
	return 0;
      syms = nlm_get_symbols (abfd);
    }

  ret = nlm_fixed_header (abfd)->numberOfRelocationFixups;

  count = bfd_get_symcount (abfd);
  while (count-- != 0)
    {
      ret += syms->rcnt;
      ++syms;
    }

  return (ret + 1) * sizeof (arelent *);
}

/* Get the relocs themselves.  */

unsigned int
nlm_canonicalize_reloc (abfd, sec, relptr, symbols)
     bfd *abfd;
     asection *sec;
     arelent **relptr;
     asymbol **symbols;
{
  arelent *rels;
  asection **secs;
  bfd_size_type count, i;
  unsigned int ret;

  /* Get the relocation fixups.  */
  rels = nlm_relocation_fixups (abfd);
  if (rels == NULL)
    {
      if (nlm_slurp_reloc_fixups (abfd) == NULL)
	return 0;
      rels = nlm_relocation_fixups (abfd);
      if (rels == NULL)
	return 0;
    }
  secs = nlm_relocation_fixup_secs (abfd);

  ret = 0;
  count = nlm_fixed_header (abfd)->numberOfRelocationFixups;
  for (i = 0; i < count; i++, rels++, secs++)
    {
      if (*secs == sec)
	{
	  *relptr++ = rels;
	  ++ret;
	}
    }

  /* Get the import symbols.  */
  count = bfd_get_symcount (abfd);
  for (i = 0; i < count; i++, symbols++)
    {
      asymbol *sym;

      sym = *symbols;
      if (bfd_asymbol_flavour (sym) == bfd_target_nlm_flavour)
	{
	  nlm_symbol_type *nlm_sym;
	  bfd_size_type j;

	  nlm_sym = (nlm_symbol_type *) sym;
	  for (j = 0; j < nlm_sym->rcnt; j++)
	    {
	      if (nlm_sym->relocs[j].section == sec)
		{
		  *relptr = &nlm_sym->relocs[j].reloc;
		  (*relptr)->sym_ptr_ptr = symbols;
		  ++relptr;
		  ++ret;
		}
	    }
	}
    }

  *relptr = NULL;

  return ret;  
}

/* Compute the section file positions for an NLM file.  All variable
   length data in the file headers must be set before this function is
   called.  If the variable length data is changed later, the
   resulting object file will be incorrect.  Unfortunately, there is
   no way to check this.

   This routine also sets the Size and Offset fields in the fixed
   header.  */

static boolean
nlm_compute_section_file_positions (abfd)
     bfd *abfd;
{
  file_ptr sofar;
  asection *sec;
  bfd_vma text, data, bss;
  bfd_vma text_low, data_low;
  int text_align, data_align, other_align;
  file_ptr text_ptr, data_ptr, other_ptr;

  if (abfd->output_has_begun == true)
    return true;

  abfd->output_has_begun = true;

  /* The fixed header.  */
  sofar = sizeof (Nlm_External_Fixed_Header);

  /* The variable header.  */
  sofar += (sizeof (nlm_variable_header (abfd)->descriptionLength)
	    + nlm_variable_header (abfd) -> descriptionLength + 1
	    + NLM_TARGET_LONG_SIZE /* stackSize */
	    + NLM_TARGET_LONG_SIZE /* reserved */
	    + sizeof (nlm_variable_header (abfd) -> oldThreadName)
	    + sizeof (nlm_variable_header (abfd) -> screenNameLength)
	    + nlm_variable_header (abfd) -> screenNameLength + 1
	    + sizeof (nlm_variable_header (abfd) -> threadNameLength)
	    + nlm_variable_header (abfd) -> threadNameLength + 1);

  /* The auxiliary headers.  */
  if (find_nonzero ((PTR) nlm_version_header (abfd),
		    sizeof (Nlm_Internal_Version_Header)))
    sofar += sizeof (Nlm_External_Version_Header);
  if (find_nonzero ((PTR) nlm_extended_header (abfd),
		    sizeof (Nlm_Internal_Extended_Header)))
    sofar += sizeof (Nlm_External_Extended_Header);
  if (find_nonzero ((PTR) nlm_custom_header (abfd),
		    sizeof (Nlm_Internal_Custom_Header)))
    sofar += sizeof (Nlm_External_Custom_Header);
  if (find_nonzero ((PTR) nlm_copyright_header (abfd),
		    sizeof (Nlm_Internal_Copyright_Header)))
    sofar += (sizeof (Nlm_External_Copyright_Header)
	      + nlm_copyright_header (abfd) -> copyrightMessageLength + 1);

  /* Compute the section file positions in two passes.  First get the
     sizes of the text and data sections, and then set the file
     positions.  This code aligns the sections in the file using the
     same alignment restrictions that apply to the sections in memory;
     this may not be necessary.  */
  text = 0;
  text_low = (bfd_vma) -1;
  text_align = 0;
  data = 0;
  data_low = (bfd_vma) -1;
  data_align = 0;
  bss = 0;
  other_align = 0;
  for (sec = abfd->sections; sec != (asection *) NULL; sec = sec->next)
    {
      flagword f;

      sec->_raw_size = BFD_ALIGN (sec->_raw_size, 1 << sec->alignment_power);

      f = bfd_get_section_flags (abfd, sec);
      if (f & SEC_CODE)
	{
	  text += sec->_raw_size;
	  if (bfd_get_section_vma (abfd, sec) < text_low)
	    text_low = bfd_get_section_vma (abfd, sec);
	  if (sec->alignment_power > text_align)
	    text_align = sec->alignment_power;
	}
      else if (f & SEC_DATA)
	{
	  data += sec->_raw_size;
	  if (bfd_get_section_vma (abfd, sec) < data_low)
	    data_low = bfd_get_section_vma (abfd, sec);
	  if (sec->alignment_power > data_align)
	    data_align = sec->alignment_power;
	}
      else if (f & SEC_HAS_CONTENTS)
	{
	  if (sec->alignment_power > other_align)
	    other_align = sec->alignment_power;
	}
      else if (f & SEC_ALLOC)
	bss += sec->_raw_size;
    }

  nlm_set_text_low (abfd, text_low);
  nlm_set_data_low (abfd, data_low);

  text_ptr = BFD_ALIGN (sofar, 1 << text_align);
  data_ptr = BFD_ALIGN (text_ptr + text, 1 << data_align);
  other_ptr = BFD_ALIGN (data_ptr + data, 1 << other_align);

  /* Fill in some fields in the header for which we now have the
     information.  */
  nlm_fixed_header (abfd)->codeImageOffset = text_ptr;
  nlm_fixed_header (abfd)->codeImageSize = text;
  nlm_fixed_header (abfd)->dataImageOffset = data_ptr;
  nlm_fixed_header (abfd)->dataImageSize = data;
  nlm_fixed_header (abfd)->uninitializedDataSize = bss;

  for (sec = abfd->sections; sec != (asection *) NULL; sec = sec->next)
    {
      flagword f;

      f = bfd_get_section_flags (abfd, sec);

      if (f & SEC_CODE)
	{
	  sec->filepos = text_ptr;
	  text_ptr += sec->_raw_size;
	}
      else if (f & SEC_DATA)
	{
	  sec->filepos = data_ptr;
	  data_ptr += sec->_raw_size;
	}
      else if (f & SEC_HAS_CONTENTS)
	{
	  sec->filepos = other_ptr;
	  other_ptr += sec->_raw_size;
	}
    }

  nlm_fixed_header (abfd)->relocationFixupOffset = other_ptr;

  return true;
}

/* Set the contents of a section.  To do this we need to know where
   the section is going to be located in the output file.  That means
   that the sizes of all the sections must be set, and all the
   variable size header information must be known.  */

boolean
nlm_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     asection *section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  if (abfd->output_has_begun == false
      && nlm_compute_section_file_positions (abfd) == false)
    return false;

  if (count == 0)
    return true;

  if (bfd_seek (abfd, (file_ptr) (section->filepos + offset), SEEK_SET) != 0
      || bfd_write (location, 1, count, abfd) != count)
    {
      bfd_error = system_call_error;
      return false;
    }

  return true;
}

/* We need to sort a list of relocs associated with sections when we
   write out the external relocs.  */

struct reloc_and_sec
{
  arelent *rel;
  asection *sec;
};

static int
nlm_external_reloc_compare (p1, p2)
     const void *p1;
     const void *p2;
{
  const struct reloc_and_sec *r1 = (const struct reloc_and_sec *) p1;
  const struct reloc_and_sec *r2 = (const struct reloc_and_sec *) p2;

  return strcmp ((*r1->rel->sym_ptr_ptr)->name,
		 (*r2->rel->sym_ptr_ptr)->name);
}

/* Write out an NLM file.  We write out the information in this order:
     fixed header
     variable header
     auxiliary headers
     code sections
     data sections
     other sections (custom data, messages, help, shared NLM, RPC,
     		     module dependencies)
     relocation fixups
     external references (imports)
     public symbols (exports)
     debugging records
   This is similar to the order used by the NetWare tools; the
   difference is that NetWare puts the sections other than code, data
   and custom data at the end of the NLM.  It is convenient for us to
   know where the sections are going to be before worrying about the
   size of the other information.

   By the time this function is called, all the section data should
   have been output using set_section_contents.  Note that custom
   data, the message file, the help file, the shared NLM file, the RPC
   data, and the module dependencies are all considered to be
   sections; the caller is responsible for filling in the offset and
   length fields in the NLM headers.  The relocation fixups and
   imports are both obtained from the list of relocs attached to each
   section.  The exports and debugging records are obtained from the
   list of outsymbols.  */

boolean
nlm_write_object_contents (abfd)
     bfd *abfd;
{
  Nlm_External_Fixed_Header fixed_header;
  asection *sec;
  boolean (*write_reloc_func) PARAMS ((bfd *, asection *, arelent *));
  bfd_size_type external_reloc_count, internal_reloc_count, i, c;
  struct reloc_and_sec *external_relocs;
  asymbol **sym_ptr_ptr;

  if (abfd->output_has_begun == false
      && nlm_compute_section_file_positions (abfd) == false)
    return false;

  /* Write out the variable length headers.  */
  if (bfd_seek (abfd, sizeof (Nlm_External_Fixed_Header), SEEK_SET) != 0)
    {
      bfd_error = system_call_error;
      return false;
    }
  if (nlm_swap_variable_header_out (abfd) == false
      || nlm_swap_auxiliary_headers_out (abfd) == false)
    {
      bfd_error = system_call_error;
      return false;
    }

  /* A weak check on whether the section file positions were
     reasonable.  */
  if (bfd_tell (abfd) > nlm_fixed_header (abfd)->codeImageOffset)
    {
      bfd_error = invalid_operation;
      return false;
    }

  /* Advance to the relocs.  */
  if (bfd_seek (abfd, nlm_fixed_header (abfd)->relocationFixupOffset,
		SEEK_SET) != 0)
    {
      bfd_error = system_call_error;
      return false;
    }

  /* The format of the relocation entries is dependent upon the
     particular target.  We use an external routine to write the reloc
     out.  */
  write_reloc_func = nlm_write_reloc_func (abfd);

  /* Write out the internal relocation fixups.  While we're looping
     over the relocs, we also count the external relocs, which is
     needed when they are written out below.  */
  internal_reloc_count = 0;
  external_reloc_count = 0;
  for (sec = abfd->sections; sec != (asection *) NULL; sec = sec->next)
    {
      arelent **rel_ptr_ptr, **rel_end;

      if (sec->reloc_count == 0)
	continue;

      /* We can only represent relocs within a code or data 	
	 section.  */
      if ((bfd_get_section_flags (abfd, sec) & (SEC_CODE | SEC_DATA)) == 0)
	{
	  bfd_error = invalid_operation;
	  return false;
	}

      /* We need to know how to write out relocs.  */
      if (write_reloc_func == NULL)
	{
	  bfd_error = invalid_operation;
	  return false;
	}

      rel_ptr_ptr = sec->orelocation;
      rel_end = rel_ptr_ptr + sec->reloc_count;
      for (; rel_ptr_ptr < rel_end; rel_ptr_ptr++)
	{
	  arelent *rel;
	  asymbol *sym;

	  rel = *rel_ptr_ptr;
	  sym = *rel->sym_ptr_ptr;

	  if ((sym->flags & BSF_SECTION_SYM) != 0)
	    {
	      ++internal_reloc_count;
	      if ((*write_reloc_func) (abfd, sec, rel) == false)
		return false;
	    }
	  else
	    ++external_reloc_count;
	}
    }
  nlm_fixed_header (abfd)->numberOfRelocationFixups = internal_reloc_count;

  /* Write out the imports (relocs against external symbols).  These
     are output as a symbol name followed by all the relocs for that
     symbol, so we must first gather together all the relocs against
     external symbols and sort them.  */
  external_relocs =
    (struct reloc_and_sec *) bfd_alloc (abfd,
					(external_reloc_count
					 * sizeof (struct reloc_and_sec)));
  if (external_relocs == (struct reloc_and_sec *) NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  i = 0;
  for (sec = abfd->sections; sec != (asection *) NULL; sec = sec->next)
    {
      arelent **rel_ptr_ptr, **rel_end;

      if (sec->reloc_count == 0)
	continue;

      rel_ptr_ptr = sec->orelocation;
      rel_end = rel_ptr_ptr + sec->reloc_count;
      for (; rel_ptr_ptr < rel_end; rel_ptr_ptr++)
	{
	  arelent *rel;
	  asymbol *sym;

	  rel = *rel_ptr_ptr;
	  sym = *rel->sym_ptr_ptr;

	  if ((sym->flags & BSF_SECTION_SYM) != 0)
	    continue;

	  external_relocs[i].rel = rel;
	  external_relocs[i].sec = sec;
	  ++i;
	}
    }

  BFD_ASSERT (i == external_reloc_count);

  /* Sort the external relocs by name.  */
  qsort (external_relocs, external_reloc_count,
	 sizeof (struct reloc_and_sec), nlm_external_reloc_compare);

  /* Write out the external relocs.  */
  nlm_fixed_header (abfd)->externalReferencesOffset = bfd_tell (abfd);
  c = 0;
  i = 0;
  while (i < external_reloc_count)
    {
      arelent *rel;
      asymbol *sym;
      bfd_byte len;
      bfd_size_type cnt;
      bfd_byte temp[NLM_TARGET_LONG_SIZE];

      ++c;

      rel = external_relocs[i].rel;
      sym = *rel->sym_ptr_ptr;

      len = strlen (sym->name);
      if ((bfd_write (&len, sizeof (bfd_byte), 1, abfd)
	   != sizeof (bfd_byte))
	  || bfd_write (sym->name, len, 1, abfd) != len)
	{
	  bfd_error = system_call_error;
	  return false;
	}

      cnt = 0;
      while (i < external_reloc_count
	     && *external_relocs[i].rel->sym_ptr_ptr == sym)
	++cnt;

      put_word (abfd, (bfd_vma) cnt, temp);
      if (bfd_write (temp, sizeof (temp), 1, abfd) != sizeof (temp))
	{
	  bfd_error = system_call_error;
	  return false;
	}

      while (cnt-- != 0)
	{
	  if ((*write_reloc_func) (abfd, external_relocs[i].sec,
				   external_relocs[i].rel) == false)
	    return false;
	  ++i;
	}
    }
  nlm_fixed_header (abfd)->numberOfExternalReferences = c;

  /* Write out the public symbols (exports).  */
  sym_ptr_ptr = bfd_get_outsymbols (abfd);
  if (sym_ptr_ptr != (asymbol **) NULL)
    {
      asymbol **sym_end;

      nlm_fixed_header (abfd)->publicsOffset = bfd_tell (abfd);
      c = 0;
      sym_end = sym_ptr_ptr + bfd_get_symcount (abfd);
      for (; sym_ptr_ptr < sym_end; sym_ptr_ptr++)
	{
	  asymbol *sym;
	  bfd_byte len;
	  bfd_vma offset;
	  asection *sec;
	  bfd_byte temp[NLM_TARGET_LONG_SIZE];

	  sym = *sym_ptr_ptr;

	  if ((sym->flags & (BSF_EXPORT | BSF_GLOBAL)) == 0)
	    continue;

	  ++c;

	  len = strlen (sym->name);
	  if ((bfd_write (&len, sizeof (bfd_byte), 1, abfd)
	       != sizeof (bfd_byte))
	      || bfd_write (sym->name, len, 1, abfd) != len)
	    {
	      bfd_error = system_call_error;
	      return false;
	    }

	  offset = bfd_asymbol_value (sym);
	  sec = sym->section;
	  if (sec->flags & SEC_CODE)
	    {
	      offset -= nlm_get_text_low (abfd);
	      offset |= NLM_HIBIT;
	    }
	  else if (sec->flags & SEC_DATA)
	    {
	      offset -= nlm_get_data_low (abfd);
	    }
	  else
	    {
	      /* We can't handle an exported symbol that is not in the
		 code or data segment.  */
	      bfd_error = invalid_operation;
	      return false;
	    }

	  put_word (abfd, offset, temp);
	  if (bfd_write (temp, sizeof (temp), 1, abfd) != sizeof (temp))
	    {
	      bfd_error = system_call_error;
	      return false;
	    }
	}	  
      nlm_fixed_header (abfd)->numberOfPublics = c;

      nlm_fixed_header (abfd)->debugInfoOffset = bfd_tell (abfd);
      c = 0;
      sym_end = sym_ptr_ptr + bfd_get_symcount (abfd);
      for (; sym_ptr_ptr < sym_end; sym_ptr_ptr++)
	{
	  asymbol *sym;
	  bfd_byte type, len;
	  bfd_vma offset;
	  asection *sec;
	  bfd_byte temp[NLM_TARGET_LONG_SIZE];

	  sym = *sym_ptr_ptr;

	  ++c;

	  offset = bfd_asymbol_value (sym);
	  sec = sym->section;
	  if (sec->flags & SEC_CODE)
	    {
	      offset -= nlm_get_text_low (abfd);
	      type = 1;
	    }
	  else if (sec->flags & SEC_DATA)
	    {
	      offset -= nlm_get_data_low (abfd);
	      type = 0;
	    }
	  else
	    type = 3;

	  /* The type is 0 for data, 1 for code, 3 for absolute.  */
	  if (bfd_write (&type, sizeof (bfd_byte), 1, abfd)
	      != sizeof (bfd_byte))
	    {
	      bfd_error = system_call_error;
	      return false;
	    }

	  put_word (abfd, offset, temp);
	  if (bfd_write (temp, sizeof (temp), 1, abfd) != sizeof (temp))
	    {
	      bfd_error = system_call_error;
	      return false;
	    }

	  len = strlen (sym->name);
	  if ((bfd_write (&len, sizeof (bfd_byte), 1, abfd)
	       != sizeof (bfd_byte))
	      || bfd_write (sym->name, len, 1, abfd) != len)
	    {
	      bfd_error = system_call_error;
	      return false;
	    }

	  /* Exported symbols may get an additional debugging record
	     without the prefix.  */
	  if ((sym->flags & (BSF_EXPORT | BSF_GLOBAL)) != 0)
	    {
	      char *s;

	      s = strchr (sym->name, '@');
	      if (s != NULL)
		{
		  ++c;

		  if ((bfd_write (&type, sizeof (bfd_byte), 1, abfd)
		       != sizeof (bfd_byte))
		      || (bfd_write (temp, sizeof (temp), 1, abfd)
			  != sizeof (temp)))
		    {
		      bfd_error = system_call_error;
		      return false;
		    }

		  ++s;
		  
		  len = strlen (s);
		  if ((bfd_write (&len, sizeof (bfd_byte), 1, abfd)
		       != sizeof (bfd_byte))
		      || bfd_write (s, len, 1, abfd) != len)
		    {
		      bfd_error = system_call_error;
		      return false;
		    }
		}
	    }
	}	  
      nlm_fixed_header (abfd)->numberOfDebugRecords = c;
    }

  /* At this point everything has been written out except the fixed
     header.  */
  memcpy (nlm_fixed_header (abfd)->signature, NLM_SIGNATURE,
	  NLM_SIGNATURE_SIZE);
  nlm_fixed_header (abfd)->version = NLM_HEADER_VERSION;
  nlm_fixed_header (abfd)->codeStartOffset =
    bfd_get_start_address (abfd) - nlm_get_text_low (abfd);

  /* We have no convenient way for the caller to pass in the exit
     procedure or the check unload procedure, so the caller must set
     the values in the header to the values of the symbols.  */
  if (nlm_fixed_header (abfd)->exitProcedureOffset == 0)
    {
      bfd_error = invalid_operation;
      return false;
    }
  nlm_fixed_header (abfd)->exitProcedureOffset -= nlm_get_text_low (abfd);
  if (nlm_fixed_header (abfd)->checkUnloadProcedureOffset != 0)
    nlm_fixed_header (abfd)->checkUnloadProcedureOffset -=
      nlm_get_text_low (abfd);

  nlm_swap_fixed_header_out (abfd, nlm_fixed_header (abfd), &fixed_header);
  if (bfd_seek (abfd, 0, SEEK_SET) != 0
      || (bfd_write (&fixed_header, sizeof fixed_header, 1, abfd)
	  != sizeof fixed_header))
    {
      bfd_error = system_call_error;
      return false;
    }

  return true;
}
