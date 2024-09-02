# Copyright 2022-2024 Free Software Foundation, Inc.

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

import re

# These are deprecated in 3.9, but required in older versions.
from typing import Mapping, Optional, Sequence

import gdb

from .events import exec_and_expect_stop, expect_process, expect_stop
from .server import capability, request
from .startup import DAPException, exec_and_log, in_gdb_thread


# A wrapper for the 'file' command that correctly quotes its argument.
@in_gdb_thread
def file_command(program):
    # Handle whitespace, quotes, and backslashes here.  Exactly what
    # to quote depends on libiberty's buildargv and safe-ctype.
    program = re.sub("[ \t\n\r\f\v\\\\'\"]", "\\\\\\g<0>", program)
    exec_and_log("file " + program)


# Any parameters here are necessarily extensions -- DAP requires this
# from implementations.  Any additions or changes here should be
# documented in the gdb manual.
@request("launch", response=False)
def launch(
    *,
    program: Optional[str] = None,
    cwd: Optional[str] = None,
    args: Sequence[str] = (),
    env: Optional[Mapping[str, str]] = None,
    stopAtBeginningOfMainSubprogram: bool = False,
    stopOnEntry: bool = False,
    **extra,
):
    if cwd is not None:
        exec_and_log("cd " + cwd)
    if program is not None:
        file_command(program)
    inf = gdb.selected_inferior()
    if stopAtBeginningOfMainSubprogram:
        main = inf.main_name
        if main is not None:
            exec_and_log("tbreak " + main)
    inf.arguments = args
    if env is not None:
        inf.clear_env()
        for name, value in env.items():
            inf.set_env(name, value)
    expect_process("process")
    exec_and_expect_stop("starti" if stopOnEntry else "run")


@request("attach")
def attach(
    *,
    program: Optional[str] = None,
    pid: Optional[int] = None,
    target: Optional[str] = None,
    **args,
):
    if program is not None:
        file_command(program)
    if pid is not None:
        cmd = "attach " + str(pid)
    elif target is not None:
        cmd = "target remote " + target
    else:
        raise DAPException("attach requires either 'pid' or 'target'")
    expect_process("attach")
    expect_stop("attach")
    exec_and_log(cmd)


@capability("supportsConfigurationDoneRequest")
@request("configurationDone")
def config_done(**args):
    # Nothing to do.
    return None
