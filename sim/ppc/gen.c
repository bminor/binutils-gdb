/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


/* BUGS:

   Instead of using/abusing macro's the semantic code should be parsed
   and each `cachable' expression replaced with the corresponding
   value.

   If not expanding fields, invalid_insn has call to its self this is
   a little fatal when semantic_invalid() is being called because an
   instruction is invalid */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"
#include "ppc-config.h"

#undef WITH_ASSERT
#define WITH_ASSERT 1

#include "debug.h"


/****************************************************************/

void
error (char *msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  vprintf(msg, ap);
  va_end(ap);
  exit (1);
}

void *
zmalloc(long size)
{
  void *memory = malloc(size);
  if (memory == NULL)
    error("zmalloc failed\n");
  bzero(memory, size);
  return memory;
}

void
dumpf (int indent, char *msg, ...)
{
  va_list ap;
  for (; indent > 0; indent--)
    printf(" ");
  va_start(ap, msg);
  vprintf(msg, ap);
  va_end(ap);
}


/****************************************************************/

int idecode_expand_semantics = WITH_IDECODE_EXPAND_SEMANTICS;
int idecode_cache = WITH_IDECODE_CACHE;
int spreg_lookup_table = WITH_SPREG_LOOKUP_TABLE;

/****************************************************************/

typedef struct {
  int valid;
  char *old_name;
  char *new_name;
  char *type;
  char *expression;
} extraction_rules;

extraction_rules cachable_values[] = WITH_IDECODE_CACHE_RULES;

/****************************************************************/

typedef struct _opcode_rules {
  int valid;
  int first;
  int last;
  int force_first;
  int force_last;
  int force_slash;
  char *force_expansion;
  int use_switch;
  unsigned special_mask;
  unsigned special_value;
  unsigned special_rule;
} opcode_rules;

/* FIXME - this should be loaded from a file */
opcode_rules opcode_table[] = WITH_IDECODE_OPCODE_RULES;

dump_opcode_rule(opcode_rules *rule,
		 int indent)
{
  printf("(opcode_rules*)0x%x\n", rule);
  dumpf(indent, "(valid %d)\n", rule->valid);
  ASSERT(rule != NULL);
  if (rule->valid) {
    dumpf(indent, "(first %d)\n", rule->first);
    dumpf(indent, "(last %d)\n", rule->last);
    dumpf(indent, "(force_first %d)\n", rule->force_first);
    dumpf(indent, "(force_last %d)\n", rule->force_last);
    dumpf(indent, "(force_slash %d)\n", rule->force_slash);
    dumpf(indent, "(force_expansion %s)\n", rule->force_expansion);
  }
}


/****************************************************************/

enum gen_constants {
  insn_size = 32,
  nr_of_sprs = 1024
};

char cache_idecode_formal[] = "cpu *processor,\n instruction_word instruction,\n unsigned_word cia,\n idecode_cache *cache_entry";
char cache_idecode_actual[] = "processor, instruction, cia, cache_entry";
char cache_insn_formal[] = "cpu *processor,\n idecode_cache *cache_entry,\n unsigned_word cia";
char cache_insn_actual[] = "processor, entry, cia";

char insn_formal[] = "cpu *processor,\n instruction_word instruction,\n unsigned_word cia";
char insn_actual[] = "processor, instruction, cia";

char insn_local[] = "unsigned_word nia = cia + 4;";


/****************************************************************/

int
bin2i(char *bin, int width)
{
  int i = 0;
  while (*bin != '\0' && width != 0) {
    i = (i << 1) + (*bin - '0');
    width--;
    bin++;
  }
  return i;
}


int
it_is(char *flag,
      char *flags)
{
  int flag_len = strlen(flag);
  while (*flags != '\0') {
    if (!strncmp(flags, flag, flag_len)
	&& (flags[flag_len] == ',' || flags[flag_len] == '\0'))
      return 1;
    while (*flags != ',') {
      if (*flags == '\0')
	return 0;
      flags++;
    }
    flags++;
  }
  return 0;
}


/****************************************************************/

typedef struct _lf lf;
struct _lf {
  FILE *stream;
  int line_nr; /* nr complete lines written, curr line is line_nr+1 */
  int indent;
  int line_blank;
  char *file_name;
};


lf *
lf_open(char *name)
{
  /* create a file object */
  lf *new_lf = zmalloc(sizeof(lf));
  ASSERT(new_lf != NULL);
  new_lf->file_name = name;

  /* attach to stdout if pipe */
  if (!strcmp(name, "-")) {
    new_lf->stream = stdout;
  }
  else {
    /* create a new file */
    new_lf->stream = fopen(name, "w");
    ASSERT(new_lf->stream != NULL);
  }
  return new_lf;
}


void
lf_close(lf *file)
{
  if (file->stream != stdout) {
    if (fclose(file->stream)) {
      perror("lf_close.fclose");
      exit(1);
    }
    free(file);
  }
}


void
lf_putchr(lf *file,
	  const char chr)
{
  if (chr == '\n') {
    file->line_nr += 1;
    file->line_blank = 1;
  }
  else if (file->line_blank) {
    int pad;
    for (pad = file->indent; pad > 0; pad--)
      putc(' ', file->stream);
    file->line_blank = 0;
  }
  putc(chr, file->stream);
}

void
lf_indent_suppress(lf *file)
{
  file->line_blank = 0;
}


void
lf_putstr(lf *file,
	  const char *string)
{
  const char *chp;
  if (string != NULL) {
    for (chp = string; *chp != '\0'; chp++) {
      lf_putchr(file, *chp);
    }
  }
}

void
do_lf_putunsigned(lf *file,
	      unsigned u)
{
  if (u > 0) {
    do_lf_putunsigned(file, u / 10);
    lf_putchr(file, (u % 10) + '0');
  }
}


void
lf_putint(lf *file,
	  int decimal)
{
  if (decimal == 0)
    lf_putchr(file, '0');
  else if (decimal < 0) {
    lf_putchr(file, '-');
    do_lf_putunsigned(file, -decimal);
  }
  else if (decimal > 0) {
    do_lf_putunsigned(file, decimal);
  }
  else
    ASSERT(0);
}

void
lf_printf(lf *file,
	  const char *fmt,
	  ...)
{
  char buf[1024];
  va_list ap;
  int nr_chars;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  /* FIXME - this is really stuffed but so is vsprintf() on a sun! */
  ASSERT(strlen(buf) > 0 && strlen(buf) < sizeof(buf));
  lf_putstr(file, buf);
  va_end(ap);
}

void
lf_print_file_line_nr(lf *file)
{
#if WITH_LINE_NUMBERS
  lf_indent_suppress(file);
  lf_putstr(file, "#line ");
  lf_putint(file, file->line_nr+2); /*line_nr == last_line, want next */
  lf_putstr(file, " \"");
  lf_putstr(file, file->file_name);
  lf_putstr(file, "\"\n");
#endif
}

lf_indent(lf *file, int delta)
{
  file->indent += delta;
}


/****************************************************************/

/* read entries from ppc.instructions */

enum {
  file_table_max_fields = 6,
};

typedef struct _file_table file_table;
struct _file_table {
  size_t size;
  char *buffer;
  char *pos;
  int line_nr;
  char *file_name;
};

typedef struct _file_table_entry file_table_entry;
struct _file_table_entry {
  char *fields[file_table_max_fields];
  int line_nr;
  char *file_name;
  char *annex;
};


file_table *
file_table_open(char *file_name)
{
  int fd;
  struct stat stat_buf;
  file_table *file;

  /* create a file descriptor */
  file = (file_table*)zmalloc(sizeof(file_table));
  ASSERT(file != NULL);

  /* save the file name */
  file->file_name = (char*)zmalloc(strlen(file_name) + 1);
  ASSERT(file->file_name != NULL);
  strcpy(file->file_name, file_name);

  /* open the file */
  fd = open(file->file_name, O_RDONLY, 0);
  ASSERT(fd >= 0);

  /* determine the size */
  if (fstat(fd, &stat_buf) < 0) {
    perror("file_table_open.fstat");
    exit(1);
  }
  file->size = stat_buf.st_size;

  /* allocate this much memory */
  file->buffer = (char*)zmalloc(file->size+1);
  if(file->buffer == NULL) {
    perror("file_table_open.calloc.file->size+1");
    exit(1);
  }
  file->pos = file->buffer;

  /* read it in */
  if (read(fd, file->buffer, file->size) < file->size) {
    perror("file_table_open.read");
    exit(1);
  }
  file->buffer[file->size] = '\0';

  /* done */
  close(fd);
  return file;
}


file_table_entry *
file_table_read(file_table *file)
{
  int field;
  file_table_entry *entry;

  /* skip comments/blanks */
  while(1) {
    /* leading white space */
    while (*file->pos != '\0'
	   && *file->pos != '\n'
	   && isspace(*file->pos))
      file->pos++;
    /* comment */
    if (*file->pos == '#') {
      do {
	file->pos++;
      } while (*file->pos != '\0' && *file->pos != '\n');
    }
    /* end of line? */
    if (*file->pos == '\n') {
      file->pos++;
      file->line_nr++;
    }
    else
      break;
  }
  if (*file->pos == '\0')
    return NULL;

  /* create this new entry */
  entry = (file_table_entry*)zmalloc(sizeof(file_table_entry));
  ASSERT(entry != NULL);
  entry->file_name = file->file_name;

  /* break the line into its colon delimitered fields */
  for (field = 0; field < file_table_max_fields-1; field++) {
    entry->fields[field] = file->pos;
    while(*file->pos && *file->pos != ':' && *file->pos != '\n')
      file->pos++;
    if (*file->pos == ':') {
      *file->pos = '\0';
      file->pos++;
    }
  }

  /* any trailing stuff not the last field */
  ASSERT(field == file_table_max_fields-1);
  entry->fields[field] = file->pos;
  while (*file->pos && *file->pos != '\n') {
    file->pos++;
  }
  if (*file->pos == '\n') {
    *file->pos = '\0';
    file->pos++;
  }
  file->line_nr++;
  entry->line_nr = file->line_nr;

  /* if following lines tab indented, put in the annex */
  if (*file->pos == '\t') {
    entry->annex = file->pos;
    do {
      do {
	file->pos++;
      } while (*file->pos != '\0' && *file->pos != '\n');
      if (*file->pos == '\n') {
	file->pos++;
	file->line_nr++;
      }
    } while (*file->pos != '\0' && *file->pos == '\t');
    if (file->pos[-1] == '\n')
      file->pos[-1] = '\0';
  }
  else
    entry->annex = NULL;

  /* return it */
  return entry;

}


void
dump_file_table_entry(file_table_entry *entry,
		      int indent)
{
  printf("(file_table_entry*)0x%x\n", entry);

  if (entry != NULL) {
    int field;
    char sep;

    sep = ' ';
    dumpf(indent, "(fields");
    for (field = 0; field < file_table_max_fields; field++) {
      printf("%c%s", sep, entry->fields[field]);
      sep = ':';
    }
    printf(")\n");

    dumpf(indent, "(line_nr %d)\n", entry->line_nr);

    dumpf(indent, "(file_name %s)\n", entry->file_name);

    dumpf(indent, "(annex\n%s\n", entry->annex);
    dumpf(indent, " )\n");

  }
}


/****************************************************************/

typedef struct _insn_field insn_field;
struct _insn_field {
  int first;
  int last;
  int width;
  int is_int;
  int is_slash;
  int is_string;
  int val_int;
  char *pos_string;
  char *val_string;
  insn_field *next;
  insn_field *prev;
};

typedef struct _insn_fields insn_fields;
struct _insn_fields {
  insn_field *bits[insn_size];
  insn_field *first;
  insn_field *last;
  unsigned value;
};

insn_field *
insn_field_new()
{
  insn_field *new_field = (insn_field*)zmalloc(sizeof(insn_field));
  ASSERT(new_field != NULL);
  return new_field;
}

insn_fields *
insn_fields_new()
{
  insn_fields *new_fields = (insn_fields*)zmalloc(sizeof(insn_fields));
  ASSERT(new_fields != NULL);
  return new_fields;
}


insn_fields *
parse_insn_format(file_table_entry *entry,
		  char *format)
{
  char *chp;
  insn_fields *fields = insn_fields_new();

  /* create a leading sentinal */
  fields->first = insn_field_new();
  fields->first->first = -1;
  fields->first->last = -1;
  fields->first->width = 0;

  /* and a trailing sentinal */
  fields->last = insn_field_new();
  fields->last->first = insn_size;
  fields->last->last = insn_size;
  fields->last->width = 0;

  /* link them together */
  fields->first->next = fields->last;
  fields->last->prev = fields->first;

  /* now work through the formats */
  chp = format;

  while (*chp != '\0') {
    char *start_pos;
    char *start_val;
    int strlen_val;
    int strlen_pos;
    insn_field *new_field;

    /* sanity check */
    if (!isdigit(*chp)) {
      fprintf(stderr, "%s:%d: missing position field at %s\n",
	      entry->file_name, entry->line_nr, chp);
      break;
    }

    /* break out the bit position */
    start_pos = chp;
    while (isdigit(*chp))
      chp++;
    strlen_pos = chp - start_pos;
    if (*chp == '.' && strlen_pos > 0)
      chp++;
    else {
      fprintf(stderr, "%s:%d: missing field value at %s\n",
	      entry->file_name, entry->line_nr, chp);
      break;
    }

    /* break out the value */
    start_val = chp;
    while (*start_val == '/' && *chp == '/'
	   || isdigit(*start_val) && isdigit(*chp)
	   || isalpha(*start_val) && (isalnum(*chp) || *chp == '_'))
      chp++;
    strlen_val = chp - start_val;
    if (*chp == ',')
      chp++;
    else if (*chp != '\0' || strlen_val == 0) {
      fprintf(stderr, "%s:%d: missing field terminator at %s\n",
	      entry->file_name, entry->line_nr, chp);
      break;
    }

    /* create a new field and insert it */
    new_field = insn_field_new();
    new_field->next = fields->last;
    new_field->prev = fields->last->prev;
    new_field->next->prev = new_field;
    new_field->prev->next = new_field;

    /* the value */
    new_field->val_string = (char*)zmalloc(strlen_val+1);
    strncpy(new_field->val_string, start_val, strlen_val);
    if (isdigit(*new_field->val_string)) {
      new_field->val_int = atoi(new_field->val_string);
      new_field->is_int = 1;
    }
    else if (new_field->val_string[0] == '/') {
      new_field->is_slash = 1;
    }
    else {
      new_field->is_string = 1;
    }
    
    /* the pos */
    new_field->pos_string = (char*)zmalloc(strlen_pos+1);
    strncpy(new_field->pos_string, start_pos, strlen_pos);
    new_field->first = atoi(new_field->pos_string);
    new_field->last = new_field->next->first - 1; /* guess */
    new_field->width = new_field->last - new_field->first + 1; /* guess */
    new_field->prev->last = new_field->first-1; /*fix*/
    new_field->prev->width = new_field->first - new_field->prev->first; /*fix*/
  }

  /* fiddle first/last so that the sentinals `disapear' */
  ASSERT(fields->first->last < 0);
  ASSERT(fields->last->first >= insn_size);
  fields->first = fields->first->next;
  fields->last = fields->last->prev;

  /* now go over this again, pointing each bit position at a field
     record */
  {
    int i;
    insn_field *field;
    field = fields->first;
    for (i = 0; i < insn_size; i++) {
      while (field->last < i)
	field = field->next;
      fields->bits[i] = field;
    }
  }

  /* go over each of the fields, and compute a `value' for the insn */
  {
    insn_field *field;
    fields->value = 0;
    for (field = fields->first;
	 field->last < insn_size;
	 field = field->next) {
      fields->value <<= field->width;
      if (field->is_int)
	fields->value |= field->val_int;
    }
  }
  return fields;
}


typedef enum {
  field_constant_int = 1,
  field_constant_slash = 2,
  field_constant_string = 3
} constant_field_types;


int
insn_field_is_constant(insn_field *field,
		       opcode_rules *rule)
{
  /* field is an integer */
  if (field->is_int)
    return field_constant_int;
  /* field is `/' and treating that as a constant */
  if (field->is_slash && rule->force_slash)
    return field_constant_slash;
  /* field, though variable is on the list */
  if (field->is_string && rule->force_expansion != NULL) {
    char *forced_fields = rule->force_expansion;
    while (*forced_fields != '\0') {
      int field_len;
      char *end = strchr(forced_fields, ',');
      if (end == NULL)
	field_len = strlen(forced_fields);
      else
	field_len = end-forced_fields;
      if (strncmp(forced_fields, field->val_string, field_len) == 0
	  && field->val_string[field_len] == '\0')
	return field_constant_string;
      forced_fields += field_len;
      if (*forced_fields == ',')
	forced_fields++;
    }
  }
  return 0;
}


void
dump_insn_field(insn_field *field,
		int indent)
{

  printf("(insn_field*)0x%x\n", field);

  dumpf(indent, "(first %d)\n", field->first);

  dumpf(indent, "(last %d)\n", field->last);

  dumpf(indent, "(width %d)\n", field->width);

  if (field->is_int)
    dumpf(indent, "(is_int %d)\n", field->val_int);

  if (field->is_slash)
    dumpf(indent, "(is_slash)\n");

  if (field->is_string)
    dumpf(indent, "(is_string `%s')\n", field->val_string);
  
  dumpf(indent, "(next 0x%x)\n", field->next);
  
  dumpf(indent, "(prev 0x%x)\n", field->prev);
  

}

void
dump_insn_fields(insn_fields *fields,
		 int indent)
{
  insn_field *field;
  int i;

  printf("(insn_fields*)0x%x\n", fields);

  dumpf(indent, "(first 0x%x)\n", fields->first);
  dumpf(indent, "(last 0x%x)\n", fields->last);

  dumpf(indent, "(value 0x%x)\n", fields->value);

  for (i = 0; i < insn_size; i++) {
    dumpf(indent, "(bits[%d] ", i, fields->bits[i]);
    dump_insn_field(fields->bits[i], indent+1);
    dumpf(indent, " )\n");
  }

}


/****************************************************************/

typedef struct _opcode_field opcode_field;
struct _opcode_field {
  int first;
  int last;
  int is_boolean;
  opcode_field *parent;
};

opcode_field *
opcode_field_new()
{
  opcode_field *new_field = (opcode_field*)zmalloc(sizeof(opcode_field));
  ASSERT(new_field != NULL);
  new_field->first = insn_size;
  new_field->last = -1;
  return new_field;
}

void
dump_opcode_field(opcode_field *field, int indent, int levels)
{
  printf("(opcode_field*)0x%x\n", field);
  if (levels && field != NULL) {
    dumpf(indent, "(first %d)\n", field->first);
    dumpf(indent, "(last %d)\n", field->last);
    dumpf(indent, "(is_boolean %d)\n", field->is_boolean);
    dumpf(indent, "(parent ");
    dump_opcode_field(field->parent, indent, levels-1);
  }
}


/****************************************************************/

typedef struct _insn_bits insn_bits;
struct _insn_bits {
  int is_expanded;
  int value;
  insn_field *field;
  opcode_field *opcode;
  insn_bits *last;
};

insn_bits *
insn_bits_new()
{
  insn_bits *new_bits = (insn_bits*)zmalloc(sizeof(insn_bits));
  ASSERT(new_bits);
  return new_bits;
}


void
dump_insn_bits(insn_bits *bits, int indent, int levels)
{
  printf("(insn_bits*)0x%x\n", bits);

  if (levels && bits != NULL) {
    dumpf(indent, "(value %d)\n", bits->value);
    dumpf(indent, "(opcode ");
    dump_opcode_field(bits->opcode, indent+1, 0);
    dumpf(indent, " )\n");
    dumpf(indent, "(field ");
    dump_insn_field(bits->field, indent+1);
    dumpf(indent, " )\n");
    dumpf(indent, "(last ");
    dump_insn_bits(bits->last, indent+1, levels-1);
  }
}


/****************************************************************/


typedef enum {
  insn_format,
  insn_form,
  insn_flags,
  insn_nmemonic,
  insn_name,
  insn_comment,
  nr_insn_table_fields = file_table_max_fields
} insn_table_fields;
char *insn_field_name[] = {
  "format", "form", "flags", "nmemonic", "name", "comments"
};

typedef struct _insn insn;
struct _insn {
  file_table_entry *file_entry;
  insn_fields *fields;
  insn *next;
};

typedef struct _insn_table insn_table;
struct _insn_table {
  int opcode_nr;
  insn_bits *expanded_bits;
  int nr_insn;
  insn *insns;
  opcode_rules *opcode_rule;
  opcode_field *opcode;
  int nr_entries;
  insn_table *entries;
  insn_table *sibling;
  insn_table *parent;
};



insn *
insn_new()
{
  insn *new_entry = ((insn*) zmalloc(sizeof(insn)));
  ASSERT(new_entry != NULL);
  return new_entry;
}

insn_table *
insn_table_new()
{
  insn_table *new_table = (insn_table*)zmalloc(sizeof(insn_table));
  ASSERT(new_table != NULL);
  return new_table;
}


void
insn_table_insert_insn(insn_table *table,
		       file_table_entry *file_entry,
		       insn_fields *fields)
{
  insn **ptr_to_cur_insn = &table->insns;
  insn *cur_insn = *ptr_to_cur_insn;

  /* create a new instruction */
  insn *new_insn = insn_new();
  new_insn->file_entry = file_entry;
  new_insn->fields = fields;

  /* insert it according to the order of the fields */
  while (cur_insn != NULL
	 && new_insn->fields->value >= cur_insn->fields->value) {
    ptr_to_cur_insn = &cur_insn->next;
    cur_insn = *ptr_to_cur_insn;
  }

  new_insn->next = cur_insn;
  *ptr_to_cur_insn = new_insn;

  table->nr_insn++;
}


opcode_field *
insn_table_find_opcode_field(insn *insns,
			     opcode_rules *rule,
			     int string_only)
{
  opcode_field *curr_opcode = opcode_field_new();
  insn *entry;

  ASSERT(rule->valid);

  for (entry = insns; entry != NULL; entry = entry->next) {
    insn_fields *fields = entry->fields;
    opcode_field new_opcode;

    /* find a start point for the opcode field */
    new_opcode.first = rule->first;
    while (new_opcode.first <= rule->last
	   && (!string_only
	       || insn_field_is_constant(fields->bits[new_opcode.first],
					 rule) != field_constant_string)
	   && (string_only
	       || !insn_field_is_constant(fields->bits[new_opcode.first],
					  rule)))
      new_opcode.first = fields->bits[new_opcode.first]->last + 1;
    ASSERT(new_opcode.first > rule->last
	   || (string_only
	       && insn_field_is_constant(fields->bits[new_opcode.first],
					 rule) == field_constant_string)
	   || (!string_only
	       && insn_field_is_constant(fields->bits[new_opcode.first],
					 rule)));
  
    /* find the end point for the opcode field */
    new_opcode.last = rule->last;
    while (new_opcode.last >= rule->first
	   && (!string_only
	       || insn_field_is_constant(fields->bits[new_opcode.last],
					 rule) != field_constant_string)
	   && (string_only
	       || !insn_field_is_constant(fields->bits[new_opcode.last],
					  rule)))
      new_opcode.last = fields->bits[new_opcode.last]->first - 1;
    ASSERT(new_opcode.last < rule->first
	   || (string_only
	       && insn_field_is_constant(fields->bits[new_opcode.last],
					 rule) == field_constant_string)
	   || (!string_only
	       && insn_field_is_constant(fields->bits[new_opcode.last],
					 rule)));

    /* now see if our current opcode needs expanding */
    if (new_opcode.first <= rule->last
	&& curr_opcode->first > new_opcode.first)
      curr_opcode->first = new_opcode.first;
    if (new_opcode.last >= rule->first
	&& curr_opcode->last < new_opcode.last)
      curr_opcode->last = new_opcode.last;
    
  }

  /* was any thing interesting found? */
  if (curr_opcode->first > rule->last) {
    ASSERT(curr_opcode->last < rule->first);
    free(curr_opcode);
    return NULL;
  }
  ASSERT(curr_opcode->last >= rule->first);
  ASSERT(curr_opcode->first <= rule->last);

  /* if something was found, check it includes the forced field range */
  if (!string_only
      && curr_opcode->first > rule->force_first) {
    curr_opcode->first = rule->force_first;
  }
  if (!string_only
      && curr_opcode->last < rule->force_last) {
    curr_opcode->last = rule->force_last;
  }
  /* handle special case elminating any need to do shift after mask */
  if (string_only
      && rule->force_last == insn_size-1) {
    curr_opcode->last = insn_size-1;
  }

  /* handle any special cases */
  switch (rule->special_rule) {
  case 0: /* let the above apply */
    break;
  case 1: /* expand a limited nr of bits, ignoring the rest */
    curr_opcode->first = rule->force_first;
    curr_opcode->last = rule->force_last;
    break;
  case 2: /* boolean field */
    curr_opcode->is_boolean = 1;
    break;
  }

  return curr_opcode;
}


void
insn_table_insert_expanded(insn_table *table,
			   insn *old_insn,
			   int new_opcode_nr,
			   insn_bits *new_bits)
{
  insn_table **ptr_to_cur_entry = &table->entries;
  insn_table *cur_entry = *ptr_to_cur_entry;

  /* find the new table for this entry */
  while (cur_entry != NULL
	 && cur_entry->opcode_nr < new_opcode_nr) {
    ptr_to_cur_entry = &cur_entry->sibling;
    cur_entry = *ptr_to_cur_entry;
  }

  if (cur_entry == NULL || cur_entry->opcode_nr != new_opcode_nr) {
    insn_table *new_entry = insn_table_new();
    new_entry->opcode_nr = new_opcode_nr;
    new_entry->expanded_bits = new_bits;
    new_entry->opcode_rule = table->opcode_rule+1;
    new_entry->sibling = cur_entry;
    new_entry->parent = table;
    *ptr_to_cur_entry = new_entry;
    cur_entry = new_entry;
    table->nr_entries++;
  }
  /* ASSERT new_bits == cur_entry bits */
  ASSERT(cur_entry != NULL && cur_entry->opcode_nr == new_opcode_nr);
  insn_table_insert_insn(cur_entry,
			 old_insn->file_entry,
			 old_insn->fields);
}

void
insn_table_expand_opcode(insn_table *table,
			 insn *instruction,
			 int field_nr,
			 int opcode_nr,
			 insn_bits *bits)
{

  if (field_nr > table->opcode->last) {
    insn_table_insert_expanded(table, instruction, opcode_nr, bits);
  }
  else {
    insn_field *field = instruction->fields->bits[field_nr];
    if (field->is_int || field->is_slash) {
      ASSERT(field->first >= table->opcode->first
	     && field->last <= table->opcode->last);
      insn_table_expand_opcode(table, instruction, field->last+1,
			       ((opcode_nr << field->width) + field->val_int),
			       bits);
    }
    else {
      int val;
      int last_pos = ((field->last < table->opcode->last)
			? field->last : table->opcode->last);
      int first_pos = ((field->first > table->opcode->first)
			 ? field->first : table->opcode->first);
      int width = last_pos - first_pos + 1;
      int last_val = (table->opcode->is_boolean
		      ? 2 : (1 << width));
      for (val = 0; val < last_val; val++) {
	insn_bits *new_bits = insn_bits_new();
	new_bits->field = field;
	new_bits->value = val;
	new_bits->last = bits;
	new_bits->opcode = table->opcode;
	insn_table_expand_opcode(table, instruction, last_pos+1,
				 ((opcode_nr << width) | val),
				 new_bits);
      }
    }
  }
}

insn_table_insert_expanding(insn_table *table,
			    insn *entry)
{
  insn_table_expand_opcode(table,
			   entry,
			   table->opcode->first,
			   0,
			   table->expanded_bits);
}


void
insn_table_expand_insns(insn_table *table)
{

  ASSERT(table->nr_insn >= 1);

  /* determine a valid opcode */
  while (table->opcode_rule->valid) {
    /* specials only for single instructions */
    if ((table->nr_insn > 1
	 && table->opcode_rule->special_mask == 0
	 && table->opcode_rule->special_rule == 0)
	|| (table->nr_insn == 1
	    && table->opcode_rule->special_mask != 0
	    && ((table->insns->fields->value
		 & table->opcode_rule->special_mask)
		== table->opcode_rule->special_value))
	|| (idecode_expand_semantics
	    && table->opcode_rule->special_mask == 0
	    && table->opcode_rule->special_rule == 0))
      table->opcode =
	insn_table_find_opcode_field(table->insns,
				     table->opcode_rule,
				     table->nr_insn == 1/*string*/
				     );
    if (table->opcode != NULL)
      break;
    table->opcode_rule++;
  }

  /* did we find anything */
  if (table->opcode == NULL) {
    return;
  }
  ASSERT(table->opcode != NULL);

  /* back link what we found to its parent */
  if (table->parent != NULL) {
    ASSERT(table->parent->opcode != NULL);
    table->opcode->parent = table->parent->opcode;
  }

  /* expand the raw instructions according to the opcode */
  {
    insn *entry;
    for (entry = table->insns; entry != NULL; entry = entry->next) {
      insn_table_insert_expanding(table, entry);
    }
  }

  /* and do the same for the sub entries */
  {
    insn_table *entry;
    for (entry = table->entries; entry != NULL; entry =  entry->sibling) {
      insn_table_expand_insns(entry);
    }
  }
}



insn_table *
insn_table_load_insns(char *file_name)
{
  file_table *file = file_table_open(file_name);
  insn_table *table = insn_table_new();
  file_table_entry *file_entry;
  table->opcode_rule = opcode_table;

  while (file_entry = file_table_read(file)) {
    insn_fields *fields;
    /* skip instructions that aren't relevant to the mode */
    if (it_is("64", file_entry->fields[insn_flags]) && !WITH_64BIT_TARGET
	|| it_is("32", file_entry->fields[insn_flags]) && WITH_64BIT_TARGET)
      continue;
    /* create/insert the new instruction */
    fields = parse_insn_format(file_entry, file_entry->fields[insn_format]);
    insn_table_insert_insn(table, file_entry, fields);
  }

  return table;
}


void
dump_insn(insn *entry, int indent, int levels)
{
  printf("(insn*)0x%x\n", entry);

  if (levels && entry != NULL) {

    dumpf(indent, "(file_entry ");
    dump_file_table_entry(entry->file_entry, indent+1);
    dumpf(indent, " )\n");

    dumpf(indent, "(fields ");
    dump_insn_fields(entry->fields, indent+1);
    dumpf(indent, " )\n");

    dumpf(indent, "(next ");
    dump_insn(entry->next, indent+1, levels-1);
    dumpf(indent, " )\n");

  }

}


void
dump_insn_table(insn_table *table,
		int indent, int levels)
{

  printf("(insn_table*)0x%x\n", table);

  if (levels && table != NULL) {
    insn *entry;

    dumpf(indent, "(opcode_nr %d)\n", table->opcode_nr);

    dumpf(indent, "(expanded_bits ");
    dump_insn_bits(table->expanded_bits, indent+1, -1);
    dumpf(indent, " )\n");

    dumpf(indent, "(int nr_insn %d)\n", table->nr_insn);

    dumpf(indent, "(insns ");
    dump_insn(table->insns, indent+1, table->nr_insn);
    dumpf(indent, " )\n");

    dumpf(indent, "(opcode_rule ");
    dump_opcode_rule(table->opcode_rule, indent+1);
    dumpf(indent, " )\n");

    dumpf(indent, "(opcode ");
    dump_opcode_field(table->opcode, indent+1, 1);
    dumpf(indent, " )\n");

    dumpf(indent, "(nr_entries %d)\n", table->entries);
    dumpf(indent, "(entries ");
    dump_insn_table(table->entries, indent+1, table->nr_entries);
    dumpf(indent, " )\n");

    dumpf(indent, "(sibling ", table->sibling);
    dump_insn_table(table->sibling, indent+1, levels-1);
    dumpf(indent, " )\n");

    dumpf(indent, "(parent ", table->parent);
    dump_insn_table(table->parent, indent+1, 0);
    dumpf(indent, " )\n");

  }
}


/****************************************************************/


void
lf_print_copyleft(lf *file)
{
  lf_putstr(file, "\
/* This file is part of psim (model of the PowerPC(tm) architecture)

   Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
   --

   PowerPC is a trademark of International Business Machines Corporation.

   --

   This file was generated by the program gen */
");
}


void
lf_print_c_line_nr(lf *file, file_table_entry *entry)
{
#if WITH_LINE_NUMBERS
  lf_indent_suppress(file);
  lf_putstr(file, "#line ");
  lf_putint(file, entry->line_nr);
  lf_putstr(file, " \"");
  lf_putstr(file, entry->file_name);
  lf_putstr(file, "\"\n");
#endif
}


void
lf_print_c_code(lf *file, char *code)
{
  char *chp = code;
  int in_bit_field = 0;
  while (*chp != '\0') {
    if (*chp == '\t')
      chp++;
    if (*chp == '#')
      lf_indent_suppress(file);
    while (*chp != '\0' && *chp != '\n') {
      if (chp[0] == '{' && !isspace(chp[1])) {
	in_bit_field = 1;
	lf_putchr(file, '_');
      }
      else if (in_bit_field && chp[0] == ':') {
	lf_putchr(file, '_');
      }
      else if (in_bit_field && *chp == '}') {
	lf_putchr(file, '_');
	in_bit_field = 0;
      }
      else {
	lf_putchr(file, *chp);
      }
      chp++;
    }
    if (in_bit_field)
      error("bit field paren miss match some where\n");
    if (*chp == '\n') {
      lf_putchr(file, '\n');
      chp++;
    }
  }
  lf_putchr(file, '\n');
}


void
lf_print_binary(lf *file, int decimal, int width)
{
  int bit;
  ASSERT(width > 0);

  for (bit = 1 << (width-1); bit != 0; bit >>= 1) {
    if (decimal & bit)
      lf_putchr(file, '1');
    else
      lf_putchr(file, '0');
  }

}


void
lf_print_insn_bits(lf *file, insn_bits *bits)
{
  if (bits == NULL)
    return;
  lf_print_insn_bits(file, bits->last);
  lf_putchr(file, '_');
  lf_putstr(file, bits->field->val_string);
  if (!bits->opcode->is_boolean || bits->value == 0) {
    if (bits->opcode->last < bits->field->last)
      lf_putint(file, bits->value << (bits->field->last - bits->opcode->last));
    else
      lf_putint(file, bits->value);
  }
}

void
lf_print_opcodes(lf *file,
		 insn_table *table)
{
  if (table != NULL) {
    while (1) {
      lf_printf(file, "_%d_%d",
		table->opcode->first,
		table->opcode->last);
      if (table->parent == NULL) break;
      lf_printf(file, "__%d", table->opcode_nr);
      table = table->parent;
    }
  }
}

void
lf_print_table_name(lf *file,
		    insn_table *table)
{
  lf_printf(file, "idecode_table");
  lf_print_opcodes(file, table);
}



typedef enum {
  function_name_prefix_semantics,
  function_name_prefix_idecode,
  function_name_prefix_none
} lf_function_name_prefixes;

void
lf_print_function_name(lf *file,
		       insn *instruction,
		       insn_bits *expanded_bits,
		       lf_function_name_prefixes prefix)
{
  /* the prefix */
  switch (prefix) {
  case function_name_prefix_semantics:
    lf_putstr(file, "semantic_");
    break;
  case function_name_prefix_idecode:
    lf_printf(file, "idecode_");
    break;
  default:
    break;
  }

  /* the function name */
  {
    char *pos;
    for (pos = instruction->file_entry->fields[insn_name];
	 *pos != '\0';
	 pos++) {
      switch (*pos) {
      case '/':
      case '-':
	break;
      case ' ':
	lf_putchr(file, '_');
	break;
      default:
	lf_putchr(file, *pos);
	break;
      }
    }
  }

  /* the suffix */
  if (idecode_expand_semantics)
    lf_print_insn_bits(file, expanded_bits);
}


void
lf_print_idecode_table(lf *file,
		       insn_table *entry)
{
  int can_assume_leaf;
  int rule;

  /* have a look at the rule table, if all table rules follow all
     switch rules, I can assume that all end points are leaves */
  rule = 0;
  while (opcode_table[rule].valid
	 && opcode_table[rule].use_switch)
    rule++;
  while (opcode_table[rule].valid
	 && !opcode_table[rule].use_switch
	 && !opcode_table[rule].special_rule)
    rule++;
  can_assume_leaf = !opcode_table[rule].valid;

  lf_printf(file, "{\n");
  lf_indent(file, +2);
  {
    lf_printf(file, "idecode_table_entry *table = ");
    lf_print_table_name(file, entry);
    lf_printf(file, ";\n");
    lf_printf(file, "int opcode = EXTRACTED32(instruction, %d, %d);\n",
	      entry->opcode->first, entry->opcode->last);
    lf_printf(file, "idecode_table_entry *table_entry = table + opcode;\n");
    lf_printf(file, "while (1) {\n");
    lf_indent(file, +2);
    {
      lf_printf(file, "while (table_entry->mask != 0) {\n");
      lf_indent(file, +2);
      {
	lf_printf(file, "table = ((idecode_table_entry*)\n");
	lf_printf(file, "         table_entry->function_or_table);\n");
	lf_printf(file, "opcode = ((instruction & table_entry->mask)\n");
	lf_printf(file, "          >> table_entry->shift);\n");
	lf_printf(file, "table_entry = table + opcode;\n");
      }
      lf_indent(file, -2);
      lf_printf(file, "}\n");
      if (!idecode_cache && can_assume_leaf) {
	lf_printf(file, "return (((idecode_semantic*)\n");
	lf_printf(file, "         table_entry->function_or_table)\n");
	lf_printf(file, "        (%s));\n", insn_actual);
      }
      else if (!idecode_cache && !can_assume_leaf) {
	lf_printf(file, "if (table_entry->shift == 0)");
	lf_printf(file, "  return (((idecode_semantic*)\n");
	lf_printf(file, "           table_entry->function_or_table)\n");
	lf_printf(file, "          (%s));\n", insn_actual);
      }
      else if (idecode_cache == 1 && can_assume_leaf) {
	lf_printf(file, "ASSERT(!entry->shift);\n");
	lf_printf(file, "return ((idecode_semantic*)\n");
	lf_printf(file, "        table_entry->function_or_table);\n");
      }
      else if (idecode_cache == 1 && !can_assume_leaf) {
	lf_printf(file, "if (table_entry->shift == 0)\n");
	lf_printf(file, "  return ((idecode_semantic*)\n");
	lf_printf(file, "          table_entry->function_or_table);\n");
	lf_printf(file, "else if (table_entry->shift == -1)\n");
	lf_printf(file, "  return (((idecode_crack*)\n");
	lf_printf(file, "           table_entry->function_or_table)\n");
	lf_printf(file, "          (%s));\n", insn_actual);
      }
      else {
	lf_printf(file, "if (table_entry->shift == 0)\n");
	lf_printf(file, "  return (((idecode_crack*)\n");
	lf_printf(file, "           table_entry->function_or_table)\n");
	lf_printf(file, "          (%s));\n", cache_idecode_actual);
      }
      if (!can_assume_leaf) {
	lf_printf(file, "opcode = (instruction & table_entry->shift) != 0;\n");
	lf_printf(file, "table = ((idecode_table_entry*)\n");
	lf_printf(file, "         table_entry->function_or_table);\n");
	lf_printf(file, "table_entry = table + opcode;\n");
      }
    }
    lf_indent(file, -2);
    lf_printf(file, "}\n");
  }
  lf_indent(file, -2);
  lf_printf(file, "}\n");
}


void
lf_print_my_prefix(lf *file,
		   file_table_entry *file_entry)
{
  lf_printf(file, "const char *const my_prefix = \n");
  lf_printf(file, "  \"%s:%s:%d:cache\";\n",
	    file_entry->file_name,
	    file_entry->fields[insn_name],
	    file_entry->line_nr);
}


void
lf_print_ptrace(lf *file)
{
  lf_printf(file, "\n");
  lf_putstr(file, "ITRACE(trace_semantics, (\"cia=0x%x\\n\", cia));\n");
}


/****************************************************************/

typedef void leaf_handler
(insn_table *entry,
 void *data,
 int depth);
typedef void padding_handler
(insn_table *table,
 void *data,
 int depth,
 int opcode_nr);


void
insn_table_traverse_tree(insn_table *table,
			 void *data,
			 int depth,
			 leaf_handler *start,
			 leaf_handler *leaf,
			 leaf_handler *end,
			 padding_handler *padding)
{
  insn_table *entry;
  int entry_nr;
  
  ASSERT(table != NULL
	 && table->opcode != NULL
	 && table->nr_entries > 0
	 && table->entries != 0);

  if (start != NULL && depth >= 0)
    start(table, data, depth);

  for (entry_nr = 0, entry = table->entries;
       entry_nr < (table->opcode->is_boolean
		   ? 2
		   : (1 << (table->opcode->last - table->opcode->first + 1)));
       entry_nr ++) {
    if (entry == NULL
	|| (!table->opcode->is_boolean
	    && entry_nr < entry->opcode_nr)) {
      if (padding != NULL && depth >= 0)
	padding(table, data, depth, entry_nr);
    }
    else {
      ASSERT(entry != NULL && (entry->opcode_nr == entry_nr
			       || table->opcode->is_boolean));
      if (entry->opcode != NULL && depth != 0) {
	insn_table_traverse_tree(entry, data, depth+1,
				 start, leaf, end, padding);
      }
      else if (depth >= 0) {
	if (leaf != NULL)
	  leaf(entry, data, depth);
      }
      entry = entry->sibling;
    }
  }
  if (end != NULL && depth >= 0)
    end(table, data, depth);
}


typedef void insn_handler
(insn_table *table,
 void *data,
 insn *instruction);

void
insn_table_traverse_insn(insn_table *table,
			 void *data,
			 insn_handler *leaf)
{
  insn *instruction;
  for (instruction = table->insns;
       instruction != NULL;
       instruction = instruction->next) {
    leaf(table, data, instruction);
  }
}


void
update_depth(insn_table *entry,
	     void *data,
	     int depth)
{
  int *max_depth = (int*)data;
  if (*max_depth < depth)
    *max_depth = depth;
}


int
insn_table_depth(insn_table *table)
{
  int depth = 0;
  insn_table_traverse_tree(table,
			   &depth,
			   1,
			   NULL, /*start*/
			   update_depth,
			   NULL, /*end*/
			   NULL); /*padding*/
  return depth;
}


/****************************************************************/

void
dump_traverse_start(insn_table *table,
		    void *data,
		    int depth)
{
  dumpf(depth*2, "(%d\n", table->opcode_nr);
}

void
dump_traverse_leaf(insn_table *entry,
		   void *data,
		   int depth)
{
  ASSERT(entry->nr_entries == 0
	 && entry->nr_insn == 1
	 && entry->opcode == NULL);
  dumpf(depth*2, ".%d %s\n", entry->opcode_nr,
	entry->insns->file_entry->fields[insn_format]);
}

void
dump_traverse_end(insn_table *table,
		  void *data,
		  int depth)
{
  dumpf(depth*2, ")\n");
}

void
dump_traverse_padding(insn_table *table,
		      void *data,
		      int depth,
		      int opcode_nr)
{
  dumpf(depth*2, ".<%d>\n", opcode_nr);
}


void
dump_traverse(insn_table *table)
{
  insn_table_traverse_tree(table, NULL, 1,
			   dump_traverse_start,
			   dump_traverse_leaf,
			   dump_traverse_end,
			   dump_traverse_padding);
}


/****************************************************************/


void
semantics_h_print_function(lf *file,
			   insn *instruction,
			   insn_bits *expanded_bits)
{
  lf_printf(file, "\n");
  lf_printf(file, "INLINE_SEMANTICS unsigned_word ");
  lf_print_function_name(file,
			 instruction,
			 expanded_bits,
			 function_name_prefix_semantics);
  lf_printf(file, "\n(%s);\n",
	    idecode_cache > 1 ? cache_insn_formal : insn_formal);
}


void
semantics_h_leaf(insn_table *entry,
		 void *data,
		 int depth)
{
  lf *file = (lf*)data;
  ASSERT(entry->nr_insn == 1);
  semantics_h_print_function(file, entry->insns, entry->expanded_bits);
}

void
semantics_h_insn(insn_table *entry,
		 void *data,
		 insn *instruction)
{
  lf *file = (lf*)data;
  semantics_h_print_function(file, instruction, NULL);
}


void 
gen_semantics_h(insn_table *table, lf *file)
{

  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _SEMANTICS_H_\n");
  lf_printf(file, "#define _SEMANTICS_H_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef INLINE_SEMANTICS\n");
  lf_printf(file, "#define INLINE_SEMANTICS\n");
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");
  lf_printf(file, "\n");

  if (idecode_expand_semantics)
    insn_table_traverse_tree(table,
			     file,
			     1,
			     NULL, /* start */
			     semantics_h_leaf, /* leaf */
			     NULL, /* end */
			     NULL); /* padding */
  else
    insn_table_traverse_insn(table,
			     file,
			     semantics_h_insn);

  lf_printf(file, "\n");
  lf_printf(file, "#endif /* _SEMANTICS_H_ */\n");

}

/****************************************************************/

typedef struct _icache_tree icache_tree;
struct _icache_tree {
  char *name;
  icache_tree *next;
  icache_tree *children;
};

icache_tree *
icache_tree_new()
{
  icache_tree *new_tree = (icache_tree*)zmalloc(sizeof(icache_tree));
  ASSERT(new_tree != NULL);
  return new_tree;
}

icache_tree *
icache_tree_insert(icache_tree *tree,
		   char *name)
{
  icache_tree *new_tree;
  /* find it */
  icache_tree **ptr_to_cur_tree = &tree->children;
  icache_tree *cur_tree = *ptr_to_cur_tree;
  while (cur_tree != NULL
	 && strcmp(cur_tree->name, name) < 0) {
    ptr_to_cur_tree = &cur_tree->next;
    cur_tree = *ptr_to_cur_tree;
  }
  ASSERT(cur_tree == NULL
	 || strcmp(cur_tree->name, name) >= 0);
  /* already in the tree */
  if (cur_tree != NULL
      && strcmp(cur_tree->name, name) == 0)
    return cur_tree;
  /* missing, insert it */
  ASSERT(cur_tree == NULL
	 || strcmp(cur_tree->name, name) > 0);
  new_tree = icache_tree_new();
  new_tree->name = name;
  new_tree->next = cur_tree;
  *ptr_to_cur_tree = new_tree;
  return new_tree;
}


icache_tree *
insn_table_cache_fields(insn_table *table)
{
  icache_tree *tree = icache_tree_new();
  insn *instruction;
  for (instruction = table->insns;
       instruction != NULL;
       instruction = instruction->next) {
    insn_field *field;
    icache_tree *form =
      icache_tree_insert(tree,
			 instruction->file_entry->fields[insn_form]);
    for (field = instruction->fields->first;
	 field != NULL;
	 field = field->next) {
      if (field->is_string)
	icache_tree_insert(form, field->val_string);
    }
  }
  return tree;
}



void
gen_icache_h(icache_tree *tree,
	     lf *file)
{
  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _ICACHE_H_\n");
  lf_printf(file, "#define _ICACHE_H_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef INLINE_ICACHE\n");
  lf_printf(file, "#define INLINE_ICACHE\n");
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");
  lf_printf(file, "\n");

  /* create an instruction cache if being used */
  if (idecode_cache) {
    lf_printf(file, "typedef struct _idecode_cache {\n");
    lf_printf(file, "  unsigned_word address;\n");
    lf_printf(file, "  void *semantic;\n");
    if (idecode_cache == 1) {
      lf_printf(file, "  instruction_word instruction;\n");
    }
    else {
      icache_tree *form;
      lf_printf(file, "  union {\n");
      for (form = tree->children;
	   form != NULL;
	   form = form->next) {
	icache_tree *field;
	lf_printf(file, "    struct {\n");
	for (field = form->children;
	     field != NULL;
	     field = field->next) {
	  extraction_rules *rule;
	  int found_rule = 0;
	  for (rule = cachable_values;
	       rule->valid;
	       rule++) {
	    if (strcmp(field->name, rule->old_name) == 0) {
	      found_rule = 1;
	      if (rule->new_name != NULL)
		lf_printf(file, "      %s %s; /* %s */\n",
			  rule->type == NULL ? "unsigned" : rule->type,
			  rule->new_name, rule->old_name);
	    }
	  }
	  if (!found_rule)
	    lf_printf(file, "      unsigned %s;\n", field->name);
	}
	lf_printf(file, "    } %s;\n", form->name);
      }
      lf_printf(file, "  } crack;\n");
    }
    lf_printf(file, "} idecode_cache;\n");
  }

  /* define various fields according to the cache */
  if (idecode_cache <= 1) {
    extraction_rules *rule;
    lf_printf(file, "\n");
    for (rule = cachable_values;
	 rule->valid;
	 rule++) {
      if (rule->expression != NULL)
	lf_printf(file, "#define %s %s\n",
		  rule->new_name, rule->expression);
    }
  }

  lf_printf(file, "\n");
  lf_printf(file, "#endif /* _ICACHE_H_ */\n");
}




/****************************************************************/


void
lf_print_c_extraction(lf *file,
		      insn *instruction,
		      char *field_name,
		      char *field_type,
		      char *field_expression,
		      insn_field *cur_field,
		      insn_bits *bits,
		      int get_value_from_cache,
		      int put_value_in_cache)
{
  ASSERT(field_name != NULL);
  if (bits != NULL
      && (!bits->opcode->is_boolean || bits->value == 0)
      && strcmp(field_name, cur_field->val_string) == 0) {
    ASSERT(bits->field == cur_field);
    ASSERT(field_type == NULL);
    lf_print_c_line_nr(file, instruction->file_entry);
    lf_printf(file, "const unsigned %s = ",
	      field_name);
    if (bits->opcode->last < bits->field->last)
      lf_printf(file, "%d;\n",
		bits->value << (bits->field->last - bits->opcode->last));
    else
      lf_printf(file, "%d;\n", bits->value);
  }
  else {
    /* put the field in the local variable */
    lf_print_c_line_nr(file, instruction->file_entry);
    lf_printf(file, "%s const %s = ",
	      field_type == NULL ? "unsigned" : field_type,
	      field_name);
    /* getting it from the cache */
    if (get_value_from_cache || put_value_in_cache) {
      lf_printf(file, "cache_entry->crack.%s.%s",
		instruction->file_entry->fields[insn_form],
		field_name);
      if (put_value_in_cache) /* also put it in the cache? */
	lf_printf(file, " = ");
    }
    if (!get_value_from_cache) {
      if (strcmp(field_name, cur_field->val_string) == 0)
	lf_printf(file, "EXTRACTED32(instruction, %d, %d)",
		  cur_field->first, cur_field->last);
      else if (field_expression != NULL)
	lf_printf(file, "%s", field_expression);
      else
	lf_printf(file, "eval_%s", field_name);
    }
    lf_printf(file, ";\n");
  }
}


void
lf_print_c_extractions(lf *file,
		       insn *instruction,
		       insn_bits *expanded_bits,
		       int get_value_from_cache,
		       int put_value_in_cache)
{
  insn_field *cur_field;

  /* extract instruction fields */
  lf_printf(file, "/* extraction: %s */\n",
	    instruction->file_entry->fields[insn_format]);

  for (cur_field = instruction->fields->first;
       cur_field->first < insn_size;
       cur_field = cur_field->next) {
    if (cur_field->is_string) {
      insn_bits *bits;
      int found_rule = 0;
      /* find any corresponding value */
      for (bits = expanded_bits;
	   bits != NULL;
	   bits = bits->last) {
	if (bits->field == cur_field)
	  break;
      }
      /* try the cache rule table for what to do */
      if (get_value_from_cache || put_value_in_cache) {      
	extraction_rules *field_rule;
	for (field_rule = cachable_values;
	     field_rule->valid;
	     field_rule++) {
	  if (strcmp(cur_field->val_string, field_rule->old_name) == 0) {
	    found_rule = 1;
	    if (field_rule->valid > 1 && put_value_in_cache)
	      lf_print_c_extraction(file,
				    instruction,
				    field_rule->new_name,
				    field_rule->type,
				    field_rule->expression,
				    cur_field,
				    bits,
				    0,
				    0);
	    else if (field_rule->valid == 1)
	      lf_print_c_extraction(file,
				    instruction,
				    field_rule->new_name,
				    field_rule->type,
				    field_rule->expression,
				    cur_field,
				    bits,
				    get_value_from_cache,
				    put_value_in_cache);
	  }
	}
      }
      if (found_rule == 0)
	lf_print_c_extraction(file,
			      instruction,
			      cur_field->val_string,
			      0,
			      0,
			      cur_field,
			      bits,
			      get_value_from_cache,
			      put_value_in_cache);
      /* if any (XXX == 0), output a corresponding test */
      if (instruction->file_entry->annex != NULL) {
	char *field_name = cur_field->val_string;
	char *is_0_ptr = instruction->file_entry->annex;
	int field_len = strlen(field_name);
	if (strlen(is_0_ptr) >= (strlen("_is_0") + field_len)) {
	  is_0_ptr += field_len;
	  while ((is_0_ptr = strstr(is_0_ptr, "_is_0")) != NULL) {
	    if (strncmp(is_0_ptr - field_len, field_name, field_len) == 0
		&& !isalpha(is_0_ptr[ - field_len - 1])) {
	      lf_print_c_line_nr(file, instruction->file_entry);
	      lf_printf(file, "const unsigned %s_is_0 = (", field_name);
	      if (bits != NULL)
		lf_printf(file, "%d", bits->value);
	      else
		lf_printf(file, "%s", field_name);
	      lf_printf(file, " == 0);\n");
	      break;
	    }
	    is_0_ptr += strlen("_is_0");
	  }
	}
      }
      /* any thing else ... */
    }
  }
  lf_print_file_line_nr(file);
}

void
lf_print_c_validate(lf *file,
		    insn *instruction,
		    opcode_field *opcodes)
{
  unsigned check_mask = 0;
  unsigned check_val = 0;
  insn_field *field;
  opcode_field *opcode;

  for (field = instruction->fields->first;
       field->first < insn_size;
       field = field->next) {

    check_mask <<= field->width;
    check_val <<= field->width;

    /* is it a constant that could need validating? */
    if (!field->is_int && !field->is_slash)
      continue;

    /* has it been checked by a table? */
    for (opcode = opcodes; opcode != NULL; opcode = opcode->parent) {
      if (field->first >= opcode->first
	  && field->last <= opcode->last)
	break;
    }
    if (opcode != NULL)
      continue;

    check_mask |= (1 << field->width)-1;
    check_val |= field->val_int;
  }

  /* if any bits not checked by opcode tables, output code to check them */
  if (!it_is("illegal", instruction->file_entry->fields[insn_flags])
      && check_mask) {
    lf_printf(file, "\n");
    lf_printf(file, "/* validate: %s */\n",
	      instruction->file_entry->fields[insn_format]);
    lf_printf(file, "if ((instruction & 0x%x) != 0x%x) {\n",
	      check_mask, check_val);
    switch (idecode_cache) {
    case 0:
      lf_printf(file, "  return semantic_illegal(%s);\n", insn_actual);
      break;
    case 1:
      lf_printf(file, "  return semantic_illegal;\n");
      break;
    default:
      lf_printf(file, "  return idecode_illegal(%s);\n", cache_idecode_actual);
    }
    lf_printf(file, "}\n");
  }

}


void
lf_print_c_cracker(lf *file,
		   insn *instruction,
		   insn_bits *expanded_bits,
		   opcode_field *opcodes)
{

  /* function header */
  lf_printf(file, "{\n");
  lf_indent(file, +2);

  lf_print_my_prefix(file,
		     instruction->file_entry);

  lf_print_ptrace(file);

  lf_print_c_validate(file, instruction, opcodes);

  lf_printf(file, "\n");
  lf_printf(file, "{\n");
  lf_indent(file, +2);
  lf_print_c_extractions(file,
			 instruction,
			 expanded_bits,
			 0/*get_value_from_cache*/,
			 1/*put_value_in_cache*/);
  lf_indent(file, -2);
  lf_printf(file, "}\n");

  /* return the function propper (main sorts this one out) */
  lf_printf(file, "\n");
  lf_printf(file, "/* semantic routine */\n");
  lf_print_c_line_nr(file, instruction->file_entry);
  lf_printf(file, "return ");
  lf_print_function_name(file,
			 instruction,
			 expanded_bits,
			 function_name_prefix_semantics);
  lf_printf(file, ";\n");

  lf_print_file_line_nr(file);
  lf_indent(file, -2);
  lf_printf(file, "}\n");
}


void
lf_print_c_semantic(lf *file,
		    insn *instruction,
		    insn_bits *expanded_bits,
		    opcode_field *opcodes)
{

  lf_printf(file, "{\n");
  lf_indent(file, +2);

  lf_print_my_prefix(file,
		     instruction->file_entry);
  lf_putstr(file, insn_local);
  lf_printf(file, "\n");

  lf_printf(file, "\n");
  lf_print_c_extractions(file,
			 instruction,
			 expanded_bits,
			 idecode_cache > 1/*get_value_from_cache*/,
			 0/*put_value_in_cache*/);

  lf_print_ptrace(file);

  /* generate code to check previously unchecked fields */
  if (idecode_cache < 2)
    lf_print_c_validate(file, instruction, opcodes);

  /* if OEA and a floating point generate a check that fp is enabled */
  if (it_is("f", instruction->file_entry->fields[insn_flags])) {
    lf_printf(file, "\n");
    lf_printf(file, "/* verify FP is enabled */\n");
    lf_printf(file, "if (!IS_FP_AVAILABLE(processor))\n");
    lf_printf(file, "  floating_point_unavailable_interrupt(processor, cia);\n");
  }

  /* generate the code (or at least something */
  if (instruction->file_entry->annex != NULL) {
    /* true code */
    lf_printf(file, "\n");
    lf_print_c_line_nr(file, instruction->file_entry);
    lf_printf(file, "{\n");
    lf_indent(file, +2);
    lf_print_c_code(file, instruction->file_entry->annex);
    lf_indent(file, -2);
    lf_printf(file, "}\n");
    lf_print_file_line_nr(file);
  }
  else if (it_is("nop", instruction->file_entry->fields[insn_flags])) {
    lf_print_file_line_nr(file);
  }
  else if (it_is("f", instruction->file_entry->fields[insn_flags])) {
    /* unimplemented floating point - call for assistance */
    lf_printf(file, "\n");
    lf_print_c_line_nr(file, instruction->file_entry);
    lf_putstr(file, "floating_point_assist_interrupt(processor, cia);\n");
    lf_print_file_line_nr(file);
  }
  else {
    /* abort so it is implemented now */
    lf_print_c_line_nr(file, instruction->file_entry);
    lf_putstr(file, "error(\"%s: unimplemented, cia=0x%x\\n\", my_prefix, cia);\n");
    lf_print_file_line_nr(file);
    lf_printf(file, "\n");
  }

  /* the function footer */
  lf_printf(file, "return nia;\n");
  lf_indent(file, -2);
  lf_printf(file, "}\n");
}

void
lf_print_c_semantic_function(lf *file,
			     insn *instruction,
			     insn_bits *expanded_bits,
			     opcode_field *opcodes)
{

  /* build the semantic routine to execute the instruction */

  /* function header */
  lf_printf(file, "\n");
  lf_printf(file, "INLINE_SEMANTICS unsigned_word\n");
  lf_print_function_name(file,
			 instruction,
			 expanded_bits,
			 function_name_prefix_semantics);
  lf_printf(file, "\n(%s)\n",
	    idecode_cache > 1 ? cache_insn_formal : insn_formal);

  lf_print_c_semantic(file,
		      instruction,
		      expanded_bits,
		      opcodes);

}


void
semantics_c_leaf(insn_table *entry,
		 void *data,
		 int depth)
{
  lf *file = (lf*)data;
  ASSERT(entry->nr_insn == 1
	 && entry->opcode == NULL
	 && entry->parent != NULL
	 && entry->parent->opcode != NULL);
  lf_print_c_semantic_function(file,
			       entry->insns,
			       entry->expanded_bits,
			       entry->parent->opcode);
}

void
semantics_c_insn(insn_table *table,
		 void *data,
		 insn *instruction)
{
  lf *file = (lf*)data;
  lf_print_c_semantic_function(file, instruction,
			       NULL, NULL);
}



void 
gen_semantics_c(insn_table *table, lf *file)
{
  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _SEMANTICS_C_\n");
  lf_printf(file, "#define _SEMANTICS_C_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef STATIC_INLINE_SEMANTICS\n");
  lf_printf(file, "#define STATIC_INLINE_SEMANTICS STATIC_INLINE\n");
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");
  lf_printf(file, "#include \"cpu.h\"\n");
  lf_printf(file, "#include \"idecode.h\"\n");
  lf_printf(file, "#include \"semantics.h\"\n");
  lf_printf(file, "\n");

  if (idecode_expand_semantics)
    insn_table_traverse_tree(table,
			     file,
			     1,
			     NULL, /* start */
			     semantics_c_leaf,
			     NULL, /* end */
			     NULL); /* padding */
  else
    insn_table_traverse_insn(table,
			     file,
			     semantics_c_insn);

  lf_printf(file, "\n");
  lf_printf(file, "#endif /* _SEMANTICS_C_ */\n");
}


/****************************************************************/

void
gen_idecode_h(insn_table *table, lf *file)
{
  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _IDECODE_H_\n");
  lf_printf(file, "#define _IDECODE_H_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef INLINE_IDECODE\n");
  lf_printf(file, "#define INLINE_IDECODE\n");
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");
  lf_printf(file, "#include \"idecode_insn.h\"\n");
  lf_printf(file, "#include \"idecode_expression.h\"\n");
  lf_printf(file, "#include \"idecode_fields.h\"\n");
  lf_printf(file, "#include \"idecode_branch.h\"\n");
  lf_printf(file, "\n");
  lf_printf(file, "#include \"icache.h\"\n");
  lf_printf(file, "\n");
  lf_printf(file, "typedef unsigned_word idecode_semantic\n(%s);\n",
	    idecode_cache < 2 ? insn_formal : cache_insn_formal);
  lf_printf(file, "\n");
  if (!idecode_cache)
    lf_printf(file, "INLINE_IDECODE unsigned_word idecode_issue\n(%s);\n",
	      insn_formal);
  else if (idecode_cache)
    lf_printf(file, "INLINE_IDECODE idecode_semantic *idecode\n(%s);\n",
	      idecode_cache == 1 ? insn_formal : cache_idecode_formal);
  lf_printf(file, "\n");
  lf_printf(file, "#endif /* _IDECODE_H_ */\n");
}


/****************************************************************/


void
idecode_table_start(insn_table *table,
		    void *data,
		    int depth)
{
  lf *file = (lf*)data;
  ASSERT(depth == 0);
  /* start of the table */
  if (!table->opcode_rule->use_switch) {
    lf_printf(file, "\n");
    lf_printf(file, "static idecode_table_entry ");
    lf_print_table_name(file, table);
    lf_printf(file, "[] = {\n");
  }
}

void
idecode_table_leaf(insn_table *entry,
		   void *data,
		   int depth)
{
  lf *file = (lf*)data;
  ASSERT(entry->parent != NULL);
  ASSERT(depth == 0);

  /* add an entry to the table */
  if (!entry->parent->opcode_rule->use_switch) {
    if (entry->opcode == NULL) {
      /* table leaf entry */
      lf_printf(file, "  /*%d*/ { 0, 0, ", entry->opcode_nr);
      lf_print_function_name(file,
			     entry->insns,
			     entry->expanded_bits,
			     (idecode_cache < 2
			      ? function_name_prefix_semantics
			      : function_name_prefix_idecode));
      lf_printf(file, " },\n");
    }
    else if (entry->opcode_rule->use_switch) {
      /* table calling switch statement */
      lf_printf(file, "  /*%d*/ { -1, 0, ",
		entry->opcode_nr);
      lf_print_table_name(file, entry);
      lf_printf(file, " },\n");
    }
    else {
      /* table `calling' another table */
      lf_printf(file, "  /*%d*/ { ", entry->opcode_nr);
      if (entry->opcode->is_boolean)
	lf_printf(file, "MASK32(%d,%d), 0, ",
		  entry->opcode->first, entry->opcode->last);
      else
	lf_printf(file, "%d, MASK32(%d,%d), ",
		  insn_size - entry->opcode->last - 1,
		  entry->opcode->first, entry->opcode->last);
      lf_print_table_name(file, entry);
      lf_printf(file, " },\n");
    }
  }
}

void
idecode_table_end(insn_table *table,
		  void *data,
		  int depth)
{
  lf *file = (lf*)data;
  ASSERT(depth == 0);

  if (!table->opcode_rule->use_switch) {
    lf_printf(file, "};\n");
  }
}

void
idecode_table_padding(insn_table *table,
		      void *data,
		      int depth,
		      int opcode_nr)
{
  lf *file = (lf*)data;
  ASSERT(depth == 0);

  if (!table->opcode_rule->use_switch) {
    lf_printf(file, "  /*%d*/ { 0, 0, %s_illegal },\n",
	      opcode_nr, idecode_cache > 1 ? "idecode" : "semantic");
  }
}


/****************************************************************/


void lf_print_idecode_switch
(lf *file, 
 insn_table *table);


void
idecode_switch_start(insn_table *table,
		void *data,
		int depth)
{
  lf *file = (lf*)data;
  ASSERT(depth == 0);
  ASSERT(table->opcode_rule->use_switch);

  lf_printf(file, "switch (EXTRACTED32(instruction, %d, %d)) {\n",
	    table->opcode->first, table->opcode->last);
}


void
idecode_switch_leaf(insn_table *entry,
		    void *data,
		    int depth)
{
  lf *file = (lf*)data;
  ASSERT(entry->parent != NULL);
  ASSERT(depth == 0);
  ASSERT(entry->parent->opcode_rule->use_switch);

  lf_printf(file, "case %d:\n", entry->opcode_nr);
  lf_indent(file, +2);
  {
    if (entry->opcode == NULL) {
      /* switch calling leaf */
      lf_printf(file, "return ");
      lf_print_function_name(file,
			     entry->insns,
			     entry->expanded_bits,
			     (idecode_cache < 2
			      ? function_name_prefix_semantics
			      : function_name_prefix_idecode));
      if (!idecode_cache)
	lf_printf(file, "(%s);\n", insn_actual);
      else if (idecode_cache == 1)
	lf_printf(file, ";\n");
      else
	lf_printf(file, "(%s);\n", cache_idecode_actual);
    }
    else if (entry->opcode_rule->use_switch) {
      /* switch calling switch */
      lf_print_idecode_switch(file, entry);
    }
    else {
      /* switch calling table */
      lf_printf(file, "return ");
      lf_print_idecode_table(file, entry);
    }
    lf_printf(file, "break;\n");
  }
  lf_indent(file, -2);
}

lf_print_idecode_switch_illegal(lf *file)
{
  switch (idecode_cache) {
  case 0:
    lf_printf(file, "  return semantic_illegal(%s);\n", insn_actual);
    break;
  case 1:
    lf_printf(file, "  return semantic_illegal;\n");
    break;
  default:
    lf_printf(file, "  return idecode_illegal(%s);\n", cache_idecode_actual);
  }
  lf_printf(file, "  break;\n");
}

void
idecode_switch_end(insn_table *table,
		   void *data,
		   int depth)
{
  lf *file = (lf*)data;
  ASSERT(depth == 0);
  ASSERT(table->opcode_rule->use_switch);

  if (table->opcode_rule->use_switch == 1) {
    lf_printf(file, "default:\n");
    lf_print_idecode_switch_illegal(file);
  }
  lf_printf(file, "}\n");
}

void
idecode_switch_padding(insn_table *table,
		       void *data,
		       int depth,
		       int opcode_nr)
{
  lf *file = (lf*)data;

  ASSERT(depth == 0);
  ASSERT(table->opcode_rule->use_switch);

  if (table->opcode_rule->use_switch > 1) {
    lf_printf(file, "case %d:\n", opcode_nr);
    lf_print_idecode_switch_illegal(file);
  }
}


void
lf_print_idecode_switch(lf *file, 
			insn_table *table)
{
  insn_table_traverse_tree(table,
			   file,
			   0,
			   idecode_switch_start,
			   idecode_switch_leaf,
			   idecode_switch_end,
			   idecode_switch_padding);
}


void
idecode_expand_if_switch(insn_table *table,
			 void *data,
			 int depth)
{
  lf *file = (lf*)data;

  if (table->opcode_rule->use_switch
      && table->parent != NULL /* don't expand the top one yet */
      && !table->parent->opcode_rule->use_switch) {
    lf_printf(file, "\n");
    lf_printf(file, "STATIC_INLINE_IDECODE void\n");
    lf_print_table_name(file, table);
    lf_printf(file, "\n(%s)\n",
	      idecode_cache ? cache_idecode_formal : insn_formal);
    lf_printf(file, "{\n");
    {
      lf_indent(file, +2);
      lf_print_idecode_switch(file, table);
      lf_indent(file, -2);
    }
    lf_printf(file, "}\n");
  }
}


lf_print_c_cracker_function(lf *file,
			    insn *instruction,
			    insn_bits *expanded_bits,
			    opcode_field *opcodes)
{
  /* if needed, generate code to enter this routine into a cache */
  lf_printf(file, "\n");
  lf_printf(file, "STATIC_INLINE_IDECODE idecode_semantic *\n");
  lf_print_function_name(file,
			 instruction,
			 expanded_bits,
			 function_name_prefix_idecode);
  lf_printf(file, "\n(%s)\n", cache_idecode_formal);

  lf_print_c_cracker(file,
		     instruction,
		     expanded_bits,
		     opcodes);
}

void
idecode_crack_leaf(insn_table *entry,
		   void *data,
		   int depth)
{
  lf *file = (lf*)data;
  ASSERT(entry->nr_insn == 1
	 && entry->opcode == NULL
	 && entry->parent != NULL
	 && entry->parent->opcode != NULL);
  lf_print_c_cracker_function(file,
			      entry->insns,
			      entry->expanded_bits,
			      entry->opcode);
}

void
idecode_crack_insn(insn_table *entry,
		   void *data,
		   insn *instruction)
{
  lf *file = (lf*)data;
  lf_print_c_cracker_function(file,
			      instruction,
			      NULL,
			      NULL);
}

/****************************************************************/

gen_idecode_c(insn_table *table, lf *file)
{
  int depth;

  /* the intro */
  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _IDECODE_C_\n");
  lf_printf(file, "#define _IDECODE_C_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef STATIC_INLINE_IDECODE\n");
  lf_printf(file, "#define STATIC_INLINE_IDECODE STATIC_INLINE\n");
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");
  lf_printf(file, "#include \"cpu.h\"\n");
  lf_printf(file, "#include \"idecode.h\"\n");
  lf_printf(file, "#include \"semantics.h\"\n");
  lf_printf(file, "\n");
  lf_printf(file, "\n");
  lf_printf(file, "typedef idecode_semantic *idecode_crack\n(%s);\n",
	    idecode_cache > 1 ? cache_idecode_formal : insn_formal);
  lf_printf(file, "\n");
  lf_printf(file, "typedef struct _idecode_table_entry {\n");
  lf_printf(file, "  unsigned shift;\n");
  lf_printf(file, "  unsigned mask;\n");
  lf_printf(file, "  void *function_or_table;\n");
  lf_printf(file, "} idecode_table_entry;\n");
  lf_printf(file, "\n");
  lf_printf(file, "\n");

  /* output cracking functions where needed */
  if (idecode_cache > 1) {
    if (idecode_expand_semantics)
      insn_table_traverse_tree(table,
			       file,
			       1,
			       NULL,
			       idecode_crack_leaf,
			       NULL,
			       NULL);
    else
      insn_table_traverse_insn(table,
			       file,
			       idecode_crack_insn);
  }


  /* output tables where needed */
  for (depth = insn_table_depth(table);
       depth > 0;
       depth--) {
    insn_table_traverse_tree(table,
			     file,
			     1-depth,
			     idecode_table_start,
			     idecode_table_leaf,
			     idecode_table_end,
			     idecode_table_padding);
  }

  /* output switch functions where needed */
  insn_table_traverse_tree(table,
			   file,
			   1,
			   idecode_expand_if_switch, /* START */
			   NULL, NULL, NULL);

  /* output the main idecode routine */
  lf_printf(file, "\n");
  if (!idecode_cache)
    lf_printf(file, "INLINE_IDECODE unsigned_word\nidecode_issue\n(%s)\n",
	      insn_formal);
  else if (idecode_cache)
    lf_printf(file, "INLINE_IDECODE idecode_semantic *\nidecode\n(%s)\n",
	      idecode_cache == 1 ? insn_formal : cache_idecode_formal);
  lf_printf(file, "{\n");
  lf_indent(file, +2);
  if (!idecode_cache)
    lf_printf(file, "cpu_increment_number_of_insns (processor);\n");
  if (table->opcode_rule->use_switch)
    lf_print_idecode_switch(file, table);
  else
    lf_print_idecode_table(file, table);
  lf_indent(file, -2);
  lf_printf(file, "}\n");
  lf_printf(file, "\n");
  lf_printf(file, "#endif\n");
}


/****************************************************************/


typedef enum {
  spreg_name,
  spreg_reg_nr,
  spreg_readonly,
  spreg_length,
  nr_spreg_registers = file_table_max_fields
} spreg_fields;

typedef struct _spreg_table_entry spreg_table_entry;
struct _spreg_table_entry {
  char *name;
  int spreg_nr;
  int is_readonly;
  int length;
  file_table_entry *entry;
  spreg_table_entry *next;
};

typedef struct _spreg_table spreg_table;
struct _spreg_table {
  spreg_table_entry *sprs;
};

spreg_table_entry *
spreg_table_entry_new()
{
  spreg_table_entry *new_entry =
    (spreg_table_entry*)zmalloc(sizeof(spreg_table_entry));
  ASSERT(new_entry != NULL);
  return new_entry;
}

spreg_table *
spreg_table_new()
{
  spreg_table *new_table = (spreg_table*)zmalloc(sizeof(spreg_table));
  ASSERT(new_table != NULL);
  return new_table;
}

void
spreg_table_insert(spreg_table *table, file_table_entry *entry)
{
  /* create a new spr entry */
  spreg_table_entry *new_spr = spreg_table_entry_new();
  new_spr->next = NULL;
  new_spr->entry = entry;
  new_spr->spreg_nr = atoi(entry->fields[spreg_reg_nr]);
  new_spr->is_readonly = (entry->fields[spreg_readonly]
			  ? atoi(entry->fields[spreg_readonly])
			  : 0);
  new_spr->length = atoi(entry->fields[spreg_length]);
  new_spr->name = (char*)zmalloc(strlen(entry->fields[spreg_name]) + 1);
  ASSERT(new_spr->name != NULL);
  {
    int i;
    for (i = 0; entry->fields[spreg_name][i] != '\0'; i++) {
      if (isupper(entry->fields[spreg_name][i]))
	new_spr->name[i] = tolower(entry->fields[spreg_name][i]);
      else
	new_spr->name[i] = entry->fields[spreg_name][i];
    }
  }

  /* insert, by spreg_nr order */
  {
    spreg_table_entry **ptr_to_spreg_entry = &table->sprs;
    spreg_table_entry *spreg_entry = *ptr_to_spreg_entry;
    while (spreg_entry != NULL && spreg_entry->spreg_nr < new_spr->spreg_nr) {
      ptr_to_spreg_entry = &spreg_entry->next;
      spreg_entry = *ptr_to_spreg_entry;
    }
    ASSERT(spreg_entry == NULL || spreg_entry->spreg_nr != new_spr->spreg_nr);
    *ptr_to_spreg_entry = new_spr;
    new_spr->next = spreg_entry;
  }

}


spreg_table *
spreg_table_load(char *file_name)
{
  file_table *file = file_table_open(file_name);
  spreg_table *table = spreg_table_new();

  {
    file_table_entry *entry;
    while (entry = file_table_read(file)) {
      spreg_table_insert(table, entry);
    }
  }

  return table;
}


/****************************************************************/

char *spreg_attributes[] = {
  "is_valid",
  "is_readonly",
  "name",
  "index",
  "length",
  0
};

void
gen_spreg_h(spreg_table *table, lf *file)
{
  spreg_table_entry *entry;
  char **attribute;

  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _SPREG_H_\n");
  lf_printf(file, "#define _SPREG_H_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef INLINE_SPREG\n");
  lf_printf(file, "#define INLINE_SPREG\n");
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");
  lf_printf(file, "typedef unsigned_word spreg;\n");
  lf_printf(file, "\n");
  lf_printf(file, "typedef enum {\n");

  for (entry = table->sprs;
       entry != NULL ;
       entry = entry->next) {
    lf_printf(file, "  spr_%s = %d,\n", entry->name, entry->spreg_nr);
  }

  lf_printf(file, "  nr_of_sprs = %d\n", nr_of_sprs);
  lf_printf(file, "} sprs;\n");
  lf_printf(file, "\n");
  for (attribute = spreg_attributes;
       *attribute != NULL;
       attribute++) {
    if (strcmp(*attribute, "name") == 0)
      lf_printf(file, "INLINE_SPREG char *spr_%s(sprs spr);\n",
		*attribute);
    else
      lf_printf(file, "INLINE_SPREG int spr_%s(sprs spr);\n",
		*attribute);
  }
  lf_printf(file, "\n");
  lf_printf(file, "#endif /* _SPREG_H_ */\n");
}


void
gen_spreg_c(spreg_table *table, lf *file)
{
  spreg_table_entry *entry;
  char **attribute;
  int spreg_nr;

  lf_print_copyleft(file);
  lf_printf(file, "\n");
  lf_printf(file, "#ifndef _SPREG_C_\n");
  lf_printf(file, "#define _SPREG_C_\n");
  lf_printf(file, "\n");
  lf_printf(file, "#include \"words.h\"\n");
  lf_printf(file, "#include \"spreg.h\"\n");

  lf_printf(file, "\n");
  lf_printf(file, "typedef struct _spreg_info {\n");
  lf_printf(file, "  char *name;\n");
  lf_printf(file, "  int is_valid;\n");
  lf_printf(file, "  int length;\n");
  lf_printf(file, "  int is_readonly;\n");
  lf_printf(file, "  int index;\n");
  lf_printf(file, "} spreg_info;\n");
  lf_printf(file, "\n");
  lf_printf(file, "static spreg_info spr_info[nr_of_sprs+1] = {\n");
  entry = table->sprs;
  for (spreg_nr = 0; spreg_nr < nr_of_sprs+1; spreg_nr++) {
    if (entry == NULL || spreg_nr < entry->spreg_nr)
      lf_printf(file, "  { 0, 0, 0, 0, %d},\n", spreg_nr);
    else {
      lf_printf(file, "  { \"%s\", %d, %d, %d, spr_%s /*%d*/ },\n",
		entry->name, 1, entry->length, entry->is_readonly,
		entry->name, entry->spreg_nr);
      entry = entry->next;
    }
  }
  lf_printf(file, "};\n");

  for (attribute = spreg_attributes;
       *attribute != NULL;
       attribute++) {
    lf_printf(file, "\n");
    if (strcmp(*attribute, "name") == 0)
      lf_printf(file, "INLINE_SPREG char *\n");
    else
      lf_printf(file, "INLINE_SPREG int\n");
    lf_printf(file, "spr_%s(sprs spr)\n", *attribute);
    lf_printf(file, "{\n");
    if (spreg_lookup_table
	|| strcmp(*attribute, "name") == 0
	|| strcmp(*attribute, "index") == 0)
      lf_printf(file, "  return spr_info[spr].%s;\n",
		*attribute);
    else {
      spreg_table_entry *entry;
      lf_printf(file, "  switch (spr) {\n");
      for (entry = table->sprs; entry != NULL; entry = entry->next) {
	lf_printf(file, "  case %d:\n", entry->spreg_nr);
	if (strcmp(*attribute, "is_valid") == 0)
	  lf_printf(file, "    return 1;\n");
	else if (strcmp(*attribute, "is_readonly") == 0)
	  lf_printf(file, "    return %d;\n", entry->is_readonly);
	else if (strcmp(*attribute, "length") == 0)
	  lf_printf(file, "    return %d;\n", entry->length);
	else
	  ASSERT(0);
      }
      lf_printf(file, "  default:\n");
      lf_printf(file, "    return 0;\n");
      lf_printf(file, "  }\n");
    }
    lf_printf(file, "}\n");
  }

  lf_printf(file, "\n");
  lf_printf(file, "#endif /* _SPREG_C_ */\n");
}



/****************************************************************/


int
main(int argc,
     char **argv,
     char **envp)
{
  insn_table *instructions = NULL;
  spreg_table *sprs = NULL;
  icache_tree *cache_fields = NULL;
  int ch;

  while ((ch = getopt(argc, argv, "i:I:r:S:s:D:d:P:p:C:")) != -1) {
    fprintf(stderr, "\t-%c %s\n", ch, optarg);
    switch(ch) {
    case 'I':
    case 'i':
      instructions = insn_table_load_insns(optarg);
      fprintf(stderr, "\texpanding ...\n");
      insn_table_expand_insns(instructions);
      fprintf(stderr, "\tcache fields ...\n");
      cache_fields = insn_table_cache_fields(instructions);
      if (ch == 'I') {
	dump_traverse(instructions);
	dump_insn_table(instructions, 0, 1);
      }
      break;
    case 'r':
      sprs = spreg_table_load(optarg);
      break;
    default:
      {
	lf *file = lf_open(optarg);
	switch (ch) {
	case 'S':
	  gen_semantics_h(instructions, file);
	  break;
	case 's':
	  gen_semantics_c(instructions, file);
	  break;
	case 'P':
	  gen_spreg_h(sprs, file);
	  break;
	case 'p':
	  gen_spreg_c(sprs, file);
	  break;
	case 'D':
	  gen_idecode_h(instructions, file);
	  break;
	case 'd':
	  gen_idecode_c(instructions, file);
	  break;
	case 'C':
	  gen_icache_h(cache_fields, file);
	  break;
	}
	lf_close(file);
      }
    }
  }
  return 0;
}
