/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)tahoe.c	1.5 (Berkeley) 6/1/90";
#endif /* not lint */

#include	"gprof.h"

    /*
     *	a namelist entry to be the child of indirect callf
     */
nltype	indirectchild = {
	"(*)" ,				/* the name */
	(unsigned long) 0 ,		/* the pc entry point */
	(unsigned long) 0 ,		/* entry point aligned to histogram */
	(double) 0.0 ,			/* ticks in this routine */
	(double) 0.0 ,			/* cumulative ticks in children */
	(long) 0 ,			/* how many times called */
	(long) 0 ,			/* how many calls to self */
	(double) 1.0 ,			/* propagation fraction */
	(double) 0.0 ,			/* self propagation time */
	(double) 0.0 ,			/* child propagation time */
	(bool) 0 ,			/* print flag */
	(int) 0 ,			/* index in the graph list */
	(int) 0 , 			/* graph call chain top-sort order */
	(int) 0 ,			/* internal number of cycle on */
	(struct nl *) &indirectchild ,	/* pointer to head of cycle */
	(struct nl *) 0 ,		/* pointer to next member of cycle */
	(arctype *) 0 ,			/* list of caller arcs */
	(arctype *) 0 			/* list of callee arcs */
    };

#ifdef	__STDC__
int
iscall (unsigned char *ip)
#else
int iscall(ip)
	unsigned char *ip;
#endif	/* __STDC__ */
{
  if (*ip == 0xeb || *ip == 0x9a) 
    return 1;
  return 0;
}

findcall( parentp , p_lowpc , p_highpc )
    nltype		*parentp;
    unsigned long	p_lowpc;
    unsigned long	p_highpc;
{
    unsigned char	*instructp;
    long		length;
    nltype		*childp;
    unsigned long	destpc;

    if ( textspace == 0 ) {
	return;
    }
    if ( p_lowpc < s_lowpc ) {
	p_lowpc = s_lowpc;
    }
    if ( p_highpc > s_highpc ) {
	p_highpc = s_highpc;
    }
#   ifdef DEBUG
	if ( debug & CALLDEBUG ) {
	    printf( "[findcall] %s: 0x%x to 0x%x\n" ,
		    parentp -> name , p_lowpc , p_highpc );
	}
#   endif DEBUG
    for (   instructp = textspace + p_lowpc ;
	    instructp < textspace + p_highpc ;
	    instructp += length ) {
	length = 1;
	if ( iscall (instructp) ) {
#	    ifdef DEBUG
	  if ( debug & CALLDEBUG ) {
	    printf( "[findcall]\t0x%x:callf" , instructp - textspace );
	  }
#	    endif DEBUG
	  length = 4;
	  /*
	   *	regular pc relative addressing
	   *	check that this is the address of 
	   *	a function.
	   */
	  destpc = ( (unsigned long)instructp + 5 - (unsigned long) textspace);
	  if ( destpc >= s_lowpc && destpc <= s_highpc ) {
	    childp = nllookup( destpc );
#			ifdef DEBUG
	    if ( debug & CALLDEBUG ) {
	      printf( "[findcall]\tdestpc 0x%x" , destpc );
	      printf( " childp->name %s" , childp -> name );
	      printf( " childp->value 0x%x\n" ,
		     childp -> value );
	    }
#			endif DEBUG
	    if ( childp -> value == destpc ) {
	      /*
	       *	a hit
	       */
	      addarc( parentp , childp , (long) 0 );
	      length += 4;	/* constant lengths */
	      continue;
	    }
	    goto botched;
	  }
	  /*
	   *	else:
	   *	it looked like a callf,
	   *	but it wasn't to anywhere.
	   */
	botched:
	  /*
	   *	something funny going on.
	   */
#		    ifdef DEBUG
	  if ( debug & CALLDEBUG ) {
	    printf( "[findcall]\tbut it's a botch\n" );
	  }
#		    endif DEBUG
	  length = 1;
	  continue;
	}
      }
  }
