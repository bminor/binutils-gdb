
/* This is how the size of an individual .o file's text segment
   is rounded on a sun.  */

#define FILEADDR_ROUND(addr) ((addr + 3) & -4)
