/* Intel 387 floating point stuff.
   Copyright (C) 1988, 1989, 1991, 1998 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "language.h"
#include "value.h"
#include "gdbcore.h"
#include "floatformat.h"

void i387_to_double PARAMS ((char *, char *));
void double_to_i387 PARAMS ((char *, char *));

static void print_387_control_bits PARAMS ((unsigned int control));
static void print_387_status_bits  PARAMS ((unsigned int status));

/* FIXME:  Eliminate these routines when we have the time to change all
   the callers.  */

void
i387_to_double (from, to)
     char *from;
     char *to;
{
  floatformat_to_double (&floatformat_i387_ext, from, (double *) to);
}

void
double_to_i387 (from, to)
     char *from;
     char *to;
{
  floatformat_from_double (&floatformat_i387_ext, (double *) from, to);
}

static void
print_387_control_bits (control)
     unsigned int control;
{
  switch ((control >> 8) & 3)
    {
    case 0:
      puts_unfiltered (" 24 bit; ");
      break;
    case 1:
      puts_unfiltered (" (bad); ");
      break;
    case 2:
      puts_unfiltered (" 53 bit; ");
      break;
    case 3:
      puts_unfiltered (" 64 bit; ");
      break;
    }
  switch ((control >> 10) & 3)
    {
    case 0:
      puts_unfiltered ("NEAR; ");
      break;
    case 1:
      puts_unfiltered ("DOWN; ");
      break;
    case 2:
      puts_unfiltered ("UP; ");
      break;
    case 3:
      puts_unfiltered ("CHOP; ");
      break;
    }
  if (control & 0x3f)
    {
      puts_unfiltered ("mask");
      if (control & 0x0001)
	puts_unfiltered (" INVAL");
      if (control & 0x0002)
	puts_unfiltered (" DENOR");
      if (control & 0x0004)
	puts_unfiltered (" DIVZ");
      if (control & 0x0008)
	puts_unfiltered (" OVERF");
      if (control & 0x0010)
	puts_unfiltered (" UNDER");
      if (control & 0x0020)
	puts_unfiltered (" LOS");
      puts_unfiltered (";");
    }

  if (control & 0xe080)
    warning ("\nreserved bits on: %s",
	     local_hex_string (control & 0xe080));
}

void
print_387_control_word (control)
     unsigned int control;
{
  printf_filtered ("control %s:", local_hex_string(control & 0xffff));
  print_387_control_bits (control);
  puts_unfiltered ("\n");
}

static void
print_387_status_bits (status)
     unsigned int status;
{
  printf_unfiltered (" flags %d%d%d%d; ",
		     (status & 0x4000) != 0,
		     (status & 0x0400) != 0,
		     (status & 0x0200) != 0,
		     (status & 0x0100) != 0);
  printf_unfiltered ("top %d; ", (status >> 11) & 7);
  if (status & 0xff) 
    {
      puts_unfiltered ("excep");
      if (status & 0x0001) puts_unfiltered (" INVAL");
      if (status & 0x0002) puts_unfiltered (" DENOR");
      if (status & 0x0004) puts_unfiltered (" DIVZ");
      if (status & 0x0008) puts_unfiltered (" OVERF");
      if (status & 0x0010) puts_unfiltered (" UNDER");
      if (status & 0x0020) puts_unfiltered (" LOS");
      if (status & 0x0040) puts_unfiltered (" STACK");
    }
}

void
print_387_status_word (status)
     unsigned int status;
{
  printf_filtered ("status %s:", local_hex_string (status & 0xffff));
  print_387_status_bits (status);
  puts_unfiltered ("\n");
}

#ifdef LD_I387
int
i387_extract_floating (PTR addr, int len, DOUBLEST *dretptr)
{
  if (len == TARGET_LONG_DOUBLE_BIT / 8)
    {
      if (HOST_LONG_DOUBLE_FORMAT == TARGET_LONG_DOUBLE_FORMAT)
	{
	  DOUBLEST retval;

	  memcpy (dretptr, addr, sizeof (retval));
	}
      else
	floatformat_to_doublest (TARGET_LONG_DOUBLE_FORMAT, addr, dretptr);

      return 1;
    }
  else
    return 0;
}

int
i387_store_floating (PTR addr, int len, DOUBLEST val)
{
  if (len == TARGET_LONG_DOUBLE_BIT / 8)
    {
      /* This `if' may be totally stupid.  I just put it in here to be
	 absolutely sure I'm preserving the semantics of the code I'm
	 frobbing, while I try to maintain portability boundaries; I
	 don't actually know exactly what it's doing.  -JimB, May 1999 */
      if (HOST_LONG_DOUBLE_FORMAT == TARGET_LONG_DOUBLE_FORMAT)
	memcpy (addr, &val, sizeof (val));
      else
	floatformat_from_doublest (TARGET_LONG_DOUBLE_FORMAT, &val, addr);

      return 1;
    }
  else
    return 0;
}
#endif /* LD_I387 */
