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


# Map from frame (thread,level) pair to frame ID numbers.  Note we
# can't use the frame itself here as it is not hashable.
_frame_ids = {}

# Map from frame ID number back to frames.
_id_to_frame = {}


# Clear all the frame IDs.
@in_gdb_thread
def _clear_frame_ids(evt):
    global _frame_ids, _id_to_frame
    _frame_ids = {}
    _id_to_frame = {}


# Clear the frame ID map whenever the inferior runs.
gdb.events.cont.connect(_clear_frame_ids)


@in_gdb_thread
def frame_id(frame):
    """Return the frame identifier for FRAME."""
    global _frame_ids, _id_to_frame
    pair = (gdb.selected_thread().global_num, frame.level)
    if pair not in _frame_ids:
        id = len(_frame_ids)
        _frame_ids[pair] = id
        _id_to_frame[id] = frame
    return _frame_ids[pair]


@in_gdb_thread
def frame_for_id(id):
    """Given a frame identifier ID, return the corresponding frame."""
    global _id_to_frame
    return _id_to_frame[id]
