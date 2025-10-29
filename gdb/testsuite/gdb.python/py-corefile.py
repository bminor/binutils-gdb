# Copyright (C) 2025 Free Software Foundation, Inc.

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

import pathlib

import gdb


class Mapping:
    def __init__(self, mapping, region):
        self._mapping = mapping
        self._region = region

    @property
    def start(self):
        return self._region.start

    @property
    def end(self):
        return self._region.end

    @property
    def offset(self):
        return self._region.file_offset

    @property
    def filename(self):
        return self._mapping.filename


def info_proc_mappings():
    print("Mapped address spaces:")
    print("")
    format_str = "%-18s %-18s %-18s %-18s %s "
    print(format_str % ("Start Addr", "End Addr", "Size", "Offset", "File"))

    core = gdb.selected_inferior().corefile
    mappings = core.mapped_files()

    result = []
    for m in mappings:
        for r in m.regions:
            result.append(Mapping(m, r))

    result.sort(key=lambda x: x.start)
    for r in result:
        sz = r.end - r.start
        print(
            format_str
            % (
                "0x%016x" % r.start,
                "0x%016x" % r.end,
                "0x%-16x" % sz,
                "0x%-16x" % r.offset,
                "%s" % r.filename,
            )
        )


class InfoProcPyMappings(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, "info proc py-mappings", gdb.COMMAND_DATA)

    def invoke(self, args, from_tty):
        info_proc_mappings()


InfoProcPyMappings()


# Assume that a core file is currently loaded.
#
# Look through all the objfiles for the current inferior, and record
# any that have a build-id.
#
# Then look through the core file mapped files.  Look for entries that
# correspond with the loaded objfiles.  For these matching entries,
# capture the build-id extracted from the core file.
#
# Finally, print a table with the build-id from the objfile, the
# build-id from the core file, and the file name.
#
# This is then processed from the test script to check the build-ids
# match.
class ShowBuildIds(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, "show-build-ids", gdb.COMMAND_DATA)

    def invoke(self, args, from_tty):
        inf = gdb.selected_inferior()
        objfiles = inf.progspace.objfiles()

        path_to_build_id = {}

        # Initial length based on column headings.
        longest_build_id = 18

        for o in objfiles:
            if not o.is_file or o.build_id is None or o.owner is not None:
                continue
            p = pathlib.Path(o.filename).resolve()
            b = o.build_id
            path_to_build_id[p] = {"objfile": b, "corefile": "missing"}
            if len(b) > longest_build_id:
                longest_build_id = len(b)

        core_mapped_files = inf.corefile.mapped_files()
        for m in core_mapped_files:
            p = pathlib.Path(m.filename).resolve()
            b = m.build_id

            if b is not None and len(b) > longest_build_id:
                longest_build_id = len(b)

            if p in path_to_build_id:
                path_to_build_id[p]["corefile"] = b

        format_str = (
            "%-" + str(longest_build_id) + "s  %-" + str(longest_build_id) + "s  %s"
        )

        def make_title(string, length=0):
            if length > 0:
                padding_len = length - len(string)
            else:
                padding_len = 0

            padding = " " * padding_len
            style = gdb.Style("title")
            return style.apply(string) + padding

        print(
            "%s  %s  %s"
            % (
                make_title("Objfile Build-Id", longest_build_id),
                make_title("Core File Build-Id", longest_build_id),
                make_title("File Name"),
            )
        )
        for p, b in path_to_build_id.items():
            print(format_str % (b["objfile"], b["corefile"], p))


ShowBuildIds()


class CheckMainExec(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, "check-main-executable", gdb.COMMAND_DATA)

    def invoke(self, args, from_tty):
        inf = gdb.selected_inferior()
        pspace = inf.progspace
        exec_filename = pathlib.Path(pspace.executable_filename).resolve()

        count = 0
        core_mapped_files = inf.corefile.mapped_files()
        for m in core_mapped_files:
            if not m.is_main_executable:
                continue

            p = pathlib.Path(m.filename).resolve()

            count += 1
            assert exec_filename == p, "main exec filename mismatch"

        assert count == 1, "invalid main executable count"

        print("PASS")


CheckMainExec()


print("Success")
