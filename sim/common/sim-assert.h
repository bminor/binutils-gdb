/*  This file is part of the program GDB.

    Copyright (C) 1997, Free Software Foundation, Inc.

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
 
    */


#ifndef _SIM_ASSERT_H_
#define _SIM_ASSERT_H_

# if defined (WITH_ASSERT)
#define SIM_ASSERT(EXPRESSION) \
do { \
  if (WITH_ASSERT) { \
    if (!(EXPRESSION)) { \
      /* strip leading path */ \
      const char *file = __FILE__; \
      const char *p = file; \
      while (*p != '\0' && *p != ':') { \
	if (*p == '/') \
	  file = p; \
	p++; \
      } \
      /* report the failure */ \
      sim_io_error (sd, "%s:%d: assertion failed - %s", \
		    file, __LINE__, #EXPRESSION); \
    } \
  } \
} while (0)
#else
#define SIM_ASSERT(EXPRESSION) do { /*nothing*/; } while (0)
#endif

#if defined (WITH_ASSERT)
#define ASSERT(EXPRESSION) \
do { \
  if (WITH_ASSERT) { \
    if (!(EXPRESSION)) { \
      /* strip leading path */ \
      const char *file = __FILE__; \
      const char *p = file; \
      while (*p != '\0' && *p != ':') { \
	if (*p == '/') \
	  file = p; \
	p++; \
      } \
      /* report the failure */ \
      sim_io_error (NULL, "%s:%d: assertion failed - %s", \
		    file, __LINE__, #EXPRESSION); \
    } \
  } \
} while (0)
#else
#define ASSERT(EXPRESSION) do { /*nothing*/; } while (0)
#endif

#endif
