/* This program is free software; you can redistribute it and/or modify
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

/* MIPS running RISC/os 4.52C.  */

#define PCB_OFFSET(FIELD) ((int)&((struct user*)0)->u_pcb.FIELD)

#include <bsd43/machine/vmparam.h>
#define NBPG BSD43_NBPG
#define UPAGES BSD43_UPAGES

#define REGISTER_U_ADDR(addr, blockend, regno) 		\
	      if (regno < FP0_REGNUM) \
		  addr =  UPAGES*NBPG-EF_SIZE+4*((regno)+EF_AT-1); \
	      else if (regno < PC_REGNUM) \
		  addr = PCB_OFFSET(pcb_fpregs[0]) + 4*(regno-FP0_REGNUM); \
              else if (regno == PS_REGNUM) \
                  addr = UPAGES*NBPG-EF_SIZE+4*EF_SR; \
              else if (regno == BADVADDR_REGNUM) \
  		  addr = UPAGES*NBPG-EF_SIZE+4*EF_BADVADDR; \
	      else if (regno == LO_REGNUM) \
		  addr = UPAGES*NBPG-EF_SIZE+4*EF_MDLO; \
	      else if (regno == HI_REGNUM) \
		  addr = UPAGES*NBPG-EF_SIZE+4*EF_MDHI; \
	      else if (regno == CAUSE_REGNUM) \
		  addr = UPAGES*NBPG-EF_SIZE+4*EF_CAUSE; \
	      else if (regno == PC_REGNUM) \
		  addr = UPAGES*NBPG-EF_SIZE+4*EF_EPC; \
              else if (regno < FCRCS_REGNUM) \
		  addr = PCB_OFFSET(pcb_fpregs[0]) + 4*(regno-FP0_REGNUM); \
	      else if (regno == FCRCS_REGNUM) \
		  addr = PCB_OFFSET(pcb_fpc_csr); \
	      else if (regno == FCRIR_REGNUM) \
		  addr = PCB_OFFSET(pcb_fpc_eir); \
              else \
                  addr = 0;

#include "mips/nm-mips.h"

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */
#define FETCH_INFERIOR_REGISTERS
