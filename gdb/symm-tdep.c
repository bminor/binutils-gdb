// OBSOLETE /* Sequent Symmetry target interface, for GDB.
// OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 2000
// OBSOLETE    Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE /* many 387-specific items of use taken from i386-dep.c */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "frame.h"
// OBSOLETE #include "inferior.h"
// OBSOLETE #include "symtab.h"
// OBSOLETE 
// OBSOLETE #include <signal.h>
// OBSOLETE #include <sys/param.h>
// OBSOLETE #include <sys/user.h>
// OBSOLETE #include <sys/dir.h>
// OBSOLETE #include <sys/ioctl.h>
// OBSOLETE #include "gdb_stat.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include <fcntl.h>
// OBSOLETE 
// OBSOLETE void
// OBSOLETE symmetry_extract_return_value (struct type *type, char *regbuf, char *valbuf)
// OBSOLETE {
// OBSOLETE   union
// OBSOLETE     {
// OBSOLETE       double d;
// OBSOLETE       int l[2];
// OBSOLETE     }
// OBSOLETE   xd;
// OBSOLETE   struct minimal_symbol *msymbol;
// OBSOLETE   float f;
// OBSOLETE 
// OBSOLETE   if (TYPE_CODE_FLT == TYPE_CODE (type))
// OBSOLETE     {
// OBSOLETE       msymbol = lookup_minimal_symbol ("1167_flt", NULL, NULL);
// OBSOLETE       if (msymbol != NULL)
// OBSOLETE 	{
// OBSOLETE 	  /* found "1167_flt" means 1167, %fp2-%fp3 */
// OBSOLETE 	  /* float & double; 19= %fp2, 20= %fp3 */
// OBSOLETE 	  /* no single precision on 1167 */
// OBSOLETE 	  xd.l[1] = *((int *) &regbuf[REGISTER_BYTE (19)]);
// OBSOLETE 	  xd.l[0] = *((int *) &regbuf[REGISTER_BYTE (20)]);
// OBSOLETE 	  switch (TYPE_LENGTH (type))
// OBSOLETE 	    {
// OBSOLETE 	    case 4:
// OBSOLETE 	      /* FIXME: broken for cross-debugging.  */
// OBSOLETE 	      f = (float) xd.d;
// OBSOLETE 	      memcpy (valbuf, &f, TYPE_LENGTH (type));
// OBSOLETE 	      break;
// OBSOLETE 	    case 8:
// OBSOLETE 	      /* FIXME: broken for cross-debugging.  */
// OBSOLETE 	      memcpy (valbuf, &xd.d, TYPE_LENGTH (type));
// OBSOLETE 	      break;
// OBSOLETE 	    default:
// OBSOLETE 	      error ("Unknown floating point size");
// OBSOLETE 	      break;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  /* 387 %st(0), gcc uses this */
// OBSOLETE 	  i387_to_double (((int *) &regbuf[REGISTER_BYTE (3)]),
// OBSOLETE 			  &xd.d);
// OBSOLETE 	  switch (TYPE_LENGTH (type))
// OBSOLETE 	    {
// OBSOLETE 	    case 4:		/* float */
// OBSOLETE 	      f = (float) xd.d;
// OBSOLETE 	      /* FIXME: broken for cross-debugging.  */
// OBSOLETE 	      memcpy (valbuf, &f, 4);
// OBSOLETE 	      break;
// OBSOLETE 	    case 8:		/* double */
// OBSOLETE 	      /* FIXME: broken for cross-debugging.  */
// OBSOLETE 	      memcpy (valbuf, &xd.d, 8);
// OBSOLETE 	      break;
// OBSOLETE 	    default:
// OBSOLETE 	      error ("Unknown floating point size");
// OBSOLETE 	      break;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       memcpy (valbuf, regbuf, TYPE_LENGTH (type));
// OBSOLETE     }
// OBSOLETE }
