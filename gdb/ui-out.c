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

/* A level of nesting (either a list or a tuple) in a ui_out output.  */

class ui_out_level
{
 public:

  explicit ui_out_level (ui_out_type type)
  : m_type (type),
    m_field_count (0)
  {
  }

  ui_out_type type () const
  {
    return m_type;
  }

  int field_count () const
  {
    return m_field_count;
  }

  void inc_field_count ()
  {
    m_field_count++;
  }

 private:

  /* The type of this level.  */
  ui_out_type m_type;

  /* Count each field; the first element is for non-list fields.  */
  int m_field_count;
};

/* Tables are special.  Maintain a separate structure that tracks
   their state.  At present an output can only contain a single table
   but that restriction might eventually be lifted.  */

class ui_out_table
{
 public:

  /* States (steps) of a table generation.  */

  enum class state
  {
    /* We are generating the table headers.  */
    HEADERS,

    /* We are generating the table body.  */
    BODY,
  };

  explicit ui_out_table (int entry_level, int nr_cols, const std::string &id)
  : m_state (state::HEADERS),
    m_entry_level (entry_level),
    m_nr_cols (nr_cols),
    m_id (id)
  {
  }

  /* Start building the body of the table.  */

  void start_body ();

  /* Add a new header to the table.  */

  void append_header (int width, ui_align alignment,
		      const std::string &col_name, const std::string &col_hdr);

  void start_row ();

  /* Extract the format information for the next header and advance
     the header iterator.  Return false if there was no next header.  */

  bool get_next_header (int *colno, int *width, ui_align *alignment,
		       const char **col_hdr);

  bool query_field (int colno, int *width, int *alignment,
		    const char **col_name) const;

  state current_state () const;

  int entry_level () const;

 private:

  state m_state;

  /* The level at which each entry of the table is to be found.  A row
     (a tuple) is made up of entries.  Consequently ENTRY_LEVEL is one
     above that of the table.  */
  int m_entry_level;

  /* Number of table columns (as specified in the table_begin call).  */
  int m_nr_cols;

  /* String identifying the table (as specified in the table_begin
     call).  */
  std::string m_id;

  /* Pointers to the column headers.  */
  std::vector<std::unique_ptr<ui_out_hdr>> m_headers;

  /* Iterator over the headers vector, used when printing successive fields.  */
  std::vector<std::unique_ptr<ui_out_hdr>>::const_iterator m_headers_iterator;
};

/* See ui-out.h.  */

void ui_out_table::start_body ()
{
  if (m_state != state::HEADERS)
    internal_error (__FILE__, __LINE__,
		    _("extra table_body call not allowed; there must be only "
		      "one table_body after a table_begin and before a "
		      "table_end."));

  /* Check if the number of defined headers matches the number of expected
     columns.  */
  if (m_headers.size () != m_nr_cols)
    internal_error (__FILE__, __LINE__,
		    _("number of headers differ from number of table "
		      "columns."));

  m_state = state::BODY;
  m_headers_iterator = m_headers.begin ();
}

/* See ui-out.h.  */

void ui_out_table::append_header (int width, ui_align alignment,
				  const std::string &col_name,
				  const std::string &col_hdr)
{
  if (m_state != state::HEADERS)
    internal_error (__FILE__, __LINE__,
		    _("table header must be specified after table_begin and "
		      "before table_body."));

  std::unique_ptr<ui_out_hdr> header (new ui_out_hdr (m_headers.size () + 1,
							width, alignment,
							col_name, col_hdr));

  m_headers.push_back (std::move (header));
}

/* See ui-out.h.  */

void ui_out_table::start_row ()
{
  m_headers_iterator = m_headers.begin ();
}

/* See ui-out.h.  */

bool ui_out_table::get_next_header (int *colno, int *width, ui_align *alignment,
				    const char **col_hdr)
{
  /* There may be no headers at all or we may have used all columns.  */
  if (m_headers_iterator == m_headers.end ())
    return false;

  ui_out_hdr *hdr = m_headers_iterator->get ();

  *colno = hdr->number ();
  *width = hdr->min_width ();
  *alignment = hdr->alignment ();
  *col_hdr = hdr->header ().c_str ();

  /* Advance the header pointer to the next entry.  */
  m_headers_iterator++;

  return true;
}

/* See ui-out.h.  */

bool ui_out_table::query_field (int colno, int *width, int *alignment,
				const char **col_name) const
{
  /* Column numbers are 1-based, so convert to 0-based index.  */
  int index = colno - 1;

  if (index >= 0 && index < m_headers.size ())
    {
      ui_out_hdr *hdr = m_headers[index].get ();

      gdb_assert (colno == hdr->number ());

      *width = hdr->min_width ();
      *alignment = hdr->alignment ();
      *col_name = hdr->name ().c_str ();

      return true;
    }
  else
    return false;
}

/* See ui-out.h.  */

ui_out_table::state ui_out_table::current_state () const
{
  return m_state;
}

/* See ui-out.h.  */

int ui_out_table::entry_level () const
{
  return m_entry_level;
}

/* The ui_out structure */

struct ui_out
  {
    int flags;
    /* Specific implementation of ui-out.  */
    const struct ui_out_impl *impl;
    void *data;

    /* Vector to store and track the ui-out levels.  */
    std::vector<std::unique_ptr<ui_out_level>> levels;

    int level () const
    {
      return this->levels.size ();
    }

    /* A table, if any.  At present only a single table is supported.  */
    std::unique_ptr<ui_out_table> table;
  };

/* The current (inner most) level.  */
static ui_out_level *
current_level (struct ui_out *uiout)
{
  return uiout->levels.back ().get ();
}

/* Create a new level, of TYPE.  */
static void
push_level (struct ui_out *uiout,
	    enum ui_out_type type)
{
  std::unique_ptr<ui_out_level> level (new ui_out_level (type));

  uiout->levels.push_back (std::move (level));
}

/* Discard the current level.  TYPE is the type of the level being
   discarded.  */
static void
pop_level (struct ui_out *uiout,
	   enum ui_out_type type)
{
  /* We had better not underflow the buffer.  */
  gdb_assert (uiout->level () > 0);
  gdb_assert (current_level (uiout)->type () == type);

  uiout->levels.pop_back ();
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
		      const char *id);
static void uo_end (struct ui_out *uiout,
		    enum ui_out_type type);
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

static void verify_field (struct ui_out *uiout, int *fldno, int *width,
			  enum ui_align *align);

/* exported functions (ui_out API) */

/* Mark beginning of a table.  */

static void
ui_out_table_begin (struct ui_out *uiout, int nr_cols,
		    int nr_rows, const std::string &tblid)
{
  if (uiout->table != nullptr)
    internal_error (__FILE__, __LINE__,
		    _("tables cannot be nested; table_begin found before \
previous table_end."));

  uiout->table.reset (
    new ui_out_table (uiout->level () + 1, nr_cols, tblid));

  uo_table_begin (uiout, nr_cols, nr_rows, tblid.c_str ());
}

void
ui_out_table_body (struct ui_out *uiout)
{
  if (uiout->table == nullptr)
    internal_error (__FILE__, __LINE__,
		    _("table_body outside a table is not valid; it must be \
after a table_begin and before a table_end."));

  uiout->table->start_body ();

  uo_table_body (uiout);
}

static void
ui_out_table_end (struct ui_out *uiout)
{
  if (uiout->table == nullptr)
    internal_error (__FILE__, __LINE__,
		    _("misplaced table_end or missing table_begin."));

  uo_table_end (uiout);

  uiout->table = nullptr;
}

void
ui_out_table_header (struct ui_out *uiout, int width, enum ui_align alignment,
		     const std::string &col_name, const std::string &col_hdr)
{
  if (uiout->table == nullptr)
    internal_error (__FILE__, __LINE__,
		    _("table_header outside a table is not valid; it must be "
		      "after a table_begin and before a table_body."));

  uiout->table->append_header (width, alignment, col_name, col_hdr);

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

  push_level (uiout, type);

  /* If the push puts us at the same level as a table row entry, we've
     got a new table row.  Put the header pointer back to the start.  */
  if (uiout->table != nullptr
      && uiout->table->current_state () == ui_out_table::state::BODY
      && uiout->table->entry_level () == uiout->level ())
    uiout->table->start_row ();

  uo_begin (uiout, type, id);
}

void
ui_out_end (struct ui_out *uiout,
	    enum ui_out_type type)
{
  pop_level (uiout, type);

  uo_end (uiout, type);
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
ui_out_test_flags (struct ui_out *uiout, ui_out_flags mask)
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

void
uo_begin (struct ui_out *uiout,
	  enum ui_out_type type,
	  const char *id)
{
  if (uiout->impl->begin == NULL)
    return;
  uiout->impl->begin (uiout, type, id);
}

void
uo_end (struct ui_out *uiout,
	enum ui_out_type type)
{
  if (uiout->impl->end == NULL)
    return;
  uiout->impl->end (uiout, type);
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

/* Verify that the field/tuple/list is correctly positioned.  Return
   the field number and corresponding alignment (if
   available/applicable).  */

static void
verify_field (struct ui_out *uiout, int *fldno, int *width,
	      enum ui_align *align)
{
  ui_out_level *current = current_level (uiout);
  const char *text;

  if (uiout->table != nullptr
      && uiout->table->current_state () != ui_out_table::state::BODY)
    {
      internal_error (__FILE__, __LINE__,
		      _("table_body missing; table fields must be \
specified after table_body and inside a list."));
    }

  current->inc_field_count ();

  if (uiout->table != nullptr
      && uiout->table->current_state () == ui_out_table::state::BODY
      && uiout->table->entry_level () == uiout->level ()
      && uiout->table->get_next_header (fldno, width, align, &text))
    {
      if (*fldno != current->field_count ())
	internal_error (__FILE__, __LINE__,
			_("ui-out internal error in handling headers."));
    }
  else
    {
      *width = 0;
      *align = ui_noalign;
      *fldno = current->field_count ();
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
  if (uiout->table == nullptr)
    return 0;

  return uiout->table->query_field (colno, width, alignment, col_name);
}

/* Initialize private members at startup.  */

struct ui_out *
ui_out_new (const struct ui_out_impl *impl, void *data,
	    ui_out_flags flags)
{
  struct ui_out *uiout = new ui_out ();

  uiout->data = data;
  uiout->impl = impl;
  uiout->flags = flags;

  /* Create the ui-out level #1, the default level.  */
  push_level (uiout, ui_out_type_tuple);

  return uiout;
}
