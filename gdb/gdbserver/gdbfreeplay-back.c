/*
 * gdbfreeplay-back.c
 *
 * Backend for gdbfreeplay.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct STOPFRAME {
  /* frame_id -- a unique identifier per stop frame.  */
  unsigned int frame_id;
  /* pc -- address of the next instruction to be "executed".  */
  unsigned long pc;
  /* predecr_pc -- address that will be reported by a breakpoint.
     These two are different on some targets, see gdb source code,
     DECR_PC_AFTER_BREAK.  */
  unsigned long predecr_pc;
  unsigned long eventpos;
  unsigned long gpos;
} StopFrame;

StopFrame *stopframe;
int last_cached_frame = 0;
int framecache_size = 0;
int cur_frame = -1;

#include "gdbfreeplay.h"

/*
 * scan_gdbreplay_file
 *
 * Make a pass through the replay log, noting the positions
 * of the stop events and certain packets within them.
 *
 * Returns: FAIL, PASS.
 */

#define INBUF_SIZE 4096
static char inbuf[INBUF_SIZE];

#define GDB_LINEBUF_SIZE 4096
static char gdb_linebuf[GDB_LINEBUF_SIZE];

static enum successcode
scan_gdbreplay_file (FILE *infile)
{
  /* Make a pass over the entire file -- cache the record positions.  */
  char *line, *p;
  int next_id = 0;
  unsigned long nextpos;
  unsigned long GPC;

  /* First skip empty lines.  */
  do {
    nextpos = ftell (infile);
    line = fgets (inbuf, sizeof (inbuf), infile);
  } while (line != NULL && inbuf[0] == '\n');

  if (line == NULL)
    return FAIL;		/* End of file, nothing there.  */

  /* Now for the main loop.
   *
   * Scan the rest of the file, recording stop events etc.
   */
  do {
    /* 'g' packet message?  */
    if (strstr (line, "$g#67") != NULL)
      {
	/* Record position of 'g' packet (next line).  */
	stopframe[last_cached_frame].gpos = ftell (infile);

	/* See if we need to grab the PC from this packet.  */
	if (stopframe[last_cached_frame].pc == 0 ||
	    stopframe[last_cached_frame].pc == (unsigned long) -1)
	  stopframe[last_cached_frame].pc = target_pc_from_g (infile);

	if (verbose)
	  fprintf (stdout, "Found 'g' packet at %u\n", 
		   stopframe[last_cached_frame].gpos);
      }

    /* Reset PC after breakpoint?  */
    else if ((p = strstr (line, "$G")) != NULL)
      {
	GPC = target_pc_from_G (p);
	if (stopframe[last_cached_frame].pc == 0 ||
	    stopframe[last_cached_frame].pc == (unsigned long) -1)
	  {
	    /* Unlikely, but if we need to, we can just grab this PC.  */
	    stopframe[last_cached_frame].pc = GPC;
	  }
	else if (stopframe[last_cached_frame].pc == GPC + 1)
	  {
	    /* OK, this is gdb decrementing the PC after a breakpoint.  */
	    stopframe[last_cached_frame].predecr_pc =
	      stopframe[last_cached_frame].pc;
	    stopframe[last_cached_frame].pc = GPC;
	  }
      }

    /* Stop event message?  */
    else if ((p = strstr (line, "$T")) != NULL ||
	     (p = strstr (line, "$S")) != NULL)
      {
	if (last_cached_frame >= framecache_size - 1)
	  {
	    /* Time to grow the frame cache buffer.  */
	    framecache_size += 2048;
	    if (verbose)
	      fprintf (stdout, "Growing frame cache to %d\n", 
		       framecache_size);
	    stopframe = realloc (stopframe, 
				 framecache_size * sizeof (stopframe[0]));
	  }
	/* Special case: 0th frame.

	   On the first pass, the FIRST time I see a stop event
	   for the 0th frame, I will not increment the stopframe
	   index.  

	   The 0th frame is special, in that its "stop event"
	   happens not as a result of a real stop event, but
	   as a result of gdb sending the "?" query.  Therefore
	   the apparent event falls in the middle of the frame.
	*/

	if (last_cached_frame != 0 ||
	    stopframe [last_cached_frame].eventpos != 0)
	  last_cached_frame++;

	/* Since we now have a known frame, default to using it.  */
	cur_frame = 0;
	stopframe[last_cached_frame].eventpos = nextpos + p - line;

	if (p[1] == 'T')
	  stopframe[last_cached_frame].pc = target_pc_from_T (p);

	if (verbose)
	  fprintf (stdout, "Record event pos at %u\n",
		   stopframe [last_cached_frame].eventpos);
      }
    nextpos = ftell (infile);
    line = fgets (inbuf, sizeof (inbuf), infile);
  } while (line != NULL);
    
  return PASS;
}

/*
 * gdbfreeplay_open
 *
 * Open the gdbfreeplay "target", which mainly means opening 
 * the gdbreplay log file.
 *
 * Returns: FAIL, PASS.
 */

static FILE *replay_file;

enum successcode
gdbfreeplay_open (char *filename)
{
  if ((replay_file = fopen (filename, "r")) == NULL)
    {
      fprintf (stderr, "GDBFREEPLAY: could not open file %s for input.\n",
	       filename);
      return FAIL;
    }

  return scan_gdbreplay_file (replay_file);
}

/*
 * gdbreadline 
 *
 * Read a line of input from gdb (from the socket).
 */

static char *
gdbreadline (int fd)
{
  char *p = gdb_linebuf;
  int saw_dollar = 0;
  int saw_pound = 0;
  int cksum_chars = 0;

  while (p < gdb_linebuf + sizeof (gdb_linebuf) - 1)
    {
      read (fd, p, 1);
      p[1] = '\0';
      switch (p[0]) {
      case '+':
	/* Receive "ack" from gdb.  
	   Assuming it is first char in buffer, ignore.  */
	if (p - gdb_linebuf != 0)
	  fprintf (stdout, "gdbreadline: '+' ack in middle of line?\n");
	continue;
	break;
      case '-':
	if (p - gdb_linebuf == 0)
	  {
	    /* Receive "nack" from gdb.  FIXME what to do?  */
	    return gdb_linebuf;
	  }
	goto default_label;
	break;
      case '$':
	if (saw_dollar)
	  fprintf (stdout, "gdbreadline: two '$' in one line:\n\t'%s'\n",
		   gdb_linebuf);

	saw_dollar = 1;
	break;
      case '#':
	if (!saw_dollar)
	  fprintf (stdout, "gdbreadline: '#' before '$':\n\t'%s'\n",
		   gdb_linebuf);

	if (saw_pound)
	  fprintf (stdout, "gdbreadline: two '#' in one line:\n\t'%s'\n",
		   gdb_linebuf);

	saw_pound = 1;
	break;
      default:
      default_label:
	if (saw_pound)
	  cksum_chars++;
	if (cksum_chars >= 2)
	  {
	    /* Got a complete message.  */
	    return gdb_linebuf;
	  }
	break;
      }
      ++p;
    }
  /* Shouldn't get here except for buffer overflow.  */
  fprintf (stdout, "gdbreadline: buffer overflow?\n");
  return gdb_linebuf;
}

/*
 * find_reply
 *
 * Given a filepos pointing to just after a given gdb request, 
 * find and return a char * containing the reply to that request.
 *
 * Return NULL on failure.
 */

static char *
find_reply (FILE *logfile, long filepos)
{
  char *line;

  if (filepos == -1)
    return NULL;

  /* Position the file (it's probably already there...)  */
  fseek (logfile, filepos, SEEK_SET);
  /* In principle, the next input line should contain the reply.  */
  line = fgets (inbuf, sizeof (inbuf), logfile);

  return line;
}

/*
 * frame_find_request
 *
 * Given a request from gdb, in the form of a string, 
 * find an identical request within the current frame
 * of the gdbreplay log file.
 *
 * Return a file position immediately following the request.
 */

static long
frame_find_request (FILE *logfile, char *request)
{
  long curpos;
  char *line;

  if (cur_frame >= 0)
    {
      /* Position the input at the beginning of the frame.  
         SPECIAL CASE: for frame zero, go to the beginning of the log.  
	 This is so we can find gdb requests that preceed the first
	 stop-event message.
      */
      if (cur_frame == 0)
	curpos = 0;
      else
	curpos = stopframe[cur_frame].eventpos;
      fseek (logfile, curpos, SEEK_SET);
      /* Now search for a matching request.  */
      while (curpos < stopframe[cur_frame + 1].eventpos)
	{
	  line = fgets (inbuf, sizeof (inbuf), logfile);
	  /* End of input?  */
	  if (line == NULL)
	    break;
	  curpos = ftell (logfile);
	  if (strstr (line, request) != NULL)
	    {
	      /* Matching request found.  Return position.  */
	      return curpos;
	    }
	}
    }

  /* Not found.  */
  return -1;
}

/*
 * gdb_ack
 * 
 * Send an ack to gdb.
 * Returns void.
 */

static void
gdb_ack (int fd)
{
  static const char *ack = "+";
  write (fd, ack, 1);
}

/*
 * hex_to_int
 */

int 
hex_to_int (int c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else if (c >= 'A' && c <= 'F')
    return c - 'F' + 10;
  else
    return 0;
}

/*
 * int_to_hex
 *
 * (lifted from gdb)
 */

int 
int_to_hex (int nib)
{
  if (nib < 10)
    return '0' + nib;
  else
    return 'a' + nib - 10;
}

#if 0
/*
 * tohex
 *
 * Borrowed from gdbreplay.
 */

static int
tohex (int ch)
{
  if (ch >= '0' && ch <= '9')
    {
      return (ch - '0');
    }
  if (ch >= 'A' && ch <= 'F')
    {
      return (ch - 'A' + 10);
    }
  if (ch >= 'a' && ch <= 'f')
    {
      return (ch - 'a' + 10);
    }
  fprintf (stderr, "\nInvalid hex digit '%c'\n", ch);
  fflush (stderr);
  exit (1);
}
#endif

#define EOI 0xdeadbeef

/*
 * convert_logchar
 *
 * Adapted from gdbreplay.
 * Some characters in the log are escaped, and
 * need to be translated before being sent back
 * to gdb.
 *
 * Returns a de-escaped char.
 */

static int
convert_logchar (char **logp)
{
  int ch, ch2;

  ch = *(*logp)++;
  switch (ch)
    {
    case '\0':
    case '\n':
      ch = EOI;	/* End of input (out of band).  */
      break;
    case '\\':
      ch = *(*logp)++;
      switch (ch)
	{
	case '\\':
	  break;
	case 'b':
	  ch = '\b';
	  break;
	case 'f':
	  ch = '\f';
	  break;
	case 'n':
	  ch = '\n';
	  break;
	case 'r':
	  ch = '\r';
	  break;
	case 't':
	  ch = '\t';
	  break;
	case 'v':
	  ch = '\v';
	  break;
	case 'x':
	  ch2 = *(*logp)++;
	  ch = hex_to_int (ch2) << 4;
	  ch2 = *(*logp)++;
	  ch |= hex_to_int (ch2);
	  break;
	default:
	  /* Treat any other char as just itself */
	  break;
	}
    default:
      break;
    }
  return (ch);
}

/*
 * gdbwriteline
 *
 * Send a line of data to gdb, stripped of any 
 * prefix or suffix stuff.
 *
 * Prefix is anything preceeding a '$'.
 * Suffix is anything following a "#xx" (including any newlines).
 *
 * Returns void.
 */

static void
gdbwriteline (int fd, char *line)
{
  char *end;
  int len, ich;
  char ch;

  if (line)
    {
      /* Strip prefix.  */
      while (*line && *line != '$')
	line++;
      if (*line)
	{
	  /* Strip suffix.  */
	  end = strchr (line, '#');
	  if (end && *end)
	    {
	      /* Got line, send it.  */
	      end[3] = '\0';
	      while ((ich = convert_logchar (&line)) != EOI)
		{
		  ch = ich;
		  write (fd, &ch, 1);
		}
	    }
	}
    }
}

/*
 * stopframe_signal
 *
 * Return the signal number for which the stopframe stopped.
 * This is a brute force implementation.  Probably should plan
 * on caching the value to speed things up.
 */

static int
stopframe_signal (FILE *infile, int id)
{
  char *line, *p;
  int sig;

  fseek (infile, stopframe[id].eventpos, SEEK_SET);
  line = fgets (inbuf, sizeof (inbuf), infile);

  if ((p = strstr (line, "$T")) != NULL ||
      (p = strstr (line, "$S")) != NULL)
    {
      /* Signal value is two ascii/hex bytes following "$S" or "$T".  */
      sig = hex_to_int (p[2]) << 8 + hex_to_int (p[3]);
      return sig;
    }
  return 0;
}

/*
 * freeplay_find_event
 *
 * Scan (forward or backward) thru the stop events, 
 * looking for a reason to stop now (assuming gdb has
 * asked us to continue).
 *
 * Returns event frame index, or -1 for failure.
 */

static int
freeplay_find_event (FILE *infile, int start, enum direction_code direction)
{
  int i;
  int signum;

  /* Here's a strange loop for you: goes forward or backward.  */
  for (i = start; 
       i >= 0 && i < last_cached_frame;
       direction == DIR_FORWARD ? i++ : i--)
    {
      signum = stopframe_signal (infile, i);
      /* FIXME we need some modal handling for SIGTRAP.  */
      if (signum != 0 && signum != 5)
	{
	  /* Here's an event we can stop at, regardless of breakpoints.  */
	  /* FIXME: there is some signal-ignoring stuff to be looked at.  */
	  return i;
	}
      else if (remote_breakpoint_here_p (SOFTWARE_BP, stopframe[i].pc) == PASS)
	{
	  /* Found a breakpoint.
	     FIXME need some DECR_PC_AFTER_BREAK handling.  */
	  return i;
	}
    }
  /* Found no reason to stop.  */
  return -1;
}

/*
 * add_checksum
 *
 * Compute the checksum for a string that will go to gdb, 
 * and concatenate it onto the string.
 *
 * Returns input string modified in place (better have room!)
 */

char *
add_checksum (char *inbuf)
{
  int cksum = 0;
  char *p = inbuf;

  /* If the string doesn't start with a '$', it's broken.  */
  if (*p++ != '$')
    return inbuf;

  /* Now add up the cksum.  */
  while (*p)
    {
      /* If there's already a '#', it doesn't count toward the cksum.  
         Stop there.  */
      if (*p == '#')
	break;
      cksum += *p++;
    }

  /* Add a '#' at the end, even if it's already there, and
     overwrite any old checksum that may be there already.  */
  *p++ = '#';
  *p++ = int_to_hex ((cksum >> 4) & 0xf);
  *p++ = int_to_hex (cksum & 0xf);
  *p++ = '\0';

  /* Done.  */
  return inbuf;
}

/*
 * handle_special_case
 *
 * Handle these requests locally, rather than through the replay buffer.
 *
 * Returns: a string reply for gdb.
 */

static char OK[8]    = "$OK#9a";
static char EMPTY[8] = "$#00";
static char STOP[8]  = "$S00#44";
static char E01[8]   = "$E01#a6";

static char *
handle_special_case (FILE *infile, int fd, char *request)
{
  unsigned long addr;
  unsigned long len;
  int next_event_frame;
  char *p;

  /* Handle 'k' (kill) request by exiting.  */
  if (strstr (request, "$k#6b") != NULL)
    {
      /* Well, to PROPERLY honor the 'kill' request, 
	 we need to actually shoot ourselves in the head...  */
      gdb_ack (fd);
      fprintf (stdout, "gdbfreeplay -- killed by gdb.\n");
      exit (0);
    }

  /* Handle 'D' (detach) request by exiting.  
     FIXME might want to stay alive and offer the socket again.  */
  if (strstr (request, "$D#44") != NULL)
    {
      /* OK, we're not yet set up to close and re-open the socket, 
	 and a new gdb does not seem able to re-attach, so for now
	 let's actually quit.  */
      gdb_ack (fd);
      gdbwriteline (fd, OK);
      fprintf (stdout, "gdbfreeplay -- gdb has detached.  Exiting.\n");
      exit (0);
    }

  /* Handle "monitor verbose on".  */
  if (strstr (request, "$qRcmd,766572626f7365206f6e#d6") != NULL)
    {
      verbose = 1;
      return OK;
    }

  /* Handle "monitor verbose off".  */
  if (strstr (request, "$qRcmd,766572626f7365206f6666#13") != NULL)
    {
      verbose = 0;
      return OK;
    }

  /* Handle 's' (step) by advancing the cur_frame index.  */
  if (strstr (request, "$s#73") != NULL)
    {
    step_label:
      if (cur_frame < last_cached_frame)
	cur_frame++;

      if (stopframe[cur_frame].eventpos != 0)
	{
	  return find_reply (infile, 
			     stopframe[cur_frame].eventpos);
	}
      else
	{
	  /* Not sure how this could even happen, but... */
	  return STOP;
	}
    }

  /* Handle 'bs' (reverse stepi) by decrementing the cur_frame index.  */
  if (strstr (request, "$bs#d5") != NULL)
    {
      if (cur_frame > 0)
	cur_frame--;

      if (stopframe[cur_frame].eventpos != 0)
	{
	  return find_reply (infile, 
			     stopframe[cur_frame].eventpos);
	}
      else
	{
	  /* Not sure how this could even happen, but... */
	  return STOP;
	}
    }

  /* Handle 'c' (continue) by searching the cache for a stop event.  */
  if (strstr (request, "$c#63") != NULL)
    {
      next_event_frame = freeplay_find_event (infile, cur_frame, DIR_FORWARD);
      if (next_event_frame != -1)
	{
	  /* Got a stop event.  Make it the current frame, and tell gdb.
	   */
	  cur_frame = next_event_frame;
#if 0
	  /* FIXME: we want to do something about DECR_PC_AFTER_BREAK.  */
	  return find_reply (infile, 
			     stopframe[cur_frame].eventpos);
#endif
	}
      else
	{
	  /* FIXME now what?  Go to final frame?  */
	  cur_frame = last_cached_frame - 1;
#if 0
	  /* FIXME: we want to do something about DECR_PC_AFTER_BREAK.  */
	  return find_reply (infile, 
			     stopframe[cur_frame].eventpos);
#endif
	}
#if 1
      /* Find the original event message for this stop event.  */
      fseek (infile, stopframe[cur_frame].eventpos, SEEK_SET);
      fgets (inbuf, sizeof (inbuf), infile);
      /* If it's a "$T", give the target a chance to re-compose it
	 (possibly allowing for DECR_PC_AFTER_BREAK).  */
      if ((p = strstr (inbuf, "$T")) != NULL)
	return add_checksum (target_compose_T_packet (p));
      /* If it's a "$S", just return it (FIXME?)  */
      else
	return &inbuf[0];
#endif
    }

  /* Handle 'bc' (revese continue) by searching the cache for a stop event.  */
  if (strstr (request, "$bc#c5") != NULL)
    {
      next_event_frame = freeplay_find_event (infile, cur_frame, DIR_BACKWARD);
      if (next_event_frame != -1)
	{
	  /* Got a stop event.  Make it the current frame, and tell gdb.
	   */
	  cur_frame = next_event_frame;
#if 0
	  /* FIXME: we want to do something about DECR_PC_AFTER_BREAK.  */
	  return find_reply (infile, 
			     stopframe[cur_frame].eventpos);
#endif
	}
      else
	{
	  /* FIXME now what?  Go to initial frame?  */
	  cur_frame = 0;
#if 0
	  /* FIXME: we want to do something about DECR_PC_AFTER_BREAK.  */
	  return find_reply (infile, 
			     stopframe[cur_frame].eventpos);
#endif
	}
#if 1
      /* Find the original event message for this stop event.  */
      fseek (infile, stopframe[cur_frame].eventpos, SEEK_SET);
      fgets (inbuf, sizeof (inbuf), infile);
      /* If it's a "$T", give the target a chance to re-compose it
	 (possibly allowing for DECR_PC_AFTER_BREAK).  */
      if ((p = strstr (inbuf, "$T")) != NULL)
	return add_checksum (target_compose_T_packet (p));
      /* If it's a "$S", just return it (FIXME?)  */
      else
	return &inbuf[0];
#endif
    }

  /* Handle Z0 set breakpoint.  */
  if ((p = strstr (request, "$Z0")) != NULL)
    {
      if (p[3] == ',')
	{
	  addr = strtoul (p + 4, &p, 16);
	  if (p[0] == ',')
	    {
	      len = strtoul (p + 1, NULL, 16);
	      remote_set_breakpoint (SOFTWARE_BP, addr, len);
	      return OK;
	    }
	}
    }

  /* Handle z0 remove breakpoint.  */
  if ((p = strstr (request, "$z0")) != NULL)
    {
      if (p[3] == ',')
	{
	  addr = strtoul (p + 4, &p, 16);
	  if (p[0] == ',')
	    {
	      len = strtoul (p + 1, NULL, 16);
	      remote_remove_breakpoint (SOFTWARE_BP, addr, len);
	      return OK;
	    }
	}
    }

  /* Handle "vCont" by saying we can't do it.
     FIXME we could do it, and moreover 'fallbacks' could handle this.  */
  if (strstr (request, "$vCont") != 0)
    return EMPTY;

  /* TODO: vCont, M, X, G, P/p, tfind...  */

  /* Let the replay buffer handle it.  */
  return NULL;
}

/*
 * fallbacks
 *
 * Attempt to handle requests that were not handled otherwise.
 * Returns: char * or NULL.
 */

static char *
fallbacks (FILE *infile, int fd, char *request)
{
  /* Handle "Hc0" request.  */
  if (strstr (request, "$Hc0#db") != NULL)
    {
      /* Debugger just wants to set the "continue thread" to zero.
	 Don't know why this isn't in the replay cache, but it's
	 harmless, just say OK.  */
      if (verbose)
	fprintf (stdout, "fallbacks: absorbing 'Hc0'\n");
      return OK;
    }
  /* Handle un-cached memory request (if not handled upstream).  */
  if (strstr (request, "$m") != NULL)
    {
      /* FIXME this need not happen so often, 
	 once we do a better job of simulating memory.  */
      if (verbose)
	fprintf (stdout, "fallbacks: failing memory request '%s'.\n",
		 request);
      return E01;
    }
  /* Handle P/p requests (if they weren't handled upstream).  */
  if (strstr (request, "$p") != NULL ||
      strstr (request, "$p") != NULL)
    {
      if (verbose)
	fprintf (stdout, "fallbacks: absorbing P/p request.\n");
      return EMPTY;	/* Tell gdb we don't know that one.  */
    }

  /* Default for any other un-handled request -- return empty string.  */
  if (verbose)
    fprintf (stdout, "fallbacks: absorbing unknown request '%s'.\n",
	     request);
  return EMPTY;
}

/*
 * gdbfreeplay
 *
 * This is the function that manages the interaction with gdb.
 */

void
gdbfreeplay (int fd)
{
  char *request;
  char *reply;

  /* Process requests from gdb.  */
  while (1)
    {
      /* Read next request from gdb.  */
      request = gdbreadline (fd);
      if (verbose)
	fprintf (stdout, "gdb request: %s\n", request);

      /* FIXME -- this is where we should handle special cases, including:
	 -- kill	$k#6b
	 -- detach	$D#44
	 -- step	$s#73
	 -- backstep	
	 -- continue	$c#63
	 -- back-cont	
	 -- breakpoint	$Z0...
	 -- vCont
	 -- M
	 -- X
	 -- G
	 -- P/p
	 -- tfind
      */

      reply = handle_special_case (replay_file, fd, request);

      if (reply == NULL)
	{
	  /* Now see if we have a matching request in the current frame.  */
	  reply = find_reply (replay_file, 
			      frame_find_request (replay_file, request));
	}

      if (reply == NULL)
	{
	  /* Found no matching request in the current frame.
	     Do fall-backs and last resorts.  */
	  reply = fallbacks (replay_file, fd, request);
	}

      if (verbose)
	fprintf (stdout, "freeplay reply: %s\n", 
		 reply == NULL ? "<not found>" : reply);

      if (reply)
	{
	  gdb_ack (fd);
	  gdbwriteline (fd, reply);
	}
      else
	{
	  /* FIXME -- gotta say something back... */
	}
    }
}
