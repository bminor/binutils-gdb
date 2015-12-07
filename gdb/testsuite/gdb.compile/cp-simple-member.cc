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

enum myenum {E_A = 10, E_B, E_C, E_D, E_E};

#if WE_DONT_LIKE_THIS
// Yet? GCC outputs DW_AT_linkage_name="<anon>"
// This *really* messes things up.
namespace {
  typedef enum {AA = 20, AB, AC, AD} ANON_E;
}
#endif

namespace N {
  typedef enum {AA = 20, AB, AC, AD} ANON_E;
}

class A
{
public:
  typedef int ATYPE;

  A(void) : public_ (1), protected_ (N::AB), private_ (3) {}
  ATYPE public_;
  static const myenum s_public_;
  friend ATYPE get_values (const A&);

protected:
  N::ANON_E protected_;
  static N::ANON_E s_protected_;

private:
  ATYPE private_;
  static myenum s_private_;
};

const myenum A::s_public_ = E_A;
N::ANON_E A::s_protected_ = N::AA;
myenum A::s_private_ = E_C;

static A::ATYPE
get_values (const A& a)
{
  A::ATYPE val;

  val = a.public_ + a.private_;	// 1 + 3
  if (a.protected_ == N::AB)	// + 21
    val += 21;
  if (a.s_public_ == E_A)	// +10
    val += 10;
  if (a.s_protected_ == N::AA)	// +20
    val += 20;
  if (a.s_private_ == E_C)	// +30
    val += 30;
  return val;			// = 85
}

int
main (void)
{
  A a;
  int var = 1234;

  return get_values (a);		// break here
}
