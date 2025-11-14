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

package Pack is
   --  With GCC, this makes a directly self-referential pointer type.
   type Direct;
   subtype Sub is Direct;
   type Direct is access Sub;

   --  With GCC, this makes two mutually recursive pointer types.
   type Second;
   type First is access Second;
   type Second is access First;
end Pack;
