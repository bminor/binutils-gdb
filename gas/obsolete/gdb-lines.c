/* gdb-lines.c -- Deal with source lines for GDB format
   Copyright (C) 1989, Free Software Foundation.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
   
#include "as.h"
#include "obstack.h"
#include "frags.h"

/* This is a souce file that we're storing .gdbline information about */
/* .gdbline refers to files by numbers.  We keep a linked list of them
   We store a list of vectors for each file.  Each element of the vector
   contains a line-number, a frag, and an offset within the frag. */
struct g_line_file {
	int	gdb_line_file_file_number;		/* fnum */
	int	gdb_line_file_number_of_vectors;	/* nv */
	long	gdb_line_table_offset;			/* taboff */
	struct g_line_vector *gdb_line_file_vectors;	/* vec */
	struct g_line_file *gdb_line_file_next_file;	/* nfile */
};

/* In order to save on space (We expect there to be LOTS of lines), we
   store line-number/address pairs in bunches of MAX_LINES_PER_VECTOR
   (originally fifty).  Each vector descriptor contains

   gdb_line_number_of_lines	the number of line-number/address pairs
   				actually in this vector.
   gdb_line_lines		The actual vector.

   gdb_line_next_vector		The next vector descriptor in the linked list.
 */
struct g_line_vector {
	int	gdb_line_number_of_lines;		/* nlines */
	struct g_line *gdb_line_lines;			/* lines */
	struct g_line_vector *gdb_line_next_vector;	/* nvec */
};


/* A .gdbline wants to store a line-number/address pair.  Unfortunatly, we
   don't know addresses yet, so we store frag/offset which we can use to
   generate the address at write-out time. */
struct g_line {
	int	gdb_line_line_number;			/* lno */
	fragS	*gdb_line_frag;				/* lfrag */
	int	gdb_line_offset;			/* loff */
};


/* The following is stolen from (gdb's? (or is it gcc's?) symseg.h file.
   These structures describe the format for the line# symbolic info in
   the gdb symbolic info file.  This info is not particularly useful,
   except to show what we're writing into. . . */

/* Source-file information.
   This describes the relation between source files and line numbers
   and addresses in the program text.  */

struct sourcevector
{
  int length;			/* Number of source files described */
  struct source *source[1];	/* Descriptions of the files */
};

/* Line number and address of one line.  */
 
struct line
{
  int linenum;
  int address;
};

/* All the information on one source file.  */

struct source
{
  char *name;			/* Name of file */
  int nlines;			/* Number of lines that follow */
  struct line lines[1];	/* Information on each line */
};

/* End of text from symseg.h */

struct g_line_file *first_file;

struct g_line_file *add_file();
struct g_line_vector *add_vector();

#define MAX_LINES_PER_VECTOR 50		/* lpv */

/* We've been told that the current address corresponds to line LINENO in
   file FILE_NUMBER */
void
gdb_line(file_number,lineno)
int file_number;
int lineno;
{
	struct g_line_file *f;
	struct g_line_vector *v;
	struct g_line *line;

	for(f=first_file;f;f=f->gdb_line_file_next_file)
		if(f->gdb_line_file_file_number==file_number)
			break;
	if(!f) f=add_file(file_number);
	v=f->gdb_line_file_vectors;
	if(!v || v->gdb_line_number_of_lines==MAX_LINES_PER_VECTOR)
		v=add_vector(f);
	line= &(v->gdb_line_lines)[v->gdb_line_number_of_lines];
	v->gdb_line_number_of_lines++;
	line->gdb_line_line_number=lineno;
	line->gdb_line_frag= frag_now;
	line->gdb_line_offset=obstack_next_free(&frags)-frag_now->fr_literal;
}

/* We've been told where to store the .line table for file FILE_NUMBER */
void
gdb_line_tab(file_number,offset)
int file_number;
int offset;
{
	struct g_line_file *f;

	for(f=first_file;f;f=f->gdb_line_file_next_file)
		if(f->gdb_line_file_file_number==file_number)
			break;
	if(!f) f=add_file(file_number);
	if(f->gdb_line_table_offset)
		as_warn("Ignoring duplicate .linetab for file %d",file_number);
	else
		f->gdb_line_table_offset=offset;
}

/* We've got a file (FILE_NUMBER) that we haven't heard about before.  Create
   an entry for it, etc. . . */
struct g_line_file *
add_file(file_number)
{
	struct g_line_file *f;

	f=(struct g_line_file *)xmalloc(sizeof(struct g_line_file));
	f->gdb_line_file_file_number=file_number;
	f->gdb_line_table_offset = 0;
	f->gdb_line_file_number_of_vectors=0;
	f->gdb_line_file_vectors=(struct g_line_vector *)0;
	f->gdb_line_file_next_file=first_file;
	first_file=f;
	return f;
}

/* The last vector for file F is full.  Allocate a new one. */
struct g_line_vector *
add_vector(f)
struct g_line_file *f;
{
	struct g_line_vector *tmp_vec;

	f->gdb_line_file_number_of_vectors++;
	tmp_vec=(struct g_line_vector *)xmalloc(sizeof(struct g_line_vector));
	tmp_vec->gdb_line_number_of_lines=0;
	tmp_vec->gdb_line_lines=(struct g_line *)xmalloc(MAX_LINES_PER_VECTOR*sizeof(struct g_line));
	tmp_vec->gdb_line_next_vector=f->gdb_line_file_vectors;
	f->gdb_line_file_vectors=tmp_vec;
	return tmp_vec;
}

/* All done.  Time to write the stuff out.  This should be fun. */
void
gdb_lines_emit()
{
	struct g_line_file *f;
	struct g_line_vector *v,*old_v,*v_tmp;
	struct g_line *current_line_pointer;	/* lp */
	int	n;
	int	previous_line_number;
	long int current_gdb_segment_pos;
	unsigned int number_of_things_in_table;

	for(f=first_file;f;f=f->gdb_line_file_next_file) {
		if(!f->gdb_line_table_offset) {
			as_warn("No .gdblinetab given for file %d.  Ignoring .gdbline(s) for it.");
			continue;
		}

		/* Reverse the linked list of vectors.  Since we built it
		   last entry first, this puts the first entry at the start
		   of the list.  Thus we can manage to put out low line #s
		   at the start of the table. . .*/
		v_tmp=0;
		old_v=0;
		for(v=f->gdb_line_file_vectors;v;v=v_tmp) {
			v_tmp=v->gdb_line_next_vector;
			v->gdb_line_next_vector=old_v;
			old_v=v;
		}
		f->gdb_line_file_vectors=old_v;

		/* Start putting stuff at the beginning of the table */
		current_gdb_segment_pos=f->gdb_line_table_offset+sizeof(long int);
		previous_line_number = -2;
		number_of_things_in_table = 0;

		/* For every vector in the table: */
		for(v=f->gdb_line_file_vectors;v;v=v->gdb_line_next_vector) {
			current_line_pointer=v->gdb_line_lines;

			/* For every element of every vector */
			for(n=v->gdb_line_number_of_lines;n;n--) {

				if(current_line_pointer->gdb_line_line_number != previous_line_number + 1) {
					/* Write out the line number */
					gdb_alter(current_gdb_segment_pos, -(current_line_pointer->gdb_line_line_number));
					current_gdb_segment_pos+=sizeof(long int);
					number_of_things_in_table++;
				}
				previous_line_number = current_line_pointer->gdb_line_line_number;

				/* And write out the address */
				gdb_alter(current_gdb_segment_pos,current_line_pointer->gdb_line_frag->fr_address+current_line_pointer->gdb_line_offset);
				current_gdb_segment_pos+=sizeof(long int);
				number_of_things_in_table++;

				current_line_pointer++;
			}
		}
		gdb_alter(f->gdb_line_table_offset,number_of_things_in_table);
	}
}
