/* ldlang.h -

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


typedef enum {
  lang_input_file_is_l_enum,
  lang_input_file_is_symbols_only_enum,
  lang_input_file_is_marker_enum,
  lang_input_file_is_fake_enum,
lang_input_file_is_search_file_enum,
  lang_input_file_is_file_enum } lang_input_file_enum_type;

typedef unsigned short fill_type;
typedef struct statement_list {
  union lang_statement_union *head;
  union lang_statement_union **tail;
} lang_statement_list_type;


typedef struct {
  boolean flag_read;
  boolean flag_write;
  boolean flag_executable;
  boolean flag_loadable;
} lang_section_flags_type;

typedef struct memory_region_struct {
  char *name;
  struct memory_region_struct *next;
  bfd_vma origin;
  bfd_offset length;
  bfd_vma current;
  lang_section_flags_type flags;
} lang_memory_region_type ;

typedef struct lang_statement_header_struct 
{
union  lang_statement_union  *next;
  enum statement_enum {
    lang_output_section_statement_enum,
    lang_assignment_statement_enum,
    lang_input_statement_enum,
    lang_address_statement_enum,
    lang_wild_statement_enum,
    lang_input_section_enum,
    lang_object_symbols_statement_enum,
    lang_fill_statement_enum,
    lang_data_statement_enum,
    lang_target_statement_enum,
    lang_output_statement_enum,
    lang_padding_statement_enum,

    lang_afile_asection_pair_statement_enum
  } type;

} lang_statement_header_type;


typedef struct 
{
  lang_statement_header_type header;
  union etree_union *exp;
}  lang_assignment_statement_type;


typedef struct lang_target_statement_struct {
  lang_statement_header_type header;
  CONST char *target;
} lang_target_statement_type;


typedef struct lang_output_statement_struct {
  lang_statement_header_type header;
  CONST char *name;
} lang_output_statement_type;


typedef struct lang_output_section_statement_struct 
{
  lang_statement_header_type header;
  union etree_union *addr_tree;
  lang_statement_list_type children;
  CONST char *memspec;
  union lang_statement_union *next;
  CONST char *name;
  unsigned long subsection_alignment;
  boolean processed;

  asection *bfd_section;
  lang_section_flags_type flags;
  struct memory_region_struct *region;
  size_t block_value;
  fill_type fill;
} lang_output_section_statement_type;


typedef struct {
  lang_statement_header_type header;
} lang_common_statement_type;

typedef struct {
  lang_statement_header_type header;
} lang_object_symbols_statement_type;

typedef struct {
  lang_statement_header_type header;
  fill_type fill;
} lang_fill_statement_type;

typedef struct {
  lang_statement_header_type header;
  unsigned int type;
  union  etree_union *exp;
  bfd_vma value;
  asection *output_section;
  bfd_vma output_vma;
} lang_data_statement_type;




typedef struct lang_input_statement_struct
  {
    lang_statement_header_type header;
    /* Name of this file.  */
    CONST char *filename;
    /* Name to use for the symbol giving address of text start */
    /* Usually the same as filename, but for a file spec'd with -l
       this is the -l switch itself rather than the filename.  */
    CONST char *local_sym_name;

    /* Describe the layout of the contents of the file */

    /* The file's a.out header.  */
    /*  struct exec header;*/
    /* Offset in file of GDB symbol segment, or 0 if there is none.  */
    int symseg_offset;

    /* Describe data from the file loaded into core */

    bfd *the_bfd;

    boolean closed;
    file_ptr passive_position;

    /* Symbol table of the file.  */
    asymbol **asymbols;
    unsigned int symbol_count;

    /* For library members only */

    /* For a library, points to chain of entries for the library members.  */
    struct lang_input_statement_struct *subfiles;
    /* For a library member, offset of the member within the archive.
       Zero for files that are not library members.  */
    /*  int starting_offset;*/
    /* Size of contents of this file, if library member.  */
    int total_size;
    /* For library member, points to the library's own entry.  */
    struct lang_input_statement_struct *superfile;
    /* For library member, points to next entry for next member.  */
    struct lang_input_statement_struct *chain;
    /* Point to the next file - whatever it is, wanders up and down
       archives */

    union lang_statement_union  *next;
    /* Point to the next file, but skips archive contents */
    union  lang_statement_union  *next_real_file;

    boolean is_archive;

    /* 1 means search a set of directories for this file.  */
    boolean search_dirs_flag;

    /* 1 means this is base file of incremental load.
       Do not load this file's text or data.
       Also default text_start to after this file's bss. */

    boolean just_syms_flag;

    boolean loaded;


    /*    unsigned int globals_in_this_file;*/
    CONST char *target;
    boolean real;

    asection *common_section;
    asection *common_output_section;
  } lang_input_statement_type;

typedef struct {
  lang_statement_header_type header;
  asection *section;
  lang_input_statement_type *ifile;

} lang_input_section_type;


typedef struct {
  lang_statement_header_type header;
  asection *section;
  union lang_statement_union *file;
} lang_afile_asection_pair_statement_type;

typedef struct lang_wild_statement_struct {
  lang_statement_header_type header;
CONST char *section_name;
CONST char *filename;
  lang_statement_list_type children;
} lang_wild_statement_type;

typedef struct lang_address_statement_struct {
  lang_statement_header_type header;
  CONST  char *section_name;
  union  etree_union *address;
} lang_address_statement_type;

typedef struct {
  lang_statement_header_type header;
  bfd_vma output_offset;
  size_t size;
  asection *output_section;
  fill_type fill;
} lang_padding_statement_type;

typedef union lang_statement_union 
{
  lang_statement_header_type header;
  union lang_statement_union *next;
  lang_wild_statement_type wild_statement;
  lang_data_statement_type data_statement;
  lang_address_statement_type address_statement;
  lang_output_section_statement_type output_section_statement;
  lang_afile_asection_pair_statement_type afile_asection_pair_statement;
  lang_assignment_statement_type assignment_statement;
  lang_input_statement_type input_statement;
  lang_target_statement_type target_statement;
  lang_output_statement_type output_statement;
  lang_input_section_type input_section;
  lang_common_statement_type common_statement;
  lang_object_symbols_statement_type object_symbols_statement;
  lang_fill_statement_type fill_statement;
  lang_padding_statement_type padding_statement;
} lang_statement_union_type;



PROTO(void,lang_init,(void));
PROTO(struct memory_region_struct ,
      *lang_memory_region_lookup,(CONST
				  char *CONST));


PROTO(void ,lang_map,(FILE *));
PROTO(void,lang_set_flags,(lang_section_flags_type *, CONST char *));
PROTO(void,lang_add_output,(CONST char *));

PROTO(void,lang_final,(void));
PROTO(struct symbol_cache_entry *,create_symbol,(CONST char *, unsigned int, struct sec *));
PROTO(void ,lang_process,(void));
PROTO(void ,lang_section_start,(CONST char *, union etree_union *));
PROTO(void,lang_add_entry,(CONST char *));
PROTO(void,lang_add_target,(CONST char *));
PROTO(void,lang_add_wild,(CONST char *CONST , CONST char *CONST));
PROTO(void,lang_add_map,(CONST char *));
PROTO(void,lang_add_fill,(int));
PROTO(void,lang_add_assignment,(union etree_union *));
PROTO(void,lang_add_attribute,(enum statement_enum));
PROTO(void,lang_startup,(CONST char *));
PROTO(void,lang_float,(enum boolean));
PROTO(void,lang_leave_output_section_statement,(bfd_vma, CONST char *));
PROTO(void,lang_abs_symbol_at_end_of,(CONST char *, CONST char *));
PROTO(void,lang_abs_symbol_at_beginning_of,(CONST char *, CONST char *));
PROTO(void,lang_statement_append,(struct statement_list *, union lang_statement_union *, union lang_statement_union **));
PROTO(void, lang_for_each_file,(void (*dothis)(lang_input_statement_type *)));

     
#define LANG_FOR_EACH_INPUT_STATEMENT(statement)               \
  extern lang_statement_list_type file_chain;			\
  lang_input_statement_type *statement;				\
  for (statement = (lang_input_statement_type *)file_chain.head;\
       statement != (lang_input_statement_type *)NULL;		\
       statement = (lang_input_statement_type *)statement->next)\

#define LANG_FOR_EACH_INPUT_SECTION(statement, abfd, section, x) \
{ extern lang_statement_list_type file_chain;			\
  lang_input_statement_type *statement;				\
  for (statement = (lang_input_statement_type *)file_chain.head;\
       statement != (lang_input_statement_type *)NULL;		\
       statement = (lang_input_statement_type *)statement->next)\
    {								\
      asection *section;					\
      bfd *abfd = statement->the_bfd;				\
      for (section = abfd->sections;				\
	   section != (asection *)NULL;				\
	   section = section->next) {				\
	x;							\
      }								\
    }								\
 }		

#define LANG_FOR_EACH_OUTPUT_SECTION(section, x)		\
 { extern bfd *output_bfd;					\
   asection *section;      					\
   for (section = output_bfd->sections;				\
	section != (asection *)NULL;				\
	section = section->next)				\
	{ x; }							\
 }
					

PROTO(void, lang_process,(void));
PROTO(void, ldlang_add_file,(lang_input_statement_type *));

PROTO(lang_output_section_statement_type
      *,lang_output_section_find,(CONST char * CONST));

PROTO(lang_input_statement_type *,
       lang_add_input_file,(char *name,
			    lang_input_file_enum_type file_type,
			    char *target));
PROTO(lang_output_section_statement_type *,
lang_output_section_statement_lookup,(CONST char * CONST name));

PROTO(void, ldlang_add_undef,(CONST char *CONST name));
PROTO(void, lang_add_output_format,(CONST char *));
