#   Copyright 1997-2025 Free Software Foundation, Inc.

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

# This is a test of gdb's ability to follow a process through a
# Unix exec() system call.

# Until "catch exec" is implemented on other targets...
#
require {istarget "*-linux*"}

standard_testfile foll-exec.c

# Compile a program that performs an exec as EXECER_LANG, and a
# program that will be exec'd as EXECEE_LANG.  Either language can be
# 'c' or 'c++'.  Then run various test associated with 'catch exec'
# using the compiled programs.
proc do_exec_tests { execer_lang execee_lang } {
   global srcfile testfile
   global gdb_prompt

   # First compile the program to be exec'd, the execee.
   set execee_base_filename "execd-prog"
   set srcfile2 ${execee_base_filename}.c
   set execee_testfile "execd-prog-${execee_lang}"
   set execee_testfile_re [string_to_regexp $execee_testfile]
   set execee_binfile [standard_output_file $execee_testfile]

   set execee_flags debug
   if { $execee_lang == "c++" } {
       lappend execee_flags "c++"
   }

   if { [build_executable "failed to build $execee_testfile" $execee_testfile \
	     $srcfile2 $execee_flags] == -1 } {
       return
   }

   if { [is_remote target] } {
       gdb_remote_download target $execee_binfile
   }


   # Now compile the program to do the exec, the execer.
   set execer_testfile "$testfile-${execee_lang}"
   set execer_binfile [standard_output_file $execer_testfile]

   set execer_flags debug
   if { $execer_lang == "c++" } {
       lappend execer_flags "c++"
   }
   lappend execer_flags "additional_flags=-DEXECD_PROG=\"${execee_testfile}\""

   if { [build_executable "failed to build $execer_testfile" $execer_testfile \
	     $srcfile $execer_flags] == -1 } {
       return
   }

   # Now we can start running the tests.
   clean_restart $execer_binfile

   # Start the program running, and stop at main.
   if {![runto_main]} {
     return
   }

   # Verify that the system supports "catch exec".
   gdb_test "catch exec" "Catchpoint \[0-9\]* \\(exec\\)" "insert first exec catchpoint"
   set has_exec_catchpoints 0
   gdb_test_multiple "continue" "continue to first exec catchpoint" {
     -re ".*Your system does not support this type\r\nof catchpoint.*$gdb_prompt $" {
       unsupported "continue to first exec catchpoint"
     }
     -re ".*Catchpoint.*$gdb_prompt $" {
       set has_exec_catchpoints 1
       pass "continue to first exec catchpoint"
     }
   }

   if {$has_exec_catchpoints == 0} {
     unsupported "exec catchpoints"
     return
   }

   clean_restart $execer_binfile

   # Start the program running, and stop at main.
   #
   if {![runto_main]} {
     return
   }

   with_test_prefix "1st run" {
       # Execute the code setting up variable PROG.
       set tbreak_line [gdb_get_line_number " tbreak-execlp " $srcfile]
       gdb_test "tbreak ${tbreak_line}" "" "insert breakpoint"
       gdb_continue_to_breakpoint "line tbreak-execlp" ".*execlp \\(.*"
   }

   # Verify that we can see various global and local variables
   # in this program, and that they have expected values.  Some
   # of these variables are also declared in the program we'll
   # exec in a moment.
   #

   send_gdb "print global_i\n"
   gdb_expect {
     -re ".* = 100.*$gdb_prompt $"\
                     {pass "print follow-exec/global_i"}
     -re "$gdb_prompt $" {fail "print follow-exec/global_i"}
     timeout         {fail "(timeout) print follow-exec/global_i"}
   }
   send_gdb "print local_j\n"
   gdb_expect {
     -re ".* = 101.*$gdb_prompt $"\
                     {pass "print follow-exec/local_j"}
     -re "$gdb_prompt $" {fail "print follow-exec/local_j"}
     timeout         {fail "(timeout) print follow-exec/local_j"}
   }
   send_gdb "print local_k\n"
   gdb_expect {
     -re ".* = 102.*$gdb_prompt $"\
                     {pass "print follow-exec/local_k"}
     -re "$gdb_prompt $" {fail "print follow-exec/local_k"}
     timeout         {fail "(timeout) print follow-exec/local_k"}
   }

   # Try stepping through an execlp call, without catching it.
   # We should stop in execd-program, at its first statement.
   #
   set execd_line [gdb_get_line_number "after-exec" $srcfile2]
   send_gdb "next\n"
   gdb_expect {
     -re ".*xecuting new program: .*${execee_testfile_re}.*${srcfile2}:${execd_line}.*int  local_j = argc;.*$gdb_prompt $"\
                     {pass "step through execlp call"}
     -re "$gdb_prompt $" {fail "step through execlp call"}
     timeout         {fail "(timeout) step through execlp call"}
   }

   # Verify that we can see the variables defined in the newly-exec'd
   # program, and CANNOT see those defined in the exec'ing program.
   #
   send_gdb "next\n"
   gdb_expect {
     -re "printf \\(.Hello .*$gdb_prompt $"\
                     {pass "step after execlp call"}
     -re "$gdb_prompt $" {fail "step after execlp call"}
     timeout         {fail "(timeout) step after execlp call"}
   }
   send_gdb "print global_i\n"
   gdb_expect {
     -re ".* = 0.*$gdb_prompt $"\
		     {pass "print execd-program/global_i, after execlp"}
     -re "$gdb_prompt $" {fail "print execd-program/global_i, after execlp"}
     timeout         {fail "(timeout) print execd-program/global_i, after execlp"}
   }
   send_gdb "print local_j\n"
   gdb_expect {
     -re ".* = 2.*$gdb_prompt $"\
		     {pass "print execd-program/local_j, after execlp"}
     -re "$gdb_prompt $" {fail "print execd-program/local_j, after execlp"}
     timeout         {fail "(timeout) print execd-program/local_j, after execlp"}
   }
   send_gdb "print local_k\n"
   gdb_expect {
     -re "No symbol \"local_k\" in current context.*$gdb_prompt $"\
		     {pass "print follow-exec/local_k, after execlp"}
     -re "$gdb_prompt $" {fail "print follow-exec/local_k, after execlp"}
     timeout         {fail "(timeout) print follow-exec/local_k, after execlp"}
   }

   # Explicitly kill this program, or a subsequent rerun actually runs
   # the exec'd program, not the original program...
   clean_restart $execer_binfile

   # Start the program running, and stop at main.
   #
   if {![runto_main]} {
     return
   }

   # Verify that we can catch an exec event, and then continue
   # to follow through the exec.  (Since there's a breakpoint on
   # "main", it'll also be transferred to the exec'd program,
   # and we expect to stop there.)
   #
   send_gdb "catch exec\n"
   gdb_expect {
     -re "Catchpoint .*(exec).*$gdb_prompt $"\
                     {pass "set catch exec"}
     -re "$gdb_prompt $" {fail "set catch exec"}
     timeout         {fail "(timeout) set catch exec"}
   }

   # Verify that the catchpoint is mentioned in an "info breakpoints",
   # and further that the catchpoint mentions no program name.
   #
   set msg "info shows catchpoint without exec pathname"
   gdb_test_multiple "info breakpoints" $msg {
       -re ".*catchpoint.*keep y.*exec\[\n\r\]+$gdb_prompt $" {
           pass $msg
       }
   }

   send_gdb "continue\n"
   gdb_expect {
     -re ".*xecuting new program:.*${execee_testfile_re}.*Catchpoint .*(exec\'d .*${execee_testfile_re}).*$gdb_prompt $"\
                     {pass "hit catch exec"}
     -re "$gdb_prompt $" {fail "hit catch exec"}
     timeout         {fail "(timeout) hit catch exec"}
   }

   # DTS CLLbs16760
   # test gets out of sync if previous test fails.
   gdb_test "bt" ".*" "sync up after possible failure 1"
   gdb_test "bt" "#0.*" "sync up after possible failure 2"

   # Verify that the catchpoint is mentioned in an "info breakpoints",
   # and further that the catchpoint managed to capture the exec'd
   # program's name.
   #
   set msg "info shows catchpoint exec pathname"
   gdb_test_multiple "info breakpoints" $msg {
       -re ".*catchpoint.*keep y.*exec, program \".*${execee_testfile_re}\".*$gdb_prompt $" {
           pass $msg
       }
   }

   # Verify that we can continue from the catchpoint, and land in the
   # main of the newly-exec'd program.
   #
   send_gdb "continue\n"
   gdb_expect {
     -re ".*${srcfile2}:${execd_line}.*$gdb_prompt $"\
                     {pass "continue after hit catch exec"}
     -re "$gdb_prompt $" {fail "continue after hit catch exec"}
     timeout         {fail "(timeout) continue after hit catch exec"}
   }

   # Explicitly kill this program, or a subsequent rerun actually runs
   # the exec'd program, not the original program...
   clean_restart $execer_binfile

   # Start the program running, and stop at main.
   #
   if {![runto_main]} {
     return
   }

   with_test_prefix "2nd run" {
       # Execute the code setting up variable PROG.
       set tbreak_line [gdb_get_line_number " tbreak-execlp " $srcfile]
       gdb_test "tbreak ${tbreak_line}" "" "insert breakpoint"
       gdb_continue_to_breakpoint "line tbreak-execlp" ".*execlp \\(.*"
   }

   # Verify that we can follow through follow an execl()
   # call.  (We must jump around earlier exec* calls.)
   #
   set tbreak_line [gdb_get_line_number " tbreak-execl " $srcfile]
   send_gdb "tbreak ${tbreak_line}\n"
   gdb_expect {
     -re "Temporary breakpoint .*file .*${srcfile}, line ${tbreak_line}.*$gdb_prompt $"\
                     {pass "prepare to jump to execl call"}
     -re "$gdb_prompt $" {fail "prepare to jump to execl call"}
     timeout         {fail "(timeout) prepare to jump to execl call"}
   }
   send_gdb "jump ${tbreak_line}\n"
   gdb_expect {
     -re "main.* at .*${srcfile}:${tbreak_line}.*$gdb_prompt $"\
                     {pass "jump to execl call"}
     -re "$gdb_prompt $" {fail "jump to execl call"}
     timeout         {fail "(timeout) jump to execl call"}
   }
   # Note that stepping through an exec call causes the step-count
   # to be reset to zero.  I.e.: you may specify "next 2" at the
   # call, but you'll actually stop at the first breakpoint set in
   # the newly-exec'd program, not after the remaining step-count
   # reaches zero.
   #
   send_gdb "next 2\n"
   gdb_expect {
     -re ".*xecuting new program: .*${execee_testfile_re}.*${srcfile2}:${execd_line}.*int  local_j = argc;.*$gdb_prompt $"\
                     {pass "step through execl call"}
     -re "$gdb_prompt $" {fail "step through execl call"}
     timeout         {fail "(timeout) step through execl call"}
   }
   send_gdb "next\n"
   gdb_expect {
     -re "printf \\(.Hello .*$gdb_prompt $"\
                     {pass "step after execl call"}
     -re "$gdb_prompt $" {fail "step after execl call"}
     timeout         {fail "(timeout) step after execl call"}
   }

   # Verify that we can print a local variable (which happens to be
   # assigned the value of main's argc).
   #
   send_gdb "print local_j\n"
   gdb_expect {
     -re ".* = 3.*$gdb_prompt $"\
		     {pass "print execd-program/local_j, after execl"}
     -re "$gdb_prompt $" {fail "print execd-program/local_j, after execl"}
     timeout         {fail "(timeout) print execd-program/local_j, after execl"}
   }

   # Explicitly kill this program, or a subsequent rerun actually runs
   # the exec'd program, not the original program...
   clean_restart $execer_binfile

   # Start the program running, and stop at main.
   #
   if {![runto_main]} {
     return
   }

   with_test_prefix "3rd run" {
       # Execute the code setting up variable PROG.
       set tbreak_line [gdb_get_line_number " tbreak-execlp " $srcfile]
       gdb_test "tbreak ${tbreak_line}" "" "insert breakpoint"
       gdb_continue_to_breakpoint "line tbreak-execlp" ".*execlp \\(.*"
   }

   # Verify that we can follow through follow an execv()
   # call.  (We must jump around earlier exec* calls.)
   #
   set tbreak_line [gdb_get_line_number "tbreak-execv"]
   send_gdb "tbreak ${tbreak_line}\n"
   gdb_expect {
     -re "Temporary breakpoint .*file .*${srcfile}, line ${tbreak_line}.*$gdb_prompt $"\
                     {pass "prepare to jump to execv call"}
     -re "$gdb_prompt $" {fail "prepare to jump to execv call"}
     timeout         {fail "(timeout) prepare to jump to execv call"}
   }
   send_gdb "jump ${tbreak_line}\n"
   gdb_expect {
     -re "main.* at .*${srcfile}:${tbreak_line}.*$gdb_prompt $"\
                     {pass "jump to execv call"}
     -re "$gdb_prompt $" {fail "jump to execv call"}
     timeout         {fail "(timeout) jump to execv call"}
   }
   send_gdb "next\n"
   gdb_expect {
     -re ".*xecuting new program: .*${execee_testfile_re}.*${srcfile2}:${execd_line}.*int  local_j = argc;.*$gdb_prompt $"\
                     {pass "step through execv call"}
     -re "$gdb_prompt $" {fail "step through execv call"}
     timeout         {fail "(timeout) step through execv call"}
   }
   send_gdb "next\n"
   gdb_expect {
     -re "printf \\(.Hello .*$gdb_prompt $"\
                     {pass "step after execv call"}
     -re "$gdb_prompt $" {fail "step after execv call"}
     timeout         {fail "(timeout) step after execv call"}
   }

   # Verify that we can print a local variable (which happens to be
   # assigned the value of main's argc).
   #
   send_gdb "print local_j\n"
   gdb_expect {
     -re ".* = 2.*$gdb_prompt $"\
		     {pass "print execd-program/local_j, after execv"}
     -re "$gdb_prompt $" {fail "print execd-program/local_j, after execv"}
     timeout         {fail "(timeout) print execd-program/local_j, after execv"}
   }

   # Explicitly kill this program, or a subsequent rerun actually runs
   # the exec'd program, not the original program...
   clean_restart $execer_binfile

   # Start the program running, and stop at main.
   #
   if {![runto_main]} {
     return
   }

   # Verify that we can just continue and thereby follow through an
   # exec call.  (Since the breakpoint on "main" is reset, we should
   # just stop in main of the newly-exec'd program.)
   #
   send_gdb "continue\n"
   gdb_expect {
     -re ".*xecuting new program: .*${execee_testfile_re}.*${srcfile2}:${execd_line}.*int  local_j = argc;.*$gdb_prompt $"\
                     {pass "continue through exec"}
     -re "$gdb_prompt $" {fail "continue through exec"}
     timeout         {fail "(timeout) continue through exec"}
   }
}

foreach_with_prefix execee_lang { c c++ } {
    do_exec_tests $lang $execee_lang
}
