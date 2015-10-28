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

class A;
static int get_values (const A& a);

class A
{
public:
  A(void) : public_ (1), protected_ (2), private_ (3) {}
  int public_;
  static int s_public_;
  friend int get_values (const A&);

protected:
  int protected_;
  static int s_protected_;

private:
  int private_;
  static int s_private_;
};

int A::s_public_ = 10;
int A::s_protected_ = 20;
int A::s_private_ = 30;

static int
get_values (const A& a)
{
  return (a.public_ + a.protected_ + a.private_
	  + a.s_public_ + a.s_protected_ + a.s_private_);
}

int
main (void)
{
  A a;
  int var = 1234;

  return get_values (a);		// break here
}
