/* This testcase is part of GDB, the GNU debugger.

   Copyright 2014 Free Software Foundation, Inc.

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

class A
{
public:
  A () {}
  A (A &obj);

  int a;
};

A::A(A &obj)
{
  a = obj.a;
}

A
f1 (int i1, int i2)
{
  A a;

  a.a = i1 + i2;

  return a;
}

class B
{
public:
  B () {}
  B (const B &obj);

  int b;
};

B::B(const B &obj)
{
  b = obj.b;
}

B
f2 (int i1, int i2)
{
  B b;

  b.b = i1 + i2;

  return b;
}

class C
{
public:
  virtual int method ();

  int c;
};

int
C::method ()
{
  return c;
}

C
f3 (int i1, int i2)
{
  C c;

  c.c = i1 + i2;

  return c;
}

class D
{
public:
  int d;
};

class E : public virtual D
{
public:
  int e;
};

E
f4 (int i1, int i2)
{
  E e;

  e.e = i1 + i2;

  return e;
}

int
main (void)
{
  int i1 = 23;
  int i2 = 100;

  return 0;  /* Break here  */
}
