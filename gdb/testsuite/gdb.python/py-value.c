/* This testcase is part of GDB, the GNU debugger.

   Copyright 2008, 2009, 2010 Free Software Foundation, Inc.

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

struct s
{
  int a;
  int b;
};

union u
{
  int a;
  float b;
};

enum e
  {
    ONE = 1,
    TWO = 2
  };

typedef struct s *PTR;

enum e evalue = TWO;

#ifdef __cplusplus
void ptr_ref(int*& rptr_int)
{
  return; /* break to inspect pointer by reference. */
}
#endif

int
main (int argc, char *argv[])
{
  char *cp = argv[0]; /* Prevent gcc from optimizing argv[] out.  */
  struct s s;
  union u u;
  PTR x = &s;
  char st[17] = "divide et impera";
  char nullst[17] = "divide\0et\0impera";
  int a[3] = {1,2,3};
  int *p = a;
  int i = 2;
  int *ptr_i = &i;

  s.a = 3;
  s.b = 5;
  u.a = 7;

#ifdef __cplusplus
  ptr_ref(ptr_i);
#endif

  return 0;      /* break to inspect struct and union */
}
