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

#define Nlm_External_Fixed_Header	NAME(Nlm,External_Fixed_Header)
#define Nlm_External_Version_Header	NAME(Nlm,External_Version_Header)
#define Nlm_External_Copyright_Header	NAME(Nlm,External_Copyright_Header)
#define Nlm_External_Extended_Header	NAME(Nlm,External_Extended_Header)
#define Nlm_External_Custom_Header	NAME(Nlm,External_Custom_Header)

#define nlm_symbol_type			NAME(nlm,symbol_type)

#define nlm_get_symtab_upper_bound	NAME(bfd_nlm,get_symtab_upper_bound)
#define nlm_get_symtab			NAME(bfd_nlm,get_symtab)
#define nlm_make_empty_symbol		NAME(bfd_nlm,make_empty_symbol)
#define nlm_print_symbol		NAME(bfd_nlm,print_symbol)
#define nlm_get_symbol_info		NAME(bfd_nlm,get_symbol_info)
#define nlm_set_arch_mach		NAME(bfd_nlm,set_arch_mach)
#define nlm_object_p			NAME(bfd_nlm,object_p)

#define MSB (~(~(unsigned long)0 >> 1))

/* Forward declarations of static functions */

static boolean add_bfd_section
  PARAMS ((bfd *, char *, file_ptr, bfd_size_type, flagword));

static void nlm_swap_fixed_header_in
  PARAMS ((bfd *, Nlm_External_Fixed_Header *, Nlm_Internal_Fixed_Header *));

static boolean nlm_swap_variable_header_in PARAMS ((bfd *));

static boolean nlm_swap_auxiliary_headers_in PARAMS ((bfd *));

static struct sec *section_from_nlm_index PARAMS ((bfd *, int));

static int nlm_section_from_bfd_section PARAMS ((bfd *, struct sec *));

static boolean nlm_slurp_symbol_table PARAMS ((bfd *, asymbol **));

static int nlm_symbol_from_bfd_symbol
  PARAMS ((bfd *, struct symbol_cache_entry **));

static void nlm_map_symbols PARAMS ((bfd *));

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
  int shindex;

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
			   SEC_CODE | SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS)
      || !add_bfd_section (abfd, NLM_INITIALIZED_DATA_NAME,
			   i_fxdhdrp -> dataImageOffset,
			   i_fxdhdrp -> dataImageSize,
			   SEC_DATA | SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS)
      || !add_bfd_section (abfd, NLM_UNINITIALIZED_DATA_NAME,
			   (file_ptr) 0,
			   i_fxdhdrp -> uninitializedDataSize,
			   SEC_DATA | SEC_ALLOC))
    {
      return (NULL);
    }

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

/* Read and swap in the variable length header.  All the fields must
   exist in the NLM, and must exist in the order they are read here. */

static boolean
DEFUN (nlm_swap_variable_header_in, (abfd),
       bfd * abfd)
{
  unsigned char temp [TARGET_LONG_SIZE];

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
  unsigned char temp [TARGET_LONG_SIZE];
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
  symcount = i_fxdhdrp -> numberOfPublics;
  symtab_size = (symcount + 1) * (sizeof (asymbol));
  return (symtab_size);
}

/* Note that bfd_get_symcount is guaranteed to be zero if slurping the
   symbol table fails. */

unsigned int
DEFUN (nlm_get_symtab, (abfd, alocation),
       bfd *abfd AND
       asymbol **alocation)
{
  nlm_slurp_symbol_table (abfd, alocation);
  return (bfd_get_symcount (abfd));
}

asymbol *
DEFUN (nlm_make_empty_symbol, (abfd),		/* FIXME! */
      bfd * abfd)
{
  asymbol *new = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  new -> the_bfd = abfd;
  return new;
}

void
DEFUN (nlm_get_symbol_info, (ignore_abfd, symbol, ret),
       bfd * ignore_abfd AND
       asymbol * symbol AND
       symbol_info * ret)
{
  bfd_symbol_info (symbol, ret);
}

boolean
DEFUN (nlm_set_arch_mach, (abfd, arch, machine),
       bfd * abfd AND
       enum bfd_architecture arch AND
       unsigned long machine)
{
   return false;
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

   If SYMPTRS is non-NULL, the bfd symbols are copied to where it points.

   When we return, the bfd symcount is either zero or contains the correct
   number of symbols.
*/

static boolean
DEFUN (nlm_slurp_symbol_table, (abfd, symptrs),
       bfd *abfd AND
       asymbol **symptrs)	/* Buffer for generated bfd symbols */
{
  Nlm_Internal_Fixed_Header *i_fxdhdrp;	/* Nlm file header, internal form */
  int symcount;				/* Number of external NLM symbols */
  int idx;
  nlm_symbol_type *sym;			/* Pointer to current bfd symbol */
  nlm_symbol_type *symbase;		/* Buffer for generated bfd symbols */
  char symlength;			/* Symbol length read into here */
  long symoffset;			/* Symbol offset read into here */

  if (bfd_get_outsymbols (abfd) != NULL)
    {
      return (true);
    }

  /* Read each raw NLM symbol, using the information to create a canonical bfd
     symbol table entry.

     Note that we allocate the initial bfd canonical symbol buffer based on a
     one-to-one mapping of the NLM symbols to canonical symbols.  We actually
     use all the NLM symbols, so there will be no space left over at the end.
     When we have all the symbols, we build the caller's pointer vector. */

  abfd -> symcount = 0;
  i_fxdhdrp = nlm_fixed_header (abfd);
  symcount = i_fxdhdrp -> numberOfPublics;
  if (symcount == 0)
    {
      return (true);
    }
  if (bfd_seek (abfd, i_fxdhdrp -> publicsOffset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return (false);
    }
  symbase = (nlm_symbol_type *)
    bfd_zalloc (abfd, symcount * sizeof (nlm_symbol_type));
  sym = symbase;

  /* We use the bfd's symcount directly as the control count, so that early
     termination of the loop leaves the symcount correct for the symbols that
     were read. */

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
      if (bfd_read ((PTR) &symoffset, sizeof (symoffset), 1, abfd)
	  != sizeof (symoffset))
	{
	  bfd_error = system_call_error;
	  return (false);
	}
      sym -> symbol.flags = BSF_GLOBAL | BSF_EXPORT;
      sym -> symbol.value = get_word (abfd, (bfd_byte *) &symoffset);
      if (sym -> symbol.value & MSB)
	{
	  sym -> symbol.value &= ~MSB;
	  sym -> symbol.flags |= BSF_FUNCTION;
	  sym -> symbol.section =
	    bfd_get_section_by_name (abfd, NLM_CODE_NAME);
	}
      else
	{
	  sym -> symbol.section =
	    bfd_get_section_by_name (abfd, NLM_INITIALIZED_DATA_NAME);
	}
      abfd -> symcount++;
      sym++;
    }

  /* Fill in the user's symbol pointer vector if needed.  */

  if (symptrs != NULL)
    {
      sym = symbase;
      symcount = abfd -> symcount;
      while (symcount-- > 0)
	{
	  *symptrs++ = &sym -> symbol;
	  sym++;
	}
      *symptrs = NULL;		/* Final NULL pointer */
    }

  return (true);
}

