/* Copyright (C) 1998, Cygnus Solutions */

#ifndef H_PKE_H
#define H_PKE_H

#include "sim-main.h"
#include "sky-device.h"



/* External functions */

void pke0_attach(SIM_DESC sd);
void pke0_issue(SIM_DESC sd);
void pke1_attach(SIM_DESC sd);
void pke1_issue(SIM_DESC sd);


/* structs declared below */
struct pke_fifo;
struct fifo_quadword;


/* Quadword data type */

typedef unsigned_4 quadword[4];

/* truncate address to quadword */
#define ADDR_TRUNC_QW(addr) ((addr) & ~0x0f)
/* extract offset in quadword */
#define ADDR_OFFSET_QW(addr) ((addr) & 0x0f)


/* SCEI memory mapping information */

#define PKE0_REGISTER_WINDOW_START 0x10003800
#define PKE1_REGISTER_WINDOW_START 0x10003C00
#define PKE0_FIFO_ADDR             0x10004000
#define PKE1_FIFO_ADDR             0x10005000


/* VU source-addr tracking tables */ /* changed from 1998-01-22 e-mail plans */
#define VU0_MEM0_SRCADDR_START 0x21000000
#define VU0_MEM1_SRCADDR_START 0x21004000
#define VU1_MEM0_SRCADDR_START 0x21008000
#define VU1_MEM1_SRCADDR_START 0x2100C000

#define VU0_CIA (VU0_REGISTER_WINDOW_START + VU_REG_CIA)
#define VU1_CIA (VU1_REGISTER_WINDOW_START + VU_REG_CIA)

/* GPUIF STAT register */
#define GPUIF_REG_STAT_APATH_E 11
#define GPUIF_REG_STAT_APATH_B 10

/* COP2 STAT register */
#define COP2_REG_STAT_ADDR VPU_STAT
#define COP2_REG_STAT_VBS1_E 8
#define COP2_REG_STAT_VBS1_B 8
#define COP2_REG_STAT_VBS0_E 0
#define COP2_REG_STAT_VBS0_B 0


/* Quadword indices of PKE registers.  Actual registers sit at bottom
   32 bits of each quadword. */
#define PKE_REG_STAT    0x00
#define PKE_REG_FBRST   0x01
#define PKE_REG_ERR     0x02
#define PKE_REG_MARK    0x03
#define PKE_REG_CYCLE   0x04
#define PKE_REG_MODE    0x05
#define PKE_REG_NUM     0x06
#define PKE_REG_MASK    0x07
#define PKE_REG_CODE    0x08
#define PKE_REG_ITOPS   0x09
#define PKE_REG_BASE    0x0a /* pke1 only */
#define PKE_REG_OFST    0x0b /* pke1 only */
#define PKE_REG_TOPS    0x0c /* pke1 only */
#define PKE_REG_ITOP    0x0d
#define PKE_REG_TOP     0x0e /* pke1 only */
#define PKE_REG_DBF     0x0f /* pke1 only */
#define PKE_REG_R0      0x10 /* R0 .. R3 must be contiguous */
#define PKE_REG_R1      0x11
#define PKE_REG_R2      0x12
#define PKE_REG_R3      0x13
#define PKE_REG_C0      0x14 /* C0 .. C3 must be contiguous */
#define PKE_REG_C1      0x15
#define PKE_REG_C2      0x16
#define PKE_REG_C3      0x17
/* one plus last index */
#define PKE_NUM_REGS    0x18

#define PKE_REGISTER_WINDOW_SIZE  (sizeof(quadword) * PKE_NUM_REGS)



/* PKE commands */

#define PKE_CMD_PKENOP_MASK 0x7F
#define PKE_CMD_PKENOP_BITS 0x00
#define PKE_CMD_STCYCL_MASK 0x7F
#define PKE_CMD_STCYCL_BITS 0x01
#define PKE_CMD_OFFSET_MASK 0x7F
#define PKE_CMD_OFFSET_BITS 0x02
#define PKE_CMD_BASE_MASK 0x7F
#define PKE_CMD_BASE_BITS 0x03
#define PKE_CMD_ITOP_MASK 0x7F
#define PKE_CMD_ITOP_BITS 0x04
#define PKE_CMD_STMOD_MASK 0x7F
#define PKE_CMD_STMOD_BITS 0x05
#define PKE_CMD_MSKPATH3_MASK 0x7F
#define PKE_CMD_MSKPATH3_BITS 0x06
#define PKE_CMD_PKEMARK_MASK 0x7F
#define PKE_CMD_PKEMARK_BITS 0x07
#define PKE_CMD_FLUSHE_MASK 0x7F
#define PKE_CMD_FLUSHE_BITS 0x10
#define PKE_CMD_FLUSH_MASK 0x7F
#define PKE_CMD_FLUSH_BITS 0x11
#define PKE_CMD_FLUSHA_MASK 0x7F
#define PKE_CMD_FLUSHA_BITS 0x13
#define PKE_CMD_PKEMSCAL_MASK 0x7F  /* CAL == "call" */
#define PKE_CMD_PKEMSCAL_BITS 0x14
#define PKE_CMD_PKEMSCNT_MASK 0x7F  /* CNT == "continue" */
#define PKE_CMD_PKEMSCNT_BITS 0x17
#define PKE_CMD_PKEMSCALF_MASK 0x7F /* CALF == "call after flush" */
#define PKE_CMD_PKEMSCALF_BITS 0x15
#define PKE_CMD_STMASK_MASK 0x7F
#define PKE_CMD_STMASK_BITS 0x20
#define PKE_CMD_STROW_MASK 0x7F
#define PKE_CMD_STROW_BITS 0x30
#define PKE_CMD_STCOL_MASK 0x7F
#define PKE_CMD_STCOL_BITS 0x31
#define PKE_CMD_MPG_MASK 0x7F
#define PKE_CMD_MPG_BITS 0x4A
#define PKE_CMD_DIRECT_MASK 0x7F
#define PKE_CMD_DIRECT_BITS 0x50
#define PKE_CMD_DIRECTHL_MASK 0x7F
#define PKE_CMD_DIRECTHL_BITS 0x51
#define PKE_CMD_UNPACK_MASK 0x60
#define PKE_CMD_UNPACK_BITS 0x60

/* test given word for particular PKE command bit pattern */
#define IS_PKE_CMD(word,cmd) (((word) & PKE_CMD_##cmd##_MASK) == PKE_CMD_##cmd##_BITS)


/* register bitmasks: bit numbers for end and beginning of fields */

/* PKE opcode */
#define PKE_OPCODE_I_E 31
#define PKE_OPCODE_I_B 31
#define PKE_OPCODE_CMD_E 30
#define PKE_OPCODE_CMD_B 24
#define PKE_OPCODE_NUM_E 23
#define PKE_OPCODE_NUM_B 16
#define PKE_OPCODE_IMM_E 15
#define PKE_OPCODE_IMM_B 0

/* STAT register */
#define PKE_REG_STAT_FQC_E 28
#define PKE_REG_STAT_FQC_B 24
#define PKE_REG_STAT_FDR_E 23
#define PKE_REG_STAT_FDR_B 23
#define PKE_REG_STAT_ER1_E 13
#define PKE_REG_STAT_ER1_B 13
#define PKE_REG_STAT_ER0_E 12
#define PKE_REG_STAT_ER0_B 12
#define PKE_REG_STAT_INT_E 11
#define PKE_REG_STAT_INT_B 11
#define PKE_REG_STAT_PIS_E 10
#define PKE_REG_STAT_PIS_B 10
#define PKE_REG_STAT_PFS_E 9
#define PKE_REG_STAT_PFS_B 9
#define PKE_REG_STAT_PSS_E 8
#define PKE_REG_STAT_PSS_B 8
#define PKE_REG_STAT_DBF_E 7
#define PKE_REG_STAT_DBF_B 7
#define PKE_REG_STAT_MRK_E 6
#define PKE_REG_STAT_MRK_B 6
#define PKE_REG_STAT_PGW_E 3
#define PKE_REG_STAT_PGW_B 3
#define PKE_REG_STAT_PEW_E 2
#define PKE_REG_STAT_PEW_B 2
#define PKE_REG_STAT_PPS_E 1
#define PKE_REG_STAT_PPS_B 0

#define PKE_REG_STAT_PPS_IDLE 0x00 /* ready to execute next instruction */
#define PKE_REG_STAT_PPS_WAIT 0x01 /* not enough words in FIFO */
#define PKE_REG_STAT_PPS_DECODE 0x02 /* decoding instruction */
#define PKE_REG_STAT_PPS_STALL 0x02 /* alias state for stall (e.g., FLUSHE) */
#define PKE_REG_STAT_PPS_XFER 0x03 /* transferring instruction operands */

/* DBF register */
#define PKE_REG_DBF_DF_E 0
#define PKE_REG_DBF_DF_B 0

/* OFST register */
#define PKE_REG_OFST_OFFSET_E 9
#define PKE_REG_OFST_OFFSET_B 0

/* OFST register */
#define PKE_REG_TOPS_TOPS_E 9
#define PKE_REG_TOPS_TOPS_B 0

/* BASE register */
#define PKE_REG_BASE_BASE_E 9
#define PKE_REG_BASE_BASE_B 0

/* ITOPS register */
#define PKE_REG_ITOPS_ITOPS_E 9
#define PKE_REG_ITOPS_ITOPS_B 0

/* MODE register */
#define PKE_REG_MODE_MDE_E 1
#define PKE_REG_MODE_MDE_B 0

/* NUM register */
#define PKE_REG_NUM_NUM_E 9
#define PKE_REG_NUM_NUM_B 0

/* MARK register */
#define PKE_REG_MARK_MARK_E 15
#define PKE_REG_MARK_MARK_B 0

/* ITOP register */
#define PKE_REG_ITOP_ITOP_E 9
#define PKE_REG_ITOP_ITOP_B 0

/* TOP register */
#define PKE_REG_TOP_TOP_E 9
#define PKE_REG_TOP_TOP_B 0

/* MASK register */
#define PKE_REG_MASK_MASK_E 31
#define PKE_REG_MASK_MASK_B 0

/* CYCLE register */
#define PKE_REG_CYCLE_WL_E 15
#define PKE_REG_CYCLE_WL_B 8
#define PKE_REG_CYCLE_CL_E 7
#define PKE_REG_CYCLE_CL_B 0

/* ERR register */
#define PKE_REG_ERR_ME1_E 2
#define PKE_REG_ERR_ME1_B 2
#define PKE_REG_ERR_ME0_E 1
#define PKE_REG_ERR_ME0_B 1
#define PKE_REG_ERR_MII_E 0
#define PKE_REG_ERR_MII_B 0

/* FBRST command bitfields */
#define PKE_REG_FBRST_STC_E 3
#define PKE_REG_FBRST_STC_B 3
#define PKE_REG_FBRST_STP_E 2
#define PKE_REG_FBRST_STP_B 2
#define PKE_REG_FBRST_FBK_E 1
#define PKE_REG_FBRST_FBK_B 1
#define PKE_REG_FBRST_RST_E 0
#define PKE_REG_FBRST_RST_B 0

/* MSKPATH3 command bitfields */
#define PKE_REG_MSKPATH3_E 15
#define PKE_REG_MSKPATH3_B 15


/* UNPACK opcodes */
#define PKE_UNPACK(vn,vl) ((vn) << 2 | (vl))
#define PKE_UNPACK_S_32  PKE_UNPACK(0, 0)
#define PKE_UNPACK_S_16  PKE_UNPACK(0, 1)
#define PKE_UNPACK_S_8   PKE_UNPACK(0, 2)
#define PKE_UNPACK_V2_32 PKE_UNPACK(1, 0)
#define PKE_UNPACK_V2_16 PKE_UNPACK(1, 1)
#define PKE_UNPACK_V2_8  PKE_UNPACK(1, 2)
#define PKE_UNPACK_V3_32 PKE_UNPACK(2, 0)
#define PKE_UNPACK_V3_16 PKE_UNPACK(2, 1)
#define PKE_UNPACK_V3_8  PKE_UNPACK(2, 2)
#define PKE_UNPACK_V4_32 PKE_UNPACK(3, 0)
#define PKE_UNPACK_V4_16 PKE_UNPACK(3, 1)
#define PKE_UNPACK_V4_8  PKE_UNPACK(3, 2)
#define PKE_UNPACK_V4_5  PKE_UNPACK(3, 3)


/* MASK register sub-field definitions */
#define PKE_MASKREG_INPUT 0
#define PKE_MASKREG_ROW 1
#define PKE_MASKREG_COLUMN 2
#define PKE_MASKREG_NOTHING 3


/* STMOD register field definitions */
#define PKE_MODE_INPUT 0
#define PKE_MODE_ADDROW 1
#define PKE_MODE_ACCROW 2


/* extract a MASK register sub-field for row [0..3] and column [0..3] */
/* MASK register is laid out of 2-bit values in this r-c order */
/* m33 m32 m31 m30 m23 m22 m21 m20 m13 m12 m11 m10 m03 m02 m01 m00 */
#define PKE_MASKREG_GET(me,row,col) \
((((me)->regs[PKE_REG_MASK][0]) >> (8*(row) + 2*(col))) & 0x03)


/* operations - replace with those in sim-bits.h when convenient */

/* unsigned 32-bit mask of given width */
#define BIT_MASK(width) ((width) == 31 ? 0xffffffff : (((unsigned_4)1) << (width+1)) - 1)
/* e.g.: BIT_MASK(4) = 00011111 */

/* mask between given given bits numbers (MSB) */
#define BIT_MASK_BTW(begin,end) ((BIT_MASK(end) & ~((begin) == 0 ? 0 : BIT_MASK((begin)-1))))
/* e.g.: BIT_MASK_BTW(4,11) = 0000111111110000 */

/* set bitfield value */
#define BIT_MASK_SET(lvalue,begin,end,value) \
do { \
  ASSERT((begin) <= (end)); \
  (lvalue) &= ~BIT_MASK_BTW((begin),(end)); \
  (lvalue) |= ((value) << (begin)) & BIT_MASK_BTW((begin),(end)); \
} while(0)

/* get bitfield value */
#define BIT_MASK_GET(rvalue,begin,end) \
  (((rvalue) & BIT_MASK_BTW(begin,end)) >> (begin))
/* e.g., BIT_MASK_GET(0000111100001111, 2, 8) = 0000000100001100 */

/* These ugly macro hacks allow succinct bitfield accesses */
/* set a bitfield in a register by "name" */
#define PKE_REG_MASK_SET(me,reg,flag,value) \
     do { \
       unsigned_4 old = BIT_MASK_GET(((me)->regs[PKE_REG_##reg][0]), \
		    PKE_REG_##reg##_##flag##_B, PKE_REG_##reg##_##flag##_E); \
       BIT_MASK_SET(((me)->regs[PKE_REG_##reg][0]), \
		    PKE_REG_##reg##_##flag##_B, PKE_REG_##reg##_##flag##_E, \
		    (value)); \
       if((me)->fifo_trace_file != NULL) \
	 { \
	   if(old != (value)) \
	     fprintf((me)->fifo_trace_file, "# Reg %s:%s = 0x%x\n", #reg, #flag, (unsigned)(value)); \
	 } \
     } while(0)

/* get a bitfield from a register by "name" */
#define PKE_REG_MASK_GET(me,reg,flag) \
     BIT_MASK_GET(((me)->regs[PKE_REG_##reg][0]), \
                  PKE_REG_##reg##_##flag##_B, PKE_REG_##reg##_##flag##_E)


#define PKE_LIMIT(value,max) ((value) > (max) ? (max) : (value))


/* Classify words in a FIFO quadword */
enum wordclass
{
  wc_dma = 'D',
  wc_pkecode = 'P',
  wc_unknown = '?',
  wc_pkedata = '.',
  wc_gpuiftag = 'g'
};


/* One row in the FIFO */
struct fifo_quadword
{
  /* 128 bits of data */
  quadword data;
  /* source main memory address (or 0: unknown) */
  unsigned_4 source_address;
  /* classification of words in quadword; wc_dma set on DMA tags at FIFO write */
  enum wordclass word_class[4];
};


/* quadword FIFO structure for PKE */ 
typedef struct pke_fifo
{
  struct fifo_quadword** quadwords; /* pointer to fifo quadwords */
  unsigned_4 origin; /* quadword serial number of quadwords[0] */
  unsigned_4 length; /* length of quadword pointer array: 0..N */
  unsigned_4 next;   /* relative index of first unfilled quadword: 0..length-1 */
} pke_fifo;

#define PKE_FIFO_GROW_SIZE 1000 /* number of quadword pointers to allocate */
#define PKE_FIFO_ARCHEOLOGY 1000 /* number of old quadwords to keep as history */


/* PKE internal state: FIFOs, registers, handle to VU friend */
struct pke_device
{
  /* common device info */
  device dev;

  /* identity: 0=PKE0, 1=PKE1 */
  int pke_number;
  int flags;

  /* quadword registers: data in [0] word only */
  quadword regs[PKE_NUM_REGS];

  /* write buffer for FIFO address */
  quadword fifo_qw_in_progress;
  int fifo_qw_done; /* bitfield */

  /* FIFO - private: use only pke_fifo_* routines to access */
  struct pke_fifo fifo;  /* array of FIFO quadword pointers */
  FILE* fifo_trace_file; /* stdio stream open in append mode, or 0 for no trace */

  /* FIFO cache -- curry last search pke_pcrel_fifo results */
  unsigned_4 last_fifo_pc;
  unsigned_4 last_qw_pc;
  unsigned_4 last_num;
  unsigned_4 last_new_fifo_pc;
  unsigned_4 last_new_qw_pc;

  /* PC */
  int fifo_pc;  /* 0 .. (fifo_num_elements-1): quadword index of next instruction */
  int qw_pc;    /* 0 .. 3:                     word index of next instruction */
};

extern struct pke_device pke0_device;
extern struct pke_device pke1_device;

int read_pke_reg (struct pke_device *device, int regno, void *buf);
int write_pke_reg (struct pke_device *device, int regno, const void *buf);
int read_pke_pc (struct pke_device *device, void *buf);


/* Flags for PKE.flags */

#define PKE_FLAG_NONE        0x00
#define PKE_FLAG_PENDING_PSS 0x01 /* PSS bit written-to; set STAT:PSS after current instruction */
#define PKE_FLAG_INT_NOLOOP  0x02 /* INT PKEcode received; INT/PIS set; suppress loop after resumption */


/* Kludge alert */

#define PKE_MEM_READ(me,addr,data,size) \
    do { \
      sim_cpu* cpu = STATE_CPU(CURRENT_STATE, 0); \
      unsigned_##size value = \
	sim_core_read_aligned_##size(cpu, CIA_GET(cpu), read_map, \
				     (SIM_ADDR)(addr)); \
      memcpy((unsigned_##size*) (data), (void*) & value, size); \
     } while(0)

#define PKE_MEM_WRITE(me,addr,data,size) \
    do { sim_cpu* cpu = STATE_CPU(CURRENT_STATE, 0); \
         unsigned_##size value; \
         memcpy((void*) & value, (unsigned_##size*)(data), size); \
         sim_core_write_aligned_##size(cpu, CIA_GET(cpu), write_map, \
				       (SIM_ADDR)(addr), value); \
         if((me)->fifo_trace_file != NULL) \
	   { \
	     int i; \
	     unsigned_##size value_te; \
	     value_te = H2T_##size(value); \
	     fprintf((me)->fifo_trace_file, "# Write %2d bytes  to  ", size); \
	     fprintf((me)->fifo_trace_file, "0x%08lx: ", (unsigned long)(addr)); \
	     for(i=0; i<size; i++) \
	       fprintf((me)->fifo_trace_file, " %02x", ((unsigned_1*)(& value_te))[i]); \
 	     fprintf((me)->fifo_trace_file, "\n"); \
	   } \
        } while(0)      



#endif /* H_PKE_H */
