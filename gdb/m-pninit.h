/* This is how the size of an individual .o file's text segment
   is rounded on a Concept.  See pn-pinsn.c for rounding function. */

#define FILEADDR_ROUND(addr) (((int)(addr) + 7) & ~0x7)
