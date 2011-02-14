/* This testcase is part of GDB, the GNU debugger.

   Copyright 2002, 2003, 2004, 2007, 2008, 2009, 2010, 2011
   Free Software Foundation, Inc.

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

#include <stdlib.h>
#include <string.h>

/* Test program partial trace data visualization.  */

/* Typedefs.  */

typedef struct TEST_STRUCT {
  char   memberc;
  int    memberi;
  float  memberf;
  double memberd;
} test_struct;

struct small_struct
{
  int member;
};

struct small_struct_b : public small_struct
{
};

typedef int test_array [4];

/* Global variables to be collected.  */

char         globalc;
int          globali;
float        globalf;
double       globald;
test_struct  globalstruct;
test_struct *globalp;
int          globalarr[16];
small_struct g_smallstruct;
small_struct_b g_smallstruct_b;

/* Strings.  */

const char g_const_string[] = "hello world";
char g_string_unavail[sizeof (g_const_string)];
char g_string_partial[sizeof (g_const_string)];
const char *g_string_p;

/* Used to check that <unavailable> is not the same as 0 in array
   element repetitions.  */

struct tuple
{
  int a;
  int b;
};

struct tuple tarray[8];

/* Random tests.  */

struct StructA
{
  int a, b;
  int array[10000];
  void *ptr;
  int bitfield:1;
};

struct StructB
{
  int d, ef;
  StructA struct_a;
  int s:1;
  static StructA static_struct_a;
  const char *string;
};

/* References.  */

int g_int;
int &g_ref = g_int;

struct StructRef
{
  StructRef (unsigned int val) : ref(d) {}

  void clear ()
  {
    d = 0;
  }

  unsigned int d;
  unsigned int &ref;
};

struct StructB struct_b;
struct StructA StructB::static_struct_a;

StructRef g_structref(0x12345678);
StructRef *g_structref_p = &g_structref;


/* Test functions.  */

static void
begin ()	/* called before anything else */
{
}

static void
end ()		/* called after everything else */
{
}

int
globals_test_func ()
{
  int i = 0;

  i += globalc + globali + globalf + globald;
  i += globalstruct.memberc + globalstruct.memberi;
  i += globalstruct.memberf + globalstruct.memberd;
  i += globalarr[1];

  return i;	/* set globals_test_func tracepoint here */
}

int
main (int argc, char **argv, char **envp)
{
  int         i = 0;
  test_struct mystruct;
  int         myarray[4];

  begin ();
  /* Assign collectable values to global variables.  */
  globalc = 71;
  globali = 72;
  globalf = 73.3;
  globald = 74.4;
  globalstruct.memberc = 81;
  globalstruct.memberi = 82;
  globalstruct.memberf = 83.3;
  globalstruct.memberd = 84.4;
  globalp = &globalstruct;

  for (i = 0; i < 15; i++)
    globalarr[i] = i;

  mystruct.memberc = 101;
  mystruct.memberi = 102;
  mystruct.memberf = 103.3;
  mystruct.memberd = 104.4;
  myarray[0] = 111;
  myarray[1] = 112;
  myarray[2] = 113;
  myarray[3] = 114;

  g_int = 123;
  memset (&struct_b, 0xaa, sizeof struct_b);
  memset (&struct_b.static_struct_a, 0xaa, sizeof struct_b.static_struct_a);
  struct_b.string = g_const_string;
  memcpy (g_string_unavail, g_const_string, sizeof (g_const_string));
  memcpy (g_string_partial, g_const_string, sizeof (g_const_string));
  g_string_p = g_const_string;

  /* Call test functions, so they can be traced and data collected.  */
  i = 0;
  i += globals_test_func ();

  /* Set 'em back to zero, so that the collected values will be
     distinctly different from the "realtime" (end of test) values.  */

  globalc = 0;
  globali = 0;
  globalf = 0;
  globald = 0;
  globalstruct.memberc = 0;
  globalstruct.memberi = 0;
  globalstruct.memberf = 0;
  globalstruct.memberd = 0;
  globalp = 0;
  for (i = 0; i < 15; i++)
    globalarr[i] = 0;

  memset (&struct_b, 0, sizeof struct_b);
  memset (&struct_b.static_struct_a, 0, sizeof struct_b.static_struct_a);
  struct_b.string = NULL;
  memset (g_string_unavail, 0, sizeof (g_string_unavail));
  memset (g_string_partial, 0, sizeof (g_string_partial));
  g_string_p = NULL;

  g_int = 0;

  g_structref.clear ();
  g_structref_p = NULL;

  end ();
  return 0;
}
