/*
 * Changes for 80386 by Pace Willisson (pace@prep.ai.mit.edu)
 * July 1988
 * 
 * i386gnu: COFF_ENCAPSULATE
 */

/*
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#define COFF_ENCAPSULATE

#include "m-i386.h"


#define NAMES_HAVE_UNDERSCORE

#undef COFF_FORMAT
#define READ_DBX_FORMAT

