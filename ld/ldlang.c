/* Copyright (C) 1991 Free Software Foundation, Inc.
   
This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ 
 *
*/



#include "sysdep.h" 
#include "bfd.h"

#include "ld.h"
#include "ldmain.h"
#include "ldsym.h"
#include "ldgram.tab.h"
#include "ldmisc.h"
#include "ldlang.h"
#include "ldexp.h"
#include "ld-emul.h"
#include "ldlex.h"

/* EXPORTS */



extern unsigned int undefined_global_sym_count;

static char *startup_file;
static lang_input_statement_type *first_file;
lang_statement_list_type statement_list;
lang_statement_list_type *stat_ptr = &statement_list;
lang_statement_list_type lang_output_section_statement;
lang_statement_list_type input_file_chain;
lang_statement_list_type file_chain;
extern char *current_file;
static boolean placed_commons = false;

boolean lang_float_flag;

static lang_output_section_statement_type *default_common_section;


/* FORWARDS */
PROTO(static void, print_statements,(void));
PROTO(static void, print_statement,(lang_statement_union_type *,
				     lang_output_section_statement_type *));



/* EXPORTS */
boolean lang_has_input_file = false;


extern bfd *output_bfd;
size_t largest_section;


extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;


extern ldsym_type *symbol_head;

bfd_vma print_dot;
unsigned int commons_pending;




extern args_type command_line;
extern ld_config_type config;

char *entry_symbol;



lang_output_section_statement_type *create_object_symbols;

extern boolean had_script;
static boolean map_option_f;


boolean had_output_filename = false;
extern boolean write_map;






size_t longest_section_name = 8;


lang_input_statement_type *script_file;

section_userdata_type common_section_userdata;
asection common_section;

#ifdef __STDC__
#define cat(a,b) a##b
#else
#define cat(a,b) a/**/b
#endif

#define new_stat(x,y) (cat(x,_type)*) new_statement(cat(x,_enum), sizeof(cat(x,_type)),y)

#define outside_section_address(q) ( (q)->output_offset + (q)->output_section->vma)

#define outside_symbol_address(q) ((q)->value +   outside_section_address(q->section))

boolean option_longmap = false;

static void lang_list_init(list)
lang_statement_list_type *list;
{
list->head = (lang_statement_union_type *)NULL;
list->tail = &list->head;
}

static void 
print_section(name)
char *name;
{
  printf("%*s", -longest_section_name, name);
}
static void 
print_space()
{
  printf(" ");
}
static void 
print_nl()
{
  printf("\n");
}
static void 
print_address(value)
bfd_vma value;
{
  printf("%8lx", value);
}
static void 
print_size(value)
size_t value;
{
  printf("%5x", (unsigned)value);
}
static void 
print_alignment(value)
unsigned int value;
{
  printf("2**%2u",value);
}
static void 
print_fill(value)
fill_type value;
{
  printf("%04x",(unsigned)value);
}


static
lang_statement_union_type *new_statement(type, size, list)
enum statement_enum type;
size_t size;
lang_statement_list_type *list;
{
  lang_statement_union_type *new = (lang_statement_union_type *)
    ldmalloc(size);
  new->header.type = type;
  new->header.next = (lang_statement_union_type *)NULL;
 lang_statement_append(list, new, &new->header.next);
  return new;
}

static lang_input_statement_type *
new_afile(name, file_type, target)
char *name;
lang_input_file_enum_type file_type;
char *target;
{
  lang_input_statement_type *p = new_stat(lang_input_statement, 
					  stat_ptr);
  lang_has_input_file = true;
  p->target = target;
  switch (file_type) {
  case  lang_input_file_is_symbols_only_enum:
    p->filename = name;
    p->is_archive =false;
    p->real = true;
    p->local_sym_name= name;
    p->just_syms_flag = true;
    p->search_dirs_flag = false;
    break;
  case lang_input_file_is_fake_enum:
    p->filename = name;
    p->is_archive =false;
    p->real = false;
    p->local_sym_name= name;
    p->just_syms_flag = false;
    p->search_dirs_flag =false;

    break;
  case lang_input_file_is_l_enum:
    p->is_archive = true;
    p->filename = name;
    p->real = true;
    p->local_sym_name = concat("-l",name,"");
    p->just_syms_flag = false;
    p->search_dirs_flag = true;
    break;

  case lang_input_file_is_search_file_enum:
  case lang_input_file_is_marker_enum:
    p->filename = name;
    p->is_archive =false;
    p->real = true;
    p->local_sym_name= name;
    p->just_syms_flag = false;
    p->search_dirs_flag =true;
    break;



  case lang_input_file_is_file_enum:
    p->filename = name;
    p->is_archive =false;
    p->real = true;
    p->local_sym_name= name;
    p->just_syms_flag = false;
    p->search_dirs_flag =false;
    break;


  default:
    FAIL();
  }
  p->asymbols = (asymbol **)NULL;
  p->superfile = (lang_input_statement_type *)NULL;

  p->next_real_file = (lang_statement_union_type*)NULL;
  p->next = (lang_statement_union_type*)NULL;
  p->symbol_count = 0;
  p->common_output_section = (asection *)NULL;

  lang_statement_append(&input_file_chain,
			(lang_statement_union_type *)p,
			&p->next_real_file);
  return p;
}

lang_input_statement_type *
lang_add_input_file(name,
		    file_type, 
		    target)
char *name;
lang_input_file_enum_type file_type;
char *target;
{
  /* Look it up or build a new one */

  lang_input_statement_type *p;

  for (p = (lang_input_statement_type *)input_file_chain.head;
       p != (lang_input_statement_type *)NULL;
       p = (lang_input_statement_type *)(p->next_real_file))
    {
      /* Sometimes we have incomplete entries in here */
      if (p->filename != (char *)NULL) {
	if(strcmp(name,p->filename) == 0)  return p;
      }
    }

  return  new_afile(name, file_type, target);
}



void
lang_init()
{

  stat_ptr= &statement_list;
  lang_list_init(stat_ptr);

  lang_list_init(&input_file_chain);
  lang_list_init(&lang_output_section_statement);
  lang_list_init(&file_chain);
  first_file = lang_add_input_file((char *)NULL, 
				   lang_input_file_is_marker_enum,
				   (char *)NULL);

}

static void 
lang_init2()
{
  script_file = lang_add_input_file("script file",
				    lang_input_file_is_fake_enum,
				    (char *)NULL);
  script_file->the_bfd = bfd_create("script file", output_bfd);
  script_file->symbol_count = 0;

  common_section.userdata = &common_section_userdata;

}



/*  this function mainains a dictionary of regions. If the *default*
    region is asked for then a pointer to the first region is
    returned. If there is no first pointer then one is created 
*/

static lang_memory_region_type *lang_memory_region_list;
static lang_memory_region_type **lang_memory_region_list_tail = &lang_memory_region_list;

lang_memory_region_type *
lang_memory_region_lookup(name)
char *name;
{

    lang_memory_region_type *p  = lang_memory_region_list;
    for (p = lang_memory_region_list;
	 p != ( lang_memory_region_type *)NULL;
	 p = p->next) {
	if (strcmp(p->name, name) == 0) {
	    return p;
	}
    }
    if (strcmp(name,"*default*")==0) {
	/* This is the default region, dig out first one on the list */
	if (lang_memory_region_list != (lang_memory_region_type*)NULL){
	    return lang_memory_region_list;
	}
    }
    {
	lang_memory_region_type *new =
	    (lang_memory_region_type *)ldmalloc(sizeof(lang_memory_region_type));
	new->name = name;
	new->next = (lang_memory_region_type *)NULL;

	*lang_memory_region_list_tail = new;
	lang_memory_region_list_tail = &new->next;
	new->origin = 0;
	new->length = ~0;
	new->current = 0;
	return new;
    }
}



lang_output_section_statement_type *
lang_output_section_find(name)
char *name;
{
  lang_statement_union_type *u;
  lang_output_section_statement_type *lookup;

  for (u = lang_output_section_statement.head;
       u != (lang_statement_union_type *)NULL;
       u = lookup->next)
    {
      lookup = &u->output_section_statement;
      if (strcmp(name, lookup->name)==0) {
	return lookup;
      }
    }
  return (lang_output_section_statement_type *)NULL;
}

lang_output_section_statement_type *
lang_output_section_statement_lookup(name)
char *name;

{
  lang_output_section_statement_type *lookup;
  lookup =lang_output_section_find(name);
  if (lookup == (lang_output_section_statement_type *)NULL) {
    
    lookup =(lang_output_section_statement_type *)
      new_stat(lang_output_section_statement, stat_ptr);
    lookup->region = (lang_memory_region_type *)NULL;
    lookup->fill = 0;
    lookup->block_value = 1;
    lookup->name = name;

    lookup->next = (lang_statement_union_type*)NULL;  
    lookup->bfd_section = (asection *)NULL;
    lookup->processed = false;
    lookup->addr_tree = (etree_type *)NULL;
    lang_list_init(&lookup->children);

    lang_statement_append(&lang_output_section_statement,
			  (lang_statement_union_type *)lookup,
			  &lookup->next);
  }
  return lookup;
}





static void
  print_flags(outfile, ignore_flags)
FILE *outfile;
lang_section_flags_type *ignore_flags;
{
  fprintf(outfile,"(");
#if 0
  if (flags->flag_read) fprintf(outfile,"R");
  if (flags->flag_write) fprintf(outfile,"W");
  if (flags->flag_executable) fprintf(outfile,"X");
  if (flags->flag_loadable) fprintf(outfile,"L");
#endif
  fprintf(outfile,")");
}

void
lang_map(outfile)
     FILE *outfile;
{
  lang_memory_region_type *m;
  fprintf(outfile,"**MEMORY CONFIGURATION**\n\n");

  fprintf(outfile,"name\t\torigin\t\tlength\t\tattributes\n");
  for (m = lang_memory_region_list;
       m != (lang_memory_region_type *)NULL;
       m = m->next) 
    {
      fprintf(outfile,"%-16s", m->name);

      fprintf(outfile,"%08lx\t%08lx\t", m->origin, m->length);
      print_flags(outfile, &m->flags);
      fprintf(outfile,"\n");
    }
  fprintf(outfile,"\n\n**LINK EDITOR MEMORY MAP**\n\n");
  fprintf(outfile,"output\t\tinput\t\tvirtual\n");
  fprintf(outfile,"section\t\tsection\t\taddress\tsize\n\n");

  print_statements();

}

/*
 *
 */
static void init_os(s) 
lang_output_section_statement_type *s;
{
  section_userdata_type *new =
    (section_userdata_type *)
      ldmalloc(sizeof(section_userdata_type));

  s->bfd_section = bfd_make_section(output_bfd, s->name);
  s->bfd_section->output_section = s->bfd_section;
  s->bfd_section->flags = SEC_NO_FLAGS;
  /* We initialize an output sections output offset to minus its own */
  /* vma to allow us to output a section through itself */
  s->bfd_section->output_offset = 0;
  get_userdata( s->bfd_section) = new;
}

static void
wild_doit(ptr, section,output, file)
lang_statement_list_type *ptr;
asection *section;
lang_output_section_statement_type *output;
lang_input_statement_type *file;
{
  if(output->bfd_section == (asection *)NULL)
    {
      init_os(output);
    }

  if (section != (asection *)NULL 
      && section->output_section == (asection *)NULL) {
    /* Add a section reference to the list */
    lang_input_section_type *new = new_stat(lang_input_section, ptr);

    new->section = section;
    new->ifile = file;
    section->output_section = output->bfd_section;
    section->output_section->flags |= section->flags;
    if (section->alignment_power > output->bfd_section->alignment_power) {
	output->bfd_section->alignment_power = section->alignment_power;
      }
      
  }
}

static asection *
our_bfd_get_section_by_name(abfd, section)
bfd *abfd;
char *section;
{
  return bfd_get_section_by_name(abfd, section);

}
static void 
wild_section(ptr, section, file , output)
lang_wild_statement_type *ptr;
char *section;
lang_input_statement_type *file;
lang_output_section_statement_type *output;
{
  asection *s;
  if (section == (char *)NULL) {
    /* Do the creation to all sections in the file */
    for (s = file->the_bfd->sections; s != (asection *)NULL; s=s->next)  {
      wild_doit(&ptr->children, s, output, file);
    }
  }
  else {
    /* Do the creation to the named section only */
    wild_doit(&ptr->children,
	      our_bfd_get_section_by_name(file->the_bfd, section),
	      output, file);
  }



}



static
lang_input_statement_type *lookup_name(name, target)
char *name;
char *target;
{
  lang_input_statement_type *search;
  for(search = (lang_input_statement_type *)input_file_chain.head;
      search != (lang_input_statement_type *)NULL;
      search = (lang_input_statement_type *)search->next_real_file)
    {
      if (search->filename == (char *)NULL && name == (char *)NULL) {
	return search;
      }
      if (search->filename != (char *)NULL && name != (char *)NULL) {
	if (strcmp(search->filename, name) == 0)  {
	  Q_read_file_symbols(search);
	  return search;
	}
      }
    }

  /* There isn't an afile entry for this file yet, this must be */
  /* because the name has only appeared inside a load script and not */
  /* on the command line */
  search = new_afile(name, lang_input_file_is_file_enum, target);
  Q_read_file_symbols(search);
  return search;
}

static void

wild(s, section, file, target, output)
lang_wild_statement_type *s;
char *section;
char *file;
char *target;
lang_output_section_statement_type *output;
{
  lang_input_statement_type *f;
  if (file == (char *)NULL) {
    /* Perform the iteration over all files in the list */
    for (f = (lang_input_statement_type *)file_chain.head;
	 f != (lang_input_statement_type *)NULL;
	 f = (lang_input_statement_type *)f->next) {
      wild_section(s,  section, f, output);
    }
  }
  else {
    /* Perform the iteration over a single file */
    wild_section( s, section, lookup_name(file, target), output);
  }
}

/*
  read in all the files 
  */
static bfd *	
open_output(name, target)
char *name;
char *target;
{
  extern char *output_filename;
  bfd * output = bfd_openw(name, target);
  output_filename = name;	    
  if (output == (bfd *)NULL) 
    {
      if (bfd_error == invalid_target) {
	info("%P%F target %s not found\n", target);
      }
      info("%P%F problem opening output file %s, %E", name);
    }
  
  output->flags |= D_PAGED;
  bfd_set_format(output, bfd_object);
  return output;
}
extern char  *default_target;
static void
lang_phase_0(sh,target)
lang_statement_union_type *sh;
char *target;
{
  lang_statement_union_type *s = (lang_statement_union_type *)sh;
  for (; s != (lang_statement_union_type *)NULL ; s = s->next) 
    {
      switch (s->header.type) {
      case lang_output_section_statement_enum:
	lang_phase_0(s->output_section_statement.children.head,
		     target);
	break;
      case lang_output_statement_enum:
#if 1
	output_bfd = open_output(s->output_statement.name,
				 target == (char *)NULL ?
				 default_target : target);
	ldemul_set_output_arch();
#endif
	break;
      case lang_target_statement_enum:
	target = s->target_statement.target;
	break;
      case lang_wild_statement_enum:
	/* Maybe we should load the file's symbols */
	if (s->wild_statement.filename) {
	  (void)  lookup_name(s->wild_statement.filename, target);
	}
	break;
	/* Attatch this to the current output section */
      case lang_common_statement_enum:
      case lang_fill_statement_enum:
      case lang_input_section_enum:
      case lang_object_symbols_statement_enum:
      case lang_address_statement_enum:
      case lang_data_statement_enum:
	break;
      case lang_afile_asection_pair_statement_enum:

	FAIL();
	break;
	   
      case lang_input_statement_enum:
	if (s->input_statement.real == true) {
	  s->input_statement.target = target;
	  lookup_name(s->input_statement.filename, target);
	}
	break;
      case lang_assignment_statement_enum:
#if 0
	(void)	  exp_fold_tree(s->assignment_statement.exp,
				output_section,
				false);
#endif
	break;

      case lang_padding_statement_enum:
	
	break;
      }
    }

}

/* If there are [COMMONS] statements, put a wild one into the bss section */

static void
lang_reasonable_defaults()
{

      lang_output_section_statement_lookup(".text");
      lang_output_section_statement_lookup(".data");

  default_common_section = 
    lang_output_section_statement_lookup(".bss");

  if (placed_commons == false) {
    lang_wild_statement_type *new =
      new_stat(lang_wild_statement,
	       &default_common_section->children);
    new->section_name = "COMMON";
    new->filename = (char *)NULL;
    lang_list_init(&new->children);
  }

}

static void lang()
{
  if (had_script == false) {
    parse_line(ldemul_get_script());
  }

  lang_reasonable_defaults();
  lang_phase_0(statement_list.head,default_target);
}


/* Open input files and attatch to output sections */
static void
lang_open_input(s, target, output_section_statement)
lang_statement_union_type *s;
char *target;
lang_output_section_statement_type *output_section_statement;
{
  for (; s != (lang_statement_union_type *)NULL ; s = s->next) 
    {
      switch (s->header.type) {
      case lang_wild_statement_enum:
 	wild(&s->wild_statement, s->wild_statement.section_name,
	     s->wild_statement.filename, target,
	     output_section_statement);

	break;

      case lang_output_section_statement_enum:
	lang_open_input(s->output_section_statement.children.head,
			target,
			&s->output_section_statement);
	break;
      case lang_output_statement_enum:
	break;
      case lang_target_statement_enum:
	target = s->target_statement.target;
	break;
      case lang_common_statement_enum:
      case lang_fill_statement_enum:
      case lang_input_section_enum:
      case lang_object_symbols_statement_enum:
      case lang_data_statement_enum:
	break;
      case lang_afile_asection_pair_statement_enum:
	FAIL();
	break;

      case lang_assignment_statement_enum:
      case lang_padding_statement_enum:

	break;
      case lang_address_statement_enum:
	/* Mark the specified section with the supplied address */
	{
	  lang_output_section_statement_type *os =
	    lang_output_section_statement_lookup
	      (s->address_statement.section_name);
	  os->addr_tree = s->address_statement.address;
	}
	break;
      case lang_input_statement_enum:
	/* A standard input statement, has no wildcards */
	/*	Q_read_file_symbols(&s->input_statement);*/
	break;
      }
    }
}





static void 
print_output_section_statement(output_section_statement)
lang_output_section_statement_type *output_section_statement;
{
  asection *section = output_section_statement->bfd_section;
  print_nl();
  print_section(output_section_statement->name);

  if (section) {
    print_dot = section->vma;
    print_space();
    print_section("");
    print_space();
    print_address(section->vma);
    print_space();
    print_size(section->size);
    print_space();
    print_alignment(section->alignment_power);
    print_space();
#if 0
    printf("%s flags", output_section_statement->region->name);
    print_flags(stdout, &output_section_statement->flags);
#endif

  }
  else {
    printf("No attached output section");
  }
  print_nl();
  print_statement(output_section_statement->children.head,
		  output_section_statement);

}

static void 
print_assignment(assignment, output_section)
lang_assignment_statement_type *assignment;
lang_output_section_statement_type *output_section;
{
  etree_value_type result;
  print_section("");
  print_space();
  print_section("");
  print_space();
  print_address(print_dot);
  print_space();
  result = exp_fold_tree(assignment->exp->assign.src,
			 output_section,
			 lang_final_phase_enum,
			 print_dot,
			 &print_dot);
			 
  if (result.valid) {
    print_address(result.value);
  }
  else 
    {
      printf("*undefined*");
    }
  print_space();
  exp_print_tree(stdout, assignment->exp);
  printf("\n");
}

static void
print_input_statement(statm)
lang_input_statement_type *statm;
{
  printf("LOAD %s\n",statm->filename);
}

static void print_symbol(q)
asymbol *q;
{
  print_section("");
  printf(" ");
  print_section("");
  printf(" ");
  print_address(outside_symbol_address(q));
  printf("              %s", q->name ? q->name : " ");
  print_nl();
}
static void 
print_input_section(in)
lang_input_section_type *in;
{
  asection *i = in->section;

  if(i->size != 0) {
    print_section("");
    printf(" ");
    print_section(i->name);
    printf(" ");
    if (i->output_section) {
      print_address(i->output_section->vma + i->output_offset);
      printf(" ");
      print_size(i->size);
      printf(" ");
      print_alignment(i->alignment_power);
      printf(" ");
      if (in->ifile) {
	bfd *abfd = in->ifile->the_bfd;
	printf(" %s ",abfd->xvec->name);
	if(abfd->my_archive != (bfd *)NULL) {
	  printf("[%s]%s", abfd->my_archive->filename,
		 abfd->filename);
	}
	else {
	  printf("%s", abfd->filename);
	}
	print_nl();

	/* Find all the symbols in this file defined in this section */
	{
	  asymbol **p;
	  for (p = in->ifile->asymbols; *p; p++) {
	    asymbol *q = *p;
	
	    if (bfd_get_section(q) == i && q->flags & BSF_GLOBAL) {
	      print_symbol(q);
	    }
	  }
	}
      }
      else {
	print_nl();
      }


      print_dot = outside_section_address(i) + i->size;
    }
    else {
      printf("No output section allocated\n");
    }
  }
}
static void
print_common_statement()
{
  ldsym_type *lgs;
  print_section("");
  print_space();
  print_section(common_section.output_section->name);
  print_space();
  print_address(common_section.output_offset +
		common_section.output_section->vma);
  print_space();
  print_size(common_section.size);
  print_space();
  printf("(common)");
  print_nl();
  /* Print out all the global symbols */


  for (lgs = symbol_head; lgs != (ldsym_type *)NULL; lgs =
       lgs->next) {
    if (lgs->sdefs_chain) {
      asymbol *def = *(lgs->sdefs_chain);
      if (def->section == &common_section) {
	print_symbol(def);
      }
    }

  }
  print_dot = common_section.output_offset +
    common_section.output_section->vma + common_section.size;


}
static void
print_fill_statement(fill)
lang_fill_statement_type *fill;
{
  printf("FILL mask ");
  print_fill( fill->fill);
}

static void
print_data_statement(data)
lang_data_statement_type *data;
{
/*  bfd_vma value; */
  print_section("");
  print_space();
  print_section("");
  print_space();
  ASSERT(print_dot == data->output_vma);

  print_address(data->output_vma);
  print_space();
  print_address(data->value);
  print_space();
  switch (data->type) {
  case BYTE :
    printf("BYTE ");
    print_dot += BYTE_SIZE;
    break;
  case SHORT:
    printf("SHORT ");
    print_dot += SHORT_SIZE;
    break;  
  case LONG:
    printf("LONG ");
    print_dot += LONG_SIZE;
    break;
  }

  exp_print_tree(stdout, data->exp);
  
  printf("\n");
}


static void
print_padding_statement(s)
lang_padding_statement_type *s;
{
  print_section("");
  print_space();
  print_section("*fill*");
  print_space();
  print_address(s->output_offset + s->output_section->vma);
  print_space();
  print_size(s->size);
  print_space();
  print_fill(s->fill);
  print_nl();
}

static void print_wild_statement(w,os)
lang_wild_statement_type *w;
lang_output_section_statement_type *os;
{
  if (w->filename != (char *)NULL) {
    printf("%s",w->filename);
  }
  else {
    printf("*");
  }
  if (w->section_name != (char *)NULL) {
    printf("(%s)",w->section_name);
  }
  else {
    printf("(*)");
  }
  print_nl();
  print_statement(w->children.head, os);

}
static void
print_statement(s, os)
lang_statement_union_type *s;
lang_output_section_statement_type *os;
{
  while (s) {
    switch (s->header.type) {
    case lang_wild_statement_enum:
     print_wild_statement(&s->wild_statement, os);
     break;
    default:
     printf("Fail with %d\n",s->header.type);
      FAIL();
      break;
   case lang_address_statement_enum:
     printf("address\n");
     break;
    case lang_common_statement_enum:
      print_common_statement();
      break;
    case lang_object_symbols_statement_enum:
      printf("object symbols\n");
      break;
    case lang_fill_statement_enum:
      print_fill_statement(&s->fill_statement);
      break;
    case lang_data_statement_enum:
      print_data_statement(&s->data_statement);
      break;


    case lang_input_section_enum:
      print_input_section(&s->input_section);
      break;
    case lang_padding_statement_enum:
      print_padding_statement(&s->padding_statement);
      break;
    case lang_output_section_statement_enum:
      print_output_section_statement(&s->output_section_statement);
      break;
    case lang_assignment_statement_enum:
      print_assignment(&s->assignment_statement,
		     os);
      break;


    case lang_target_statement_enum:
      printf("TARGET(%s)\n", s->target_statement.target);
      break;
    case lang_output_statement_enum:
      printf("OUTPUT(%s)\n", s->output_statement.name);
      break;
    case lang_input_statement_enum:
      print_input_statement(&s->input_statement);
      break;
    case lang_afile_asection_pair_statement_enum:
      FAIL();
      break;
    }
    s = s->next;
  }
}


static void
print_statements()
{
  print_statement(statement_list.head,
		  (lang_output_section_statement_type *)NULL);
}

static bfd_vma
insert_pad(this_ptr, fill, power, output_section_statement, dot)
lang_statement_union_type **this_ptr;
fill_type fill;
unsigned int power;
asection * output_section_statement;
bfd_vma dot;
{
  /* Align this section first to the 
     input sections requirement, then
     to the output section's requirement.
     If this alignment is > than any seen before, 
     then record it too. Perform the alignment by
     inserting a magic 'padding' statement.
     */

  unsigned int alignment_needed =  align_power(dot, power) - dot;

  if (alignment_needed != 0) 
    {
      lang_statement_union_type *new = 
	(lang_statement_union_type *)
	  ldmalloc(sizeof(lang_padding_statement_type));
      /* Link into existing chain */
      new->header.next = *this_ptr;
      *this_ptr = new;
      new->header.type = lang_padding_statement_enum;
      new->padding_statement.output_section = output_section_statement;
      new->padding_statement.output_offset =
	dot - output_section_statement->vma;
      new->padding_statement.fill = fill;
      new->padding_statement.size = alignment_needed;
    }


  /* Remember the most restrictive alignment */
  if (power > output_section_statement->alignment_power) {
    output_section_statement->alignment_power = power;
  }
  output_section_statement->size += alignment_needed;
  return alignment_needed + dot;

}

/*
 size_common runs run though each global symboxl, and works
 out how big the common section will be.
 */

static bfd_vma 
size_common(output_section_statement, this_ptr, dot)
lang_output_section_statement_type *output_section_statement;
lang_statement_union_type **this_ptr;
bfd_vma dot;
{
  extern ldsym_type *symbol_head;
  ldsym_type *sp;
  /* Make sure that each symbol is only defined once.
     Allocate common symbols
     Make the ref chain point to the defining asymbol.
     */
  /* Now, for each symbol, verify that it is defined globally at most once.
     Put the global value into the symbol entry.
     Common symbols are allocated here, in the BSS section.
     Each defined symbol is given a '->defined' field
     which is the correct N_ code for its definition,
     except in the case of common symbols with -r.
     Then make all the references point at the symbol entry
     instead of being chained together. */
  

  common_section.name = output_section_statement->bfd_section->name;
  common_section.output_section = output_section_statement->bfd_section;
  common_section.output_offset =
    dot - output_section_statement->bfd_section->vma;
  if (config.relocateable_output == false ||
      command_line.force_common_definition== true) {
    dot = insert_pad(this_ptr,
		     0x0, 4, output_section_statement->bfd_section, dot);

    for (sp = symbol_head; sp != (ldsym_type *)NULL; sp = sp->next)  
      {
	/* Attatch this symbol to the correct output section*/

	/* Allocate as common if wanted */

	if (sp->scoms_chain )

	  {
	    unsigned long com = (*(sp->scoms_chain))->value;
	    /* Work out what alignment this common item s
	       hould be put on. Anything < int is int aligned,
	       anything bigger is  self aligned,
	       up to the restriction of the machine */

	    unsigned int align = sizeof(int);

	    /* Round up size of object to nearest int */
	    com = ALIGN(com, sizeof(int));
	    /* See what alignment is necessary -*/
	    if (com) {		
	      while ((com & align)==0) align <<=1;
	      /* FIXME */
	      if (align > 8)  {
		align = 8;
	      }
	    }
	    dot =  ALIGN(dot, align);
		

	    /* Transmogrify this from a common symbol
	       into a definition of a symbol in common
	       */
	    sp->sdefs_chain = sp->scoms_chain;

	    {
	      asymbol *com_ptr = *(sp->sdefs_chain);

	      sp->scoms_chain = (asymbol **)NULL;
	      commons_pending--;
	      /* Assign address, but keep section relative */

	      /* Force the symbol to belong in the bss section */
	      com_ptr->flags = BSF_EXPORT | BSF_GLOBAL ;
	      com_ptr->section = &common_section;
	      common_section.size += com;
	      if (write_map) 
		{
		  printf ("Allocating common %s: %lx at %lx\n",
			  sp->name, 
			  com,
			  com_ptr->value);
		}
	      com_ptr->value = common_section.size;
	    }
	  }
      }
  }
  if (dot >
      (common_section.output_section->vma +
       common_section.output_section->size)) {
    common_section.output_section->size =
      dot - common_section.output_section->vma;
  }
  return dot + common_section.size;
}

static bfd_vma 
size_input_section( this_ptr, output_section_statement, fill,  dot)
lang_statement_union_type **this_ptr;
lang_output_section_statement_type*output_section_statement;
unsigned short fill;
bfd_vma dot;
{
  lang_input_section_type *is = &((*this_ptr)->input_section);
  asection *i = is->section;
	    
  dot =  insert_pad(this_ptr, fill, i->alignment_power,
		    output_section_statement->bfd_section, dot);

  /* remember the largest size so we can malloc the largest area */
  /* needed for the output stage */
  if (i->size > largest_section) {
    largest_section = i->size;
  }

  /* Remember where in the output section this input section goes */
  i->output_offset = dot - output_section_statement->bfd_section->vma;

  /* Mark how big the output section must be to contain this now */
  dot += i->size;
  output_section_statement->bfd_section->size =
    dot - output_section_statement->bfd_section->vma;


  return dot ;
}


/* Work out the size of the output sections 
 from the sizes of the input sections */
static bfd_vma
lang_size_sections(s, output_section_statement, prev, fill, dot)
lang_statement_union_type *s;
lang_output_section_statement_type * output_section_statement;
lang_statement_union_type **prev;
unsigned short fill;
bfd_vma dot;
{
  /* Size up the sections from their constituent parts */
  for (; s != (lang_statement_union_type *)NULL ; s = s->next) 
    {
      switch (s->header.type) {
      case lang_output_section_statement_enum:
	{
	  bfd_vma after;
	  lang_output_section_statement_type *os =
	    &(s->output_section_statement);
	  /* The start of a section */
	  
	  if (os->addr_tree == (etree_type *)NULL) {
	    /* No address specified for this section, get one
	       from the region specification
	       */
	      if (os->region == (lang_memory_region_type *)NULL) {
		  os->region = lang_memory_region_lookup("*default*");
	      }
	      dot = os->region->current;
	  }
	  else {
	    etree_value_type r ;
	    r = exp_fold_tree(os->addr_tree,
			      (lang_output_section_statement_type *)NULL,
			      lang_allocating_phase_enum,
			      dot, &dot);
	    if (r.valid == false) {
	      info("%F%S: non constant address expression for section %s\n",
		   os->name);
	    }
	    dot = r.value;
	  }
	  /* The section starts here */
	  /* First, align to what the section needs */

	  dot = align_power(dot, os->bfd_section->alignment_power);
	  os->bfd_section->vma = dot;
	  os->bfd_section->output_offset = 0;

	  (void)  lang_size_sections(os->children.head, os, &os->children.head,
				     os->fill, dot);
	  /* Ignore the size of the input sections, use the vma and size to */
	  /* align against */


	  after = ALIGN(os->bfd_section->vma +
			os->bfd_section->size,
			os->block_value) ;


	  os->bfd_section->size = after - os->bfd_section->vma;
	  dot = os->bfd_section->vma + os->bfd_section->size;
	  os->processed = true;

	  /* Replace into region ? */
	  if (os->addr_tree == (etree_type *)NULL 
	      && os->region !=(lang_memory_region_type*)NULL ) {
	    os->region->current = dot;
	  }
	}

	break;

      case lang_data_statement_enum: 
	{
	unsigned int size;
	s->data_statement.output_vma = dot;
	s->data_statement.output_section =
	  output_section_statement->bfd_section;

	switch (s->data_statement.type) {
	case LONG:
	  size = LONG_SIZE;
	  break;
	case SHORT:
	  size = SHORT_SIZE;
	  break;
	case BYTE:
	  size = BYTE_SIZE;
	  break;

	}
	dot += size;
	output_section_statement->bfd_section->size += size;
      }
	break;

      case lang_wild_statement_enum:

	dot =	lang_size_sections(s->wild_statement.children.head,
				   output_section_statement,
				   &s->wild_statement.children.head,

				   fill, dot);

	break;

      case lang_object_symbols_statement_enum:
	create_object_symbols = output_section_statement;
	break;
      case lang_output_statement_enum:

      case lang_target_statement_enum:
	break;
      case lang_common_statement_enum:
	dot = 	size_common(output_section_statement, prev, dot);

	break;

      case lang_input_section_enum:
	dot = 	size_input_section(prev,
				   output_section_statement,
				   output_section_statement->fill, dot);
	break;
      case lang_input_statement_enum:
	break;
      case lang_fill_statement_enum:
	fill = s->fill_statement.fill;
	break;
      case lang_assignment_statement_enum:
	{
	  bfd_vma newdot = dot;
	  exp_fold_tree(s->assignment_statement.exp,
			output_section_statement,
			lang_allocating_phase_enum,
			dot,
			&newdot);

	  if (newdot != dot) 
	    /* We've been moved ! so insert a pad */
	    {
	      lang_statement_union_type *new = 
		(lang_statement_union_type *)
		  ldmalloc(sizeof(lang_padding_statement_type));
	      /* Link into existing chain */
	      new->header.next = *prev;
	      *prev = new;
	      new->header.type = lang_padding_statement_enum;
	      new->padding_statement.output_section =
		output_section_statement->bfd_section;
	      new->padding_statement.output_offset =
		dot - output_section_statement->bfd_section->vma;
	      new->padding_statement.fill = fill;
	      new->padding_statement.size = newdot - dot;
	      output_section_statement->bfd_section->size +=
		new->padding_statement.size;
	      dot =   newdot;
	    }
	}

	break;
      case lang_padding_statement_enum:
	FAIL();
	break;
      default:
	FAIL();
	break;
      case lang_address_statement_enum:
	break;
      }
      prev = &s->header.next;      
    }
  return dot;
}


static bfd_vma
lang_do_assignments(s, output_section_statement, fill, dot)
lang_statement_union_type *s;
lang_output_section_statement_type * output_section_statement;
unsigned short fill;
bfd_vma dot;
{

  for (; s != (lang_statement_union_type *)NULL ; s = s->next) 
    {
      switch (s->header.type) {
      case lang_output_section_statement_enum:
	{
	  lang_output_section_statement_type *os =
	    &(s->output_section_statement);
	  dot = os->bfd_section->vma;
	  (void) lang_do_assignments(os->children.head, os, os->fill, dot);
	  dot = os->bfd_section->vma + os->bfd_section->size;
	}
	break;
      case lang_wild_statement_enum:

	dot = lang_do_assignments(s->wild_statement.children.head,
				    output_section_statement,
				    fill, dot);

	break;

      case lang_object_symbols_statement_enum:
      case lang_output_statement_enum:
      case lang_target_statement_enum:
      case lang_common_statement_enum:
	break;
      case lang_data_statement_enum:
	{
	  etree_value_type value ;
	  value =   exp_fold_tree(s->data_statement.exp,
				  0, lang_final_phase_enum, dot, &dot);
	  s->data_statement.value = value.value;
	  if (value.valid == false) info("%F%P: Invalid data statement\n");
	}
	switch (s->data_statement.type) {
	case LONG:
	  dot += LONG_SIZE;
	  break;
	case SHORT:
	  dot += SHORT_SIZE;
	  break;
	case BYTE:
	  dot += BYTE_SIZE;
	  break;
	}
	break;
      case lang_input_section_enum:
	{
	  asection *in =    s->input_section.section;
	  dot += in->size;
	}
	break;

      case lang_input_statement_enum:
	break;
      case lang_fill_statement_enum:
	fill = s->fill_statement.fill;
	break;
      case lang_assignment_statement_enum:
	{
	  exp_fold_tree(s->assignment_statement.exp,
			output_section_statement,
			lang_final_phase_enum,
			dot,
			&dot);
	}

	break;
      case lang_padding_statement_enum:
	dot += s->padding_statement.size;
	break;
      default:
	FAIL();
	break;
      case lang_address_statement_enum:
	break;
      }

    }
  return dot;
}



static void lang_relocate_globals() 
{ 

  /*
     Each ldsym_type maintains a chain of pointers to asymbols which
     references the definition.  Replace each pointer to the referenence
     with a pointer to only one place, preferably the definition. If
     the defintion isn't available then the common symbol, and if
     there isn't one of them then choose one reference.
     */

  FOR_EACH_LDSYM(lgs) {
    asymbol *it;
    if (lgs->sdefs_chain) {
      it = *(lgs->sdefs_chain);
    }
    else if (lgs->scoms_chain != (asymbol **)NULL) {
      it = *(lgs->scoms_chain);
    }
    else if (lgs->srefs_chain != (asymbol **)NULL) {
      it = *(lgs->srefs_chain);
    }
    else {
      FAIL();
    }
    if (it != (asymbol *)NULL)
      {
	asymbol **ptr= lgs->srefs_chain;

	while (ptr != (asymbol **)NULL) {
	  asymbol *ref = *ptr;
	  *ptr = it;
	  ptr = (asymbol **)(ref->udata);
	}
      }
  }
}



/* now that all the jiggery pokery is finished, copy important data from
 * out internal form to the bfd way. Also create a section
 * for each dummy file
 */

static void
lang_create_output_section_statements()
{
  lang_statement_union_type*os;
  for (os = lang_output_section_statement.head;
       os != (lang_statement_union_type*)NULL;
       os = os->output_section_statement.next) {
    lang_output_section_statement_type *s =
      &os->output_section_statement;
    init_os(s);
  }
  script_file->the_bfd->sections = output_bfd->sections;
}

static void
lang_finish()
{
  ldsym_type *lgs;

  if (entry_symbol == (char *)NULL) {
    /* No entry has been specified, look for start */
    entry_symbol = "start";
  }
  lgs = ldsym_get_soft(entry_symbol);
  if (lgs && lgs->sdefs_chain) {
    asymbol *sy = *(lgs->sdefs_chain);
    /* We can set the entry address*/
    bfd_set_start_address(output_bfd,
			  outside_symbol_address(sy));

  }
  else {
    /* Can't find anything reasonable, 
       use the first address in the text section 
       */
    asection *ts = bfd_get_section_by_name(output_bfd, ".text");
    if (ts) {
      bfd_set_start_address(output_bfd, ts->vma);
    }
  }
}

/* By now we know the target architecture, and we may have an */
/* ldfile_output_machine_name */
static void
lang_check()
{
  lang_statement_union_type *file;

  
  for (file = file_chain.head;
       file != (lang_statement_union_type *)NULL;
       file=file->input_statement.next) 
    {
      /* Inspect the architecture and ensure we're linking like
	 with like
	 */

      if (bfd_arch_compatible( file->input_statement.the_bfd,
			      output_bfd,
			      &ldfile_output_architecture,
			      &ldfile_output_machine)) {
	bfd_set_arch_mach(output_bfd,
			  ldfile_output_architecture, ldfile_output_machine);
      }
      else {
	enum bfd_architecture this_architecture =
	  bfd_get_architecture(file->input_statement.the_bfd);
	unsigned long this_machine =
	  bfd_get_machine(file->input_statement.the_bfd);
	
	info("%I: architecture %s",
	     file,
	     bfd_printable_arch_mach(this_architecture, this_machine));
	info(" incompatible with output %s\n",
	     bfd_printable_arch_mach(ldfile_output_architecture,
				     ldfile_output_machine));
	ldfile_output_architecture = this_architecture;
	ldfile_output_machine = this_machine;
	bfd_set_arch_mach(output_bfd,
			  ldfile_output_architecture,
			  ldfile_output_machine);


      }
    }
}


/*
 * run through all the global common symbols and tie them 
 * to the output section requested.
 */

static void
lang_common()
{
  ldsym_type *lgs;
  if (config.relocateable_output == false ||
      command_line.force_common_definition== true) {
    for (lgs = symbol_head;
	 lgs != (ldsym_type *)NULL;
	 lgs=lgs->next)
      {
	asymbol *com ;
	size_t size;
	size_t align;
	if (lgs->scoms_chain != (asymbol **)NULL) {

	  com = *(lgs->scoms_chain);
	  size = com->value;
	  align = sizeof(int);
	  /* Round up size of object to nearest int */
	  size = ALIGN(size, sizeof(int));
	  /* Force alignment */
	  if (size) {
	    while ((size & align)==0) align<<=1;
	    if (align > 8) {
	      align = 8;
	    }
	  }
	  /* Change from a common symbol into a definition of
	     a symbol */
	  lgs->sdefs_chain = lgs->scoms_chain;
	  lgs->scoms_chain = (asymbol **)NULL;
	  commons_pending--;
	  /* Point to the correct common section */
	  com->section =
	    ((lang_input_statement_type *)
	     (com->the_bfd->usrdata))->common_section;
	  /*  Fix the size of the common section */


	  com->flags = BSF_EXPORT | BSF_GLOBAL;

	  if (write_map) 
	    {
	      printf ("Allocating common %s: %x at %x\n",
		      lgs->name, 
		      (unsigned) size,
		      (unsigned)  com->section->size);
	    }
	  com->value = com->section->size;
	  com->section->size += size;
	}
      }
  }
}

/*
run through the input files and ensure that every input 
section has somewhere to go. If one is found without
a destination then create an input request and place it
into the statement tree.
*/

static void lang_place_orphans()
{
  lang_input_statement_type *file;
  for (file = (lang_input_statement_type*)file_chain.head;
       file != (lang_input_statement_type*)NULL;
       file = (lang_input_statement_type*)file->next) {
    asection *s;  
    for (s = file->the_bfd->sections;
	 s != (asection *)NULL;
	 s = s->next) {
      if ( s->output_section == (asection *)NULL) {
	/* This section of the file is not attatched, root
	   around for a sensible place for it to go */

	if (file->common_section == s) {
	  /* This is a lonely common section which must
	     have come from an archive. We attatch to the
	     section with the wildcard  */
	  wild_doit(&default_common_section->children, s, 
		    default_common_section, file);	  
	}
	else {
	  lang_output_section_statement_type *os = 
	    lang_output_section_statement_lookup(s->name);

	  wild_doit(&os->children, s, os, file);
	}
      }
    }

  }
}


/*
 * phase_2
 *
 * peformed after every file has been opened and symbols read
 */
static void
lang_phase_2()
{
  lang_init2();
 
  lang_create_output_section_statements();
  lang_open_input(statement_list.head, (char *)NULL, 
		  ( lang_output_section_statement_type *)NULL);
  lang_place_orphans();
  lang_common();

  ldemul_before_allocation();

  lang_size_sections(statement_list.head,
		     (lang_output_section_statement_type *)NULL,
		     &(statement_list.head), 0, (bfd_vma)0);
  ldemul_after_allocation();
  /* Do it once again now that we know the sizes of everything */

  lang_do_assignments(statement_list.head,
		      (lang_output_section_statement_type *)NULL,
		      0, (bfd_vma)0);



  lang_check();

  lang_relocate_globals();


  lang_finish();
}




void
lang_set_flags(ptr, flags)
lang_section_flags_type *ptr;
char *flags;
{
  boolean state = true;
  ptr->flag_read = false;
  ptr->flag_write = false;
  ptr->flag_executable = false;
  ptr->flag_loadable= false;
  while (*flags)
    {
      if (*flags == '!') {
	state = false;
	flags++;
      }
      else state = true;
      switch (*flags) {
      case 'R':
	ptr->flag_read = state; 
	break;
      case 'W':
	ptr->flag_write = state; 
	break;
      case 'X':
	ptr->flag_executable= state;
	break;
      case 'L':
	ptr->flag_loadable= state;
	break;
      default:
	info("%P%F illegal syntax in flags\n");
	break;
      }
      flags++;
    }
}



void
lang_for_each_file(func)
void (*func)();
{
  lang_input_statement_type *f;
  for (f = (lang_input_statement_type *)file_chain.head; 
       f != (lang_input_statement_type *)NULL;
       f = (lang_input_statement_type *)f->next)  
    {
      func(f);
    }
}


void
lang_for_each_input_section(func)
void (*func)();
{
  lang_input_statement_type *f;
  for (f = (lang_input_statement_type *)file_chain.head; 
       f != (lang_input_statement_type *)NULL;
       f = (lang_input_statement_type *)f->next)  
    {
      asection *s;
      for (s = f->the_bfd->sections;
	   s != (asection *)NULL;
	   s = s->next) {
	func(f->the_bfd, s);
      }
    }
}



void 
ldlang_add_file(entry)
lang_input_statement_type *entry;
{
  lang_has_input_file = true;
  lang_statement_append(&file_chain,
			(lang_statement_union_type *)entry,
			&entry->next);
}



void
lang_add_output(name)
char *name;
{
  lang_output_statement_type *new = new_stat(lang_output_statement,
					     stat_ptr);
  new->name = name;
  had_output_filename = true;
}


static lang_output_section_statement_type *current_section;

void
lang_enter_output_section_statement(output_section_statement_name,
address_exp,
block_value)
char *output_section_statement_name;
etree_type *address_exp;
bfd_vma block_value;
{
  lang_output_section_statement_type *os;
  current_section = 
    os =
      lang_output_section_statement_lookup(output_section_statement_name);


  /* Add this statement to tree */
  /*  add_statement(lang_output_section_statement_enum,
      output_section_statement);*/
  /* Make next things chain into subchain of this */

  if (os->addr_tree ==
      (etree_type *)NULL) {
    os->addr_tree =
      address_exp;
  }
  os->block_value = block_value;
  stat_ptr = & os->children;

}


void 
lang_final()
{
  if (had_output_filename == false) {
    lang_add_output("a.out");
  }


}





asymbol *create_symbol(name, flags, section)
char *name;
flagword flags;
asection *section;
{
  extern lang_input_statement_type *script_file;
  asymbol **def_ptr = (asymbol **)ldmalloc(sizeof(asymbol **));
  /* Add this definition to script file */
  asymbol *def =  (asymbol  *)bfd_make_empty_symbol(script_file->the_bfd);
  def->name = name;
  def->udata = 0;
  def->flags = flags;
  def->section = section;

  *def_ptr = def;
  Q_enter_global_ref(def_ptr);
  return def;
}


void
lang_process()
{
  lang();
  lang_phase_2();
}


/* EXPORTED TO YACC */
void
lang_section_start(name, address)
char *name;
etree_type *address;
{
  lang_address_statement_type *ad =new_stat(lang_address_statement, stat_ptr);
  ad->section_name = name;
  ad->address = address;
}
void lang_add_entry(name)
char *name;
{
  entry_symbol = name;
}

void
lang_add_target(name)
char *name;
{
  lang_target_statement_type *new = new_stat(lang_target_statement,
					    stat_ptr);
  new->target = name;

}
void
lang_add_wild(section_name, filename)
char *section_name;
char *filename;
{
  lang_wild_statement_type *new = new_stat(lang_wild_statement,
					   stat_ptr);

  if (section_name != (char *)NULL && strcmp(section_name,"COMMON") == 0)
    {
      placed_commons = true;
    }
  new->section_name = section_name;
  new->filename = filename;
  lang_list_init(&new->children);
}

void
lang_add_map(name)
char *name;
{
  while (*name) {
    switch (*name) {
    case 'F':
      map_option_f = true;
      break;
    }
    name++;
  }
}

void lang_add_fill(exp)
int exp;
{
  lang_fill_statement_type *new =   new_stat(lang_fill_statement, 
					      stat_ptr);
  new->fill = exp;
}

void lang_add_data(type, exp)
int type;
union etree_union *exp;
{

 lang_data_statement_type *new = new_stat(lang_data_statement,
					    stat_ptr);
 new->exp = exp;
 new->type = type;

}
void
lang_add_assignment(exp)
etree_type *exp;
{
  lang_assignment_statement_type *new = new_stat(lang_assignment_statement,
					    stat_ptr);
  new->exp = exp;
}

void
lang_add_attribute(attribute)
enum statement_enum attribute;
{
  new_statement(attribute, sizeof(lang_statement_union_type),stat_ptr);
}



void 
lang_startup(name)
char *name;
{
  if (startup_file != (char *)NULL) {
   info("%P%FMultiple STARTUP files\n");
  }
  first_file->filename = name;
  first_file->local_sym_name = name;

  startup_file= name;
}
void 
lang_float(maybe)
boolean maybe;
{
  lang_float_flag = maybe;
}

void 
lang_leave_output_section_statement(fill, memspec)
bfd_vma fill;
char *memspec;
{
  current_section->fill = fill;
  current_section->region = lang_memory_region_lookup(memspec);
  stat_ptr = &statement_list;
}
/*
 Create an absolute symbol with the given name with the value of the
 address of first byte of the section named.

 If the symbol already exists, then do nothing.
*/
void
lang_abs_symbol_at_beginning_of(section, name)
char *section;
char *name;
{
  if (ldsym_undefined(name)) {
    extern bfd *output_bfd;
    extern asymbol *create_symbol();
    asection *s = bfd_get_section_by_name(output_bfd, section);
    asymbol *def = create_symbol(name,
				 BSF_GLOBAL | BSF_EXPORT |
				 BSF_ABSOLUTE,
				 (asection *)NULL);
    if (s != (asection *)NULL) {
      def->value = s->vma;
    }
    else {
      def->value = 0;
    }
  }
}

/*
 Create an absolute symbol with the given name with the value of the
 address of the first byte after the end of the section named.

 If the symbol already exists, then do nothing.
*/
void
lang_abs_symbol_at_end_of(section, name)
char *section;
char *name;
{
  if (ldsym_undefined(name)){
    extern bfd *output_bfd;
    extern asymbol *create_symbol();
    asection *s = bfd_get_section_by_name(output_bfd, section);
    /* Add a symbol called _end */
    asymbol *def = create_symbol(name,
				 BSF_GLOBAL | BSF_EXPORT |
				 BSF_ABSOLUTE,
				 (asection *)NULL);
    if (s != (asection *)NULL) {
      def->value = s->vma + s->size;
    }
    else {
      def->value = 0;
    }
  }
}

void 
lang_statement_append(list, element, field)
lang_statement_list_type *list;
lang_statement_union_type *element;
lang_statement_union_type **field;
{
  *(list->tail) = element;
  list->tail = field;
}


static void 
lang_for_each_statement_worker(func,  s)
void (*func)();
lang_statement_union_type *s;
{
  for (; s != (lang_statement_union_type *)NULL ; s = s->next) 
    {
      func(s);

      switch (s->header.type) {
      case lang_output_section_statement_enum:
	lang_for_each_statement_worker
	  (func, 
	   s->output_section_statement.children.head);
	break;
      case lang_wild_statement_enum:
	lang_for_each_statement_worker
	  (func, 
	   s->wild_statement.children.head);
	break;
      case lang_data_statement_enum:
      case lang_object_symbols_statement_enum:
      case lang_output_statement_enum:
      case lang_target_statement_enum:
      case lang_common_statement_enum:
      case lang_input_section_enum:
      case lang_input_statement_enum:
      case lang_fill_statement_enum:
      case lang_assignment_statement_enum:
      case lang_padding_statement_enum:
      case lang_address_statement_enum:
	break;
      default:
	FAIL();
	break;
      }
    }
}

void lang_for_each_statement(func)
void (*func)();
{
  lang_for_each_statement_worker(func,
				 statement_list.head);

}
