/*
 * remote-breakpoint.c
 *
 * A breakpoint list for a remote gdb target.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "remote-breakpoint.h"

static breakpoint *bplist[5];

extern int verbose;

/*
 * insert_breakpoint
 *
 * returns: FAIL/PASS
 */

static enum successcode
insert_breakpoint (enum breakpoint_type bptype,
		   unsigned long        addr,
		   unsigned long        len)
{
  breakpoint *this_bp;

  switch (bptype) {
  case ACCESS_BP:
  case HARDWARE_BP:
  case READ_BP:
  case WRITE_BP:
  default:
    /* Can't do those.  */
    return FAIL;
    break;
  case SOFTWARE_BP:
    this_bp = malloc (sizeof (breakpoint));
    this_bp->addr = addr;
    this_bp->len  = len;
    this_bp->next = bplist[bptype];
    bplist[bptype] = this_bp;
    return PASS;
  }
}
			 
/*
 * unlink_breakpoint
 *
 * returns: 0 for fail, 1 for success
 */

static int
unlink_breakpoint (enum breakpoint_type bptype,
		   unsigned long        addr,
		   unsigned long        len)
{
  breakpoint *this_bp, *tmp;

  switch (bptype) {
  case ACCESS_BP:
  case HARDWARE_BP:
  case READ_BP:
  case WRITE_BP:
  default:
    /* Can't do those.  */
    return FAIL;
    break;
  case SOFTWARE_BP:
    /* Special case - list is empty.  */
    if (bplist[bptype] == NULL)
      return FAIL;

    /* Start from list head.  */
    this_bp = bplist[bptype];
    /* Special case -- remove head of list.  */
    if (this_bp->addr == addr &&
	this_bp->len  == len)
      {
	bplist[bptype] = this_bp->next;
	return PASS;
      }

    /* Scan list.  */
    for (; this_bp && this_bp->next; this_bp = this_bp->next)
      if (this_bp->next->addr == addr &&
	  this_bp->next->len  == len)
	{
	  /* Remove from middle of list.  */
	  tmp = this_bp->next->next;
	  free (this_bp->next);
	  this_bp->next = tmp;
	  return PASS;
	}

    /* Not found.  */
    return FAIL;
  }    
}
			 

extern enum successcode
remote_remove_breakpoint (enum breakpoint_type bptype,
			  unsigned long        addr,
			  unsigned long        len)
{
  if (verbose)
    fprintf (stdout, "remote-breakpoint: Remove sw breakpoint type %d\n", 
	     bptype);
  if (unlink_breakpoint (bptype, addr, len) == 0)
    {
      fprintf (stderr, "       FAILED!\n");
      return FAIL;
    }
  return PASS;
}

extern enum successcode
remote_set_breakpoint (enum breakpoint_type bptype,
		       unsigned long        addr,
		       unsigned long        len)
{
  if (verbose)
    fprintf (stdout, "remote-breakpoint: Set sw breakpoint type %d\n", 
	     bptype);
  if (insert_breakpoint (bptype, addr, len) == 0)
    {
      fprintf (stderr, "       FAILED!\n");
      return FAIL;
    }
  return PASS;
}

/*
 * remote_breakpoint_here_p
 *
 * Scan the list of breakpoints of type BPTYPE.
 * Return PASS if there is one that matches ADDR, else FAIL.
 *
 * FIXME: do I need to consider the length?
 */

enum successcode 
remote_breakpoint_here_p (enum breakpoint_type bptype,
			  unsigned long addr)
{
  breakpoint *bp = bplist[bptype];

  while (bp != NULL)
    {
      if (bp->addr == addr)
	return PASS;
      bp = bp->next;
    }
  return FAIL;
}
