# Copyright (C) 2021-2022 Free Software Foundation, Inc.

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

import xml.etree.ElementTree as ET
import gdb

# Make use of gdb.RemoteTargetConnection.send_packet to fetch the
# thread list from the remote target.
#
# Sending existing serial protocol packets like this is not a good
# idea, there should be better ways to get this information using an
# official API, this is just being used as a test case.
#
# Really, the send_packet API would be used to send target
# specific packets to the target, but these are, by definition, target
# specific, so hard to test in a general testsuite.
def get_thread_list_str():
    start_pos = 0
    thread_desc = ""
    conn = gdb.selected_inferior().connection
    if not isinstance(conn, gdb.RemoteTargetConnection):
        raise gdb.GdbError("connection is the wrong type")
    while True:
        str = conn.send_packet("qXfer:threads:read::%d,200" % start_pos).decode("ascii")
        start_pos += 200
        c = str[0]
        str = str[1:]
        thread_desc += str
        if c == "l":
            break
    return thread_desc


# Use gdb.RemoteTargetConnection.send_packet to manually fetch the
# thread list, then extract the thread list using the gdb.Inferior and
# gdb.InferiorThread API.  Compare the two results to ensure we
# managed to successfully read the thread list from the remote.
def run_send_packet_test():
    # Find the IDs of all current threads.
    all_threads = {}
    for inf in gdb.inferiors():
        for thr in inf.threads():
            id = "p%x.%x" % (thr.ptid[0], thr.ptid[1])
            all_threads[id] = False

    # Now fetch the thread list from the remote, and parse the XML.
    str = get_thread_list_str()
    threads_xml = ET.fromstring(str)

    # Look over all threads in the XML list and check we expected to
    # find them, mark the ones we do find.
    for thr in threads_xml:
        id = thr.get("id")
        if not id in all_threads:
            raise "found unexpected thread in remote thread list"
        else:
            all_threads[id] = True

    # Check that all the threads were found in the XML list.
    for id in all_threads:
        if not all_threads[id]:
            raise "thread missingt from remote thread list"

    # Test complete.
    print("Send packet test passed")


# Convert a bytes object to a string.  This follows the same rules as
# the 'maint packet' command so that the output from the two sources
# can be compared.
def bytes_to_string(byte_array):

    # Python 2/3 compatibility.  We need a function that can give us
    # the value of a single element in BYTE_ARRAY as an integer.
    if sys.version_info[0] > 2:
        value_of_single_byte = int
    else:
        value_of_single_byte = ord

    res = ""
    for b in byte_array:
        b = value_of_single_byte(b)
        if b >= 32 and b <= 126:
            res = res + ("%c" % b)
        else:
            res = res + ("\\x%02x" % b)
    return res


# A very simple test for sending the packet that reads the auxv data.
# We convert the result to a string and expect to find some
# hex-encoded bytes in the output.  This test will only work on
# targets that actually supply auxv data.
def run_auxv_send_packet_test(expected_result):
    inf = gdb.selected_inferior()
    conn = inf.connection
    assert isinstance(conn, gdb.RemoteTargetConnection)
    res = conn.send_packet("qXfer:auxv:read::0,1000")
    assert isinstance(res, bytes)
    string = bytes_to_string(res)
    assert string.count("\\x") > 0
    assert string == expected_result
    print("auxv send packet test passed")


# Check that the value of 'global_var' is EXPECTED_VAL.
def check_global_var(expected_val):
    val = int(gdb.parse_and_eval("global_var"))
    val = val & 0xFFFFFFFF
    if val != expected_val:
        raise gdb.GdbError("global_var is 0x%x, expected 0x%x" % (val, expected_val))


# Set the 'X' packet to the remote target to set a global variable.
# Checks that we can send byte values.
def run_set_global_var_test():
    inf = gdb.selected_inferior()
    conn = inf.connection
    assert isinstance(conn, gdb.RemoteTargetConnection)
    addr = gdb.parse_and_eval("&global_var")
    res = conn.send_packet("X%x,4:\x01\x01\x01\x01" % addr)
    assert isinstance(res, bytes)
    check_global_var(0x01010101)
    res = conn.send_packet(b"X%x,4:\x02\x02\x02\x02" % addr)
    assert isinstance(res, bytes)
    check_global_var(0x02020202)
    if sys.version_info[0] > 2:
        # On Python 3 this first attempt will not work as we're
        # passing a Unicode string containing non-ascii characters.
        saw_error = False
        try:
            res = conn.send_packet("X%x,4:\xff\xff\xff\xff" % addr)
        except UnicodeError:
            saw_error = True
        except:
            assert False
        assert saw_error
        check_global_var(0x02020202)
        # Now we pass a bytes object, which will work.
        res = conn.send_packet(b"X%x,4:\xff\xff\xff\xff" % addr)
        check_global_var(0xFFFFFFFF)
    else:
        # On Python 2 we need to force the creation of a Unicode
        # string, but, with that done, we expect to see the same error
        # as on Python 3; the unicode string contains non-ascii
        # characters.
        saw_error = False
        try:
            res = conn.send_packet(unicode("X%x,4:\xff\xff\xff\xff") % addr)
        except UnicodeError:
            saw_error = True
        except:
            assert False
        assert saw_error
        check_global_var(0x02020202)
        # Now we pass a plain string, which, on Python 2, is the same
        # as a bytes object, this, we expect to work.
        res = conn.send_packet("X%x,4:\xff\xff\xff\xff" % addr)
        check_global_var(0xFFFFFFFF)
    print("set global_var test passed")


# Just to indicate the file was sourced correctly.
print("Sourcing complete.")
