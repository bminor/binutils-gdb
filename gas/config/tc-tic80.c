/* tc-tic80.c -- Assemble for the TI TMS320C80 (MV)
   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "as.h"
#include "opcode/tic80.h"


/* Generic assembler global variables which must be defined by all targets. */

/* Characters which always start a comment. */
const char comment_chars[] = ";";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = ";*";

/* Characters which may be used to separate multiple commands on a 
   single line.  */
const char line_separator_chars[] = ";";

/* Characters which are used to indicate an exponent in a floating 
   point number.  */
const char EXP_CHARS[] = "eE";

/* Characters which mean that a number is a floating point constant, 
   as in 0d1.0.  */
const char FLT_CHARS[] = "dD";


/* end of tc-tic80.c */
