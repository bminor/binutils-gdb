// debug_msg.cc -- a test case for printing debug info for missing symbols.

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

// This file is constructed to have undefined references.  In
// debug_msg.sh, we will try to link this file, and analyze the
// error messages that are produced.

extern int undef_int;
extern float undef_float;
extern void undef_fn1();
extern void undef_fn2();

int* badref1 = &undef_int;
static float* badref2 = &undef_float;
void (*fn_array[])() =
{
  undef_fn1,
  undef_fn2
};

template<class Foo>
int testfn(Foo x)
{
  undef_fn1();
  undef_fn2();
  return undef_int;
}

class Base
{
  virtual void virtfn() { undef_fn1(); }
};

class Derived : public Base
{
  virtual void virtfn() { undef_fn2(); }
};

int main()
{
  testfn(5);
  testfn(4.0);

  Base b;
  Derived d;

  return 0;
}
