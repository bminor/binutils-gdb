/* This file just picks the correct udiphxxx.h depending on the host */
/* The two hosts that are now defined are SUN and MSDOS */

/* The way this now works, all machines other than DOS are considered to
   be Sun-like.  This is known to work for HP700's.  If this doesn't work
   for some host, create an alternative udiphXXX.h file.  

   This scheme for determining which file to include is *ugly* and should be
   fixed at some later date.  -- zoo - 930106
*/

#ifdef MSDOS
#define CONFIG_INCLUDED
#include "udiphdos.h"
#endif

#ifndef CONFIG_INCLUDED
#include "udiphsun.h"
#endif
