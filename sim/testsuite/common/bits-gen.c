/* Miscellaneous simulator utilities.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include <stdio.h>


void
gen_struct (void)
{
  printf ("\n");
  printf ("typedef struct _test_tuples {\n");
  printf ("  int line;\n");
  printf ("  int row;\n");
  printf ("  int col;\n");
  printf ("  long long val;\n");
  printf ("  long long check;\n");
  printf ("} test_tuples;\n");
  printf ("\n");
  printf ("typedef struct _test_spec {\n");
  printf ("  const char *file;\n");
  printf ("  const char *macro;\n");
  printf ("  int nr_rows;\n");
  printf ("  int nr_cols;\n");
  printf ("  test_tuples *tuples;\n");
  printf ("} test_spec;\n");
}


void
gen_bit (int bitsize,
	 int msb,
	 const char *macro,
	 int nr_bits)
{
  int i;

  printf ("\n/* Test the BIT%s macro */\n", macro);
  printf ("test_tuples bit%s_tuples[%d] = {\n", macro, nr_bits);
  for (i = 0; i < nr_bits; i++)
    {
      /* compute what we think the value is */
      long long bit = 1;
      if (msb == 0)
	bit <<= nr_bits - i - 1;
      else
	bit <<= i;
      if (bitsize == 32)
	bit = (long) bit;
      /* write it out */
      printf ("  { __LINE__, ");
      printf ("%d, %d, ", -1, i);
      printf ("BIT%s (%2d), ", macro, i);
      printf ("0x%08lx%08lxLL, ", (long) (bit >> 32), (long) bit);
      printf ("},\n");
    }
  printf ("};\n");
  printf ("\n");
  printf ("test_spec bit%s_test = { __FILE__, \"BIT%s\", 1, %d, bit%s_tuples, };\n",
	  macro, macro, nr_bits, macro);
  printf ("\n");
}


void
gen_mask (int bitsize,
	  const char *msb,
	  const char *macro,
	  int nr_bits)
{
  int l;
  int h;
  printf ("\n/* Test the %sMASK%s macro */\n", msb, macro);
  printf ("test_tuples mask%s_tuples[%d][%d] = {\n", macro, nr_bits, nr_bits);
  for (l = 0; l < nr_bits; l++)
    {
      printf ("  {\n");
      for (h = 0; h < nr_bits; h++)
	{
	  printf ("    { __LINE__, ");
	  if ((strcmp (msb, "MS") == 0 && l <= h)
	      || (strcmp (msb, "MS") != 0 && l >= h)
	      || (strcmp (macro, "") == 0))
	    {
	      /* compute the mask */
	      long long mask = 0;
	      int b;
	      for (b = 0; b < nr_bits; b++)
		{
		  long long bit = 1;
		  if (strcmp (msb, "MS") == 0)
		    {
		      if ((l <= b && b <= h)
			  || (h < l && (b <= h || b >= l)))
			bit <<= (nr_bits - b - 1);
		      else
			bit = 0;
		    }
		  else
		    {
		      if ((l >= b && b >= h)
			  || (h > l && (b >= h || b <= l)))
			bit <<= b;
		      else
			bit = 0;
		    }
		  mask |= bit;
		}
	      if (bitsize == 32)
		mask = (unsigned long) mask;
	      printf ("%d, %d, ", l, h);
	      printf ("%sMASK%s (%2d, %2d), ", msb, macro, l, h);
	      printf ("0x%08lx%08lxLL, ", (long) (mask >> 32), (long) mask);
	    }
	  else
	    printf ("-1, -1, ");
	  printf ("},\n");
	}
      printf ("  },\n");
    }
  printf ("};\n");
  printf ("\n");
  printf ("test_spec mask%s_test = { __FILE__, \"%sMASK%s\", %d, %d, &mask%s_tuples[0][0], };\n",
	  macro, msb, macro, nr_bits, nr_bits, macro);
  printf ("\n");
}


void
usage (int reason)
{
  fprintf (stderr, "Usage:\n");
  fprintf (stderr, "  bits-gen <nr-bits> <msb>\n");
  fprintf (stderr, "Generate a test case for the simulator bit manipulation code\n");
  fprintf (stderr, "  <nr-bits> = { 32 | 64 }\n");
  fprintf (stderr, "  <msb> = { 0 | { 31 | 63 } }\n");

  switch (reason)
    {
    case 1: fprintf (stderr, "Wrong number of arguments\n");
      break;
    case 2:
      fprintf (stderr, "Invalid <nr-bits> argument\n");
      break;
    case 3:
      fprintf (stderr, "Invalid <msb> argument\n");
      break;
    default:
    }

  exit (1);
}



int
main (argc, argv)
     int argc;
     char **argv;
{
  int bitsize;
  int msb;
  char *ms;

  /* parse the only argument */
  if (argc != 3)
    usage (1);
  if (strcmp (argv [1], "32") == 0)
    bitsize = 32;
  else if (strcmp (argv [1], "64") == 0)
    bitsize = 64;
  else
    usage (2);
  if (strcmp (argv [2], "0") == 0)
    msb = 0;
  else if (strcmp (argv [2], "31") == 0 && bitsize == 32)
    msb = 31;
  else if (strcmp (argv [2], "63") == 0 && bitsize == 64)
    msb = 63;
  else
    usage (3);
  if (msb == 0)
    ms = "MS";
  else
    ms = "LS";

  printf ("#define WITH_TARGET_WORD_BITSIZE %d\n", bitsize);
  printf ("#define WITH_TARGET_WORD_MSB %d\n", msb);
  printf ("#define WITH_HOST_WORD_BITSIZE %d\n", sizeof (int) * 8);
  printf ("\n");
  printf ("#define SIM_BITS_INLINE (INCLUDE_MODULE | INCLUDED_BY_MODULE)\n");
  printf ("\n");
  printf ("#define ASSERT(X) do { if (!(X)) abort(); } while (0)\n");
  printf ("\n");
  printf ("#include \"sim-basics.h\"\n");
  printf ("#include \"sim-types.h\"\n");
  printf ("#include \"sim-bits.h\"\n");

  gen_struct ();



  printf ("#define DO_BIT_TESTS\n");
  gen_bit ( 4, msb,  "4",  4);
  gen_bit ( 5, msb,  "5",  5);
  gen_bit ( 8, msb,  "8",  8);
  gen_bit (10, msb, "10", 10);
  gen_bit (16, msb, "16", 16);
  gen_bit (32, msb, "32", 32);
  gen_bit (64, msb, "64", 64);
  gen_bit (bitsize, msb, "", 64);

  printf ("test_spec *(bit_tests[]) = {\n");
  printf ("  &bit4_test,\n");
  printf ("  &bit5_test,\n");
  printf ("  &bit8_test,\n");
  printf ("  &bit10_test,\n");
  printf ("  &bit16_test,\n");
  printf ("  &bit32_test,\n");
  printf ("  &bit64_test,\n");
  printf ("  &bit_test,\n");
  printf ("  0,\n");
  printf ("};\n\n");



  printf ("#define DO_MASK_TESTS\n");
  gen_mask (16, ms, "16", 16);
  gen_mask (32, ms, "32", 32);
  gen_mask (64, ms, "64", 64);
  gen_mask (bitsize, ms, "", 64);

  printf ("test_spec *(mask_tests[]) = {\n");
  printf ("  &mask16_test,\n");
  printf ("  &mask32_test,\n");
  printf ("  &mask64_test,\n");
  printf ("  &mask_test,\n");
  printf ("  0,\n");
  printf ("};\n\n");

  return 0;
}
