/* Copyright (C) 1998, Cygnus Solutions */

#ifndef H_PKE_H
#define H_PKE_H

#include "sim-main.h"
#include "sky-device.h"


/* External functions */

void pke0_attach(SIM_DESC sd);
void pke0_issue();
void pke1_attach(SIM_DESC sd);
void pke1_issue();


/* Quadword data type */

typedef unsigned int quadword[4];

/* truncate address to quadword */
#define ADDR_TRUNC_QW(addr) ((addr) & ~0x0f)
/* extract offset in quadword */
#define ADDR_OFFSET_QW(addr) ((addr) & 0x0f)


/* SCEI memory mapping information */

#define PKE0_REGISTER_WINDOW_START 0x10000800
#define PKE1_REGISTER_WINDOW_START 0x10000A00
#define PKE0_FIFO_START            0x10008000
#define PKE1_FIFO_START            0x10008010


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
#define PKE_REG_R0      0x10
#define PKE_REG_R1      0x11
#define PKE_REG_R2      0x12
#define PKE_REG_R3      0x13
#define PKE_REG_C0      0x14
#define PKE_REG_C1      0x15
#define PKE_REG_C2      0x16
#define PKE_REG_C3      0x17
/* one plus last index */
#define PKE_NUM_REGS    0x18

#define PKE_REGISTER_WINDOW_SIZE  (sizeof(quadword) * PKE_NUM_REGS)

/* virtual addresses for source-addr tracking */
#define PKE0_SRCADDR    0x20000020
#define PKE1_SRCADDR    0x20000024


/* One row in the FIFO */
struct fifo_quadword
{
  /* 128 bits of data */
  quadword data;
  /* source main memory address (or 0: unknown) */
  address_word source_address;
};


/* PKE internal state: FIFOs, registers, handle to VU friend */
struct pke_device
{
  /* common device info */
  device dev;

  /* identity: 0=PKE0, 1=PKE1 */
  int pke_number;
  int flags;

  address_word register_memory_addr;
  address_word fifo_memory_addr;

  /* quadword registers */
  quadword regs[PKE_NUM_REGS];

  /* FIFO */
  struct fifo_quadword* fifo;
  int fifo_num_elements; /* no. of quadwords occupied in FIFO */
  int fifo_buffer_size;  /* no. of quadwords of space in FIFO */
  FILE* fifo_trace_file; /* or 0 for no trace */

  /* index into FIFO of current instruction */
  int program_counter; 

};


/* Flags for PKE.flags */

#define PKE_FLAG_NONE 0
/* none at present */



#endif /* H_PKE_H */
