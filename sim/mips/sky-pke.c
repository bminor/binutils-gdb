/* Copyright (C) 1998, Cygnus Solutions */

#include <stdlib.h>
#include "sky-pke.h"
#include "sky-dma.h"
#include "sim-assert.h"
#include "sky-vu0.h"
#include "sky-vu1.h"
#include "sky-gpuif.h"


/* Imported functions */

void device_error (device *me, char* message);  /* device.c */


/* Internal function declarations */

static int pke_io_read_buffer(device*, void*, int, address_word,
			       unsigned, sim_cpu*, sim_cia);
static int pke_io_write_buffer(device*, const void*, int, address_word,
			       unsigned, sim_cpu*, sim_cia);
static void pke_issue(struct pke_device*);
static void pke_pc_advance(struct pke_device*, int num_words);
static unsigned_4* pke_pc_operand(struct pke_device*, int word_num);
static struct fifo_quadword* pke_pc_fifo(struct pke_device*, int word_num);
static int pke_track_write(struct pke_device*, const void* src, int len,
			   address_word dest, unsigned_4 sourceaddr);
static void pke_attach(SIM_DESC sd, struct pke_device* me);



/* Static data */

struct pke_device pke0_device = 
{ 
  { "pke0", &pke_io_read_buffer, &pke_io_write_buffer }, /* device */
  0, 0,        /* ID, flags */
  {},          /* regs */
  NULL, 0, 0, NULL,  /* FIFO */
  0, 0            /* pc */
};


struct pke_device pke1_device = 
{ 
  { "pke1", &pke_io_read_buffer, &pke_io_write_buffer }, /* device */
  1, 0,        /* ID, flags */
  {},          /* regs */
  NULL, 0, 0, NULL, /* FIFO */
  0, 0         /* pc */
};



/* External functions */


/* Attach PKE addresses to main memory */

void
pke0_attach(SIM_DESC sd) 
{
  pke_attach(sd, & pke0_device);
}

void
pke1_attach(SIM_DESC sd) 
{
  pke_attach(sd, & pke1_device);
}



/* Issue a PKE instruction if possible */

void 
pke0_issue() 
{
  pke_issue(& pke0_device);
}

void 
pke1_issue() 
{
  pke_issue(& pke0_device);
}



/* Internal functions */


/* Attach PKE memory regions to simulator */

void 
pke_attach(SIM_DESC sd, struct pke_device* me) 
{
  /* register file */
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
		   (me->pke_number == 0) ? PKE0_REGISTER_WINDOW_START : PKE1_REGISTER_WINDOW_START,
                   PKE_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) &pke0_device,
                   NULL /*buffer*/);

  /* FIFO port */
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
		   (me->pke_number == 0) ? PKE0_FIFO_ADDR : PKE1_FIFO_ADDR,
                   sizeof(quadword) /*nr_bytes*/,
                   0 /*modulo*/,
                   (device*) &pke1_device,
                   NULL /*buffer*/);

  /* source-addr tracking word */
  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
		   (me->pke_number == 0) ? PKE0_SRCADDR : PKE1_SRCADDR,
                   sizeof(unsigned_4) /*nr_bytes*/,
                   0 /*modulo*/,
		   NULL, 
                   zalloc(sizeof(unsigned_4)) /*buffer*/);
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
      int readable = 1;
      quadword result;

      /* clear result */
      result[0] = result[1] = result[2] = result[3] = 0;

      /* handle reads to individual registers; clear `readable' on error */
      switch(reg_num)
	{
	  /* handle common case of register reading, side-effect free */
	  /* PKE1-only registers*/
	case PKE_REG_BASE:
	case PKE_REG_OFST:
	case PKE_REG_TOPS:
	case PKE_REG_TOP:
	case PKE_REG_DBF:
	  if(me->pke_number == 0)
	    readable = 0;
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
	  result[0] = me->regs[reg_num][0];
	  break;

	  /* handle common case of write-only registers */
	case PKE_REG_FBRST:
	  readable = 0;
	  break;

	default:
	  ASSERT(0); /* test above should prevent this possibility */
	}

      /* perform transfer & return */
      if(readable) 
	{
	  /* copy the bits */
	  memcpy(dest, ((unsigned_1*) &result) + reg_byte, nr_bytes);
	  /* okay */
	  return nr_bytes;
	}
      else
	{
	  /* error */
	  return 0;
	} 

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


/* Handle a PKE read; return no. of bytes written */

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
      int writeable = 1;
      quadword input;

      /* clear input */
      input[0] = input[1] = input[2] = input[3] = 0;

      /* write user-given bytes into input */
      memcpy(((unsigned_1*) &input) + reg_byte, src, nr_bytes);

      /* handle writes to individual registers; clear `writeable' on error */
      switch(reg_num)
	{
	case PKE_REG_FBRST:
	  /* XXX: order of evaluation?  STP && STC ?? */
	  if(BIT_MASK_GET(input[0], 0, 0)) /* RST bit */
	    {
	      /* clear FIFO: also prevents re-execution attempt of
                 possible stalled instruction */
	      me->fifo_num_elements = me->fifo_pc;
	      /* clear registers */
	      memset(me->regs, 0, sizeof(me->regs));
	      me->flags = 0;
	      me->qw_pc = 0;
	    }
	  if(BIT_MASK_GET(input[0], 1, 1)) /* FBK bit */
	    {
	      PKE_REG_MASK_SET(me, STAT, PFS, 1);
	    }
	  if(BIT_MASK_GET(input[0], 2, 2)) /* STP bit */
	    {
	      /* XXX: how to safely abort "currently executing" (=> stalled) instruction? */
	      PKE_REG_MASK_SET(me, STAT, PSS, 1);
	    }
	  if(BIT_MASK_GET(input[0], 2, 2)) /* STC bit */
	    {
	      /* clear a bunch of status bits */
	      PKE_REG_MASK_SET(me, STAT, PSS, 0);
	      PKE_REG_MASK_SET(me, STAT, PFS, 0);
	      PKE_REG_MASK_SET(me, STAT, PIS, 0);
	      PKE_REG_MASK_SET(me, STAT, INT, 0);
	      PKE_REG_MASK_SET(me, STAT, ER0, 0);
	      PKE_REG_MASK_SET(me, STAT, ER1, 0);
	      /* will allow resumption of possible stalled instruction */
	    }
	  break;

	case PKE_REG_ERR:
	  /* copy bottom three bits */
	  BIT_MASK_SET(me->regs[PKE_REG_ERR][0], 0, 2, BIT_MASK_GET(input[0], 0, 2));
	  break;

	case PKE_REG_MARK:
	  /* copy bottom sixteen bits */
	  PKE_REG_MASK_SET(me, MARK, MARK, BIT_MASK_GET(input[0], 0, 15));
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
      if(writeable) 
	{
	  /* okay */
	  return nr_bytes;
	}
      else
	{
	  /* error */
	  return 0;
	} 

      /* NOTREACHED */
    }
  else if(addr >= my_fifo_addr &&
	  addr < my_fifo_addr + sizeof(quadword))
    {
      /* FIFO */
      struct fifo_quadword* fqw;

      /* assert transfer size == 128 bits */
      if(nr_bytes != sizeof(quadword))
	return 0;

      /* ensure FIFO has enough elements */
      if(me->fifo_num_elements == me->fifo_buffer_size)
	{
	  /* time to grow */
	  int new_fifo_buffer_size = me->fifo_buffer_size + 20;
	  void* ptr = realloc((void*) me->fifo, new_fifo_buffer_size*sizeof(quadword));

	  if(ptr == NULL)
	    {
	      /* oops, cannot enlarge FIFO any more */
	      device_error(me_, "Cannot enlarge FIFO buffer\n");
	      return 0;
	    }

	  me->fifo_buffer_size = new_fifo_buffer_size;
	}

      /* add new quadword at end of FIFO */
      fqw = & me->fifo[me->fifo_num_elements];
      memcpy((void*) fqw->data, src, nr_bytes);
      sim_read(CPU_STATE(cpu),
	       (SIM_ADDR) (me->pke_number == 0 ? DMA_CHANNEL0_SRCADDR : DMA_CHANNEL1_SRCADDR),
	       (void*) & fqw->source_address,
	       sizeof(address_word));
      sim_read(CPU_STATE(cpu),
	       (SIM_ADDR) (me->pke_number == 0 ? DMA_CHANNEL0_PKTFLAG : DMA_CHANNEL1_PKTFLAG),
	       (void*) & fqw->dma_tag_present,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      me->fifo_num_elements++;

      /* set FQC to "1" as FIFO is now not empty */ 
      PKE_REG_MASK_SET(me, STAT, FQC, 1);
      
      /* okay */
      return nr_bytes;
    }

  /* NOTREACHED */
  return 0;
}



/* Issue & swallow next PKE opcode if possible/available */

void
pke_issue(struct pke_device* me)
{
  struct fifo_quadword* fqw;
  unsigned_4 fw;
  unsigned_4 cmd, intr, num;
  unsigned_4 imm;
  int next_pps_state; /* PPS after this instruction issue attempt */

  /* 1 -- test go / no-go for PKE execution */

  /* check for stall/halt control bits */
  /* XXX: What is the PEW bit for? */
  if(PKE_REG_MASK_GET(me, STAT, PSS) ||
     PKE_REG_MASK_GET(me, STAT, PFS) ||
     /* maskable stall controls: ER0, ER1, PIS */
     (PKE_REG_MASK_GET(me, STAT, ER0) && !PKE_REG_MASK_GET(me, ERR, ME0)) ||
     (PKE_REG_MASK_GET(me, STAT, ER1) && !PKE_REG_MASK_GET(me, ERR, ME1)) ||
     (PKE_REG_MASK_GET(me, STAT, PIS) && !PKE_REG_MASK_GET(me, ERR, MII)))
    {
      /* XXX */
    }
  /* XXX: handle PSS by *skipping* instruction? */

  /* confirm availability of new quadword of PKE instructions */
  if(me->fifo_num_elements <= me->fifo_pc)
    return;


  /* 2 -- fetch PKE instruction */

  /* "fetch" instruction quadword */ 
  fqw = & me->fifo[me->fifo_pc];

  /* skip over DMA tags, if present */
  if((fqw->dma_tag_present != 0) && (me->qw_pc < 2))
    {
      ASSERT(me->qw_pc == 0);
      /* XXX: check validity of DMA tag; if bad, set ER0 flag */
      me->qw_pc = 2;
    }
  
  /* "fetch" instruction word */
  fw = fqw->data[me->qw_pc];

  /* store it in PKECODE register */
  me->regs[PKE_REG_CODE][0] = fw;


  /* 3 -- decode PKE instruction */

  /* PKE instruction format: [intr 0:0][pke-command 6:0][num 7:0][immediate 15:0],
     so op-code is in top byte. */
  intr = BIT_MASK_GET(fw, PKE_OPCODE_I_B,   PKE_OPCODE_I_E);
  cmd  = BIT_MASK_GET(fw, PKE_OPCODE_CMD_B, PKE_OPCODE_CMD_E);
  num  = BIT_MASK_GET(fw, PKE_OPCODE_NUM_B, PKE_OPCODE_NUM_E);
  imm  = BIT_MASK_GET(fw, PKE_OPCODE_IMM_B, PKE_OPCODE_IMM_E);

  if(intr)
    {
      /* set INT flag in STAT register */
      PKE_REG_MASK_SET(me, STAT, INT, 1);
      /* XXX: send interrupt to R5900? */
    }

  /* decoding */
  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_DECODE);
  next_pps_state = PKE_REG_STAT_PPS_IDLE;  /* assume instruction completes */

  /* decode */
  if(IS_PKE_CMD(cmd, PKENOP))
    {
      /* no work required, yey */
      pke_pc_advance(me, 1);
    }
  else if(IS_PKE_CMD(cmd, STCYCL))
    {
      /* copy immediate value into CYCLE reg */
      me->regs[PKE_REG_CYCLE][0] = imm;
      pke_pc_advance(me, 1);
    }
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, OFFSET))
    {
      /* copy 10 bits to OFFSET field */
      PKE_REG_MASK_SET(me, OFST, OFFSET, BIT_MASK_GET(imm, 0, 9));
      /* clear DBF bit */
      PKE_REG_MASK_SET(me, DBF, DF, 0);
      /* clear other DBF bit */
      PKE_REG_MASK_SET(me, STAT, DBF, 0);
      /* set TOPS = BASE */
      PKE_REG_MASK_SET(me, TOPS, TOPS,
		       PKE_REG_MASK_GET(me, BASE, BASE));
      pke_pc_advance(me, 1);
    }
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, BASE))
    {
      /* copy 10 bits to BASE field */
      PKE_REG_MASK_SET(me, BASE, BASE, BIT_MASK_GET(imm, 0, 9));
      /* clear DBF bit */
      PKE_REG_MASK_SET(me, DBF, DF, 0);
      /* clear other DBF bit */
      PKE_REG_MASK_SET(me, STAT, DBF, 0);
      /* set TOPS = BASE */
      PKE_REG_MASK_SET(me, TOPS, TOPS,
		       PKE_REG_MASK_GET(me, BASE, BASE));
      pke_pc_advance(me, 1);
    }
  else if(IS_PKE_CMD(cmd, ITOP))
    {
      /* copy 10 bits to ITOPS field */
      PKE_REG_MASK_SET(me, ITOPS, ITOPS, BIT_MASK_GET(imm, 0, 9));
      pke_pc_advance(me, 1);
    }
  else if(IS_PKE_CMD(cmd, STMOD))
    {
      /* copy 2 bits to MODE register */
      PKE_REG_MASK_SET(me, MODE, MDE, BIT_MASK_GET(imm, 0, 2));
      pke_pc_advance(me, 1);
    }
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, MSKPATH3)) /* MSKPATH3 */
    {
      /* XXX: what to do with this?  DMA control register? */
      pke_pc_advance(me, 1);
    }
  else if(IS_PKE_CMD(cmd, PKEMARK))
    {
      /* copy 16 bits to MARK register */
      PKE_REG_MASK_SET(me, MARK, MARK, BIT_MASK_GET(imm, 0, 15));
      /* set MRK bit in STAT register - CPU2 v2.1 docs incorrect */
      PKE_REG_MASK_SET(me, STAT, MRK, 1);
      pke_pc_advance(me, 1);
    }
  else if(IS_PKE_CMD(cmd, FLUSHE))
    {
      /* read VU status word */
      unsigned_4 vu_stat;
      sim_read(NULL,
	       (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
	       (void*) & vu_stat,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      /* check if VBS bit is clear, i.e., VU is idle */
      if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0)
	{
	  /* VU idle */
	  /* advance PC */
	  pke_pc_advance(me, 1);
	}
      else
	{
	  /* VU busy */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, FLUSH))
    {
      /* read VU status word */
      unsigned_4 vu_stat;
      sim_read(NULL,
	       (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
	       (void*) & vu_stat,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      /* check if VGW bit is clear, i.e., PATH1 is idle */
      /* simulator design implies PATH2 is always "idle" */
      if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0 &&
	 BIT_MASK_GET(vu_stat, VU_REG_STAT_VGW_B, VU_REG_STAT_VGW_E) == 0 &&
	 1 /* PATH2 always idle */)
	{
	  /* VU idle */
	  /* PATH1 idle */
	  /* PATH2 idle */
	  /* advance PC */
	  pke_pc_advance(me, 1);
	}
      else
	{
	  /* GPUIF busy */
	  /* retry this instruction next clock */
	}
    }
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, FLUSHA))
    {
      /* read VU status word */
      unsigned_4 vu_stat;
      sim_read(NULL,
	       (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
	       (void*) & vu_stat,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      /* check if VGW bit is clear, i.e., PATH1 is idle */
      /* simulator design implies PATH2 is always "idle" */
      /* XXX: simulator design implies PATH3 is always "idle" */
      if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0 &&
	 BIT_MASK_GET(vu_stat, VU_REG_STAT_VGW_B, VU_REG_STAT_VGW_E) == 0 &&
	 1 /* PATH2 always idle */ &&
         1 /* PATH3 always idle */)
	{
	  /* VU idle */
	  /* PATH1 idle */
	  /* PATH2 idle */
	  /* PATH3 idle */
	  /* advance PC */
	  pke_pc_advance(me, 1);
	}
      else
	{
	  /* GPUIF busy */
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, PKEMSCAL))
    {
      /* read VU status word */
      unsigned_4 vu_stat;
      sim_read(NULL,
	       (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
	       (void*) & vu_stat,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      /* check if VBS bit is clear, i.e., VU is idle */
      if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0)
	{
	  /* VU idle */
	  unsigned_4 vu_pc;

	  /* perform PKE1-unique processing for microprogram calls */
	  if(me->pke_number == 1)
	    {
	      /* flip DBF */
	      PKE_REG_MASK_SET(me, DBF, DF,
			       PKE_REG_MASK_GET(me, DBF, DF) ? 0 : 1);
	      PKE_REG_MASK_SET(me, STAT, DBF, PKE_REG_MASK_GET(me, DBF, DF));
	      /* compute new TOPS */
	      PKE_REG_MASK_SET(me, TOPS, TOPS,
			       (PKE_REG_MASK_GET(me, BASE, BASE) +
				(PKE_REG_MASK_GET(me, DBF, DF) *
				 PKE_REG_MASK_GET(me, OFST, OFFSET))));
	      /* compute new ITOP and TOP */
	      PKE_REG_MASK_SET(me, ITOP, ITOP,
			       PKE_REG_MASK_GET(me, ITOPS, ITOPS));
	      PKE_REG_MASK_SET(me, TOP, TOP,
			       PKE_REG_MASK_GET(me, TOPS, TOPS));
	    }

	  /* compute new PC */
	  vu_pc = BIT_MASK_GET(imm, 0, 15); /* XXX: all bits significant? */
	  /* write new PC; callback function gets VU running */
	  sim_write(NULL,
		    (SIM_ADDR) (me->pke_number == 0 ? VU0_PC_START : VU1_PC_START),
		    (void*) & vu_pc,
		    sizeof(unsigned_4));
	  /* advance PC */
	  pke_pc_advance(me, 1);
	}
      else
	{
	  /* VU busy */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, PKEMSCNT))
    {
      /* read VU status word */
      unsigned_4 vu_stat;
      sim_read(NULL,
	       (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
	       (void*) & vu_stat,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      /* check if VBS bit is clear, i.e., VU is idle */
      if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0)
	{
	  /* VU idle */
	  unsigned_4 vu_pc;

	  /* flip DBF etc. for PKE1 */
	  if(me->pke_number == 1)
	    {
	      PKE_REG_MASK_SET(me, DBF, DF,
			       PKE_REG_MASK_GET(me, DBF, DF) ? 0 : 1);
	      PKE_REG_MASK_SET(me, STAT, DBF, PKE_REG_MASK_GET(me, DBF, DF));
	      PKE_REG_MASK_SET(me, TOPS, TOPS,
			       (PKE_REG_MASK_GET(me, BASE, BASE) +
				(PKE_REG_MASK_GET(me, DBF, DF) *
				 PKE_REG_MASK_GET(me, OFST, OFFSET))));
	      PKE_REG_MASK_SET(me, ITOP, ITOP,
			       PKE_REG_MASK_GET(me, ITOPS, ITOPS));
	      PKE_REG_MASK_SET(me, TOP, TOP,
			       PKE_REG_MASK_GET(me, TOPS, TOPS));
	    }

	  /* read old PC */
	  sim_read(NULL,
		   (SIM_ADDR) (me->pke_number == 0 ? VU0_PC_START : VU1_PC_START),
		   (void*) & vu_pc,
		   sizeof(unsigned_4));
	  /* rewrite its PC; callback function gets VU running */
	  sim_write(NULL,
		    (SIM_ADDR) (me->pke_number == 0 ? VU0_PC_START : VU1_PC_START),
		    (void*) & vu_pc,
		    sizeof(unsigned_4));
	  /* advance PC */
	  pke_pc_advance(me, 1);
	}
      else
	{
	  /* VU busy */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(me->pke_number == 1 && IS_PKE_CMD(cmd, PKEMSCALF))
    {
      /* read VU status word */
      unsigned_4 vu_stat;
      sim_read(NULL,
	       (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
	       (void*) & vu_stat,
	       sizeof(unsigned_4));
      /* XXX: check RC */

      /* check if VGW bit is clear, i.e., PATH1 is idle */
      /* simulator design implies PATH2 is always "idle" */
      if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0 &&
	 BIT_MASK_GET(vu_stat, VU_REG_STAT_VGW_B, VU_REG_STAT_VGW_E) == 0 &&
	 1 /* PATH2 always idle */)
	{
	  /* VU idle */
	  /* PATH1 idle */
	  /* PATH2 idle */
	  unsigned_4 vu_pc;

	  /* flip DBF etc. for PKE1 */
	  if(me->pke_number == 1)
	    {
	      PKE_REG_MASK_SET(me, DBF, DF,
			       PKE_REG_MASK_GET(me, DBF, DF) ? 0 : 1);
	      PKE_REG_MASK_SET(me, STAT, DBF, PKE_REG_MASK_GET(me, DBF, DF));
	      PKE_REG_MASK_SET(me, TOPS, TOPS,
			       (PKE_REG_MASK_GET(me, BASE, BASE) +
				(PKE_REG_MASK_GET(me, DBF, DF) *
				 PKE_REG_MASK_GET(me, OFST, OFFSET))));
	      PKE_REG_MASK_SET(me, ITOP, ITOP,
			       PKE_REG_MASK_GET(me, ITOPS, ITOPS));
	      PKE_REG_MASK_SET(me, TOP, TOP,
			       PKE_REG_MASK_GET(me, TOPS, TOPS));
	    }

	  /* compute new PC */
	  vu_pc = BIT_MASK_GET(imm, 0, 15); /* XXX: all bits significant? */
	  /* write new PC; callback function gets VU running */
	  sim_write(NULL,
		    (SIM_ADDR) (me->pke_number == 0 ? VU0_PC_START : VU1_PC_START),
		    (void*) & vu_pc,
		    sizeof(unsigned_4));
	  /* advance PC */
	  pke_pc_advance(me, 1);
	}
      else
	{
	  /* VU busy */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, STMASK))
    {
      /* check that FIFO has one more word for STMASK operand */
      unsigned_4* mask;

      mask = pke_pc_operand(me, 1);
      if(mask != NULL)
	{
	  /* "transferring" operand */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);
	  /* fill the register */
	  PKE_REG_MASK_SET(me, MASK, MASK, *mask);
	  /* advance PC */
	  pke_pc_advance(me, 2);
	}
      else
	{
	  /* need to wait for another word */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, STROW))
    {
      /* check that FIFO has four more words for STROW operand */
      unsigned_4* last_op;

      last_op = pke_pc_operand(me, 4);
      if(last_op != NULL)
	{
	  /* "transferring" operand */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);

	  /* copy ROW registers: must all exist if 4th operand exists */
	  me->regs[PKE_REG_R0][0] = * pke_pc_operand(me, 1);
	  me->regs[PKE_REG_R1][0] = * pke_pc_operand(me, 2);
	  me->regs[PKE_REG_R2][0] = * pke_pc_operand(me, 3);
	  me->regs[PKE_REG_R3][0] = * pke_pc_operand(me, 4);

	  /* advance PC */
	  pke_pc_advance(me, 5);
	}
      else
	{
	  /* need to wait for another word */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, STCOL))
    {
      /* check that FIFO has four more words for STCOL operand */
      unsigned_4* last_op;

      last_op = pke_pc_operand(me, 4);
      if(last_op != NULL)
	{
	  /* "transferring" operand */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);

	  /* copy COL registers: must all exist if 4th operand exists */
	  me->regs[PKE_REG_C0][0] = * pke_pc_operand(me, 1);
	  me->regs[PKE_REG_C1][0] = * pke_pc_operand(me, 2);
	  me->regs[PKE_REG_C2][0] = * pke_pc_operand(me, 3);
	  me->regs[PKE_REG_C3][0] = * pke_pc_operand(me, 4);

	  /* advance PC */
	  pke_pc_advance(me, 5);
	}
      else
	{
	  /* need to wait for another word */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, MPG))
    {
      unsigned_4* last_mpg_word;

      /* map zero to max+1 */
      if(num==0) num=0x100;

      /* check that FIFO has a few more words for MPG operand */
      last_mpg_word = pke_pc_operand(me, num*2); /* num: number of 64-bit words */
      if(last_mpg_word != NULL)
	{
	  /* perform implied FLUSHE */
	  /* read VU status word */
	  unsigned_4 vu_stat;
	  sim_read(NULL,
		   (SIM_ADDR) (me->pke_number == 0 ? VPE0_STAT : VPE1_STAT),
		   (void*) & vu_stat,
		   sizeof(unsigned_4));
	  /* XXX: check RC */
	  
	  /* check if VBS bit is clear, i.e., VU is idle */
	  if(BIT_MASK_GET(vu_stat, VU_REG_STAT_VBS_B, VU_REG_STAT_VBS_E) == 0)
	    {
	      /* VU idle */
	      int i;

	      /* "transferring" operand */
	      PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);

	      /* transfer VU instructions, one word per iteration */
	      for(i=0; i<num*2; i++)
		{
		  address_word vu_addr_base, vu_addr;
		  address_word vutrack_addr_base, vutrack_addr;
		  struct fifo_quadword* fq = pke_pc_fifo(me, num);
		  unsigned_4* operand = pke_pc_operand(me, num);

		  /* imm: in 64-bit units for MPG instruction */

		  /* XXX: set NUM */

		  /* VU*_MEM0 : instruction memory */
		  vu_addr_base = (me->pke_number == 0) ?
		    VU0_MEM0_WINDOW_START : VU0_MEM0_WINDOW_START;
		  vu_addr = vu_addr_base + (imm*2) + i;

		  /* VU*_MEM0_TRACK : source-addr tracking table */
		  vutrack_addr_base = (me->pke_number == 0) ?
		    VU0_MEM0_SRCADDR_START : VU1_MEM0_SRCADDR_START;
		  vutrack_addr = vu_addr_base + (imm*2) + i;

		  /* write data into VU memory */
		  pke_track_write(me, operand, sizeof(unsigned_4),
				  vu_addr, fq->source_address);

		  /* write srcaddr into VU srcaddr tracking table */
		  sim_write(NULL,
			    (SIM_ADDR) vutrack_addr,
			    (void*) & fq->source_address,
			    sizeof(unsigned_4));
		  /* XXX: check RC */
		} /* VU xfer loop */
	      
	      /* advance PC */
	      pke_pc_advance(me, 1 + num*2);
	    }
	  else
	    {
	      /* VU busy */
	      next_pps_state = PKE_REG_STAT_PPS_WAIT;
	      /* retry this instruction next clock */
	    }
	} /* if FIFO full enough */
      else
	{
	  /* need to wait for another word */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, DIRECT) || IS_PKE_CMD(cmd, DIRECTHL)) /* treat identically */
    {
      /* check that FIFO has a few more words for DIRECT operand */
      unsigned_4* last_direct_word;

      /* map zero to max+1 */
      if(imm==0) imm=0x10000;

      last_direct_word = pke_pc_operand(me, imm*4); /* num: number of 128-bit words */
      if(last_direct_word != NULL)
	{
	  /* VU idle */
	  int i;
	  quadword fifo_data;

	  /* "transferring" operand */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);

	  /* transfer GPUIF quadwords, one word per iteration */
	  for(i=0; i<imm*4; i++)
	    {
	      struct fifo_quadword* fq = pke_pc_fifo(me, num);
	      unsigned_4* operand = pke_pc_operand(me, num);
	      
	      /* collect word into quadword */
	      fifo_data[i%4] = *operand;

	      /* write to GPUIF FIFO only with full word */
	      if(i%4 == 3)
		{
		  address_word gpuif_fifo = GPUIF_PATH2_FIFO_ADDR+(i/4);
		  pke_track_write(me, fifo_data, sizeof(quadword),
				  (SIM_ADDR) gpuif_fifo, fq->source_address);
		  /* XXX: check RC */
		} /* write collected quadword */

	    } /* GPUIF xfer loop */
	      
	  /* advance PC */
	  pke_pc_advance(me, 1 + imm*4);
	} /* if FIFO full enough */
      else
	{
	  /* need to wait for another word */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  else if(IS_PKE_CMD(cmd, UNPACK)) /* warning: monster complexity */
    {
      short vn = BIT_MASK_GET(cmd, 2, 3);
      short vl = BIT_MASK_GET(cmd, 0, 1);
      short vnvl = BIT_MASK_GET(cmd, 0, 3);
      int m = BIT_MASK_GET(cmd, 4, 4);
      short cl = PKE_REG_MASK_GET(me, CYCLE, CL);
      short wl = PKE_REG_MASK_GET(me, CYCLE, WL);
      int n, num_operands;
      unsigned_4* last_operand_word;

      /* map zero to max+1 */
      if(num==0) num=0x100;

      /* compute PKEcode length, as given in CPU2 spec, v2.1 pg. 11 */
      if(wl <= cl)
	n = num;
      else
	n = cl * (num/wl) + PKE_LIMIT(num % wl, cl);
      num_operands = (((sizeof(unsigned_4) >> vl) * (vn+1) * n)/sizeof(unsigned_4));

      /* confirm that FIFO has enough words in it */
      last_operand_word = pke_pc_operand(me, num_operands);
      if(last_operand_word != NULL)
	{
	  address_word vu_addr_base;
	  int operand_num, vector_num;

	  /* "transferring" operand */
	  PKE_REG_MASK_SET(me, STAT, PPS, PKE_REG_STAT_PPS_XFER);

	  /* XXX: don't check whether VU is idle?? */

	  if(me->pke_number == 0)
	    vu_addr_base = VU0_MEM1_WINDOW_START + BIT_MASK_GET(imm, 0, 9);
	  else
	    {
	      vu_addr_base = VU1_MEM1_WINDOW_START + BIT_MASK_GET(imm, 0, 9);
	      if(BIT_MASK_GET(imm, 15, 15)) /* fetch R flag from imm word */
		vu_addr_base += PKE_REG_MASK_GET(me, TOPS, TOPS);
	    }

	  /* XXX: vu_addr overflow check */

	  /* transfer given number of vectors */
	  operand_num = 1; /* word index into instruction stream: 1..num_operands */
	  vector_num = 0;  /* vector number being processed: 0..num-1 */
	  while(operand_num <= num_operands)
	    {
	      quadword vu_old_data;
	      quadword vu_new_data;
	      quadword unpacked_data;
	      address_word vu_addr;
	      struct fifo_quadword* fq;
	      int i;

	      /* XXX: set NUM */

	      /* compute VU destination address, as bytes in R5900 memory */
	      if(cl >= wl)
		{
		  /* map zero to max+1 */
		  if(wl == 0) wl = 0x0100;
		  vu_addr = vu_addr_base + 16*(cl*(vector_num/wl) + (vector_num%wl));
		}
	      else
		vu_addr = vu_addr_base + 16*vector_num;

	      /* read old VU data word at address */
	      sim_read(NULL, (SIM_ADDR) vu_addr, (void*) & vu_old_data, sizeof(vu_old_data));

	      /* Let sourceaddr track the first operand */
	      fq = pke_pc_fifo(me, operand_num);

	      /* For cyclic unpack, next operand quadword may come from instruction stream
		 or be zero. */
	      if((cl < wl) && ((vector_num % wl) >= cl)) /* wl != 0, set above */
		{
		  /* clear operand - used only in a "indeterminate" state */
		  for(i = 0; i < 4; i++)
		    unpacked_data[i] = 0;
		}
	      else
		{
		  /* compute unpacked words from instruction stream */
		  switch(vnvl)
		    {
		    case PKE_UNPACK_S_32:
		    case PKE_UNPACK_V2_32:
		    case PKE_UNPACK_V3_32:
		    case PKE_UNPACK_V4_32:
		      /* copy (vn+1) 32-bit values */
		      for(i = 0; i < vn+1; i++)
			{
			  unsigned_4* operand = pke_pc_operand(me, operand_num);
			  unpacked_data[i] = *operand;
			  operand_num ++;
			}
		      break;
		      
		    case PKE_UNPACK_S_16:
		    case PKE_UNPACK_V2_16:
		    case PKE_UNPACK_V3_16:
		    case PKE_UNPACK_V4_16:
		      /* copy (vn+1) 16-bit values, packed two-per-word */
		      for(i=0; i<vn+1; i+=2)
			{
			  unsigned_4* operand = pke_pc_operand(me, operand_num);
			  unpacked_data[i] = BIT_MASK_GET_SX(*operand, 0, 15, 31);
			  unpacked_data[i+1] = BIT_MASK_GET_SX(*operand, 16, 31, 31);
			  operand_num ++;
			}
		      break;
		      
		    case PKE_UNPACK_S_8:
		    case PKE_UNPACK_V2_8:
		    case PKE_UNPACK_V3_8:
		    case PKE_UNPACK_V4_8:
		      /* copy (vn+1) 8-bit values, packed four-per-word */
		      for(i=0; i<vn+1; i+=4)
			{
			  unsigned_4* operand = pke_pc_operand(me, operand_num);
			  unpacked_data[i] = BIT_MASK_GET_SX(*operand, 0, 7, 31);
			  unpacked_data[i+1] = BIT_MASK_GET_SX(*operand, 8, 15, 31);
			  unpacked_data[i+2] = BIT_MASK_GET_SX(*operand, 16, 23, 31);
			  unpacked_data[i+3] = BIT_MASK_GET_SX(*operand, 24, 31, 31);
			  operand_num ++;
			}
		      break;
		      
		    case PKE_UNPACK_V4_5:
		      /* copy four 1/5/5/5-bit values, packed into a sixteen-bit */
		      for(i=0; i<vn+1; i+=4)
			{
			  unsigned_4* operand = pke_pc_operand(me, operand_num);
			  unpacked_data[i] = BIT_MASK_GET_SX(*operand, 0, 4, 31);
			  unpacked_data[i+1] = BIT_MASK_GET_SX(*operand, 5, 9, 31);
			  unpacked_data[i+2] = BIT_MASK_GET_SX(*operand, 10, 14, 31);
			  unpacked_data[i+3] = BIT_MASK_GET_SX(*operand, 15, 15, 31);
			  /* ignore other 16 bits in operand */
			  operand_num ++;
			}
		      break;
		      
		    default: /* bad UNPACK code */
		      {
			/* XXX: how to handle? */
			/* set ER1 flag in STAT register */
			PKE_REG_MASK_SET(me, STAT, ER1, 1);
		      }		  
		    }
		}
	      
	      /* compute replacement word - function of vn, vl, mask */
	      if(m) /* use mask register? */
		{
		  /* compute index into mask register for this word */
		  int mask_index = PKE_LIMIT(vector_num % wl, 3);  /* wl != 0, set above */

		  for(i=0; i<3; i++) /* loop over columns */
		    {
		      int mask_op = PKE_MASKREG_GET(me, mask_index, i);
		      unsigned_4* masked_value = NULL;
		      unsigned_4 zero = 0;

		      switch(mask_op)
			{
			case PKE_MASKREG_INPUT: 
			  /* for vn == 0, all columns are copied from column 0 */
			  if(vn == 0)
			    masked_value = & unpacked_data[0];
			  else if(i > vn)
			    masked_value = & zero; /* XXX: what to put here? */
			  else
			    masked_value = & unpacked_data[i];
			  break;

			case PKE_MASKREG_ROW: /* exploit R0..R3 contiguity */
			  masked_value = & me->regs[PKE_REG_R0 + i][0];
			  break;

			case PKE_MASKREG_COLUMN: /* exploit C0..C3 contiguity */
			  masked_value = & me->regs[PKE_REG_C0 + PKE_LIMIT(vector_num,3)][0];
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
		      memcpy(& vu_new_data[i], masked_value, sizeof(unsigned_4));
		    } /* loop over columns */
		}
	      else
		{
		  /* no mask - just copy over entire unpacked quadword */
		  memcpy(vu_new_data, unpacked_data, sizeof(unpacked_data));
		}

	      /* process STMOD register for accumulation operations */
	      switch(PKE_REG_MASK_GET(me, MODE, MDE))
		{
		case PKE_MODE_ADDROW: /* add row registers to output data */
		  for(i=0; i<4; i++)
		    /* exploit R0..R3 contiguity */
		    vu_new_data[i] += me->regs[PKE_REG_R0 + i][0];
		  break;

		case PKE_MODE_ACCROW: /* add row registers to output data; accumulate */
		  for(i=0; i<4; i++)
		    {
		      /* exploit R0..R3 contiguity */
		      vu_new_data[i] += me->regs[PKE_REG_R0 + i][0];
		      me->regs[PKE_REG_R0 + i][0] = vu_new_data[i];
		    }
		  break;

		case PKE_MODE_INPUT: /* pass data through */
		default:
		  ;
		}

	      /* write replacement word */
	      pke_track_write(me, vu_new_data, sizeof(vu_new_data),
			      (SIM_ADDR) vu_addr, fq->source_address);

	      /* next vector please */
	      vector_num ++;
	    } /* vector transfer loop */
	} /* PKE FIFO full enough */
      else
	{
	  /* need to wait for another word */
	  next_pps_state = PKE_REG_STAT_PPS_WAIT;
	  /* retry this instruction next clock */
	}
    }
  /* ... */
  else
    {
      /* set ER1 flag in STAT register */
      PKE_REG_MASK_SET(me, STAT, ER1, 1);
      /* advance over faulty word */
      pke_pc_advance(me, 1);
    }

  /* PKE is now idle or waiting */
  PKE_REG_MASK_SET(me, STAT, PPS, next_pps_state);
}






/* advance the PC by given number of words; update STAT/FQC field */

void
pke_pc_advance(struct pke_device* me, int num_words)
{
  ASSERT(num_words > 0);

  me->qw_pc += num_words;
  /* handle overflow */
  while(me->qw_pc >= 4)
    {
      me->qw_pc -= 4;
      me->fifo_pc ++;
    }

  /* clear FQC if FIFO is now empty */ 
  if(me->fifo_num_elements == me->fifo_pc)
    {
      PKE_REG_MASK_SET(me, STAT, FQC, 0);
    }

}



/* Return pointer to given operand# in FIFO.  `word_num' starts at 1.
   If FIFO is not full enough, return 0. */

unsigned_4*
pke_pc_operand(struct pke_device* me, int word_num)
{
  int new_qw_pc, new_fifo_pc;
  unsigned_4* operand;

  ASSERT(word_num > 0);

  new_fifo_pc = me->fifo_pc;
  new_qw_pc += me->qw_pc + word_num;

  /* handle overflow */
  while(new_qw_pc >= 4)
    {
      new_qw_pc -= 4;
      new_fifo_pc ++;
    }

  /* not enough elements */
  if(me->fifo_num_elements == me->fifo_pc)
    operand = NULL;
  else
    operand = & me->fifo[new_fifo_pc].data[new_qw_pc];

  return operand;
}



/* Return pointer to FIFO quadword containing given operand# in FIFO.
   `word_num' starts at 1.  If FIFO is not full enough, return 0. */

struct fifo_quadword*
pke_pc_fifo(struct pke_device* me, int word_num)
{
  int new_qw_pc, new_fifo_pc;
  struct fifo_quadword* operand;

  ASSERT(word_num > 0);

  new_fifo_pc = me->fifo_pc;
  new_qw_pc += me->qw_pc + word_num;

  /* handle overflow */
  while(new_qw_pc >= 4)
    {
      new_qw_pc -= 4;
      new_fifo_pc ++;
    }

  /* not enough elements */
  if(me->fifo_num_elements == me->fifo_pc)
    operand = NULL;
  else
    operand = & me->fifo[new_fifo_pc];

  return operand;
}



/* Write a bunch of bytes into simulator memory.  Store the given source address into the
   PKE sourceaddr tracking word. */
int
pke_track_write(struct pke_device* me, const void* src, int len, 
		address_word dest, unsigned_4 sourceaddr)
{
  int rc;
  unsigned_4 no_sourceaddr = 0;

  /* write srcaddr into PKE srcaddr tracking */
  sim_write(NULL,
	    (SIM_ADDR) (me->pke_number == 0) ? PKE0_SRCADDR : PKE1_SRCADDR,
	    (void*) & sourceaddr,
	    sizeof(unsigned_4));
  
  /* write bytes into simulator */
  rc = sim_write(NULL,
		 (SIM_ADDR) dest,
		 (void*) src,
		 len);
  
  /* clear srcaddr from PKE srcaddr tracking */
  sim_write(NULL,
	    (SIM_ADDR) (me->pke_number == 0) ? PKE0_SRCADDR : PKE1_SRCADDR,
	    (void*) & no_sourceaddr,
	    sizeof(unsigned_4));

  return rc;
}
