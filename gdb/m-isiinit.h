/* Customize initialize.h for Integrated Solutions machines.  */

/* Define this if you are using system version 4; undefine it for
   version 3.  This alters the action of m-isi-ov.h as well as this file.  */
#define BSD43_ISI40D

#ifdef BSD43_ISI40D
#define FILEADDR_ROUND(addr) (addr)
#else
#define FILEADDR_ROUND(addr) ((addr + 3) & -4)
#endif
