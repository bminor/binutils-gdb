/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors:
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com>
      Richard Stuckey      <richard.stuckey@arc.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This file defines register numbers for ARC processors.                 */
/*                                                                            */
/* N.B. this file is NOT a module header file and must NOT have an inclusion  */
/*      guard: it may be included at multiple points in a module.             */
/*                                                                            */
/*                      THIS FILE IS NO LONGER USED!!!!                       */
/*                                                                            */
/******************************************************************************/

#ifndef STANDALONE_TEST
#error This file is no longer used
#endif

/* Auxiliary Registers.  */

#ifdef RAUX

RAUX ( STATUS,               0x0,   "Status (obsolete)",                          ARC_STATUS_REGNUM,                      0xFEFFFFFF,   RO,   ARCompact )
RAUX ( SEMAPHORE,            0x1,   "Semaphore",                                  ARC_SEMAPHORE_REGNUM,                   0x0000000F,   RW,   ARCompact )
RAUX ( LP_START,             0x2,   "Loop Start",                                 ARC_LP_START_REGNUM,                    0xFFFFFFFE,   RW,   ARCompact )
RAUX ( LP_END,               0x3,   "Loop End",                                   ARC_LP_END_REGNUM,                      0xFFFFFFFE,   RW,   ARCompact )
RAUX ( IDENTITY,             0x4,   "Identity",                                   ARC_IDENTITY_REGNUM,                    0xFFFFFFFF,   RO,   ARCompact )
RAUX ( DEBUG,                0x5,   "Debug",                                      ARC_DEBUG_REGNUM,                       0xF0800802,   RO,   ARCompact )
RAUX ( PC,                   0x6,   "PC",                                         ARC_PC_REGNUM,                          0xFFFFFFFE,   RO,   ARCompact )
RAUX ( STATUS32,             0xA,   "STATUS32",                                   ARC_STATUS32_REGNUM,                    0x00001FFF,   RO,   ARCompact )
RAUX ( STATUS32_L1,          0xB,   "STATUS32 register in case of L1 interrupts", ARC_STATUS32_L1_REGNUM,                 0x00001FFE,   RW,   ARCompact )
RAUX ( STATUS32_L2,          0xC,   "STATUS32 register in case of L2 interrupts", ARC_STATUS32_L2_REGNUM,                 0x00001FFE,   RW,   ARCompact )
RAUX ( COUNT0,               0x21,  "Processor Timer 1 Count Value",              ARC_COUNT0_REGNUM,                      0xFFFFFFFF,   RW,   ARCompact )
RAUX ( CONTROL0,             0x22,  "Processor Timer 1 Control Value",            ARC_CONTROL0_REGNUM,                    0x0000000F,   RW,   ARCompact )
RAUX ( LIMIT0,               0x23,  "Processor Timer 1 Limit Value",              ARC_LIMIT0_REGNUM,                      0xFFFFFFFF,   RW,   ARCompact )
RAUX ( INT_VECTOR_BASE,      0x25,  "Interrupt Vector Base Register",             ARC_INT_VECTOR_BASE_REGNUM,             0xFFFFFC00,   RW,   ARCompact )
RAUX ( AUX_MACMODE,          0x41,  "Aux MAC Mode",                               ARC_AUX_MACMODE_REGNUM,                 0xFFFFFFFF,   RW,   ARCompact )
RAUX ( AUX_IRQ_LV12,         0x43,  "Aux IRQ Level 2",                            ARC_AUX_IRQ_LV12_REGNUM,                0x00000003,   RW,   ARCompact )
RAUX ( COUNT1,               0x100, "Processor Timer 1 Count Value",              ARC_COUNT1_REGNUM,                      0xFFFFFFFF,   RW,   ARCompact )
RAUX ( CONTROL1,             0x101, "Processor Timer 1 Control Value",            ARC_CONTROL1_REGNUM,                    0x0000000F,   RW,   ARCompact )
RAUX ( LIMIT1,               0x102, "Processor Timer 1 Limit Value",              ARC_LIMIT1_REGNUM,                      0xFFFFFFFF,   RW,   ARCompact )
RAUX ( AUX_IRQ_LEV,          0x200, "Interrupt Level programming",                ARC_AUX_IRQ_LEV_REGNUM,                 0xFFFFFFF8,   RW,   ARCompact )
RAUX ( AUX_IRQ_HINT,         0x201, "Software Triggered Interrupt",               ARC_AUX_IRQ_HINT_REGNUM,                0x0000001F,   RW,   ARCompact )
RAUX ( ERET,                 0x400, "Exception Return",                           ARC_ERET_REGNUM,                        0xFFFFFFFE,   RW,   ARC700    )
RAUX ( ERBTA,                0x401, "Exception BTA",                              ARC_ERBTA_REGNUM,                       0xFFFFFFFE,   RW,   ARC700    )
RAUX ( ERSTATUS,             0x402, "Exception Return Status",                    ARC_ERSTATUS_REGNUM,                    0x00001FFE,   RW,   ARC700    )
RAUX ( ECR,                  0x403, "Exception Cause Register",                   ARC_ECR_REGNUM,                         0x00FFFFFF,   RO,   ARC700    )
RAUX ( EFA,                  0x404, "Exception Fault Address",                    ARC_EFA_REGNUM,                         0xFFFFFFFF,   RW,   ARC700    )
RAUX ( ICAUSE1,              0x40A, "Interrupt Cause (Level 1)",                  ARC_ICAUSE1_REGNUM,                     0x0000001F,   RO,   ARC700    )
RAUX ( ICAUSE2,              0x40B, "Interrupt Cause (Level 2)",                  ARC_ICAUSE2_REGNUM,                     0x0000001F,   RO,   ARC700    )
RAUX ( AUX_IENABLE,          0x40C, "Interrupt Mask Programming",                 ARC_AUX_IENABLE_REGNUM,                 0xFFFFFFF8,   RW,   ARC700    )
RAUX ( AUX_ITRIGGER,         0x40D, "Interrupt Sensitivity Programming",          ARC_AUX_ITRIGGER_REGNUM,                0xFFFFFFF8,   RW,   ARC700    )
RAUX ( XPU,                  0x410, "User Mode Extension Permissions",            ARC_XPU_REGNUM,                         0xFFFFFFFF,   RW,   ARC700    )
RAUX ( BTA,                  0x412, "Branch Target Address",                      ARC_BTA_REGNUM,                         0xFFFFFFFE,   RO,   ARC700    )
RAUX ( BTA_L1,               0x413, "Branch Target Address in Level 1",           ARC_BTA_L1_REGNUM,                      0xFFFFFFFE,   RW,   ARC700    )
RAUX ( BTA_L2,               0x414, "Branch Target Address in Level 2",           ARC_BTA_L2_REGNUM,                      0xFFFFFFFE,   RW,   ARC700    )
RAUX ( AUX_IRQ_PULSE_CANCEL, 0x415, "Interrupt Pulse Cancel",                     ARC_AUX_IRQ_PULSE_CANCEL_REGNUM,        0xFFFFFFFA,   WO,   ARC700    )
RAUX ( AUX_IRQ_PENDING,      0x416, "Interrupt Pending Register",                 ARC_AUX_IRQ_PENDING_REGNUM,             0xFFFFFFF8,   RO,   ARC700    )

#endif // RAUX


/* Build Configuration Registers.  */

#ifdef RBCR

RBCR ( UNUSED_0,             0x60,  "unused",                                            ARC_BCR_0_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( DCCM_BASE_BUILD,      0x61,  "Base address for DCCM",                             ARC_BCR_1_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( CRC_BASE_BUILD,       0x62,  "BCR for CRC Unit",                                  ARC_BCR_2_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( BTA_LINK_BUILD,       0x63,  "Interrupt Link Registers Available for BTA",        ARC_BCR_3_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( DVBF_BUILD,           0x64,  "BCR for Dual Viterbi Instruction",                  ARC_BCR_4_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( TEL_INSTR_BUILD,      0x65,  "BCR for Extended Arithmetic Instructions",          ARC_BCR_5_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( UNUSED_6,             0x66,  "unused",                                            ARC_BCR_6_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( MEMSUBSYS,            0x67,  "BCR for Memory Subsystem",                          ARC_BCR_7_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( VECBASE_AC_BUILD,     0x68,  "BCR for Interrupt Vector Base",                     ARC_BCR_8_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( P_BASE_ADDRESS,       0x69,  "Peripheral Base Address",                           ARC_BCR_9_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( UNUSED_A,             0x6A,  "unused",                                            ARC_BCR_A_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( UNUSED_B,             0x6B,  "unused",                                            ARC_BCR_B_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( UNUSED_C,             0x6C,  "unused",                                            ARC_BCR_C_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( UNUSED_D,             0x6D,  "unused",                                            ARC_BCR_D_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( UNUSED_E,             0x6E,  "unused",                                            ARC_BCR_E_REGNUM,                0xFFFFFFFF,   UU,   ARCompact )
RBCR ( MMU_BUILD,            0x6F,  "MMU Build",                                         ARC_BCR_F_REGNUM,                0xFFFFFFFF,   RO,   ARCompact )
RBCR ( ARCANGEL_BUILD,       0x70,  "ARC Angel Build Config",                            ARC_BCR_10_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( UNUSED_11,            0x6E,  "unused",                                            ARC_BCR_11_REGNUM,               0xFFFFFFFF,   UU,   ARCompact )
RBCR ( D_CACHE_BUILD,        0x72,  "D Cache Build Config",                              ARC_BCR_12_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( MADI_BUILD,           0x73,  "Multiple ARC Debug Interface",                      ARC_BCR_13_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( DCCM_BUILD,           0x74,  "BCR for DCCM (Data Closely Coupled Memory)",        ARC_BCR_14_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( TIMER_BUILD,          0x75,  "BCR for Timers",                                    ARC_BCR_15_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( AP_BUILD,             0x76,  "Actionpoints build",                                ARC_BCR_16_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( ICACHE_BUILD,         0x77,  "Instruction Cache BCR",                             ARC_BCR_17_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( ICCM_BUILD,           0x78,  "ICCM BCR (Instruction Closely Coupled Memory)",     ARC_BCR_18_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( DSPRAM_BUILD,         0x79,  "DSP RAM Build",                                     ARC_BCR_19_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( MAC_BUILD,            0x7A,  "MAC Unit Build",                                    ARC_BCR_1A_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( MULTIPLY_BUILD,       0x7B,  "(32 X 32) Multiply Unit Build",                     ARC_BCR_1B_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( SWAP_BUILD,           0x7C,  "SWAP Build",                                        ARC_BCR_1C_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( NORM_BUILD,           0x7D,  "NORM Unit Build",                                   ARC_BCR_1D_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( MINMAX_BUILD,         0x7E,  "Minmax Unit Build",                                 ARC_BCR_1E_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )
RBCR ( BARREL_BUILD,         0x7F,  "Barrel Shifter Build",                              ARC_BCR_1F_REGNUM,               0xFFFFFFFF,   RO,   ARCompact )

#endif // RBCR

/******************************************************************************/
