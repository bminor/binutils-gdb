/* This is how the size of an individual .o file's text segment
   is rounded on a NP1.  See np1-pinsn.c for rounding function. */

#define FILEADDR_ROUND(addr) (((int)(addr) + 31) & ~0xf)
