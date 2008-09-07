/*
 * gdbfreeplay-i386.c
 *
 * Target-dependent component of gdbfreeplay for i386.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gdbfreeplay.h"

/*
 * Utility functions
 */

/*
 * ix86_hex_to_unsigned_long
 *
 * Convert target-order ascii bytes to host unsigned long.
 * Returns host unsigned long.
 */

static unsigned long
ix86_hex_to_unsigned_long (char *hex)
{
  unsigned long ul;
  int i;

  for (i = 6; i >= 0; i-=2)
    {
      ul <<= 8;
      ul += hex_to_int (hex[i]) << 4;
      ul += hex_to_int (hex[i+1]);
    }
  return ul;
}

/*
 * nybble_to_char
 */

static char
nybble_to_char (int nybble)
{
  if (nybble < 10)
    return '0' + nybble;
  else
    return 'a' + (nybble - 10);
}

/*
 * ix86_unsigned_long_to_hex
 *
 * Convert host unsigned long to target-order ascii string.
 * Return char buffer, static, use before calling again.
 */

static char *
ix86_unsigned_long_to_hex (unsigned long value)
{
  int i;
  static char buf[16];
  char *p;

  p = buf;
  for (i = 0; i < 4; i++)
    {
      /* Intel byte order.  */
      *p++ = nybble_to_char ((value >> 4) & 0xf);
      *p++ = nybble_to_char (value & 0xf);
      value >>= 8;
    }
  *p = '\0';
  return buf;
}

/*
 * target_pc_from_T
 *
 * Extract the PC value from the gdb protocol 'T' packet.
 * Returns PC as host unsigned long long.
 */

unsigned long long
target_pc_from_T (char *tpacket)
{
  char *p;

  if (tpacket[0] == '$' && tpacket[1] == 'T' &&
      (p = strstr (tpacket, ";08:")) != NULL)
    {
      return (unsigned long long) ix86_hex_to_unsigned_long (p + 4);
    }

  /* Fail -- just assume no legitimate PC will ever be -1...  */
  return (unsigned long long) -1;
}

/*
 * ix86_pc_from_registers
 * 
 * Extract the PC value from a gdb protocol registers file.
 * Returns PC as host unsigned long long.
 */

static unsigned long long
ix86_pc_from_registers (char *regs)
{
  return (unsigned long long) ix86_hex_to_unsigned_long (regs + 64);
}

/*
 * target_pc_from_G
 *
 * Extract the PC value from the gdb protocol 'G' packet.
 * Returns PC as host unsigned long long.
 */

unsigned long long
target_pc_from_G (char *gpacket)
{
  if (gpacket[0] == '$' && gpacket[1] == 'G')
    {
      return (unsigned long long) ix86_pc_from_registers (gpacket + 2);
    }

  /* Fail -- just assume no legitimate PC will ever be -1...  */
  return (unsigned long long) -1;
}

/*
 * expand_rle
 *
 * Stubs sometimes send us data with RLE compression.
 * FIXME this code is generic, move into gdbfreeplay-back.c
 * The code for this function lifted from gdb.
 */

static char *
expand_rle (char *input)
{
  static char *buf = NULL;
  static int sizeof_buf;
  long bc = 0;
  int c, repeat;

  /* Allocate buf on first time thru.  */
  if (buf == NULL)
    {
      sizeof_buf = 4096;
      buf = realloc (buf, sizeof_buf);
    }

  while (1)
    {
      c = *input++;
      switch (c) {
      case '#':	/* End of interesting data.  We don't care about checksum.  */
      case '\0':/* End of string data.  */
	buf[bc] = '\0';
	return buf;
	break;
      case '$':
	/* Shouldn't happen, but... */
	fprintf (stderr, "Warning: expand_rle saw '$' in middle of packet.\n");
      default:
	if (bc >= sizeof_buf - 1)
	  {
	    /* Make some more room in the buffer.  */
	    sizeof_buf *= 2;
	    buf = realloc (buf, sizeof_buf);
	  }
	buf[bc++] = c;
	continue;
	break;
      case '*':				/* Run length encoding.  */
	c = *input++;
	repeat = c - ' ' + 3;		/* Compute repeat count.  */

	/* The character before '*' is repeated.  */
	if (bc + repeat - 1 >= sizeof_buf - 1)
	  {
	    /* Make some more room in the buffer.  */
	    sizeof_buf *= 2;
	    buf = realloc (buf, sizeof_buf);
	  }

	memset (&buf[bc], buf[bc - 1], repeat);
	bc += repeat;
	continue;
	break;
      }
    }
}

/*
 * target_pc_from_g
 *
 * Extract the PC value from the gdb protocol 'g' packet reply.
 * 
 * Unlike the two above, this function accepts a FILE pointer
 * rather than a char pointer, and must read data from the file.
 *
 * Returns PC as host unsigned long long.
 */

unsigned long long
target_pc_from_g (char *gpacket)
{
  if (gpacket[0] == 'r' && gpacket[1] == ' ')
    {
      gpacket += 2;
      if (gpacket[0] == '+')
	gpacket++;
      if (gpacket[0] == '$')
	gpacket++;
    }

  return (unsigned long long) ix86_pc_from_registers (expand_rle (gpacket));
}

/*
 * target_compose_T_packet
 *
 * On targets where DECR_PC_AFTER_BREAK is zero, this is a no-op.
 * We just send back the T packet that was sent to us.
 *
 * On targets like i386, where DECR_PC_AFTER_BREAK is non-zero, 
 * it gets complicated.  We have two pieces of information:
 *
 *  1) The address of the current instruction, and
 *  2) Whether we arrived at the current instruction
 *     by virtue of a breakpoint -- ie. whether gdb is
 *     expecting the PC to be off-by-one.
 *
 * Based on that info, we decide whether the T packet that was
 * sent to us is suitable as it is, or else we compose one and
 * send it back.
 *
 * Returns a string T packet, possibly WITHOUT CHECKSUM.
 * We leave it to the caller to handle that, if necessary,
 * because it is target independent and therefore inappropriate
 * to do here.  However the caller has to check for it, because
 * if we simply return the T packet that we received, it will
 * already have a checksum.
 */

char *
target_compose_T_packet (char *origTpacket, 
			 unsigned long long instruction_pc,
			 int breakpoint_p)
{
  unsigned long long origTpacket_pc = target_pc_from_T (origTpacket);
  static char reply_buf[128];
  char *p;

  /* There are four possibilities.
     1) We got here by stepping, and instruction_pc == origTpacket_pc.
     2) We got here by stepping, and instruction_pc != origTpacket_pc.
     3) We got here by breakpoint, and instruction_pc == origTpacket_pc.
     4) We got here by breakpoint, and instruction_pc != origTpacket_pc.

     Actually, there's one more (not well understood):
     5) instruction_pc and origTpacket_pc bear no relationship to each other.

     In that case, we should bail and let gdb get the original Tpacket.  */

  /* Case #5.  */
  if (instruction_pc != origTpacket_pc &&
      instruction_pc != origTpacket_pc - 1)
    return origTpacket;

  /* For #1 and #4, we don't have to do anything, and can return 
     the original T packet.  */

  /* Case #1.  */
  if (!breakpoint_p && (instruction_pc == origTpacket_pc))
    return origTpacket;

  /* Case #4.  */
  if (breakpoint_p && (instruction_pc == origTpacket_pc - 1))
    return origTpacket;

  /* That's it for the easy cases.  For the other two, we have work to do.
     Start by making a copy of the original T packet.  */

  strcpy (reply_buf, origTpacket);
  if ((p = strstr (reply_buf, ";08:")) != NULL)
    {
#if 0
      /* Snip off the PC value from the original T packet.  */
      p[4] = '\0';
      /* Case #2.  */
      if (breakpoint_p)
	{
	  /* Compose a T packet using instruction_pc + 1.  */
	  strcat (p, ix86_unsigned_long_to_hex (instruction_pc + 1));
	}
      else
	{
	  /* Compose a T packet using instruction_pc.  */
	  strcat (p, ix86_unsigned_long_to_hex (instruction_pc));
	}
#else
      /* Insert the new PC value in place 
	 (without disturbing whatever follows it).  */

      /* Case #2.  */
      if (breakpoint_p)
	{
	  /* Compose a T packet using instruction_pc + 1.  */
	  memcpy (p + 4, 
		  ix86_unsigned_long_to_hex (instruction_pc + 1),
		  8);
	}
      else
	{
	  /* Compose a T packet using instruction_pc.  */
	  memcpy (p + 4, 
		  ix86_unsigned_long_to_hex (instruction_pc),
		  8);
	}
#endif
      /* Caller has to recompute checksum.  */
      return reply_buf;
    }
  /* Bail... */
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

#define IX86_TARGET_GBYTES 624
static char gbuffer[IX86_TARGET_GBYTES + 1];
#define IX86_BOGUS_NUMREGS (IX86_TARGET_GBYTES / 8)

static unsigned long regval[IX86_BOGUS_NUMREGS];
static int gotreg[IX86_BOGUS_NUMREGS];

char *
target_compose_g_packet (char *tpac)
{
  int i;
  int regnum;
  int signum;

  /* See which regs we can get from the T packet.  Assume none... */
  for (i = 0; i < IX86_BOGUS_NUMREGS; i++)
    gotreg[i] = 0;

  /* OK, scan off the prefix -- $T plus signal number.  */
  if (*tpac++ == '$' && *tpac++ == 'T')
    {
      /* We won't actually use signum.  */
      signum = (hex_to_int (*tpac++) << 4);
      signum += hex_to_int (*tpac++);
      while (*tpac)
	{
	  regnum = (hex_to_int (*tpac++) << 4);
	  regnum += hex_to_int (*tpac++);
	  if (*tpac++ == ':')
	    {
	      gotreg[regnum] = 1;
	      regval[regnum] = ix86_hex_to_unsigned_long (expand_rle (tpac));
	      tpac = strchr (tpac, ';');
	    }
	  else goto gpacket_fail;

	  if (tpac && *tpac == ';')
	    tpac++;
	  else goto gpacket_fail;

	  if (strncmp (tpac, "thread", 6) == 0)
	    break;
	}

      /* Got values, now get to composin'.  */
      strcpy (gbuffer, "$");
      for (i = 0; i < IX86_BOGUS_NUMREGS; i++)
	if (gotreg[i])
	  strcat (gbuffer, ix86_unsigned_long_to_hex (regval[i]));
	else
	  strcat (gbuffer, "xxxxxxxx");

      /* Return composed g packet reply.
	 Caller has responsibility of appending checksum.  */
      return gbuffer;
    }
 gpacket_fail:
  /* Fail.  */
  return NULL;
}
