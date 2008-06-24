/*
 * gdbfreeplay-mips64.c
 *
 * Target-dependent component of gdbfreeplay for mips64.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdbfreeplay.h"

/*
 * target_pc_from_T
 *
 * Extract the PC value from the gdb protocol 'T' packet.
 * Returns PC as host unsigned long.
 */

unsigned long
target_pc_from_T (char *tpacket)
{
  /* Unimplimented -- make caller fall back to using g packet.  */
  return -1;
}

/*
 * target_pc_from_G
 *
 * Extract the PC value from the gdb protocol 'G' packet.
 * Returns PC as host unsigned long.
 */

unsigned long
target_pc_from_G (char *gpacket)
{
  char localbuf [24];

  if (gpacket[0] == '$' && gpacket[1] == 'G')
    {
      strncpy (localbuf, gpacket + 592, 8);
      localbuf[16] = '\0';
      return strtoul (localbuf, NULL, 16);
    }

  /* Fail -- just assume no legitimate PC will ever be -1...  */
  return (unsigned long) -1;
}

/*
 * target_pc_from_g
 *
 * Extract the PC value from the gdb protocol 'g' packet reply.
 * 
 * Unlike the two above, this function accepts a FILE pointer
 * rather than a char pointer, and must read data from the file.
 *
 * Returns PC as host unsigned long.
 */

unsigned long
target_pc_from_g (char *gpacket)
{
  char localbuf [24];

  if (gpacket[0] == 'r' && gpacket[1] == ' ')
    {
      gpacket += 2;
      if (gpacket[0] == '+')
	gpacket++;
      if (gpacket[0] == '$')
	gpacket++;
    }

  strncpy (localbuf, gpacket + 592, 8);
  localbuf[16] = '\0';
  return strtoul (localbuf, NULL, 16);
}



/*
 * target_compose_T_packet
 *
 * On targets where DECR_PC_AFTER_BREAK is zero, this is a no-op.
 * We just send back the T packet that was sent to us.
 *
 */

char *
target_compose_T_packet (char *origTpacket, 
			 unsigned long instruction_pc,
			 int breakpoint_p)
{
  return origTpacket;
}



/*
 * target_compose_g_packet
 *
 * Take the registers from the 'T' packet, and compose them into a 
 * 'g' packet response.  Registers for which we have no values will
 * be filled in with 'xxxx', in the manner of tracepoints.
 *
 * Returns: string, g packet reply.
 */

char *
target_compose_g_packet (char *tpac)
{
  /* stub */
  return NULL;
}
