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
import os

from .frames import frame_id
from .server import request, capability
from .startup import send_gdb_with_response, in_gdb_thread
from .state import set_thread


# Helper function to safely get the name of a frame as a string.
@in_gdb_thread
def _frame_name(frame):
    name = frame.name()
    if name is None:
        name = "???"
    return name


# Helper function to get a frame's SAL without an error.
@in_gdb_thread
def _safe_sal(frame):
    try:
        return frame.find_sal()
    except gdb.error:
        return None


# Helper function to compute a stack trace.
@in_gdb_thread
def _backtrace(thread_id, levels, startFrame):
    set_thread(thread_id)
    frames = []
    current_number = 0
    try:
        current_frame = gdb.newest_frame()
    except gdb.error:
        current_frame = None
    # Note that we always iterate over all frames, which is lame, but
    # seemingly necessary to support the totalFrames response.
    # FIXME maybe the mildly mysterious note about "monotonically
    # increasing totalFrames values" would let us fix this.
    while current_frame is not None:
        # This condition handles the startFrame==0 case as well.
        if current_number >= startFrame and (levels == 0 or len(frames) < levels):
            newframe = {
                "id": frame_id(current_frame),
                "name": _frame_name(current_frame),
                # This must always be supplied, but we will set it
                # correctly later if that is possible.
                "line": 0,
                # GDB doesn't support columns.
                "column": 0,
                "instructionPointerReference": hex(current_frame.pc()),
            }
            sal = _safe_sal(current_frame)
            if sal is not None and sal.symtab is not None:
                newframe["source"] = {
                    "name": os.path.basename(sal.symtab.filename),
                    "path": sal.symtab.filename,
                    # We probably don't need this but it doesn't hurt
                    # to be explicit.
                    "sourceReference": 0,
                }
                newframe["line"] = sal.line
            frames.append(newframe)
        current_number = current_number + 1
        current_frame = current_frame.older()
    return {
        "stackFrames": frames,
        "totalFrames": current_number,
    }


@request("stackTrace")
@capability("supportsDelayedStackTraceLoading")
def stacktrace(*, levels=0, startFrame=0, threadId, **extra):
    return send_gdb_with_response(lambda: _backtrace(threadId, levels, startFrame))
