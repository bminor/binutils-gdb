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

import json
import queue
import sys

from .io import start_json_writer, read_json
from .startup import (
    in_dap_thread,
    start_thread,
    log,
    log_stack,
    send_gdb_with_response,
)
from .typecheck import type_check


# Map capability names to values.
_capabilities = {}

# Map command names to callables.
_commands = {}

# The global server.
_server = None


class Server:
    """The DAP server class."""

    def __init__(self, in_stream, out_stream, child_stream):
        self.in_stream = in_stream
        self.out_stream = out_stream
        self.child_stream = child_stream
        self.delayed_events = []
        # This queue accepts JSON objects that are then sent to the
        # DAP client.  Writing is done in a separate thread to avoid
        # blocking the read loop.
        if sys.version_info[0] == 3 and sys.version_info[1] <= 6:
            self.write_queue = queue.Queue()
        else:
            self.write_queue = queue.SimpleQueue()
        self.done = False
        global _server
        _server = self

    # Treat PARAMS as a JSON-RPC request and perform its action.
    # PARAMS is just a dictionary from the JSON.
    @in_dap_thread
    def _handle_command(self, params):
        # We don't handle 'cancel' for now.
        result = {
            "request_seq": params["seq"],
            "type": "response",
            "command": params["command"],
        }
        try:
            if "arguments" in params:
                args = params["arguments"]
            else:
                args = {}
            global _commands
            body = _commands[params["command"]](**args)
            if body is not None:
                result["body"] = body
            result["success"] = True
        except BaseException as e:
            log_stack()
            result["success"] = False
            result["message"] = str(e)
        return result

    # Read inferior output and sends OutputEvents to the client.  It
    # is run in its own thread.
    def _read_inferior_output(self):
        while True:
            line = self.child_stream.readline()
            self.send_event(
                "output",
                {
                    "category": "stdout",
                    "output": line,
                },
            )

    # Send OBJ to the client, logging first if needed.
    def _send_json(self, obj):
        log("WROTE: <<<" + json.dumps(obj) + ">>>")
        self.write_queue.put(obj)

    @in_dap_thread
    def main_loop(self):
        """The main loop of the DAP server."""
        # Before looping, start the thread that writes JSON to the
        # client, and the thread that reads output from the inferior.
        start_thread("output reader", self._read_inferior_output)
        start_json_writer(self.out_stream, self.write_queue)
        while not self.done:
            cmd = read_json(self.in_stream)
            log("READ: <<<" + json.dumps(cmd) + ">>>")
            result = self._handle_command(cmd)
            self._send_json(result)
            events = self.delayed_events
            self.delayed_events = []
            for event, body in events:
                self.send_event(event, body)
        # Got the terminate request.  This is handled by the
        # JSON-writing thread, so that we can ensure that all
        # responses are flushed to the client before exiting.
        self.write_queue.put(None)

    @in_dap_thread
    def send_event_later(self, event, body=None):
        """Send a DAP event back to the client, but only after the
        current request has completed."""
        self.delayed_events.append((event, body))

    # Note that this does not need to be run in any particular thread,
    # because it just creates an object and writes it to a thread-safe
    # queue.
    def send_event(self, event, body=None):
        """Send an event to the DAP client.
        EVENT is the name of the event, a string.
        BODY is the body of the event, an arbitrary object."""
        obj = {
            "type": "event",
            "event": event,
        }
        if body is not None:
            obj["body"] = body
        self._send_json(obj)

    def shutdown(self):
        """Request that the server shut down."""
        # Just set a flag.  This operation is complicated because we
        # want to write the result of the request before exiting.  See
        # main_loop.
        self.done = True


def send_event(event, body):
    """Send an event to the DAP client.
    EVENT is the name of the event, a string.
    BODY is the body of the event, an arbitrary object."""
    global _server
    _server.send_event(event, body)


def request(name):
    """A decorator that indicates that the wrapper function implements
    the DAP request NAME."""

    def wrap(func):
        global _commands
        # All requests must run in the DAP thread.
        # Also type-check the calls.
        func = in_dap_thread(type_check(func))
        _commands[name] = func
        return func

    return wrap


def capability(name, value=True):
    """A decorator that indicates that the wrapper function implements
    the DAP capability NAME."""

    def wrap(func):
        global _capabilities
        _capabilities[name] = value
        return func

    return wrap


def client_bool_capability(name):
    """Return the value of a boolean client capability.

    If the capability was not specified, or did not have boolean type,
    False is returned."""
    global _server
    if name in _server.config and isinstance(_server.config[name], bool):
        return _server.config[name]
    return False


@request("initialize")
def initialize(**args):
    global _server, _capabilities
    _server.config = args
    _server.send_event_later("initialized")
    return _capabilities.copy()


@request("terminate")
@capability("supportsTerminateRequest")
def terminate(**args):
    # We can ignore the result here, because we only really need to
    # synchronize.
    send_gdb_with_response("kill")


@request("disconnect")
@capability("supportTerminateDebuggee")
def disconnect(*, terminateDebuggee: bool = False, **args):
    if terminateDebuggee:
        terminate()
    _server.shutdown()
