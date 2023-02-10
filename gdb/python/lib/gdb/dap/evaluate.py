# Copyright 2022, 2023 Free Software Foundation, Inc.

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

from .frames import frame_for_id
from .server import request
from .startup import send_gdb_with_response, in_gdb_thread


# Helper function to evaluate an expression in a certain frame.
@in_gdb_thread
def _evaluate(expr, frame_id):
    if frame_id is not None:
        frame = frame_for_id(frame_id)
        frame.select()
    return str(gdb.parse_and_eval(expr))


# FIXME 'format' & hex
# FIXME return a structured response using pretty-printers / varobj
# FIXME supportsVariableType handling
@request("evaluate")
def eval_request(*, expression, frameId=None, **args):
    result = send_gdb_with_response(lambda: _evaluate(expression, frameId))
    return {
        "result": result,
        # FIXME
        "variablesReference": -1,
    }
