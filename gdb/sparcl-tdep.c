// OBSOLETE /* Target dependent code for the Fujitsu SPARClite for GDB, the GNU debugger.
// OBSOLETE    Copyright 1994, 1995, 1996, 1998, 1999, 2000, 2001
// OBSOLETE    Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #include "defs.h"
// OBSOLETE #include "gdbcore.h"
// OBSOLETE #include "breakpoint.h"
// OBSOLETE #include "target.h"
// OBSOLETE #include "serial.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE #include <sys/types.h>
// OBSOLETE 
// OBSOLETE #if (!defined(__GO32__) && !defined(_WIN32)) || defined(__CYGWIN__)
// OBSOLETE #define HAVE_SOCKETS
// OBSOLETE #include <sys/time.h>
// OBSOLETE #include <sys/socket.h>
// OBSOLETE #include <netinet/in.h>
// OBSOLETE #include <netdb.h>
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE static struct target_ops sparclite_ops;
// OBSOLETE 
// OBSOLETE static char *remote_target_name = NULL;
// OBSOLETE static struct serial *remote_desc = NULL;
// OBSOLETE static int serial_flag;
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE static int udp_fd = -1;
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE static struct serial *open_tty (char *name);
// OBSOLETE static int send_resp (struct serial *desc, char c);
// OBSOLETE static void close_tty (void * ignore);
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE static int recv_udp_buf (int fd, unsigned char *buf, int len, int timeout);
// OBSOLETE static int send_udp_buf (int fd, unsigned char *buf, int len);
// OBSOLETE #endif
// OBSOLETE static void sparclite_open (char *name, int from_tty);
// OBSOLETE static void sparclite_close (int quitting);
// OBSOLETE static void download (char *target_name, char *args, int from_tty,
// OBSOLETE 		      void (*write_routine) (bfd * from_bfd,
// OBSOLETE 					     asection * from_sec,
// OBSOLETE 					     file_ptr from_addr,
// OBSOLETE 					     bfd_vma to_addr, int len),
// OBSOLETE 		      void (*start_routine) (bfd_vma entry));
// OBSOLETE static void sparclite_serial_start (bfd_vma entry);
// OBSOLETE static void sparclite_serial_write (bfd * from_bfd, asection * from_sec,
// OBSOLETE 				    file_ptr from_addr,
// OBSOLETE 				    bfd_vma to_addr, int len);
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE static unsigned short calc_checksum (unsigned char *buffer, int count);
// OBSOLETE static void sparclite_udp_start (bfd_vma entry);
// OBSOLETE static void sparclite_udp_write (bfd * from_bfd, asection * from_sec,
// OBSOLETE 				 file_ptr from_addr, bfd_vma to_addr,
// OBSOLETE 				 int len);
// OBSOLETE #endif
// OBSOLETE static void sparclite_download (char *filename, int from_tty);
// OBSOLETE 
// OBSOLETE #define DDA2_SUP_ASI		0xb000000
// OBSOLETE #define DDA1_SUP_ASI		0xb0000
// OBSOLETE 
// OBSOLETE #define DDA2_ASI_MASK 		0xff000000
// OBSOLETE #define DDA1_ASI_MASK 		0xff0000
// OBSOLETE #define DIA2_SUP_MODE 		0x8000
// OBSOLETE #define DIA1_SUP_MODE 		0x4000
// OBSOLETE #define DDA2_ENABLE 		0x100
// OBSOLETE #define DDA1_ENABLE 		0x80
// OBSOLETE #define DIA2_ENABLE 		0x40
// OBSOLETE #define DIA1_ENABLE 		0x20
// OBSOLETE #define DSINGLE_STEP 		0x10	/* not used */
// OBSOLETE #define DDV_TYPE_MASK 		0xc
// OBSOLETE #define DDV_TYPE_LOAD 		0x0
// OBSOLETE #define DDV_TYPE_STORE 		0x4
// OBSOLETE #define DDV_TYPE_ACCESS 	0x8
// OBSOLETE #define DDV_TYPE_ALWAYS		0xc
// OBSOLETE #define DDV_COND		0x2
// OBSOLETE #define DDV_MASK		0x1
// OBSOLETE 
// OBSOLETE int
// OBSOLETE sparclite_insert_watchpoint (CORE_ADDR addr, int len, int type)
// OBSOLETE {
// OBSOLETE   CORE_ADDR dcr;
// OBSOLETE 
// OBSOLETE   dcr = read_register (DCR_REGNUM);
// OBSOLETE 
// OBSOLETE   if (!(dcr & DDA1_ENABLE))
// OBSOLETE     {
// OBSOLETE       write_register (DDA1_REGNUM, addr);
// OBSOLETE       dcr &= ~(DDA1_ASI_MASK | DDV_TYPE_MASK);
// OBSOLETE       dcr |= (DDA1_SUP_ASI | DDA1_ENABLE);
// OBSOLETE       if (type == 1)
// OBSOLETE 	{
// OBSOLETE 	  write_register (DDV1_REGNUM, 0);
// OBSOLETE 	  write_register (DDV2_REGNUM, 0xffffffff);
// OBSOLETE 	  dcr |= (DDV_TYPE_LOAD & (~DDV_COND & ~DDV_MASK));
// OBSOLETE 	}
// OBSOLETE       else if (type == 0)
// OBSOLETE 	{
// OBSOLETE 	  write_register (DDV1_REGNUM, 0);
// OBSOLETE 	  write_register (DDV2_REGNUM, 0xffffffff);
// OBSOLETE 	  dcr |= (DDV_TYPE_STORE & (~DDV_COND & ~DDV_MASK));
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  write_register (DDV1_REGNUM, 0);
// OBSOLETE 	  write_register (DDV2_REGNUM, 0xffffffff);
// OBSOLETE 	  dcr |= (DDV_TYPE_ACCESS);
// OBSOLETE 	}
// OBSOLETE       write_register (DCR_REGNUM, dcr);
// OBSOLETE     }
// OBSOLETE   else if (!(dcr & DDA2_ENABLE))
// OBSOLETE     {
// OBSOLETE       write_register (DDA2_REGNUM, addr);
// OBSOLETE       dcr &= ~(DDA2_ASI_MASK & DDV_TYPE_MASK);
// OBSOLETE       dcr |= (DDA2_SUP_ASI | DDA2_ENABLE);
// OBSOLETE       if (type == 1)
// OBSOLETE 	{
// OBSOLETE 	  write_register (DDV1_REGNUM, 0);
// OBSOLETE 	  write_register (DDV2_REGNUM, 0xffffffff);
// OBSOLETE 	  dcr |= (DDV_TYPE_LOAD & ~DDV_COND & ~DDV_MASK);
// OBSOLETE 	}
// OBSOLETE       else if (type == 0)
// OBSOLETE 	{
// OBSOLETE 	  write_register (DDV1_REGNUM, 0);
// OBSOLETE 	  write_register (DDV2_REGNUM, 0xffffffff);
// OBSOLETE 	  dcr |= (DDV_TYPE_STORE & ~DDV_COND & ~DDV_MASK);
// OBSOLETE 	}
// OBSOLETE       else
// OBSOLETE 	{
// OBSOLETE 	  write_register (DDV1_REGNUM, 0);
// OBSOLETE 	  write_register (DDV2_REGNUM, 0xffffffff);
// OBSOLETE 	  dcr |= (DDV_TYPE_ACCESS);
// OBSOLETE 	}
// OBSOLETE       write_register (DCR_REGNUM, dcr);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE sparclite_remove_watchpoint (CORE_ADDR addr, int len, int type)
// OBSOLETE {
// OBSOLETE   CORE_ADDR dcr, dda1, dda2;
// OBSOLETE 
// OBSOLETE   dcr = read_register (DCR_REGNUM);
// OBSOLETE   dda1 = read_register (DDA1_REGNUM);
// OBSOLETE   dda2 = read_register (DDA2_REGNUM);
// OBSOLETE 
// OBSOLETE   if ((dcr & DDA1_ENABLE) && addr == dda1)
// OBSOLETE     write_register (DCR_REGNUM, (dcr & ~DDA1_ENABLE));
// OBSOLETE   else if ((dcr & DDA2_ENABLE) && addr == dda2)
// OBSOLETE     write_register (DCR_REGNUM, (dcr & ~DDA2_ENABLE));
// OBSOLETE   else
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE sparclite_insert_hw_breakpoint (CORE_ADDR addr, int len)
// OBSOLETE {
// OBSOLETE   CORE_ADDR dcr;
// OBSOLETE 
// OBSOLETE   dcr = read_register (DCR_REGNUM);
// OBSOLETE 
// OBSOLETE   if (!(dcr & DIA1_ENABLE))
// OBSOLETE     {
// OBSOLETE       write_register (DIA1_REGNUM, addr);
// OBSOLETE       write_register (DCR_REGNUM, (dcr | DIA1_ENABLE | DIA1_SUP_MODE));
// OBSOLETE     }
// OBSOLETE   else if (!(dcr & DIA2_ENABLE))
// OBSOLETE     {
// OBSOLETE       write_register (DIA2_REGNUM, addr);
// OBSOLETE       write_register (DCR_REGNUM, (dcr | DIA2_ENABLE | DIA2_SUP_MODE));
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE sparclite_remove_hw_breakpoint (CORE_ADDR addr, int shadow)
// OBSOLETE {
// OBSOLETE   CORE_ADDR dcr, dia1, dia2;
// OBSOLETE 
// OBSOLETE   dcr = read_register (DCR_REGNUM);
// OBSOLETE   dia1 = read_register (DIA1_REGNUM);
// OBSOLETE   dia2 = read_register (DIA2_REGNUM);
// OBSOLETE 
// OBSOLETE   if ((dcr & DIA1_ENABLE) && addr == dia1)
// OBSOLETE     write_register (DCR_REGNUM, (dcr & ~DIA1_ENABLE));
// OBSOLETE   else if ((dcr & DIA2_ENABLE) && addr == dia2)
// OBSOLETE     write_register (DCR_REGNUM, (dcr & ~DIA2_ENABLE));
// OBSOLETE   else
// OBSOLETE     return -1;
// OBSOLETE 
// OBSOLETE   return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE int
// OBSOLETE sparclite_check_watch_resources (int type, int cnt, int ot)
// OBSOLETE {
// OBSOLETE   /* Watchpoints not supported on simulator.  */
// OBSOLETE   if (strcmp (target_shortname, "sim") == 0)
// OBSOLETE     return 0;
// OBSOLETE 
// OBSOLETE   if (type == bp_hardware_breakpoint)
// OBSOLETE     {
// OBSOLETE       if (TARGET_HW_BREAK_LIMIT == 0)
// OBSOLETE 	return 0;
// OBSOLETE       else if (cnt <= TARGET_HW_BREAK_LIMIT)
// OBSOLETE 	return 1;
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       if (TARGET_HW_WATCH_LIMIT == 0)
// OBSOLETE 	return 0;
// OBSOLETE       else if (ot)
// OBSOLETE 	return -1;
// OBSOLETE       else if (cnt <= TARGET_HW_WATCH_LIMIT)
// OBSOLETE 	return 1;
// OBSOLETE     }
// OBSOLETE   return -1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE CORE_ADDR
// OBSOLETE sparclite_stopped_data_address (void)
// OBSOLETE {
// OBSOLETE   CORE_ADDR dsr, dda1, dda2;
// OBSOLETE 
// OBSOLETE   dsr = read_register (DSR_REGNUM);
// OBSOLETE   dda1 = read_register (DDA1_REGNUM);
// OBSOLETE   dda2 = read_register (DDA2_REGNUM);
// OBSOLETE 
// OBSOLETE   if (dsr & 0x10)
// OBSOLETE     return dda1;
// OBSOLETE   else if (dsr & 0x20)
// OBSOLETE     return dda2;
// OBSOLETE   else
// OBSOLETE     return 0;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static struct serial *
// OBSOLETE open_tty (char *name)
// OBSOLETE {
// OBSOLETE   struct serial *desc;
// OBSOLETE 
// OBSOLETE   desc = serial_open (name);
// OBSOLETE   if (!desc)
// OBSOLETE     perror_with_name (name);
// OBSOLETE 
// OBSOLETE   if (baud_rate != -1)
// OBSOLETE     {
// OBSOLETE       if (serial_setbaudrate (desc, baud_rate))
// OBSOLETE 	{
// OBSOLETE 	  serial_close (desc);
// OBSOLETE 	  perror_with_name (name);
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   serial_raw (desc);
// OBSOLETE 
// OBSOLETE   serial_flush_input (desc);
// OBSOLETE 
// OBSOLETE   return desc;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Read a single character from the remote end, masking it down to 7 bits. */
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE readchar (struct serial *desc, int timeout)
// OBSOLETE {
// OBSOLETE   int ch;
// OBSOLETE   char s[10];
// OBSOLETE 
// OBSOLETE   ch = serial_readchar (desc, timeout);
// OBSOLETE 
// OBSOLETE   switch (ch)
// OBSOLETE     {
// OBSOLETE     case SERIAL_EOF:
// OBSOLETE       error ("SPARClite remote connection closed");
// OBSOLETE     case SERIAL_ERROR:
// OBSOLETE       perror_with_name ("SPARClite communication error");
// OBSOLETE     case SERIAL_TIMEOUT:
// OBSOLETE       error ("SPARClite remote timeout");
// OBSOLETE     default:
// OBSOLETE       if (remote_debug > 0)
// OBSOLETE 	{
// OBSOLETE 	  sprintf (s, "[%02x]", ch & 0xff);
// OBSOLETE 	  puts_debug ("read -->", s, "<--");
// OBSOLETE 	}
// OBSOLETE       return ch;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE debug_serial_write (struct serial *desc, char *buf, int len)
// OBSOLETE {
// OBSOLETE   char s[10];
// OBSOLETE 
// OBSOLETE   serial_write (desc, buf, len);
// OBSOLETE   if (remote_debug > 0)
// OBSOLETE     {
// OBSOLETE       while (len-- > 0)
// OBSOLETE 	{
// OBSOLETE 	  sprintf (s, "[%02x]", *buf & 0xff);
// OBSOLETE 	  puts_debug ("Sent -->", s, "<--");
// OBSOLETE 	  buf++;
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE send_resp (struct serial *desc, char c)
// OBSOLETE {
// OBSOLETE   debug_serial_write (desc, &c, 1);
// OBSOLETE   return readchar (desc, remote_timeout);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE close_tty (void *ignore)
// OBSOLETE {
// OBSOLETE   if (!remote_desc)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   serial_close (remote_desc);
// OBSOLETE 
// OBSOLETE   remote_desc = NULL;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE static int
// OBSOLETE recv_udp_buf (int fd, unsigned char *buf, int len, int timeout)
// OBSOLETE {
// OBSOLETE   int cc;
// OBSOLETE   fd_set readfds;
// OBSOLETE 
// OBSOLETE   FD_ZERO (&readfds);
// OBSOLETE   FD_SET (fd, &readfds);
// OBSOLETE 
// OBSOLETE   if (timeout >= 0)
// OBSOLETE     {
// OBSOLETE       struct timeval timebuf;
// OBSOLETE 
// OBSOLETE       timebuf.tv_sec = timeout;
// OBSOLETE       timebuf.tv_usec = 0;
// OBSOLETE       cc = select (fd + 1, &readfds, 0, 0, &timebuf);
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     cc = select (fd + 1, &readfds, 0, 0, 0);
// OBSOLETE 
// OBSOLETE   if (cc == 0)
// OBSOLETE     return 0;
// OBSOLETE 
// OBSOLETE   if (cc != 1)
// OBSOLETE     perror_with_name ("recv_udp_buf: Bad return value from select:");
// OBSOLETE 
// OBSOLETE   cc = recv (fd, buf, len, 0);
// OBSOLETE 
// OBSOLETE   if (cc < 0)
// OBSOLETE     perror_with_name ("Got an error from recv: ");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE send_udp_buf (int fd, unsigned char *buf, int len)
// OBSOLETE {
// OBSOLETE   int cc;
// OBSOLETE 
// OBSOLETE   cc = send (fd, buf, len, 0);
// OBSOLETE 
// OBSOLETE   if (cc == len)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   if (cc < 0)
// OBSOLETE     perror_with_name ("Got an error from send: ");
// OBSOLETE 
// OBSOLETE   error ("Short count in send: tried %d, sent %d\n", len, cc);
// OBSOLETE }
// OBSOLETE #endif /* HAVE_SOCKETS */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_open (char *name, int from_tty)
// OBSOLETE {
// OBSOLETE   struct cleanup *old_chain;
// OBSOLETE   int c;
// OBSOLETE   char *p;
// OBSOLETE 
// OBSOLETE   if (!name)
// OBSOLETE     error ("You need to specify what device or hostname is associated with the SparcLite board.");
// OBSOLETE 
// OBSOLETE   target_preopen (from_tty);
// OBSOLETE 
// OBSOLETE   unpush_target (&sparclite_ops);
// OBSOLETE 
// OBSOLETE   if (remote_target_name)
// OBSOLETE     xfree (remote_target_name);
// OBSOLETE 
// OBSOLETE   remote_target_name = xstrdup (name);
// OBSOLETE 
// OBSOLETE   /* We need a 'serial' or 'udp' keyword to disambiguate host:port, which can
// OBSOLETE      mean either a serial port on a terminal server, or the IP address of a
// OBSOLETE      SPARClite demo board.  If there's no colon, then it pretty much has to be
// OBSOLETE      a local device (except for DOS... grrmble) */
// OBSOLETE 
// OBSOLETE   p = strchr (name, ' ');
// OBSOLETE 
// OBSOLETE   if (p)
// OBSOLETE     {
// OBSOLETE       *p++ = '\000';
// OBSOLETE       while ((*p != '\000') && isspace (*p))
// OBSOLETE 	p++;
// OBSOLETE 
// OBSOLETE       if (strncmp (name, "serial", strlen (name)) == 0)
// OBSOLETE 	serial_flag = 1;
// OBSOLETE       else if (strncmp (name, "udp", strlen (name)) == 0)
// OBSOLETE 	serial_flag = 0;
// OBSOLETE       else
// OBSOLETE 	error ("Must specify either `serial' or `udp'.");
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE       p = name;
// OBSOLETE 
// OBSOLETE       if (!strchr (name, ':'))
// OBSOLETE 	serial_flag = 1;	/* No colon is unambiguous (local device) */
// OBSOLETE       else
// OBSOLETE 	error ("Usage: target sparclite serial /dev/ttyb\n\
// OBSOLETE or: target sparclite udp host");
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (serial_flag)
// OBSOLETE     {
// OBSOLETE       remote_desc = open_tty (p);
// OBSOLETE 
// OBSOLETE       old_chain = make_cleanup (close_tty, 0 /*ignore*/);
// OBSOLETE 
// OBSOLETE       c = send_resp (remote_desc, 0x00);
// OBSOLETE 
// OBSOLETE       if (c != 0xaa)
// OBSOLETE 	error ("Unknown response (0x%x) from SparcLite.  Try resetting the board.",
// OBSOLETE 	       c);
// OBSOLETE 
// OBSOLETE       c = send_resp (remote_desc, 0x55);
// OBSOLETE 
// OBSOLETE       if (c != 0x55)
// OBSOLETE 	error ("Sparclite appears to be ill.");
// OBSOLETE     }
// OBSOLETE   else
// OBSOLETE     {
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE       struct hostent *he;
// OBSOLETE       struct sockaddr_in sockaddr;
// OBSOLETE       unsigned char buffer[100];
// OBSOLETE       int cc;
// OBSOLETE 
// OBSOLETE       /* Setup the socket.  Must be raw UDP. */
// OBSOLETE 
// OBSOLETE       he = gethostbyname (p);
// OBSOLETE 
// OBSOLETE       if (!he)
// OBSOLETE 	error ("No such host %s.", p);
// OBSOLETE 
// OBSOLETE       udp_fd = socket (PF_INET, SOCK_DGRAM, 0);
// OBSOLETE 
// OBSOLETE       old_chain = make_cleanup (close, udp_fd);
// OBSOLETE 
// OBSOLETE       sockaddr.sin_family = PF_INET;
// OBSOLETE       sockaddr.sin_port = htons (7000);
// OBSOLETE       memcpy (&sockaddr.sin_addr.s_addr, he->h_addr, sizeof (struct in_addr));
// OBSOLETE 
// OBSOLETE       if (connect (udp_fd, &sockaddr, sizeof (sockaddr)))
// OBSOLETE 	perror_with_name ("Connect failed");
// OBSOLETE 
// OBSOLETE       buffer[0] = 0x5;
// OBSOLETE       buffer[1] = 0;
// OBSOLETE 
// OBSOLETE       send_udp_buf (udp_fd, buffer, 2);		/* Request version */
// OBSOLETE       cc = recv_udp_buf (udp_fd, buffer, sizeof (buffer), 5);	/* Get response */
// OBSOLETE       if (cc == 0)
// OBSOLETE 	error ("SPARClite isn't responding.");
// OBSOLETE 
// OBSOLETE       if (cc < 3)
// OBSOLETE 	error ("SPARClite appears to be ill.");
// OBSOLETE #else
// OBSOLETE       error ("UDP downloading is not supported for DOS hosts.");
// OBSOLETE #endif /* HAVE_SOCKETS */
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   printf_unfiltered ("[SPARClite appears to be alive]\n");
// OBSOLETE 
// OBSOLETE   push_target (&sparclite_ops);
// OBSOLETE 
// OBSOLETE   discard_cleanups (old_chain);
// OBSOLETE 
// OBSOLETE   return;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_close (int quitting)
// OBSOLETE {
// OBSOLETE   if (serial_flag)
// OBSOLETE     close_tty (0);
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE   else if (udp_fd != -1)
// OBSOLETE     close (udp_fd);
// OBSOLETE #endif
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #define LOAD_ADDRESS 0x40000000
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE download (char *target_name, char *args, int from_tty,
// OBSOLETE 	  void (*write_routine) (bfd *from_bfd, asection *from_sec,
// OBSOLETE 				 file_ptr from_addr, bfd_vma to_addr, int len),
// OBSOLETE 	  void (*start_routine) (bfd_vma entry))
// OBSOLETE {
// OBSOLETE   struct cleanup *old_chain;
// OBSOLETE   asection *section;
// OBSOLETE   bfd *pbfd;
// OBSOLETE   bfd_vma entry;
// OBSOLETE   int i;
// OBSOLETE #define WRITESIZE 1024
// OBSOLETE   char *filename;
// OBSOLETE   int quiet;
// OBSOLETE   int nostart;
// OBSOLETE 
// OBSOLETE   quiet = 0;
// OBSOLETE   nostart = 0;
// OBSOLETE   filename = NULL;
// OBSOLETE 
// OBSOLETE   while (*args != '\000')
// OBSOLETE     {
// OBSOLETE       char *arg;
// OBSOLETE 
// OBSOLETE       while (isspace (*args))
// OBSOLETE 	args++;
// OBSOLETE 
// OBSOLETE       arg = args;
// OBSOLETE 
// OBSOLETE       while ((*args != '\000') && !isspace (*args))
// OBSOLETE 	args++;
// OBSOLETE 
// OBSOLETE       if (*args != '\000')
// OBSOLETE 	*args++ = '\000';
// OBSOLETE 
// OBSOLETE       if (*arg != '-')
// OBSOLETE 	filename = arg;
// OBSOLETE       else if (strncmp (arg, "-quiet", strlen (arg)) == 0)
// OBSOLETE 	quiet = 1;
// OBSOLETE       else if (strncmp (arg, "-nostart", strlen (arg)) == 0)
// OBSOLETE 	nostart = 1;
// OBSOLETE       else
// OBSOLETE 	error ("unknown option `%s'", arg);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (!filename)
// OBSOLETE     filename = get_exec_file (1);
// OBSOLETE 
// OBSOLETE   pbfd = bfd_openr (filename, gnutarget);
// OBSOLETE   if (pbfd == NULL)
// OBSOLETE     {
// OBSOLETE       perror_with_name (filename);
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE   old_chain = make_cleanup_bfd_close (pbfd);
// OBSOLETE 
// OBSOLETE   if (!bfd_check_format (pbfd, bfd_object))
// OBSOLETE     error ("\"%s\" is not an object file: %s", filename,
// OBSOLETE 	   bfd_errmsg (bfd_get_error ()));
// OBSOLETE 
// OBSOLETE   for (section = pbfd->sections; section; section = section->next)
// OBSOLETE     {
// OBSOLETE       if (bfd_get_section_flags (pbfd, section) & SEC_LOAD)
// OBSOLETE 	{
// OBSOLETE 	  bfd_vma section_address;
// OBSOLETE 	  bfd_size_type section_size;
// OBSOLETE 	  file_ptr fptr;
// OBSOLETE 	  const char *section_name;
// OBSOLETE 
// OBSOLETE 	  section_name = bfd_get_section_name (pbfd, section);
// OBSOLETE 
// OBSOLETE 	  section_address = bfd_get_section_vma (pbfd, section);
// OBSOLETE 
// OBSOLETE 	  /* Adjust sections from a.out files, since they don't
// OBSOLETE 	     carry their addresses with.  */
// OBSOLETE 	  if (bfd_get_flavour (pbfd) == bfd_target_aout_flavour)
// OBSOLETE 	    {
// OBSOLETE 	      if (strcmp (section_name, ".text") == 0)
// OBSOLETE 		section_address = bfd_get_start_address (pbfd);
// OBSOLETE 	      else if (strcmp (section_name, ".data") == 0)
// OBSOLETE 		{
// OBSOLETE 		  /* Read the first 8 bytes of the data section.
// OBSOLETE 		     There should be the string 'DaTa' followed by
// OBSOLETE 		     a word containing the actual section address. */
// OBSOLETE 		  struct data_marker
// OBSOLETE 		    {
// OBSOLETE 		      char signature[4];	/* 'DaTa' */
// OBSOLETE 		      unsigned char sdata[4];	/* &sdata */
// OBSOLETE 		    }
// OBSOLETE 		  marker;
// OBSOLETE 		  bfd_get_section_contents (pbfd, section, &marker, 0,
// OBSOLETE 					    sizeof (marker));
// OBSOLETE 		  if (strncmp (marker.signature, "DaTa", 4) == 0)
// OBSOLETE 		    {
// OBSOLETE 		      if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
// OBSOLETE 			section_address = bfd_getb32 (marker.sdata);
// OBSOLETE 		      else
// OBSOLETE 			section_address = bfd_getl32 (marker.sdata);
// OBSOLETE 		    }
// OBSOLETE 		}
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE 	  section_size = bfd_get_section_size_before_reloc (section);
// OBSOLETE 
// OBSOLETE 	  if (!quiet)
// OBSOLETE 	    printf_filtered ("[Loading section %s at 0x%x (%d bytes)]\n",
// OBSOLETE 			     bfd_get_section_name (pbfd, section),
// OBSOLETE 			     section_address,
// OBSOLETE 			     section_size);
// OBSOLETE 
// OBSOLETE 	  fptr = 0;
// OBSOLETE 	  while (section_size > 0)
// OBSOLETE 	    {
// OBSOLETE 	      int count;
// OBSOLETE 	      static char inds[] = "|/-\\";
// OBSOLETE 	      static int k = 0;
// OBSOLETE 
// OBSOLETE 	      QUIT;
// OBSOLETE 
// OBSOLETE 	      count = min (section_size, WRITESIZE);
// OBSOLETE 
// OBSOLETE 	      write_routine (pbfd, section, fptr, section_address, count);
// OBSOLETE 
// OBSOLETE 	      if (!quiet)
// OBSOLETE 		{
// OBSOLETE 		  printf_unfiltered ("\r%c", inds[k++ % 4]);
// OBSOLETE 		  gdb_flush (gdb_stdout);
// OBSOLETE 		}
// OBSOLETE 
// OBSOLETE 	      section_address += count;
// OBSOLETE 	      fptr += count;
// OBSOLETE 	      section_size -= count;
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   if (!nostart)
// OBSOLETE     {
// OBSOLETE       entry = bfd_get_start_address (pbfd);
// OBSOLETE 
// OBSOLETE       if (!quiet)
// OBSOLETE 	printf_unfiltered ("[Starting %s at 0x%x]\n", filename, entry);
// OBSOLETE 
// OBSOLETE       start_routine (entry);
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   do_cleanups (old_chain);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_serial_start (bfd_vma entry)
// OBSOLETE {
// OBSOLETE   char buffer[5];
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   buffer[0] = 0x03;
// OBSOLETE   store_unsigned_integer (buffer + 1, 4, entry);
// OBSOLETE 
// OBSOLETE   debug_serial_write (remote_desc, buffer, 1 + 4);
// OBSOLETE   i = readchar (remote_desc, remote_timeout);
// OBSOLETE   if (i != 0x55)
// OBSOLETE     error ("Can't start SparcLite.  Error code %d\n", i);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_serial_write (bfd *from_bfd, asection *from_sec, file_ptr from_addr,
// OBSOLETE 			bfd_vma to_addr, int len)
// OBSOLETE {
// OBSOLETE   char buffer[4 + 4 + WRITESIZE];	/* addr + len + data */
// OBSOLETE   unsigned char checksum;
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   store_unsigned_integer (buffer, 4, to_addr);	/* Address */
// OBSOLETE   store_unsigned_integer (buffer + 4, 4, len);	/* Length */
// OBSOLETE 
// OBSOLETE   bfd_get_section_contents (from_bfd, from_sec, buffer + 8, from_addr, len);
// OBSOLETE 
// OBSOLETE   checksum = 0;
// OBSOLETE   for (i = 0; i < len; i++)
// OBSOLETE     checksum += buffer[8 + i];
// OBSOLETE 
// OBSOLETE   i = send_resp (remote_desc, 0x01);
// OBSOLETE 
// OBSOLETE   if (i != 0x5a)
// OBSOLETE     error ("Bad response from load command (0x%x)", i);
// OBSOLETE 
// OBSOLETE   debug_serial_write (remote_desc, buffer, 4 + 4 + len);
// OBSOLETE   i = readchar (remote_desc, remote_timeout);
// OBSOLETE 
// OBSOLETE   if (i != checksum)
// OBSOLETE     error ("Bad checksum from load command (0x%x)", i);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE 
// OBSOLETE static unsigned short
// OBSOLETE calc_checksum (unsigned char *buffer, int count)
// OBSOLETE {
// OBSOLETE   unsigned short checksum;
// OBSOLETE 
// OBSOLETE   checksum = 0;
// OBSOLETE   for (; count > 0; count -= 2, buffer += 2)
// OBSOLETE     checksum += (*buffer << 8) | *(buffer + 1);
// OBSOLETE 
// OBSOLETE   if (count != 0)
// OBSOLETE     checksum += *buffer << 8;
// OBSOLETE 
// OBSOLETE   return checksum;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_udp_start (bfd_vma entry)
// OBSOLETE {
// OBSOLETE   unsigned char buffer[6];
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   buffer[0] = 0x3;
// OBSOLETE   buffer[1] = 0;
// OBSOLETE   buffer[2] = entry >> 24;
// OBSOLETE   buffer[3] = entry >> 16;
// OBSOLETE   buffer[4] = entry >> 8;
// OBSOLETE   buffer[5] = entry;
// OBSOLETE 
// OBSOLETE   send_udp_buf (udp_fd, buffer, 6);	/* Send start addr */
// OBSOLETE   i = recv_udp_buf (udp_fd, buffer, sizeof (buffer), -1);	/* Get response */
// OBSOLETE 
// OBSOLETE   if (i < 1 || buffer[0] != 0x55)
// OBSOLETE     error ("Failed to take start address.");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_udp_write (bfd *from_bfd, asection *from_sec, file_ptr from_addr,
// OBSOLETE 		     bfd_vma to_addr, int len)
// OBSOLETE {
// OBSOLETE   unsigned char buffer[2000];
// OBSOLETE   unsigned short checksum;
// OBSOLETE   static int pkt_num = 0;
// OBSOLETE   static unsigned long old_addr = -1;
// OBSOLETE   int i;
// OBSOLETE 
// OBSOLETE   while (1)
// OBSOLETE     {
// OBSOLETE       if (to_addr != old_addr)
// OBSOLETE 	{
// OBSOLETE 	  buffer[0] = 0x1;	/* Load command */
// OBSOLETE 	  buffer[1] = 0x1;	/* Loading address */
// OBSOLETE 	  buffer[2] = to_addr >> 24;
// OBSOLETE 	  buffer[3] = to_addr >> 16;
// OBSOLETE 	  buffer[4] = to_addr >> 8;
// OBSOLETE 	  buffer[5] = to_addr;
// OBSOLETE 
// OBSOLETE 	  checksum = 0;
// OBSOLETE 	  for (i = 0; i < 6; i++)
// OBSOLETE 	    checksum += buffer[i];
// OBSOLETE 	  checksum &= 0xff;
// OBSOLETE 
// OBSOLETE 	  send_udp_buf (udp_fd, buffer, 6);
// OBSOLETE 	  i = recv_udp_buf (udp_fd, buffer, sizeof buffer, -1);
// OBSOLETE 
// OBSOLETE 	  if (i < 1)
// OBSOLETE 	    error ("Got back short checksum for load addr.");
// OBSOLETE 
// OBSOLETE 	  if (checksum != buffer[0])
// OBSOLETE 	    error ("Got back bad checksum for load addr.");
// OBSOLETE 
// OBSOLETE 	  pkt_num = 0;		/* Load addr resets packet seq # */
// OBSOLETE 	  old_addr = to_addr;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       bfd_get_section_contents (from_bfd, from_sec, buffer + 6, from_addr,
// OBSOLETE 				len);
// OBSOLETE 
// OBSOLETE       checksum = calc_checksum (buffer + 6, len);
// OBSOLETE 
// OBSOLETE       buffer[0] = 0x1;		/* Load command */
// OBSOLETE       buffer[1] = 0x2;		/* Loading data */
// OBSOLETE       buffer[2] = pkt_num >> 8;
// OBSOLETE       buffer[3] = pkt_num;
// OBSOLETE       buffer[4] = checksum >> 8;
// OBSOLETE       buffer[5] = checksum;
// OBSOLETE 
// OBSOLETE       send_udp_buf (udp_fd, buffer, len + 6);
// OBSOLETE       i = recv_udp_buf (udp_fd, buffer, sizeof buffer, 3);
// OBSOLETE 
// OBSOLETE       if (i == 0)
// OBSOLETE 	{
// OBSOLETE 	  fprintf_unfiltered (gdb_stderr, "send_data: timeout sending %d bytes to address 0x%x retrying\n", len, to_addr);
// OBSOLETE 	  continue;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       if (buffer[0] != 0xff)
// OBSOLETE 	error ("Got back bad response for load data.");
// OBSOLETE 
// OBSOLETE       old_addr += len;
// OBSOLETE       pkt_num++;
// OBSOLETE 
// OBSOLETE       return;
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #endif /* HAVE_SOCKETS */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE sparclite_download (char *filename, int from_tty)
// OBSOLETE {
// OBSOLETE   if (!serial_flag)
// OBSOLETE #ifdef HAVE_SOCKETS
// OBSOLETE     download (remote_target_name, filename, from_tty, sparclite_udp_write,
// OBSOLETE 	      sparclite_udp_start);
// OBSOLETE #else
// OBSOLETE     internal_error (__FILE__, __LINE__, "failed internal consistency check");			/* sparclite_open should prevent this! */
// OBSOLETE #endif
// OBSOLETE   else
// OBSOLETE     download (remote_target_name, filename, from_tty, sparclite_serial_write,
// OBSOLETE 	      sparclite_serial_start);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Set up the sparclite target vector.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE init_sparclite_ops (void)
// OBSOLETE {
// OBSOLETE   sparclite_ops.to_shortname = "sparclite";
// OBSOLETE   sparclite_ops.to_longname = "SPARClite download target";
// OBSOLETE   sparclite_ops.to_doc = "Download to a remote SPARClite target board via serial of UDP.\n\
// OBSOLETE Specify the device it is connected to (e.g. /dev/ttya).";
// OBSOLETE   sparclite_ops.to_open = sparclite_open;
// OBSOLETE   sparclite_ops.to_close = sparclite_close;
// OBSOLETE   sparclite_ops.to_load = sparclite_download;
// OBSOLETE   sparclite_ops.to_stratum = download_stratum;
// OBSOLETE   sparclite_ops.to_magic = OPS_MAGIC;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE void
// OBSOLETE _initialize_sparcl_tdep (void)
// OBSOLETE {
// OBSOLETE   init_sparclite_ops ();
// OBSOLETE   add_target (&sparclite_ops);
// OBSOLETE }
