/* Data structures and API for location specs in GDB.
   Copyright (C) 2013-2022 Free Software Foundation, Inc.

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

#ifndef LOCATION_H
#define LOCATION_H

#include "symtab.h"

struct language_defn;
struct location_spec;

/* An enumeration of possible signs for a line offset.  */

enum offset_relative_sign
{
  /* No sign  */
  LINE_OFFSET_NONE,

  /* A plus sign ("+")  */
  LINE_OFFSET_PLUS,

  /* A minus sign ("-")  */
  LINE_OFFSET_MINUS,

  /* A special "sign" for unspecified offset.  */
  LINE_OFFSET_UNKNOWN
};

/* A line offset in a location.  */

struct line_offset
{
  /* Line offset and any specified sign.  */
  int offset;
  enum offset_relative_sign sign;
};

/* An enumeration of the various ways to specify a location spec.  */

enum location_spec_type
{
  /* A traditional linespec.  */
  LINESPEC_LOCATION_SPEC,

  /* An address location spec.  */
  ADDRESS_LOCATION_SPEC,

  /* An explicit location spec.  */
  EXPLICIT_LOCATION_SPEC,

  /* A probe location spec.  */
  PROBE_LOCATION_SPEC
};

/* A traditional linespec.  */

struct linespec_location
{
  /* Whether the function name is fully-qualified or not.  */
  symbol_name_match_type match_type;

  /* The linespec.  */
  char *spec_string;
};

/* An explicit location spec.  This structure is used to bypass the
   parsing done on linespecs.  It still has the same requirements
   as linespecs, though.  For example, source_filename requires
   at least one other field.  */

struct explicit_location
{
  /* The source filename. Malloc'd.  */
  char *source_filename;

  /* The function name.  Malloc'd.  */
  char *function_name;

  /* Whether the function name is fully-qualified or not.  */
  symbol_name_match_type func_name_match_type;

  /* The name of a label.  Malloc'd.  */
  char *label_name;

  /* A line offset relative to the start of the symbol
     identified by the above fields or the current symtab
     if the other fields are NULL.  */
  struct line_offset line_offset;
};

/* Return the type of the given location spec.  */

extern enum location_spec_type
  location_spec_type (const location_spec *);

/* Return a linespec string representation of the given explicit
   location spec.  The location spec must already be
   canonicalized/valid.  */

extern std::string explicit_location_to_linespec
  (const explicit_location *explicit_locspec);

/* Return a string representation of LOCSPEC.
   This function may return NULL for unspecified linespecs,
   e.g, LINESPEC_LOCATION_SPEC and spec_string is NULL.

   The result is cached in LOCSPEC.  */

extern const char *
  location_spec_to_string (location_spec *locspec);

/* A deleter for a struct location_spec.  */

struct location_spec_deleter
{
  void operator() (location_spec *locspec) const;
};

/* A unique pointer for location_spec.  */
typedef std::unique_ptr<location_spec, location_spec_deleter>
     location_spec_up;

/* Create a new linespec location spec.  */

extern location_spec_up new_linespec_location_spec
  (const char **linespec, symbol_name_match_type match_type);

/* Return the linespec location spec of the given location_spec (which
   must be of type LINESPEC_LOCATION_SPEC).  */

extern const linespec_location *
  get_linespec_location (const location_spec *locspec);

/* Create a new address location spec.
   ADDR is the address corresponding to this location_spec.
   ADDR_STRING, a string of ADDR_STRING_LEN characters, is
   the expression that was parsed to determine the address ADDR.  */

extern location_spec_up new_address_location_spec (CORE_ADDR addr,
						   const char *addr_string,
						   int addr_string_len);

/* Return the address (a CORE_ADDR) of the given location_spec, which
   must be of type ADDRESS_LOCATION_SPEC.  */

extern CORE_ADDR
  get_address_location (const location_spec *locspec);

/* Return the expression (a string) that was used to compute the
   address of the given location_spec, which must be of type
   ADDRESS_LOCATION_SPEC.  */

extern const char *
  get_address_string_location (const location_spec *locspec);

/* Create a new probe location.  */

extern location_spec_up new_probe_location_spec (std::string &&probe);

/* Return the probe location spec string of the given location_spec,
   which must be of type PROBE_LOCATION_SPEC.  */

extern const char *
  get_probe_location_spec_string (const location_spec *locspec);

/* Initialize the given explicit location.  */

extern void
  initialize_explicit_location (explicit_location *locspec);

/* Create a new explicit location.  If not NULL, EXPLICIT is checked for
   validity.  If invalid, an exception is thrown.  */

extern location_spec_up
  new_explicit_location_spec (const explicit_location *locspec);

/* Return the explicit location spec of the given location_spec, which
   must be of type EXPLICIT_LOCATION.  */

extern struct explicit_location *
  get_explicit_location (location_spec *locspec);

/* A const version of the above.  */

extern const explicit_location *
  get_explicit_location_const (const location_spec *locspec);

/* Return a copy of the given SRC location spec.  */

extern location_spec_up copy_location_spec (const location_spec *src);

/* Attempt to convert the input string in *ARGP into a location_spec.
   ARGP is advanced past any processed input.  Always returns a non-nullptr
   location_spec unique pointer object.

   This function may call error() if *ARGP looks like properly formed, but
   invalid, input, e.g., if it is called with missing argument parameters
   or invalid options.

   This function is intended to be used by CLI commands and will parse
   explicit location specs in a CLI-centric way.  Other interfaces should use
   string_to_location_spec_basic if they want to maintain support for
   legacy specifications of probe, address, and linespec location specs.

   MATCH_TYPE should be either WILD or FULL.  If -q/--qualified is specified
   in the input string, it will take precedence over this parameter.  */

extern location_spec_up string_to_location_spec
  (const char **argp, const struct language_defn *language,
   symbol_name_match_type match_type = symbol_name_match_type::WILD);

/* Like string_to_location_spec, but does not attempt to parse
   explicit location specs.  MATCH_TYPE indicates how function names
   should be matched.  */

extern location_spec_up
  string_to_location_spec_basic (const char **argp,
				 const struct language_defn *language,
				 symbol_name_match_type match_type);

/* Structure filled in by string_to_explicit_location_spec to aid the
   completer.  */
struct explicit_completion_info
{
  /* Pointer to the last option found.  E.g., in "b -sou src.c -fun
     func", LAST_OPTION is left pointing at "-fun func".  */
  const char *last_option = NULL;

  /* These point to the start and end of a quoted argument, iff the
     last argument was quoted.  If parsing finds an incomplete quoted
     string (e.g., "break -function 'fun"), then QUOTED_ARG_START is
     set to point to the opening \', and QUOTED_ARG_END is left NULL.
     If the last option is not quoted, then both are set to NULL. */
  const char *quoted_arg_start = NULL;
  const char *quoted_arg_end = NULL;

  /* True if we saw an explicit location spec option, as opposed to
     only flags that affect both explicit location specs and
     linespecs, like "-qualified".  */
  bool saw_explicit_location_spec_option = false;
};

/* Attempt to convert the input string in *ARGP into an explicit
   location spec.  ARGP is advanced past any processed input.  Returns
   a location_spec (malloc'd) if an explicit location spec was
   successfully found in *ARGP, NULL otherwise.

   If COMPLETION_INFO is NULL, this function may call error() if *ARGP
   looks like improperly formed input, e.g., if it is called with
   missing argument parameters or invalid options.  If COMPLETION_INFO
   is not NULL, this function will not throw any exceptions.  */

extern location_spec_up
  string_to_explicit_location_spec (const char **argp,
				    const struct language_defn *language,
				    explicit_completion_info *completion_info);

/* A convenience function for testing for unset location specs.  */

extern int location_spec_empty_p (const location_spec *locspec);

/* Set the location specs's string representation.  */

extern void set_location_spec_string (struct location_spec *locspec,
				      std::string &&string);

#endif /* LOCATION_H */
