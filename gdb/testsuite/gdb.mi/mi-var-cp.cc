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
  /*: BEGIN: reference_update :*/
  int x = 167;
  /*: mi_create_varobj "RX" "rx" "create varobj for rx" :*/
  int& rx = x;
  /*: mi_varobj_update RX {RX} "update RX (1)"
      mi_check_varobj_value RX 167 "check RX: expect 167"
      :*/
  x = 567;
  /*: mi_varobj_update RX {RX} "update RX (2)"
      mi_check_varobj_value RX 567 "check RX: expect 567"
      :*/  
  x = 567;
  /*: mi_varobj_update RX {} "update RX (3)"
    :*/

  /*: END: reference_update :*/
}

struct S { int i; int j; };
struct S2 : S {};
        
int base_in_reference_test (S2& s2)
{
  /*: BEGIN: base_in_reference :*/
  return s2.i;
  /*: 
    mi_create_varobj "S2" "s2" "create varobj for s2"
    mi_list_varobj_children "S2" {
       {"S2.S" "S" "1" "S"}
    } "list children of s2"
    mi_list_varobj_children "S2.S" {
       {"S2.S.public" "public" "2"}
    } "list children of s2.s"
    mi_list_varobj_children "S2.S.public" {
       {"S2.S.public.i" "i" "0" "int"}
       {"S2.S.public.j" "j" "0" "int"}
    } "list children of s2.s.public"

    mi_check_varobj_value "S2.S.public.i" "67" "check S2.S.public.i"
    mi_check_varobj_value "S2.S.public.j" "89" "check S2.S.public.j"

  :*/
  /*: END: base_in_reference :*/
}
        
void base_in_reference_test_main ()
{
  S2 s;
  s.i = 67;
  s.j = 89;
  base_in_reference_test (s);
}

int reference_to_pointer ()
{
  S s, *ptr_s, *& rptr_s = ptr_s;
  s.i = 67;
  s.j = 89;
  ptr_s = &s;
  /*: BEGIN: reference_to_pointer :*/
  return 99;
  /*: 
    mi_create_varobj RPTR rptr_s "create varobj for rptr_s"

    mi_list_varobj_children RPTR {{RPTR.public public 2}}	\
    "list public child of RPTR"

    mi_list_varobj_children  RPTR.public	\
    {{RPTR.public.i i 0 int}
    {RPTR.public.j j 0 int}} "list children of reference to pointer"

    mi_check_varobj_value RPTR.public.i 67 "check i member"
    mi_check_varobj_value RPTR.public.j 89 "check j member"
  :*/
  /*: END: reference_to_pointer :*/
}

int main ()
{
  reference_update_tests ();
  base_in_reference_test_main ();
  reference_to_pointer ();
  return 0;
}
