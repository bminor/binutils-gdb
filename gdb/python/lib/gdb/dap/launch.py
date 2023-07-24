# Copyright 2022-2023 Free Software Foundation, Inc.

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

# These are deprecated in 3.9, but required in older versions.
from typing import Mapping, Optional, Sequence

from .events import ExecutionInvoker
from .server import request, capability
from .startup import send_gdb, send_gdb_with_response, in_gdb_thread, exec_and_log


# The program being launched, or None.  This should only be access
# from the DAP thread.
_program = None


@in_gdb_thread
def _launch_setup(program, cwd, args, env, stopAtBeginningOfMainSubprogram):
    if cwd is not None:
        exec_and_log("cd " + cwd)
    if program is not None:
        exec_and_log("file " + program)
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


# Any parameters here are necessarily extensions -- DAP requires this
# from implementations.  Any additions or changes here should be
# documented in the gdb manual.
@request("launch")
def launch(
    *,
    program: Optional[str] = None,
    cwd: Optional[str] = None,
    args: Sequence[str] = (),
    env: Optional[Mapping[str, str]] = None,
    stopAtBeginningOfMainSubprogram: bool = False,
    **extra,
):
    global _program
    _program = program
    send_gdb(
        lambda: _launch_setup(program, cwd, args, env, stopAtBeginningOfMainSubprogram)
    )


@request("attach")
def attach(*, pid: Optional[int] = None, target: Optional[str] = None, **args):
    # Ensure configurationDone does not try to run.
    global _program
    _program = None
    if pid is not None:
        cmd = "attach " + str(pid)
    elif target is not None:
        cmd = "target remote " + target
    else:
        raise Exception("attach requires either 'pid' or 'target'")
    # Use send_gdb_with_response to ensure we get an error if the
    # attach fails.
    send_gdb_with_response(cmd)
    return None


@capability("supportsConfigurationDoneRequest")
@request("configurationDone")
def config_done(**args):
    global _program
    if _program is not None:
        # Suppress the continue event, but don't set any particular
        # expected stop.
        send_gdb(ExecutionInvoker("run", None))
