#include <signal.h>
#include "sysdep.h"
#include "bfd.h"
#include "callback.h"
#include "remote-sim.h"

#include "d10v_sim.h"

#define IMEM_SIZE 18		/* D10V instruction memory size is 18 bits */
#define DMEM_SIZE 16		/* Data memory is 64K (but only 32K internal RAM) */
#define UMEM_SIZE 17		/* Each unified memory segment is 17 bits */
#define UMEM_SEGMENTS 128	/* Number of segments in unified memory region */

enum _leftright { LEFT_FIRST, RIGHT_FIRST };

static char *myname;
static SIM_OPEN_KIND sim_kind;
int d10v_debug;
host_callback *d10v_callback;
unsigned long ins_type_counters[ (int)INS_MAX ];

uint16 OP[4];

static int init_text_p = 0;
/* non-zero if we opened prog_bfd */
static int prog_bfd_was_opened_p;
bfd *prog_bfd;
asection *text;
bfd_vma text_start;
bfd_vma text_end;

static long hash PARAMS ((long insn, int format));
static struct hash_entry *lookup_hash PARAMS ((uint32 ins, int size));
static void get_operands PARAMS ((struct simops *s, uint32 ins));
static void do_long PARAMS ((uint32 ins));
static void do_2_short PARAMS ((uint16 ins1, uint16 ins2, enum _leftright leftright));
static void do_parallel PARAMS ((uint16 ins1, uint16 ins2));
static char *add_commas PARAMS ((char *buf, int sizeof_buf, unsigned long value));
extern void sim_set_profile PARAMS ((int n));
extern void sim_set_profile_size PARAMS ((int n));

#ifndef INLINE
#if defined(__GNUC__) && defined(__OPTIMIZE__)
#define INLINE __inline__
#else
#define INLINE
#endif
#endif

#define MAX_HASH  63
struct hash_entry
{
  struct hash_entry *next;
  long opcode;
  long mask;
  int size;
  struct simops *ops;
};

struct hash_entry hash_table[MAX_HASH+1];

INLINE static long 
hash(insn, format)
     long insn;
     int format;
{
  if (format & LONG_OPCODE)
    return ((insn & 0x3F000000) >> 24);
  else
    return((insn & 0x7E00) >> 9);
}

INLINE static struct hash_entry *
lookup_hash (ins, size)
     uint32 ins;
     int size;
{
  struct hash_entry *h;

  if (size)
    h = &hash_table[(ins & 0x3F000000) >> 24];
  else
    h = &hash_table[(ins & 0x7E00) >> 9];

  while ((ins & h->mask) != h->opcode || h->size != size)
    {
      if (h->next == NULL)
	{
	  (*d10v_callback->printf_filtered) (d10v_callback, "ERROR looking up hash for %x at PC %x\n",ins, PC);
	  exit (1);
	}
      h = h->next;
    }
  return (h);
}

INLINE static void
get_operands (struct simops *s, uint32 ins)
{
  int i, shift, bits, flags;
  uint32 mask;
  for (i=0; i < s->numops; i++)
    {
      shift = s->operands[3*i];
      bits = s->operands[3*i+1];
      flags = s->operands[3*i+2];
      mask = 0x7FFFFFFF >> (31 - bits);
      OP[i] = (ins >> shift) & mask;
    }
}

bfd_vma
decode_pc ()
{
  asection *s;
  if (!init_text_p && prog_bfd != NULL)
    {
      init_text_p = 1;
      for (s = prog_bfd->sections; s; s = s->next)
	if (strcmp (bfd_get_section_name (prog_bfd, s), ".text") == 0)
	  {
	    text = s;
	    text_start = bfd_get_section_vma (prog_bfd, s);
	    text_end = text_start + bfd_section_size (prog_bfd, s);
	    break;
	  }
    }

  return (PC << 2) + text_start;
}

static void
do_long (ins)
     uint32 ins;
{
  struct hash_entry *h;
#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    (*d10v_callback->printf_filtered) (d10v_callback, "do_long 0x%x\n", ins);
#endif
  h = lookup_hash (ins, 1);
  get_operands (h->ops, ins);
  State.ins_type = INS_LONG;
  ins_type_counters[ (int)State.ins_type ]++;
  (h->ops->func)();
}

static void
do_2_short (ins1, ins2, leftright)
     uint16 ins1, ins2;
     enum _leftright leftright;
{
  struct hash_entry *h;
  reg_t orig_pc = PC;
  enum _ins_type first, second;

#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    (*d10v_callback->printf_filtered) (d10v_callback, "do_2_short 0x%x (%s) -> 0x%x\n",
				       ins1, (leftright) ? "left" : "right", ins2);
#endif

  if (leftright == LEFT_FIRST)
    {
      first = INS_LEFT;
      second = INS_RIGHT;
      ins_type_counters[ (int)INS_LEFTRIGHT ]++;
    }
  else
    {
      first = INS_RIGHT;
      second = INS_LEFT;
      ins_type_counters[ (int)INS_RIGHTLEFT ]++;
    }

  h = lookup_hash (ins1, 0);
  get_operands (h->ops, ins1);
  State.ins_type = first;
  ins_type_counters[ (int)State.ins_type ]++;
  (h->ops->func)();

  /* If the PC has changed (ie, a jump), don't do the second instruction */
  if (orig_pc == PC && !State.exception)
    {
      h = lookup_hash (ins2, 0);
      get_operands (h->ops, ins2);
      State.ins_type = second;
      ins_type_counters[ (int)State.ins_type ]++;
      ins_type_counters[ (int)INS_CYCLES ]++;
      (h->ops->func)();
    }
  else if (orig_pc != PC && !State.exception)
    ins_type_counters[ (int)INS_COND_JUMP ]++;
}

static void
do_parallel (ins1, ins2)
     uint16 ins1, ins2;
{
  struct hash_entry *h1, *h2;
#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    (*d10v_callback->printf_filtered) (d10v_callback, "do_parallel 0x%x || 0x%x\n", ins1, ins2);
#endif
  ins_type_counters[ (int)INS_PARALLEL ]++;
  h1 = lookup_hash (ins1, 0);
  h2 = lookup_hash (ins2, 0);

  if (h1->ops->exec_type == PARONLY)
    {
      get_operands (h1->ops, ins1);
      State.ins_type = INS_LEFT_COND_TEST;
      ins_type_counters[ (int)State.ins_type ]++;
      (h1->ops->func)();
      if (State.exe)
	{
	  ins_type_counters[ (int)INS_COND_TRUE ]++;
	  get_operands (h2->ops, ins2);
	  State.ins_type = INS_RIGHT_COND_EXE;
	  ins_type_counters[ (int)State.ins_type ]++;
	  (h2->ops->func)();
	}
      else
	ins_type_counters[ (int)INS_COND_FALSE ]++;
    }
  else if (h2->ops->exec_type == PARONLY)
    {
      get_operands (h2->ops, ins2);
      State.ins_type = INS_RIGHT_COND_TEST;
      ins_type_counters[ (int)State.ins_type ]++;
      (h2->ops->func)();
      if (State.exe)
	{
	  ins_type_counters[ (int)INS_COND_TRUE ]++;
	  get_operands (h1->ops, ins1);
	  State.ins_type = INS_LEFT_COND_EXE;
	  ins_type_counters[ (int)State.ins_type ]++;
	  (h1->ops->func)();
	}
      else
	ins_type_counters[ (int)INS_COND_FALSE ]++;
    }
  else
    {
      get_operands (h1->ops, ins1);
      State.ins_type = INS_LEFT_PARALLEL;
      ins_type_counters[ (int)State.ins_type ]++;
      (h1->ops->func)();
      if (!State.exception)
	{
	  get_operands (h2->ops, ins2);
	  State.ins_type = INS_RIGHT_PARALLEL;
	  ins_type_counters[ (int)State.ins_type ]++;
	  (h2->ops->func)();
	}
    }
}
 
static char *
add_commas(buf, sizeof_buf, value)
     char *buf;
     int sizeof_buf;
     unsigned long value;
{
  int comma = 3;
  char *endbuf = buf + sizeof_buf - 1;

  *--endbuf = '\0';
  do {
    if (comma-- == 0)
      {
	*--endbuf = ',';
	comma = 2;
      }

    *--endbuf = (value % 10) + '0';
  } while ((value /= 10) != 0);

  return endbuf;
}

void
sim_size (power)
     int power;

{
  int i;

  if (State.imem)
    {
      for (i=0;i<UMEM_SEGMENTS;i++)
	{
	  if (State.umem[i])
	    {
	      free (State.umem[i]);
	      State.umem[i] = NULL;
	    }
	}
      free (State.imem);
      free (State.dmem);
    }

  State.imem = (uint8 *)calloc(1,1<<IMEM_SIZE);
  State.dmem = (uint8 *)calloc(1,1<<DMEM_SIZE);
  for (i=1;i<(UMEM_SEGMENTS-1);i++)
    State.umem[i] = NULL;
  State.umem[0] = (uint8 *)calloc(1,1<<UMEM_SIZE);
  State.umem[1] = (uint8 *)calloc(1,1<<UMEM_SIZE);
  State.umem[2] = (uint8 *)calloc(1,1<<UMEM_SIZE);
  State.umem[UMEM_SEGMENTS-1] = (uint8 *)calloc(1,1<<UMEM_SIZE);
  if (!State.imem || !State.dmem || !State.umem[0] || !State.umem[1] || !State.umem[2] || !State.umem[UMEM_SEGMENTS-1] )
    {
      (*d10v_callback->printf_filtered) (d10v_callback, "Memory allocation failed.\n");
      exit(1);
    }
  
#ifdef DEBUG
  if ((d10v_debug & DEBUG_MEMSIZE) != 0)
    {
      char buffer[20];
      (*d10v_callback->printf_filtered) (d10v_callback,
					 "Allocated %s bytes instruction memory and\n",
					 add_commas (buffer, sizeof (buffer), (1UL<<IMEM_SIZE)));

      (*d10v_callback->printf_filtered) (d10v_callback, "          %s bytes data memory.\n",
					 add_commas (buffer, sizeof (buffer), (1UL<<IMEM_SIZE)));
    }
#endif
}

/* Transfer data to/from simulated memory.  Since a bug in either the
   simulated program or in gdb or the simulator itself may cause a
   bogus address to be passed in, we need to do some sanity checking
   on addresses to make sure they are within bounds.  When an address
   fails the bounds check, treat it as a zero length read/write rather
   than aborting the entire run. */

static int
xfer_mem (SIM_ADDR addr,
	  unsigned char *buffer,
	  int size,
	  int write_p)
{
  unsigned char *memory;
  int segment = ((addr >> 24) & 0xff);
  addr = (addr & 0x00ffffff);

#ifdef DEBUG
  if ((d10v_debug & DEBUG_INSTRUCTION) != 0)
    {
      if (write_p)
	{
	  (*d10v_callback->printf_filtered) (d10v_callback, "sim_write %d bytes to 0x%02x:%06x\n", size, segment, addr);
	}
      else
	{
	  (*d10v_callback->printf_filtered) (d10v_callback, "sim_read %d bytes from 0x%2x:%6x\n", size, segment, addr);
	}
    }
#endif

  /* to access data, we use the following mapping 
      0x00xxxxxx: Logical data address segment        (DMAP translated memory)
      0x01xxxxxx: Logical instruction address segment (IMAP translated memory)
      0x10xxxxxx: Physical data memory segment        (On-chip data memory)
      0x11xxxxxx: Physical instruction memory segment (On-chip insn memory)
      0x12xxxxxx: Phisical unified memory segment     (Unified memory)
   */

  switch (segment)
    {
    case 0x00: /* DMAP translated memory */
      {
	int byte;
	for (byte = 0; byte < size; byte++)
	  {
	    uint8 *mem = dmem_addr (addr + byte);
	    if (mem == NULL)
	      return byte;
	    else if (write_p)
	      *mem = buffer[byte];
	    else
	      buffer[byte] = *mem;
	  }
	return byte;
      }

    case 0x01: /* IMAP translated memory */
      {
	int byte;
	for (byte = 0; byte < size; byte++)
	  {
	    uint8 *mem = imem_addr (addr + byte);
	    if (mem == NULL)
	      return byte;
	    else if (write_p)
	      *mem = buffer[byte];
	    else
	      buffer[byte] = *mem;
	  }
	return byte;
      }

    case 0x10: /* On-chip data memory */
      {
	addr &= ((1 << DMEM_SIZE) - 1);
	if ((addr + size) > (1 << DMEM_SIZE))
	  {
	    (*d10v_callback->printf_filtered) (d10v_callback, "ERROR: data address 0x%x is outside range 0-0x%x.\n",
					       addr + size - 1, (1 << DMEM_SIZE) - 1);
	    return (0);
	  }
	memory = State.dmem + addr;
	break;
      }

    case 0x11: /* On-chip insn memory */
      {
	addr &= ((1 << IMEM_SIZE) - 1);
	if ((addr + size) > (1 << IMEM_SIZE))
	  {
	    (*d10v_callback->printf_filtered) (d10v_callback, "ERROR: instruction address 0x%x is outside range 0-0x%x.\n",
					       addr + size - 1, (1 << IMEM_SIZE) - 1);
	    return (0);
	  }
	memory = State.imem + addr;
      }

    case 0x12: /* Unified memory */
      {
	int startsegment, startoffset;	/* Segment and offset within segment where xfer starts */
	int endsegment, endoffset;	/* Segment and offset within segment where xfer ends */
	
	startsegment = addr >> UMEM_SIZE;
	startoffset = addr & ((1 << UMEM_SIZE) - 1);
	endsegment = (addr + size) >> UMEM_SIZE;
	endoffset = (addr + size) & ((1 << UMEM_SIZE) - 1);
	
	/* FIXME: We do not currently implement xfers across segments,
           so detect this case and fail gracefully. */
	
	if ((startsegment != endsegment) && !((endsegment == (startsegment + 1)) && endoffset == 0))
	  {
	    (*d10v_callback->printf_filtered) (d10v_callback, "ERROR: Unimplemented support for transfers across unified memory segment boundaries\n");
	    return (0);
	  }
	if (!State.umem[startsegment])
	  {
#ifdef DEBUG
	    if ((d10v_debug & DEBUG_MEMSIZE) != 0)
	      {
		(*d10v_callback->printf_filtered) (d10v_callback,"Allocating %s bytes unified memory to region %d\n",
						   add_commas (buffer, sizeof (buffer), (1UL<<IMEM_SIZE)), startsegment);
	      }
#endif
	    State.umem[startsegment] = (uint8 *)calloc(1,1<<UMEM_SIZE);
	  }
	if (!State.umem[startsegment])
	  {
	    (*d10v_callback->printf_filtered) (d10v_callback, "ERROR: Memory allocation of 0x%x bytes failed.\n", 1<<UMEM_SIZE);
	    return (0);
	  }
	memory = State.umem[startsegment] + startoffset;
	break;
      }

    default:
      {
	(*d10v_callback->printf_filtered) (d10v_callback, "ERROR: address 0x%lx is not in valid range\n", (long) addr);
 	(*d10v_callback->printf_filtered) (d10v_callback, "0x00xxxxxx:  Logical data address segment		(DMAP translated memory)\n");
 	(*d10v_callback->printf_filtered) (d10v_callback, "0x01xxxxxx:  Logical instruction address segment	(IMAP translated memory)\n");
 	(*d10v_callback->printf_filtered) (d10v_callback, "0x10xxxxxx:  Physical data memory segment		(On-chip data memory)\n");
 	(*d10v_callback->printf_filtered) (d10v_callback, "0x11xxxxxx:  Physical instruction memory segment	(On-chip insn memory)\n");
 	(*d10v_callback->printf_filtered) (d10v_callback, "0x12xxxxxx:  Phisical unified memory segment		(Unified memory)\n");
	return (0);
      }
    }

  if (write_p)
    {
      memcpy (memory, buffer, size);
    }
  else
    {
      memcpy (buffer, memory, size);
    }

  return size;
}


int
sim_write (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  /* FIXME: this should be performing a virtual transfer */
  return xfer_mem( addr, buffer, size, 1);
}

int
sim_read (sd, addr, buffer, size)
     SIM_DESC sd;
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  /* FIXME: this should be performing a virtual transfer */
  return xfer_mem( addr, buffer, size, 0);
}


SIM_DESC
sim_open (kind, callback, abfd, argv)
     SIM_OPEN_KIND kind;
     host_callback *callback;
     struct _bfd *abfd;
     char **argv;
{
  struct simops *s;
  struct hash_entry *h;
  static int init_p = 0;
  char **p;

  sim_kind = kind;
  d10v_callback = callback;
  myname = argv[0];

  for (p = argv + 1; *p; ++p)
    {
#ifdef DEBUG
      if (strcmp (*p, "-t") == 0)
	d10v_debug = DEBUG;
      else
#endif
	(*d10v_callback->printf_filtered) (d10v_callback, "ERROR: unsupported option(s): %s\n",*p);
    }
  
  /* put all the opcodes in the hash table */
  if (!init_p++)
    {
      for (s = Simops; s->func; s++)
	{
	  h = &hash_table[hash(s->opcode,s->format)];
      
	  /* go to the last entry in the chain */
	  while (h->next)
	    h = h->next;

	  if (h->ops)
	    {
	      h->next = (struct hash_entry *) calloc(1,sizeof(struct hash_entry));
	      if (!h->next)
		perror ("malloc failure");

	      h = h->next;
	    }
	  h->ops = s;
	  h->mask = s->mask;
	  h->opcode = s->opcode;
	  h->size = s->is_long;
	}
    }

  /* reset the processor state */
  if (!State.imem)
    sim_size(1);
  sim_create_inferior ((SIM_DESC) 1, NULL, NULL, NULL);

  /* Fudge our descriptor.  */
  return (SIM_DESC) 1;
}


void
sim_close (sd, quitting)
     SIM_DESC sd;
     int quitting;
{
  if (prog_bfd != NULL && prog_bfd_was_opened_p)
    {
      bfd_close (prog_bfd);
      prog_bfd = NULL;
      prog_bfd_was_opened_p = 0;
    }
}

void
sim_set_profile (n)
     int n;
{
  (*d10v_callback->printf_filtered) (d10v_callback, "sim_set_profile %d\n",n);
}

void
sim_set_profile_size (n)
     int n;
{
  (*d10v_callback->printf_filtered) (d10v_callback, "sim_set_profile_size %d\n",n);
}


uint8 *
dmem_addr( addr )
     uint32 addr;
{
  int seg;

  addr &= 0xffff;

  if (addr > 0xbfff)
    {
      if ( (addr & 0xfff0) != 0xff00)
	{
	  (*d10v_callback->printf_filtered) (d10v_callback, "Data address 0x%lx is in I/O space, pc = 0x%lx.\n",
					     (long)addr, (long)decode_pc ());
	  State.exception = SIGBUS;
	}

      return State.dmem + addr;
    }
  
  if (addr > 0x7fff)
    {
      if (DMAP & 0x1000)
	{
	  /* instruction memory */
	  return (DMAP & 0xf) * 0x4000 + State.imem;
	}
      /* unified memory */
      /* this is ugly because we allocate unified memory in 128K segments and */
      /* dmap addresses 16k segments */
      seg = (DMAP & 0x3ff) >> 3;
      if (State.umem[seg] == NULL)
	{
	  (*d10v_callback->printf_filtered) (d10v_callback, "ERROR:  unified memory region %d unmapped, pc = 0x%lx\n",
					     seg, (long)decode_pc ());
	  State.exception = SIGBUS;
	}
      return State.umem[seg] + (DMAP & 7) * 0x4000;
    }

  return State.dmem + addr;
}


uint8 *
imem_addr (uint32 pc)
{
  uint16 imap;

  if (pc & 0x20000)
    imap = IMAP1;
  else
    imap = IMAP0;
  
  if (imap & 0x1000)
    return State.imem + pc;

  if (State.umem[imap & 0xff] == NULL)
    return 0;

  /* Discard upper bit(s) of PC in case IMAP1 selects unified memory. */
  pc &= (1 << UMEM_SIZE) - 1;

  return State.umem[imap & 0xff] + pc;
}


static int stop_simulator = 0;

int
sim_stop (sd)
     SIM_DESC sd;
{
  stop_simulator = 1;
  return 1;
}


/* Run (or resume) the program.  */
void
sim_resume (sd, step, siggnal)
     SIM_DESC sd;
     int step, siggnal;
{
  uint32 inst;
  int do_iba;
  uint8 *iaddr;

/*   (*d10v_callback->printf_filtered) (d10v_callback, "sim_resume (%d,%d)  PC=0x%x\n",step,siggnal,PC); */
  State.exception = 0;
  if (step)
    sim_stop (sd);

  do
    {
      iaddr = imem_addr ((uint32)PC << 2);
      if (iaddr == NULL)
 	{
 	  State.exception = SIGBUS;
 	  break;
 	}
 
      inst = get_longword( iaddr ); 
 
      State.pc_changed = 0;
      ins_type_counters[ (int)INS_CYCLES ]++;
      
      /* check to see if IBA should be triggered after
	 this instruction */
      if (State.DB && (PC == IBA))
	do_iba = 1;
      else
	do_iba = 0;

      switch (inst & 0xC0000000)
	{
	case 0xC0000000:
	  /* long instruction */
	  do_long (inst & 0x3FFFFFFF);
	  break;
	case 0x80000000:
	  /* R -> L */
	  do_2_short ( inst & 0x7FFF, (inst & 0x3FFF8000) >> 15, RIGHT_FIRST);
	  break;
	case 0x40000000:
	  /* L -> R */
	  do_2_short ((inst & 0x3FFF8000) >> 15, inst & 0x7FFF, LEFT_FIRST);
	  break;
	case 0:
	  do_parallel ((inst & 0x3FFF8000) >> 15, inst & 0x7FFF);
	  break;
	}
      
      /* calculate the next PC */
      if (!State.pc_changed)
	{
	  if (State.RP && PC == RPT_E)
	    {
	      /* Note: The behavour of a branch instruction at RPT_E
                 is implementation dependant, this simulator takes the
                 branch.  Branching to RPT_E is valid, the instruction
                 must be executed before the loop is taken.  */
	      RPT_C -= 1;
	      if (RPT_C == 0)
		{
		  State.RP = 0;
		  PC++;
		}
	      else
		PC = RPT_S;
	    }
	  else
	    PC++;
	}
      
      if (do_iba)
	{
	  BPC = PC;
	  move_to_cr (BPSW_CR, PSW);
	  move_to_cr (PSW_CR, PSW & PSW_SM_BIT);
	  PC = SDBT_VECTOR_START;
	}
    }
  while ( !State.exception && !stop_simulator);
  
  if (step && !State.exception)
    State.exception = SIGTRAP;
}

int
sim_trace (sd)
     SIM_DESC sd;
{
#ifdef DEBUG
  d10v_debug = DEBUG;
#endif
  sim_resume (sd, 0, 0);
  return 1;
}

void
sim_info (sd, verbose)
     SIM_DESC sd;
     int verbose;
{
  char buf1[40];
  char buf2[40];
  char buf3[40];
  char buf4[40];
  char buf5[40];
  unsigned long left		= ins_type_counters[ (int)INS_LEFT ] + ins_type_counters[ (int)INS_LEFT_COND_EXE ];
  unsigned long left_nops	= ins_type_counters[ (int)INS_LEFT_NOPS ];
  unsigned long left_parallel	= ins_type_counters[ (int)INS_LEFT_PARALLEL ];
  unsigned long left_cond	= ins_type_counters[ (int)INS_LEFT_COND_TEST ];
  unsigned long left_total	= left + left_parallel + left_cond + left_nops;

  unsigned long right		= ins_type_counters[ (int)INS_RIGHT ] + ins_type_counters[ (int)INS_RIGHT_COND_EXE ];
  unsigned long right_nops	= ins_type_counters[ (int)INS_RIGHT_NOPS ];
  unsigned long right_parallel	= ins_type_counters[ (int)INS_RIGHT_PARALLEL ];
  unsigned long right_cond	= ins_type_counters[ (int)INS_RIGHT_COND_TEST ];
  unsigned long right_total	= right + right_parallel + right_cond + right_nops;

  unsigned long unknown		= ins_type_counters[ (int)INS_UNKNOWN ];
  unsigned long ins_long	= ins_type_counters[ (int)INS_LONG ];
  unsigned long parallel	= ins_type_counters[ (int)INS_PARALLEL ];
  unsigned long leftright	= ins_type_counters[ (int)INS_LEFTRIGHT ];
  unsigned long rightleft	= ins_type_counters[ (int)INS_RIGHTLEFT ];
  unsigned long cond_true	= ins_type_counters[ (int)INS_COND_TRUE ];
  unsigned long cond_false	= ins_type_counters[ (int)INS_COND_FALSE ];
  unsigned long cond_jump	= ins_type_counters[ (int)INS_COND_JUMP ];
  unsigned long cycles		= ins_type_counters[ (int)INS_CYCLES ];
  unsigned long total		= (unknown + left_total + right_total + ins_long);

  int size			= strlen (add_commas (buf1, sizeof (buf1), total));
  int parallel_size		= strlen (add_commas (buf1, sizeof (buf1),
						      (left_parallel > right_parallel) ? left_parallel : right_parallel));
  int cond_size			= strlen (add_commas (buf1, sizeof (buf1), (left_cond > right_cond) ? left_cond : right_cond));
  int nop_size			= strlen (add_commas (buf1, sizeof (buf1), (left_nops > right_nops) ? left_nops : right_nops));
  int normal_size		= strlen (add_commas (buf1, sizeof (buf1), (left > right) ? left : right));

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*s left  instruction(s), %*s normal, %*s parallel, %*s EXExxx, %*s nops\n",
				     size, add_commas (buf1, sizeof (buf1), left_total),
				     normal_size, add_commas (buf2, sizeof (buf2), left),
				     parallel_size, add_commas (buf3, sizeof (buf3), left_parallel),
				     cond_size, add_commas (buf4, sizeof (buf4), left_cond),
				     nop_size, add_commas (buf5, sizeof (buf5), left_nops));

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*s right instruction(s), %*s normal, %*s parallel, %*s EXExxx, %*s nops\n",
				     size, add_commas (buf1, sizeof (buf1), right_total),
				     normal_size, add_commas (buf2, sizeof (buf2), right),
				     parallel_size, add_commas (buf3, sizeof (buf3), right_parallel),
				     cond_size, add_commas (buf4, sizeof (buf4), right_cond),
				     nop_size, add_commas (buf5, sizeof (buf5), right_nops));

  if (ins_long)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "executed %*s long instruction(s)\n",
				       size, add_commas (buf1, sizeof (buf1), ins_long));

  if (parallel)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "executed %*s parallel instruction(s)\n",
				       size, add_commas (buf1, sizeof (buf1), parallel));

  if (leftright)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "executed %*s instruction(s) encoded L->R\n",
				       size, add_commas (buf1, sizeof (buf1), leftright));

  if (rightleft)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "executed %*s instruction(s) encoded R->L\n",
				       size, add_commas (buf1, sizeof (buf1), rightleft));

  if (unknown)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "executed %*s unknown instruction(s)\n",
				       size, add_commas (buf1, sizeof (buf1), unknown));

  if (cond_true)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "executed %*s instruction(s) due to EXExxx condition being true\n",
				       size, add_commas (buf1, sizeof (buf1), cond_true));

  if (cond_false)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "skipped  %*s instruction(s) due to EXExxx condition being false\n",
				       size, add_commas (buf1, sizeof (buf1), cond_false));

  if (cond_jump)
    (*d10v_callback->printf_filtered) (d10v_callback,
				       "skipped  %*s instruction(s) due to conditional branch succeeding\n",
				       size, add_commas (buf1, sizeof (buf1), cond_jump));

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*s cycle(s)\n",
				     size, add_commas (buf1, sizeof (buf1), cycles));

  (*d10v_callback->printf_filtered) (d10v_callback,
				     "executed %*s total instructions\n",
				     size, add_commas (buf1, sizeof (buf1), total));
}

SIM_RC
sim_create_inferior (sd, abfd, argv, env)
     SIM_DESC sd;
     struct _bfd *abfd;
     char **argv;
     char **env;
{
  bfd_vma start_address;

  /* reset all state information */
  memset (&State.regs, 0, (int)&State.imem - (int)&State.regs[0]);

  if (argv)
    {
      /* a hack to set r0/r1 with argc/argv */
      /* some high memory that won't be overwritten by the stack soon */
      addr = State.regs[0] = 0x7C00;
      p = 20;
      i = 0;
      while (argv[i])
 	{
 	  SW (addr + 2*i, addr + p); 
 	  size = strlen (argv[i]) + 1;
 	  sim_write (sd, addr + 0, argv[i], size);
 	  p += size;
 	  i++;
 	}
      State.regs[1] = i;
    }

  /* set PC */
  if (abfd != NULL)
    start_address = bfd_get_start_address (abfd);
  else
    start_address = 0xffc0 << 2;
#ifdef DEBUG
  if (d10v_debug)
    (*d10v_callback->printf_filtered) (d10v_callback, "sim_create_inferior:  PC=0x%lx\n", (long) start_address);
#endif
  PC = start_address >> 2;

  /* cpu resets imap0 to 0 and imap1 to 0x7f, but D10V-EVA board */
  /* resets imap0 and imap1 to 0x1000. */
  if (1)
    {
      SET_IMAP0 (0x0000);
      SET_IMAP1 (0x007f);
      SET_DMAP (0x0000);
    }
  else
    {
      SET_IMAP0(0x1000);
      SET_IMAP1(0x1000);
      SET_DMAP(0);
    }

  return SIM_RC_OK;
}


void
sim_set_callbacks (p)
     host_callback *p;
{
  d10v_callback = p;
}

void
sim_stop_reason (sd, reason, sigrc)
     SIM_DESC sd;
     enum sim_stop *reason;
     int *sigrc;
{
/*   (*d10v_callback->printf_filtered) (d10v_callback, "sim_stop_reason:  PC=0x%x\n",PC<<2); */

  switch (State.exception)
    {
    case SIG_D10V_STOP:			/* stop instruction */
      *reason = sim_exited;
      *sigrc = 0;
      break;

    case SIG_D10V_EXIT:			/* exit trap */
      *reason = sim_exited;
      *sigrc = State.regs[0];
      break;

    default:				/* some signal */
      *reason = sim_stopped;
      if (stop_simulator && !State.exception)
	*sigrc = SIGINT;
      else
	*sigrc = State.exception;
      break;
    }

  stop_simulator = 0;
}

void
sim_fetch_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  if (rn > 34)
    WRITE_64 (memory, State.a[rn-35]);
  else if (rn == 32)
    WRITE_16 (memory, IMAP0);
  else if (rn == 33)
    WRITE_16 (memory, IMAP1);
  else if (rn == 34)
    WRITE_16 (memory, DMAP);
  else if (rn >= 16)
    WRITE_16 (memory, move_from_cr (rn - 16));
  else
    WRITE_16 (memory, State.regs[rn]);
}
 
void
sim_store_register (sd, rn, memory)
     SIM_DESC sd;
     int rn;
     unsigned char *memory;
{
  if (rn > 34)
    State.a[rn-35] =  READ_64 (memory) & MASK40;
  else if (rn == 34)
    SET_DMAP( READ_16(memory) );
  else if (rn == 33)
    SET_IMAP1( READ_16(memory) );
  else if (rn == 32)
    SET_IMAP0( READ_16(memory) );
  else if (rn >= 16)
    move_to_cr (rn - 16, READ_16 (memory));
  else
    State.regs[rn]= READ_16 (memory);
}


void
sim_do_command (sd, cmd)
     SIM_DESC sd;
     char *cmd;
{ 
  (*d10v_callback->printf_filtered) (d10v_callback, "sim_do_command: %s\n",cmd);
}

SIM_RC
sim_load (sd, prog, abfd, from_tty)
     SIM_DESC sd;
     char *prog;
     bfd *abfd;
     int from_tty;
{
  extern bfd *sim_load_file (); /* ??? Don't know where this should live.  */

  if (prog_bfd != NULL && prog_bfd_was_opened_p)
    {
      bfd_close (prog_bfd);
      prog_bfd_was_opened_p = 0;
    }
  prog_bfd = sim_load_file (sd, myname, d10v_callback, prog, abfd,
			    sim_kind == SIM_OPEN_DEBUG,
			    1/*LMA*/, sim_write);
  if (prog_bfd == NULL)
    return SIM_RC_FAIL;
  prog_bfd_was_opened_p = abfd == NULL;
  return SIM_RC_OK;
} 
