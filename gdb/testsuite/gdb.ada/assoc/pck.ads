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

package Pck is
   type Posn is (Left, Center, Right);
   type My_Array is array (Posn) of Integer;

   Value : My_Array := (Left => 1, Center => 2, Right => 3);

   type Structured is
     record
       Center : Integer;
     end record;

   SValue : Structured := (Center => 23);

end Pck;
