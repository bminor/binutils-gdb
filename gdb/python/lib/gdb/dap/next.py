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

from .events import StopKinds, ExecutionInvoker
from .server import capability, request
from .startup import send_gdb
from .state import set_thread


# Helper function to set the current thread.
def _handle_thread_step(threadId):
    # Ensure we're going to step the correct thread.
    send_gdb(lambda: set_thread(threadId))


@request("next")
def next(*, threadId, granularity="statement", **args):
    _handle_thread_step(threadId)
    cmd = "next"
    if granularity == "instruction":
        cmd += "i"
    send_gdb(ExecutionInvoker(cmd, StopKinds.STEP))


@capability("supportsSteppingGranularity")
@request("stepIn")
def stepIn(*, threadId, granularity="statement", **args):
    _handle_thread_step(threadId)
    cmd = "step"
    if granularity == "instruction":
        cmd += "i"
    send_gdb(ExecutionInvoker(cmd, StopKinds.STEP))


@request("continue")
def continue_request(**args):
    send_gdb(ExecutionInvoker("continue", None))
    return {"allThreadsContinued": True}
