/* Copyright 2003 Free Software Foundation, Inc.
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
 
   Please email any bugs, comments, and/or additions to this file to:
   bug-gdb@gnu.org
 
   This file is part of the gdb testsuite.  */

void sub (void);

main()
{
        sub();
}

asm(".text\n"
    "    .align 5\n"
    "sub:\n"
    "    mov.l  r14,@-r15\n"
    "    mov.w  .STACK2,r3\n"
    "    sub    r3,r15\n"
    "    mov    r15,r14\n"
    "    mov.w  .STACK2,r7\n"
    "    add    r7,r14\n"
    "    mov    r14,r15\n"
    "    mov.l  @r15+,r14\n"
    "    rts\n"
    "    nop\n"
    "    .align 1\n"
    ".STACK2:\n"
    "    .short  260\n");
