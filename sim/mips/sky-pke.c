/* Copyright (C) 1998, Cygnus Solutions */


#include "config.h"

#include <stdlib.h>
#include "sim-main.h"
#include "sim-bits.h"
#include "sim-assert.h"
#include "sky-pke.h"
#include "sky-dma.h"
#include "sky-vu.h"
#include "sky-gpuif.h"
#include "sky-device.h"


#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif


/* Internal function declarations */

static int pke_io_read_buffer(device*, void*, int, address_word,
			       unsigned, sim_cpu*, sim_cia);
static int pke_io_write_buffer(device*, const void*, int, address_word,
			       unsigned, sim_cpu*, sim_cia);
static void pke_reset(struct pke_device*);
static void pke_issue(SIM_DESC, struct pke_device*);
static void pke_pc_advance(struct pke_device*, int num_words);
static struct fifo_quadword* pke_pcrel_fifo(struct pke_device*, int operand_num, 
					    unsigned_4** operand);
static unsigned_4* pke_pcrel_operand(struct pke_device*, int operand_num);
static unsigned_4 pke_pcrel_operand_bits(struct pke_device*, int bit_offset,
					 int bit_width, unsigned_4* sourceaddr);
static void pke_attach(SIM_DESC sd, struct pke_device* me);
enum pke_check_target { chk_vu, chk_path1, chk_path2, chk_path3 };
static int pke_check_stall(struct pke_device* me, enum pke_check_target what);
static void pke_flip_dbf(struct pke_device* me);
static void pke_begin_interrupt_stall(struct pke_device* me);
/* PKEcode handlers */
static void pke_code_nop(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_stcycl(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_offset(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_base(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_itop(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_stmod(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_mskpath3(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_pkemark(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_flushe(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_flush(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_flusha(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_pkemscal(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_pkemscnt(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_pkemscalf(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_stmask(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_strow(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_stcol(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_mpg(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_direct(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_directhl(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_unpack(struct pke_device* me, unsigned_4 pkecode);
static void pke_code_error(struct pke_device* me, unsigned_4 pkecode);
unsigned_4 pke_fifo_flush(struct pke_fifo*);
void pke_fifo_reset(struct pke_fifo*);
struct fifo_quadword* pke_fifo_fit(struct pke_fifo*);
struct fifo_quadword* pke_fifo_access(struct pke_fifo*, unsigned_4 qwnum);
void pke_fifo_old(struct pke_fifo*, unsigned_4 qwnum);



/* Static data */

struct pke_device pke0_device = 
{ 
  { "vif0", &pke_io_read_buffer, &pke_io_write_buffer }, /* device */
  0, 0,        /* ID, flags */
  {},          /* regs */
  {}, 0,       /* FIFO write buffer */
  { NULL, 0, 0, 0 }, /* FIFO */
  NULL, NULL,   /* FIFO trace file descriptor and name */
  -1, -1, 0, 0, 0, /* invalid FIFO cache */
  0, 0,        /* pc */
  NULL, NULL   /* disassembly trace file descriptor and name  */
};


struct pke_device pke1_device = 
{ 
  { "vif1", &pke_io_read_buffer, &pke_io_write_buffer }, /* device */
  1, 0,        /* ID, flags */
  {},          /* regs */
  {}, 0,       /* FIFO write buffer */
  { NULL, 0, 0, 0 }, /* FIFO */
  NULL, NULL,  /* FIFO trace file descriptor and name  */
  -1, -1, 0, 0, 0, /* invalid FIFO cache */
  0, 0,        /* pc */
  NULL, NULL   /* disassembly trace file descriptor and name  */
};



/* External functions */


/* Attach PKE addresses to main memory */

void
pke0_attach(SIM_DESC sd) 
{
  pke_attach(sd, & pke0_device);
  pke_reset(& pke0_device);
}

void
pke1_attach(SIM_DESC sd) 
{
  pke_attach(sd, & pke1_device);
  pke_reset(& pke1_device);
}



/* Issue a PKE instruction if possible */

void 
pke0_issue(SIM_DESC sd) 
{
  pke_issue(sd, & pke0_device);
}

void 
pke1_issue(SIM_DESC sd) 
{
  pke_issue(sd, & pke1_device);
}



/* Internal functions */


/* Attach PKE memory regions to simulator */

void 
pke_attach(SIM_DESC sd, struct pke_device* me) 
{
  /* register file */
  sim_core_attach (sd, NULL, 0, access_read_write, 0,
		   (me->pke_number == 0) ? PKE0_REGISTER_WINDOW_START : PKE1_REGISTER_WINDOW_START,
                   PKE_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) me,
                   NULL /*buffer*/);

  /* FIFO port */
  sim_core_attach (sd, NULL, 0, access_read_write, 0,
		   (me->pke_number == 0) ? PKE0_FIFO_ADDR : PKE1_FIFO_ADDR,
                   sizeof(quadword) /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) me,
                   NULL /*buffer*/);

  /* VU MEM0 tracking table */
  sim_core_attach (sd, NULL, 0, access_read_write, 0,
		   ((me->pke_number == 0) ? VU0_MEM0_SRCADDR_START : VU1_MEM0_SRCADDR_START),
                   ((me->pke_number == 0) ? VU0_MEM0_SIZE : VU1_MEM0_SIZE) / 2,
                   0 /*modulo*/,
                   NULL,
                   NULL /*buffer*/);

  /* VU MEM1 tracking table */
  sim_core_attach (sd, NULL, 0, access_read_write, 0,
		   ((me->pke_number == 0) ? VU0_MEM1_SRCADDR_START : VU1_MEM1_SRCADDR_START),
                   ((me->pke_number == 0) ? VU0_MEM1_SIZE : VU1_MEM1_SIZE) / 4,
                   0 /*modulo*/,
                   NULL,
                   NULL /*buffer*/);
}


/* Read PKE Pseudo-PC index into buf in target order */
int
read_pke_pcx (struct pke_device *me, void *buf)
{
  *((int *) buf) = H2T_4( (me->fifo_pc << 2) | me->qw_pc );
  return 4;
}


/* Read PKE Pseudo-PC source address into buf in target order */
int
read_pke_pc (struct pke_device *me, void *buf)
{
  struct fifo_quadword* fqw = pke_fifo_access(& me->fifo, me->fifo_pc);
  unsigned_4 addr;

  if (fqw == NULL)
    *((int *) buf) = 0;
  else
    {
      addr = (fqw->source_address & ~15) | (me->qw_pc << 2);
      *((unsigned_4 *) buf) = H2T_4( addr );
    }

  return 4;
}


/* Read PKE reg into buf in target order */
int
read_pke_reg (struct pke_device *me, int reg_num, void *buf)
{
  /* handle reads to individual registers; clear `readable' on error */
  switch (reg_num)
    {
    /* handle common case of register reading, side-effect free */
    /* PKE1-only registers*/
    case PKE_REG_BASE:
    case PKE_REG_OFST:
    case PKE_REG_TOPS:
    case PKE_REG_TOP:
    case PKE_REG_DBF:
      if (me->pke_number == 0)
	{
	  *((int *) buf) = 0;
	  break;
	}
      /* fall through */

    /* PKE0 & PKE1 common registers*/
    case PKE_REG_STAT:
    case PKE_REG_ERR:
    case PKE_REG_MARK:
    case PKE_REG_CYCLE:
    case PKE_REG_MODE:
    case PKE_REG_NUM:
    case PKE_REG_MASK:
    case PKE_REG_CODE:
    case PKE_REG_ITOPS:
    case PKE_REG_ITOP:
    case PKE_REG_R0:
    case PKE_REG_R1:
    case PKE_REG_R2:
    case PKE_REG_R3:
    case PKE_REG_C0:
    case PKE_REG_C1:
    case PKE_REG_C2:
    case PKE_REG_C3:
      *((int *) buf) = H2T_4(me->regs[reg_num][0]);
      break;

    /* handle common case of write-only registers */
    case PKE_REG_FBRST:
      *((int *) buf) = 0;
      break;

    default:
      ASSERT(0); /* tests above should prevent this possibility */
    }

  return 4;
}


/* Handle a PKE read; return no. of bytes read */

int
pke_io_read_buffer(device *me_,
		   void *dest,
		   int space,
		   address_word addr,
		   unsigned nr_bytes,
		   sim_cpu *cpu,
		   sim_cia cia)
{
  /* downcast to gather embedding pke_device struct */
  struct pke_device* me = (struct pke_device*) me_;

  /* find my address ranges */
  address_word my_reg_start =
    (me->pke_number == 0) ? PKE0_REGISTER_WINDOW_START : PKE1_REGISTER_WINDOW_START;
  address_word my_fifo_addr =
    (me->pke_number == 0) ? PKE0_FIFO_ADDR : PKE1_FIFO_ADDR;

  /* enforce that an access does not span more than one quadword */
  address_word low = ADDR_TRUNC_QW(addr);
  address_word high = ADDR_TRUNC_QW(addr + nr_bytes - 1);
  if(low != high)
    return 0;

  /* classify address & handle */
  if((addr >= my_reg_start) && (addr < my_reg_start + PKE_REGISTER_WINDOW_SIZE))
    {
      /* register bank */
      int reg_num = ADDR_TRUNC_QW(addr - my_reg_start) >> 4;
      int reg_byte = ADDR_OFFSET_QW(addr);      /* find byte-offset inside register bank */
      quadword result;

      /* clear result */
      result[0] = result[1] = result[2] = result[3] = 0;

      read_pke_reg (me, reg_num, result);

      /* perform transfer & return */
      memcpy(dest, ((unsigned_1*) &result) + reg_byte, nr_bytes);

      return nr_bytes;
      /* NOTREACHED */
    }
  else if(addr >= my_fifo_addr &&
	  addr < my_fifo_addr + sizeof(quadword))
    {
      /* FIFO */

      /* FIFO is not readable: return a word of zeroes */
      memset(dest, 0, nr_bytes);
      return nr_bytes;
    }

  /* NOTREACHED */
  return 0;
}

/* Write PKE reg from buf, which is in target order */
int
write_pke_reg (struct pke_device *me, int reg_num, const void *buf)
{
  int writeable = 1;
  /* make words host-endian */
  unsigned_4 input = T2H_4( *((unsigned_4 *) buf) );

  /* handle writes to individual registers; clear `writeable' on error */
  switch (reg_num)
    {
    case PKE_REG_FBRST:
      /* Order these tests from least to most overriding, in case
	 multiple bits are set. */
      if(BIT_MASK_GET(input, PKE_REG_FBRST_STC_B, PKE_REG_FBRST_STC_E))
	{
	  /* clear a bunch of status bits */
	  PKE_REG_MASK_SET(me, STAT, PSS, 0);
	  PKE_REG_MASK_SET(me, STAT, PFS, 0);
	  PKE_REG_MASK_SET(me, STAT, PIS, 0);
	  PKE_REG_MASK_SET(me, STAT, INT, 0);
	  PKE_REG_MASK_SET(me, STAT, ER0, 0);
	  PKE_REG_MASK_SET(me, STAT, ER1, 0);
	  me->flags &= ~PKE_FLAG_PENDING_PSS;
	  /* will allow resumption of possible stalled instruction */
	}
      if(BIT_MASK_GET(input, PKE_REG_FBRST_STP_B, PKE_REG_FBRST_STP_E))
	{
	  me->flags |= PKE_FLAG_PENDING_PSS;
	}
      if(BIT_MASK_GET(input, PKE_REG_FBRST_FBK_B, PKE_REG_FBRST_FBK_E))
	{
	  PKE_REG_MASK_SET(me, STAT, PFS, 1);
	}
      if(BIT_MASK_GET(input, PKE_REG_FBRST_RST_B, PKE_REG_FBRST_RST_E))
	{
	  pke_reset(me);
	}
      break;
      
    case PKE_REG_ERR:
      /* copy bottom three bits */
      BIT_MASK_SET(me->regs[PKE_REG_ERR][0], 0, 2, BIT_MASK_GET(input, 0, 2));
      break;

    case PKE_REG_MARK:
      /* copy bottom sixteen bits */
      PKE_REG_MASK_SET(me, MARK, MARK, BIT_MASK_GET(input, 0, 15));
      /* reset MRK bit in STAT */
      PKE_REG_MASK_SET(me, STAT, MRK, 0);
      break;

      /* handle common case of read-only registers */
      /* PKE1-only registers - not really necessary to handle separately */
    case PKE_REG_BASE:
    case PKE_REG_OFST:
    case PKE_REG_TOPS:
    case PKE_REG_TOP:
    case PKE_REG_DBF:
      if(me->pke_number == 0)
	writeable = 0;
      /* fall through */
      /* PKE0 & PKE1 common registers*/
    case PKE_REG_STAT:
      /* ignore FDR bit for PKE1_STAT -- simulator does not implement PKE->RAM transfers */
    case PKE_REG_CYCLE:
    case PKE_REG_MODE:
    case PKE_REG_NUM:
    case PKE_REG_MASK:
    case PKE_REG_CODE:
    case PKE_REG_ITOPS:
    case PKE_REG_ITOP:
    case PKE_REG_R0:
    case PKE_REG_R1:
    case PKE_REG_R2:
    case PKE_REG_R3:
    case PKE_REG_C0:
    case PKE_REG_C1:
    case PKE_REG_C2:
    case PKE_REG_C3:
      writeable = 0;
      break;

    default:
      ASSERT(0); /* test above should prevent this possibility */
    }

  /* perform return */
  if(! writeable) 
    {
      return 0; /* error */
    } 

  return 4;
}


/* Handle a PKE write; return no. of bytes written */

int
pke_io_write_buffer(device *me_,
		    const void *src,
		    int space,
		    address_word addr,
		    unsigned nr_bytes,
		    sim_cpu *cpu,
		    sim_cia cia)
{ 
  /* downcast to gather embedding pke_device struct */
  struct pke_device* me = (struct pke_device*) me_;

  /* find my address ranges */
  address_word my_reg_start =
    (me->pke_number == 0) ? PKE0_REGISTER_WINDOW_START : PKE1_REGISTER_WINDOW_START;
  address_word my_fifo_addr =
    (me->pke_number == 0) ? PKE0_FIFO_ADDR : PKE1_FIFO_ADDR;

  /* enforce that an access does not span more than one quadword */
  address_word low = ADDR_TRUNC_QW(addr);
  address_word high = ADDR_TRUNC_QW(addr + nr_bytes - 1);
  if(low != high)
    return 0;

  /* classify address & handle */
  if((addr >= my_reg_start) && (addr < my_reg_start + PKE_REGISTER_WINDOW_SIZE))
    {
      /* register bank */
      int reg_num = ADDR_TRUNC_QW(addr - my_reg_start) >> 4;
      int reg_byte = ADDR_OFFSET_QW(addr);      /* find byte-offset inside register bank */
      quadword input;

      /* clear input */
      input[0] = input[1] = input[2] = input[3] = 0;

      /* write user-given bytes into input */
      memcpy(((unsigned_1*) &input) + reg_byte, src, nr_bytes);

      write_pke_reg (me, reg_num, input);
      return nr_bytes;

      /* NOTREACHED */
    }
  else if(addr >= my_fifo_addr &&
	  addr < my_fifo_addr + sizeof(quadword))
    {
      /* FIFO */
      struct fifo_quadword* fqw;
      int fifo_byte = ADDR_OFFSET_QW(addr);      /* find byte-offset inside fifo quadword */
      unsigned_4 dma_tag_present = 0;
      int i;

      /* collect potentially-partial quadword in write buffer; LE byte order */
      memcpy(((unsigned_1*)& me->fifo_qw_in_progress) + fifo_byte, src, nr_bytes);
      /* mark bytes written */
      for(i = fifo_byte; i < fifo_byte + nr_bytes; i++)
	BIT_MASK_SET(me->fifo_qw_done, i, i, 1);

      /* return if quadword not quite written yet */
      if(BIT_MASK_GET(me->fifo_qw_done, 0, sizeof(quadword)-1) !=
	 BIT_MASK_BTW(0, sizeof(quadword)-1))
	return nr_bytes;

      /* all done - process quadword after clearing flag */
      BIT_MASK_SET(me->fifo_qw_done, 0, sizeof(quadword)-1, 0);

      /* allocate required address in FIFO */
      fqw = pke_fifo_fit(& me->fifo);
      ASSERT(fqw != NULL);

      /* fill in unclassified FIFO quadword data in host byte order */
      fqw->word_class[0] = fqw->word_class[1] = 
	fqw->word_class[2] = fqw->word_class[3] = wc_unknown;
      fqw->data[0] = T2H_4(me->fifo_qw_in_progress[0]);
      fqw->data[1] = T2H_4(me->fifo_qw_in_progress[1]); 
      fqw->data[2] = T2H_4(me->fifo_qw_in_progress[2]); 
      fqw->data[3] = T2H_4(me->fifo_qw_in_progress[3]); 

      /* read DMAC-supplied indicators */
      ASSERT(sizeof(unsigned_4) == 4);
      PKE_MEM_READ(me, (me->pke_number == 0 ? DMA_D0_MADR : DMA_D1_MADR),
		   & fqw->source_address, /* converted to host-endian */
		   4);
      PKE_MEM_READ(me, (me->pke_number == 0 ? DMA_D0_PKTFLAG : DMA_D1_PKTFLAG),
		   & dma_tag_present,
		   4);

      if(dma_tag_present)
	{
	  /* lower two words are DMA tags */
	  fqw->word_class[0] = fqw->word_class[1] = wc_dma;
	}

      /* set FQC to "1" as FIFO is now not empty */ 
      PKE_REG_MASK_SET(me, STAT, FQC, 1);
      
      /* okay */
      return nr_bytes;
    }

  /* NOTREACHED */
  return 0;
}



/* Reset the PKE */
void
pke_reset(struct pke_device* me)
{
  /* advance PC over last quadword in FIFO; keep previous FIFO history  */
  me->fifo_pc = pke_fifo_flush(& me->fifo);
  me->qw_pc = 0;
  /* clear registers, flag, other state */
  memset(me->regs, 0, sizeof(me->regs));
  me->fifo_qw_done = 0;
  if ( me->trace_file != NULL ) 
    {
      fclose (me->trace_file);
      me->trace_file = NULL;
    }
  /* Command options will remain alive over the reset.  */
  me->flags &= PKE_FLAG_TRACE_ON;
}



/* Issue & swallow next PKE opcode if possible/available */

void
pke_issue(SIM_DESC sd, struct pke_device* me)
{
  struct fifo_quadword* fqw;
  unsigned_4 fw;
  unsigned_4 cmd, intr;

  /* 1 -- fetch PKE instruction */

  /* confirm availability of new quadword of PKE instructions */
  fqw = pke_fifo_access(& me->fifo, me->fifo_pc);
  if(fqw == NULL)
    return;

  /* skip over DMA tag, if present */
  pke_pc_advance(me, 0);
  /* note: this can only change qw_pc from 0 to 2 and will not
     invalidate fqw */

  /* "fetch" instruction quadword and word */ 
  fw = fqw->data[me->qw_pc];

  /* store word in PKECODE register */
  me->regs[PKE_REG_CODE][0] = fw;


  /* 2 -- test go / no-go for PKE execution */

  /* switch on STAT:PSS if PSS-pending and in idle state */
  if((PKE_REG_MASK_GET(me, STAT, PPS) == PKE_REG_STAT_PPS_IDLE) &&
     (me->flags & PKE_FLAG_PENDING_PSS) != 0)
    {
      me->flags &= ~PKE_FLAG_PENDING_PSS;
      PKE_REG_MASK_SET(me, STAT, PSS, 1);
    }

  /* check for stall/halt control bits */
  if(PKE_REG_MASK_GET(me, STAT, PFS) ||
     PKE_REG_MASK_GET(me, STAT, PSS) || /* note special treatment below */
     /* PEW bit not a reason to keep stalling - it's just an indication, re-computed below */
     /* PGW bit not a reason to keep stalling - it's just an indication, re-computed below */
     /* ER0/ER1 not a reason to keep stalling - it's just an indication */
     PKE_REG_MASK_GET(me, STAT, PIS))
    {
      /* (still) stalled */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
      /* try again next cycle */
      return;
    }


  /* 3 -- decode PKE instruction */

  /* decoding */
  if(PKE_REG_MASK_GET(me, STAT, PPS) == PKE_REG_STAT_PPS_IDLE)
    PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_DECODE);

  /* Extract relevant bits from PKEcode */
  intr = BIT_MASK_GET(fw, PKE_OPCODE_I_B,   PKE_OPCODE_I_E);
  cmd  = BIT_MASK_GET(fw, PKE_OPCODE_CMD_B, PKE_OPCODE_CMD_E);

  /* handle interrupts */
  if(intr)
    {
      /* are we resuming an interrupt-stalled instruction? */
      if(me->flags & PKE_FLAG_INT_NOLOOP)
	{
	  /* clear loop-prevention flag */
	  me->flags &= ~PKE_FLAG_INT_NOLOOP;

	  /* fall through to decode & execute */
	  /* The pke_code_* functions should not check the MSB in the
             pkecode. */
	}
      else /* new interrupt-flagged instruction */
	{
	  /* set INT flag in STAT register */
	  PKE_REG_MASK_SET(me, STAT, INT, 1);
	  /* set loop-prevention flag */
	  me->flags |= PKE_FLAG_INT_NOLOOP;

	  /* set PIS if stall not masked */
	  if(!PKE_REG_MASK_GET(me, ERR, MII))
	    pke_begin_interrupt_stall(me);

	  /* suspend this instruction unless it's PKEMARK */
	  if(!IS_PKE_CMD(cmd, PKEMARK))
	    {
	      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
	      return;
	    }
	  else
	    {
	      ; /* fall through to decode & execute */
	    }
	}
    }


  /* decode & execute */
  if(IS_PKE_CMD(cmd, PKENOP))
    pke_code_nop(me, fw);
  else if(IS_PKE_CMD(cmd, STCYCL))
    pke_code_stcycl(me, fw);
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, OFFSET))
    pke_code_offset(me, fw);
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, BASE))
    pke_code_base(me, fw);
  else if(IS_PKE_CMD(cmd, ITOP))
    pke_code_itop(me, fw);
  else if(IS_PKE_CMD(cmd, STMOD))
    pke_code_stmod(me, fw);
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, MSKPATH3))
    pke_code_mskpath3(me, fw);
  else if(IS_PKE_CMD(cmd, PKEMARK))
    pke_code_pkemark(me, fw);
  else if(IS_PKE_CMD(cmd, FLUSHE))
    pke_code_flushe(me, fw);
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, FLUSH))
    pke_code_flush(me, fw);
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, FLUSHA))
    pke_code_flusha(me, fw);
  else if(IS_PKE_CMD(cmd, PKEMSCAL))
    pke_code_pkemscal(me, fw);
  else if(IS_PKE_CMD(cmd, PKEMSCNT))
    pke_code_pkemscnt(me, fw);
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, PKEMSCALF))
    pke_code_pkemscalf(me, fw);
  else if(IS_PKE_CMD(cmd, STMASK))
    pke_code_stmask(me, fw);
  else if(IS_PKE_CMD(cmd, STROW))
    pke_code_strow(me, fw);
  else if(IS_PKE_CMD(cmd, STCOL))
    pke_code_stcol(me, fw);
  else if(IS_PKE_CMD(cmd, MPG))
    pke_code_mpg(me, fw);
  else if(IS_PKE_CMD(cmd, DIRECT))
    pke_code_direct(me, fw);
  else if(IS_PKE_CMD(cmd, DIRECTHL))
    pke_code_directhl(me, fw);
  else if(IS_PKE_CMD(cmd, UNPACK))
    pke_code_unpack(me, fw);
  else if(cmd == TXVU_VIF_BRK_MASK)
    {
      sim_cpu *cpu = STATE_CPU (sd, 0);
      unsigned_4 pc_addr = (fqw->source_address & ~15) | (me->qw_pc << 2);
	      
      sim_engine_halt (sd, cpu, NULL, pc_addr, sim_stopped, SIM_SIGTRAP);
    }
  /* ... no other commands ... */
  else
    pke_code_error(me, fw);
}



/* Clear out contents of FIFO; act as if it was empty.  Return PC
   pointing to one-past-last word. */

unsigned_4
pke_fifo_flush(struct pke_fifo* fifo)
{
  /* don't modify any state! */
  return fifo->origin + fifo->next;
}



/* Clear out contents of FIFO; make it really empty. */

void
pke_fifo_reset(struct pke_fifo* fifo)
{
  int i;

  /* clear fifo quadwords */
  for(i=0; i<fifo->next; i++)
    {
      zfree(fifo->quadwords[i]);
      fifo->quadwords[i] = NULL;
    }

  /* reset pointers */
  fifo->origin = 0;
  fifo->next = 0;
}



/* Make space for the next quadword in the FIFO.  Allocate/enlarge
   FIFO pointer block if necessary.  Return a pointer to it. */

struct fifo_quadword*
pke_fifo_fit(struct pke_fifo* fifo)
{
  struct fifo_quadword* fqw;

  /* out of space on quadword pointer array? */
  if(fifo->next == fifo->length) /* also triggered before fifo->quadwords allocated */
    {
      struct fifo_quadword** new_qw;
      unsigned_4 new_length = fifo->length + PKE_FIFO_GROW_SIZE;

      /* allocate new pointer block */
      new_qw = zalloc(new_length * sizeof(struct fifo_quadword*));
      ASSERT(new_qw != NULL);

      /* copy over old contents, if any */
      if(fifo->quadwords != NULL)
	{
	  /* copy over old pointers to beginning of new block */
	  memcpy(new_qw, fifo->quadwords,
		 fifo->length * sizeof(struct fifo_quadword*));
	  
	  /* free old block */
	  zfree(fifo->quadwords);
	}

      /* replace pointers & counts */
      fifo->quadwords = new_qw;
      fifo->length = new_length;
    }

  /* sanity check */
  ASSERT(fifo->quadwords != NULL);

  /* allocate new quadword from heap */
  fqw = zalloc(sizeof(struct fifo_quadword));
  ASSERT(fqw != NULL);

  /* push quadword onto fifo */
  fifo->quadwords[fifo->next] = fqw;
  fifo->next++;
  return fqw;
}



/* Return a pointer to the FIFO quadword with given absolute index, or
   NULL if it is out of range */

struct fifo_quadword*
pke_fifo_access(struct pke_fifo* fifo, unsigned_4 qwnum)
{
  struct fifo_quadword* fqw;

  if((qwnum < fifo->origin) ||  /* before history */
     (qwnum >= fifo->origin + fifo->next)) /* after last available quadword */
    fqw = NULL;
  else
    {
      ASSERT(fifo->quadwords != NULL); /* must be allocated already */
      fqw = fifo->quadwords[qwnum - fifo->origin]; /* pull out pointer from array */
      ASSERT(fqw != NULL); /* must be allocated already */
    }

  return fqw;
}


/* Authorize release of any FIFO entries older than given absolute quadword. */
void
pke_fifo_old(struct pke_fifo* fifo, unsigned_4 qwnum)
{
  /* do we have any too-old FIFO elements? */
  if(fifo->origin + PKE_FIFO_ARCHEOLOGY < qwnum)
    {
      /* count quadwords to forget */
      int horizon = qwnum - (fifo->origin + PKE_FIFO_ARCHEOLOGY); 
      int i;

      /* free quadwords at indices below horizon */
      for(i=0; i < horizon; i++)
	zfree(fifo->quadwords[i]);

      /* move surviving quadword pointers down to beginning of array */
      for(i=horizon; i < fifo->next; i++)
	fifo->quadwords[i-horizon] = fifo->quadwords[i];

      /* clear duplicate pointers */
      for(i=fifo->next - horizon; i < fifo->next; i++)
	fifo->quadwords[i] = NULL;

      /* adjust FIFO pointers */
      fifo->origin = fifo->origin + horizon;
      fifo->next = fifo->next - horizon;
    }
}




/* advance the PC by given number of data words; update STAT/FQC
   field; assume FIFO is filled enough; classify passed-over words;
   write FIFO trace line */

void
pke_pc_advance(struct pke_device* me, int num_words)
{
  int num = num_words;
  struct fifo_quadword* fq = NULL;
  unsigned_4 old_fifo_pc = me->fifo_pc;

  ASSERT(num_words >= 0);

  /* printf("pke %d pc_advance num_words %d\n", me->pke_number, num_words); */

  while(1)
    {
      /* find next quadword, if any */
      fq = pke_fifo_access(& me->fifo, me->fifo_pc);

      /* skip over DMA tag words if present in word 0 or 1 */
      if(fq != NULL && fq->word_class[me->qw_pc] == wc_dma)
	{
	  /* skip by going around loop an extra time */
	  num ++;
	}
      
      /* nothing left to skip / no DMA tag here */
      if(num == 0)
	break;

      /* we are supposed to skip existing words */
      ASSERT(fq != NULL);

      /* one word skipped */
      num --;
      
      /* point to next word */
      me->qw_pc ++;
      if(me->qw_pc == 4)
	{
	  me->qw_pc = 0;
	  me->fifo_pc ++;
	  
	  /* trace the consumption of the FIFO quadword we just skipped over */
	  /* fq still points to it */
	  if ( indebug (me->dev.name)) 
       {
         if (( me->fifo_trace_file == NULL) &&
             ( me->fifo_trace_file_name != NULL ))
           sky_open_file (&me->fifo_trace_file, me->fifo_trace_file_name,
                         (char *) NULL);

         /* assert complete classification */
	      ASSERT(fq->word_class[3] != wc_unknown);
	      ASSERT(fq->word_class[2] != wc_unknown);
	      ASSERT(fq->word_class[1] != wc_unknown);
	      ASSERT(fq->word_class[0] != wc_unknown);
	      
	      /* print trace record */
         fprintf((me->fifo_trace_file != NULL) ? me->fifo_trace_file : stdout,
		        "%d 0x%08x_%08x_%08x_%08x 0x%08x %c%c%c%c\n",
		        (me->pke_number == 0 ? 0 : 1),
		        (unsigned) fq->data[3], (unsigned) fq->data[2],
		        (unsigned) fq->data[1], (unsigned) fq->data[0],
		        (unsigned) fq->source_address,
		        fq->word_class[3], fq->word_class[2],
		        fq->word_class[1], fq->word_class[0]);
       }
	} /* next quadword */
    }

  /* age old entries before PC */
  if(me->fifo_pc != old_fifo_pc) 
    {
      /* we advanced the fifo-pc; authorize disposal of anything
         before previous PKEcode */
      pke_fifo_old(& me->fifo, old_fifo_pc);
    }

  /* clear FQC if FIFO is now empty */ 
  fq = pke_fifo_access(& me->fifo, me->fifo_pc);
  if(fq == NULL)
    {
      PKE_REG_MASK_SET(me, STAT, FQC, 0);
    }
  else /* annote the word where the PC lands as an PKEcode */
    {
      ASSERT(fq->word_class[me->qw_pc] == wc_pkecode || fq->word_class[me->qw_pc] == wc_unknown);
      fq->word_class[me->qw_pc] = wc_pkecode;
    }
}





/* Return pointer to FIFO quadword containing given operand# in FIFO.
   `operand_num' starts at 1.  Return pointer to operand word in last
   argument, if non-NULL.  If FIFO is not full enough, return 0.
   Signal an ER0 indication upon skipping a DMA tag.  */

struct fifo_quadword*
pke_pcrel_fifo(struct pke_device* me, int operand_num, unsigned_4** operand)
{
  int num;
  int new_qw_pc, new_fifo_pc;
  struct fifo_quadword* fq = NULL;

  /* check for validity of last search results in cache */
  if(me->last_fifo_pc == me->fifo_pc &&
     me->last_qw_pc == me->qw_pc &&
     operand_num > me->last_num)
    {
      /* continue search from last stop */
      new_fifo_pc = me->last_new_fifo_pc;
      new_qw_pc = me->last_new_qw_pc;
      num = operand_num - me->last_num;
    }
  else
    {
      /* start search from scratch */
      new_fifo_pc = me->fifo_pc;
      new_qw_pc = me->qw_pc;
      num = operand_num;
    }

  ASSERT(num > 0);

  /* printf("pke %d pcrel_fifo operand_num %d\n", me->pke_number, operand_num); */

  do
    {
      /* one word skipped */
      num --;

      /* point to next word */
      new_qw_pc ++;
      if(new_qw_pc == 4)
	{
	  new_qw_pc = 0;
	  new_fifo_pc ++;
	}

      fq = pke_fifo_access(& me->fifo, new_fifo_pc);

      /* check for FIFO underflow */
      if(fq == NULL)
	break;

      /* skip over DMA tag words if present in word 0 or 1 */
      if(fq->word_class[new_qw_pc] == wc_dma)
	{
	  /* set ER0 */
	  PKE_REG_MASK_SET(me, STAT, ER0, 1);

	  /* mismatch error! */
	  if(! PKE_REG_MASK_GET(me, ERR, ME0))
	    {
	      pke_begin_interrupt_stall(me);
	      /* don't stall just yet -- finish this instruction */
	      /* the PPS_STALL state will be entered by pke_issue() next time */
	    }
	  /* skip by going around loop an extra time */
	  num ++;
	}
    }
  while(num > 0);

  /* return pointer to operand word itself */
  if(fq != NULL)
    {
      *operand = & fq->data[new_qw_pc];

      /* annote the word where the pseudo-PC lands as an PKE operand */
      ASSERT(fq->word_class[new_qw_pc] == wc_pkedata || fq->word_class[new_qw_pc] == wc_unknown);
      fq->word_class[new_qw_pc] = wc_pkedata;

      /* store search results in cache */
      /* keys */
      me->last_fifo_pc = me->fifo_pc;
      me->last_qw_pc = me->qw_pc;
      /* values */
      me->last_num = operand_num;
      me->last_new_fifo_pc = new_fifo_pc;
      me->last_new_qw_pc = new_qw_pc;
    }

  return fq;
}


/* Return pointer to given operand# in FIFO.  `operand_num' starts at 1.
   If FIFO is not full enough, return 0.  Skip over DMA tags, but mark
   them as an error (ER0). */

unsigned_4*
pke_pcrel_operand(struct pke_device* me, int operand_num)
{
  unsigned_4* operand = NULL;
  struct fifo_quadword* fifo_operand;

  fifo_operand = pke_pcrel_fifo(me, operand_num, & operand);

  if(fifo_operand == NULL)
    ASSERT(operand == NULL); /* pke_pcrel_fifo() ought leave it untouched */

  return operand;
}


/* Return a bit-field extract of given operand# in FIFO, and its
   word-accurate source-addr.  `bit_offset' starts at 0, referring to
   LSB after PKE instruction word.  Width must be >0, <=32.  Assume
   FIFO is full enough.  Skip over DMA tags, but mark them as an error
   (ER0).  */

unsigned_4
pke_pcrel_operand_bits(struct pke_device* me, int bit_offset, int bit_width, unsigned_4* source_addr)
{
  unsigned_4* word = NULL;
  unsigned_4 value;
  struct fifo_quadword* fifo_operand;
  int wordnumber, bitnumber;
  int i;

  wordnumber = bit_offset/32;
  bitnumber = bit_offset%32;

  /* find operand word with bitfield */
  fifo_operand = pke_pcrel_fifo(me, wordnumber + 1, &word);
  ASSERT(word != NULL);

  /* extract bitfield from word */
  value = BIT_MASK_GET(*word, bitnumber, bitnumber + bit_width - 1);

  /* extract source addr from fifo word */
  *source_addr = fifo_operand->source_address;

  /* add word offset */
  for(i=0; i<3; i++)
    if(word == & fifo_operand->data[i])
      *source_addr += sizeof(unsigned_4) * i;

  return value;
}



/* check for stall conditions on indicated devices (path* only on
   PKE1), do not change status; return 0 iff no stall */
int
pke_check_stall(struct pke_device* me, enum pke_check_target what)
{
  int any_stall = 0;
  unsigned_4 cop2_stat, gpuif_stat;

  /* read status words */
  ASSERT(sizeof(unsigned_4) == 4);
  PKE_MEM_READ(me, (GIF_REG_STAT),
	       & gpuif_stat,
	       4);
  PKE_MEM_READ(me, (COP2_REG_STAT_ADDR),
	       & cop2_stat,
	       4);

  /* perform checks */
  if(what == chk_vu)
    {
      if(me->pke_number == 0)
	any_stall = BIT_MASK_GET(cop2_stat, COP2_REG_STAT_VBS0_B, COP2_REG_STAT_VBS0_E);
      else /* if(me->pke_number == 1) */
	any_stall = BIT_MASK_GET(cop2_stat, COP2_REG_STAT_VBS1_B, COP2_REG_STAT_VBS1_E);
    }
  else if(what == chk_path1) /* VU -> GPUIF */
    {
      ASSERT(me->pke_number == 1);
      if(BIT_MASK_GET(gpuif_stat, GPUIF_REG_STAT_APATH_B, GPUIF_REG_STAT_APATH_E) == 1)
	any_stall = 1;
    }
  else if(what == chk_path2) /* PKE -> GPUIF */
    {
      ASSERT(me->pke_number == 1);
      if(BIT_MASK_GET(gpuif_stat, GPUIF_REG_STAT_APATH_B, GPUIF_REG_STAT_APATH_E) == 2)
	any_stall = 1;
    }
  else if(what == chk_path3) /* DMA -> GPUIF */
    {
      ASSERT(me->pke_number == 1);
      if(BIT_MASK_GET(gpuif_stat, GPUIF_REG_STAT_APATH_B, GPUIF_REG_STAT_APATH_E) == 3)
	any_stall = 1;
    }
  else
    {
      /* invalid what */
      ASSERT(0);
    }

  /* any stall reasons? */
  return any_stall;
}


/* PKE1 only: flip the DBF bit; recompute TOPS, TOP */
void
pke_flip_dbf(struct pke_device* me)
{
  int newdf;
  /* compute new TOP */
  PKE_REG_MASK_SET(me, TOP, TOP,
		   PKE_REG_MASK_GET(me, TOPS, TOPS));
  /* flip DBF */
  newdf = PKE_REG_MASK_GET(me, DBF, DF) ? 0 : 1;
  PKE_REG_MASK_SET(me, DBF, DF, newdf);
  PKE_REG_MASK_SET(me, STAT, DBF, newdf);
  /* compute new TOPS */
  PKE_REG_MASK_SET(me, TOPS, TOPS,
		   (PKE_REG_MASK_GET(me, BASE, BASE) +
		    newdf * PKE_REG_MASK_GET(me, OFST, OFFSET)));

  /* this is equivalent to last word from okadaa (98-02-25):
     1) TOP=TOPS;
     2) TOPS=BASE + !DBF*OFFSET
     3) DBF=!DBF */
}


/* set the STAT:PIS bit and send an interrupt to the 5900 */
void
pke_begin_interrupt_stall(struct pke_device* me)
{
  /* set PIS */
  PKE_REG_MASK_SET(me, STAT, PIS, 1);
  sky_signal_interrupt();
}




/* PKEcode handler functions -- responsible for checking and
   confirming old stall conditions, executing pkecode, updating PC and
   status registers -- may assume being run on correct PKE unit */
   
void 
pke_code_nop(struct pke_device* me, unsigned_4 pkecode)
{
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_stcycl(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  /* copy immediate value into CYCLE reg */
  PKE_REG_MASK_SET(me, CYCLE, WL, BIT_MASK_GET(imm, 8, 15));
  PKE_REG_MASK_SET(me, CYCLE, CL, BIT_MASK_GET(imm, 0, 7));
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_offset(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  /* copy 10 bits to OFFSET field */
  PKE_REG_MASK_SET(me, OFST, OFFSET, BIT_MASK_GET(imm, 0, 9));
  /* clear DBF bit */
  PKE_REG_MASK_SET(me, DBF, DF, 0);
  /* clear other DBF bit */
  PKE_REG_MASK_SET(me, STAT, DBF, 0);
  /* set TOPS = BASE */
  PKE_REG_MASK_SET(me, TOPS, TOPS, PKE_REG_MASK_GET(me, BASE, BASE));
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_base(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  /* copy 10 bits to BASE field */
  PKE_REG_MASK_SET(me, BASE, BASE, BIT_MASK_GET(imm, 0, 9));
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_itop(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  /* copy 10 bits to ITOPS field */
  PKE_REG_MASK_SET(me, ITOPS, ITOPS, BIT_MASK_GET(imm, 0, 9));
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_stmod(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  /* copy 2 bits to MODE register */
  PKE_REG_MASK_SET(me, MODE, MDE, BIT_MASK_GET(imm, 0, 2));
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_mskpath3(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);
  unsigned_4 gif_mode;

  /* set appropriate bit */
  if(BIT_MASK_GET(imm, PKE_REG_MSKPATH3_B, PKE_REG_MSKPATH3_E) != 0)
    gif_mode = GIF_REG_STAT_M3P;
  else
    gif_mode = 0;

  /* write register to "read-only" register; gpuif code will look at M3P bit only */
  PKE_MEM_WRITE(me, GIF_REG_VIF_M3P, & gif_mode, 4);

  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_pkemark(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);
  /* copy 16 bits to MARK register */
  PKE_REG_MASK_SET(me, MARK, MARK, BIT_MASK_GET(imm, 0, 15));
  /* set MRK bit in STAT register - CPU2 v2.1 docs incorrect */
  PKE_REG_MASK_SET(me, STAT, MRK, 1);
  /* done */
  pke_pc_advance(me, 1);
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
}


void
pke_code_flushe(struct pke_device* me, unsigned_4 pkecode)
{
  /* compute next PEW bit */
  if(pke_check_stall(me, chk_vu))
    {
      /* VU busy */
      PKE_REG_MASK_SET(me, STAT, PEW, 1);
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
      /* try again next cycle */
    }
  else
    {
      /* VU idle */
      PKE_REG_MASK_SET(me, STAT, PEW, 0);
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1);
    }
}


void
pke_code_flush(struct pke_device* me, unsigned_4 pkecode)
{
  int something_busy = 0;

  /* compute next PEW, PGW bits */
  if(pke_check_stall(me, chk_vu))
    {
      something_busy = 1;
      PKE_REG_MASK_SET(me, STAT, PEW, 1);
    }
  else
    PKE_REG_MASK_SET(me, STAT, PEW, 0);


  if(pke_check_stall(me, chk_path1) ||
     pke_check_stall(me, chk_path2))
    {
      something_busy = 1;
      PKE_REG_MASK_SET(me, STAT, PGW, 1);
    }
  else
    PKE_REG_MASK_SET(me, STAT, PGW, 0);

  /* go or no go */
  if(something_busy)
    {
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* try again next cycle */
    }
  else
    {
      /* all idle */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1);
    }
}


void
pke_code_flusha(struct pke_device* me, unsigned_4 pkecode)
{
  int something_busy = 0;

  /* compute next PEW, PGW bits */
  if(pke_check_stall(me, chk_vu))
    {
      something_busy = 1;
      PKE_REG_MASK_SET(me, STAT, PEW, 1);
    }
  else
    PKE_REG_MASK_SET(me, STAT, PEW, 0);


  if(pke_check_stall(me, chk_path1) ||
     pke_check_stall(me, chk_path2) ||
     pke_check_stall(me, chk_path3))
    {
      something_busy = 1;
      PKE_REG_MASK_SET(me, STAT, PGW, 1);
    }
  else
    PKE_REG_MASK_SET(me, STAT, PGW, 0);

  if(something_busy)
    {
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* try again next cycle */
    }
  else
    {
      /* all idle */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1);
    }
}


void
pke_code_pkemscal(struct pke_device* me, unsigned_4 pkecode)
{
  /* compute next PEW bit */
  if(pke_check_stall(me, chk_vu))
    {
      /* VU busy */
      PKE_REG_MASK_SET(me, STAT, PEW, 1);
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
      /* try again next cycle */
    }
  else
    {
      unsigned_4 vu_pc;
      int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

      /* VU idle */
      PKE_REG_MASK_SET(me, STAT, PEW, 0);

      /* flip DBF on PKE1 */
      if(me->pke_number == 1)
	pke_flip_dbf(me);

      /* compute new PC for VU (host byte-order) */
      vu_pc = BIT_MASK_GET(imm, 0, 15);
      vu_pc = T2H_4(vu_pc);

      /* write new PC; callback function gets VU running */
      ASSERT(sizeof(unsigned_4) == 4);
      PKE_MEM_WRITE(me, (me->pke_number == 0 ? VU0_CIA : VU1_CIA),
		    & vu_pc,
		    4);

      /* copy ITOPS field to ITOP */
      PKE_REG_MASK_SET(me, ITOP, ITOP, PKE_REG_MASK_GET(me, ITOPS, ITOPS));

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1);
    }
}



void
pke_code_pkemscnt(struct pke_device* me, unsigned_4 pkecode)
{
  /* compute next PEW bit */
  if(pke_check_stall(me, chk_vu))
    {
      /* VU busy */
      PKE_REG_MASK_SET(me, STAT, PEW, 1);
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
      /* try again next cycle */
    }
  else
    {
      unsigned_4 vu_pc;

      /* VU idle */
      PKE_REG_MASK_SET(me, STAT, PEW, 0);

      /* flip DBF on PKE1 */
      if(me->pke_number == 1)
	pke_flip_dbf(me);

      /* read old PC */
      ASSERT(sizeof(unsigned_4) == 4);
      PKE_MEM_READ(me, (me->pke_number == 0 ? VU0_CIA : VU1_CIA),
		   & vu_pc,
		   4);

      /* rewrite new PC; callback function gets VU running */
      ASSERT(sizeof(unsigned_4) == 4);
      PKE_MEM_WRITE(me, (me->pke_number == 0 ? VU0_CIA : VU1_CIA),
		    & vu_pc,
		    4);

      /* copy ITOPS field to ITOP */
      PKE_REG_MASK_SET(me, ITOP, ITOP, PKE_REG_MASK_GET(me, ITOPS, ITOPS));

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1);
    }
}


void
pke_code_pkemscalf(struct pke_device* me, unsigned_4 pkecode)
{
  int something_busy = 0;

  /* compute next PEW, PGW bits */
  if(pke_check_stall(me, chk_vu))
    {
      something_busy = 1;
      PKE_REG_MASK_SET(me, STAT, PEW, 1);
    }
  else
    PKE_REG_MASK_SET(me, STAT, PEW, 0);


  if(pke_check_stall(me, chk_path1) ||
     pke_check_stall(me, chk_path2) ||
     pke_check_stall(me, chk_path3))
    {
      something_busy = 1;
      PKE_REG_MASK_SET(me, STAT, PGW, 1);
    }
  else
    PKE_REG_MASK_SET(me, STAT, PGW, 0);

  /* go or no go */
  if(something_busy)
    {
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* try again next cycle */
    }
  else
    {
      unsigned_4 vu_pc;
      int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);
      
      /* flip DBF on PKE1 */
      if(me->pke_number == 1)
	pke_flip_dbf(me);

      /* compute new PC for VU (host byte-order) */
      vu_pc = BIT_MASK_GET(imm, 0, 15);
      vu_pc = T2H_4(vu_pc);

      /* rewrite new PC; callback function gets VU running */
      ASSERT(sizeof(unsigned_4) == 4);
      PKE_MEM_WRITE(me, (me->pke_number == 0 ? VU0_CIA : VU1_CIA),
		    & vu_pc,
		    4);

      /* copy ITOPS field to ITOP */
      PKE_REG_MASK_SET(me, ITOP, ITOP, PKE_REG_MASK_GET(me, ITOPS, ITOPS));

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1);
    }
}


void
pke_code_stmask(struct pke_device* me, unsigned_4 pkecode)
{
  unsigned_4* mask;

  /* check that FIFO has one more word for STMASK operand */
  mask = pke_pcrel_operand(me, 1);
  if(mask != NULL)
    {
      /* "transferring" operand */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);

      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, 1);

      /* fill the register */
      PKE_REG_MASK_SET(me, MASK, MASK, *mask);

      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, 0);

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 2);
    }
  else
    {
      /* need to wait for another word */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* try again next cycle */
    }
}


void
pke_code_strow(struct pke_device* me, unsigned_4 pkecode)
{
  /* check that FIFO has four more words for STROW operand */
  unsigned_4* last_op;
  
  last_op = pke_pcrel_operand(me, 4);
  if(last_op != NULL)
    {
      /* "transferring" operand */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);
      
      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, 1);

      /* copy ROW registers: must all exist if 4th operand exists */
      me->regs[PKE_REG_R0][0] = * pke_pcrel_operand(me, 1);
      me->regs[PKE_REG_R1][0] = * pke_pcrel_operand(me, 2);
      me->regs[PKE_REG_R2][0] = * pke_pcrel_operand(me, 3);
      me->regs[PKE_REG_R3][0] = * pke_pcrel_operand(me, 4);
      
      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, 0);

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 5);
    }
  else
    {
      /* need to wait for another word */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* try again next cycle */
    }
}


void
pke_code_stcol(struct pke_device* me, unsigned_4 pkecode)
{
  /* check that FIFO has four more words for STCOL operand */
  unsigned_4* last_op;
  
  last_op = pke_pcrel_operand(me, 4);
  if(last_op != NULL)
    {
      /* "transferring" operand */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);
      
      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, 1);

      /* copy COL registers: must all exist if 4th operand exists */
      me->regs[PKE_REG_C0][0] = * pke_pcrel_operand(me, 1);
      me->regs[PKE_REG_C1][0] = * pke_pcrel_operand(me, 2);
      me->regs[PKE_REG_C2][0] = * pke_pcrel_operand(me, 3);
      me->regs[PKE_REG_C3][0] = * pke_pcrel_operand(me, 4);
      
      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, 0);

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 5);
    }
  else
    {
      /* need to wait for another word */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* try again next cycle */
    }
}


void
pke_code_mpg(struct pke_device* me, unsigned_4 pkecode)
{
  unsigned_4* last_mpg_word;
  int num = BIT_MASK_GET(pkecode, PKE_OPCODE_NUM_B, PKE_OPCODE_NUM_E);
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  /* assert 64-bit alignment of MPG operand */
  if(me->qw_pc != 3 && me->qw_pc != 1)
    return pke_code_error(me, pkecode);

  /* map zero to max+1 */
  if(num==0) num=0x100;
  
  /* check that FIFO has a few more words for MPG operand */
  last_mpg_word = pke_pcrel_operand(me, num*2); /* num: number of 64-bit words */
  if(last_mpg_word != NULL)
    {
      /* perform implied FLUSHE */
      if(pke_check_stall(me, chk_vu))
	{
	  /* VU busy */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
	  /* retry this instruction next clock */
	}
      else
	{
	  /* VU idle */
	  int i;
	  
	  /* "transferring" operand */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);
	  
	  /* set NUM */
	  PKE_REG_MASK_SET(me, NUM, NUM, num);

	  /* transfer VU instructions, one word-pair per iteration */
	  for(i=0; i<num; i++)
	    {
	      address_word vu_addr_base, vu_addr;
	      address_word vutrack_addr_base, vutrack_addr;
	      address_word vu_addr_max_size;
	      unsigned_4 vu_lower_opcode, vu_upper_opcode;
	      unsigned_4* operand;
	      unsigned_4 source_addr;
	      struct fifo_quadword* fq;
	      int next_num;
	      int j;

	      /* decrement NUM */
	      next_num = PKE_REG_MASK_GET(me, NUM, NUM) - 1;
	      PKE_REG_MASK_SET(me, NUM, NUM, next_num);
	      
	      /* imm: in 64-bit units for MPG instruction */
	      /* VU*_MEM0 : instruction memory */
	      vu_addr_base = (me->pke_number == 0) ?
		VU0_MEM0_WINDOW_START : VU1_MEM0_WINDOW_START;
	      vu_addr_max_size = (me->pke_number == 0) ?
		VU0_MEM0_SIZE : VU1_MEM0_SIZE;
	      vutrack_addr_base = (me->pke_number == 0) ?
		VU0_MEM0_SRCADDR_START : VU1_MEM0_SRCADDR_START;

	      /* compute VU address for this word-pair */
	      vu_addr = vu_addr_base + (imm + i) * 8;
	      /* check for vu_addr overflow */
	      while(vu_addr >= vu_addr_base + vu_addr_max_size)
		vu_addr -= vu_addr_max_size;

	      /* compute VU tracking address */
	      vutrack_addr = vutrack_addr_base + ((signed_8)vu_addr - (signed_8)vu_addr_base) / 2;

	      /* Fetch operand words; assume they are already little-endian for VU imem */
	      fq = pke_pcrel_fifo(me, i*2 + 1, & operand);
	      vu_lower_opcode = *operand;

	      source_addr = fq->source_address;
	      /* add word offset */
	      for(j=0; j<3; j++)
		if(operand == & fq->data[j])
		  source_addr += sizeof(unsigned_4) * j;

	      fq = pke_pcrel_fifo(me, i*2 + 2, & operand);
	      vu_upper_opcode = *operand;
	      
	      /* write data into VU memory */
	      /* lower (scalar) opcode comes in first word ; macro performs H2T! */
	      PKE_MEM_WRITE(me, vu_addr,
			    & vu_lower_opcode,
			    4);
	      /* upper (vector) opcode comes in second word ; H2T */
	      ASSERT(sizeof(unsigned_4) == 4);
	      PKE_MEM_WRITE(me, vu_addr + 4,
			    & vu_upper_opcode,
			    4);
	      
	      /* write tracking address in target byte-order */
	      ASSERT(sizeof(unsigned_4) == 4);
	      PKE_MEM_WRITE(me, vutrack_addr,
			    & source_addr,
			    4);
	    } /* VU xfer loop */

	  /* check NUM */
	  ASSERT(PKE_REG_MASK_GET(me, NUM, NUM) == 0);
	  
	  /* done */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
	  pke_pc_advance(me, 1 + num*2);
	}
    } /* if FIFO full enough */
  else
    {
      /* need to wait for another word */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* retry this instruction next clock */
    }
}


void
pke_code_direct(struct pke_device* me, unsigned_4 pkecode)
{
  /* check that FIFO has a few more words for DIRECT operand */
  unsigned_4* last_direct_word;
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);
  
  /* assert 128-bit alignment of DIRECT operand */
  if(me->qw_pc != 3)
    return pke_code_error(me, pkecode);

  /* map zero to max+1 */
  if(imm==0) imm=0x10000;
  
  last_direct_word = pke_pcrel_operand(me, imm*4); /* imm: number of 128-bit words */
  if(last_direct_word != NULL)
    {
      /* VU idle */
      int i;
      unsigned_16 fifo_data;
      
      /* "transferring" operand */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);
      
      /* transfer GPUIF quadwords, one word per iteration */
      for(i=0; i<imm*4; i++)
	{
	  unsigned_4* operand = pke_pcrel_operand(me, 1+i);
	  
	  /* collect word into quadword */
	  *A4_16(&fifo_data, 3 - (i % 4)) = *operand;

	  /* write to GPUIF FIFO only with full quadword */
	  if(i % 4 == 3)
	    {
	      ASSERT(sizeof(fifo_data) == 16);
	      PKE_MEM_WRITE(me, GIF_PATH2_FIFO_ADDR,
			    & fifo_data,
			    16);
	    } /* write collected quadword */
	} /* GPUIF xfer loop */
      
      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1 + imm*4);
    } /* if FIFO full enough */
  else
    {
      /* need to wait for another word */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* retry this instruction next clock */
    }
}


void
pke_code_directhl(struct pke_device* me, unsigned_4 pkecode)
{
  /* treat the same as DIRECTH */
  pke_code_direct(me, pkecode);
}


void
pke_code_unpack(struct pke_device* me, unsigned_4 pkecode)
{
  int imm = BIT_MASK_GET(pkecode, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);
  int cmd = BIT_MASK_GET(pkecode, PKE_OPCODE_CMD_B, PKE_OPCODE_CMD_E);
  int num = BIT_MASK_GET(pkecode, PKE_OPCODE_NUM_B, PKE_OPCODE_NUM_E);
  int nummx = (num == 0) ? 0x0100 : num;
  short vn = BIT_MASK_GET(cmd, 2, 3); /* unpack shape controls */
  short vl = BIT_MASK_GET(cmd, 0, 1);
  int m = BIT_MASK_GET(cmd, 4, 4);
  short cl = PKE_REG_MASK_GET(me, CYCLE, CL); /* cycle controls */
  short wl = PKE_REG_MASK_GET(me, CYCLE, WL);
  short addrwl = (wl == 0) ? 0x0100 : wl;
  int r = BIT_MASK_GET(imm, 15, 15); /* indicator bits in imm value */
  int usn = BIT_MASK_GET(imm, 14, 14);

  int n, num_operands;
  unsigned_4* last_operand_word = NULL;

  /* catch all illegal UNPACK variants */
  if(vl == 3 && vn < 3)
    {
      pke_code_error(me, pkecode);
      return;
    }
  
  /* compute PKEcode length, as given in CPU2 spec, v2.1 pg. 11 */
  if(cl >= addrwl)
    n = num;
  else
    n = cl * (nummx / addrwl) + PKE_LIMIT(nummx % addrwl, cl);
  num_operands = (31 + (32 >> vl) * (vn+1) * n)/32; /* round up to next word */
  
  /* confirm that FIFO has enough words in it */
  if(num_operands > 0)
    last_operand_word = pke_pcrel_operand(me, num_operands);
  if(last_operand_word != NULL || num_operands == 0)
    {
      address_word vu_addr_base, vutrack_addr_base;
      address_word vu_addr_max_size;
      int vector_num_out, vector_num_in;
      
      /* "transferring" operand */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);
      
      /* don't check whether VU is idle */

      /* compute VU address base */
      if(me->pke_number == 0)
	{
	  vu_addr_base = VU0_MEM1_WINDOW_START;
	  vu_addr_max_size = VU0_MEM1_SIZE;
	  vutrack_addr_base = VU0_MEM1_SRCADDR_START;
	  r = 0;
	}
      else
	{
	  vu_addr_base = VU1_MEM1_WINDOW_START;
	  vu_addr_max_size = VU1_MEM1_SIZE;
	  vutrack_addr_base = VU1_MEM1_SRCADDR_START;
	}

      /* set NUM */
      PKE_REG_MASK_SET(me, NUM, NUM, nummx);

      /* transfer given number of vectors */
      vector_num_out = 0;  /* output vector number being processed */
      vector_num_in = 0;  /* argument vector number being processed */
      do
	{
	  quadword vu_old_data;
	  quadword vu_new_data;
	  quadword unpacked_data;
	  address_word vu_addr;
	  address_word vutrack_addr;
	  unsigned_4 source_addr = 0;
	  int i;
	  int next_num;

	  /* decrement NUM */
	  next_num = PKE_REG_MASK_GET(me, NUM, NUM) - 1;
	  PKE_REG_MASK_SET(me, NUM, NUM, next_num);

	  /* compute VU destination address, as bytes in R5900 memory */
	  if(cl >= wl)
	    {
	      /* map zero to max+1 */
	      vu_addr = vu_addr_base + 16 * (BIT_MASK_GET(imm, 0, 9) +
					     (vector_num_out / addrwl) * cl +
					     (vector_num_out % addrwl));
	    }
	  else
	    vu_addr = vu_addr_base + 16 * (BIT_MASK_GET(imm, 0, 9) +
					   vector_num_out);
	  
	  /* handle "R" double-buffering bit */
	  if(r)
	    vu_addr += 16 * PKE_REG_MASK_GET(me, TOPS, TOPS);

	  /* check for vu_addr overflow */
	  while(vu_addr >= vu_addr_base + vu_addr_max_size)
	    vu_addr -= vu_addr_max_size;

	  /* compute address of tracking table entry */
	  vutrack_addr = vutrack_addr_base + ((signed_8)vu_addr - (signed_8)vu_addr_base) / 4;

	  /* read old VU data word at address; reverse words if needed */
	  {
	    unsigned_16 vu_old_badwords;
	    ASSERT(sizeof(vu_old_badwords) == 16);
	    PKE_MEM_READ(me, vu_addr,
			 &vu_old_badwords, 16);
	    vu_old_data[0] = * A4_16(& vu_old_badwords, 3);
	    vu_old_data[1] = * A4_16(& vu_old_badwords, 2);
	    vu_old_data[2] = * A4_16(& vu_old_badwords, 1);
	    vu_old_data[3] = * A4_16(& vu_old_badwords, 0);
	  }

	  /* For cyclic unpack, next operand quadword may come from instruction stream
	     or be zero. */
	  if((cl < addrwl) &&
	     (vector_num_out % addrwl) >= cl)
	    {
	      /* clear operand - used only in a "indeterminate" state */
	      for(i = 0; i < 4; i++)
		unpacked_data[i] = 0;
	    }
	  else
	    {
	      /* compute packed vector dimensions */
	      int vectorbits = 0, unitbits = 0;

	      if(vl < 3) /* PKE_UNPACK_*_{32,16,8} */
		{
		  unitbits = (32 >> vl);
		  vectorbits = unitbits * (vn+1);
		}
	      else if(vl == 3 && vn == 3) /* PKE_UNPACK_V4_5 */
		{
		  unitbits = 5;
		  vectorbits = 16;
		}
	      else /* illegal unpack variant */
		{
		  /* should have been caught at top of function */
		  ASSERT(0);
		}
	      
	      /* loop over columns */
	      for(i=0; i<=vn; i++)
		{
		  unsigned_4 operand;

		  /* offset in bits in current operand word */
		  int bitoffset =
		    (vector_num_in * vectorbits) + (i * unitbits); /* # of bits from PKEcode */

		  /* last unit of V4_5 is only one bit wide */
		  if(vl == 3 && vn == 3 && i == 3) /* PKE_UNPACK_V4_5 */
		    unitbits = 1;

		  /* confirm we're not reading more than we said we needed */
		  if(vector_num_in * vectorbits >= num_operands * 32)
		    {
		      /* this condition may be triggered by illegal
                         PKEcode / CYCLE combinations. */
		      pke_code_error(me, pkecode);
		      /* XXX: this case needs to be better understood,
                         and detected at a better time. */
		      return;
		    }

		  /* fetch bitfield operand */
		  operand = pke_pcrel_operand_bits(me, bitoffset, unitbits, & source_addr);

		  /* selectively sign-extend; not for V4_5 1-bit value */
		  if(usn || unitbits == 1)
		    unpacked_data[i] = operand;
		  else
		    unpacked_data[i] = SEXT32(operand, unitbits-1);
		}

	      /* set remaining top words in vector */
	      for(i=vn+1; i<4; i++)
		{
		  if(vn == 0) /* S_{32,16,8}: copy lowest element */
		    unpacked_data[i] = unpacked_data[0];
		  else
		    unpacked_data[i] = 0;
		}

	      /* consumed a vector from the PKE instruction stream */
	      vector_num_in ++;
	    } /* unpack word from instruction operand */
	  
	  /* process STMOD register for accumulation operations */
	  switch(PKE_REG_MASK_GET(me, MODE, MDE))
	    {
	    case PKE_MODE_ADDROW: /* add row registers to output data */
	    case PKE_MODE_ACCROW: /* same .. later conditionally accumulate */
	      for(i=0; i<4; i++)
		/* exploit R0..R3 contiguity */
		unpacked_data[i] += me->regs[PKE_REG_R0 + i][0];
	      break;

	    case PKE_MODE_INPUT: /* pass data through */
	    default: /* specified as undefined */
	      ;
	    }

	  /* compute replacement word */
	  if(m) /* use mask register? */
	    {
	      /* compute index into mask register for this word */
	      int mask_index = PKE_LIMIT(vector_num_out % addrwl, 3);
	      
	      for(i=0; i<4; i++) /* loop over columns */
		{
		  int mask_op = PKE_MASKREG_GET(me, mask_index, i);
		  unsigned_4* masked_value = NULL;
		  
		  switch(mask_op)
		    {
		    case PKE_MASKREG_INPUT: 
		      masked_value = & unpacked_data[i];

		      /* conditionally accumulate */
		      if(PKE_REG_MASK_GET(me, MODE, MDE) == PKE_MODE_ACCROW)
			me->regs[PKE_REG_R0 + i][0] = unpacked_data[i];

		      break;
		      
		    case PKE_MASKREG_ROW: /* exploit R0..R3 contiguity */
		      masked_value = & me->regs[PKE_REG_R0 + i][0];
		      break;
		      
		    case PKE_MASKREG_COLUMN: /* exploit C0..C3 contiguity */
		      masked_value = & me->regs[PKE_REG_C0 + mask_index][0];
		      break;
		      
		    case PKE_MASKREG_NOTHING:
		      /* "write inhibit" by re-copying old data */
		      masked_value = & vu_old_data[i];
		      break;
		      
		    default:
		      ASSERT(0);
		      /* no other cases possible */
		    }
		  
		  /* copy masked value for column */
		  vu_new_data[i] = *masked_value;
		} /* loop over columns */
	    } /* mask */
	  else
	    {
	      /* no mask - just copy over entire unpacked quadword */
	      memcpy(vu_new_data, unpacked_data, sizeof(unpacked_data));

	      /* conditionally store accumulated row results */
	      if(PKE_REG_MASK_GET(me, MODE, MDE) == PKE_MODE_ACCROW)
		for(i=0; i<4; i++)
		  me->regs[PKE_REG_R0 + i][0] = unpacked_data[i];
	    }

	  /* write new VU data word at address; reverse words if needed */
	  {
	    unsigned_16 vu_new_badwords;
	    * A4_16(& vu_new_badwords, 3) = vu_new_data[0];
	    * A4_16(& vu_new_badwords, 2) = vu_new_data[1];
	    * A4_16(& vu_new_badwords, 1) = vu_new_data[2];
	    * A4_16(& vu_new_badwords, 0) = vu_new_data[3];
	    ASSERT(sizeof(vu_new_badwords) == 16);
	    PKE_MEM_WRITE(me, vu_addr,
			 &vu_new_badwords, 16);
	  }

	  /* write tracking address */
	  ASSERT(sizeof(unsigned_4) == 4);
	  PKE_MEM_WRITE(me, vutrack_addr,
			& source_addr,
			4);

	  /* next vector please */
	  vector_num_out ++;
	} /* vector transfer loop */
      while(PKE_REG_MASK_GET(me, NUM, NUM) > 0);

      /* confirm we've written as many vectors as told */
      ASSERT(nummx == vector_num_out);

      /* done */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
      pke_pc_advance(me, 1 + num_operands);
    } /* PKE FIFO full enough */
  else
    {
      /* need to wait for another word */
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_WAIT);
      /* retry this instruction next clock */
    }
}


void
pke_code_error(struct pke_device* me, unsigned_4 pkecode)
{
  /* set ER1 flag in STAT register */
  PKE_REG_MASK_SET(me, STAT, ER1, 1);

  if(! PKE_REG_MASK_GET(me, ERR, ME1))
    {
      pke_begin_interrupt_stall(me);
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_STALL);
    }
  else
    {
      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_IDLE);
    }

  /* advance over faulty word */
  pke_pc_advance(me, 1);
}

void
pke_options(struct pke_device *me, unsigned_4 option, char *option_string)  
{
  switch (option) 
    {
    case PKE_OPT_DEBUG_NAME:
      if ( me->fifo_trace_file != NULL ) 
        {
          fclose (me->fifo_trace_file);
          me->fifo_trace_file = NULL;
        }
      sky_store_file_name (&me->fifo_trace_file_name, option_string);
      break;

    case PKE_OPT_TRACE_ON:
      me->flags |= PKE_FLAG_TRACE_ON;
      break;

    case PKE_OPT_TRACE_OFF:
    case PKE_OPT_TRACE_NAME:
      if ( me->trace_file != NULL ) 
        {
          fclose (me->trace_file);
          me->trace_file = NULL;
        }
      
      if ( option == PKE_OPT_TRACE_OFF ) 
        me->flags &= ~PKE_FLAG_TRACE_ON;
      else
        sky_store_file_name (&me->trace_file_name, option_string);
      
      break;
   
    case PKE_OPT_CLOSE:
      if (me->trace_file != NULL) 
        fclose (me->trace_file);
      if (me->fifo_trace_file != NULL ) 
        fclose (me->fifo_trace_file);
      break;

    default: 
      ASSERT (0);
      break;
    }

  return;
}
