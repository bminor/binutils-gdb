/* Macro definitions for GDB on an S390.
   Copyright 2001 Free Software Foundation, Inc.
   Contributed by D.J. Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
   for IBM Deutschland Entwicklung GmbH, IBM Corporation.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#if !defined(TM_S390_H)
#define TM_S390_H 1

#define S390_NUM_GPRS      (16)
#define S390_GPR_SIZE      DEPRECATED_REGISTER_SIZE
#define S390_PSW_MASK_SIZE DEPRECATED_REGISTER_SIZE
#define S390_PSW_ADDR_SIZE DEPRECATED_REGISTER_SIZE
#define S390_NUM_FPRS      (16)
#define S390_FPR_SIZE      (8)
#define S390_FPC_SIZE      (4)
#define S390_FPC_PAD_SIZE  (4)	/* gcc insists on aligning the fpregs */
#define S390_NUM_CRS       (16)
#define S390_CR_SIZE       DEPRECATED_REGISTER_SIZE
#define S390_NUM_ACRS      (16)
#define S390_ACR_SIZE      (4)

#define S390_NUM_REGS      (2+S390_NUM_GPRS+S390_NUM_ACRS+S390_NUM_CRS+1+S390_NUM_FPRS)
#define S390_FIRST_ACR     (2+S390_NUM_GPRS)
#define S390_LAST_ACR      (S390_FIRST_ACR+S390_NUM_ACRS-1)
#define S390_FIRST_CR      (S390_FIRST_ACR+S390_NUM_ACRS)
#define S390_LAST_CR       (S390_FIRST_CR+S390_NUM_CRS-1)

#define S390_PSWM_REGNUM    0
#define S390_PC_REGNUM      1
#define	S390_GP0_REGNUM     2	/* GPR register 0 */
#define S390_GP_LAST_REGNUM (S390_GP0_REGNUM+S390_NUM_GPRS-1)
/* Usually return address */
#define S390_RETADDR_REGNUM (S390_GP0_REGNUM+14)
/* Contains address of top of stack */
#define S390_SP_REGNUM      (S390_GP0_REGNUM+15)
/* needed in findvar.c still */
#define S390_FP_REGNUM     S390_SP_REGNUM
#define S390_FRAME_REGNUM  (S390_GP0_REGNUM+11)
#define S390_FPC_REGNUM    (S390_GP0_REGNUM+S390_NUM_GPRS+S390_NUM_ACRS+S390_NUM_CRS)
/* FPR (Floating point) register 0 */
#define S390_FP0_REGNUM    (S390_FPC_REGNUM+1)
/* Last floating point register */
#define S390_FPLAST_REGNUM (S390_FP0_REGNUM+S390_NUM_FPRS-1)
#define S390_LAST_REGNUM   S390_FPLAST_REGNUM


#define S390_ACR0_OFFSET ((S390_PSW_MASK_SIZE+S390_PSW_ADDR_SIZE)+(S390_GPR_SIZE*S390_NUM_GPRS))
#define S390_CR0_OFFSET (S390_ACR0_OFFSET+(S390_ACR_SIZE*S390_NUM_ACRS))
#define S390_FPC_OFFSET (S390_CR0_OFFSET+(S390_CR_SIZE*S390_NUM_CRS))
#define S390_FP0_OFFSET (S390_FPC_OFFSET+(S390_FPC_SIZE+S390_FPC_PAD_SIZE))
#define S390_GPR6_STACK_OFFSET (GDB_TARGET_IS_ESAME ? 48:24)

#define S390_REGISTER_BYTES ((4+4)+(4*S390_NUM_GPRS)+(4*S390_NUM_ACRS)+ \
(4*S390_NUM_CRS)+(S390_FPC_SIZE+S390_FPC_PAD_SIZE)+(S390_FPR_SIZE*S390_NUM_FPRS))

#define S390X_REGISTER_BYTES ((8+8)+(8*S390_NUM_GPRS)+(4*S390_NUM_ACRS)+ \
(8*S390_NUM_CRS)+(S390_FPC_SIZE+S390_FPC_PAD_SIZE)+(S390_FPR_SIZE*S390_NUM_FPRS))

#define GDB_TARGET_IS_ESAME (TARGET_ARCHITECTURE->mach == bfd_mach_s390_64)

#endif /* ifndef TM_S390_H */
