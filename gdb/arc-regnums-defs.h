/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com> 

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#ifdef RAUX
#ifdef ARC4_JTAG

RAUX ( STATUS , 0x0, "Status Regnum " , ARC_STATUS_REGNUM , A4 )
RAUX ( SEMAPHORE ,  0x1, "Semaphore Regnum ", ARC_SEMAPHORE_REGNUM , A4  ) 
RAUX ( LP_START , 0x2, "Loop Start" , ARC_LP_START_REGNUM , A4  )
RAUX ( LP_END , 0x3, "Loop End", ARC_LP_END_REGNUM , A4 )
RAUX ( IDENTITY , 0x4, "Identity", ARC_IDENTITY_REGNUM , A4 )
RAUX ( DEBUG , 0x5, "Debug" , ARC_DEBUG_REGNUM , A4 )

#else

RAUX ( STATUS , 0x0, "Status Regnum (obsolete)" , ARC_STATUS_REGNUM , ARCompact )
RAUX ( SEMAPHORE ,  0x1, "Semaphore Regnum ", ARC_SEMAPHORE_REGNUM , ARCompact  ) 
RAUX ( LP_START , 0x2, "Loop Start" , ARC_LP_START_REGNUM , ARCompact  )
RAUX ( LP_END , 0x3, "Loop End", ARC_LP_END_REGNUM , ARCompact )
RAUX ( IDENTITY , 0x4, "Identity", ARC_IDENTITY_REGNUM , ARCompact )
RAUX ( DEBUG , 0x5, "Debug" , ARC_DEBUG_REGNUM , ARCompact )
RAUX ( PC, 0x6,"PC" , ARC_PC_REGNUM ,ARCompact )
RAUX ( STATUS32 ,0xA,"STATUS32", ARC_STATUS32_REGNUM , ARCompact ) 
RAUX ( STATUS32_L1 , 0xB, "STATUS32 register in case of L1 interrupts" ,ARC_STATUS32_L1_REGNUM , ARCompact )
RAUX ( STATUS32_L2 , 0xC, "STATUS32 register in case of L2 interrupts" ,ARC_STATUS32_L2_REGNUM , ARCompact )
RAUX ( COUNT0 , 0x21, "Processor Timer 1 Count Value", ARC_COUNT0_REGNUM , ARCompact )
RAUX ( CONTROL0 , 0x22, "Processor Timer 1 Control Value" , ARC_CONTROL0_REGNUM, ARCompact)
RAUX ( LIMIT0 , 0x23, "Processor Timer 1 Limit Value" , ARC_LIMIT0_REGNUM, ARCompact )
RAUX ( INT_VECTOR_BASE , 0x25, "Interrupt Vector Base Register", ARC_INT_VECTOR_BASE_REGNUM , ARCompact )
RAUX ( AUX_IRQ_MACMODE , 0x41, "Aux IRQ MAC Mode " , ARC_AUX_MACMODE_REGNUM , ARCompact )
RAUX ( AUX_IRQ_LV12 , 0x42, "Aux IRQ Level 2 " , ARC_AUX_IRQ_LV12_REGNUM , ARCompact )
RAUX ( COUNT1 , 0x100, "Processor Timer 1 Count Value", ARC_COUNT1_REGNUM , ARCompact )
RAUX ( CONTROL1 , 0x101, "Processor Timer 1 Control Value" , ARC_CONTROL1_REGNUM , ARCompact)
RAUX ( LIMIT1 , 0x102, "Processor Timer 1 Limit Value", ARC_LIMIT1_REGNUM , ARCompact)
RAUX ( AUX_IRQ_LEV , 0x200, "Interrupt Level programming. ", ARC_AUX_IRQ_LEV_REGNUM , ARCompact)
RAUX ( AUX_IRQ_HINT , 0x201, "Software Triggered Interrupt" , ARC_AUX_IRQ_HINT_REGNUM, ARCompact )
RAUX ( ERET , 0x400, "Exception Return " , ARC_ERET_REGNUM, ARC700 )
RAUX ( ERBTA , 0x401, "Exception BTA ", ARC_ERBTA_REGNUM, ARC700 )
RAUX ( ERSTATUS , 0x402, "Exception Return Status" , ARC_ERSTATUS_REGNUM, ARC700 )
RAUX ( ECR , 0x403, "Exception Cause Register" , ARC_ECR_REGNUM, ARC700 )
RAUX ( EFA , 0x404, "Exception Fault Address" , ARC_EFA_REGNUM, ARC700 )
RAUX ( ICAUSE1 , 0x40A, "Interrupt Cause (Level 1)", ARC_ICAUSE1_REGNUM,ARC700 )
RAUX ( ICAUSE2 , 0x40B, "Interrupt Cause (Level 2)", ARC_ICAUSE2_REGNUM, ARC700)
RAUX ( AUX_IENABLE , 0x40C, "Interrupt Mask Programming", ARC_AUX_IENABLE_REGNUM, ARC700 )
RAUX ( AUX_ITRIGGER , 0x40D, "Interrupt Sensitivity Programming", ARC_AUX_ITRIGGER_REGNUM, ARC700 )
RAUX ( XPU , 0x410, "User Mode Extension Permissions", ARC_XPU_REGNUM, ARC700 )
RAUX ( BTA , 0x412, "Branch Target Address", ARC_BTA_REGNUM, ARC700 )
RAUX ( BTA_L1 , 0x413, "Branch Target Address in Level 1", ARC_BTA_L1_REGNUM, ARC700 )
RAUX ( BTA_L2 , 0x414, "Branch Target Address in Level 2", ARC_BTA_L2_REGNUM, ARC700 )
RAUX ( AUX_IRQ_PULSE_CANCEL , 0x415, "Interrupt Pulse Cancel", ARC_AUX_IRQ_PULSE_CANCEL_REGNUM, ARC700 )
RAUX ( AUX_IRQ_PENDING , 0x416, "Interrupt Pending Register", ARC_AUX_IRQ_PENDING_REGNUM, ARC700 )
#endif // ARC4_JTAG
#endif // RAUX

#ifdef RBCR
#ifndef ARC4_JTAG

RBCR ( DCCM_BASE_BUILD , 0x61, "Base address for DCCM.", ARC_BCR_1_REGNUM, ARCompact)
RBCR ( CRC_BASE_BUILD , 0x62, "BCRBCR for CRC Unit.", ARC_BCR_2_REGNUM , ARCompact)
RBCR ( BTA_LINK_BUILD , 0x63, "Interrupt Link Registers Available for BTA",ARC_BCR_3_REGNUM, ARCompact )
RBCR ( DVBF_BUILD , 0x64, "BCRBCR for Dual Viterbi Instruction.",ARC_BCR_4_REGNUM, ARCompact )
RBCR ( TEL_INSTR_BUILD , 0x65, "BCRBCR for Extended Arithmetic Instructions. ",ARC_BCR_5_REGNUM, ARCompact)
RBCR ( MEMSUBSYS , 0x67, "BCRBCR for Memory Subsystem. ",ARC_BCR_7_REGNUM, ARCompact)
RBCR ( VECBASE_AC_BUILD ,0x68, "BCRBCR for Interrupt Vector Base. ", ARC_BCR_8_REGNUM,ARCompact) 
RBCR ( P_BASE_ADDRESS , 0x69, "Peripheral Base Address" , ARC_BCR_9_REGNUM , ARCompact) 
RBCR ( MMU_BUILD , 0x6F, "MMU Build. " , ARC_BCR_F_REGNUM, ARCompact)
RBCR ( ARCANGEL_BUILD , 0x70, "ARC Angel Build config. ", ARC_BCR_10_REGNUM, ARCompact)
RBCR ( D_CACHE_BUILD , 0x72, "D Cache Build Config. ", ARC_BCR_12_REGNUM , ARCompact )
RBCR ( MADI_BUILD , 0x73 , "Multiple ARC Debug Interface. " , ARC_BCR_13_REGNUM , ARCompact)
RBCR ( DCCM_BUILD , 0x74, "BCRBCR for DCCM.(Data Closely coupled Memory", ARC_BCR_14_REGNUM, ARCompact)
RBCR ( TIMER_BUILD , 0x75, "BCRBCR for Timers. " , ARC_BCR_15_REGNUM , ARCompact)
RBCR ( AP_BUILD, 0x76, "Actionpoints build. ", ARC_BCR_16_REGNUM , ARCompact )
RBCR ( ICACHE_BUILD , 0x77, "Instruction Cache BCR", ARC_BCR_17_REGNUM , ARCompact )
RBCR ( ICCM_BUILD , 0x78, "ICCM BCRBCR (Instruction Closely Coupled Memory.", ARC_BCR_18_REGNUM , ARCompact)
RBCR ( DSPRAM_BUILD , 0x79, "DSP RAM Build", ARC_BCR_19_REGNUM , ARCompact)
RBCR ( MAC_BUILD , 0x7A, "MAC Unit Build", ARC_BCR_1A_REGNUM , ARCompact)
RBCR ( MULTIPLY_BUILD , 0x7B, "(32 X 32) Multiply Unit Build", ARC_BCR_1B_REGNUM , ARCompact)
RBCR ( SWAP_BUILD , 0x7C, "SWAP Build", ARC_BCR_1C_REGNUM , ARCompact)
RBCR ( NORM_BUILD , 0x7D ,"NORM Unit Build", ARC_BCR_1D_REGNUM, ARCompact)
RBCR ( MINMAX_BUILD , 0x7E, "Minmax Unit Build", ARC_BCR_1E_REGNUM, ARCompact)
RBCR ( BARREL_BUILD , 0x7F, "Barrel Shifter Build", ARC_BCR_1F_REGNUM , ARCompact) 


#endif // ARC4_JTAG
#endif // RBCR 
