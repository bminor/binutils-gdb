/* This testcase is part of GDB, the GNU debugger.

   Copyright 2018 Free Software Foundation, Inc.

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

/* This file is used for testing GDBs ability to pass structures to, and
   return structures from, functions.  All of the structures in this test
   are special in that they are small structures containing only 1 or 2
   scalar fields, the fields can be inside nested structures, and there can
   be empty structures around too.

   This test was originally written for RiscV which has some special ABI
   rules for structures like these, however, there should be no harm in
   running these tests on other targets, though in many cases the
   structures will treated no differently to the structures already covered
   in the structs.exp test script.  */

#include <string.h>

/* Useful abreviations.  */
typedef char tc;
typedef short ts;
typedef int ti;
typedef long tl;
typedef long long tll;
typedef float tf;
typedef double td;
typedef long double tld;

#ifdef TEST_COMPLEX
typedef float _Complex tfc;
typedef double _Complex tdc;
typedef long double _Complex tldc;
#endif /* TEST_COMPLEX */

#define MAKE_CHECK_FUNCS(TYPE)					\
  int								\
  check_arg_ ## TYPE (struct TYPE arg)				\
  {								\
    return cmp_ ## TYPE (arg, ref_val_ ## TYPE);		\
  }								\
								\
  struct TYPE							\
  rtn_str_ ## TYPE (void)					\
  {								\
    return (ref_val_ ## TYPE);					\
  }

#define REF_VAL(NAME) struct NAME ref_val_ ## NAME
#define ES(NAME) struct { } NAME

#if defined tA && ! defined tB

/* Structures with a single field nested to various depths, along with
   some empty structures.  */
struct struct01 { ES(es1); struct { struct { tA a; } s1; } s2; };
struct struct02 { tA a; struct { struct { ES(es1); } s1; } s2; };
struct struct03 { struct { struct { ES(es1); } s1; } s2; ES(es1); struct { struct { tA a; } s3; } s4;};
struct struct04 { ES(es1); ES(es2); tA a; ES(es3); };

int cmp_struct01 (struct struct01 a, struct struct01 b)
{ return a.s2.s1.a == b.s2.s1.a; }

int cmp_struct02 (struct struct02 a, struct struct02 b)
{ return a.a == b.a; }

int cmp_struct03 (struct struct03 a, struct struct03 b)
{ return a.s4.s3.a == b.s4.s3.a; }

int cmp_struct04 (struct struct04 a, struct struct04 b)
{ return a.a == b.a; }

REF_VAL(struct01) = { {}, { { 'a' } } };
REF_VAL(struct02) = { 'a', { { {} } } };
REF_VAL(struct03) = { { { {} } }, {}, { { 'a' } } };
REF_VAL(struct04) = { {}, {}, 'a', {} };

#elif defined tA && defined tB

/* These structures all have 2 fields, nested to various depths, along
   with some empty structures.  */
struct struct01 { struct { tA a; } s1; ES (e1); struct { struct { tB b; } s2;} s3;};
struct struct02 { struct { struct { tA a; } s1; ES(e1); } s2; struct { struct { tB b; } s3;} s4; ES(e2);};
struct struct03 { ES(e1); tA a; ES (e2); struct { struct { tB b; } s2;} s3;};
struct struct04 { ES(e1); ES (e2); struct { struct { struct { tA a; struct { ES(e3); } s1; tB b; } s2; } s3;} s4;};

int cmp_struct01 (struct struct01 a, struct struct01 b)
{ return a.s1.a == b.s1.a && a.s3.s2.b == b.s3.s2.b; }

int cmp_struct02 (struct struct02 a, struct struct02 b)
{ return a.s2.s1.a == b.s2.s1.a && a.s4.s3.b == b.s4.s3.b; }

int cmp_struct03 (struct struct03 a, struct struct03 b)
{ return a.a == b.a && a.s3.s2.b == b.s3.s2.b; }

int cmp_struct04 (struct struct04 a, struct struct04 b)
{ return a.s4.s3.s2.a == b.s4.s3.s2.a && a.s4.s3.s2.b == b.s4.s3.s2.b; }

REF_VAL(struct01) = { { 'a' }, {}, { { '1' } } };
REF_VAL(struct02) = { { { 'a' }, {} }, { { '1' } }, {} };
REF_VAL(struct03) = { {}, 'a', {}, { { '1' } } };
REF_VAL(struct04) = { {}, {}, { { { 'a', {}, '1'} } } } ;

#else

#error "Incorrect configuration of tA and tB defines"

#endif

/* Create all of the functions GDB will call to check functionality.  */
MAKE_CHECK_FUNCS(struct01)
MAKE_CHECK_FUNCS(struct02)
MAKE_CHECK_FUNCS(struct03)
MAKE_CHECK_FUNCS(struct04)

#define CALL_LINE(NAME) val += check_arg_ ## NAME (rtn_str_ ## NAME ())

int
call_all ()
{
  int val;

  CALL_LINE(struct01);
  CALL_LINE(struct02);
  CALL_LINE(struct03);
  CALL_LINE(struct04);

  return (val != 4);
}

void
breakpt (void)
{
  /* Nothing.  */
}

int
main ()
{
  int res;

  res = call_all ();
  breakpt (); /* Break Here.  */
  return res;
}
