--  Copyright 2021-2025 Free Software Foundation, Inc.
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

package body Pck is
   procedure Overloaded (Value : Global_Enum_Type) is
   begin
      null;
   end Overloaded;

   function Overloaded (Value : Character) return Global_Enum_Type is
   begin
      return 'Y';
   end Overloaded;

   procedure Do_Nothing (A : System.Address) is
   begin
      null;
   end Do_Nothing;
end Pck;
