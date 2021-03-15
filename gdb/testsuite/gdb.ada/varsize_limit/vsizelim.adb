--  Copyright 2018-2021 Free Software Foundation, Inc.
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 3 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program.  If not, see <http://www.gnu.org/licenses/>.

with Pck; use Pck;
with System;
with Unchecked_Conversion;

procedure VsizeLim is
   Small : String := Ident ("1234567890");
   Larger : String := Ident ("1234567890|1234567890|1234567890");

   type String_Ptr is access all String;
   type Big_String_Ptr is access all String (Positive);

   function To_Ptr is
     new Unchecked_Conversion (System.Address, Big_String_Ptr);

   Name_Str : String_Ptr := new String'(Larger);
   Name : Big_String_Ptr := To_Ptr (Name_Str.all'Address);

begin
   Do_Nothing (Small'Address); -- STOP
   Do_Nothing (Larger'Address);
   Do_Nothing (Name'Address);
end VsizeLim;
