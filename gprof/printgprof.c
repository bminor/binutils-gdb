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
static char sccsid[] = "@(#)printgprof.c	5.7 (Berkeley) 6/1/90";
#endif /* not lint */

#include "gprof.h"
#include <demangle.h>

printprof()
{
    register nltype	*np;
    nltype		**sortednlp;
    int			index, timecmp();

    actime = 0.0;
    if ( bsd_style_output ) {
	printf( "\f\n" );
	if ( bflag) {
	    printf( "\n\n\nflat profile:\n" );
	    flat_blurb(stdout);
	}
    }
    else
	printf ("Flat profile:\n");
    flatprofheader();
	/*
	 *	Sort the symbol table in by time
	 */
    sortednlp = (nltype **) calloc( nname , sizeof(nltype *) );
    if ( sortednlp == (nltype **) 0 ) {
	fprintf( stderr , "[printprof] ran out of memory for time sorting\n" );
    }
    for ( index = 0 ; index < nname ; index += 1 ) {
	sortednlp[ index ] = &nl[ index ];
    }
    qsort( sortednlp , nname , sizeof(nltype *) , timecmp );
    for ( index = 0 ; index < nname ; index += 1 ) {
	np = sortednlp[ index ];
	flatprofline( np );
    }
    actime = 0.0;
    free( sortednlp );
    if ( bflag && !bsd_style_output ) {
	flat_blurb(stdout);
    }
}

timecmp( npp1 , npp2 )
    nltype **npp1, **npp2;
{
    double	timediff;
    long	calldiff;

    timediff = (*npp2) -> time - (*npp1) -> time;
    if ( timediff > 0.0 )
	return 1 ;
    if ( timediff < 0.0 )
	return -1;
    calldiff = (*npp2) -> ncall - (*npp1) -> ncall;
    if ( calldiff > 0 )
	return 1;
    if ( calldiff < 0 )
	return -1;
    return( strcmp( (*npp1) -> name , (*npp2) -> name ) );
}

    /*
     *	header for flatprofline
     */
flatprofheader()
{
    
    if (bsd_style_output) {
	printf( "\ngranularity: each sample hit covers %d byte(s)" ,
	       (long) scale * sizeof(UNIT) );
	if (totime > 0.0)
	  printf(" for %.2f%% of %.2f seconds\n\n", 100.0/totime, totime / hz);
    }
    else {
	printf( "\nEach sample counts as %g seconds.\n",
	       1.0 / hz);
    }
    if (totime <= 0.0)
      {
	printf(" no time accumulated\n\n");
	/* This doesn't hurt since all the numerators will be zero.  */
	totime = 1.0;
      }
    printf( "%5.5s %10.10s %8.8s %8.8s %8.8s %8.8s  %-8.8s\n" ,
	    "%  " , "cumulative" , "self  " , "" , "self  " , "total " , "" );
    printf( "%5.5s %10.10s %8.8s %8.8s %8.8s %8.8s  %-8.8s\n" ,
	    "time" , "seconds " , "seconds" , "calls" ,
	    "ms/call" , "ms/call" , "name" );
}

flatprofline( np )
    register nltype	*np;
{

    if ( zflag == 0 && np -> ncall == 0 && np -> time == 0 ) {
	return;
    }
    actime += np -> time;
    if (bsd_style_output)
	printf( "%5.1f %10.2f %8.2f" ,
	       100 * np -> time / totime , actime / hz , np -> time / hz );
    else
	printf( "%6.2f %9.2f %8.2f" ,
	       100 * np -> time / totime , actime / hz , np -> time / hz );
    if ( np -> ncall != 0 ) {
	printf( " %8d %8.2f %8.2f  " , np -> ncall ,
	    1000 * np -> time / hz / np -> ncall ,
	    1000 * ( np -> time + np -> childtime ) / hz / np -> ncall );
    } else {
	printf( " %8.8s %8.8s %8.8s  " , "" , "" , "" );
    }
    if (bsd_style_output)
	printname( np );
    else
	printnameonly( np );
    printf( "\n" );
}

gprofheader()
{

    if (!bsd_style_output)
	if (bflag)
	    printf ("\t\t     Call graph (explanation follows)\n\n");
	else
	    printf ("\t\t\tCall graph\n\n");
    printf( "\ngranularity: each sample hit covers %d byte(s)" ,
	    (long) scale * sizeof(UNIT) );
    if ( printtime > 0.0 ) {
	printf( " for %.2f%% of %.2f seconds\n\n" ,
		100.0/printtime , printtime / hz );
    } else {
	printf( " no time propagated\n\n" );
	    /*
	     *	this doesn't hurt, since all the numerators will be 0.0
	     */
	printtime = 1.0;
    }
    if (bsd_style_output) {
	printf( "%6.6s %5.5s %7.7s %11.11s %7.7s/%-7.7s     %-8.8s\n" ,
	       "" , "" , "" , "" , "called" , "total" , "parents");
	printf( "%-6.6s %5.5s %7.7s %11.11s %7.7s+%-7.7s %-8.8s\t%5.5s\n" ,
	       "index" , "%time" , "self" , "descendents" ,
	       "called" , "self" , "name" , "index" );
	printf( "%6.6s %5.5s %7.7s %11.11s %7.7s/%-7.7s     %-8.8s\n" ,
	       "" , "" , "" , "" , "called" , "total" , "children");
	printf( "\n" );
    } else {
	printf( "index %% time    self  children    called     name\n" );
    }
}

gprofline( np )
    register nltype	*np;
{
    char	kirkbuffer[ BUFSIZ ];

    sprintf( kirkbuffer , "[%d]" , np -> index );
    printf(bsd_style_output
	   ? "%-6.6s %5.1f %7.2f %11.2f"
	   : "%-6.6s %5.1f %7.2f %7.2f" ,
	   kirkbuffer ,
	   100 * ( np -> propself + np -> propchild ) / printtime ,
	   np -> propself / hz ,
	   np -> propchild / hz );
    if ( ( np -> ncall + np -> selfcalls ) != 0 ) {
	printf( " %7d" , np -> ncall );
	if ( np -> selfcalls != 0 ) {
	    printf( "+%-7d " , np -> selfcalls );
	} else {
	    printf( " %7.7s " , "" );
	}
    } else {
	printf( " %7.7s %7.7s " , "" , "" );
    }
    printname( np );
    printf( "\n" );
}

printgprof(timesortnlp)
    nltype	**timesortnlp;
{
    int		index;
    nltype	*parentp;

	/*
	 *	Print out the structured profiling list
	 */
    if ( bflag && bsd_style_output ) {
	bsd_callg_blurb(stdout);
    }
    gprofheader();
    for ( index = 0 ; index < nname + ncycle ; index ++ ) {
	parentp = timesortnlp[ index ];
	if ( zflag == 0 &&
	     parentp -> ncall == 0 &&
	     parentp -> selfcalls == 0 &&
	     parentp -> propself == 0 &&
	     parentp -> propchild == 0 ) {
	    continue;
	}
	if ( ! parentp -> printflag ) {
	    continue;
	}
	if ( parentp -> name == 0 && parentp -> cycleno != 0 ) {
		/*
		 *	cycle header
		 */
	    printcycle( parentp );
	    printmembers( parentp );
	} else {
	    printparents( parentp );
	    gprofline( parentp );
	    printchildren( parentp );
	}
	if (bsd_style_output)
	    printf( "\n" );
	printf( "-----------------------------------------------\n" );
	if (bsd_style_output)
	    printf( "\n" );
    }
    free( timesortnlp );
    if ( bflag && !bsd_style_output) {
	fsf_callg_blurb(stdout);
    }
}

    /*
     *	sort by decreasing propagated time
     *	if times are equal, but one is a cycle header,
     *		say that's first (e.g. less, i.e. -1).
     *	if one's name doesn't have an underscore and the other does,
     *		say the one is first.
     *	all else being equal, sort by names.
     */
int
totalcmp( npp1 , npp2 )
    nltype	**npp1;
    nltype	**npp2;
{
    register nltype	*np1 = *npp1;
    register nltype	*np2 = *npp2;
    double		diff;

    diff =    ( np1 -> propself + np1 -> propchild )
	    - ( np2 -> propself + np2 -> propchild );
    if ( diff < 0.0 )
	    return 1;
    if ( diff > 0.0 )
	    return -1;
    if ( np1 -> name == 0 && np1 -> cycleno != 0 ) 
	return -1;
    if ( np2 -> name == 0 && np2 -> cycleno != 0 )
	return 1;
    if ( np1 -> name == 0 )
	return -1;
    if ( np2 -> name == 0 )
	return 1;
    if ( *(np1 -> name) != '_' && *(np2 -> name) == '_' )
	return -1;
    if ( *(np1 -> name) == '_' && *(np2 -> name) != '_' )
	return 1;
    if ( np1 -> ncall > np2 -> ncall )
	return -1;
    if ( np1 -> ncall < np2 -> ncall ) 
	return 1;
    return strcmp( np1 -> name , np2 -> name );
}

printparents( childp )
    nltype	*childp;
{
    nltype	*parentp;
    arctype	*arcp;
    nltype	*cycleheadp;

    if ( childp -> cyclehead != 0 ) {
	cycleheadp = childp -> cyclehead;
    } else {
	cycleheadp = childp;
    }
    if ( childp -> parents == 0 ) {
	printf(bsd_style_output 
	       ? "%6.6s %5.5s %7.7s %11.11s %7.7s %7.7s     <spontaneous>\n"
	       : "%6.6s %5.5s %7.7s %7.7s %7.7s %7.7s     <spontaneous>\n" ,
		"" , "" , "" , "" , "" , "" );
	return;
    }
    sortparents( childp );
    for ( arcp = childp -> parents ; arcp ; arcp = arcp -> arc_parentlist ) {
	parentp = arcp -> arc_parentp;
	if ( childp == parentp ||
	     ( childp->cycleno != 0 && parentp->cycleno == childp->cycleno ) ) {
		/*
		 *	selfcall or call among siblings
		 */
	    printf(bsd_style_output
		   ? "%6.6s %5.5s %7.7s %11.11s %7d %7.7s     "
		   : "%6.6s %5.5s %7.7s %7.7s %7d %7.7s     " ,
		    "" , "" , "" , "" ,
		    arcp -> arc_count , "" );
	    printname( parentp );
	    printf( "\n" );
	} else {
		/*
		 *	regular parent of child
		 */
	    printf(bsd_style_output
		   ? "%6.6s %5.5s %7.2f %11.2f %7d/%-7d     "
		   : "%6.6s %5.5s %7.2f %7.2f %7d/%-7d     ",
		    "" , "" ,
		    arcp -> arc_time / hz , arcp -> arc_childtime / hz ,
		    arcp -> arc_count , cycleheadp -> ncall );
	    printname( parentp );
	    printf( "\n" );
	}
    }
}

printchildren( parentp )
    nltype	*parentp;
{
    nltype	*childp;
    arctype	*arcp;

    sortchildren( parentp );
    arcp = parentp -> children;
    for ( arcp = parentp -> children ; arcp ; arcp = arcp -> arc_childlist ) {
	childp = arcp -> arc_childp;
	if ( childp == parentp ||
	    ( childp->cycleno != 0 && childp->cycleno == parentp->cycleno ) ) {
		/*
		 *	self call or call to sibling
		 */
	    printf(bsd_style_output
		   ? "%6.6s %5.5s %7.7s %11.11s %7d %7.7s     "
		   : "%6.6s %5.5s %7.7s %7.7s %7d %7.7s     " ,
		   "" , "" , "" , "" , arcp -> arc_count , "" );
	    printname( childp );
	    printf( "\n" );
	} else {
		/*
		 *	regular child of parent
		 */
	    printf(bsd_style_output
		   ? "%6.6s %5.5s %7.2f %11.2f %7d/%-7d     "
		   : "%6.6s %5.5s %7.2f %7.2f %7d/%-7d     " ,
		   "" , "" ,
		   arcp -> arc_time / hz , arcp -> arc_childtime / hz ,
		   arcp -> arc_count , childp -> cyclehead -> ncall );
	    printname( childp );
	    printf( "\n" );
	}
    }
}

/* Print name of symbol.  Return number of characters printed. */

int
printnameonly ( selfp )
    nltype	*selfp;
{
    int size = 0;
    CONST char *name = selfp->name;
    if (name != NULL) {
	char *demangled = NULL;
	if (!bsd_style_output) {
	    if (name[0] == '_' && name[1] && discard_underscores)
		name++;
	    demangled = cplus_demangle (name, DMGL_ANSI|DMGL_PARAMS);
	    if (demangled)
		name = demangled;
	}
	printf( "%s" , name );
	size = strlen (name);
	if (demangled)
	    free (demangled);
#ifdef DEBUG
	if ( debug & DFNDEBUG ) {
	    printf( "{%d} " , selfp -> toporder );
	}
	if ( debug & PROPDEBUG ) {
	    printf( "%5.2f%% " , selfp -> propfraction );
	}
#endif DEBUG
    }
    return size;
}

printname( selfp )
    nltype	*selfp;
{
    printnameonly (selfp);

    if ( selfp -> cycleno != 0 ) {
	printf( " <cycle %d>" , selfp -> cycleno );
    }
    if ( selfp -> index != 0 ) {
	if ( selfp -> printflag ) {
	    printf( " [%d]" , selfp -> index );
	} else {
	    printf( " (%d)" , selfp -> index );
	}
    }
}

sortchildren( parentp )
    nltype	*parentp;
{
    arctype	*arcp;
    arctype	*detachedp;
    arctype	sorted;
    arctype	*prevp;

	/*
	 *	unlink children from parent,
	 *	then insertion sort back on to sorted's children.
	 *	    *arcp	the arc you have detached and are inserting.
	 *	    *detachedp	the rest of the arcs to be sorted.
	 *	    sorted	arc list onto which you insertion sort.
	 *	    *prevp	arc before the arc you are comparing.
	 */
    sorted.arc_childlist = 0;
    for (  (arcp = parentp -> children)&&(detachedp = arcp -> arc_childlist);
	    arcp ;
	   (arcp = detachedp)&&(detachedp = detachedp -> arc_childlist)) {
	    /*
	     *	consider *arcp as disconnected
	     *	insert it into sorted
	     */
	for (   prevp = &sorted ;
		prevp -> arc_childlist ;
		prevp = prevp -> arc_childlist ) {
	    if ( arccmp( arcp , prevp -> arc_childlist ) != LESSTHAN ) {
		break;
	    }
	}
	arcp -> arc_childlist = prevp -> arc_childlist;
	prevp -> arc_childlist = arcp;
    }
	/*
	 *	reattach sorted children to parent
	 */
    parentp -> children = sorted.arc_childlist;
}

sortparents( childp )
    nltype	*childp;
{
    arctype	*arcp;
    arctype	*detachedp;
    arctype	sorted;
    arctype	*prevp;

	/*
	 *	unlink parents from child,
	 *	then insertion sort back on to sorted's parents.
	 *	    *arcp	the arc you have detached and are inserting.
	 *	    *detachedp	the rest of the arcs to be sorted.
	 *	    sorted	arc list onto which you insertion sort.
	 *	    *prevp	arc before the arc you are comparing.
	 */
    sorted.arc_parentlist = 0;
    for (  (arcp = childp -> parents)&&(detachedp = arcp -> arc_parentlist);
	    arcp ;
	   (arcp = detachedp)&&(detachedp = detachedp -> arc_parentlist)) {
	    /*
	     *	consider *arcp as disconnected
	     *	insert it into sorted
	     */
	for (   prevp = &sorted ;
		prevp -> arc_parentlist ;
		prevp = prevp -> arc_parentlist ) {
	    if ( arccmp( arcp , prevp -> arc_parentlist ) != GREATERTHAN ) {
		break;
	    }
	}
	arcp -> arc_parentlist = prevp -> arc_parentlist;
	prevp -> arc_parentlist = arcp;
    }
	/*
	 *	reattach sorted arcs to child
	 */
    childp -> parents = sorted.arc_parentlist;
}

    /*
     *	print a cycle header
     */
printcycle( cyclep )
    nltype	*cyclep;
{
    char	kirkbuffer[ BUFSIZ ];

    sprintf( kirkbuffer , "[%d]" , cyclep -> index );
    printf( "%-6.6s %5.1f %7.2f %11.2f %7d" ,
	    kirkbuffer ,
	    100 * ( cyclep -> propself + cyclep -> propchild ) / printtime ,
	    cyclep -> propself / hz ,
	    cyclep -> propchild / hz ,
	    cyclep -> ncall );
    if ( cyclep -> selfcalls != 0 ) {
	printf( "+%-7d" , cyclep -> selfcalls );
    } else {
	printf( " %7.7s" , "" );
    }
    printf( " <cycle %d as a whole>\t[%d]\n" ,
	    cyclep -> cycleno , cyclep -> index );
}

    /*
     *	print the members of a cycle
     */
printmembers( cyclep )
    nltype	*cyclep;
{
    nltype	*memberp;

    sortmembers( cyclep );
    for ( memberp = cyclep -> cnext ; memberp ; memberp = memberp -> cnext ) {
	printf( "%6.6s %5.5s %7.2f %11.2f %7d" , 
		"" , "" , memberp -> propself / hz , memberp -> propchild / hz ,
		memberp -> ncall );
	if ( memberp -> selfcalls != 0 ) {
	    printf( "+%-7d" , memberp -> selfcalls );
	} else {
	    printf( " %7.7s" , "" );
	}
	printf( "     " );
	printname( memberp );
	printf( "\n" );
    }
}

    /*
     *	sort members of a cycle
     */
sortmembers( cyclep )
    nltype	*cyclep;
{
    nltype	*todo;
    nltype	*doing;
    nltype	*prev;

	/*
	 *	detach cycle members from cyclehead,
	 *	and insertion sort them back on.
	 */
    todo = cyclep -> cnext;
    cyclep -> cnext = 0;
    for (  (doing = todo)&&(todo = doing -> cnext);
	    doing ;
	   (doing = todo )&&(todo = doing -> cnext )){
	for ( prev = cyclep ; prev -> cnext ; prev = prev -> cnext ) {
	    if ( membercmp( doing , prev -> cnext ) == GREATERTHAN ) {
		break;
	    }
	}
	doing -> cnext = prev -> cnext;
	prev -> cnext = doing;
    }
}

    /*
     *	major sort is on propself + propchild,
     *	next is sort on ncalls + selfcalls.
     */
int
membercmp( this , that )
    nltype	*this;
    nltype	*that;
{
    double	thistime = this -> propself + this -> propchild;
    double	thattime = that -> propself + that -> propchild;
    long	thiscalls = this -> ncall + this -> selfcalls;
    long	thatcalls = that -> ncall + that -> selfcalls;

    if ( thistime > thattime ) {
	return GREATERTHAN;
    }
    if ( thistime < thattime ) {
	return LESSTHAN;
    }
    if ( thiscalls > thatcalls ) {
	return GREATERTHAN;
    }
    if ( thiscalls < thatcalls ) {
	return LESSTHAN;
    }
    return EQUALTO;
}
    /*
     *	compare two arcs to/from the same child/parent.
     *	- if one arc is a self arc, it's least.
     *	- if one arc is within a cycle, it's less than.
     *	- if both arcs are within a cycle, compare arc counts.
     *	- if neither arc is within a cycle, compare with
     *		arc_time + arc_childtime as major key
     *		arc count as minor key
     */
int
arccmp( thisp , thatp )
    arctype	*thisp;
    arctype	*thatp;
{
    nltype	*thisparentp = thisp -> arc_parentp;
    nltype	*thischildp = thisp -> arc_childp;
    nltype	*thatparentp = thatp -> arc_parentp;
    nltype	*thatchildp = thatp -> arc_childp;
    double	thistime;
    double	thattime;

#   ifdef DEBUG
	if ( debug & TIMEDEBUG ) {
	    printf( "[arccmp] " );
	    printname( thisparentp );
	    printf( " calls " );
	    printname ( thischildp );
	    printf( " %f + %f %d/%d\n" ,
		    thisp -> arc_time , thisp -> arc_childtime ,
		    thisp -> arc_count , thischildp -> ncall );
	    printf( "[arccmp] " );
	    printname( thatparentp );
	    printf( " calls " );
	    printname( thatchildp );
	    printf( " %f + %f %d/%d\n" ,
		    thatp -> arc_time , thatp -> arc_childtime ,
		    thatp -> arc_count , thatchildp -> ncall );
	    printf( "\n" );
	}
#   endif DEBUG
    if ( thisparentp == thischildp ) {
	    /* this is a self call */
	return LESSTHAN;
    }
    if ( thatparentp == thatchildp ) {
	    /* that is a self call */
	return GREATERTHAN;
    }
    if ( thisparentp -> cycleno != 0 && thischildp -> cycleno != 0 &&
	thisparentp -> cycleno == thischildp -> cycleno ) {
	    /* this is a call within a cycle */
	if ( thatparentp -> cycleno != 0 && thatchildp -> cycleno != 0 &&
	    thatparentp -> cycleno == thatchildp -> cycleno ) {
		/* that is a call within the cycle, too */
	    if ( thisp -> arc_count < thatp -> arc_count ) {
		return LESSTHAN;
	    }
	    if ( thisp -> arc_count > thatp -> arc_count ) {
		return GREATERTHAN;
	    }
	    return EQUALTO;
	} else {
		/* that isn't a call within the cycle */
	    return LESSTHAN;
	}
    } else {
	    /* this isn't a call within a cycle */
	if ( thatparentp -> cycleno != 0 && thatchildp -> cycleno != 0 &&
	    thatparentp -> cycleno == thatchildp -> cycleno ) {
		/* that is a call within a cycle */
	    return GREATERTHAN;
	} else {
		/* neither is a call within a cycle */
	    thistime = thisp -> arc_time + thisp -> arc_childtime;
	    thattime = thatp -> arc_time + thatp -> arc_childtime;
	    if ( thistime < thattime )
		return LESSTHAN;
	    if ( thistime > thattime )
		return GREATERTHAN;
	    if ( thisp -> arc_count < thatp -> arc_count )
		return LESSTHAN;
	    if ( thisp -> arc_count > thatp -> arc_count )
		return GREATERTHAN;
	    return EQUALTO;
	}
    }
}

int
namecmp( npp1 , npp2 )
    nltype **npp1, **npp2;
{
    return( strcmp( (*npp1) -> name , (*npp2) -> name ) );
}

printindex()
{
    nltype		**namesortnlp;
    register nltype	*nlp;
    int			index, nnames, todo, i, j;
    char		peterbuffer[20];

	/*
	 *	Now, sort regular function name alphbetically
	 *	to create an index.
	 */
    namesortnlp = (nltype **) calloc( nname + ncycle , sizeof(nltype *) );
    if ( namesortnlp == (nltype **) 0 ) {
	fprintf( stderr , "%s: ran out of memory for sorting\n" , whoami );
    }
    for ( index = 0 , nnames = 0 ; index < nname ; index++ ) {
	if ( zflag == 0 && nl[index].ncall == 0 && nl[index].time == 0 )
		continue;
	namesortnlp[nnames++] = &nl[index];
    }
    qsort( namesortnlp , nnames , sizeof(nltype *) , namecmp );
    for ( index = 1 , todo = nnames ; index <= ncycle ; index++ ) {
	namesortnlp[todo++] = &cyclenl[index];
    }
    printf( "\f\nIndex by function name\n\n" );
    index = ( todo + 2 ) / 3;
    for ( i = 0; i < index ; i++ ) {
	for ( j = i; j < todo ; j += index ) {
	    nlp = namesortnlp[ j ];
	    if ( nlp -> printflag ) {
		sprintf( peterbuffer , "[%d]" , nlp -> index );
	    } else {
		sprintf( peterbuffer , "(%d)" , nlp -> index );
	    }
	    if ( j < nnames ) {
		printf( "%6.6s " , peterbuffer );
		if (bsd_style_output)
		    printf ("%-19.19s" , nlp->name);
		else {
		    int size = printnameonly(nlp);
		    for ( ; size < 19; size++) putchar(' ');
		}
	    } else {
		printf( "%6.6s " , peterbuffer );
		sprintf( peterbuffer , "<cycle %d>" , nlp -> cycleno );
		printf( "%-19.19s" , peterbuffer );
	    }
	}
	printf( "\n" );
    }
    free( namesortnlp );
}

PTR
xmalloc (size)
     long size;
{
    PTR val = (PTR) malloc (size);
    if (val == NULL) {
	fprintf (stderr, "virtual memory exhaused\n");
	exit (1);
    }
    return val;
}

PTR
xrealloc (oldval, size)
     PTR oldval;
     long size;
{
    PTR val = (PTR) realloc (oldval, size);
    if (val == NULL) {
	fprintf (stderr, "virtual memory exhaused\n");
	exit (1);
    }
    return val;
}

