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

from .events import ExecutionInvoker
from .server import request, capability
from .startup import send_gdb


_program = None


@request("launch")
def launch(*, program=None, **args):
    if program is not None:
        global _program
        _program = program
        send_gdb(f"file {_program}")


@capability("supportsConfigurationDoneRequest")
@request("configurationDone")
def config_done(**args):
    global _program
    if _program is not None:
        # Suppress the continue event, but don't set any particular
        # expected stop.
        send_gdb(ExecutionInvoker("run", None))
