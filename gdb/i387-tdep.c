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
  printf_unfiltered ("\n");
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

void
i387_print_register (raw_regs, regnum)
     char *raw_regs;
     int regnum;
{
  unsigned char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];
  unsigned long val;
  int j, sign, special;
  unsigned swd, tags, expon, top, norm, ls, ms;
  char string[12];

#if (FPREG_RAW_SIZE != 10)
#error "Bad FPREG_RAW_SIZE"
#endif

  printf_filtered ("%8.8s: ", REGISTER_NAME (regnum));
  if (regnum < FPDATA_REGNUM)
    {
      val = extract_unsigned_integer (raw_regs + REGISTER_BYTE (regnum), 4);
      if ( (regnum < FPSTART_REGNUM + 3) ||
	   (regnum == FPSTART_REGNUM + 6) )
	/* Don't print the un-modifiable bytes. */
	sprintf(string, "0x%04x", val & 0xffff);
      else
	sprintf(string, "0x%08x", val);

      printf_unfiltered ("%10.10s", string);

      if (regnum == FPCONTROL_REGNUM)
	print_387_control_bits (val);
      else if (regnum == FPSTATUS_REGNUM)
	print_387_status_bits (val);
    }
  else
    {
      /* An FPU stack register. */
      if ( REGISTER_RAW_SIZE (regnum) != FPREG_RAW_SIZE )
	error ("GDB bug: i387-tdep.c (i387_print_register): wrong size for FPU stack register");

      /* Put the data in the buffer.  No conversions are ever necessary. */
      memcpy (virtual_buffer, raw_regs + REGISTER_BYTE (regnum),
	      FPREG_RAW_SIZE);

      swd = extract_signed_integer (raw_regs + REGISTER_BYTE (FPSTATUS_REGNUM),
				    4);
      top = (swd >> 11) & 7;
      tags = extract_signed_integer (raw_regs + REGISTER_BYTE (FPTAG_REGNUM),
				     4);

      puts_unfiltered ("0x");
      for (j = 0; j < FPREG_RAW_SIZE; j++)
	printf_unfiltered ("%02x",
			   (unsigned char)raw_regs[REGISTER_BYTE (regnum)
						  + FPREG_RAW_SIZE - 1 - j]);
      
      puts_unfiltered ("  ");
      special = 0;
      switch ((tags >> (((regnum - FPDATA_REGNUM + top) & 7) * 2)) & 3) 
	{
	case 0: puts_unfiltered ("Valid "); break;
	case 1: puts_unfiltered ("Zero  "); break;
	case 2: puts_unfiltered ("Spec  ");
	  special = 1;
	  break;
	case 3: puts_unfiltered ("Empty "); break;
	}

      expon = extract_unsigned_integer (raw_regs + REGISTER_BYTE (regnum)
					+ FPREG_RAW_SIZE - 2, 2);
      sign = expon & 0x8000;
      expon &= 0x7fff;
      ms = extract_unsigned_integer (raw_regs + REGISTER_BYTE (regnum) + 4, 4);
      ls = extract_signed_integer (raw_regs + REGISTER_BYTE (regnum), 4);
      norm = ms & 0x80000000;

      if ( expon == 0 )
	{
	  if ( ms | ls )
	    {
	      /* Denormal or Pseudodenormal. */
	      if ( norm )
		puts_unfiltered ("Pseudo ");
	      else
		puts_unfiltered ("Denorm ");
	    }
	  else
	    {
	      /* Zero. */
	      puts_unfiltered ("Zero   ");
	    }
	}
      else if ( expon == 0x7fff )
	{
	  /* Infinity, NaN or unsupported. */
	  if ( (ms == 0x80000000) &&
	       (ls == 0) )
	    {
              puts_unfiltered ("Infty  ");
	    }
	  else if ( norm )
	    {
	      if ( ms & 0x40000000 )
		puts_unfiltered ("QNaN   ");
	      else
		puts_unfiltered ("SNaN   ");
	    }
	  else
	    {
              puts_unfiltered ("Unsupp ");
	    }
	}
      else
	{
	  /* Normal or unsupported. */
	  if ( norm )
	    puts_unfiltered ("Normal ");
	  else
	    puts_unfiltered ("Unsupp ");
	}

      val_print (REGISTER_VIRTUAL_TYPE (regnum), virtual_buffer, 0, 0,
		 gdb_stdout, 0,
		 1, 0, Val_pretty_default);
    }
  puts_filtered ("\n");
}

void i387_float_info(void)
{
  char raw_regs [REGISTER_BYTES];
  int i;

  for (i = FPSTART_REGNUM; i <= FPEND_REGNUM; i++)
    read_relative_register_raw_bytes (i, raw_regs + REGISTER_BYTE (i));

  for (i = FPSTART_REGNUM; i <= FPEND_REGNUM; i++)
    i387_print_register (raw_regs, i);
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
