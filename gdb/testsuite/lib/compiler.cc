/* This test file is part of GDB, the GNU debugger.

   Copyright 1995, 1999, 2003 Free Software Foundation, Inc.

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
   bug-gdb@prep.ai.mit.edu  */

/* Often the behavior of any particular test depends upon what compiler was
   used to compile the test.  As each test is compiled, this file is
   preprocessed by the same compiler used to compile that specific test
   (different tests might be compiled by different compilers, particularly
   if compiled at different times), and used to generate a *.ci (compiler
   info) file for that test.

   I.E., when callfuncs is compiled, a callfuncs.ci file will be generated,
   which can then be sourced by callfuncs.exp to give callfuncs.exp access
   to information about the compilation environment.

   TODO:  It might be a good idea to add expect code that tests each
   definition made with 'set" to see if one already exists, and if so
   warn about conflicts if it is being set to something else.  */

#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
set supports_template_debugging 1
#else
set supports_template_debugging 0
#endif

#if defined(__cplusplus) 
set supports_template_debugging 1
#else
set supports_template_debugging 0
#endif

#if defined (__GNUC__)
set gcc_compiled __GNUC__
set compiler_info [join {gcc __GNUC__ __GNUC_MINOR__ } -]
#else
set gcc_compiled 0
set compiler_info ""
#endif

return 0
