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
from .events import ExecutionInvoker
from .server import request, capability
from .startup import send_gdb, in_gdb_thread


_program = None


@in_gdb_thread
def _set_args_env(args, env):
    inf = gdb.selected_inferior()
    inf.arguments = args
    if env is not None:
        inf.clear_env()
        for name, value in env.items():
            inf.set_env(name, value)


# Any parameters here are necessarily extensions -- DAP requires this
# from implementations.  Any additions or changes here should be
# documented in the gdb manual.
@request("launch")
def launch(*, program=None, args=[], env=None, **extra):
    if program is not None:
        global _program
        _program = program
        send_gdb(f"file {_program}")
    if len(args) > 0 or env is not None:
        send_gdb(lambda: _set_args_env(args, env))


@capability("supportsConfigurationDoneRequest")
@request("configurationDone")
def config_done(**args):
    global _program
    if _program is not None:
        # Suppress the continue event, but don't set any particular
        # expected stop.
        send_gdb(ExecutionInvoker("run", None))
