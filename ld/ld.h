/* ld.h -

   Copyright (C) 1991 Free Software Foundation, Inc.

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


#define flag_is_not_at_end(x) ((x) & BSF_NOT_AT_END)
#define flag_is_ordinary_local(x) (((x) & (BSF_LOCAL))&!((x) & (BSF_DEBUGGING)))
#define flag_is_debugger(x) ((x) & BSF_DEBUGGING)
#define flag_is_undefined_or_global(x) ((x) & (BSF_UNDEFINED | BSF_GLOBAL))
#define flag_is_defined(x) (!((x) & (BSF_UNDEFINED)))
#define flag_is_global_or_common(x) ((x) & (BSF_GLOBAL | BSF_FORT_COMM))
#define flag_is_undefined_or_global_or_common(x) ((x) & (BSF_UNDEFINED | BSF_GLOBAL | BSF_FORT_COMM))
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
enum { STRIP_NONE, STRIP_ALL, STRIP_DEBUGGER } strip_symbols;




/* Which local symbols should be omitted:
   none, all, or those starting with L.
   This is irrelevant if STRIP_NONE.  */
enum { DISCARD_NONE, DISCARD_ALL, DISCARD_L } discard_locals;






#define ALIGN(this, boundary)  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))

typedef struct {
  /* 1 => assign space to common symbols even if `relocatable_output'.  */
  boolean force_common_definition;

} args_type;

typedef int token_code_type;

typedef struct 
{
  unsigned int specified_data_size;
  boolean magic_demand_paged;
  boolean make_executable;
  /* 1 => write relocation into output file so can re-input it later.  */
  boolean relocateable_output;

  /* Will we build contstructors, or leave alone ? */
  boolean build_constructors;
  /* 1 => write relocation such that a UNIX linker can understand it.
     This is used mainly to finish of sets that were built.  */
  boolean unix_relocate;


} ld_config_type;
#define set_asymbol_chain(x,y) ((x)->udata = (PTR)y)
#define get_asymbol_chain(x) ((asymbol **)((x)->udata))
#define get_loader_symbol(x) ((loader_global_asymbol *)((x)->udata))
#define set_loader_symbol(x,y) ((x)->udata = (PTR)y)






typedef enum {
  lang_first_phase_enum,
  lang_allocating_phase_enum,
  lang_final_phase_enum } lang_phase_type;




