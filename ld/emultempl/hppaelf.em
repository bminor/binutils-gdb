# This shell script emits a C file. -*- C -*-
# It does some substitutions.
cat >em_${EMULATION_NAME}.c <<EOF
/* An emulation for HP PA-RISC ELF linkers.
   Copyright (C) 1991, 1993 Free Software Foundation, Inc.
   Written by Steve Chamberlain steve@cygnus.com

This file is part of GLD, the Gnu Linker.

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

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"

#include "ld.h"
#include "config.h"
#include "ldemul.h"
#include "ldfile.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldmisc.h"
#include "ldmain.h"
#include "ldctor.h"

static lang_input_statement_type *stub_file = 0;
static lang_input_section_type *stub_input_section = NULL;

/* FIXME.  This doesn't belong here.  */
extern asymbol *hppa_look_for_stubs_in_section ();
  
/* Perform some emulation specific initialization.  For PA ELF we set
   up the local label prefix and the output architecture.  */

static void
hppaelf_before_parse ()
{
  link_info.lprefix = "L$";
  link_info.lprefix_len = 2;

  ldfile_output_architecture = bfd_arch_hppa;
}

/* Walk all the lang statements splicing out any padding statements from 
   the list.  */

static void
hppaelf_search_for_padding_statements (s, prev)
     lang_statement_union_type *s;
     lang_statement_union_type **prev;
{
  lang_statement_union_type *sprev = NULL;
  for (; s != NULL; s = s->next)
    {
      switch (s->header.type)
	{

	/* We want recursively walk these sections.  */
	case lang_constructors_statement_enum:
	  hppaelf_search_for_padding_statements (constructor_list.head,
						 &constructor_list.head);
	  break;

	case lang_output_section_statement_enum:
	  hppaelf_search_for_padding_statements (s->output_section_statement.
						   children.head,
						 &s->output_section_statement.
						   children.head);
	  break;

	/* Huh?  What is a lang_wild_statement?  */
	case lang_wild_statement_enum:
	  hppaelf_search_for_padding_statements(s->wild_statement.
						  children.head,
						&s->wild_statement.
						  children.head);
	  break;

	/* Here's what we are really looking for.  Splice these out of
	   the list.  */
	case lang_padding_statement_enum:
	  if (sprev)
	    sprev->header.next = s->header.next;
	  else
	    **prev = *s;
	  break;

	/* We don't care about these cases.  */
	case lang_data_statement_enum:
	case lang_object_symbols_statement_enum:
	case lang_output_statement_enum:
	case lang_target_statement_enum:
	case lang_input_section_enum:
	case lang_input_statement_enum:
	case lang_assignment_statement_enum:
	case lang_address_statement_enum:
	  break;

	default:
	  abort ();
	  break;
	}
      sprev = s;
    }
}

/* Final emulation specific call.  For the PA we use this opportunity
   to determine what linker stubs are needed and generate them.
   
   FIXME: fast-linker work broke this in a big way.  statement->asymbols
   doesn't have anything useful in it anymore.  And if we slurp in
   the symbol table here and pass it down then we get lots of undefined
   symbols.  Egad.  */

static void
hppaelf_finish ()
{

  /* Disabled until it's fixed to work with the new linker.  A noteworty
     amount of code will still function without linker stubs allowing us
     to continue testing.  */

#if 0  
  /* Only create stubs for final objects.  */
  if (link_info.relocateable == false)
    {
      lang_input_statement_type *statement;
      
      /* Look at all the statements.  */
      for (statement = (lang_input_statement_type *)file_chain.head;
	   statement != NULL;
	   statement = (lang_input_statement_type *)statement->next)
	{
	  asection *section;
	  bfd *abfd = statement->the_bfd;
	  
	  /* Look at all the sections attached to the bfd associated with
	     the current statement.  */
	  for (section = abfd->sections;
	       section != (asection *)NULL;
	       section = section ->next)
	    {
	      int new_sym_cnt = 0;
	      int i,j;
	      asymbol *syms;
	      
	      /* Do the dirty work; an array of symbols for each new stub 
		 will be returned.  */
	      syms = hppa_look_for_stubs_in_section (stub_file->the_bfd,
						     abfd,
						     output_bfd,
						     section,
						     statement->asymbols,
						     &new_sym_cnt,
						     &link_info);
	      
	      if (new_sym_cnt > 0 && syms)
		{
		  struct symbol_cache_entry **old_asymbols;
		  
		  old_asymbols = stub_file->asymbols;
		  
		  /* Allocate space for the updated symbols  */ 
		  stub_file->asymbols
		    = xmalloc ((stub_file->symbol_count + new_sym_cnt)
			       * sizeof(asymbol *));
		  if (stub_file->asymbols == NULL)
		    abort ();
		  
		  /* Copy the old symbols.

		     FIXME.  Shouldn't we free the space used by the old 
		     symbols here?  Might there be dangling references
		     made within hppa_look_for_stubs_in_section?  */
		  for (j = 0; j < stub_file->symbol_count; j++)
		    stub_file->asymbols[j] = old_asymbols[j];
		  
		  /* Copy in the new symbols.  */
		  for (j = 0, i = stub_file->symbol_count;
		       j < new_sym_cnt;
		       j++, i++)
		    stub_file->asymbols[i] = &syms[j];
		  
		  /* Finally, adjust the symbol count.  */
		  stub_file->symbol_count += new_sym_cnt;
		}
	    }
        }
      
      /* Add a statement to get the linker stubs included in the output.  */
      lang_add_wild (".hppa_linker_stubs",NULL);
      
      /* If stubs were added, then remove all the (now invalid) padding
	 statements.  */
      hppaelf_search_for_padding_statements (stat_ptr->head, 
					     &stat_ptr->head);
    }
  
  /* Size up the sections again.  */
  lang_size_sections (stat_ptr->head, abs_output_section,
		      &stat_ptr->head, 0, (bfd_vma) 0, false);
  
  /* FIXME:  Do we need to redo the "assignments" too?  */
#endif
}

/* Create any emulation specific output statements.  FIXME?  Is this
   redundant with above lang_add_wild or the code in the script?  */

static void
hppaelf_create_output_section_statements ()
{
  asection *stub_sec;
  asection *output_text_sec = bfd_make_section_old_way (output_bfd, ".text");
  lang_input_section_type *new_input_sec;

  /* Add a new "input file" (the linker stubs themselves).  */
  stub_file = lang_add_input_file ("linker stubs",
				   lang_input_file_is_fake_enum,
				   NULL);
  stub_file->the_bfd = bfd_create ("linker stubs", output_bfd);
  stub_file->symbol_count = 0;
  stub_file->the_bfd->sections = 0;

  /* Add a section to the fake input file.  */
  stub_sec = bfd_make_section_old_way (stub_file->the_bfd,
				       ".hppa_linker_stubs");
  stub_sec->output_section = output_text_sec;
  bfd_set_section_flags (stub_file->the_bfd, stub_sec,
			 SEC_HAS_CONTENTS | SEC_ALLOC | SEC_CODE | SEC_RELOC);
  
  /* The user data of a bfd points to the input statement attached.  */
  stub_file->the_bfd->usrdata  = (void *)stub_file;
  stub_file->common_section = bfd_make_section(stub_file->the_bfd,"COMMON");

  new_input_sec = (lang_input_section_type *)
    stat_alloc (sizeof (lang_input_section_type));

  if (new_input_sec)
    {
	lang_output_section_statement_type *text_output_sec;
	lang_statement_union_type *stmt;

	new_input_sec->section = stub_sec;
	new_input_sec->ifile = stub_file;
	new_input_sec->header.type = lang_input_section_enum;
	new_input_sec->header.next = NULL;

	stub_input_section = new_input_sec;

	/* Find the output_section_statement for .text,
	   then find the wild_statement for .hppa_linker_stubs.  */
	text_output_sec = lang_output_section_find (".text");

	stmt = text_output_sec->children.head;

	while (stmt && stmt->header.type != lang_wild_statement_enum)
	  stmt = stmt->header.next;

	/* Do something with the wild statement.  FIXME.  */
	if (stmt)
	  {
	    lang_wild_statement_type *wstmt = (lang_wild_statement_type *)stmt;
	    lang_list_init (&wstmt->children);	    
	    lang_statement_append (&wstmt->children,
				   (lang_statement_union_type *)new_input_sec,
				   &new_input_sec->header.next);
	  }
    }
}

/* Set the output architecture and machine.  */

static void
hppaelf_set_output_arch()
{
  unsigned long machine = 0;

  bfd_set_arch_mach (output_bfd, ldfile_output_architecture, machine);
}

/* The script itself gets inserted here.  */

static char *
hppaelf_get_script(isfile)
     int *isfile;
EOF

if test -n "$COMPILE_IN"
then
# Scripts compiled in.

# sed commands to quote an ld script as a C string.
sc='s/["\\]/\\&/g
s/$/\\n\\/
1s/^/"/
$s/$/n"/
'

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  *isfile = 0;

  if (link_info.relocateable == true && config.build_constructors == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xu`;
  else if (link_info.relocateable == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xr`;
  else if (!config.text_read_only)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xbn`;
  else if (!config.magic_demand_paged)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xn`;
  else
    return `sed "$sc" ldscripts/${EMULATION_NAME}.x`;
}
EOF

else
# Scripts read from the filesystem.

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  *isfile = 1;

  if (link_info.relocateable == true && config.build_constructors == true)
    return "ldscripts/${EMULATION_NAME}.xu";
  else if (link_info.relocateable == true)
    return "ldscripts/${EMULATION_NAME}.xr";
  else if (!config.text_read_only)
    return "ldscripts/${EMULATION_NAME}.xbn";
  else if (!config.magic_demand_paged)
    return "ldscripts/${EMULATION_NAME}.xn";
  else
    return "ldscripts/${EMULATION_NAME}.x";
}
EOF

fi

cat >>em_${EMULATION_NAME}.c <<EOF

struct ld_emulation_xfer_struct ld_hppaelf_emulation = 
{
  hppaelf_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_allocation_default,
  hppaelf_set_output_arch,
  ldemul_default_target,
  before_allocation_default,
  hppaelf_get_script,
  "hppaelf",
  "elf32-hppa",
  hppaelf_finish,
  hppaelf_create_output_section_statements
};
EOF
