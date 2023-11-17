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
from .startup import exec_and_log, in_gdb_thread, log
from .modules import is_module, make_module


# True when the inferior is thought to be running, False otherwise.
# This may be accessed from any thread, which can be racy.  However,
# this unimportant because this global is only used for the
# 'notStopped' response, which itself is inherently racy.
inferior_running = False


@in_gdb_thread
def _on_exit(event):
    global inferior_running
    inferior_running = False
    code = 0
    if hasattr(event, "exit_code"):
        code = event.exit_code
    send_event(
        "exited",
        {
            "exitCode": code,
        },
    )
    send_event("terminated")


@in_gdb_thread
def thread_event(event, reason):
    send_event(
        "thread",
        {
            "reason": reason,
            "threadId": event.inferior_thread.global_num,
        },
    )


@in_gdb_thread
def _new_thread(event):
    global inferior_running
    inferior_running = True
    thread_event(event, "started")


@in_gdb_thread
def _thread_exited(event):
    thread_event(event, "exited")


@in_gdb_thread
def _new_objfile(event):
    if is_module(event.new_objfile):
        send_event(
            "module",
            {
                "reason": "new",
                "module": make_module(event.new_objfile),
            },
        )


@in_gdb_thread
def _objfile_removed(event):
    if is_module(event.objfile):
        send_event(
            "module",
            {
                "reason": "removed",
                "module": make_module(event.objfile),
            },
        )


_suppress_cont = False


@in_gdb_thread
def _cont(event):
    global inferior_running
    inferior_running = True
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
def exec_and_expect_stop(cmd, reason):
    """Indicate that a stop is expected, then execute CMD"""
    global _expected_stop
    _expected_stop = reason
    if reason != StopKinds.PAUSE:
        global _suppress_cont
        _suppress_cont = True
    # FIXME if the call fails should we clear _suppress_cont?
    exec_and_log(cmd)


@in_gdb_thread
def _on_stop(event):
    global inferior_running
    inferior_running = False
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


# This keeps a bit of state between the start of an inferior call and
# the end.  If the inferior was already running when the call started
# (as can happen if a breakpoint condition calls a function), then we
# do not want to emit 'continued' or 'stop' events for the call.  Note
# that, for some reason, gdb.events.cont does not fire for an infcall.
_infcall_was_running = False


@in_gdb_thread
def _on_inferior_call(event):
    global _infcall_was_running
    if isinstance(event, gdb.InferiorCallPreEvent):
        _infcall_was_running = inferior_running
        if not _infcall_was_running:
            _cont(None)
    else:
        if not _infcall_was_running:
            _on_stop(None)


gdb.events.stop.connect(_on_stop)
gdb.events.exited.connect(_on_exit)
gdb.events.new_thread.connect(_new_thread)
gdb.events.thread_exited.connect(_thread_exited)
gdb.events.cont.connect(_cont)
gdb.events.new_objfile.connect(_new_objfile)
gdb.events.free_objfile.connect(_objfile_removed)
gdb.events.inferior_call.connect(_on_inferior_call)
