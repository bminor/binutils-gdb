/* ld.h -

   Copyright (C) 1991, 1993 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#define flag_is_not_at_end(x) ((x) & BSF_NOT_AT_END)
#define flag_is_ordinary_local(x) (((x) & (BSF_LOCAL))&!((x) & (BSF_DEBUGGING)))
#define flag_is_debugger(x) ((x) & BSF_DEBUGGING)
#define flag_is_undefined_or_global(x) ((x) & (BSF_UNDEFINED | BSF_GLOBAL))
#define flag_is_defined(x) (!((x) & (BSF_UNDEFINED)))
#define flag_is_global_or_common(x) ((x) & (BSF_GLOBAL | BSF_FORT_COMM))
#define flag_is_undefined_or_global_or_common(x) ((x) & (BSF_UNDEFINED | BSF_GLOBAL | BSF_FORT_COMM))
#define flag_is_undefined_or_global_or_common_or_constructor(x) ((x) & (BSF_UNDEFINED | BSF_GLOBAL | BSF_FORT_COMM | BSF_CONSTRUCTOR))
#define flag_is_constructor(x) ((x) & BSF_CONSTRUCTOR)
#define flag_is_common(x) ((x) & BSF_FORT_COMM)
#define flag_is_global(x) ((x) & (BSF_GLOBAL))
#define flag_is_undefined(x) ((x) & BSF_UNDEFINED)
#define flag_set(x,y) (x = y)
#define flag_is_fort_comm(x) ((x) & BSF_FORT_COMM)
#define flag_is_absolute(x) ((x) & BSF_ABSOLUTE)
/* Extra information we hold on sections */
typedef struct  user_section_struct {
  /* Pointer to the section where this data will go */
  struct lang_input_statement_struct *file;
} section_userdata_type;


#define get_userdata(x) ((x)->userdata)
#define as_output_section_statement(x) ((x)->otheruserdata)



/* Which symbols should be stripped (omitted from the output):
   none, all, or debugger symbols.  */
typedef  enum { STRIP_NONE, STRIP_ALL, STRIP_DEBUGGER, STRIP_SOME } strip_symbols_type;




/* Which local symbols should be omitted:
   none, all, or those starting with L.
   This is irrelevant if STRIP_NONE.  */
typedef  enum { DISCARD_NONE, DISCARD_ALL, DISCARD_L } discard_locals_type;


#define BYTE_SIZE	(1)
#define SHORT_SIZE	(2)
#define LONG_SIZE	(4)

/* ALIGN macro changed to ALIGN_N to avoid	*/
/* conflict in /usr/include/machine/machparam.h */
#define ALIGN_N(this, boundary)  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))

typedef struct {
  /* 1 => assign space to common symbols even if `relocatable_output'.  */
  boolean force_common_definition;
  boolean relax;
  
} args_type;

typedef int token_code_type;

typedef struct 
{
  bfd_size_type specified_data_size;
  boolean magic_demand_paged;
  boolean make_executable;
  /* 1 => write relocation into output file so can re-input it later.  */
  boolean relocateable_output;

  /* Will we build contstructors, or leave alone ? */
  boolean build_constructors;

  /* If true, warn about merging common symbols with others.  */
  boolean warn_common;

  boolean sort_common;
/* these flags may seem mutually exclusive, but not setting them
   allows the back end to decide what would be the best thing to do */
  boolean text_read_only;

   char *map_filename;
   FILE *map_file;

} ld_config_type;
#define set_asymbol_chain(x,y) ((x)->udata = (PTR)y)
#define get_asymbol_chain(x) ((asymbol **)((x)->udata))
#define get_loader_symbol(x) ((loader_global_asymbol *)((x)->udata))
#define set_loader_symbol(x,y) ((x)->udata = (PTR)y)






typedef enum {
  lang_first_phase_enum,
  lang_allocating_phase_enum,
  lang_final_phase_enum } lang_phase_type;





int yyparse();
#define BYTE_SIZE 1
#define SHORT_SIZE 2
#define LONG_SIZE 4
