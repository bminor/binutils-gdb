/* CLI utilities.

   Copyright (c) 2011 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef CLI_UTILS_H
#define CLI_UTILS_H

/* *PP is a string denoting a number.  Get the number of the.  Advance
   *PP after the string and any trailing whitespace.

   Currently the string can either be a number or "$" followed by the
   name of a convenience variable.  */

extern int get_number (char **);

/* Parse a number or a range.
   A number will be of the form handled by get_number.
   A range will be of the form <number1> - <number2>, and 
   will represent all the integers between number1 and number2,
   inclusive.

   While processing a range, this fuction is called iteratively;
   At each call it will return the next value in the range.

   At the beginning of parsing a range, the char pointer PP will
   be advanced past <number1> and left pointing at the '-' token.
   Subsequent calls will not advance the pointer until the range
   is completed.  The call that completes the range will advance
   pointer PP past <number2>.  */

extern int get_number_or_range (char **);

/* Skip leading whitespace characters in INP, returning an updated
   pointer.  If INP is NULL, return NULL.  */

extern char *skip_spaces (char *inp);

/* Skip leading non-whitespace characters in INP, returning an updated
   pointer.  If INP is NULL, return NULL.  */

extern char *skip_to_space (char *inp);

#endif /* CLI_UTILS_H */
