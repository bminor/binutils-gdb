/* Remote debugging with the XLNT Designs, Inc (XDI) NetROM.
   Copyright 1990, 1991, 1992, 1995 Free Software Foundation, Inc.
   Contributed by:
	 Roger Moyers 
	 XLNT Designs, Inc.
	 15050 Avenue of Science, Suite 106
	 San Diego, CA  92128
	 (619)487-9320
	 roger@xlnt.com
   Adapted from work done at Cygnus Support in remote-nindy.c,
   later merged in by Stan Shebs at Cygnus.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "defs.h"
#include "gdbcmd.h"
#include "serial.h"

#include <string.h>
#include "inferior.h"
#include "wait.h"
#include "value.h"
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "terminal.h"
#include "target.h"
#include "gdbcore.h"

/* Packet header found on every packet sent to/from GDB.  */

typedef struct gpkthdr {
  unsigned char csum;		/* Check sum */
  unsigned char seq_num;	/* Sequence number */
  unsigned short len;		/* Number of bytes in packet */
  unsigned char cmd;		/* GDB command */
  unsigned char pad[3];
  unsigned long addr;		/* Address if needed */
  unsigned long datalen;	/* # of bytes to read/write */
} GPKTHDR;

#define GDB_START_DELIMITER '['
#define GDB_END_DELIMITER ']'

/* GDB requests. */

#define GDB_QUERY_CMD   0x01
#define GDB_STATUS      0x02
#define GDB_READ_REGS   0x03
#define GDB_WRITE_REGS  0x04
#define GDB_READ_MEM    0x05
#define GDB_WRITE_MEM   0x06
#define GDB_CONTINUE    0x07
#define GDB_STEP        0x08

/* Responses.  */

#define GDB_ACK            0x11
#define GDB_NACK           0x12
#define GDB_READ_REG_RESP  0x13
#define GDB_READ_MEM_RESP  0x14
#define GDB_WRITE_REG_RESP 0x15
#define GDB_WRITE_MEM_RESP 0x16

#define GDB_BP_TRAP     "05"
#define GDB_BUSERR_TRAP "10"
#define GDB_ILLOP_TRAP  "40"

#define GDB_ACK_VALUE   "OK"

/* Default ports used to talk with the NetROM.  */

#define DEFAULT_NETROM_TARGET_PORT  1235
#define DEFAULT_NETROM_LOAD_PORT    1236
#define DEFAULT_NETROM_CONTROL_PORT 1237

#define UC(b) (((long)b)&0xff)

#define NROM_BUF_SIZE	2048
#define MAX_SEND_ATTEMPTS 10
#define READ_BUF_SIZE	2048

/* Definitions for filetype.  */

#define BINARY_FTYPE 0
#define MOTO_SREC 1
#define INTEL_HEX 2

#if 0 /* for debugging, if anyone cares */
static char *GCMDTYPE[] = {
  "ZERO",
  "GDB_QUERY",
  "GDB_STATUS",
  "GDB_READ_REGS",
  "GDB_WRITE_REGS",
  "GDB_READ_MEM",
  "GDB_WRITE_MEM",
  "GDB_CONTINUE",
  "GDB_STEP",
  "CMD9",
  "CMDA",
  "CMDB",
  "CMDC",
  "CMDD",
  "CMDE",
  "CMDF",
  "RESP0",
  "GDB_ACK",
  "GDB_NACK",
  "GDB_READ_REG_RESP",
  "GDB_READ_MEM_RESP",
  "GDB_WRITE_REG_RESP",
  "GDB_WRITE_MEM_RESP"
};
#endif

static void nrom_attach PARAMS ((char *, int));

static int  nrom_can_run PARAMS ((void));

static void nrom_close PARAMS ((int));

static void nrom_create_inferior PARAMS ((char *, char *, char **));

static void nrom_detach PARAMS ((char *, int));

static void nrom_fetch_registers PARAMS ((int));

static void nrom_files_info PARAMS ((struct target_ops *));

static void nrom_kill PARAMS ((void));

static void nrom_load PARAMS ((char *, int));

static void nrom_mourn PARAMS ((void));

static void nrom_open PARAMS ((char *,int));

static void nrom_resume PARAMS ((int, int, enum target_signal));

static void nrom_prepare_to_store PARAMS ((void));

static void nrom_store_registers PARAMS ((int));

static int nrom_wait PARAMS ((int pid, struct target_waitstatus *status));

static int nrom_xfer_inferior_memory PARAMS ((CORE_ADDR, char *, int, int,
					       struct target_ops *));

static int nrom_write_inferior_memory PARAMS ((CORE_ADDR, char *, int));

static int nrom_read_inferior_memory PARAMS ((CORE_ADDR, char *, int));

/* New commands.  */

static void nrom_set_target_port PARAMS ((char *, int));

static void nrom_set_control_port PARAMS ((char *, int));

static void nrom_set_load_port PARAMS ((char *, int));

static void nrom_set_ipaddr PARAMS ((char *, int));

static void nrom_set_filetype PARAMS ((char *, int));

static void nrom_show_status PARAMS ((char *, int));

static void nrom_passthru PARAMS ((char *, int));

/* Packet functions.  */

static void build_pkt PARAMS ((int, unsigned char *, long, 
			       unsigned char *, unsigned long, unsigned long,
			       int));

static int compute_csum PARAMS ((unsigned char *, int));

#if 0
static void dump_pkt PARAMS ((GPKTHDR *, unsigned char *));
#endif

static int get_seq_number PARAMS ((void));

static char *hex2mem PARAMS ((char *, char *, int));

static char *mem2hex PARAMS ((char *, char *, int));

static void nrom_send PARAMS ((int, char *, int, long, long, char *));

static void send_query_cmd PARAMS ((void));

static int send_pkt PARAMS ((int, char *, int, long, int));

static int read_pkt PARAMS ((char *));

static void send_query_cmd PARAMS ((void));

static int tohex PARAMS ((int));

static int parse_pkt PARAMS ((unsigned char *, GPKTHDR *, char *));

static int writen PARAMS ((int, char *, int));

/* Private globals. */

/* We talk to the NetROM over these sockets.  */

static int nrom_load_sock = -1;
static int nrom_targ_sock = -1;
static int nrom_ctrl_sock = -1;

static serial_t load_desc = NULL;
static serial_t targ_desc = NULL;
static serial_t ctrl_desc = NULL;

/* For binding to the socket we ned a sockaddr_in structure.  */

static struct sockaddr_in nrom_sin;

/* The IP Address of the NetROM is needed so we know who to talk to.  */

static unsigned long nrom_ipaddr = 0;

static int load_port = DEFAULT_NETROM_LOAD_PORT;
static int target_port = DEFAULT_NETROM_TARGET_PORT;
static int control_port = DEFAULT_NETROM_CONTROL_PORT;

static int nrom_filetype = BINARY_FTYPE;

static unsigned char host_seq_num = 0;

static char hexchars[] = "0123456789abcdef";

static char freadbuf[READ_BUF_SIZE];

static char readbuf[NROM_BUF_SIZE];
static int bufdata = 0;
static int bufindex = 0;

static char workbuf[NROM_BUF_SIZE];
static char sendbuf[NROM_BUF_SIZE];

static char nrom_hostname[100];

/* Forward data declaration. */

extern struct target_ops nrom_ops;

/* This routine builds a packet to send to gdb running on the host.  */

static void
build_pkt (cmd, data, datalen, pkt, addr, len, seq)
     int cmd;
     unsigned char *data;
     long datalen;
     unsigned char *pkt;
     unsigned long addr;
     unsigned long len;
     int seq;
{
  GPKTHDR phdr;
  char *dptr;
  char *pktptr;
  unsigned char csum;
  int plen;
  
  phdr.csum = 0;
  phdr.len = sizeof(GPKTHDR) + datalen;
  phdr.seq_num = seq;
  phdr.cmd = cmd;
  phdr.pad[0] = phdr.pad[1] = phdr.pad[2] = 0;
  phdr.addr = addr;
  phdr.datalen = len;
  /* Convert packet header to ASCII.  */
  dptr = mem2hex ((char *) &phdr, pkt, sizeof(GPKTHDR));

  /* Calculate pkt length now that it is converted.  */
  plen = (int) ((unsigned long)dptr - (unsigned long)pkt) + datalen;
  /* Put data in packet.  */
  if (datalen > 0)
    memcpy (dptr, data, datalen);

  /* Compute checksum.  For computing checksum we skip first two bytes
     since this is where the checksum will go.  */
  pktptr = pkt + 2;
  csum = compute_csum (pktptr, plen - 2);
  *pkt++ = hexchars[csum >> 4];
  *pkt++ = hexchars[csum % 16];
  dptr += datalen;
  *dptr = '\0';
}

static int
compute_csum (data, len)
     unsigned char *data;
     int len;
{
  unsigned char csum;

  csum = 0;
  while (len > 0)
    {
      csum += *data++;
      --len;
    }
  return csum;
}

#if 0 /* for debugging, if anyone cares */
static void
dump_pkt (hdr, data)
     GPKTHDR *hdr;
     unsigned char *data;
{
  int i;

  printf_filtered ("PACKET: csum = %x,seq = %d,len = %d\n",hdr->csum,hdr->seq_num,
	 hdr->len);

  printf_filtered ("cmd = %s,addr = %x, datalen = %d\n", GCMDTYPE[hdr->cmd],
	 hdr->addr,hdr->datalen);
  if (hdr->datalen)
    {
      for (i = 0; i < hdr->datalen * 2; i++)
	printf_filtered ("%x",data[i]);
      printf_filtered ("\n");
    }
}
#endif

static int
get_seq_number()
{
  return host_seq_num++;
}

int 
hex (ch)
     int ch;
{
  if ((ch >= 'a') && (ch <= 'f'))
    return (ch - 'a' + 10);
  if((ch >= 'A') && (ch <= 'F'))
    return ((ch - 'A') + 10);
  if ((ch >= '0') && (ch <= '9'))
    return (ch - '0');
  return 0;
}

/* Convert the hex array pointed to by buf into binary to be placed in mem
   return a pointer to the character AFTER the last byte written.  */

static char*
hex2mem(buf, mem, count)
     char* buf;
     char* mem;
     int   count;
{
  int i;
  unsigned char ch;

  for (i = 0; i < count; i++)
    {
      ch = hex(*buf++) << 4;
      ch = ch + hex(*buf++);
      *mem++ = ch;
    }
  return mem;
}

/* Convert the memory pointed to by mem into hex, placing result in buf
   return a pointer to the last char put in buf (null) */

static char *
mem2hex (mem, buf, count)
     char* mem;
     char* buf;
     int   count;
{
  int i;
  unsigned char ch;

  for (i = 0; i < count; i++)
    {
      ch = *mem++;
      *buf++ = hexchars[ch >> 4];
      *buf++ = hexchars[ch % 16];
    }
  *buf = 0;
  return buf;
}

/* Scan input from the remote system, until STRING is found.  If BUF is non-
   zero, then collect input until we have collected either STRING or BUFLEN-1
   chars.  In either case we terminate BUF with a 0.  If input overflows BUF
   because STRING can't be found, return -1, else return number of chars in BUF
   (minus the terminating NUL).  Note that in the non-overflow case, STRING
   will be at the end of BUF.  */

static int
expect (string)
     char *string;
{
  char *p = string;
  int c;

  immediate_quit = 1;

  while (1)
    {
      c = SERIAL_READCHAR (ctrl_desc, 5);

      if (c == *p++)
	{
	  if (*p == '\0')
	    {
	      immediate_quit = 0;

	      return 0;
	    }
	}
      else
	{
	  fputc_unfiltered (c, gdb_stdout);
	  p = string;
	  if (c == *p)
	    p++;
	}
    }
}

static int
nrom_control_send (s, nbytes)
     char *s;
     int nbytes;
{
  SERIAL_WRITE (ctrl_desc, s, nbytes);

  return 0;
}

static void
nrom_kill ()
{
  nrom_close (0);
}

static serial_t
open_socket (name, port)
     char *name;
     int port;
{
  char sockname[100];
  serial_t desc;

  sprintf (sockname, "%s:%d", name, port);
  desc = SERIAL_OPEN (sockname);
  if (!desc)
    perror_with_name (sockname);

  return desc;
}

/* Download a file specified in ARGS to the netROM.  */

static void
nrom_load (args, fromtty)
     char *args;
     int fromtty;
{
  int fd, rd_amt, fsize;
  struct sockaddr_in sin;
  bfd *pbfd;
  asection *section;
  char *downloadstring = "download 0\n";

  /* Tell the netrom to get ready to download. */
  if (nrom_control_send (downloadstring, strlen (downloadstring)) < 0)
    error ("nrom_load: control_send() of `%s' failed", downloadstring);

  expect ("Waiting for a connection...\n");

  load_desc = open_socket (nrom_hostname, load_port);

  pbfd = bfd_openr (args, 0);

  if (pbfd)
    {
      if (!bfd_check_format (pbfd, bfd_object)) 
	error ("\"%s\": not in executable format: %s",
	       args, bfd_errmsg (bfd_get_error ()));

      for (section = pbfd->sections; section; section = section->next) 
	{
	  if (bfd_get_section_flags (pbfd, section) & SEC_ALLOC)
	    {
	      bfd_vma section_address;
	      unsigned long section_size;
	      const char *section_name;

	      section_name = bfd_get_section_name (pbfd, section);
	      section_address = bfd_get_section_vma (pbfd, section);
	      section_size = bfd_section_size (pbfd, section);

	      if (bfd_get_section_flags (pbfd, section) & SEC_LOAD)
		{
		  file_ptr fptr;

		  printf_filtered ("[Loading section %s at %x (%d bytes)]\n",
				   section_name, section_address,
				   section_size);

		  fptr = 0;

		  while (section_size > 0)
		    {
		      char buffer[1024];
		      int count;
		      
		      count = min (section_size, 1024);

		      bfd_get_section_contents (pbfd, section, buffer, fptr,
						count);

		      SERIAL_WRITE (load_desc, buffer, count);
		      section_address += count;
		      fptr += count;
		      section_size -= count;
		    }
		}
	      else			/* BSS and such */
		{
		  printf_filtered ("[section %s: not loading]\n",
				   section_name);
		}
	    }
	}
    }
  else
    error ("\"%s\": Could not open", args);

  SERIAL_CLOSE (load_desc);
  load_desc = NULL;
}

/* This is called not only when we first attach, but also when the
   user types "run" after having attached.  */

static void
nrom_create_inferior (execfile, args, env)
     char *execfile;
     char *args;
     char **env;
{
}

/* Open a connection to the remote NetROM devices.  */

static void
nrom_open (name, from_tty)
     char *name;
     int from_tty;
{
  int errn;

  if (!name || strchr (name, '/') || strchr (name, ':'))
    error (
"To open a NetROM connection, you must specify the hostname\n\
or IP address of the NetROM device you wish to use.");

  strcpy (nrom_hostname, name);

  target_preopen (from_tty);

  unpush_target (&nrom_ops);

  targ_desc = open_socket (nrom_hostname, target_port);
  ctrl_desc = open_socket (nrom_hostname, control_port);

  push_target (&nrom_ops);

  if (from_tty)
    printf_filtered ("Connected to NetROM device \"%s\"\n", nrom_hostname);
}

static int
nrom_can_run ()
{
  return 1;
}

/* Close out all files and local state before this target loses control. */

static void
nrom_close (quitting)
     int quitting;
{
  if (load_desc)
    SERIAL_CLOSE (load_desc);
  if (targ_desc)
    SERIAL_CLOSE (targ_desc);
  if (ctrl_desc)
    SERIAL_CLOSE (ctrl_desc);

  load_desc = NULL;
  targ_desc = NULL;
  ctrl_desc = NULL;
}

/* Attach to the target that is already loaded and possibly running */

static void
nrom_attach (args, from_tty)
     char *args;
     int from_tty;
{
  int nwaiting;
  char buf[10];

  if (from_tty)
    printf_filtered ("Attaching to NetROM\n");

  /* clear any pending data on the socket */
  printf_filtered ("Waiting for pending data to arrive... ");
  fflush(stdout);
  sleep(1);
  printf_filtered ("that's long enough!\n");
  while (1)
    {
      if (ioctl(nrom_targ_sock, FIONREAD, &nwaiting) != 0)
	{
	  /* couldn't determine data left */
	  perror("nrom_attach: ioctl FIONREAD");
	  break;
	}
      else if (nwaiting > 0)
	{
	  /* flush incoming data */
	  while (nwaiting != 0)
	    {
	      if (read (nrom_targ_sock, buf, 1) < 0)
		{
		  perror("nrom_attach: read");
		  exit(1);
		}
	      if (remote_debug > 2)
		putc(buf[0], stdout);
	      nwaiting--;
	    }
	} 
      else 
	{
	  /* no more data */
	  break;
	}
    }
  printf_filtered ("Pending data removed\n");

  /* We will get a task spawn event immediately.  */
  send_query_cmd ();
  target_has_execution = 1;
/*
  start_remote();
*/
}

/* Terminate the open connection to the remote debugger.  Use this
   when you want to detach and do something else with your gdb.  */

static void
nrom_detach (args, from_tty)
     char *args;
     int from_tty;
{
  pop_target ();
  if (from_tty)
    printf_filtered ("Ending remote debugging\n");
}
 
/* Tell the remote machine to resume.  */

static void
nrom_prepare_to_store()
{
}

static void
nrom_resume (pid, step, siggnal)
     int pid, step;
     enum target_signal siggnal;
{
  if (step)
    send_pkt (GDB_STEP, NULL, 0, 0, 0);
  else
    send_pkt (GDB_CONTINUE, NULL, 0, 0, 0);
}

/* Wait until the remote machine stops, then return,
   storing status in STATUS just as `wait' would.  */

static int
nrom_wait (pid, status)
     int pid;
     struct target_waitstatus *status;
{
  static char pkt[NROM_BUF_SIZE], inbuf[NROM_BUF_SIZE];
  GPKTHDR phdr;

  status->kind = TARGET_WAITKIND_EXITED;
  status->value.integer = 0;

  while (1)
    {
      if (read_pkt (pkt) == -1)
	continue;

      if (parse_pkt (pkt, &phdr, inbuf) < 0)
	{
	  if (remote_debug)
	    printf_filtered ("Bad packet in nrom_wait\n");
	  send_query_cmd ();
	  continue;
	}

      /* Got good packet.  Verify command is right.  */
      if (phdr.cmd != GDB_STATUS)
	{
	  /*  Wrong response.  Resend command.  */
	  send_query_cmd ();
	  continue;
	}
      /* Packet is fine.  Exit loop.  */
      return inferior_pid;
    }
}

/* Read the remote registers.  */

static void
nrom_fetch_registers (regno)
     int regno;
{
  char buf[NROM_BUF_SIZE];
  char regs[REGISTER_BYTES];
  int i;

#ifdef DEBUG
  printf_filtered ("reg no is %d\n",regno);
#endif

  nrom_send (GDB_READ_REGS, NULL, 0, -1, 0, buf);
  memcpy (regs, buf, REGISTER_BYTES);
  for (i = 0; i < NUM_REGS; i++)
    supply_register (i, &regs[REGISTER_BYTE(i)]);
#ifdef NO_SINGLE_REG
  nrom_send (GDB_READ_REGS, NULL, 0, regno, 0, buf);
  if (regno != -1)
    supply_register(regno,buf);
  else
    {
      memcpy (regs, buf, REGISTER_BYTES);
      for (i = 0; i < NUM_REGS; i++)
	supply_register (i, &regs[REGISTER_BYTE(i)]);
    }
#endif
}

static void
nrom_send (cmd, data, datalen, addr, len, resp)
     int cmd;
     char *data;
     int datalen;
     long addr;
     long len;
     char *resp;
{
  GPKTHDR phdr;
  char inbuf[NROM_BUF_SIZE];
		
  while (1)
    {
      while (send_pkt (cmd, data, datalen, addr, len) < 0)
	;
      if (read_pkt (inbuf) != -1)
	{
	  if (parse_pkt (inbuf, &phdr, resp) < 0)
	    continue;
	  if (phdr.cmd != GDB_NACK)
	    return;
        }
    }
}

static void 
nrom_set_filetype (args, fromtty)
     char *args;
     int fromtty;
{
  if (args[0] == 'b')
    nrom_filetype = BINARY_FTYPE;
  else if (args[0] == 'm')
    nrom_filetype = MOTO_SREC;
  else if (args[0] == 'i')
    nrom_filetype = INTEL_HEX;
  else
    printf_filtered ("Unknown file type\n");
}

static void 
nrom_set_ipaddr (args, fromtty)
     char *args;
     int fromtty;
{
  struct hostent *h;
  char buf[10];
  int i,j,val;
  unsigned long	newip = 0;

  /* First do a check to see if they typed in a hostname.  */
  if (!(*args))
    error ("Please enter a hostname or IP address");

  h = gethostbyname (args);
  if (h)
    {
      /* It is a hostname.  We just need the ipaddress.  */
      memcpy (&nrom_ipaddr, h->h_addr, h->h_length);
    }
  else
    {
      /* Better be in decimal dot notation,ie. xx.xx.xx.xx  */
      if (isdigit (args[0]) && strchr (args, '.'))
	{
	  j = 4;
	  while (j)
	    {
	      memset (buf, 0, 10);
	      i = 0;
	      while((*args) && (*args != '.'))
		{
		  buf[i] = *args++;
		  i++;
		}
	      if (i)
		{
		  val = (int) strtol (buf, NULL, 10);

		  if (val > 255)
		    error ("Invalid IP address");

		  j--;
		  newip |= val << (8 * j);
		  args++;
		}
	    }
	}
      else
	{
	  error ("Invalid host name/address");
	}
      nrom_ipaddr = newip;
    }
}

static void 
nrom_set_load_port (args, fromtty)
     char *args;
     int fromtty;
{
  load_port = (int) strtol (args, NULL, 10);
}

static void 
nrom_set_target_port (args, from_tty)
     char *args;
     int from_tty;
{
  target_port = (int) strtol (args, NULL, 10);
}

static void
nrom_set_control_port (args, fromtty)
     char *args;
     int fromtty;
{
  control_port = (int) strtol (args, NULL, 10);
}

static void 
nrom_show_status (args,from_tty)
     char *args;
     int from_tty;
{
  unsigned char *i;

  i = (unsigned char *)&nrom_ipaddr;

  printf_filtered ("NetROM target port is %d\n", target_port);
  printf_filtered ("NetROM download port is %d\n", load_port);
  printf_filtered ("NetROM debug control port is %d\n", control_port);
  
  printf_filtered ("NetROM IP Address is %d.%d.%d.%d\n",
		    UC(i[0]), UC(i[1]), UC(i[2]), UC(i[3]));				
  if (nrom_filetype == BINARY_FTYPE)
    printf_filtered ("download filetype is binary\n");
  else if (nrom_filetype == MOTO_SREC)
    printf_filtered ("download filetype is moto-srec\n");
  else if (nrom_filetype == INTEL_HEX)
    printf_filtered ("download filetype is intel-hex\n");
}

/* Pass arguments directly to the NetROM. */

static void
nrom_passthru (args, fromtty)
     char *args;
     int fromtty;
{
  char buf[1024];

  sprintf(buf, "%s\n", args);
  if (nrom_control_send (buf, strlen (buf)) < 0)
    error ("nrom_reset: control_send() of `%s'failed", args);
}

static void 
nrom_store_registers (regno)
int regno;
{
  char buf[NROM_BUF_SIZE];
  int i;
  char *p;

  p = buf;
  for (i = 0; i < REGISTER_BYTES; i++)
    {
      *p++ = tohex ((registers[i] >> 4) & 0xf);
      *p++ = tohex (registers[i] & 0xf);
    }
  *p = '\0';
  nrom_send (GDB_WRITE_REGS, buf, REGISTER_BYTES * 2, 0, REGISTER_BYTES * 2,
	     buf);
}

static int
nrom_xfer_inferior_memory (memaddr, myaddr, len, write, target)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
     int write;
     struct target_ops *target;
{
  if (write)
    return nrom_write_inferior_memory (memaddr, myaddr, len);
  else
    return nrom_read_inferior_memory (memaddr, myaddr, len);
}

/* Copy LEN bytes of data from debugger memory at MYADDR
   to inferior's memory at MEMADDR.  Returns errno value.  */

static int
nrom_write_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  char buf[NROM_BUF_SIZE],obuf[NROM_BUF_SIZE];
  int i;
  char *p;

  p = obuf;
  for (i = 0; i < len; i++)
    {
      *p++ = tohex ((myaddr[i] >> 4) & 0xf);
      *p++ = tohex (myaddr[i] & 0xf);
    }
  *p = '\0';
  nrom_send (GDB_WRITE_MEM, obuf, len * 2, memaddr, len, buf);

  return len;
}

/* Read LEN bytes from inferior memory at MEMADDR.  Put the result
   at debugger address MYADDR.  Returns errno value.  */

static int
nrom_read_inferior_memory (memaddr, myaddr, len)
     CORE_ADDR memaddr;
     char *myaddr;
     int len;
{
  char buf[NROM_BUF_SIZE];

  nrom_send (GDB_READ_MEM, NULL, 0, memaddr, len, buf);
  memcpy (myaddr, buf, len);
  return len;
}

static void
nrom_files_info (ignore)
     struct target_ops *ignore;
{
}

static void
nrom_mourn() 
{ 
  unpush_target (&nrom_ops);
  generic_mourn_inferior ();
}

/* Convert a packet into its parts and verify check sum.  */

static int
parse_pkt (pkt, hdr, data)
     unsigned char *pkt;
     GPKTHDR *hdr;
     char *data;
{
  unsigned char xcsum;
  unsigned char *dptr;

  /* Build packet header from received data.  */
  hex2mem (pkt, (char *) hdr, sizeof(GPKTHDR));
  if (remote_debug > 1)
    {
      printf_filtered ("Packet received: csum = %x,seq number = %x,len = %d\n",
	      hdr->csum,hdr->seq_num,hdr->len);
      printf_filtered ("cmd = %x,addr = %x,datalen = %d\n",
	      hdr->cmd,hdr->addr,hdr->datalen);
    }
  /* Skip first two bytes of packet when computing checksum.  */
  dptr = (sizeof(GPKTHDR) * 2) + pkt;
  pkt += 2;
  if (remote_debug > 1)
    {
      printf_filtered ("Computing checksum over pkt %s\n",pkt);
      printf_filtered ("Of length %d\n",strlen(pkt));
    }
  xcsum = compute_csum (pkt, strlen (pkt));
  if (xcsum != hdr->csum)
    {
      if (remote_debug)
	printf_filtered ("Checksum failure: computed %x, received %x\n",xcsum,	
		hdr->csum);
      return (-1);
    }

  /* Copy data portion to callers data buffer converting from ASCII
     to data as we go.  */
  hex2mem (dptr, data, hdr->datalen);
  return 0;
}

static int
read_pkt (pkt)
     char *pkt;
{
  int n, tries;
  struct sockaddr_in from;
  int from_len = sizeof(from);
  int gotstart;
  int total_len;
  char *p;

  p = pkt;
  total_len = 0;
  gotstart = 0;

  while (1)
    {
      /* Don't let us get wedged if the target is losing.  */
      QUIT;

      if (bufdata == 0)
	{
	  bufindex = 0;
	  n = NROM_BUF_SIZE;
	  /* Perform read on socket.  This will wait.  */
	  bufdata = recvfrom (nrom_targ_sock, readbuf, n, 0, &from, &from_len);
	  if (bufdata < 0)
	    {
	      printf_filtered ("Error on socket read of %d\n",errno);
	      return (-1);
	    }
	  if (remote_debug > 2)
	    {
	      readbuf[bufdata] = '\0';
	      printf_filtered ("Received %d bytes. Data is %s\n",
		      bufdata, readbuf);
	    }
	}

      /* skip stuff between packets */
      while (gotstart == 0 && bufdata != 0
	     && readbuf[bufindex] != GDB_START_DELIMITER)
	{
	  bufdata--;
	  bufindex++;
	}

      /* look for a start if we haven't seen one */
      if (gotstart == 0 && bufdata != 0
	  && readbuf[bufindex] == GDB_START_DELIMITER)
	{
	  gotstart = 1;
	  bufindex++;
	  bufdata--;
	}

      /* copy packet data */
      if (gotstart != 0)
	{
	  while (bufdata && readbuf[bufindex] != GDB_END_DELIMITER)
	    {
	      *p = readbuf[bufindex];
	      p++;
	      bufdata--;
	      bufindex++;
	      total_len++;
	      if (total_len > NROM_BUF_SIZE)
		{
		  error ("read_pkt: packet length exceeds %d\n", 
			 NROM_BUF_SIZE);
		  return (-1);
		}
	    }
	  *p = '\0';
	  if (remote_debug > 2)
	    printf_filtered ("Packet is '%s'\n", pkt);
	}

      /* Make sure this is the end of the packet.  */
      if (gotstart != 0 && bufdata != 0
	  && readbuf[bufindex] == GDB_END_DELIMITER)
	{
	  gotstart = 0;
	  bufindex++;
	  bufdata--;
	  /* Ensure that the packet is terminated.  */
	  *p = '\0';
	  if (remote_debug > 2)
	    printf_filtered ("Returning '%s'\n", pkt);
	  return 0;
	}
    }
}

static void
send_query_cmd ()
{
  while (send_pkt (GDB_QUERY_CMD, NULL, 0, 0, 0) < 0)
    ;
}

static int
send_pkt (cmd, data, datalen, addr, len)
     int cmd;
     char *data;
     int datalen;
     long addr;
     int len;
{
  char c[2];
  unsigned char seq;
  struct sockaddr_in mysin;
  int send_cnt;

  while (1)
    {
      /*  Get a sequence number for this packet.  */
      seq = get_seq_number ();
      /*  Build the packet.  */
      build_pkt (cmd, data, datalen, workbuf, addr, len, seq);
      /* Put delimiters around the pkt.  */
      memset (sendbuf, 0, NROM_BUF_SIZE);
      sendbuf[0] = GDB_START_DELIMITER;
      strcat (sendbuf, workbuf);
      c[0] = GDB_END_DELIMITER;
      c[1] = '\0';
      strcat (sendbuf, c);

      /* Send the packet out on our socket.  */
      if (remote_debug > 1)
	printf_filtered ("Sending pkt: %s\n", sendbuf);
      mysin.sin_family = AF_INET;
      mysin.sin_port = target_port;
      mysin.sin_addr.S_un.S_addr = nrom_ipaddr;

      send_cnt = 0;
      while (send_cnt < MAX_SEND_ATTEMPTS)
	{
	  if (sendto (nrom_targ_sock, sendbuf, strlen(sendbuf), 0, &mysin,
		      sizeof(struct sockaddr_in)) < 0)
	    {
	      printf_filtered ("sendto error of %d\n", errno);
	      send_cnt++;
	    }
	  else
	    break;
	}
      if (send_cnt >= MAX_SEND_ATTEMPTS)
	{
	  printf_filtered ("Socket send failed after %d tries\n",
			   MAX_SEND_ATTEMPTS);
	  return (-1);
	}
      return 1;
    }
}

/* Convert number NIB to a hex digit.  */

static int
tohex (nib)
     int nib;
{
  if (nib < 10)
    return '0' + nib;
  else
    return 'a' + nib - 10;
}

/* snatched from Stevens, pp279+ */

int
writen (sock, ptr, nbytes)
     int sock;
     char *ptr;
     int nbytes;
{
  int nleft, nwritten;
  char *buf = ptr;

  nleft = nbytes;
  while (nleft > 0)
    {
      nwritten = write (sock, buf, nleft);
      if (nwritten <= 0)
	return nwritten;
      nleft -= nwritten;
      buf += nwritten;
    }
  return (nbytes - nleft);
}

/* Define the target vector. */

struct target_ops nrom_ops = {
  "nrom",			/* to_shortname */
  "Remote XDI `NetROM' target",	/* to_longname */
  "Remote debug using a NetROM over Ethernet",
  nrom_open,			/* to_open */
  nrom_close,
  nrom_attach,
  nrom_detach,
  nrom_resume,
  nrom_wait,			/* to_wait */
  nrom_fetch_registers,		/* to_fetch_registers */
  nrom_store_registers,		/* to_store_registers */
  nrom_prepare_to_store,	/* to_prepare_to_store */
  nrom_xfer_inferior_memory,	/* to_xfer_memory */
  nrom_files_info,		/* to_files_info */
  NULL,				/* to_insert_breakpoint */
  NULL,				/* to_remove_breakpoint */
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  nrom_kill,
  nrom_load,
  NULL,
  nrom_create_inferior,		/* to_create_inferior */
  nrom_mourn,
  nrom_can_run,
  0,				/* to_notice_signals */
  0,
  download_stratum,		/* to_stratum */
  NULL,				/* to_next */
  1,
  1,
  1,
  1,
  0,				/* to_has_execution */
  NULL,				/* sections */
  NULL,				/* sections_end */
  OPS_MAGIC			/* to_magic */
};

void
_initialize_remote_nrom ()
{
  add_target (&nrom_ops);

  /* Add some commands for helpers.  */
  add_cmd ("nrom_ipaddr", no_class, nrom_set_ipaddr,
	  "Set the IP Address of the NetROM\n",
	  &setlist);
  add_cmd ("target_port", no_class, nrom_set_target_port,
	   "Set the Port to use for NetROM target communication\n",
	   &setlist);
  add_cmd ("load_port", no_class, nrom_set_load_port,
	   "Set the Port to use for NetROM downloads\n",
	   &setlist);
  add_cmd ("control_port", no_class, nrom_set_control_port,
	   "Set the Port to use for NetROM debugger services\n",
	   &setlist);
  add_cmd ("nrom_filetype", no_class, nrom_set_filetype,
	   "Set the filetype to use on NetROM downloads",
	   &setlist);

  add_cmd ("nrom", no_class, nrom_show_status,
	   "Show the current NetROM status\n",
	   &showlist);

  add_cmd ("nrom", no_class, nrom_passthru,
	   "Pass arguments as command to NetROM",
	   &cmdlist);
}
