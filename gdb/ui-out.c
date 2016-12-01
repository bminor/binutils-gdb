/* Output generating routines for GDB.

   Copyright (C) 1999-2016 Free Software Foundation, Inc.

   Contributed by Cygnus Solutions.
   Written by Fernando Nasser for Cygnus.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "expression.h"		/* For language.h */
#include "language.h"
#include "ui-out.h"

#include <vector>
#include <memory>
#include <string>

/* A header of a ui_out_table.  */

class ui_out_hdr
{
 public:

  explicit ui_out_hdr (int number, int min_width, ui_align alignment,
		       const std::string &name, const std::string &header)
  : m_number (number),
    m_min_width (min_width),
    m_alignment (alignment),
    m_name (name),
    m_header (header)
  {
  }

  int number () const
  {
    return m_number;
  }

  int min_width () const
  {
    return m_min_width;
  }

  ui_align alignment () const
  {
    return m_alignment;
  }

  const std::string &header () const
  {
    return m_header;
  }

  const std::string &name () const
  {
    return m_name;
  }

 private:

  /* The number of the table column this header represents, 1-based.  */
  int m_number;

  /* Minimal column width in characters.  May or may not be applicable,
     depending on the actual implementation of ui_out.  */
  int m_min_width;

  /* Alignment of the content in the column.  May or may not be applicable,
     depending on the actual implementation of ui_out.  */
  ui_align m_alignment;

  /* Internal column name, used to internally refer to the column.  */
  std::string m_name;

  /* Printed header text of the column.  */
  std::string m_header;
};

struct ui_out_level
  {
    /* Count each field; the first element is for non-list fields.  */
    int field_count;
    /* The type of this level.  */
    enum ui_out_type type;
  };


/* Tables are special.  Maintain a separate structure that tracks
   their state.  At present an output can only contain a single table
   but that restriction might eventually be lifted.  */

struct ui_out_table
{
  /* If on, a table is being generated.  */
  int flag;

  /* If on, the body of a table is being generated.  If off, the table
     header is being generated.  */
  int body_flag;

  /* The level at which each entry of the table is to be found.  A row
     (a tuple) is made up of entries.  Consequently ENTRY_LEVEL is one
     above that of the table.  */
  int entry_level;

  /* Number of table columns (as specified in the table_begin call).  */
  int columns;

  /* String identifying the table (as specified in the table_begin
     call).  */
  std::string id;

  /* Pointers to the column headers.  */
  std::vector<std::unique_ptr<ui_out_hdr>> headers;

  /* Iterator over the headers vector, used when printing successive fields.  */
  std::vector<std::unique_ptr<ui_out_hdr>>::const_iterator headers_iterator;

};


/* The ui_out structure */

struct ui_out
  {
    int flags;
    /* Specific implementation of ui-out.  */
    const struct ui_out_impl *impl;
    void *data;

    /* Current level.  */
    int level;

    /* Vector to store and track the ui-out levels.  */
    std::vector<std::unique_ptr<ui_out_level>> levels;

    /* A table, if any.  At present only a single table is supported.  */
    struct ui_out_table table;
  };

/* The current (inner most) level.  */
static struct ui_out_level *
current_level (struct ui_out *uiout)
{
  return uiout->levels[uiout->level].get ();
}

/* Create a new level, of TYPE.  Return the new level's index.  */
static int
push_level (struct ui_out *uiout,
	    enum ui_out_type type)
{
  std::unique_ptr<ui_out_level> current (new ui_out_level ());

  current->field_count = 0;
  current->type = type;

  uiout->level++;
  uiout->levels.push_back (std::move (current));

  return uiout->level;
}

/* Discard the current level, return the discarded level's index.
   TYPE is the type of the level being discarded.  */
static int
pop_level (struct ui_out *uiout,
	   enum ui_out_type type)
{
  /* We had better not underflow the buffer.  */
  gdb_assert (uiout->level > 0);
  gdb_assert (current_level (uiout)->type == type);

  uiout->levels.pop_back ();
  uiout->level--;

  return uiout->level + 1;
}

/* These are the interfaces to implementation functions.  */

static void uo_table_begin (struct ui_out *uiout, int nbrofcols,
			    int nr_rows, const char *tblid);
static void uo_table_body (struct ui_out *uiout);
static void uo_table_end (struct ui_out *uiout);
static void uo_table_header (struct ui_out *uiout, int width,
			     enum ui_align align,
			     const std::string &col_name,
			     const std::string &col_hdr);
static void uo_begin (struct ui_out *uiout,
		      enum ui_out_type type,
		      int level, const char *id);
static void uo_end (struct ui_out *uiout,
		    enum ui_out_type type,
		    int level);
static void uo_field_int (struct ui_out *uiout, int fldno, int width,
			  enum ui_align align, const char *fldname, int value);
static void uo_field_skip (struct ui_out *uiout, int fldno, int width,
			   enum ui_align align, const char *fldname);
static void uo_field_fmt (struct ui_out *uiout, int fldno, int width,
			  enum ui_align align, const char *fldname,
			  const char *format, va_list args)
     ATTRIBUTE_PRINTF (6, 0);
static void uo_spaces (struct ui_out *uiout, int numspaces);
static void uo_text (struct ui_out *uiout, const char *string);
static void uo_message (struct ui_out *uiout,
			const char *format, va_list args)
     ATTRIBUTE_PRINTF (2, 0);
static void uo_wrap_hint (struct ui_out *uiout, const char *identstring);
static void uo_flush (struct ui_out *uiout);
static int uo_redirect (struct ui_out *uiout, struct ui_file *outstream);

/* Prototypes for local functions */

static void append_header_to_list (struct ui_out *uiout, int width,
				   enum ui_align alignment,
				   const std::string &col_name,
				   const std::string &col_hdr);
static int get_next_header (struct ui_out *uiout, int *colno, int *width,
			    enum ui_align *alignment, const char **col_hdr);
static void clear_header_list (struct ui_out *uiout);
static void clear_table (struct ui_out *uiout);
static void verify_field (struct ui_out *uiout, int *fldno, int *width,
			  enum ui_align *align);

/* exported functions (ui_out API) */

/* Mark beginning of a table.  */

static void
ui_out_table_begin (struct ui_out *uiout, int nbrofcols,
		    int nr_rows, const std::string &tblid)
{
  if (uiout->table.flag)
    internal_error (__FILE__, __LINE__,
		    _("tables cannot be nested; table_begin found before \
previous table_end."));

  uiout->table.flag = 1;
  uiout->table.body_flag = 0;
  uiout->table.entry_level = uiout->level + 1;
  uiout->table.columns = nbrofcols;
  uiout->table.id = tblid;

  clear_header_list (uiout);

  uo_table_begin (uiout, nbrofcols, nr_rows, uiout->table.id.c_str ());
}

void
ui_out_table_body (struct ui_out *uiout)
{
  if (!uiout->table.flag)
    internal_error (__FILE__, __LINE__,
		    _("table_body outside a table is not valid; it must be \
after a table_begin and before a table_end."));
  if (uiout->table.body_flag)
    internal_error (__FILE__, __LINE__,
		    _("extra table_body call not allowed; there must be \
only one table_body after a table_begin and before a table_end."));
  if (uiout->table.headers.size () != uiout->table.columns)
    internal_error (__FILE__, __LINE__,
		    _("number of headers differ from number of table \
columns."));

  uiout->table.body_flag = 1;

  uo_table_body (uiout);
}

static void
ui_out_table_end (struct ui_out *uiout)
{
  if (!uiout->table.flag)
    internal_error (__FILE__, __LINE__,
		    _("misplaced table_end or missing table_begin."));

  uiout->table.entry_level = 0;
  uiout->table.body_flag = 0;
  uiout->table.flag = 0;

  uo_table_end (uiout);
  clear_table (uiout);
}

void
ui_out_table_header (struct ui_out *uiout, int width, enum ui_align alignment,
		     const std::string &col_name, const std::string &col_hdr)
{
  if (!uiout->table.flag || uiout->table.body_flag)
    internal_error (__FILE__, __LINE__,
		    _("table header must be specified after table_begin \
and before table_body."));

  append_header_to_list (uiout, width, alignment, col_name, col_hdr);

  uo_table_header (uiout, width, alignment, col_name, col_hdr);
}

static void
do_cleanup_table_end (void *data)
{
  struct ui_out *ui_out = (struct ui_out *) data;

  ui_out_table_end (ui_out);
}

struct cleanup *
make_cleanup_ui_out_table_begin_end (struct ui_out *ui_out, int nr_cols,
                                     int nr_rows, const char *tblid)
{
  ui_out_table_begin (ui_out, nr_cols, nr_rows, tblid);
  return make_cleanup (do_cleanup_table_end, ui_out);
}

void
ui_out_begin (struct ui_out *uiout,
	      enum ui_out_type type,
	      const char *id)
{
  int new_level;

  if (uiout->table.flag && !uiout->table.body_flag)
    internal_error (__FILE__, __LINE__,
		    _("table header or table_body expected; lists must be \
specified after table_body."));

  /* Be careful to verify the ``field'' before the new tuple/list is
     pushed onto the stack.  That way the containing list/table/row is
     verified and not the newly created tuple/list.  This verification
     is needed (at least) for the case where a table row entry
     contains either a tuple/list.  For that case bookkeeping such as
     updating the column count or advancing to the next heading still
     needs to be performed.  */
  {
    int fldno;
    int width;
    enum ui_align align;

    verify_field (uiout, &fldno, &width, &align);
  }

  new_level = push_level (uiout, type);

  /* If the push puts us at the same level as a table row entry, we've
     got a new table row.  Put the header pointer back to the start.  */
  if (uiout->table.body_flag
      && uiout->table.entry_level == new_level)
    uiout->table.headers_iterator = uiout->table.headers.begin ();

  uo_begin (uiout, type, new_level, id);
}

void
ui_out_end (struct ui_out *uiout,
	    enum ui_out_type type)
{
  int old_level = pop_level (uiout, type);

  uo_end (uiout, type, old_level);
}

struct ui_out_end_cleanup_data
{
  struct ui_out *uiout;
  enum ui_out_type type;
};

static void
do_cleanup_end (void *data)
{
  struct ui_out_end_cleanup_data *end_cleanup_data
    = (struct ui_out_end_cleanup_data *) data;

  ui_out_end (end_cleanup_data->uiout, end_cleanup_data->type);
  xfree (end_cleanup_data);
}

static struct cleanup *
make_cleanup_ui_out_end (struct ui_out *uiout,
			 enum ui_out_type type)
{
  struct ui_out_end_cleanup_data *end_cleanup_data;

  end_cleanup_data = XNEW (struct ui_out_end_cleanup_data);
  end_cleanup_data->uiout = uiout;
  end_cleanup_data->type = type;
  return make_cleanup (do_cleanup_end, end_cleanup_data);
}

struct cleanup *
make_cleanup_ui_out_tuple_begin_end (struct ui_out *uiout,
				     const char *id)
{
  ui_out_begin (uiout, ui_out_type_tuple, id);
  return make_cleanup_ui_out_end (uiout, ui_out_type_tuple);
}

struct cleanup *
make_cleanup_ui_out_list_begin_end (struct ui_out *uiout,
				    const char *id)
{
  ui_out_begin (uiout, ui_out_type_list, id);
  return make_cleanup_ui_out_end (uiout, ui_out_type_list);
}

void
ui_out_field_int (struct ui_out *uiout,
		  const char *fldname,
		  int value)
{
  int fldno;
  int width;
  enum ui_align align;

  verify_field (uiout, &fldno, &width, &align);

  uo_field_int (uiout, fldno, width, align, fldname, value);
}

void
ui_out_field_fmt_int (struct ui_out *uiout,
                      int input_width,
                      enum ui_align input_align,
		      const char *fldname,
		      int value)
{
  int fldno;
  int width;
  enum ui_align align;

  verify_field (uiout, &fldno, &width, &align);

  uo_field_int (uiout, fldno, input_width, input_align, fldname, value);
}

/* Documented in ui-out.h.  */

void
ui_out_field_core_addr (struct ui_out *uiout,
			const char *fldname,
			struct gdbarch *gdbarch,
			CORE_ADDR address)
{
  ui_out_field_string (uiout, fldname,
		       print_core_address (gdbarch, address));
}

void
ui_out_field_stream (struct ui_out *uiout,
		     const char *fldname,
		     struct ui_file *stream)
{
  std::string buffer = ui_file_as_string (stream);

  if (!buffer.empty ())
    ui_out_field_string (uiout, fldname, buffer.c_str ());
  else
    ui_out_field_skip (uiout, fldname);
  ui_file_rewind (stream);
}

/* Used to omit a field.  */

void
ui_out_field_skip (struct ui_out *uiout,
		   const char *fldname)
{
  int fldno;
  int width;
  enum ui_align align;

  verify_field (uiout, &fldno, &width, &align);

  uo_field_skip (uiout, fldno, width, align, fldname);
}

void
ui_out_field_string (struct ui_out *uiout,
		     const char *fldname,
		     const char *string)
{
  int fldno;
  int width;
  enum ui_align align;

  verify_field (uiout, &fldno, &width, &align);

  uo_field_string (uiout, fldno, width, align, fldname, string);
}

/* VARARGS */
void
ui_out_field_fmt (struct ui_out *uiout,
		  const char *fldname,
		  const char *format, ...)
{
  va_list args;
  int fldno;
  int width;
  enum ui_align align;

  /* Will not align, but has to call anyway.  */
  verify_field (uiout, &fldno, &width, &align);

  va_start (args, format);

  uo_field_fmt (uiout, fldno, width, align, fldname, format, args);

  va_end (args);
}

void
ui_out_spaces (struct ui_out *uiout, int numspaces)
{
  uo_spaces (uiout, numspaces);
}

void
ui_out_text (struct ui_out *uiout,
	     const char *string)
{
  uo_text (uiout, string);
}

void
ui_out_message (struct ui_out *uiout, const char *format, ...)
{
  va_list args;

  va_start (args, format);
  uo_message (uiout, format, args);
  va_end (args);
}

void
ui_out_wrap_hint (struct ui_out *uiout, const char *identstring)
{
  uo_wrap_hint (uiout, identstring);
}

void
ui_out_flush (struct ui_out *uiout)
{
  uo_flush (uiout);
}

int
ui_out_redirect (struct ui_out *uiout, struct ui_file *outstream)
{
  return uo_redirect (uiout, outstream);
}

/* Test the flags against the mask given.  */
int
ui_out_test_flags (struct ui_out *uiout, int mask)
{
  return (uiout->flags & mask);
}

int
ui_out_is_mi_like_p (struct ui_out *uiout)
{
  return uiout->impl->is_mi_like_p;
}

/* Interface to the implementation functions.  */

void
uo_table_begin (struct ui_out *uiout, int nbrofcols,
		int nr_rows,
		const char *tblid)
{
  if (!uiout->impl->table_begin)
    return;
  uiout->impl->table_begin (uiout, nbrofcols, nr_rows, tblid);
}

void
uo_table_body (struct ui_out *uiout)
{
  if (!uiout->impl->table_body)
    return;
  uiout->impl->table_body (uiout);
}

void
uo_table_end (struct ui_out *uiout)
{
  if (!uiout->impl->table_end)
    return;
  uiout->impl->table_end (uiout);
}

void
uo_table_header (struct ui_out *uiout, int width, enum ui_align align,
		 const std::string &col_name, const std::string &col_hdr)
{
  if (!uiout->impl->table_header)
    return;
  uiout->impl->table_header (uiout, width, align, col_name, col_hdr);
}

/* Clear the table associated with UIOUT.  */

static void
clear_table (struct ui_out *uiout)
{
  uiout->table.id.clear ();
  clear_header_list (uiout);
}

void
uo_begin (struct ui_out *uiout,
	  enum ui_out_type type,
	  int level,
	  const char *id)
{
  if (uiout->impl->begin == NULL)
    return;
  uiout->impl->begin (uiout, type, level, id);
}

void
uo_end (struct ui_out *uiout,
	enum ui_out_type type,
	int level)
{
  if (uiout->impl->end == NULL)
    return;
  uiout->impl->end (uiout, type, level);
}

void
uo_field_int (struct ui_out *uiout, int fldno, int width, enum ui_align align,
	      const char *fldname,
	      int value)
{
  if (!uiout->impl->field_int)
    return;
  uiout->impl->field_int (uiout, fldno, width, align, fldname, value);
}

void
uo_field_skip (struct ui_out *uiout, int fldno, int width, enum ui_align align,
	       const char *fldname)
{
  if (!uiout->impl->field_skip)
    return;
  uiout->impl->field_skip (uiout, fldno, width, align, fldname);
}

void
uo_field_string (struct ui_out *uiout, int fldno, int width,
		 enum ui_align align,
		 const char *fldname,
		 const char *string)
{
  if (!uiout->impl->field_string)
    return;
  uiout->impl->field_string (uiout, fldno, width, align, fldname, string);
}

void
uo_field_fmt (struct ui_out *uiout, int fldno, int width, enum ui_align align,
	      const char *fldname,
	      const char *format,
	      va_list args)
{
  if (!uiout->impl->field_fmt)
    return;
  uiout->impl->field_fmt (uiout, fldno, width, align, fldname, format, args);
}

void
uo_spaces (struct ui_out *uiout, int numspaces)
{
  if (!uiout->impl->spaces)
    return;
  uiout->impl->spaces (uiout, numspaces);
}

void
uo_text (struct ui_out *uiout,
	 const char *string)
{
  if (!uiout->impl->text)
    return;
  uiout->impl->text (uiout, string);
}

void
uo_message (struct ui_out *uiout,
	    const char *format,
	    va_list args)
{
  if (!uiout->impl->message)
    return;
  uiout->impl->message (uiout, format, args);
}

void
uo_wrap_hint (struct ui_out *uiout, const char *identstring)
{
  if (!uiout->impl->wrap_hint)
    return;
  uiout->impl->wrap_hint (uiout, identstring);
}

void
uo_flush (struct ui_out *uiout)
{
  if (!uiout->impl->flush)
    return;
  uiout->impl->flush (uiout);
}

int
uo_redirect (struct ui_out *uiout, struct ui_file *outstream)
{
  if (!uiout->impl->redirect)
    return -1;
  return uiout->impl->redirect (uiout, outstream);
}

/* local functions */

/* List of column headers manipulation routines.  */

static void
clear_header_list (struct ui_out *uiout)
{
  uiout->table.headers.clear ();
  uiout->table.headers_iterator = uiout->table.headers.end ();
}

static void
append_header_to_list (struct ui_out *uiout,
		       int width,
		       enum ui_align alignment,
		       const std::string &col_name,
		       const std::string &col_hdr)
{
  std::unique_ptr<ui_out_hdr> temphdr(
    new ui_out_hdr (uiout->table.headers.size () + 1, width,
		    alignment, col_name, col_hdr));

  uiout->table.headers.push_back (std::move (temphdr));
}

/* Extract the format information for the NEXT header and advance
   the header pointer.  Return 0 if there was no next header.  */

static int
get_next_header (struct ui_out *uiout,
		 int *colno,
		 int *width,
		 enum ui_align *alignment,
		 const char **col_hdr)
{
  /* There may be no headers at all or we may have used all columns.  */
  if (uiout->table.headers_iterator == uiout->table.headers.end ())
    return 0;

  ui_out_hdr *hdr = uiout->table.headers_iterator->get ();

  *colno = hdr->number ();
  *width = hdr->min_width ();
  *alignment = hdr->alignment ();
  *col_hdr = hdr->header ().c_str ();

  /* Advance the header pointer to the next entry.  */
  uiout->table.headers_iterator++;

  return 1;
}


/* Verify that the field/tuple/list is correctly positioned.  Return
   the field number and corresponding alignment (if
   available/applicable).  */

static void
verify_field (struct ui_out *uiout, int *fldno, int *width,
	      enum ui_align *align)
{
  struct ui_out_level *current = current_level (uiout);
  const char *text;

  if (uiout->table.flag)
    {
      if (!uiout->table.body_flag)
	internal_error (__FILE__, __LINE__,
			_("table_body missing; table fields must be \
specified after table_body and inside a list."));
      /* NOTE: cagney/2001-12-08: There was a check here to ensure
	 that this code was only executed when uiout->level was
	 greater than zero.  That no longer applies - this code is run
	 before each table row tuple is started and at that point the
	 level is zero.  */
    }

  current->field_count += 1;

  if (uiout->table.body_flag
      && uiout->table.entry_level == uiout->level
      && get_next_header (uiout, fldno, width, align, &text))
    {
      if (*fldno != current->field_count)
	internal_error (__FILE__, __LINE__,
			_("ui-out internal error in handling headers."));
    }
  else
    {
      *width = 0;
      *align = ui_noalign;
      *fldno = current->field_count;
    }
}


/* Access to ui-out members data.  */

void *
ui_out_data (struct ui_out *uiout)
{
  return uiout->data;
}

/* Access table field parameters.  */
int
ui_out_query_field (struct ui_out *uiout, int colno,
		    int *width, int *alignment, const char **col_name)
{
  if (!uiout->table.flag)
    return 0;

  /* Column numbers are 1-based, so convert to 0-based index.  */
  int index = colno - 1;

  if (index >= 0 && index < uiout->table.headers.size ())
    {
      ui_out_hdr *hdr = uiout->table.headers[index].get ();

      gdb_assert (colno == hdr->number ());

      *width = hdr->min_width ();
      *alignment = hdr->alignment ();
      *col_name = hdr->name ().c_str ();

      return 1;
    }
  else
    return 0;
}

/* Initialize private members at startup.  */

struct ui_out *
ui_out_new (const struct ui_out_impl *impl, void *data,
	    int flags)
{
  struct ui_out *uiout = new ui_out ();
  std::unique_ptr<ui_out_level> current (new ui_out_level ());

  uiout->data = data;
  uiout->impl = impl;
  uiout->flags = flags;
  uiout->table.flag = 0;
  uiout->table.body_flag = 0;
  uiout->level = 0;

  /* Create uiout->level 0, the default level.  */
  current->type = ui_out_type_tuple;
  current->field_count = 0;
  uiout->levels.push_back (std::move (current));

  uiout->table.headers_iterator = uiout->table.headers.end ();

  return uiout;
}
