/* Copyright 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

void reference_update_tests ()
{
  int x = 167;
  int& rx = x;
  x = 567;
  x = 567;
}

struct S { int i; int j; };
struct S2 : S {};
        
int base_in_reference_test (S2& s2)
{
  return s2.i;
}
        
void base_in_reference_test_main ()
{
  S2 s;
  s.i = 67;
  s.j = 89;
  base_in_reference_test (s);
}


int main ()
{
  reference_update_tests ();
  base_in_reference_test_main ();
  return 0;
}
