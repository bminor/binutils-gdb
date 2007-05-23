/* winduni.c -- unicode support for the windres program.
   Copyright 1997, 1998, 2000, 2001, 2003, 2007
   Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support.
   Rewritten by Kai Tietz, Onevision.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* This file contains unicode support routines for the windres
   program.  Ideally, we would have generic unicode support which
   would work on all systems.  However, we don't.  Instead, on a
   Windows host, we are prepared to call some Windows routines.  This
   means that we will generate different output on Windows and Unix
   hosts, but that seems better than not really supporting unicode at
   all.  */

#include "sysdep.h"
#include "bfd.h"
#include "bucomm.h"
#include "winduni.h"
#include "safe-ctype.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* Prototypes.  */
static int unichar_isascii (const unichar *, rc_uint_type);

/* Convert an ASCII string to a unicode string.  We just copy it,
   expanding chars to shorts, rather than doing something intelligent.  */

void
unicode_from_ascii (rc_uint_type *length, unichar **unicode, const char *ascii)
{
  rc_uint_type len;
#ifndef _WIN32
  const char *s;
  unsigned short *w;

  len = strlen (ascii);
  *unicode = ((unichar *) res_alloc ((len + 1) * sizeof (unichar)));
  for (s = ascii, w = *unicode; *s != '\0'; s++, w++)
    *w = *s & 0xff;
  *w = 0;
#else
  /* We use  MultiByteToWideChar rather than strlen to get the unicode
     string length to allow multibyte "ascii" chars. The value returned
     by this function includes the trailing '\0'.  */
  len = (rc_uint_type) MultiByteToWideChar (CP_ACP, 0, ascii, -1, NULL, 0);
  if (len)
    {
      *unicode = ((unichar *) res_alloc (len * sizeof (unichar)));
      MultiByteToWideChar (CP_ACP, 0, ascii, -1, *unicode, (int) len);
    }
  /* Discount the trailing '/0'.  If MultiByteToWideChar failed,
     this will set *length to -1.  */
  len--;
#endif

  if (length != NULL)
    *length = len;
}

/* Convert an unicode string to an ASCII string.  We just copy it,
   shrink shorts to chars, rather than doing something intelligent.
   Shorts with not within the char range are replaced by '_'.  */

void
ascii_from_unicode (rc_uint_type *length, const unichar *unicode, char **ascii)
{
  rc_uint_type len;
#ifndef _WIN32
  char *s;
  const unsigned short *w;

  len = 0;
  while (unicode[len] != 0)
    ++len;
  *ascii = ((char *) res_alloc (len + 1));
  for (s = *ascii, w = unicode; *w != '\0'; w++, s++)
    {
      if(w[0]==(w[0]&0xff))
	*s = (char) w[0];
      else
	*s = '_';
    }
  *s = 0;
#else
  WINBOOL used_def = FALSE;
  /* We use  MultiByteToWideChar rather than strlen to get the unicode
     string length to allow multibyte "ascii" chars. The value returned
     by this function includes the trailing '\0'.  */
  len = (rc_uint_type) WideCharToMultiByte (CP_ACP, WC_DEFAULTCHAR, unicode, -1, NULL,
  				       0, "_", &used_def);
  if (len)
    {
      *ascii = (char *) res_alloc (len * sizeof (char));
      WideCharToMultiByte (CP_ACP, WC_DEFAULTCHAR, unicode, -1, *ascii, (int) len,
      			   "_", &used_def);
    }
  /* Discount the trailing '/0'.  If MultiByteToWideChar failed,
     this will set *length to -1.  */
  len--;
#endif

  if (length != NULL)
    *length = len;
}

/* Print the unicode string UNICODE to the file E.  LENGTH is the
   number of characters to print, or -1 if we should print until the
   end of the string.  FIXME: On a Windows host, we should be calling
   some Windows function, probably WideCharToMultiByte.  */

void
unicode_print (FILE *e, const unichar *unicode, rc_uint_type length)
{
  while (1)
    {
      unichar ch;

      if (length == 0)
	return;
      if ((bfd_signed_vma) length > 0)
	--length;

      ch = *unicode;

      if (ch == 0 && (bfd_signed_vma) length < 0)
	return;

      ++unicode;

      if ((ch & 0x7f) == ch)
	{
	  if (ch == '\\')
	    fputs ("\\\\", e);
	  else if (ch == '"')
	    fputs ("\"\"", e);
	  else if (ISPRINT (ch))
	    putc (ch, e);
	  else
	    {
	      switch (ch)
		{
		case ESCAPE_A:
		  fputs ("\\a", e);
		  break;

		case ESCAPE_B:
		  fputs ("\\b", e);
		  break;

		case ESCAPE_F:
		  fputs ("\\f", e);
		  break;

		case ESCAPE_N:
		  fputs ("\\n", e);
		  break;

		case ESCAPE_R:
		  fputs ("\\r", e);
		  break;

		case ESCAPE_T:
		  fputs ("\\t", e);
		  break;

		case ESCAPE_V:
		  fputs ("\\v", e);
		  break;

		default:
		  fprintf (e, "\\%03o", (unsigned int) ch);
		  break;
		}
	    }
	}
      else if ((ch & 0xff) == ch)
	fprintf (e, "\\%03o", (unsigned int) ch);
      else
	fprintf (e, "\\x%x", (unsigned int) ch);
    }
}

/* Print a unicode string to a file.  */
void
ascii_print (FILE *e, const char *s, rc_uint_type length)
{
  while (1)
    {
      char ch;

      if (length == 0)
	return;
      if ((bfd_signed_vma) length > 0)
	--length;

      ch = *s;

      if (ch == 0 && (bfd_signed_vma) length < 0)
	return;

      ++s;

      if ((ch & 0x7f) == ch)
	{
	  if (ch == '\\')
	    fputs ("\\\\", e);
	  else if (ch == '"')
	    fputs ("\"\"", e);
	  else if (ISPRINT (ch))
	    putc (ch, e);
	  else
	    {
	      switch (ch)
		{
		case ESCAPE_A:
		  fputs ("\\a", e);
		  break;

		case ESCAPE_B:
		  fputs ("\\b", e);
		  break;

		case ESCAPE_F:
		  fputs ("\\f", e);
		  break;

		case ESCAPE_N:
		  fputs ("\\n", e);
		  break;

		case ESCAPE_R:
		  fputs ("\\r", e);
		  break;

		case ESCAPE_T:
		  fputs ("\\t", e);
		  break;

		case ESCAPE_V:
		  fputs ("\\v", e);
		  break;

		default:
		  fprintf (e, "\\%03o", (unsigned int) ch);
		  break;
		}
	    }
	}
      else
	fprintf (e, "\\%03o", (unsigned int) ch & 0xff);
    }
}

rc_uint_type
unichar_len (const unichar *unicode)
{
  rc_uint_type r = 0;
  if (unicode)
    while (unicode[r] != 0)
      r++;
  else
    --r;
  return r;
}

unichar *
unichar_dup (const unichar *unicode)
{
  unichar *r;
  int len;

  if (! unicode)
    return NULL;
  for (len = 0; unicode[len] != 0; ++len)
    ;
  ++len;
  r = ((unichar *) res_alloc (len * sizeof (unichar)));
  memcpy (r, unicode, len * sizeof (unichar));
  return r;
}

unichar *
unichar_dup_uppercase (const unichar *u)
{
  unichar *r = unichar_dup (u);
  int i;

  if (! r)
    return NULL;

  for (i = 0; r[i] != 0; ++i)
    {
      if (r[i] >= 'a' && r[i] <= 'z')
	r[i] &= 0xdf;
    }
  return r;
}

static int
unichar_isascii (const unichar *u, rc_uint_type len)
{
  rc_uint_type i;
  if ((bfd_signed_vma) len < 0)
    {
      if (u)
	len = (rc_uint_type) unichar_len (u);
      else
	len = 0;
    }

  for (i = 0; i < len; i++)
    if ((u[i] & 0xff80) != 0)
      return 0;
  return 1;
}

void
unicode_print_quoted (FILE *e, const unichar *u, rc_uint_type len)
{
  if (! unichar_isascii (u, len))
    fputc ('L', e);
  fputc ('"', e);
  unicode_print (e, u, len);
  fputc ('"', e);
}
