# This shell script emits a C file. -*- C -*-
#   Copyright (C) 1991, 93, 94, 95, 97, 99, 2000
#   Free Software Foundation, Inc.
#
# This file is part of GLD, the Gnu Linker.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# This file is sourced from elf32.em, and defines extra hppa-elf
# specific routines.
#
cat >>e${EMULATION_NAME}.c <<EOF

#include "ldctor.h"
#include "elf32-hppa.h"

static void hppaelf_set_output_arch PARAMS ((void));
static void hppaelf_create_output_section_statements PARAMS ((void));
static void hppaelf_delete_padding_statements
  PARAMS ((lang_statement_list_type *));
static asection *hppaelf_add_stub_section
  PARAMS ((const char *, asection *));
static void hppaelf_layaout_sections_again PARAMS ((void));
static void hppaelf_finish PARAMS ((void));


/* Fake input file for stubs.  */
static lang_input_statement_type *stub_file;


/* Set the output architecture and machine.  */

static void
hppaelf_set_output_arch ()
{
  unsigned long machine = 0;

  bfd_set_arch_mach (output_bfd, ldfile_output_architecture, machine);
}

/* This is called before the input files are opened.  We create a new
   fake input file to hold the stub sections.  */

static void
hppaelf_create_output_section_statements ()
{
  stub_file = lang_add_input_file ("linker stubs",
				   lang_input_file_is_fake_enum,
				   NULL);
  stub_file->the_bfd = bfd_create ("linker stubs", output_bfd);
  if (stub_file->the_bfd == NULL
      || ! bfd_set_arch_mach (stub_file->the_bfd,
			      bfd_get_arch (output_bfd),
			      bfd_get_mach (output_bfd)))
    {
      einfo ("%X%P: can not create BFD %E\n");
      return;
    }

  ldlang_add_file (stub_file);
}

/* Walk all the lang statements splicing out any padding statements from
   the list.  */

static void
hppaelf_delete_padding_statements (list)
     lang_statement_list_type *list;
{
  lang_statement_union_type *s;
  lang_statement_union_type **ps;
  for (ps = &list->head; (s = *ps) != NULL; ps = &s->next)
    {
      switch (s->header.type)
	{

	/* We want to recursively walk these sections.  */
	case lang_constructors_statement_enum:
	  hppaelf_delete_padding_statements (&constructor_list);
	  break;

	case lang_output_section_statement_enum:
	  hppaelf_delete_padding_statements (&s->output_section_statement.children);
	  break;

	case lang_group_statement_enum:
	  hppaelf_delete_padding_statements (&s->group_statement.children);
	  break;

	case lang_wild_statement_enum:
	  hppaelf_delete_padding_statements (&s->wild_statement.children);
	  break;

	/* Here's what we are really looking for.  Splice these out of
	   the list.  */
	case lang_padding_statement_enum:
	  *ps = s->next;
	  if (*ps == NULL)
	    list->tail = ps;
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
    }
}


struct hook_stub_info
{
  lang_statement_list_type add;
  asection *input_section;
};

/* Traverse the linker tree to find the spot where the stub goes.  */

static boolean hook_in_stub
  PARAMS ((struct hook_stub_info *, lang_statement_union_type **));

static boolean
hook_in_stub (info, lp)
     struct hook_stub_info *info;
     lang_statement_union_type **lp;
{
  lang_statement_union_type *l;
  boolean ret;

  for (; (l = *lp) != NULL; lp = &l->next)
    {
      switch (l->header.type)
	{
	case lang_constructors_statement_enum:
	  ret = hook_in_stub (info, &constructor_list.head);
	  if (ret)
	    return ret;
	  break;

	case lang_output_section_statement_enum:
	  ret = hook_in_stub (info,
			      &l->output_section_statement.children.head);
	  if (ret)
	    return ret;
	  break;

	case lang_wild_statement_enum:
	  ret = hook_in_stub (info, &l->wild_statement.children.head);
	  if (ret)
	    return ret;
	  break;

	case lang_group_statement_enum:
	  ret = hook_in_stub (info, &l->group_statement.children.head);
	  if (ret)
	    return ret;
	  break;

	case lang_input_section_enum:
	  if (l->input_section.section == info->input_section)
	    {
	      /* We've found our section.  Insert the stub immediately
		 before its associated input section.  */
	      *lp = info->add.head;
	      *(info->add.tail) = l;
	      return true;
	    }
	  break;

	case lang_data_statement_enum:
	case lang_reloc_statement_enum:
	case lang_object_symbols_statement_enum:
	case lang_output_statement_enum:
	case lang_target_statement_enum:
	case lang_input_statement_enum:
	case lang_assignment_statement_enum:
	case lang_padding_statement_enum:
	case lang_address_statement_enum:
	case lang_fill_statement_enum:
	  break;

	default:
	  FAIL ();
	  break;
	}
    }
  return false;
}


/* Call-back for elf32_hppa_size_stubs.  */

/* Create a new stub section, and arrange for it to be linked
   immediately before INPUT_SECTION.  */

static asection *
hppaelf_add_stub_section (stub_name, input_section)
     const char *stub_name;
     asection *input_section;
{
  asection *stub_sec;
  flagword flags;
  asection *output_section;
  const char *secname;
  lang_output_section_statement_type *os;
  struct hook_stub_info info;

  stub_sec = bfd_make_section_anyway (stub_file->the_bfd, stub_name);
  if (stub_sec == NULL)
    goto err_ret;

  flags = (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE
	   | SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_KEEP);
  if (!bfd_set_section_flags (stub_file->the_bfd, stub_sec, flags))
    goto err_ret;

  output_section = input_section->output_section;
  secname = bfd_get_section_name (output_section->owner, output_section);
  os = lang_output_section_find (secname);

  info.input_section = input_section;
  lang_list_init (&info.add);
  wild_doit (&info.add, stub_sec, os, stub_file);

  if (info.add.head == NULL)
    goto err_ret;

  if (hook_in_stub (&info, &os->children.head))
    return stub_sec;

 err_ret:
  einfo ("%X%P: can not make stub section: %E\n");
  return NULL;
}


/* Another call-back for elf32_hppa_size_stubs.  */

static void
hppaelf_layaout_sections_again ()
{
  /* If we have changed sizes of the stub sections, then we need
     to recalculate all the section offsets.  This may mean we need to
     add even more stubs.  */

  /* Delete all the padding statements, they're no longer valid.  */
  hppaelf_delete_padding_statements (stat_ptr);

  /* Resize the sections.  */
  lang_size_sections (stat_ptr->head, abs_output_section,
		      &stat_ptr->head, 0, (bfd_vma) 0, false);

  /* Redo special stuff.  */
  ldemul_after_allocation ();

  /* Do the assignments again.  */
  lang_do_assignments (stat_ptr->head, abs_output_section,
		       (fill_type) 0, (bfd_vma) 0);
}


/* Final emulation specific call.  For the PA we use this opportunity
   to build linker stubs.  */

static void
hppaelf_finish ()
{
  /* If generating a relocateable output file, then we don't
     have to examine the relocs.  */
  if (link_info.relocateable)
    return;

  /* Call into the BFD backend to do the real work.  */
  if (! elf32_hppa_size_stubs (stub_file->the_bfd,
			       &link_info,
			       &hppaelf_add_stub_section,
			       &hppaelf_layaout_sections_again))
    {
      einfo ("%X%P: can not size stub section: %E\n");
      return;
    }

  /* Now build the linker stubs.  */
  if (stub_file->the_bfd->sections != NULL)
    {
      if (! elf32_hppa_build_stubs (stub_file->the_bfd, &link_info))
	einfo ("%X%P: can not build stubs: %E\n");
    }
}

EOF

# Put these routines in ld_${EMULATION_NAME}_emulation
#
LDEMUL_SET_OUTPUT_ARCH=hppaelf_set_output_arch
LDEMUL_FINISH=hppaelf_finish
LDEMUL_CREATE_OUTPUT_SECTION_STATEMENTS=hppaelf_create_output_section_statements
