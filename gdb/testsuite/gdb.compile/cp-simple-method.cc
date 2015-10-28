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

class A
{
public:
  A (void) : a_ (21) {}
  int get_var (void) { return a_; }
  int get_var (unsigned long a) { return 100; }
  int get_var (int a) { return 101; }
  int get_var (float a) { return 102; }
  int get_var (void *a) { return 103;}

  int get_var1 (int n) { return a_ << n; }
  int get_var2 (int incr, unsigned n) { return (a_ + incr) << n; }

  static int get_1 (int a) { return a + 1; }
  static int get_2 (int a, int b) { return a + b + 2; }

private:
  int a_;
};

int
main (void)
{
  A *a = new A ();
  int var = 1234;
  float f = 1.23;
  unsigned long ul = 0xdeadbeef;

  var -= a->get_var ();		// break here
  var -= a->get_var (1);
  var -= a->get_var (ul);
  var -= a->get_var (f);
  var -= a->get_var (a);
  var -= a->get_var1 (1);
  var -= a->get_var2 (1, 2);
  return var - A::get_1 (1) + A::get_2 (1, 2);
}
