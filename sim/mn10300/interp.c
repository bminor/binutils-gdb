#include <signal.h>
#include "sysdep.h"
#include "bfd.h"

#include "mn10300_sim.h"

#ifndef INLINE
#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif
#endif

host_callback *mn10300_callback;
int mn10300_debug;
static SIM_OPEN_KIND sim_kind;
static char *myname;

static struct hash_entry *lookup_hash PARAMS ((uint32 ins, int));
static long hash PARAMS ((long));
static void init_system PARAMS ((void));
#define MAX_HASH  63

struct hash_entry
{
  struct hash_entry *next;
  long opcode;
  long mask;
  struct simops *ops;
};

struct hash_entry hash_table[MAX_HASH+1];


/* This probably doesn't do a very good job at bucket filling, but
   it's simple... */
static INLINE long 
hash(insn)
     long insn;
{
  /* These are one byte insns.  */
  if ((insn & 0xffffff00) == 0)
    {
      if ((insn & 0xf0) == 0x00
	  || (insn & 0xf0) == 0x40)
	return (insn & 0xf3) & 0x3f;

      if ((insn & 0xf0) == 0x10
	  || (insn & 0xf0) == 0x30
	  || (insn & 0xf0) == 0x50)
	return (insn & 0xfc) & 0x3f;

      if ((insn & 0xf0) == 0x60
	  || (insn & 0xf0) == 0x70
	  || (insn & 0xf0) == 0x80
	  || (insn & 0xf0) == 0x90
	  || (insn & 0xf0) == 0xa0
	  || (insn & 0xf0) == 0xb0
	  || (insn & 0xf0) == 0xe0)
	return (insn & 0xf0) & 0x3f;

      return (insn & 0xff) & 0x3f;
    }

  /* These are two byte insns */
  if ((insn & 0xffff0000) == 0)
    {
      if ((insn & 0xf000) == 0x2000
	  || (insn & 0xf000) == 0x5000)
	return ((insn & 0xfc00) >> 8) & 0x3f;

      if ((insn & 0xf000) == 0x4000)
	return ((insn & 0xf300) >> 8) & 0x3f;

      if ((insn & 0xf000) == 0x8000
	  || (insn & 0xf000) == 0x9000
	  || (insn & 0xf000) == 0xa000
	  || (insn & 0xf000) == 0xb000)
	return ((insn & 0xf000) >> 8) & 0x3f;

      return ((insn & 0xff00) >> 8) & 0x3f;
    }

  /* These are three byte insns.  */
  if ((insn & 0xff000000) == 0)
    {
      if ((insn & 0xf00000) == 0x000000)
	return ((insn & 0xf30000) >> 16) & 0x3f;

      if ((insn & 0xf00000) == 0x200000
	  || (insn & 0xf00000) == 0x300000)
	return ((insn & 0xfc0000) >> 16) & 0x3f;

        return ((insn & 0xff0000) >> 16) & 0x3f;
    }

  /* These are four byte or larger insns.  */
  return ((insn & 0xff000000) >> 24) & 0x3f;
}

static struct hash_entry *
lookup_hash (ins, length)
     uint32 ins;
     int length;
{
  struct hash_entry *h;

  h = &hash_table[hash(ins)];

  while ((ins & h->mask) != h->opcode
	 || (length != h->ops->length))
    {
      if (h->next == NULL)
	{
	  (*mn10300_callback->printf_filtered) (mn10300_callback, "ERROR looking up hash for 0x%x, PC=0x%x\n", ins, PC);
	  exit(1);
	}
      h = h->next;
    }
  return (h);
}

/* FIXME These would more efficient to use than load_mem/store_mem,
   but need to be changed to use the memory map.  */

uint8
get_byte (x)
     uint8 *x;
{
  return *x;
}

uint16
get_half (x)
     uint8 *x;
{
  uint8 *a = x;
  return (a[1] << 8) + (a[0]);
}

uint32
get_word (x)
      uint8 *x;
{
  uint8 *a = x;
  return (a[3]<<24) + (a[2]<<16) + (a[1]<<8) + (a[0]);
}

void
put_byte (addr, data)
     uint8 *addr;
     uint8 data;
{
  uint8 *a = addr;
  a[0] = data;
}

void
put_half (addr, data)
     uint8 *addr;
     uint16 data;
{
  uint8 *a = addr;
  a[0] = data & 0xff;
  a[1] = (data >> 8) & 0xff;
}

void
put_word (addr, data)
     uint8 *addr;
     uint32 data;
{
  uint8 *a = addr;
  a[0] = data & 0xff;
  a[1] = (data >> 8) & 0xff;
  a[2] = (data >> 16) & 0xff;
  a[3] = (data >> 24) & 0xff;
}


uint32
load_mem_big (addr, len)
     SIM_ADDR addr;
     int len;
{
  uint8 *p = addr + State.mem;

  switch (len)
    {
    case 1:
      return p[0];
    case 2:
      return p[0] << 8 | p[1];
    case 3:
      return p[0] << 16 | p[1] << 8 | p[2];
    case 4:
      return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
    default:
      abort ();
    }
}

uint32
load_mem (addr, len)
     SIM_ADDR addr;
     int len;
{
  uint8 *p = addr + State.mem;

  switch (len)
    {
    case 1:
      return p[0];
    case 2:
      return p[1] << 8 | p[0];
    case 3:
      return p[2] << 16 | p[1] << 8 | p[0];
    case 4:
      return p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
    default:
      abort ();
    }
}

void
store_mem (addr, len, data)
     SIM_ADDR addr;
     int len;
     uint32 data;
{
  uint8 *p = addr + State.mem;

  switch (len)
    {
    case 1:
      p[0] = data;
      return;
    case 2:
      p[0] = data;
      p[1] = data >> 8;
      return;
    case 4:
      p[0] = data;
      p[1] = data >> 8;
      p[2] = data >> 16;
      p[3] = data >> 24;
      return;
    default:
      abort ();
    }
}

void
sim_size (power)
     int power;

{
  if (State.mem)
    free (State.mem);

  State.mem = (uint8 *) calloc (1,  1 << power);
  if (!State.mem)
    {
      (*mn10300_callback->printf_filtered) (mn10300_callback, "Allocation of main memory failed.\n");
      exit (1);
    }
}

static void
init_system ()
{
  if (!State.mem)
    sim_size(19);
}

int
sim_write (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;

  init_system ();

  for (i = 0; i < size; i++)
    store_mem (addr + i, 1, buffer[i]);

  return size;
}

SIM_DESC
sim_open (kind,argv)
     SIM_OPEN_KIND kind;
     char **argv;
{
  struct simops *s;
  struct hash_entry *h;
  char **p;

  sim_kind = kind;
  myname = argv[0];

  for (p = argv + 1; *p; ++p)
    {
      if (strcmp (*p, "-E") == 0)
	++p; /* ignore endian spec */
      else
#ifdef DEBUG
      if (strcmp (*p, "-t") == 0)
	mn10300_debug = DEBUG;
      else
#endif
	(*mn10300_callback->printf_filtered) (mn10300_callback, "ERROR: unsupported option(s): %s\n",*p);
    }

  /* put all the opcodes in the hash table */
  for (s = Simops; s->func; s++)
    {
      h = &hash_table[hash(s->opcode)];
      
      /* go to the last entry in the chain */
      while (h->next)
	  h = h->next;

      if (h->ops)
	{
	  h->next = calloc(1,sizeof(struct hash_entry));
	  h = h->next;
	}
      h->ops = s;
      h->mask = s->mask;
      h->opcode = s->opcode;
    }

  /* fudge our descriptor for now */
  return (SIM_DESC) 1;
}


void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  /* nothing to do */
}

void
sim_set_profile (n)
     int n;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "sim_set_profile %d\n", n);
}

void
sim_set_profile_size (n)
     int n;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "sim_set_profile_size %d\n", n);
}

void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  uint32 inst;
  reg_t oldpc;
  struct hash_entry *h;

  if (step)
    State.exception = SIGTRAP;
  else
    State.exception = 0;

  do
    {
      unsigned long insn, extension;

      /* Fetch the current instruction.  */
      inst = load_mem_big (PC, 1);
      oldpc = PC;

      /* These are one byte insns.  */
      if ((inst & 0xf3) == 0x00
	  || (inst & 0xf0) == 0x10
	  || (inst & 0xfc) == 0x3c
	  || (inst & 0xf3) == 0x41
	  || (inst & 0xf3) == 0x40
	  || (inst & 0xfc) == 0x50
	  || (inst & 0xfc) == 0x54
	  || (inst & 0xf0) == 0x60
	  || (inst & 0xf0) == 0x70
	  || ((inst & 0xf0) == 0x80
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || ((inst & 0xf0) == 0x90
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || ((inst & 0xf0) == 0xa0
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || ((inst & 0xf0) == 0xb0
	      && (inst & 0x0c) >> 2 != (inst & 0x03))
	  || (inst & 0xff) == 0xcb
	  || (inst & 0xfc) == 0xd0
	  || (inst & 0xfc) == 0xd4
	  || (inst & 0xfc) == 0xd8
	  || (inst & 0xf0) == 0xe0
	  || (inst & 0xff) == 0xff)
	{
	  insn = inst;
	  h = lookup_hash (insn, 1);
	  extension = 0;
	  (h->ops->func)(insn, extension);
	  PC += 1;
	}

      /* These are two byte insns.  */
      else if ((inst & 0xf0) == 0x80
	       || (inst & 0xf0) == 0x90
	       || (inst & 0xf0) == 0xa0
	       || (inst & 0xf0) == 0xb0
	       || (inst & 0xfc) == 0x20
	       || (inst & 0xfc) == 0x28
	       || (inst & 0xf3) == 0x43
	       || (inst & 0xf3) == 0x42
	       || (inst & 0xfc) == 0x58
	       || (inst & 0xfc) == 0x5c
	       || ((inst & 0xf0) == 0xc0
		   && (inst & 0xff) != 0xcb
		   && (inst & 0xff) != 0xcc
		   && (inst & 0xff) != 0xcd)
	       || (inst & 0xff) == 0xf0
	       || (inst & 0xff) == 0xf1
	       || (inst & 0xff) == 0xf2
	       || (inst & 0xff) == 0xf3
	       || (inst & 0xff) == 0xf4
	       || (inst & 0xff) == 0xf5
	       || (inst & 0xff) == 0xf6)
	{
	  insn = load_mem_big (PC, 2);
	  h = lookup_hash (insn, 2);
	  extension = 0;
	  (h->ops->func)(insn, extension);
	  PC += 2;
	}

      /* These are three byte insns.  */
      else if ((inst & 0xff) == 0xf8
	       || (inst & 0xff) == 0xcc 
	       || (inst & 0xff) == 0xf9
	       || (inst & 0xf3) == 0x01
	       || (inst & 0xf3) == 0x02
	       || (inst & 0xf3) == 0x03
	       || (inst & 0xfc) == 0x24
	       || (inst & 0xfc) == 0x2c
	       || (inst & 0xfc) == 0x30
	       || (inst & 0xfc) == 0x34
	       || (inst & 0xfc) == 0x38
	       || (inst & 0xff) == 0xde
	       || (inst & 0xff) == 0xdf
	       || (inst & 0xff) == 0xcc)
	{
	  insn = load_mem_big (PC, 3);
	  h = lookup_hash (insn, 3);
	  extension = 0;
	  /* If it's a format D1 insn, "ret", or "retf" insn, then
	     there's no need to worry about endianness.  Others have
	     a 16bit immediate in little endian form that we need to
	     extract.  */
	  if (h->ops->format == FMT_D1
	      || h->opcode == 0xdf0000
	      || h->opcode == 0xde0000)
	    (h->ops->func)(insn, extension);
	  else
	    {
	      insn &= 0xff0000;
	      insn |= load_mem (PC + 1, 2);
	      (h->ops->func)(insn, extension);
	    }
	  PC += 3;
	}

      /* These are four byte insns.  */
      else if ((inst & 0xff) == 0xfa
	       || (inst & 0xff) == 0xfb)
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 4);
	  extension = 0;
	  /* This must be a format D2 insn; a small number of such insns
	     don't have any 16bit immediates (they instead have two 8 bit
	     immediates).  */
 	  if (h->opcode == 0xfaf80000
	      || h->opcode == 0xfaf00000
	      || h->opcode == 0xfaf40000)
	    (h->ops->func)(insn, extension);
	  else
	    {
	      insn &= 0xffff0000;
	      insn |= load_mem (PC + 2, 2);
	      (h->ops->func)(insn, extension);
	    }
	  PC += 4;
	}

      /* These are five byte insns.  */
      else if ((inst & 0xff) == 0xcd
	       || (inst & 0xff) == 0xdc)
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 5);

	  /* This must be a format S4 insn.  */
	  if (h->opcode == 0xdc000000)
	    {
	      /* A "jmp" instruction with a 32bit immediate stored
	    	 in little endian form.  */
	      unsigned long temp;
	      temp = load_mem (PC + 1, 4);
	      insn &= 0xff000000;
	      insn |= (temp & 0xffffff00) >> 8;
	      extension = temp & 0xff;
	    }
	  else
	    {
	      /* A "call" instruction with a 16bit immediate in little
		 endian form.  */
	      unsigned long temp;
	      temp = load_mem (PC + 1, 2);
	      insn &= 0xff0000ff;
	      insn |= temp << 8;
	      extension = load_mem (PC + 4, 1);
	    }
	  (h->ops->func)(insn, extension);
	  PC += 5;
	}

      /* These are six byte insns.  */
      else if ((inst & 0xff) == 0xfd
	       || (inst & 0xff) == 0xfc)
	{
	  unsigned long temp;

	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 6);

	  temp = load_mem (PC + 2, 4);
	  insn &= 0xffff0000;
	  insn |= (temp >> 16) & 0xffff;
	  extension = temp & 0xffff;
	  (h->ops->func)(insn, extension);
	  PC += 6;
	}

      /* Else its a seven byte insns (in theory).  */
      else
	{
	  insn = load_mem_big (PC, 4);
	  h = lookup_hash (insn, 7);

	  if (h->ops->format == FMT_S6)
	    {
	      unsigned long temp;

	      temp = load_mem (PC + 1, 4);
	      insn &= 0xff000000;
	      insn |= (temp >> 8) & 0xffffff;

	      extension = (temp & 0xff) << 16;
	      extension |= load_mem (PC + 5, 1) << 8;
	      extension |= load_mem (PC + 6, 1);
	    }
	  else
	    {
	      unsigned long temp;

	      temp = load_mem (PC + 2, 4);
	      insn &= 0xffff0000;
	      insn |= (temp >> 16) & 0xffff;
	      extension = (temp & 0xffff) << 8;
	      extension = load_mem (PC + 6, 1);
	    }
	  (h->ops->func)(insn, extension);
	  PC += 7;
	}
    }
  while (!State.exception);
}

int
sim_trace (sd)
     SIM_DESC sd;
{
#ifdef DEBUG
  mn10300_debug = DEBUG;
#endif
  sim_resume (sd, 0, 0);
  return 1;
}

void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "sim_info\n");
}

SIM_RC
sim_create_inferior (sd, argv, env)
     SIM_DESC sd;
     char **argv;
     char **env;
{
  return SIM_RC_OK;
}

void
sim_kill (sd)
     SIM_DESC sd;
{
  /* nothing to do */
}

void
sim_set_callbacks (sd, p)
     SIM_DESC sd;
     host_callback *p;
{
  mn10300_callback = p;
}

/* All the code for exiting, signals, etc needs to be revamped.

   This is enough to get c-torture limping though.  */

void
sim_stop_reason (sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop *reason;
     int *sigrc;
{
  *reason = sim_stopped;
  if (State.exception == SIGQUIT)
    *sigrc = 0;
  else
    *sigrc = State.exception;
}

void
sim_fetch_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  put_word (memory, State.regs[rn]);
}
 
void
sim_store_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  State.regs[rn] = get_word (memory);
}

int
sim_read (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  for (i = 0; i < size; i++)
    buffer[i] = load_mem (addr + i, 1);

  return size;
} 

void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{
  (*mn10300_callback->printf_filtered) (mn10300_callback, "\"%s\" is not a valid mn10300 simulator command.\n", cmd);
}

SIM_RC
sim_load (sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char *prog;
     bfd *abfd;
     int from_tty;
{
  extern bfd *sim_load_file (); /* ??? Don't know where this should live.  */
  bfd *prog_bfd;

  prog_bfd = sim_load_file (sd, myname, mn10300_callback, prog, abfd,
			    sim_kind == SIM_OPEN_DEBUG);
  if (prog_bfd == NULL)
    return SIM_RC_FAIL;
  PC = bfd_get_start_address (prog_bfd);
  if (abfd == NULL)
    bfd_close (prog_bfd);
  return SIM_RC_OK;
} 
