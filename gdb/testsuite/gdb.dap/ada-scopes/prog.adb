--  Copyright 2023-2025 Free Software Foundation, Inc.
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

with Pack; use Pack;

procedure Prog is
   type Enum_Type is (one, two, three);
   Value : Enum_Type := three;

   My_String : constant String := "Hello World";
begin
   Do_Nothing (Value'address);
   Do_Nothing (My_String'address);  -- STOP
end Prog;
