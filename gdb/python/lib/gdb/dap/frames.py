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

from .startup import in_gdb_thread


# A list of all the frames we've reported.  A frame's index in the
# list is its ID.  We don't use a hash here because frames are not
# hashable.
_all_frames = []


# Clear all the frame IDs.
@in_gdb_thread
def _clear_frame_ids(evt):
    global _all_frames
    _all_frames = []


# Clear the frame ID map whenever the inferior runs.
gdb.events.cont.connect(_clear_frame_ids)


@in_gdb_thread
def frame_id(frame):
    """Return the frame identifier for FRAME."""
    global _all_frames
    for i in range(0, len(_all_frames)):
        if _all_frames[i] == frame:
            return i
    result = len(_all_frames)
    _all_frames.append(frame)
    return result


@in_gdb_thread
def frame_for_id(id):
    """Given a frame identifier ID, return the corresponding frame."""
    global _all_frames
    return _all_frames[id]


@in_gdb_thread
def select_frame(id):
    """Given a frame identifier ID, select the corresponding frame."""
    frame = frame_for_id(id)
    frame.inferior_frame().select()
