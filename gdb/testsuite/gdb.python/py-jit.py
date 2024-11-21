# Copyright (C) 2024-2024 Free Software Foundation, Inc.
#
#   This file is part of GDB.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

# This code is same (modulo small tweaks) as the code in documentation,
# section "JIT Interface in Python". If changed the documentation should
# be checked and updated accordingly if necessary.
import gdb

objfile = None
compunit = None
block = None
symtab = None
symbol = None

def get_linetable_entries(addr):
    # Entries are not in increasing order to test that
    # gdb.LineTable.__init__() sorts them properly.
    return [
	gdb.LineTableEntry(31, addr+6, True),
	gdb.LineTableEntry(29, addr, True),
	gdb.LineTableEntry(30, addr+3, True)
    ]


class JITRegisterCode(gdb.Breakpoint):
    def stop(self):

        global objfile 
        global compunit
        global block
        global symtab
        global symbol

        frame = gdb.newest_frame()
        name = frame.read_var('name').string()
        addr = int(frame.read_var('code'))
        size = int(frame.read_var('size'))
        linetable_entries = get_linetable_entries(addr)

        # Create objfile and compunit for allocated "jit" code
        objfile = gdb.Objfile(name)
        compunit = gdb.Compunit(name, objfile, addr, addr + size)

        # Mark the objfile as "jitted code". This will be used later when
        # unregistering discarded code to check the objfile was indeed
        # created for jitted code.
        setattr(objfile, "is_jit_code", True)

        # Create block for jitted function
        block = gdb.Block(compunit.static_block(), addr, addr + size)

        # Create symbol table holding info about jitted function, ...
        symtab = gdb.Symtab("py-jit.c", compunit)
        linetable = gdb.LineTable(symtab, linetable_entries)

        # ...type of the jitted function...
        int64_t = gdb.selected_inferior().architecture().integer_type(64)
        func_t = int64_t.function(int64_t, int64_t)

        # ...and symbol representing jitted function.
        symbol = gdb.Symbol(name, symtab, func_t,
                                gdb.SYMBOL_FUNCTION_DOMAIN, gdb.SYMBOL_LOC_BLOCK,
                                block)

        # Finally, register the symbol in static block
        compunit.static_block().add_symbol(symbol)

        return False # Continue execution

# Create breakpoint to register new code
JITRegisterCode("jit_register_code", internal=True)


class JITUnregisterCode(gdb.Breakpoint):
    def stop(self):
        frame = gdb.newest_frame()
        addr = int(frame.read_var('code'))

        objfile = gdb.current_progspace().objfile_for_address(addr)
        if objfile is None:
            # No objfile for given addr - bail out (this should not happen)
            assert False
            return False # Continue execution

        if not getattr(objfile, "is_jit_code", False):
            # Not a jitted addr - bail out (this should not happen either)
            assert False
            return False # Continue execution

        # Remove the objfile and all debug info associated with it.
        objfile.unlink()

        return False # Continue execution

# Create breakpoint to discard old code
JITUnregisterCode("jit_unregister_code", internal=True)
