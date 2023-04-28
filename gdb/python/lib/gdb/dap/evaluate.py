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
import gdb.printing

from .frames import frame_for_id
from .server import request
from .startup import send_gdb_with_response, in_gdb_thread
from .varref import find_variable, VariableReference


class EvaluateResult(VariableReference):
    def __init__(self, value):
        super().__init__(None, value, "result")


# Helper function to evaluate an expression in a certain frame.
@in_gdb_thread
def _evaluate(expr, frame_id):
    global_context = True
    if frame_id is not None:
        frame = frame_for_id(frame_id)
        frame.select()
        global_context = False
    val = gdb.parse_and_eval(expr, global_context=global_context)
    ref = EvaluateResult(val)
    return ref.to_object()


# Helper function to evaluate a gdb command in a certain frame.
@in_gdb_thread
def _repl(command, frame_id):
    if frame_id is not None:
        frame = frame_for_id(frame_id)
        frame.select()
    val = gdb.execute(command, from_tty=True, to_string=True)
    return {
        "result": val,
        "variablesReference": 0,
    }


# FIXME supportsVariableType handling
@request("evaluate")
def eval_request(*, expression, frameId=None, context="variables", **args):
    if context in ("watch", "variables"):
        # These seem to be expression-like.
        return send_gdb_with_response(lambda: _evaluate(expression, frameId))
    elif context == "repl":
        return send_gdb_with_response(lambda: _repl(expression, frameId))
    else:
        raise Exception(f'unknown evaluate context "{context}"')


@in_gdb_thread
def _variables(ref, start, count):
    var = find_variable(ref)
    children = var.fetch_children(start, count)
    return [x.to_object() for x in children]


@request("variables")
# Note that we ignore the 'filter' field.  That seems to be
# specific to javascript.
def variables(*, variablesReference, start=0, count=0, **args):
    result = send_gdb_with_response(
        lambda: _variables(variablesReference, start, count)
    )
    return {"variables": result}
