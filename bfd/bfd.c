			   /* -*- C -*- */

/*** bfd -- binary file diddling routines by Gumby Wallace of Cygnus Support.
	    Every definition in this file should be exported and declared
	    in bfd.c.  If you don't want it to be user-visible, put it in
	    libbfd.c!
*/

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

short _bfd_host_big_endian = 0x0100;
	/* Accessing the above as (*(char*)&_bfd_host_big_endian), will
	   return 1 if the host is big-endian, 0 otherwise.
	   (assuming that a short is two bytes long!!!  FIXME)
	   (See HOST_IS_BIG_ENDIAN_P in bfd.h.)  */

/** Error handling
    o - Most functions return nonzero on success (check doc for
	precise semantics); 0 or NULL on error.
    o - Internal errors are documented by the value of bfd_error.
	If that is system_call_error then check errno.
    o - The easiest way to report this to the user is to use bfd_perror.
*/

bfd_ec bfd_error = no_error;

char *bfd_errmsgs[] = {	"No error",
			"System call error",
			"Invalid target",
			"File in wrong format",
			"Invalid operation",
			"Memory exhausted",
			"No symbols",
			"No relocation info",
			"No more archived files",
			"Malformed archive",
			"Symbol not found",
			"File format not recognized",
			"File format is ambiguous",
			"Section has no contents",
			"Nonrepresentable section on output",
			"#<Invalid error code>"
		       };

static 
void 
DEFUN(bfd_nonrepresentable_section,(abfd, name),
	 CONST  bfd * CONST abfd AND
	 CONST  char * CONST name)
{
  printf("bfd error writing file %s, format %s can't represent section %s\n",
	 abfd->filename, 
	 abfd->xvec->name,
	 name);
  exit(1);
}

bfd_error_vector_type bfd_error_vector = 
  {
  bfd_nonrepresentable_section 
  };

#if !defined(ANSI_LIBRARIES)
char *
strerror (code)
     int code;
{
  extern int sys_nerr;
  extern char *sys_errlist[];

  return (((code < 0) || (code >= sys_nerr)) ? "(unknown error)" :
	  sys_errlist [code]);
}
#endif /* not ANSI_LIBRARIES */


char *
bfd_errmsg (error_tag)
     bfd_ec error_tag;
{

  if (error_tag == system_call_error)
    return strerror (errno);

  if ((((int)error_tag <(int) no_error) ||
       ((int)error_tag > (int)invalid_error_code)))
    error_tag = invalid_error_code;/* sanity check */

  return bfd_errmsgs [(int)error_tag];
}


void bfd_default_error_trap(error_tag)
bfd_ec error_tag;
{
  printf("bfd assert fail (%s)\n", bfd_errmsg(error_tag));
}

void (*bfd_error_trap)() = bfd_default_error_trap;
void (*bfd_error_nonrepresentabltrap)() = bfd_default_error_trap;

void
DEFUN(bfd_perror,(message),
      CONST char *message)
{
  if (bfd_error == system_call_error)
    perror((char *)message);		/* must be system error then... */
  else {
    if (message == NULL || *message == '\0')
      fprintf (stderr, "%s\n", bfd_errmsg (bfd_error));
    else
      fprintf (stderr, "%s: %s\n", message, bfd_errmsg (bfd_error));
  }
}

/* for error messages */
char *
bfd_format_string (format)
     bfd_format format;
{
  if (((int)format <(int) bfd_unknown) || ((int)format >=(int) bfd_type_end)) return "invalid";
  
  switch (format) {
  case bfd_object: return "object"; /* linker/assember/compiler output */
  case bfd_archive: return "archive"; /* object archive file */
  case bfd_core: return "core";	/* core dump */
  default: return "unknown";
  }
}

/** Target configurations */

extern bfd_target *target_vector[];

/* Returns a pointer to the transfer vector for the object target
   named target_name.  If target_name is NULL, chooses the one in the
   environment variable GNUTARGET; if that is null or not defined then
   the first entry in the target list is chosen.  Passing in the
   string "default" or setting the environment variable to "default"
   will cause the first entry in the target list to be returned. */

bfd_target *
DEFUN(bfd_find_target,(target_name),
      CONST char *target_name)
{
  bfd_target **target;
  extern char *getenv ();
  CONST char *targname = (target_name ? target_name : getenv ("GNUTARGET"));

  /* This is safe; the vector cannot be null */
  if (targname == NULL || !strcmp (targname, "default"))
    return target_vector[0];

  for (target = &target_vector[0]; *target != NULL; target++) {
    if (!strcmp (targname, (*target)->name))
      return *target;
  }

  bfd_error = invalid_target;
  return NULL;
}

/* Returns a freshly-consed, NULL-terminated vector of the names of all the
   valid bfd targets.  Do not modify the names */

char **
bfd_target_list ()
{
  int vec_length= 0;
  bfd_target **target;
  char **name_list, **name_ptr;

  for (target = &target_vector[0]; *target != NULL; target++)
    vec_length++;

  name_ptr = name_list = (char **) zalloc ((vec_length + 1) * sizeof (char **));

  if (name_list == NULL) {
    bfd_error = no_memory;
    return NULL;
  }

  for (target = &target_vector[0]; *target != NULL; target++)
    *(name_ptr++) = (*target)->name;

  return name_list;
}

/** Init a bfd for read of the proper format.
 */

/* We should be able to find out if the target was defaulted or user-specified.
   If the user specified the target explicitly then we should do no search.
   I guess the best way to do this is to pass an extra argument which specifies
   the DWIM. */

/* I have chanegd this always to set the filepos to the origin before
   guessing.  -- Gumby, 14 Februar 1991*/

boolean
bfd_check_format (abfd, format)
     bfd *abfd;
     bfd_format format;
{
  bfd_target **target, *save_targ, *right_targ;
  int match_count;

  if (!bfd_read_p (abfd) ||
      ((int)(abfd->format) < (int)bfd_unknown) ||
      ((int)(abfd->format) >= (int)bfd_type_end)) {
    bfd_error = invalid_operation;
    return false;
  }

  if (abfd->format != bfd_unknown) return (abfd->format == format) ? true:false;

  /* presume the answer is yes */
  abfd->format = format;

  bfd_seek (abfd, (file_ptr)0, SEEK_SET);	/* rewind! */

  right_targ = BFD_SEND_FMT (abfd, _bfd_check_format, (abfd));
  if (right_targ) {
    abfd->xvec = right_targ;		/* Set the target as returned */
    return true;			/* File position has moved, BTW */
  }

  /* This isn't a <format> file in the specified or defaulted target type.
     See if we recognize it for any other target type.  (We check them
     all to make sure it's uniquely recognized.)  */

  save_targ = abfd->xvec;
  match_count = 0;
  right_targ = 0;

  for (target = target_vector; *target != NULL; target++) {
    bfd_target *temp;

    abfd->xvec = *target;	/* Change BFD's target temporarily */
    bfd_seek (abfd, (file_ptr)0, SEEK_SET);
    temp = BFD_SEND_FMT (abfd, _bfd_check_format, (abfd));
    if (temp) {				/* This format checks out as ok! */
      right_targ = temp;
      match_count++;
#ifdef GNU960
      /* Big- and little-endian b.out archives look the same, but it doesn't
       * matter: there is no difference in their headers, and member file byte
       * orders will (I hope) be handled appropriately by bfd.  Ditto for big
       * and little coff archives.  And the 4 coff/b.out object formats are
       * unambiguous.  So accept the first match we find.
       */
      break;
#endif
    }
  }

  if (match_count == 1) {
    abfd->xvec = right_targ;		/* Change BFD's target permanently */
    return true;			/* File position has moved, BTW */
  }

  abfd->xvec = save_targ;		/* Restore original target type */
  abfd->format = bfd_unknown;		/* Restore original format */
  bfd_error = ((match_count == 0) ? file_not_recognized :
	       file_ambiguously_recognized);
  return false;
}

boolean
bfd_set_format (abfd, format)
     bfd *abfd;
     bfd_format format;
{

  if (bfd_read_p (abfd) ||
      ((int)abfd->format < (int)bfd_unknown) ||
      ((int)abfd->format >= (int)bfd_type_end)) {
    bfd_error = invalid_operation;
    return false;
  }

  if (abfd->format != bfd_unknown) return (abfd->format == format) ? true:false;

  /* presume the answer is yes */
  abfd->format = format;

  if (!BFD_SEND_FMT (abfd, _bfd_set_format, (abfd))) {
    abfd->format = bfd_unknown;
    return false;
  }

  return true;
}

/* Hack object and core file sections */

sec_ptr
DEFUN(bfd_get_section_by_name,(abfd, name),
      bfd *abfd AND
      CONST char *name)
{
  asection *sect;
  
  for (sect = abfd->sections; sect != NULL; sect = sect->next)
    if (!strcmp (sect->name, name)) return sect;
  return NULL;
}

/* If you try to create a section with a name which is already in use,
   returns the old section by that name instead. */
sec_ptr
DEFUN(bfd_make_section,(abfd, name),
      bfd *abfd AND
      CONST char *name)
{
  asection *newsect;  
  asection **  prev = &abfd->sections;
  asection * sect = abfd->sections;
  
  if (abfd->output_has_begun) {
    bfd_error = invalid_operation;
    return NULL;
  }

  while (sect) {
    if (!strcmp(sect->name, name)) return sect;
    prev = &sect->next;
    sect = sect->next;
  }

  newsect = (asection *) bfd_zalloc(abfd, sizeof (asection));
  if (newsect == NULL) {
    bfd_error = no_memory;
    return NULL;
  }

  newsect->name = name;
  newsect->index = abfd->section_count++;
  newsect->flags = SEC_NO_FLAGS;

  newsect->userdata = 0;
  newsect->next = (asection *)NULL;
  newsect->relocation = (arelent *)NULL;
  newsect->reloc_count = 0;
  newsect->line_filepos =0;

  if (BFD_SEND (abfd, _new_section_hook, (abfd, newsect)) != true) {
    free (newsect);
    return NULL;
  }

  *prev = newsect;
  return newsect;
}

/* Call operation on each section.  Operation gets three args: the bfd,
   the section, and a void * pointer (whatever the user supplied). */

/* This is attractive except that without lexical closures its use is hard
   to make reentrant. */
/*VARARGS2*/
void
bfd_map_over_sections (abfd, operation, user_storage)
     bfd *abfd;
     void (*operation)();
     PTR user_storage;
{
  asection *sect;
  int i = 0;
  
  for (sect = abfd->sections; sect != NULL; i++, sect = sect->next)
    (*operation) (abfd, sect, user_storage);

  if (i != abfd->section_count)		/* Debugging */
    abort();
}

boolean
bfd_set_section_flags (abfd, section, flags)
     bfd *abfd;
     sec_ptr section;
     flagword flags;
{
  if ((flags & bfd_applicable_section_flags (abfd)) != flags) {
    bfd_error = invalid_operation;
    return false;
  }

   section->flags = flags;
return true;
}


boolean
bfd_set_section_size (abfd, ptr, val)
     bfd *abfd;
     sec_ptr ptr;
     unsigned long val;
{
  /* Once you've started writing to any section you cannot create or change
     the size of any others. */

  if (abfd->output_has_begun) {
    bfd_error = invalid_operation;
    return false;
  }

  ptr->size = val;
  
  return true;
}

boolean
bfd_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     int count;
{
	if (!(bfd_get_section_flags(abfd, section) &
	      SEC_HAS_CONTENTS)) {
		bfd_error = no_contents;
		return(false);
	} /* if section has no contents */

  if (BFD_SEND (abfd, _bfd_set_section_contents,
	    (abfd, section, location, offset, count))) {
    abfd->output_has_begun = true;
    return true;
  }

  return false;
}

boolean
bfd_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     int count;
{
  if (section->flags & SEC_CONSTRUCTOR) {
    memset(location, 0, count);
    return true;
  }
  else {
    return  (BFD_SEND (abfd, _bfd_get_section_contents,
		       (abfd, section, location, offset, count)));
  }
}


/** Some core file info commands */

/* Returns a read-only string explaining what program was running when
   it failed. */

char *
bfd_core_file_failing_command (abfd)
     bfd *abfd;
{
  if (abfd->format != bfd_core) {
    bfd_error = invalid_operation;
    return NULL;
  }
  return BFD_SEND (abfd, _core_file_failing_command, (abfd));
}

int
bfd_core_file_failing_signal (abfd)
     bfd *abfd;
{
  if (abfd->format != bfd_core) {
    bfd_error = invalid_operation;
    return NULL;
  }
  return BFD_SEND (abfd, _core_file_failing_signal, (abfd));
}

boolean
core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  if ((core_bfd->format != bfd_core) || (exec_bfd->format != bfd_object)) {
    bfd_error = wrong_format;
    return false;
  }

  return BFD_SEND (core_bfd, _core_file_matches_executable_p, (core_bfd, exec_bfd));
}

/** Symbols */

boolean
bfd_set_symtab (abfd, location, symcount)
     bfd *abfd;
     asymbol **location;
     unsigned int symcount;
{
  if ((abfd->format != bfd_object) || (bfd_read_p (abfd))) {
    bfd_error = invalid_operation;
    return false;
  }

  bfd_get_outsymbols (abfd) = location;
  bfd_get_symcount (abfd) = symcount;
  return true;
}

/* returns the number of octets of storage required */
unsigned int
get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (abfd->format != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }

  return BFD_SEND (abfd, _get_reloc_upper_bound, (abfd, asect));
}

unsigned int
bfd_canonicalize_reloc (abfd, asect, location, symbols)
     bfd *abfd;
     sec_ptr asect;
     arelent **location;
     asymbol **symbols;
{
  if (abfd->format != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }

  return BFD_SEND (abfd, _bfd_canonicalize_reloc, (abfd, asect, location, symbols));
}

void
bfd_print_symbol_vandf(file, symbol)
PTR file;
asymbol *symbol;
{
  flagword type = symbol->flags;
  if (symbol->section != (asection *)NULL)
    {
      fprintf(file,"%08lx ", symbol->value+symbol->section->vma);
    }
  else 
    {
      fprintf(file,"%08lx ", symbol->value);
    }
  fprintf(file,"%c%c%c%c%c%c%c",
	  (type & BSF_LOCAL)  ? 'l':' ',
	  (type & BSF_GLOBAL) ? 'g' : ' ',
	  (type & BSF_IMPORT) ? 'i' : ' ',
	  (type & BSF_EXPORT) ? 'e' : ' ',
	  (type & BSF_UNDEFINED) ? 'u' : ' ',
	  (type & BSF_FORT_COMM) ? 'c' : ' ',
	  (type & BSF_DEBUGGING) ? 'd' :' ');

}


boolean
bfd_set_file_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  if (abfd->format != bfd_object) {
    bfd_error = wrong_format;
    return false;
  }

  if (bfd_read_p (abfd)) {
    bfd_error = invalid_operation;
    return false;
  }

  if ((flags & bfd_applicable_file_flags (abfd)) != flags) {
    bfd_error = invalid_operation;
    return false;
  }

  bfd_get_file_flags (abfd) = flags;
return true;
}


void
bfd_set_reloc (ignore_abfd, asect, location, count)
     bfd *ignore_abfd;
     sec_ptr asect;
     arelent **location;
     unsigned int count;
{
  asect->orelocation  = location;
  asect->reloc_count = count;
}
/*
If an output_bfd is supplied to this function the generated image
will be relocatable, the relocations are copied to the output file
after they have been changed to reflect the new state of the world.
There are two ways of reflecting the results of partial linkage in an
output file; by modifying the output data in place, and by modifying
the relocation record. Some native formats (eg basic a.out and basic
coff) have no way of specifying an addend in the relocation type, so
the addend has to go in the output data.  This is no big deal since in
these formats the output data slot will always be big enough for the
addend. Complex reloc types with addends were invented to solve just
this problem.
*/

bfd_reloc_status_enum_type
bfd_perform_relocation(abfd,
		       reloc_entry,
		       data,
		       input_section,
		       output_bfd)
bfd *abfd;
arelent *reloc_entry;
PTR data;
asection *input_section;
bfd *output_bfd;
{
  bfd_vma relocation;
  bfd_reloc_status_enum_type flag = bfd_reloc_ok;
  bfd_vma addr = reloc_entry->address ;
  bfd_vma output_base = 0;
  reloc_howto_type *howto = reloc_entry->howto;
  asection *reloc_target_output_section;
  asection *reloc_target_input_section;
  asymbol *symbol;

  if (reloc_entry->sym_ptr_ptr) {
    symbol = *( reloc_entry->sym_ptr_ptr);
    if ((symbol->flags & BSF_UNDEFINED) && output_bfd == (bfd *)NULL) {
      flag = bfd_reloc_undefined;
    }
  }
  else {
    symbol = (asymbol*)NULL;
  }

  if (howto->special_function){
    bfd_reloc_status_enum_type cont;
    cont = howto->special_function(abfd,
				   reloc_entry,
				   symbol,
				   data,
				   input_section);
    if (cont != bfd_reloc_continue) return cont;
  }

  /* 
    Work out which section the relocation is targetted at and the
    initial relocation command value.
    */


  if (symbol != (asymbol *)NULL){
    if (symbol->flags & BSF_FORT_COMM) {
      relocation = 0;
    }
    else {
      relocation = symbol->value;
    }
    if (symbol->section != (asection *)NULL)
	{
	  reloc_target_input_section = symbol->section;
	}
    else {
      reloc_target_input_section = (asection *)NULL;
    }
  }
  else if (reloc_entry->section != (asection *)NULL)
      {
	relocation = 0;
	reloc_target_input_section = reloc_entry->section;
      }
  else {
    relocation = 0;
    reloc_target_input_section = (asection *)NULL;
  }


  if (reloc_target_input_section != (asection *)NULL) {

    reloc_target_output_section =
      reloc_target_input_section->output_section;

    if (output_bfd && howto->partial_inplace==false) {
      output_base = 0;
    }
    else {
      output_base = reloc_target_output_section->vma;

    }

    relocation += output_base +   reloc_target_input_section->output_offset;
  }

  relocation += reloc_entry->addend ;


  if(reloc_entry->address > (bfd_vma)(input_section->size)) 
      {
	return bfd_reloc_outofrange;
      }
	  

  if (howto->pc_relative == true)
      {
	/*
	  Anything which started out as pc relative should end up that
	  way too. 

	  There are two ways we can see a pcrel instruction. Sometimes
	  the pcrel displacement has been partially calculated, it
	  includes the distance from the start of the section to the
	  instruction in it (eg sun3), and sometimes the field is
	  totally blank - eg m88kbcs.
	  */

	
	relocation -= 
	  output_base +   input_section->output_offset;

	if (howto->pcrel_offset == true) {
	  relocation -= reloc_entry->address;
	}

      }

  if (output_bfd!= (bfd *)NULL) {
    if ( howto->partial_inplace == false)  {
      /*
	This is a partial relocation, and we want to apply the relocation
	to the reloc entry rather than the raw data. Modify the reloc
	inplace to reflect what we now know.
	*/
      reloc_entry->addend = relocation  ;
      reloc_entry->section = reloc_target_input_section;
      if (reloc_target_input_section != (asection *)NULL) {
	/* If we know the output section we can forget the symbol */
	reloc_entry->sym_ptr_ptr = (asymbol**)NULL;
      }
      reloc_entry->address += 
	input_section->output_offset;
      return flag;
    }
    else 
	{
	  /* This is a partial relocation, but inplace, so modify the
	     reloc record a bit
	     */

	}
  }

  reloc_entry->addend = 0;


  /* 
    Either we are relocating all the way, or we don't want to apply
    the relocation to the reloc entry (probably because there isn't
    any room in the output format to describe addends to relocs)
    */
  relocation >>= howto->rightshift;

  /* Shift everything up to where it's going to be used */
   
  relocation <<= howto->bitpos;

  /* Wait for the day when all have the mask in them */

  /* What we do:
     i instruction to be left alone
     o offset within instruction
     r relocation offset to apply
     S src mask
     D dst mask
     N ~dst mask
     A part 1
     B part 2
     R result
     
     Do this:
     i i i i i o o o o o 	from bfd_get<size>
     and           S S S S S 	to get the size offset we want
     +   r r r r r r r r r r  to get the final value to place
     and           D D D D D  to chop to right size
     -----------------------
     A A A A A 
     And this:
     ...   i i i i i o o o o o	from bfd_get<size>
     and   N N N N N 	        get instruction
     -----------------------
     ...   B B B B B
     
     And then:	     
     B B B B B       
     or 	     A A A A A     
     -----------------------
     R R R R R R R R R R 	put into bfd_put<size>
     */

#define DOIT(x) \
  x = ( (x & ~howto->dst_mask) | (((x & howto->src_mask) +  relocation) & howto->dst_mask))

    switch (howto->size)
	{
	case 0:
	    {
	      char x = bfd_getchar(abfd, (char *)data + addr);
	      DOIT(x);
	      bfd_putchar(abfd,x, (unsigned char *) data + addr);
	    }
	  break;

	case 1:
	    { 
	      short x = bfd_getshort(abfd, (bfd_byte *)data + addr);
	      DOIT(x);
	      bfd_putshort(abfd, x,   (unsigned char *)data + addr);
	    }
	  break;
	case 2:
	    {
	      long  x = bfd_getlong(abfd, (bfd_byte *) data + addr);
	      DOIT(x);
	      bfd_putlong(abfd,x,    (bfd_byte *)data + addr);
	    }	   
	  break;
	case 3:
	  /* Do nothing */
	  break;
	default:
	  return bfd_reloc_other;
	}

  return flag;
}

void
bfd_assert(file, line)
char *file;
int line;
{
  printf("bfd assertion fail %s:%d\n",file,line);
}


boolean
bfd_set_start_address(abfd, vma)
bfd *abfd;
bfd_vma vma;
{
  abfd->start_address = vma;
  return true;
}


bfd_vma bfd_log2(x)
bfd_vma x;
{
  bfd_vma  result = 0;
  while ( (bfd_vma)(1<< result) < x)
    result++;
  return result;
}

/* bfd_get_mtime:  Return cached file modification time (e.g. as read
   from archive header for archive members, or from file system if we have
   been called before); else determine modify time, cache it, and
   return it.  */

long
bfd_get_mtime (abfd)
     bfd *abfd;
{
  FILE *fp;
  struct stat buf;

  if (abfd->mtime_set)
    return abfd->mtime;

  fp = bfd_cache_lookup (abfd);
  if (0 != fstat (fileno (fp), &buf))
    return 0;

  abfd->mtime_set = true;
  abfd->mtime = buf.st_mtime;
  return abfd->mtime;
}
