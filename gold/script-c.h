/* script-c.h -- C interface for linker scripts in gold.  */

/* Copyright 2006, 2007 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <iant@google.com>.

   This file is part of gold.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* This file exists so that both the bison parser and script.cc can
   include it, so that they can communicate back and forth.  */

#ifndef GOLD_SCRIPT_C_H
#define GOLD_SCRIPT_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "yyscript.h"

/* The bison parser function.  */

extern int
yyparse(void* closure);

/* Called by the bison parser skeleton to return the next token.  */

extern int
yylex(YYSTYPE*, void* closure);

/* Called by the bison parser skeleton to report an error.  */

extern void
yyerror(void* closure, const char*);

/* Called by the bison parser to add a file to the link.  */

extern void
script_add_file(void* closure, const char*);

/* Called by the bison parser to start and stop a group.  */

extern void
script_start_group(void* closure);
extern void
script_end_group(void* closure);

/* Called by the bison parser to start and end an AS_NEEDED list.  */

extern void
script_start_as_needed(void* closure);
extern void
script_end_as_needed(void* closure);

/* Called by the bison parser to set the entry symbol.  */

extern void
script_set_entry(void* closure, const char*);

/* Called by the bison parser to parse an OPTION.  */
extern void
script_parse_option(void* closure, const char*);

#ifdef __cplusplus
}
#endif

#endif /* !defined(GOLD_SCRIPT_C_H) */
