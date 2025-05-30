--  Copyright 2011-2025 Free Software Foundation, Inc.
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

procedure Foo is
   type BA is access all Boolean;
   type BAs is array (1 .. 1) of BA;

   type Blob is record
      Val : Integer;
      Bees : BAs;
   end record;

   My_Blob : Blob := (Val => 1, Bees => (1 => null));
begin
   Do_Nothing (My_Blob'Address);  --  STOP
end Foo;
