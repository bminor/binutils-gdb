
/*
 * Ovlymgr.c -- Runtime Overlay Manager for the Mitsubishi D10V
 */

#include "ovlymgr.h"

/* Local functions and data: */

extern unsigned long _ovly_table[][4], _novlys;
enum ovly_index { VMA, SIZE, LMA, MAPPED };
enum ovly_direction { IN, OUT };

static void D10VCopy (unsigned long dst, unsigned long src, long size);

/* OverlayLoad:
 * Copy the overlay into its runtime region,
 * and mark the overlay as "mapped".
 */

void
OverlayLoad (int ovlyno)
{
  int i;

  if (ovlyno < 0 || ovlyno >= _novlys)
    exit (-1);	/* fail, bad ovly number */

  if (_ovly_table[ovlyno][MAPPED])
    return;	/* this overlay already mapped -- nothing to do! */

  for (i = 0; i < _novlys; i++)
    if (i == ovlyno)
      _ovly_table[i][MAPPED] = 1;	/* this one now mapped */
    else if (_ovly_table[i][VMA] == _ovly_table[ovlyno][VMA])
      _ovly_table[i][MAPPED] = 0;	/* this one now un-mapped */

  /* copy overlay using D10V DMAP register */
  D10VCopy (_ovly_table[ovlyno][VMA], _ovly_table[ovlyno][LMA], 
	    _ovly_table[ovlyno][SIZE]);
}

/* OverlayUnload:
 * Copy the overlay back into its "load" region.
 * Does NOT mark overlay as "unmapped", therefore may be called 
 * more than once for the same mapped overlay.
 */

void
OverlayUnload (int ovlyno)
{
  if (ovlyno < 0 || ovlyno >= _novlys)
    exit (-1);	/* fail, bad ovly number */

  if (!_ovly_table[ovlyno][MAPPED])
    exit (-1);	/* error, can't copy out a segment that's not "in" */

  D10VCopy (_ovly_table[ovlyno][LMA], _ovly_table[ovlyno][VMA], 
	    _ovly_table[ovlyno][SIZE]);
}

/* D10VCopy:
 * Copy a region of memory from src to dest.
 * Like memcpy, but can copy anywhere in Universal, Data or Instruction memory.
 */

#define IMAP0       (*(int *)(0xff00))
#define IMAP1       (*(int *)(0xff02))
#define DMAP        (*(int *)(0xff04))

static void
D10VTranslate (unsigned long logical,
	       short *dmap,
	       unsigned long **addr)
{
  unsigned long physical;
  unsigned long seg;
  unsigned long off;
  int err = 0;
  /* to access data, we use the following mapping 
     0x00xxxxxx: Logical data address segment        (DMAP translated memory)
     0x01xxxxxx: Logical instruction address segment (IMAP translated memory)
     0x10xxxxxx: Physical data memory segment        (On-chip data memory)
     0x11xxxxxx: Physical instruction memory segment (On-chip insn memory)
     0x12xxxxxx: Phisical unified memory segment     (Unified memory)
     */

  /* Addresses must be correctly aligned */
  if (logical & (sizeof (**addr) - 1))
    exit (-1);

  /* If the address is in one of the two logical address spaces, it is
     first translated into a physical address */
  seg = (logical >> 24);
  off = (logical & 0xffffffL);
  switch (seg) 
      {
      case 0x00: /* in logical data address segment */
	if (off <= 0x7fffL)
	  physical = (0x10L << 24) + off;
	else
	  /* Logical address out side of on-chip segment, not
             supported */
	  exit (-1);
	break;
      case 0x01: /* in logical instruction address segment */
	{
	  short map;
	  if (off <= 0x1ffffL)
	    map = IMAP0;
	  else if (off <= 0x3ffffL)
	    map = IMAP1;
	  else
	    /* Logical address outside of IMAP[01] segment, not
	       supported */
	    exit (-1);
	  if (map & 0x1000L)
	    {
	    /* Instruction memory */
	      physical = (0x11L << 24) | off;
	    }
	  else
	    {
	    /* Unified memory */
	      physical = ((map & 0x7fL) << 17) + (off & 0x1ffffL);
	      if (physical > 0xffffffL)
		/* Address outside of unified address segment */
		exit (-1);
	      physical |= (0x12L << 24);
	    }
	  break;
	}
      case 0x10:
      case 0x11:
      case 0x12:
	physical = logical;
	break;
      default:
	exit (-1);	/* error */
      }

  seg = (physical >> 24);
  off = (physical & 0xffffffL);
  switch (seg) 
    {
    case 0x10:	/* dst is a 15 bit offset into the on-chip memory */
      *dmap = 0;
      *addr = (long *) (0x0000 + ((int)off & 0x7fff));
      break;
    case 0x11:	/* dst is an 18-bit offset into the on-chip
		   instruction memory */
      *dmap = 0x1000L | ((off & 0x3ffffL) >> 14);
      *addr = (long *) (0x8000 + ((int)off & 0x3fff));
      break;
    case 0x12:	/* dst is a 24-bit offset into unified memory */
      *dmap = off >> 14;
      *addr = (long *) (0x8000 + ((int)off & 0x3fff));
      break;
    default:
      exit (-1);	/* error */
    }

#if 0
  printf ("D10VTranslate: logical = %08lx, dmap = %04x, addr = %04x\n",
	  logical, *dmap, *addr);
#endif
}

static void 
D10VCopy (unsigned long dst, unsigned long src, long size)
{
  unsigned long *s, *d, tmp;
  short dmap_src, dmap_dst;
  short dmap_save;

#if 0
  printf ("D10VCopy: dst = %08lx, src = %08lx, size = %ld\n",
	  dst, src, size);
#endif

  /* all section sizes should by multiples of 4 bytes */
  dmap_save = DMAP;

  D10VTranslate (src, &dmap_src, &s);
  D10VTranslate (dst, &dmap_dst, &d);

  while (size > 0)
    {
      /* NB: Transfer 4 byte (long) quantites, problems occure
	 when only two bytes are transfered */
      DMAP = dmap_src;
      tmp = *s;
      DMAP = dmap_dst;
      *d = tmp; 
      d++;
      s++;
      size -= sizeof (tmp);
      src += sizeof (tmp);
      dst += sizeof (tmp);
      if ((src & 0x3fff) == 0)
	D10VTranslate (src, &dmap_src, &s);
      if ((dst & 0x3fff) == 0)
	D10VTranslate (dst, &dmap_dst, &d);
    }
  DMAP = dmap_save;
}

