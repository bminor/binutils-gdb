/* Copyright 2015 Free Software Foundation, Inc.

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

struct A
{
  virtual int doit (void) { return 1; }
};

struct B : virtual A
{
  int doit (void) { return 2; }
};

struct C : virtual A
{
  int doit (void) { return 3; }
};

struct D : B, C
{
  int doit (void) { return 4; }
};

int
main (void)
{
  int var = 1234;
  A a;
  B b;
  C c;
  D d;
  A *ap = &d;

  var = (a.doit () + b.doit () + c.doit () + d.doit ()
	 + ap->doit ());	// break here

  return 0;
}

