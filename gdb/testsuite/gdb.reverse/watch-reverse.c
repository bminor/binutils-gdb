/* This testcase is part of GDB, the GNU debugger.

   Copyright 2008-2025 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <unistd.h>
/*
 *	Since using watchpoints can be very slow, we have to take some pains to
 *	ensure that we don't run too long with them enabled or we run the risk
 *	of having the test timeout.  To help avoid this, we insert some marker
 *	functions in the execution stream so we can set breakpoints at known
 *	locations, without worrying about invalidating line numbers by changing
 *	this file.  We use null bodied functions are markers since gdb does
 *	not support breakpoints at labeled text points at this time.
 *
 *	One place we need is a marker for when we start executing our tests
 *	instructions rather than any process startup code, so we insert one
 *	right after entering main().  Another is right before we finish, before
 *	we start executing any process termination code.
 *
 *	Another problem we have to guard against, at least for the test
 *	suite, is that we need to ensure that the line that causes the
 *	watchpoint to be hit is still the current line when gdb notices
 *	the hit.  Depending upon the specific code generated by the compiler,
 *	the instruction after the one that triggers the hit may be part of
 *	the same line or part of the next line.  Thus we ensure that there
 *	are always some instructions to execute on the same line after the
 *	code that should trigger the hit.
 */

int count = -1;
int ival1 = -1;
int ival2 = -1;
int ival3 = -1;
int ival4 = -1;
int ival5 = -1;
char buf[10];
struct foo
{
  int val;
};
struct foo struct1, struct2, *ptr1, *ptr2;

int doread = 0;

char *global_ptr;

void marker1 ()
{
}

void marker2 ()
{
}

void marker4 ()
{
}

void marker5 ()
{
}

void marker6 ()
{
}

void recurser (int  x)
{
  int  local_x;

  if (x > 0)
    recurser (x-1);
  local_x = x;
}

void
func2 ()
{
  int  local_a;
  static int  static_b;

  ival5++;
  local_a = ival5;
  static_b = local_a;
}

void
func3 ()
{
  int x;
  int y;

  x = 0;
  x = 1;				/* second x assignment */
  y = 1;
  y = 2;
}

int
func1 ()
{
  /* The point of this is that we will set a breakpoint at this call.

     Then, if DECR_PC_AFTER_BREAK equals the size of a function call
     instruction (true on a sun3 if this is gcc-compiled--FIXME we
     should use asm() to make it work for any compiler, present or
     future), then we will end up branching to the location just after
     the breakpoint.  And we better not confuse that with hitting the
     breakpoint.  */
  func2 ();
  return 73;
}

void
func4 ()
{
  buf[0] = 3;
  global_ptr = buf;
  buf[0] = 7;
}

int main ()
{
  struct1.val = 1;
  struct2.val = 2;
  ptr1 = &struct1;
  ptr2 = &struct2;
  marker1 ();
  func1 ();
  for (count = 0; count < 4; count++) {
    ival1 = count;
    ival3 = count; ival4 = count;
  }
  ival1 = count; /* Outside loop */
  ival2 = count;
  ival3 = count; ival4 = count;
  marker2 ();
  if (doread)
    {
      static char msg[] = "type stuff for buf now:";
      write (1, msg, sizeof (msg) - 1);
      read (0, &buf[0], 5);
    }
  marker4 ();

  /* We have a watchpoint on ptr1->val.  It should be triggered if
     ptr1's value changes.  */
  ptr1 = ptr2;

  /* This should not trigger the watchpoint.  If it does, then we
     used the wrong value chain to re-insert the watchpoints or we
     are not evaluating the watchpoint expression correctly.  */
  struct1.val = 5;
  marker5 ();

  /* We have a watchpoint on ptr1->val.  It should be triggered if
     ptr1's value changes.  */
  ptr1 = ptr2;

  /* This should not trigger the watchpoint.  If it does, then we
     used the wrong value chain to re-insert the watchpoints or we
     are not evaluating the watchpoint expression correctly.  */
  struct1.val = 5;
  marker5 ();

  /* We're going to watch locals of func2, to see that out-of-scope
     watchpoints are detected and properly deleted.
     */
  marker6 ();

  /* This invocation is used for watches of a single
     local variable. */
  func2 ();

  /* This invocation is used for watches of an expression
     involving a local variable. */
  func2 ();

  /* This invocation is used for watches of a static
     (non-stack-based) local variable. */
  func2 ();

  /* This invocation is used for watches of a local variable
     when recursion happens.
     */
  marker6 ();
  recurser (2);

  marker6 ();

  func3 ();

  func4 ();

  return 0; /* end of main */
}

