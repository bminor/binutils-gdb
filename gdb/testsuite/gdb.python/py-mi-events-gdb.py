# Copyright (C) 2016 Free Software Foundation, Inc.

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

# This file is part of the GDB testsuite.  It tests python printing
# to string from event handlers.


import gdb


def signal_stop_handler (event):
    """Stop event handler"""
    assert (isinstance (event, gdb.StopEvent))
    print ("stop_handler")
    print gdb.execute("info break", False, True)


def continue_handler (event):
    """Continue event handler"""
    assert (isinstance (event, gdb.ContinueEvent))
    print ("continue_handler")
    print gdb.execute("info break", False, True)


class test_events (gdb.Command):
    """Test events."""

    def __init__ (self):
        gdb.Command.__init__ (self, "test-events", gdb.COMMAND_STACK)

    def invoke (self, arg, from_tty):
        gdb.events.stop.connect (signal_stop_handler)
        gdb.events.cont.connect (continue_handler)
        print ("Event testers registered.")

test_events ()
