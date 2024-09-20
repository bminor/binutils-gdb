/* This testcase is part of GDB, the GNU debugger.

   Copyright 2024 Free Software Foundation, Inc.

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

#include "attributes.h"

/* A global to do some work on.  This being volatile is important.  Without
   this the compiler might optimise the whole program away.  */
volatile int global = 0;

__attribute__((noinline)) ATTRIBUTE_NOCLONE void
breakpt ()
{
  /* Some filler work.  */
  global++;
}

struct MyClass;

struct ptr
{
  /* The following line is a single line to aid matching in the test
     script.  Sometimes the DWARF will point GDB at the '{' and sometimes
     at the body of the function.  We don't really care for this test, so
     placing everything on one line removes this variability.  */
  MyClass* get_myclass () { return t; }

  MyClass* t;
};

struct MyClass
{
  void call();
};

void
MyClass::call ()
{
  breakpt ();	/* Final breakpoint.  */
}

static void
intermediate (ptr p)
{
  p.get_myclass ()->call ();
}

int
main ()
{
  intermediate (ptr {new MyClass});
}
