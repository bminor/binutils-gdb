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

import enum
import gdb

from .server import send_event
from .startup import in_gdb_thread, Invoker, log
from .breakpoint import breakpoint_descriptor


@in_gdb_thread
def _on_exit(event):
    code = 0
    if hasattr(event, "exit_code"):
        code = event.exit_code
    send_event(
        "exited",
        {
            "exitCode": code,
        },
    )


@in_gdb_thread
def _bp_modified(event):
    send_event(
        "breakpoint",
        {
            "reason": "changed",
            "breakpoint": breakpoint_descriptor(event),
        },
    )


@in_gdb_thread
def _bp_created(event):
    send_event(
        "breakpoint",
        {
            "reason": "new",
            "breakpoint": breakpoint_descriptor(event),
        },
    )


@in_gdb_thread
def _bp_deleted(event):
    send_event(
        "breakpoint",
        {
            "reason": "removed",
            "breakpoint": breakpoint_descriptor(event),
        },
    )


@in_gdb_thread
def _new_thread(event):
    send_event(
        "thread",
        {
            "reason": "started",
            "threadId": event.inferior_thread.global_num,
        },
    )


_suppress_cont = False


@in_gdb_thread
def _cont(event):
    global _suppress_cont
    if _suppress_cont:
        log("_suppress_cont case")
        _suppress_cont = False
    else:
        send_event(
            "continued",
            {
                "threadId": gdb.selected_thread().global_num,
                "allThreadsContinued": True,
            },
        )


class StopKinds(enum.Enum):
    # The values here are chosen to follow the DAP spec.
    STEP = "step"
    BREAKPOINT = "breakpoint"
    PAUSE = "pause"
    EXCEPTION = "exception"


_expected_stop = None


@in_gdb_thread
def expect_stop(reason):
    """Indicate that a stop is expected, for the reason given."""
    global _expected_stop
    _expected_stop = reason


# A wrapper for Invoker that also sets the expected stop.
class ExecutionInvoker(Invoker):
    """A subclass of Invoker that sets the expected stop.
    Note that this assumes that the command will restart the inferior,
    so it will also cause ContinuedEvents to be suppressed."""

    def __init__(self, cmd, expected):
        super().__init__(cmd)
        self.expected = expected

    @in_gdb_thread
    def __call__(self):
        expect_stop(self.expected)
        global _suppress_cont
        _suppress_cont = True
        # FIXME if the call fails should we clear _suppress_cont?
        super().__call__()


@in_gdb_thread
def _on_stop(event):
    log("entering _on_stop: " + repr(event))
    global _expected_stop
    obj = {
        "threadId": gdb.selected_thread().global_num,
        "allThreadsStopped": True,
    }
    if isinstance(event, gdb.BreakpointEvent):
        # Ignore the expected stop, we hit a breakpoint instead.
        _expected_stop = StopKinds.BREAKPOINT
        obj["hitBreakpointIds"] = [x.number for x in event.breakpoints]
    elif _expected_stop is None:
        # FIXME what is even correct here
        _expected_stop = StopKinds.EXCEPTION
    obj["reason"] = _expected_stop.value
    _expected_stop = None
    send_event("stopped", obj)


gdb.events.stop.connect(_on_stop)
gdb.events.exited.connect(_on_exit)
gdb.events.breakpoint_created.connect(_bp_created)
gdb.events.breakpoint_modified.connect(_bp_modified)
gdb.events.breakpoint_deleted.connect(_bp_deleted)
gdb.events.new_thread.connect(_new_thread)
gdb.events.cont.connect(_cont)
