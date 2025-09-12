--  Copyright 2025 Free Software Foundation, Inc.
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

procedure Main is

   type Shorter_Integer is range -2147483648 .. 2147483647;
   type Longer_Integer is range -9223372036854775808 .. 9223372036854775807;
   type My_Array is array (Shorter_Integer range <>,
                           Longer_Integer range <>) of Integer;

   type My_Reg_Acc is access all My_Array;

   Some_Regular_Access : My_Reg_Acc := new My_Array'((1, 2, 3), (4, 5, 6));

begin
   Do_Nothing (Some_Regular_Access'Address); -- STOP
end Main;
