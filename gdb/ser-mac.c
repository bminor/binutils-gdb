/* OBSOLETE /* Remote serial interface for local (hardwired) serial ports for Macintosh. */
/* OBSOLETE    Copyright 1994, 1995, 1996, 1998, 2000 Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by Cygnus Support.  Written by Stan Shebs. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #include "defs.h" */
/* OBSOLETE #include "serial.h" */
/* OBSOLETE  */
/* OBSOLETE #include <Types.h> */
/* OBSOLETE #include <Devices.h> */
/* OBSOLETE /* This is the regular Mac Serial.h, but copied to a different name */
/* OBSOLETE    so as not to get confused with the GDB serial.h above.  */ */
/* OBSOLETE #include "MacSerial.h" */
/* OBSOLETE  */
/* OBSOLETE /* This is unused for now.  We just return a placeholder. */ */
/* OBSOLETE  */
/* OBSOLETE struct mac_ttystate */
/* OBSOLETE   { */
/* OBSOLETE     int bogus; */
/* OBSOLETE   }; */
/* OBSOLETE  */
/* OBSOLETE static int mac_open (serial_t scb, const char *name); */
/* OBSOLETE static void mac_raw (serial_t scb); */
/* OBSOLETE static int mac_readchar (serial_t scb, int timeout); */
/* OBSOLETE static int mac_setbaudrate (serial_t scb, int rate); */
/* OBSOLETE static int mac_write (serial_t scb, const char *str, int len); */
/* OBSOLETE static void mac_close (serial_t scb); */
/* OBSOLETE static serial_ttystate mac_get_tty_state (serial_t scb); */
/* OBSOLETE static int mac_set_tty_state (serial_t scb, serial_ttystate state); */
/* OBSOLETE static char *aptr (short p); */
/* OBSOLETE  */
/* OBSOLETE short input_refnum; */
/* OBSOLETE short output_refnum; */
/* OBSOLETE  */
/* OBSOLETE char *mac_input_buffer; */
/* OBSOLETE char *mac_output_buffer; */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_open (serial_t scb, const char *name) */
/* OBSOLETE { */
/* OBSOLETE   OSErr err; */
/* OBSOLETE  */
/* OBSOLETE   /* Alloc buffer space first - that way any allocation failures are */
/* OBSOLETE      intercepted before the serial driver gets involved. */ */
/* OBSOLETE   if (mac_input_buffer == NULL) */
/* OBSOLETE     mac_input_buffer = (char *) xmalloc (4096); */
/* OBSOLETE   /* Match on a name and open a port. */ */
/* OBSOLETE   if (strcmp (name, "modem") == 0) */
/* OBSOLETE     { */
/* OBSOLETE       err = OpenDriver ("\p.AIn", &input_refnum); */
/* OBSOLETE       if (err != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  return (-1); */
/* OBSOLETE 	} */
/* OBSOLETE       err = OpenDriver ("\p.AOut", &output_refnum); */
/* OBSOLETE       if (err != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  CloseDriver (input_refnum); */
/* OBSOLETE 	  return (-1); */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   else if (strcmp (name, "printer") == 0) */
/* OBSOLETE     { */
/* OBSOLETE       err = OpenDriver ("\p.BIn", &input_refnum); */
/* OBSOLETE       if (err != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  return (-1); */
/* OBSOLETE 	} */
/* OBSOLETE       err = OpenDriver ("\p.BOut", &output_refnum); */
/* OBSOLETE       if (err != 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  CloseDriver (input_refnum); */
/* OBSOLETE 	  return (-1); */
/* OBSOLETE 	} */
/* OBSOLETE       /* fake */ */
/* OBSOLETE       scb->fd = 1; */
/* OBSOLETE       return 0; */
/* OBSOLETE     } */
/* OBSOLETE   else */
/* OBSOLETE     { */
/* OBSOLETE       error ("You must specify a valid serial port name; your choices are `modem' or `printer'."); */
/* OBSOLETE       errno = ENOENT; */
/* OBSOLETE       return (-1); */
/* OBSOLETE     } */
/* OBSOLETE   /* We got something open. */ */
/* OBSOLETE   if (1 /* using custom buffer */ ) */
/* OBSOLETE     SerSetBuf (input_refnum, mac_input_buffer, 4096); */
/* OBSOLETE   /* Set to a GDB-preferred state. */ */
/* OBSOLETE   SerReset (input_refnum, stop10 | noParity | data8 | baud9600); */
/* OBSOLETE   SerReset (output_refnum, stop10 | noParity | data8 | baud9600); */
/* OBSOLETE   { */
/* OBSOLETE     CntrlParam cb; */
/* OBSOLETE     struct SerShk *handshake; */
/* OBSOLETE  */
/* OBSOLETE     cb.ioCRefNum = output_refnum; */
/* OBSOLETE     cb.csCode = 14; */
/* OBSOLETE     handshake = (struct SerShk *) &cb.csParam[0]; */
/* OBSOLETE     handshake->fXOn = 0; */
/* OBSOLETE     handshake->fCTS = 0; */
/* OBSOLETE     handshake->xOn = 0; */
/* OBSOLETE     handshake->xOff = 0; */
/* OBSOLETE     handshake->errs = 0; */
/* OBSOLETE     handshake->evts = 0; */
/* OBSOLETE     handshake->fInX = 0; */
/* OBSOLETE     handshake->fDTR = 0; */
/* OBSOLETE     err = PBControl ((ParmBlkPtr) & cb, 0); */
/* OBSOLETE     if (err < 0) */
/* OBSOLETE       return (-1); */
/* OBSOLETE   } */
/* OBSOLETE   /* fake */ */
/* OBSOLETE   scb->fd = 1; */
/* OBSOLETE   return 0; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_noop (serial_t scb) */
/* OBSOLETE { */
/* OBSOLETE   return 0; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE mac_raw (serial_t scb) */
/* OBSOLETE { */
/* OBSOLETE   /* Always effectively in raw mode. */ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Read a character with user-specified timeout.  TIMEOUT is number of seconds */
/* OBSOLETE    to wait, or -1 to wait forever.  Use timeout of 0 to effect a poll.  Returns */
/* OBSOLETE    char if successful.  Returns -2 if timeout expired, EOF if line dropped */
/* OBSOLETE    dead, or -3 for any other error (see errno in that case). */ */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_readchar (serial_t scb, int timeout) */
/* OBSOLETE { */
/* OBSOLETE   int status, n; */
/* OBSOLETE   /* time_t */ unsigned long start_time, now; */
/* OBSOLETE   OSErr err; */
/* OBSOLETE   CntrlParam cb; */
/* OBSOLETE   IOParam pb; */
/* OBSOLETE  */
/* OBSOLETE   if (scb->bufcnt-- > 0) */
/* OBSOLETE     return *scb->bufp++; */
/* OBSOLETE  */
/* OBSOLETE   time (&start_time); */
/* OBSOLETE  */
/* OBSOLETE   while (1) */
/* OBSOLETE     { */
/* OBSOLETE       cb.ioCRefNum = input_refnum; */
/* OBSOLETE       cb.csCode = 2; */
/* OBSOLETE       err = PBStatus ((ParmBlkPtr) & cb, 0); */
/* OBSOLETE       if (err < 0) */
/* OBSOLETE 	return SERIAL_ERROR; */
/* OBSOLETE       n = *((long *) &cb.csParam[0]); */
/* OBSOLETE       if (n > 0) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  pb.ioRefNum = input_refnum; */
/* OBSOLETE 	  pb.ioBuffer = (Ptr) (scb->buf); */
/* OBSOLETE 	  pb.ioReqCount = (n > 64 ? 64 : n); */
/* OBSOLETE 	  err = PBRead ((ParmBlkPtr) & pb, 0); */
/* OBSOLETE 	  if (err < 0) */
/* OBSOLETE 	    return SERIAL_ERROR; */
/* OBSOLETE 	  scb->bufcnt = pb.ioReqCount; */
/* OBSOLETE 	  scb->bufcnt--; */
/* OBSOLETE 	  scb->bufp = scb->buf; */
/* OBSOLETE 	  return *scb->bufp++; */
/* OBSOLETE 	} */
/* OBSOLETE       else if (timeout == 0) */
/* OBSOLETE 	return SERIAL_TIMEOUT; */
/* OBSOLETE       else if (timeout == -1) */
/* OBSOLETE 	; */
/* OBSOLETE       else */
/* OBSOLETE 	{ */
/* OBSOLETE 	  time (&now); */
/* OBSOLETE 	  if (now > start_time + timeout) */
/* OBSOLETE 	    return SERIAL_TIMEOUT; */
/* OBSOLETE 	} */
/* OBSOLETE       PROGRESS (1); */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* mac_{get set}_tty_state() are both dummys to fill out the function */
/* OBSOLETE    vector.  Someday, they may do something real... */ */
/* OBSOLETE  */
/* OBSOLETE static serial_ttystate */
/* OBSOLETE mac_get_tty_state (serial_t scb) */
/* OBSOLETE { */
/* OBSOLETE   struct mac_ttystate *state; */
/* OBSOLETE  */
/* OBSOLETE   state = (struct mac_ttystate *) xmalloc (sizeof *state); */
/* OBSOLETE  */
/* OBSOLETE   return (serial_ttystate) state; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_set_tty_state (serial_t scb, serial_ttystate ttystate) */
/* OBSOLETE { */
/* OBSOLETE   return 0; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_noflush_set_tty_state (serial_t scb, serial_ttystate new_ttystate, */
/* OBSOLETE 			   serial_ttystate old_ttystate) */
/* OBSOLETE { */
/* OBSOLETE   return 0; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE mac_print_tty_state (serial_t scb, */
/* OBSOLETE 		     serial_ttystate ttystate, */
/* OBSOLETE 		     struct ui_file *stream) */
/* OBSOLETE { */
/* OBSOLETE   /* Nothing to print.  */ */
/* OBSOLETE   return; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* If there is a tricky formula to relate real baud rates */
/* OBSOLETE    to what the serial driver wants, we should use it.  Until */
/* OBSOLETE    we get one, this table will have to do.  */ */
/* OBSOLETE  */
/* OBSOLETE static struct */
/* OBSOLETE { */
/* OBSOLETE   int real_rate; */
/* OBSOLETE   int bits; */
/* OBSOLETE } */
/* OBSOLETE mac_baud_rate_table[] = */
/* OBSOLETE { */
/* OBSOLETE   { */
/* OBSOLETE     57600, baud57600 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     38400, 1 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     19200, baud19200 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     9600, baud9600 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     7200, baud7200 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     4800, baud4800 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     3600, baud3600 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     2400, baud2400 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     1800, baud1800 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     1200, baud1200 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     600, baud600 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     300, baud300 */
/* OBSOLETE   } */
/* OBSOLETE   , */
/* OBSOLETE   { */
/* OBSOLETE     0, 0 */
/* OBSOLETE   } */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_set_baud_rate (serial_t scb, int rate) */
/* OBSOLETE { */
/* OBSOLETE   int i, bits; */
/* OBSOLETE  */
/* OBSOLETE   for (i = 0; mac_baud_rate_table[i].real_rate != 0; ++i) */
/* OBSOLETE     { */
/* OBSOLETE       if (mac_baud_rate_table[i].real_rate == rate) */
/* OBSOLETE 	{ */
/* OBSOLETE 	  bits = mac_baud_rate_table[i].bits; */
/* OBSOLETE 	  break; */
/* OBSOLETE 	} */
/* OBSOLETE     } */
/* OBSOLETE   SerReset (input_refnum, stop10 | noParity | data8 | bits); */
/* OBSOLETE   SerReset (output_refnum, stop10 | noParity | data8 | bits); */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_set_stop_bits (serial_t scb, int num) */
/* OBSOLETE { */
/* OBSOLETE   return 0; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE int first_mac_write = 0; */
/* OBSOLETE  */
/* OBSOLETE static int */
/* OBSOLETE mac_write (serial_t scb, const char *str, int len) */
/* OBSOLETE { */
/* OBSOLETE   OSErr err; */
/* OBSOLETE   IOParam pb; */
/* OBSOLETE  */
/* OBSOLETE   if (first_mac_write++ < 4) */
/* OBSOLETE     { */
/* OBSOLETE       sleep (1); */
/* OBSOLETE     } */
/* OBSOLETE   pb.ioRefNum = output_refnum; */
/* OBSOLETE   pb.ioBuffer = (Ptr) str; */
/* OBSOLETE   pb.ioReqCount = len; */
/* OBSOLETE   err = PBWrite ((ParmBlkPtr) & pb, 0); */
/* OBSOLETE   if (err < 0) */
/* OBSOLETE     { */
/* OBSOLETE       return 1; */
/* OBSOLETE     } */
/* OBSOLETE   return 0; */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static void */
/* OBSOLETE mac_close (serial_t scb) */
/* OBSOLETE { */
/* OBSOLETE   if (input_refnum) */
/* OBSOLETE     { */
/* OBSOLETE       if (1 /* custom buffer */ ) */
/* OBSOLETE 	SerSetBuf (input_refnum, mac_input_buffer, 0); */
/* OBSOLETE       CloseDriver (input_refnum); */
/* OBSOLETE       input_refnum = 0; */
/* OBSOLETE     } */
/* OBSOLETE   if (output_refnum) */
/* OBSOLETE     { */
/* OBSOLETE       if (0 /* custom buffer */ ) */
/* OBSOLETE 	SerSetBuf (input_refnum, mac_output_buffer, 0); */
/* OBSOLETE       CloseDriver (output_refnum); */
/* OBSOLETE       output_refnum = 0; */
/* OBSOLETE     } */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE static struct serial_ops mac_ops = */
/* OBSOLETE { */
/* OBSOLETE   "hardwire", */
/* OBSOLETE   0, */
/* OBSOLETE   mac_open, */
/* OBSOLETE   mac_close, */
/* OBSOLETE   mac_readchar, */
/* OBSOLETE   mac_write, */
/* OBSOLETE   mac_noop,			/* flush output */ */
/* OBSOLETE   mac_noop,			/* flush input */ */
/* OBSOLETE   mac_noop,			/* send break -- currently only for nindy */ */
/* OBSOLETE   mac_raw, */
/* OBSOLETE   mac_get_tty_state, */
/* OBSOLETE   mac_set_tty_state, */
/* OBSOLETE   mac_print_tty_state, */
/* OBSOLETE   mac_noflush_set_tty_state, */
/* OBSOLETE   mac_set_baud_rate, */
/* OBSOLETE   mac_set_stop_bits, */
/* OBSOLETE   mac_noop,			/* wait for output to drain */ */
/* OBSOLETE }; */
/* OBSOLETE  */
/* OBSOLETE void */
/* OBSOLETE _initialize_ser_mac (void) */
/* OBSOLETE { */
/* OBSOLETE   serial_add_interface (&mac_ops); */
/* OBSOLETE } */
