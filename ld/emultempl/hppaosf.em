# This shell script emits a C file. -*- C -*-
# It does some substitutions.
cat >em_${EMULATION_NAME}.c <<EOF
/* An emulation for HP PA-RISC OSF/1 linkers.
   Copyright (C) 1991 Free Software Foundation, Inc.
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


#include "ld.h"
#include "config.h"
#include "ldemul.h"
#include "ldfile.h"
#include "ldlang.h"
#include "ldmisc.h"

extern  boolean lang_float_flag;


extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;

extern bfd *output_bfd;

static void hppaosf_before_parse()
{
  ldfile_output_architecture = bfd_arch_hppa;
}

static lang_input_statement_type *stub_file = 0;

static lang_input_section_type *stub_input_section = NULL;

extern lang_statement_list_type *stat_ptr;
/* List of statements needed to handle constructors */
extern lang_statement_list_type constructor_list;

static void
hppaosf_search_for_padding_statements(s,prev)
	lang_statement_union_type *s;
	lang_statement_union_type **prev;
{
  lang_statement_union_type *sprev = NULL;
  for (; s != (lang_statement_union_type *) NULL; s = s->next)
    {
      switch (s->header.type)
	{
	case lang_constructors_statement_enum:
	  hppaosf_search_for_padding_statements (constructor_list.head,&constructor_list.head);
	  break;
	case lang_output_section_statement_enum:
	  hppaosf_search_for_padding_statements
	    (s->output_section_statement.children.head,&s->output_section_statement.children.head);
	  break;
	case lang_wild_statement_enum:
	  hppaosf_search_for_padding_statements
	    (s->wild_statement.children.head,&s->wild_statement.children.head);
	  break;
	case lang_data_statement_enum:
	case lang_object_symbols_statement_enum:
	case lang_output_statement_enum:
	case lang_target_statement_enum:
	case lang_input_section_enum:
	case lang_input_statement_enum:
	case lang_assignment_statement_enum:
	case lang_address_statement_enum:
	  break;
	case lang_padding_statement_enum:
	  if ( sprev )
	    {
	      sprev->header.next = s->header.next;
	    }
	  else
	    {
	      **prev = *s;
	    }
	  break;
	default:
	  FAIL ();
	  break;
	}
      sprev = s;
    }
}

static void
hppaosf_finish()
{
  extern asymbol *hppa_look_for_stubs_in_section();
  extern ld_config_type config;

  if (config.relocateable_output == false)
    {
      /* check for needed stubs */
      LANG_FOR_EACH_INPUT_SECTION
	(statement, abfd, section,
	 (
	  {
	    int new_sym_cnt = 0;
	    int i,j;
	    asymbol *syms = hppa_look_for_stubs_in_section (stub_file->the_bfd,
							    abfd,
							    output_bfd,
							    section,
							    statement->asymbols,
							    &new_sym_cnt);

	    if ( (new_sym_cnt > 0) && syms )
	      {
		struct symbol_cache_entry **old_asymbols = stub_file->asymbols;

		stub_file->asymbols = ldmalloc((stub_file->symbol_count + new_sym_cnt) * sizeof(asymbol *));

		for ( j = 0; j < stub_file->symbol_count; j++ )
		  stub_file->asymbols[j] = old_asymbols[j];
		
		for ( j = 0, i = stub_file->symbol_count; j < new_sym_cnt; j++, i++ )
		  stub_file->asymbols[i] = &syms[j];
		
		stub_file->symbol_count += new_sym_cnt;
		/* Now, attach the contents of the new linker stub(s) */
		/* to the linker stub input section. */

		
	      }
	  }
	  )
	 )
      /* Add a statement to get the linker stubs included in the output */
      lang_add_wild(".hppa_linker_stubs",NULL);

      /* If we've added stubs,remove the padding_statements because */
      /* they are no longer valid */
      hppaosf_search_for_padding_statements(stat_ptr->head,&(stat_ptr->head));
    }
}

static void
hppaosf_create_output_section_statements()
{
  asection *stub_sec;
  asection *output_text_sec = bfd_make_section_old_way(output_bfd,".text");
  lang_input_section_type *new_input_sec;
  
  stub_file = lang_add_input_file ("linker stubs",
				   lang_input_file_is_fake_enum,
				   (char *) NULL);
  stub_file->the_bfd = bfd_create ("linker stubs", output_bfd);
  stub_file->symbol_count = 0;
  stub_file->the_bfd->sections = 0;
  
  stub_sec = bfd_make_section_old_way(stub_file->the_bfd,".hppa_linker_stubs");
  stub_sec->output_section = output_text_sec;
  bfd_set_section_flags(stub_file->the_bfd, stub_sec, SEC_HAS_CONTENTS | SEC_ALLOC | SEC_CODE | SEC_RELOC );
  
  /* The user data of a bfd points to the input statement attached */
  stub_file->the_bfd->usrdata  = (void *)stub_file;
  stub_file->common_section =
    bfd_make_section(stub_file->the_bfd,"COMMON");

  new_input_sec = (lang_input_section_type *)stat_alloc(sizeof(lang_input_section_type));
  if ( new_input_sec )
    {
	lang_output_section_statement_type *text_output_sec;
	lang_statement_union_type *stmt;
	lang_wild_statement_type *stub_statement;
	new_input_sec->section = stub_sec;
	new_input_sec->ifile = stub_file;
	new_input_sec->header.type = lang_input_section_enum;
	new_input_sec->header.next = NULL;

	stub_input_section = new_input_sec;

	/* Find the output_section_statement for .text, */
	/* then find the wild_statement for .hppa_linker_stubs */

	text_output_sec = lang_output_section_find(".text");

	stmt = text_output_sec->children.head;

	while (stmt && stmt->header.type != lang_wild_statement_enum)
	  {
		stmt = stmt->header.next;
	  }
	
	if ( stmt )
	  {
		lang_wild_statement_type *wstmt = (lang_wild_statement_type *)stmt;
		lang_list_init(&wstmt->children);	    
		lang_statement_append(&wstmt->children,
				      (lang_statement_union_type *)new_input_sec,
				      &new_input_sec->header.next);
	  }
    }
}

static void
hppaosf_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *
hppaosf_get_script(isfile)
     int *isfile;
EOF

if test -n "$COMPILE_IN"
then
# Scripts compiled in.

# sed commands to quote an ld script as a C string.
sc='s/["\\]/\\&/g
s/$/\\n\\/
1s/^/"{/
$s/$/n}"/
'

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  extern ld_config_type config;

  *isfile = 0;

  if (config.relocateable_output == true && config.build_constructors == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xu`;
  else if (config.relocateable_output == true)
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
  extern ld_config_type config;

  *isfile = 1;

  if (config.relocateable_output == true && config.build_constructors == true)
    return "ldscripts/${EMULATION_NAME}.xu";
  else if (config.relocateable_output == true)
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

struct ld_emulation_xfer_struct ld_hppaosf_emulation = 
{
  hppaosf_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_allocation_default,
  hppaosf_set_output_arch,
  ldemul_default_target,
  before_allocation_default,
  hppaosf_get_script,
  "hppaosf",
  "elf32-hppa",
  hppaosf_finish,
  hppaosf_create_output_section_statements
};
EOF
