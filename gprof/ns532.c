#include "gprof.h"

/*
 * dummy.c -- This file should be used for an unsupported processor type.
 * It does nothing, but prevents findcall() from being unresolved.
 */

findcall( parentp , p_lowpc , p_highpc )
    nltype		*parentp;
    unsigned long	p_lowpc;
    unsigned long	p_highpc;
{
}
