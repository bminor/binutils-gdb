/* ldexp.h -

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




/* The result of an expression tree */
typedef struct 
{
  bfd_vma value;
  struct lang_output_section_statement_struct *section;
  boolean valid;
} etree_value_type;



typedef struct 
{
  int node_code;
  enum { etree_binary,
	   etree_trinary,
	   etree_unary,
	   etree_name,
	   etree_assign,
	   etree_undef,
	   etree_unspec,
	   etree_value } node_class;
} node_type;



typedef union etree_union 
{
  node_type type;
  struct {
    node_type type;
    union etree_union *lhs;
    union etree_union *rhs;
  } binary;
  struct {
    node_type type;
    union etree_union *cond;
    union etree_union *lhs;
    union etree_union *rhs;
  } trinary;
  struct {
    node_type type;
    char *dst;
    union etree_union *src;
  } assign;

  struct {
    node_type type;
    union   etree_union *child;
  } unary;
  struct {
    node_type type;
    char *name;
  } name;
  struct {
    node_type type;
    bfd_vma value;
  } value;

} etree_type;


PROTO(etree_type *,exp_intop,(bfd_vma));

PROTO(etree_value_type, invalid,(void));
PROTO(etree_value_type, exp_fold_tree,(etree_type *, struct
				    lang_output_section_statement_struct *, lang_phase_type,
					bfd_vma, bfd_vma *));

PROTO(etree_type *, exp_binop,(int, etree_type *, etree_type *));
PROTO(etree_type *,exp_trinop,(int,etree_type *, etree_type *, etree_type *));
PROTO(etree_type *,exp_unop,(int, etree_type *));
PROTO(etree_type *,exp_nameop,(int, char *));
PROTO(etree_type *,exp_assop,(int, char *, etree_type *));
PROTO(void, exp_print_tree,(struct _iobuf *, etree_type *));
PROTO(bfd_vma, exp_get_vma,(etree_type *, bfd_vma, char *, enum boolean));
PROTO(int, exp_get_value_int,(etree_type *, int, char *, enum boolean));
#if 0
#define LONG 1000
#define SHORT 1001
#define BYTE 1002
#define NAME 1003
#define NEXT 1004
#define DEFINED 1005
#define SIZEOF 1006
#define ADDR 1007
#define ALIGN_K 1008
#define BLOCK 1009
#define SECTIONS 1010
#define SIZEOF_HEADERS 1011
#define MEMORY 1012
#define DSECT 1013
#define COPY 1014
#define INFO 1015
#define OVERLAY 1016
#define TARGET_K 1017
#define MAP 1018
#define ENTRY 1019
#define ORIGIN 1020
#define LENGTH 1021
#define NOLOAD 1022
#define SEARCH_DIR 1023
#define ALIGNMENT 1024

#endif
