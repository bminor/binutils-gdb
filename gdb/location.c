/* Data structures and API for event locations in GDB.
   Copyright (C) 2013-2015 Free Software Foundation, Inc.

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
#include "gdb_assert.h"
#include "location.h"
#include "symtab.h"
#include "language.h"
#include "linespec.h"
#include "cli/cli-utils.h"
#include "probe.h"

#include <ctype.h>
#include <string.h>

/* An event location used to set a stop event in the inferior.
   This structure is an amalgam of the various ways
   to specify where a stop event should be set.  */

struct event_location
{
  /* The type of this breakpoint specification.  */
  enum event_location_type type;
#define EL_TYPE(PTR) (PTR)->type

  union
  {
    /* A generic "this is a string specification" for a location.
       This representation is used by both "normal" linespecs and
       probes.  */
    char *addr_string;
#define EL_LINESPEC(PTR) ((PTR)->u.addr_string)
#define EL_PROBE(PTR) ((PTR)->u.addr_string)

    /* An address in the inferior.  */
    CORE_ADDR address;
#define EL_ADDRESS(PTR) (PTR)->u.address
  } u;

  /* Cached string representation of this location.  This is used, e.g., to
     save stop event locations to file.  Malloc'd.  */
  char *as_string;
#define EL_STRING(PTR) ((PTR)->as_string)
};

/* See description in location.h.  */

enum event_location_type
event_location_type (const struct event_location *location)
{
  return EL_TYPE (location);
}

/* See description in location.h.  */

struct event_location *
new_linespec_location (char **linespec)
{
  struct event_location *location;

  location = XCNEW (struct event_location);
  EL_TYPE (location) = LINESPEC_LOCATION;
  if (*linespec != NULL)
    {
      char *p;
      char *orig = *linespec;

      linespec_lex_to_end (linespec);
      p = remove_trailing_whitespace (orig, *linespec);
      if ((p - orig) > 0)
	EL_LINESPEC (location) = savestring (orig, p - orig);
    }
  return location;
}

/* See description in location.h.  */

const char *
get_linespec_location (const struct event_location *location)
{
  gdb_assert (EL_TYPE (location) == LINESPEC_LOCATION);
  return EL_LINESPEC (location);
}

/* See description in location.h.  */

struct event_location *
new_address_location (CORE_ADDR addr)
{
  struct event_location *location;

  location = XCNEW (struct event_location);
  EL_TYPE (location) = ADDRESS_LOCATION;
  EL_ADDRESS (location) = addr;
  return location;
}

/* See description in location.h.  */

CORE_ADDR
get_address_location (const struct event_location *location)
{
  gdb_assert (EL_TYPE (location) == ADDRESS_LOCATION);
  return EL_ADDRESS (location);
}

/* See description in location.h.  */

struct event_location *
new_probe_location (const char *probe)
{
  struct event_location *location;

  location = XCNEW (struct event_location);
  EL_TYPE (location) = PROBE_LOCATION;
  if (probe != NULL)
    EL_PROBE (location) = xstrdup (probe);
  return location;
}

/* See description in location.h.  */

const char *
get_probe_location (const struct event_location *location)
{
  gdb_assert (EL_TYPE (location) == PROBE_LOCATION);
  return EL_PROBE (location);
}

/* See description in location.h.  */

struct event_location *
copy_event_location (const struct event_location *src)
{
  struct event_location *dst;

  dst = XCNEW (struct event_location);
  EL_TYPE (dst) = EL_TYPE (src);
  if (EL_STRING (src) != NULL)
    EL_STRING (dst) = xstrdup (EL_STRING (src));

  switch (EL_TYPE (src))
    {
    case LINESPEC_LOCATION:
      if (EL_LINESPEC (src) != NULL)
	EL_LINESPEC (dst) = xstrdup (EL_LINESPEC (src));
      break;

    case ADDRESS_LOCATION:
      EL_ADDRESS (dst) = EL_ADDRESS (src);
      break;

    case PROBE_LOCATION:
      if (EL_PROBE (src) != NULL)
	EL_PROBE (dst) = xstrdup (EL_PROBE (src));
      break;

    default:
      gdb_assert_not_reached ("unknown event location type");
    }

  return dst;
}

/* A cleanup function for struct event_location.  */

static void
delete_event_location_cleanup (void *data)
{
  struct event_location *location = (struct event_location *) data;

  delete_event_location (location);
}

/* See description in location.h.  */

struct cleanup *
make_cleanup_delete_event_location (struct event_location *location)
{
  return make_cleanup (delete_event_location_cleanup, location);
}

/* See description in location.h.  */

void
delete_event_location (struct event_location *location)
{
  if (location != NULL)
    {
      xfree (EL_STRING (location));

      switch (EL_TYPE (location))
	{
	case LINESPEC_LOCATION:
	  xfree (EL_LINESPEC (location));
	  break;

	case ADDRESS_LOCATION:
	  /* Nothing to do.  */
	  break;

	case PROBE_LOCATION:
	  xfree (EL_PROBE (location));
	  break;

	default:
	  gdb_assert_not_reached ("unknown event location type");
	}

      xfree (location);
    }
}

/* See description in location.h.  */

const char *
event_location_to_string (struct event_location *location)
{
  if (EL_STRING (location) == NULL)
    {
      switch (EL_TYPE (location))
	{
	case LINESPEC_LOCATION:
	  if (EL_LINESPEC (location) != NULL)
	    EL_STRING (location) = xstrdup (EL_LINESPEC (location));
	  break;

	case ADDRESS_LOCATION:
	  EL_STRING (location)
	    = xstrprintf ("*%s",
			  core_addr_to_string (EL_ADDRESS (location)));
	  break;

	case PROBE_LOCATION:
	  EL_STRING (location) = xstrdup (EL_PROBE (location));
	  break;

	default:
	  gdb_assert_not_reached ("unknown event location type");
	}
    }

  return EL_STRING (location);
}

/* See description in location.h.  */

struct event_location *
string_to_event_location (char **stringp,
			  const struct language_defn *language)
{
  struct event_location *location;

  /* First, check if the string is an address location.  */
  if (*stringp != NULL && **stringp == '*')
    {
      const char *arg, *orig;
      CORE_ADDR addr;

      orig = arg = *stringp;
      addr = linespec_expression_to_pc (&arg);
      location = new_address_location (addr);
      *stringp += arg - orig;
    }
  else
    {
      const char *cs;

      /* Next, try the input as a probe spec.  */
      cs = *stringp;
      if (cs != NULL && probe_linespec_to_ops (&cs) != NULL)
	{
	  location = new_probe_location (*stringp);
	  *stringp += strlen (*stringp);
	}
      else
	{
	  /* Everything else is a linespec.  */
	  location = new_linespec_location (stringp);
	}
    }

  return location;
}

/* See description in location.h.  */

int
event_location_empty_p (const struct event_location *location)
{
  switch (EL_TYPE (location))
    {
    case LINESPEC_LOCATION:
      /* Linespecs are never "empty."  (NULL is a valid linespec)  */
      return 0;

    case ADDRESS_LOCATION:
      return 0;

    case PROBE_LOCATION:
      return EL_PROBE (location) == NULL;

    default:
      gdb_assert_not_reached ("unknown event location type");
    }
}

/* See description in location.h.  */

void
set_event_location_string (struct event_location *location,
			   const char *string)
{
  xfree (EL_STRING (location));
  EL_STRING (location) = string == NULL ?  NULL : xstrdup (string);
}
