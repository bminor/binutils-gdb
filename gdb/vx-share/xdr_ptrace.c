/* xdr_ptrace.c  - xdr routines for remote ptrace calls */
 
/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
/* extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;*/

/*
modification history
--------------------
01a,05jun90,llk  extracted from xdr_ptrace.h, version 01c.
*/

#include <vxWorks.h>
#include <rpc/rpc.h>
#include <xdr_ptrace.h>

#define MAX_LEN 32000

/********************************************************************
*
* xdr_regs_ptr -
*
* xdr routine to get regs* branch of discriminated union ptrace_info
*
*/

LOCAL bool_t xdr_regs_ptr(xdrs,objp)
    XDR *xdrs;
    struct regs **objp;
    {
    return (xdr_pointer(xdrs, (char **) objp, sizeof(struct regs), xdr_regs));
    } /* xdr_regs_ptr */

/********************************************************************
*
* xdr_fp_status_ptr -
*
* xdr routine for fp_status * branch of discrimanated union
*
*/

LOCAL bool_t xdr_fp_status_ptr(xdrs,objp)
    XDR *xdrs;
    struct fp_status **objp;
    {
    return(xdr_pointer(xdrs, (char **) objp, sizeof(struct fp_status), 
	    xdr_fp_status));
    } /* xdr_fp_status_ptr */

#ifndef I80960
/********************************************************************
*
* xdr_fpa_regs_ptr - 
*
* xdr routine for fpa_regs* branch of ptrace_info
*
*/

LOCAL bool_t xdr_fpa_regs_ptr(xdrs,objp)
    XDR *xdrs;
    struct fpa_regs **objp;
    {
    if (! xdr_pointer(xdrs, (char **) objp, sizeof(struct fpa_regs), 
	    	      xdr_fpa_regs)) 
	return(FALSE);
    else
	return(TRUE);
    } /* xdr_fpa_regs_ptr */
#endif

/********************************************************************
*
* xdr_c_bytes_ptr -
*
* xdr routine for counted bytes branch of ptrace_info
*
*/

LOCAL bool_t xdr_c_bytes_ptr(xdrs,objp)
    XDR *xdrs;
    C_bytes **objp;
    {
    return(xdr_pointer(xdrs, (char **) objp, sizeof(C_bytes), xdr_c_bytes));
    } /* xdr_c_bytes_ptr */

/********************************************************************
*
* xdr_ptrace_info -
*
* xdr routine for discriminated union ptrace_info
*
*/

bool_t xdr_ptrace_info(xdrs,objp)
    XDR *xdrs;
    Ptrace_info *objp;
    {
    static struct xdr_discrim choices[] = 
	{
	    { (int) REGS, xdr_regs_ptr },
	    { (int) FPREGS, xdr_fp_status_ptr },
#ifndef I80960
	    { (int) FPAREGS, xdr_fpa_regs_ptr },
#endif
	    { (int) DATA, xdr_c_bytes_ptr },
	    { __dontcare__, NULL }
	};

    return(xdr_union(xdrs, (enum_t *) &objp->ttype, 
	(char *) &objp->more_data, choices, xdr_void));
    } /* xdr_ptrace_info */

/********************************************************************
*
* xdr_rptrace - 
*
* xdr routine for remote ptrace data into server
*
*/

bool_t xdr_rptrace(xdrs,objp)
    XDR *xdrs;
    Rptrace *objp;
    {
    if (! xdr_int(xdrs, &objp->pid)) 
	return(FALSE);
    if (! xdr_int(xdrs, &objp->data)) 
	return(FALSE);
    if (! xdr_int(xdrs, &objp->addr)) 
	return(FALSE);
    if (! xdr_ptrace_info(xdrs, &objp->info)) 
	return(FALSE);

    return(TRUE);
    } /* xdr_rptrace */

/********************************************************************
*
* xdr_ptrace_return - 
*
* xdr routine for remote ptrace data returned by server
*
*/

bool_t xdr_ptrace_return(xdrs, objp)
    XDR *xdrs;
    Ptrace_return *objp;
    {
    if (! xdr_int(xdrs, &objp->status)) 
	return(FALSE);
    if (! xdr_int(xdrs, &objp->errno)) 
	return(FALSE);
    if (! xdr_ptrace_info(xdrs, &objp->info)) 
	return(FALSE);

    return(TRUE);
    } /* xdr_ptrace_return */	

/********************************************************************
*
* xdr_c_bytes -
*
* xdr routine for counted bytes  
*
*/
bool_t xdr_c_bytes(xdrs,objp)
    XDR *xdrs;
    C_bytes *objp;
    {
    return(xdr_bytes(xdrs, &objp->bytes, (u_int *) &objp->len, MAX_LEN));
    } /* xdr_c_bytes */

