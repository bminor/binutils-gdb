/* Copyright 2009 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   This file is part of the gdb testsuite.
   It tests displaced stepping over various insns that require special
   handling.  */

	.text

	.global main
main:
	nop

/* test call/ret */

	.global test_call
test_call:
	call test_call_subr
	nop
	.global test_ret_end
test_ret_end:
	nop

/* test syscall */

	.global test_syscall
	mov $0x14,%eax /* getpid */
test_syscall:
	int $0x80
	nop
test_syscall_end:
	nop

/* all done */

	pushl $0
	call exit
	hlt

/***********************************************/

/* subroutine to help test call/ret */

test_call_subr:
	nop
	.global test_call_end
test_call_end:
	nop

	.global test_ret
test_ret:
	ret
