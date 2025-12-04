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

procedure Prog is
   type Mod1_Type_Base is mod 2 ** 32;
   type Mod2_Type_Base is mod 2 ** 64;

   --  We use subtypes here because GCC emits the above modular types
   --  as base types with the expected size, which gdb then displays
   --  as <4-byte integer> (e.g.).  However we want to see the real
   --  modular type display.
   subtype Mod1_Type is Mod1_Type_Base;
   subtype Mod2_Type is Mod2_Type_Base;

   X : Mod1_Type := 23;
   Y : Mod2_Type := 91;

begin
   null;  -- STOP
end Prog;
