/* ldwrite.c -- write out the linked file
   Copyright (C) 1993 Free Software Foundation, Inc.
   Written by Steve Chamberlain sac@cygnus.com

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
#include "ldexp.h"
#include "ldlang.h"
#include "ldwrite.h"
#include "ldmisc.h"
#include "ldgram.h"
#include "ldmain.h"

static void build_link_order PARAMS ((lang_statement_union_type *));
static void print_symbol_table PARAMS ((void));
static void print_file_stuff PARAMS ((lang_input_statement_type *));
static boolean print_symbol PARAMS ((struct bfd_link_hash_entry *, PTR));

/* Build link_order structures for the BFD linker.  */

static void
build_link_order (statement)
     lang_statement_union_type *statement;
{
  switch (statement->header.type)
    {
    case lang_data_statement_enum:
      /* FIXME: This should probably build a link_order, but instead
	 it just does the output directly.  */
      {
	bfd_vma value = statement->data_statement.value;
	bfd_byte play_area[QUAD_SIZE];
	unsigned int size = 0;
	asection *output_section = statement->data_statement.output_section;

	ASSERT (output_section->owner == output_bfd);
	switch (statement->data_statement.type)
	  {
	  case QUAD:
	    bfd_put_64 (output_bfd, value, play_area);
	    size = QUAD_SIZE;
	    break;
	  case LONG:
	    bfd_put_32 (output_bfd, value, play_area);
	    size = LONG_SIZE;
	    break;
	  case SHORT:
	    bfd_put_16 (output_bfd, value, play_area);
	    size = SHORT_SIZE;
	    break;
	  case BYTE:
	    bfd_put_8 (output_bfd, value, play_area);
	    size = BYTE_SIZE;
	    break;
	  default:
	    abort ();
	  }

	if (! bfd_set_section_contents (output_bfd, output_section,
					play_area,
					statement->data_statement.output_vma,
					size))
	  einfo ("%P%X: writing data failed: %E\n");
      }
      break;

    case lang_reloc_statement_enum:
      {
	lang_reloc_statement_type *rs;
	asection *output_section;
	struct bfd_link_order *link_order;

	rs = &statement->reloc_statement;

	output_section = rs->output_section;
	ASSERT (output_section->owner == output_bfd);

	link_order = bfd_new_link_order (output_bfd, output_section);
	if (link_order == NULL)
	  einfo ("%P%F: bfd_new_link_order failed");

	link_order->offset = rs->output_vma;
	link_order->size = bfd_get_reloc_size (rs->howto);

	link_order->u.reloc.p =
	  ((struct bfd_link_order_reloc *)
	   xmalloc (sizeof (struct bfd_link_order_reloc)));

	link_order->u.reloc.p->reloc = rs->reloc;
	link_order->u.reloc.p->addend = rs->addend_value;

	if (rs->section != (asection *) NULL)
	  {
	    ASSERT (rs->name == (const char *) NULL);
	    link_order->type = bfd_section_reloc_link_order;
	    if (rs->section->owner == output_bfd)
	      link_order->u.reloc.p->u.section = rs->section;
	    else
	      {
		link_order->u.reloc.p->u.section = rs->section->output_section;
		link_order->u.reloc.p->addend += rs->section->output_offset;
	      }
	  }
	else
	  {
	    ASSERT (rs->name != (const char *) NULL);
	    link_order->type = bfd_symbol_reloc_link_order;
	    link_order->u.reloc.p->u.name = rs->name;
	  }
      }
      break;

    case lang_input_section_enum:
      /* Create a new link_order in the output section with this
	 attached */
      if (statement->input_section.ifile->just_syms_flag == false)
	{
	  asection *i = statement->input_section.section;
	  asection *output_section = i->output_section;

	  ASSERT (output_section->owner == output_bfd);

	  if ((output_section->flags & SEC_HAS_CONTENTS) != 0)
	    {
	      struct bfd_link_order *link_order;

	      link_order = bfd_new_link_order (output_bfd, output_section);

	      if (i->flags & SEC_NEVER_LOAD)
		{
		  /* We've got a never load section inside one which
		     is going to be output, we'll change it into a
		     fill link_order */
		  link_order->type = bfd_fill_link_order;
		  link_order->u.fill.value = 0;
		}
	      else
		{
		  link_order->type = bfd_indirect_link_order;
		  link_order->u.indirect.section = i;
		  ASSERT (i->output_section == output_section);
		}
	      if (i->_cooked_size)
		link_order->size = i->_cooked_size;
	      else
		link_order->size = bfd_get_section_size_before_reloc (i);
	      link_order->offset = i->output_offset;
	    }
	}
      break;

    case lang_padding_statement_enum:
      /* Make a new link_order with the right filler */
      {
	asection *output_section;
	struct bfd_link_order *link_order;

	output_section = statement->padding_statement.output_section;
	ASSERT (statement->padding_statement.output_section->owner
		== output_bfd);
	if ((output_section->flags & SEC_HAS_CONTENTS) != 0)
	  {
	    link_order = bfd_new_link_order (output_bfd, output_section);
	    link_order->type = bfd_fill_link_order;
	    link_order->size = statement->padding_statement.size;
	    link_order->offset = statement->padding_statement.output_offset;
	    link_order->u.fill.value = statement->padding_statement.fill;
	  }
      }
      break;

    default:
      /* All the other ones fall through */
      break;
    }
}

/* Call BFD to write out the linked file.  */

void
ldwrite ()
{
  lang_for_each_statement (build_link_order);

  if (! bfd_final_link (output_bfd, &link_info))
    einfo ("%F%P: final link failed: %E\n", output_bfd);

  if (config.map_file)
    {
      print_symbol_table ();
      lang_map ();
    }
}

/* Print the symbol table.  */

static void
print_symbol_table ()
{
  fprintf (config.map_file, "**FILES**\n\n");
  lang_for_each_file (print_file_stuff);

  fprintf (config.map_file, "**GLOBAL SYMBOLS**\n\n");
  fprintf (config.map_file, "offset    section    offset   symbol\n");
  bfd_link_hash_traverse (link_info.hash, print_symbol, (PTR) NULL);
}

/* Print information about a file.  */

static void
print_file_stuff (f)
     lang_input_statement_type * f;
{
  fprintf (config.map_file, "  %s\n", f->filename);
  if (f->just_syms_flag)
    {
      fprintf (config.map_file, " symbols only\n");
    }
  else
    {
      asection *s;
      if (true)
	{
	  for (s = f->the_bfd->sections;
	       s != (asection *) NULL;
	       s = s->next)
	    {
	      print_address (s->output_offset);
	      if (s->reloc_done)
		{
		  fprintf (config.map_file, " %08x 2**%2ud %s\n",
			   (unsigned) bfd_get_section_size_after_reloc (s),
			   s->alignment_power, s->name);
		}

	      else
		{
		  fprintf (config.map_file, " %08x 2**%2ud %s\n",
			   (unsigned) bfd_get_section_size_before_reloc (s),
			   s->alignment_power, s->name);
		}
	    }
	}
      else
	{
	  for (s = f->the_bfd->sections;
	       s != (asection *) NULL;
	       s = s->next)
	    {
	      fprintf (config.map_file, "%s ", s->name);
	      print_address (s->output_offset);
	      fprintf (config.map_file, "(%x)",
		       (unsigned) bfd_get_section_size_after_reloc (s));
	    }
	  fprintf (config.map_file, "hex \n");
	}
    }
  print_nl ();
}

/* Print a symbol.  */

/*ARGSUSED*/
static boolean
print_symbol (p, ignore)
     struct bfd_link_hash_entry *p;
     PTR ignore;
{
  while (p->type == bfd_link_hash_indirect
	 || p->type == bfd_link_hash_warning)
    p = p->u.i.link;

  switch (p->type) 
    {
    case bfd_link_hash_new:
      abort ();

    case bfd_link_hash_undefined:
      fprintf (config.map_file, "undefined                     ");
      fprintf (config.map_file, "%s ", p->root.string);
      print_nl ();    
      break;

    case bfd_link_hash_weak:
      fprintf (config.map_file, "weak                          ");
      fprintf (config.map_file, "%s ", p->root.string);
      print_nl ();    
      break;

    case bfd_link_hash_defined:	    
      {
	asection *defsec = p->u.def.section;

	print_address (p->u.def.value);
	if (defsec)
	  {
	    fprintf (config.map_file, "  %-10s",
		     bfd_section_name (output_bfd, defsec));
	    print_space ();
	    print_address (p->u.def.value + defsec->vma);
	  }
	else
	  {
	    fprintf (config.map_file, "         .......");
	  }
	fprintf (config.map_file, " %s ", p->root.string);
      }
      print_nl ();    
      break;

    case bfd_link_hash_common:
      fprintf (config.map_file, "common               ");
      print_address (p->u.c.size);
      fprintf (config.map_file, " %s ", p->root.string);
      print_nl ();
      break;

    default:
      abort ();
    }

  return true;
}
