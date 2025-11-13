# Copyright 2025 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gdb


class TypeViewer(gdb.Command):
    """A command which takes a string and looks up types with that name.

    The types are expected to all be structs.  This command prints a
    basic representation of the struct.  This is only going to work when
    used with the types defined in the gdb-index-many-types test source
    files."""

    def __init__(self):
        super().__init__("py-show-type", gdb.COMMAND_USER)

    def invoke(self, args, from_tty):
        argv = gdb.string_to_argv(args)
        if argv[0] == "":
            raise gdb.GdbError("missing argument")
        print("Looking for type '" + argv[0] + "':")
        syms = gdb.lookup_static_symbols(argv[0], gdb.SYMBOL_TYPE_DOMAIN)
        count = len(syms)
        print("  Found %d type symbol%s" % (count, "" if count == 1 else "s"))
        for i, s in enumerate(syms, start=1):
            t = s.type
            if t is None:
                print("    %d: No type." % i)
            else:
                fields = "struct " + argv[0] + " {"
                try:
                    for f in t.fields():
                        if len(fields) > 0:
                            fields = fields + " "
                        fields = fields + "%s %s;" % (str(f.type), f.name)
                except Exception:
                    pass
                fields = fields + " }"

                print("    %d: %s" % (i, fields))


TypeViewer()
