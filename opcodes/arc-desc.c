/* CPU data for arc.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2005 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#include "sysdep.h"
#include <stdio.h>
#include <stdarg.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "arc-desc.h"
#include "arc-opc.h"
#include "opintl.h"
#include "libiberty.h"
#include "xregex.h"

/* Attributes.  */

static const CGEN_ATTR_ENTRY bool_attr[] =
{
  { "#f", 0 },
  { "#t", 1 },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY MACH_attr[] ATTRIBUTE_UNUSED =
{
  { "base", MACH_BASE },
  { "a5", MACH_A5 },
  { "arc600", MACH_ARC600 },
  { "arc700", MACH_ARC700 },
  { "max", MACH_MAX },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY ISA_attr[] ATTRIBUTE_UNUSED =
{
  { "ARCompact", ISA_ARCOMPACT },
  { "max", ISA_MAX },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY LIMM_attr[] ATTRIBUTE_UNUSED =
{
  { "none", LIMM_NONE },
  { "h", LIMM_H },
  { "B", LIMM_B },
  { "BC", LIMM_BC },
  { "C", LIMM_C },
  { 0, 0 }
};

#ifdef UNUSED
const CGEN_ATTR_TABLE arc_cgen_ifield_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "RESERVED", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE arc_cgen_hardware_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "CACHE-ADDR", &bool_attr[0], &bool_attr[0] },
  { "PC", &bool_attr[0], &bool_attr[0] },
  { "PROFILE", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE arc_cgen_operand_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { "NEGATIVE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "SEM-ONLY", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE arc_cgen_insn_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "LIMM", & LIMM_attr[0], & LIMM_attr[0] },
  { "ALIAS", &bool_attr[0], &bool_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNCOND-CTI", &bool_attr[0], &bool_attr[0] },
  { "COND-CTI", &bool_attr[0], &bool_attr[0] },
  { "SKIP-CTI", &bool_attr[0], &bool_attr[0] },
  { "DELAY-SLOT", &bool_attr[0], &bool_attr[0] },
  { "RELAXABLE", &bool_attr[0], &bool_attr[0] },
  { "RELAXED", &bool_attr[0], &bool_attr[0] },
  { "NO-DIS", &bool_attr[0], &bool_attr[0] },
  { "PBB", &bool_attr[0], &bool_attr[0] },
  { "SHORT_P", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};
#endif

/* Instruction set variants.  */

static const CGEN_ISA arc_cgen_isa_table[] = {
  { "ARCompact", 32, 32, 32, 32 },
  { 0, 0, 0, 0, 0 }
};

/* Machine variants.  */

static const CGEN_MACH arc_cgen_mach_table[] = {
  { "a5", "A5", MACH_A5, 16 },
  { "arc600", "ARC600", MACH_ARC600, 16 },
  { "arc700", "ARC700", MACH_ARC700, 16 },
  { 0, 0, 0, 0 }
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_cr_names_entries[] =
{
  { "gp", 26, {0, {{{0, 0}}}}, 0, 0 },
  { "fp", 27, {0, {{{0, 0}}}}, 0, 0 },
  { "sp", 28, {0, {{{0, 0}}}}, 0, 0 },
  { "blink", 31, {0, {{{0, 0}}}}, 0, 0 },
  { "mlo", 57, {0, {{{0, 0}}}}, 0, 0 },
  { "mmid", 58, {0, {{{0, 0}}}}, 0, 0 },
  { "mhi", 59, {0, {{{0, 0}}}}, 0, 0 },
  { "lp_count", 60, {0, {{{0, 0}}}}, 0, 0 },
  { "pcl", 63, {0, {{{0, 0}}}}, 0, 0 },
  { "ilink1", 29, {0, {{{0, 0}}}}, 0, 0 },
  { "ilink2", 30, {0, {{{0, 0}}}}, 0, 0 },
  { "r29", 29, {0, {{{0, 0}}}}, 0, 0 },
  { "r30", 30, {0, {{{0, 0}}}}, 0, 0 },
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "r8", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "r9", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "r10", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "r11", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "r12", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "r13", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "r14", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "r15", 15, {0, {{{0, 0}}}}, 0, 0 },
  { "r16", 16, {0, {{{0, 0}}}}, 0, 0 },
  { "r17", 17, {0, {{{0, 0}}}}, 0, 0 },
  { "r18", 18, {0, {{{0, 0}}}}, 0, 0 },
  { "r19", 19, {0, {{{0, 0}}}}, 0, 0 },
  { "r20", 20, {0, {{{0, 0}}}}, 0, 0 },
  { "r21", 21, {0, {{{0, 0}}}}, 0, 0 },
  { "r22", 22, {0, {{{0, 0}}}}, 0, 0 },
  { "r23", 23, {0, {{{0, 0}}}}, 0, 0 },
  { "r24", 24, {0, {{{0, 0}}}}, 0, 0 },
  { "r25", 25, {0, {{{0, 0}}}}, 0, 0 },
  { "r26", 26, {0, {{{0, 0}}}}, 0, 0 },
  { "r27", 27, {0, {{{0, 0}}}}, 0, 0 },
  { "r28", 28, {0, {{{0, 0}}}}, 0, 0 },
  { "r31", 31, {0, {{{0, 0}}}}, 0, 0 },
  { "r32", 32, {0, {{{0, 0}}}}, 0, 0 },
  { "r33", 33, {0, {{{0, 0}}}}, 0, 0 },
  { "r34", 34, {0, {{{0, 0}}}}, 0, 0 },
  { "r35", 35, {0, {{{0, 0}}}}, 0, 0 },
  { "r36", 36, {0, {{{0, 0}}}}, 0, 0 },
  { "r37", 37, {0, {{{0, 0}}}}, 0, 0 },
  { "r38", 38, {0, {{{0, 0}}}}, 0, 0 },
  { "r39", 39, {0, {{{0, 0}}}}, 0, 0 },
  { "r40", 40, {0, {{{0, 0}}}}, 0, 0 },
  { "r41", 41, {0, {{{0, 0}}}}, 0, 0 },
  { "r42", 42, {0, {{{0, 0}}}}, 0, 0 },
  { "r43", 43, {0, {{{0, 0}}}}, 0, 0 },
  { "r44", 44, {0, {{{0, 0}}}}, 0, 0 },
  { "r45", 45, {0, {{{0, 0}}}}, 0, 0 },
  { "r46", 46, {0, {{{0, 0}}}}, 0, 0 },
  { "r47", 47, {0, {{{0, 0}}}}, 0, 0 },
  { "r48", 48, {0, {{{0, 0}}}}, 0, 0 },
  { "r49", 49, {0, {{{0, 0}}}}, 0, 0 },
  { "r50", 50, {0, {{{0, 0}}}}, 0, 0 },
  { "r51", 51, {0, {{{0, 0}}}}, 0, 0 },
  { "r52", 52, {0, {{{0, 0}}}}, 0, 0 },
  { "r53", 53, {0, {{{0, 0}}}}, 0, 0 },
  { "r54", 54, {0, {{{0, 0}}}}, 0, 0 },
  { "r55", 55, {0, {{{0, 0}}}}, 0, 0 },
  { "r56", 56, {0, {{{0, 0}}}}, 0, 0 },
  { "r57", 57, {0, {{{0, 0}}}}, 0, 0 },
  { "r58", 58, {0, {{{0, 0}}}}, 0, 0 },
  { "r59", 59, {0, {{{0, 0}}}}, 0, 0 },
  { "r60", 60, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_cr_names =
{
  & arc_cgen_opval_cr_names_entries[0],
  72,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_Qcondb_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "ra", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "al", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "eq", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "z", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "ne", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "nz", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "pl", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "p", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "mi", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "n", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "cs", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "c", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "lo", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "cc", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "nc", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "hs", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "vs", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "v", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "vc", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "nv", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "gt", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "ge", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "lt", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "le", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "hi", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "ls", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "pnz", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_Qcondb =
{
  & arc_cgen_opval_h_Qcondb_entries[0],
  28,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_Qcondj_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "al", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "eq", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "z", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "ne", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "nz", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "pl", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "p", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "mi", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "n", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "cs", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "c", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "lo", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "cc", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "nc", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "hs", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "vs", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "v", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "vc", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "nv", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "gt", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "ge", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "lt", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "le", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "hi", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "ls", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "pnz", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_Qcondj =
{
  & arc_cgen_opval_h_Qcondj_entries[0],
  27,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_Qcondi_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".al", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".eq", 1, {0, {{{0, 0}}}}, 0, 0 },
  { ".z", 1, {0, {{{0, 0}}}}, 0, 0 },
  { ".ne", 2, {0, {{{0, 0}}}}, 0, 0 },
  { ".nz", 2, {0, {{{0, 0}}}}, 0, 0 },
  { ".pl", 3, {0, {{{0, 0}}}}, 0, 0 },
  { ".p", 3, {0, {{{0, 0}}}}, 0, 0 },
  { ".mi", 4, {0, {{{0, 0}}}}, 0, 0 },
  { ".n", 4, {0, {{{0, 0}}}}, 0, 0 },
  { ".cs", 5, {0, {{{0, 0}}}}, 0, 0 },
  { ".c", 5, {0, {{{0, 0}}}}, 0, 0 },
  { ".lo", 5, {0, {{{0, 0}}}}, 0, 0 },
  { ".cc", 6, {0, {{{0, 0}}}}, 0, 0 },
  { ".nc", 6, {0, {{{0, 0}}}}, 0, 0 },
  { ".hs", 6, {0, {{{0, 0}}}}, 0, 0 },
  { ".vs", 7, {0, {{{0, 0}}}}, 0, 0 },
  { ".v", 7, {0, {{{0, 0}}}}, 0, 0 },
  { ".vc", 8, {0, {{{0, 0}}}}, 0, 0 },
  { ".nv", 8, {0, {{{0, 0}}}}, 0, 0 },
  { ".gt", 9, {0, {{{0, 0}}}}, 0, 0 },
  { ".ge", 10, {0, {{{0, 0}}}}, 0, 0 },
  { ".lt", 11, {0, {{{0, 0}}}}, 0, 0 },
  { ".le", 12, {0, {{{0, 0}}}}, 0, 0 },
  { ".hi", 13, {0, {{{0, 0}}}}, 0, 0 },
  { ".ls", 14, {0, {{{0, 0}}}}, 0, 0 },
  { ".pnz", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_Qcondi =
{
  & arc_cgen_opval_h_Qcondi_entries[0],
  27,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_uncondb_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "al", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "ra", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_uncondb =
{
  & arc_cgen_opval_h_uncondb_entries[0],
  3,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_uncondj_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "al", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_uncondj =
{
  & arc_cgen_opval_h_uncondj_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_uncondi_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".al", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_uncondi =
{
  & arc_cgen_opval_h_uncondi_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_i2cond_entries[] =
{
  { "COND2_", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "COND2_al", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "COND2_ra", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "COND2_eq", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "COND2_z", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "COND2_ne", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "COND2_nz", 2, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_i2cond =
{
  & arc_cgen_opval_h_i2cond_entries[0],
  7,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_i3cond_entries[] =
{
  { "COND3_gt", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_ge", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_lt", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_le", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_hi", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_cc", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_nc", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_hs", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_cs", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_c", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_lo", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "COND3_ls", 7, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_i3cond =
{
  & arc_cgen_opval_h_i3cond_entries[0],
  12,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_delay_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".d", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_delay =
{
  & arc_cgen_opval_h_delay_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_uflags_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".f", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_uflags =
{
  & arc_cgen_opval_h_uflags_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_nil_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_nil =
{
  & arc_cgen_opval_h_nil_entries[0],
  1,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_auflags_entries[] =
{
  { "", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_auflags =
{
  & arc_cgen_opval_h_auflags_entries[0],
  1,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_aufflags_entries[] =
{
  { ".f", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_aufflags =
{
  & arc_cgen_opval_h_aufflags_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_Di_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".di", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_Di =
{
  & arc_cgen_opval_h_Di_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_insn16_entries[] =
{
  { "_s", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_insn16 =
{
  & arc_cgen_opval_h_insn16_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_insn32_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "_l", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_insn32 =
{
  & arc_cgen_opval_h_insn32_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h__aw_entries[] =
{
  { ".a", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".aw", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h__aw =
{
  & arc_cgen_opval_h__aw_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_cr16_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r12", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r13", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r14", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r15", 7, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_cr16 =
{
  & arc_cgen_opval_h_cr16_entries[0],
  8,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_r0_entries[] =
{
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_r0 =
{
  & arc_cgen_opval_h_r0_entries[0],
  1,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_gp_entries[] =
{
  { "r26", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "gp", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_gp =
{
  & arc_cgen_opval_h_gp_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_sp_entries[] =
{
  { "sp", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r28", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_sp =
{
  & arc_cgen_opval_h_sp_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_pcl_entries[] =
{
  { "pcl", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r63", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_pcl =
{
  & arc_cgen_opval_h_pcl_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_noilink_entries[] =
{
  { "gp", 26, {0, {{{0, 0}}}}, 0, 0 },
  { "fp", 27, {0, {{{0, 0}}}}, 0, 0 },
  { "sp", 28, {0, {{{0, 0}}}}, 0, 0 },
  { "blink", 31, {0, {{{0, 0}}}}, 0, 0 },
  { "mlo", 57, {0, {{{0, 0}}}}, 0, 0 },
  { "mmid", 58, {0, {{{0, 0}}}}, 0, 0 },
  { "mhi", 59, {0, {{{0, 0}}}}, 0, 0 },
  { "lp_count", 60, {0, {{{0, 0}}}}, 0, 0 },
  { "pcl", 63, {0, {{{0, 0}}}}, 0, 0 },
  { "r0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "r2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "r3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "r4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "r5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "r6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "r7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "r8", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "r9", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "r10", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "r11", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "r12", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "r13", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "r14", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "r15", 15, {0, {{{0, 0}}}}, 0, 0 },
  { "r16", 16, {0, {{{0, 0}}}}, 0, 0 },
  { "r17", 17, {0, {{{0, 0}}}}, 0, 0 },
  { "r18", 18, {0, {{{0, 0}}}}, 0, 0 },
  { "r19", 19, {0, {{{0, 0}}}}, 0, 0 },
  { "r20", 20, {0, {{{0, 0}}}}, 0, 0 },
  { "r21", 21, {0, {{{0, 0}}}}, 0, 0 },
  { "r22", 22, {0, {{{0, 0}}}}, 0, 0 },
  { "r23", 23, {0, {{{0, 0}}}}, 0, 0 },
  { "r24", 24, {0, {{{0, 0}}}}, 0, 0 },
  { "r25", 25, {0, {{{0, 0}}}}, 0, 0 },
  { "r26", 26, {0, {{{0, 0}}}}, 0, 0 },
  { "r27", 27, {0, {{{0, 0}}}}, 0, 0 },
  { "r28", 28, {0, {{{0, 0}}}}, 0, 0 },
  { "r31", 31, {0, {{{0, 0}}}}, 0, 0 },
  { "r32", 32, {0, {{{0, 0}}}}, 0, 0 },
  { "r33", 33, {0, {{{0, 0}}}}, 0, 0 },
  { "r34", 34, {0, {{{0, 0}}}}, 0, 0 },
  { "r35", 35, {0, {{{0, 0}}}}, 0, 0 },
  { "r36", 36, {0, {{{0, 0}}}}, 0, 0 },
  { "r37", 37, {0, {{{0, 0}}}}, 0, 0 },
  { "r38", 38, {0, {{{0, 0}}}}, 0, 0 },
  { "r39", 39, {0, {{{0, 0}}}}, 0, 0 },
  { "r40", 40, {0, {{{0, 0}}}}, 0, 0 },
  { "r41", 41, {0, {{{0, 0}}}}, 0, 0 },
  { "r42", 42, {0, {{{0, 0}}}}, 0, 0 },
  { "r43", 43, {0, {{{0, 0}}}}, 0, 0 },
  { "r44", 44, {0, {{{0, 0}}}}, 0, 0 },
  { "r45", 45, {0, {{{0, 0}}}}, 0, 0 },
  { "r46", 46, {0, {{{0, 0}}}}, 0, 0 },
  { "r47", 47, {0, {{{0, 0}}}}, 0, 0 },
  { "r48", 48, {0, {{{0, 0}}}}, 0, 0 },
  { "r49", 49, {0, {{{0, 0}}}}, 0, 0 },
  { "r50", 50, {0, {{{0, 0}}}}, 0, 0 },
  { "r51", 51, {0, {{{0, 0}}}}, 0, 0 },
  { "r52", 52, {0, {{{0, 0}}}}, 0, 0 },
  { "r53", 53, {0, {{{0, 0}}}}, 0, 0 },
  { "r54", 54, {0, {{{0, 0}}}}, 0, 0 },
  { "r55", 55, {0, {{{0, 0}}}}, 0, 0 },
  { "r56", 56, {0, {{{0, 0}}}}, 0, 0 },
  { "r57", 57, {0, {{{0, 0}}}}, 0, 0 },
  { "r58", 58, {0, {{{0, 0}}}}, 0, 0 },
  { "r59", 59, {0, {{{0, 0}}}}, 0, 0 },
  { "r60", 60, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_noilink =
{
  & arc_cgen_opval_h_noilink_entries[0],
  68,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_ilinkx_entries[] =
{
  { "ilink1", 29, {0, {{{0, 0}}}}, 0, 0 },
  { "r29", 29, {0, {{{0, 0}}}}, 0, 0 },
  { "ilink2", 30, {0, {{{0, 0}}}}, 0, 0 },
  { "r30", 30, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_ilinkx =
{
  & arc_cgen_opval_h_ilinkx_entries[0],
  4,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_r31_entries[] =
{
  { "blink", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "r31", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_r31 =
{
  & arc_cgen_opval_h_r31_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_status32_entries[] =
{
  { "status32", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_status32 =
{
  & arc_cgen_opval_h_status32_entries[0],
  1,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_ne_entries[] =
{
  { "ne", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_ne =
{
  & arc_cgen_opval_h_ne_entries[0],
  1,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_RccS_entries[] =
{
  { "eq", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "ne", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_RccS =
{
  & arc_cgen_opval_h_RccS_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY arc_cgen_opval_h_Rcc_entries[] =
{
  { "req", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "rne", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "rlt", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "rge", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "rlo", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "rhs", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "bit0", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "bit1", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD arc_cgen_opval_h_Rcc =
{
  & arc_cgen_opval_h_Rcc_entries[0],
  8,
  0, 0, 0, 0, ""
};


/* The hardware table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_HW_##a)
#else
#define A(a) (1 << CGEN_HW_/**/a)
#endif

static const CGEN_HW_ENTRY arc_cgen_hw_table[] =
{
  { "h-memory", HW_H_MEMORY, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-sint", HW_H_SINT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uint", HW_H_UINT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-addr", HW_H_ADDR, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-iaddr", HW_H_IADDR, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-lbit", HW_H_LBIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-zbit", HW_H_ZBIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-nbit", HW_H_NBIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-cbit", HW_H_CBIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-vbit", HW_H_VBIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-ubit", HW_H_UBIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-e1", HW_H_E1, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-e2", HW_H_E2, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-s1bit", HW_H_S1BIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-s2bit", HW_H_S2BIT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-Qcondb", HW_H_QCONDB, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_Qcondb, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-Qcondj", HW_H_QCONDJ, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_Qcondj, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-Qcondi", HW_H_QCONDI, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_Qcondi, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uncondb", HW_H_UNCONDB, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_uncondb, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uncondj", HW_H_UNCONDJ, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_uncondj, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uncondi", HW_H_UNCONDI, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_uncondi, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-i2cond", HW_H_I2COND, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_i2cond, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-i3cond", HW_H_I3COND, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_i3cond, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-delay", HW_H_DELAY, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_delay, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uflags", HW_H_UFLAGS, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_uflags, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-nil", HW_H_NIL, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_nil, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-auflags", HW_H_AUFLAGS, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_auflags, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-aufflags", HW_H_AUFFLAGS, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_aufflags, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-Di", HW_H_DI, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_Di, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-insn16", HW_H_INSN16, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_insn16, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-insn32", HW_H_INSN32, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_insn32, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-_aw", HW_H__AW, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h__aw, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-cr", HW_H_CR, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_cr_names, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-cr16", HW_H_CR16, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_cr16, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-r0", HW_H_R0, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_r0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-gp", HW_H_GP, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_gp, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-sp", HW_H_SP, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_sp, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-pcl", HW_H_PCL, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_pcl, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-noilink", HW_H_NOILINK, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_noilink, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-ilinkx", HW_H_ILINKX, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_ilinkx, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-r31", HW_H_R31, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_r31, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-auxr", HW_H_AUXR, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_cr_names, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-status32", HW_H_STATUS32, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_status32, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-timer-expire", HW_H_TIMER_EXPIRE, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-prof-offset", HW_H_PROF_OFFSET, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-ne", HW_H_NE, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_ne, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-pc", HW_H_PC, CGEN_ASM_NONE, 0, { 0|A(PROFILE)|A(PC), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-RccS", HW_H_RCCS, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_RccS, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-Rcc", HW_H_RCC, CGEN_ASM_KEYWORD, (PTR) & arc_cgen_opval_h_Rcc, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { 0, 0, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A


/* The instruction field table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_IFLD_##a)
#else
#define A(a) (1 << CGEN_IFLD_/**/a)
#endif

const CGEN_IFLD arc_cgen_ifld_table[] =
{
  { ARC_F_NIL, "f-nil", 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_ANYOF, "f-anyof", 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_COND_Q, "f-cond-Q", 0, 32, 27, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_COND_I2, "f-cond-i2", 0, 32, 5, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_COND_I3, "f-cond-i3", 0, 32, 7, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_BRCOND, "f-brcond", 0, 32, 28, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP__A, "f-op--a", 0, 32, 13, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP__B, "f-op--b", 0, 32, 5, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP__C, "f-op--c", 0, 32, 8, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_B_5_3, "f-B-5-3", 0, 32, 17, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP_B, "f-op-B", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP_C, "f-op-C", 0, 32, 20, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP_CJ, "f-op-Cj", 0, 32, 20, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_H_2_0, "f-h-2-0", 0, 32, 8, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_H_5_3, "f-h-5-3", 0, 32, 13, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP_H, "f-op-h", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U6, "f-u6", 0, 32, 20, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U6X2, "f-u6x2", 0, 32, 20, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_DELAY_N, "f-delay-N", 0, 32, 26, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_RES27, "f-res27", 0, 32, 27, 1, { 0|A(RESERVED), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_F, "f-F", 0, 32, 16, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_CBRANCH_IMM, "f-cbranch-imm", 0, 32, 27, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OP_A, "f-op-A", 0, 32, 26, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S12H, "f-s12h", 0, 32, 26, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S12, "f-s12", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S12X2, "f-s12x2", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL10, "f-rel10", 0, 32, 7, 9, { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL7, "f-rel7", 0, 32, 10, 6, { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL8, "f-rel8", 0, 32, 9, 7, { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL13BL, "f-rel13bl", 0, 32, 5, 11, { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D21L, "f-d21l", 0, 32, 5, 10, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D21BL, "f-d21bl", 0, 32, 5, 9, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D21H, "f-d21h", 0, 32, 16, 10, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D25M, "f-d25m", 0, 32, 16, 10, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D25H, "f-d25h", 0, 32, 28, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL21, "f-rel21", 0, 0, 0, 0,{ 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL21BL, "f-rel21bl", 0, 0, 0, 0,{ 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL25, "f-rel25", 0, 0, 0, 0,{ 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL25BL, "f-rel25bl", 0, 0, 0, 0,{ 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D9L, "f-d9l", 0, 32, 8, 7, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_D9H, "f-d9h", 0, 32, 16, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_REL9, "f-rel9", 0, 0, 0, 0,{ 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U3, "f-u3", 0, 32, 13, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U5, "f-u5", 0, 32, 11, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U7, "f-u7", 0, 32, 9, 7, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U8, "f-u8", 0, 32, 8, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S9, "f-s9", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U5X2, "f-u5x2", 0, 32, 11, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U5X4, "f-u5x4", 0, 32, 11, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_U8X4, "f-u8x4", 0, 32, 8, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S9X1, "f-s9x1", 0, 32, 7, 9, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S9X2, "f-s9x2", 0, 32, 7, 9, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_S9X4, "f-s9x4", 0, 32, 7, 9, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_DUMMY, "f-dummy", 0, 32, 16, 16, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_OPM, "f-opm", 0, 32, 0, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_GO_TYPE, "f-go-type", 0, 32, 8, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_GO_CC_TYPE, "f-go-cc-type", 0, 32, 26, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_GO_OP, "f-go-op", 0, 32, 10, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_I16_43, "f-i16-43", 0, 32, 11, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_I16_GO, "f-i16-go", 0, 32, 11, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_I16_GP_TYPE, "f-i16-gp-type", 0, 32, 5, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_I16ADDCMPU7_TYPE, "f-i16addcmpu7-type", 0, 32, 8, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_BUF, "f-buf", 0, 32, 15, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_BR, "f-br", 0, 32, 27, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_BLUF, "f-bluf", 0, 32, 14, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_BRSCOND, "f-brscond", 0, 32, 8, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_LDOZZX, "f-ldozzx", 0, 32, 23, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_LDR6ZZX, "f-ldr6zzx", 0, 32, 10, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_STOZZR, "f-stozzr", 0, 32, 29, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_LDOAA, "f-ldoaa", 0, 32, 21, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_LDRAA, "f-ldraa", 0, 32, 8, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_STOAA, "f-stoaa", 0, 32, 27, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_LDODI, "f-LDODi", 0, 32, 20, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_LDRDI, "f-LDRDi", 0, 32, 16, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_STODI, "f-STODi", 0, 32, 26, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { ARC_F_TRAPNUM, "f-trapnum", 0, 32, 5, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { 0, 0, 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A



/* multi ifield declarations */

static const CGEN_MAYBE_MULTI_IFLD ARC_F_OP_B_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_OP_H_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_S12_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_S12X2_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL21_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL21BL_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL25_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL25BL_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL9_MULTI_IFIELD [];
static const CGEN_MAYBE_MULTI_IFLD ARC_F_S9_MULTI_IFIELD [];


/* multi ifield definitions */

static const CGEN_MAYBE_MULTI_IFLD ARC_F_OP_B_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP__B] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_B_5_3] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_OP_H_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_H_2_0] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_H_5_3] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_S12_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U6] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_S12H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_S12X2_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U6] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_S12H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL21_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D21L] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D21H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL21BL_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D21BL] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D21H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL25_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D21L] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D25M] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D25H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL25BL_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D21BL] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D25M] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D25H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_REL9_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D9L] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D9H] } },
    { 0, { (const PTR) 0 } }
};
static const CGEN_MAYBE_MULTI_IFLD ARC_F_S9_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U8] } },
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_D9H] } },
    { 0, { (const PTR) 0 } }
};

/* The operand table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_OPERAND_##a)
#else
#define A(a) (1 << CGEN_OPERAND_/**/a)
#endif
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define OPERAND(op) ARC_OPERAND_##op
#else
#define OPERAND(op) ARC_OPERAND_/**/op
#endif

static const CGEN_OPERAND arc_cgen_operand_table[] =
{
/* pc: program counter */
  { "pc", ARC_OPERAND_PC, HW_H_PC, 0, 0,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_NIL] } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* lbit: loop inhibit bit */
  { "lbit", ARC_OPERAND_LBIT, HW_H_LBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* zbit: zero bit */
  { "zbit", ARC_OPERAND_ZBIT, HW_H_ZBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* nbit: negative bit */
  { "nbit", ARC_OPERAND_NBIT, HW_H_NBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* cbit: carry bit */
  { "cbit", ARC_OPERAND_CBIT, HW_H_CBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* vbit: overflow bit */
  { "vbit", ARC_OPERAND_VBIT, HW_H_VBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1bit: channel 1 saturate */
  { "s1bit", ARC_OPERAND_S1BIT, HW_H_S1BIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s2bit: channel 2 saturate */
  { "s2bit", ARC_OPERAND_S2BIT, HW_H_S2BIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Qcondb: Condition */
  { "Qcondb", ARC_OPERAND_QCONDB, HW_H_QCONDB, 27, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_COND_Q] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Qcondj: Condition */
  { "Qcondj", ARC_OPERAND_QCONDJ, HW_H_QCONDJ, 27, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_COND_Q] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Qcondi: Condition */
  { "Qcondi", ARC_OPERAND_QCONDI, HW_H_QCONDI, 27, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_COND_Q] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uncondb: unconditional branch */
  { "uncondb", ARC_OPERAND_UNCONDB, HW_H_UNCONDB, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uncondj: unconditional jump */
  { "uncondj", ARC_OPERAND_UNCONDJ, HW_H_UNCONDJ, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* uncondi: unconditional insn */
  { "uncondi", ARC_OPERAND_UNCONDI, HW_H_UNCONDI, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* i2cond: Condition */
  { "i2cond", ARC_OPERAND_I2COND, HW_H_I2COND, 5, 2,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_COND_I2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* i3cond: Condition */
  { "i3cond", ARC_OPERAND_I3COND, HW_H_I3COND, 7, 3,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_COND_I3] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* delay_N: Delay slot exposed */
  { "delay_N", ARC_OPERAND_DELAY_N, HW_H_DELAY, 26, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_DELAY_N] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* _S: 16 bit opcode */
  { "_S", ARC_OPERAND__S, HW_H_INSN16, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* _L: 32 bit opcode */
  { "_L", ARC_OPERAND__L, HW_H_INSN32, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* F: update flags */
  { "F", ARC_OPERAND_F, HW_H_UFLAGS, 16, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_F] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* F1: always update flags */
  { "F1", ARC_OPERAND_F1, HW_H_AUFLAGS, 16, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_F] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* F1F: always update flags; .F allowed */
  { "F1F", ARC_OPERAND_F1F, HW_H_AUFFLAGS, 16, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_F] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* F0: never update flags */
  { "F0", ARC_OPERAND_F0, HW_H_NIL, 16, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_F] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* R_a: Core Register a */
  { "R_a", ARC_OPERAND_R_A, HW_H_CR16, 13, 3,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP__A] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RA: Core Register A */
  { "RA", ARC_OPERAND_RA, HW_H_CR, 26, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP_A] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* R_b: Core Register b */
  { "R_b", ARC_OPERAND_R_B, HW_H_CR16, 5, 3,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP__B] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RB: Core Register B */
  { "RB", ARC_OPERAND_RB, HW_H_CR, 5, 6,
    { 2, { (const PTR) &ARC_F_OP_B_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* R_c: Core Register b */
  { "R_c", ARC_OPERAND_R_C, HW_H_CR16, 8, 3,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP__C] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RC: Core Register C */
  { "RC", ARC_OPERAND_RC, HW_H_CR, 20, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP_C] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Rh: Core register h */
  { "Rh", ARC_OPERAND_RH, HW_H_CR, 8, 6,
    { 2, { (const PTR) &ARC_F_OP_H_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* R0: Core Register 0 */
  { "R0", ARC_OPERAND_R0, HW_H_R0, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* R31: Core Register 31 */
  { "R31", ARC_OPERAND_R31, HW_H_R31, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* GP: Global Pointer */
  { "GP", ARC_OPERAND_GP, HW_H_GP, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* SP: Stack Pointer */
  { "SP", ARC_OPERAND_SP, HW_H_SP, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* PCL: read PC - aligned */
  { "PCL", ARC_OPERAND_PCL, HW_H_PCL, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RA_0: encode A as 0 */
  { "RA_0", ARC_OPERAND_RA_0, HW_H_NIL, 26, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP_A] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RB_0: encode B as 0 */
  { "RB_0", ARC_OPERAND_RB_0, HW_H_NIL, 5, 6,
    { 2, { (const PTR) &ARC_F_OP_B_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* RC_ilink: inlink[01] as op C */
  { "RC_ilink", ARC_OPERAND_RC_ILINK, HW_H_ILINKX, 20, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP_CJ] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RC_noilink: Core reg C, not ilink */
  { "RC_noilink", ARC_OPERAND_RC_NOILINK, HW_H_NOILINK, 20, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_OP_CJ] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* NE: NE condition  */
  { "NE", ARC_OPERAND_NE, HW_H_NE, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* U6: 6 bit unsigned immediate */
  { "U6", ARC_OPERAND_U6, HW_H_UINT, 20, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U6] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* U6x2: 6 bit unsigned immediate */
  { "U6x2", ARC_OPERAND_U6X2, HW_H_UINT, 20, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U6X2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* u3: 3 bit unsigned immediate */
  { "u3", ARC_OPERAND_U3, HW_H_UINT, 13, 3,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U3] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* u5: 5 bit unsigned immediate */
  { "u5", ARC_OPERAND_U5, HW_H_UINT, 11, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U5] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* u7: 7 bit unsigned immediate */
  { "u7", ARC_OPERAND_U7, HW_H_UINT, 9, 7,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U7] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* u8: 8 bit unsigned immediate */
  { "u8", ARC_OPERAND_U8, HW_H_UINT, 8, 8,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U8] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s9: 8 bit signed immediate */
  { "s9", ARC_OPERAND_S9, HW_H_SINT, 8, 9,
    { 2, { (const PTR) &ARC_F_S9_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* s12: 12 bit signed immediate */
  { "s12", ARC_OPERAND_S12, HW_H_SINT, 20, 12,
    { 2, { (const PTR) &ARC_F_S12_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* s12x2: 12 bit signed immediate */
  { "s12x2", ARC_OPERAND_S12X2, HW_H_SINT, 20, 12,
    { 2, { (const PTR) &ARC_F_S12X2_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* u5x4: 5 bit uns imm times 4 */
  { "u5x4", ARC_OPERAND_U5X4, HW_H_UINT, 11, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U5X4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sc_u5_: 5 bit uns imm times 4 */
  { "sc_u5_", ARC_OPERAND_SC_U5_, HW_H_UINT, 11, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U5X4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sc_u5w: 5 bit uns imm times 2 */
  { "sc_u5w", ARC_OPERAND_SC_U5W, HW_H_UINT, 11, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U5X2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sc_u5b: 5 bit uns imm times 1 */
  { "sc_u5b", ARC_OPERAND_SC_U5B, HW_H_UINT, 11, 5,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U5] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* u8x4: 8 bit uns imm times 4 */
  { "u8x4", ARC_OPERAND_U8X4, HW_H_UINT, 8, 8,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_U8X4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s9x4: 9 bit sgn imm times 4 */
  { "s9x4", ARC_OPERAND_S9X4, HW_H_UINT, 7, 9,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_S9X4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sc_s9_: 8 bit uns imm times 4 */
  { "sc_s9_", ARC_OPERAND_SC_S9_, HW_H_UINT, 7, 9,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_S9X4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sc_s9w: 8 bit uns imm times 2 */
  { "sc_s9w", ARC_OPERAND_SC_S9W, HW_H_UINT, 7, 9,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_S9X2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sc_s9b: 8 bit uns imm times 1 */
  { "sc_s9b", ARC_OPERAND_SC_S9B, HW_H_UINT, 7, 9,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_S9X1] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* label7: 7 bit pc relative address */
  { "label7", ARC_OPERAND_LABEL7, HW_H_IADDR, 10, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_REL7] } }, 
    { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
/* label8: 8 bit pc relative address */
  { "label8", ARC_OPERAND_LABEL8, HW_H_IADDR, 9, 7,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_REL8] } }, 
    { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
/* label9: 9 bit pc relative address */
  { "label9", ARC_OPERAND_LABEL9, HW_H_IADDR, 8, 8,
    { 2, { (const PTR) &ARC_F_REL9_MULTI_IFIELD[0] } }, 
    { 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* label10: 10 bit pc relative address */
  { "label10", ARC_OPERAND_LABEL10, HW_H_IADDR, 7, 9,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_REL10] } }, 
    { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
/* label13a: 13 bit bl pc rel address */
  { "label13a", ARC_OPERAND_LABEL13A, HW_H_IADDR, 5, 11,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_REL13BL] } }, 
    { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
/* label21: 21 bit pc relative address */
  { "label21", ARC_OPERAND_LABEL21, HW_H_IADDR, 5, 20,
    { 2, { (const PTR) &ARC_F_REL21_MULTI_IFIELD[0] } }, 
    { 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* label21a: 21 bit bl pc rel address */
  { "label21a", ARC_OPERAND_LABEL21A, HW_H_IADDR, 5, 19,
    { 2, { (const PTR) &ARC_F_REL21BL_MULTI_IFIELD[0] } }, 
    { 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* label25: 25 bit pc relative address */
  { "label25", ARC_OPERAND_LABEL25, HW_H_IADDR, 5, 24,
    { 3, { (const PTR) &ARC_F_REL25_MULTI_IFIELD[0] } }, 
    { 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* label25a: 25 bit bl pc rel address */
  { "label25a", ARC_OPERAND_LABEL25A, HW_H_IADDR, 5, 23,
    { 3, { (const PTR) &ARC_F_REL25BL_MULTI_IFIELD[0] } }, 
    { 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* dummy-op: (first 16 bit of) next insn */
  { "dummy-op", ARC_OPERAND_DUMMY_OP, HW_H_UINT, 16, 16,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_DUMMY] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* RccS: BRcc_s */
  { "RccS", ARC_OPERAND_RCCS, HW_H_RCCS, 8, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_BRSCOND] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Rcc: BRcc / BBIT Condition */
  { "Rcc", ARC_OPERAND_RCC, HW_H_RCC, 28, 4,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_BRCOND] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* LDODi: ld /w offs Direct mem access */
  { "LDODi", ARC_OPERAND_LDODI, HW_H_DI, 20, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_LDODI] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* LDRDi: ld reg-reg Direct mem access */
  { "LDRDi", ARC_OPERAND_LDRDI, HW_H_DI, 16, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_LDRDI] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* STODi: ld w/ offs Direct mem access */
  { "STODi", ARC_OPERAND_STODI, HW_H_DI, 26, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_STODI] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* EXDi: ex Direct memory access */
  { "EXDi", ARC_OPERAND_EXDI, HW_H_DI, 16, 1,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_F] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* _AW: .AW suffix */
  { "_AW", ARC_OPERAND__AW, HW_H__AW, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* trapnum: 6 bit trap number */
  { "trapnum", ARC_OPERAND_TRAPNUM, HW_H_UINT, 5, 6,
    { 0, { (const PTR) &arc_cgen_ifld_table[ARC_F_TRAPNUM] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sentinel */
  { 0, 0, 0, 0, 0,
    { 0, { (const PTR) 0 } },
    { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A


/* The instruction table.  */

#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_INSN_##a)
#else
#define A(a) (1 << CGEN_INSN_/**/a)
#endif

static const CGEN_IBASE arc_cgen_insn_table[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } } },
/* b$i2cond $label10 */
  {
    ARC_INSN_B_S, "b_s", "b", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$i3cond$_S $label7 */
  {
    ARC_INSN_BCC_S, "bcc_s", "b", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* br$RccS$_S $R_b,0,$label8 */
  {
    ARC_INSN_BRCC_S, "brcc_s", "br", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$Qcondb$_L $label21 */
  {
    ARC_INSN_BCC_L, "bcc_l", "b", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$Qcondb$_L.d $label21 */
  {
    ARC_INSN_BCC_L_D, "bcc_l.d", "b", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$uncondb$_L $label25 */
  {
    ARC_INSN_B_L, "b_l", "b", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$uncondb$_L.d $label25 */
  {
    ARC_INSN_B_L_D, "b_l.d", "b", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$Rcc $RB,$RC,$label9 */
  {
    ARC_INSN_BRCC_RC, "brcc_RC", "b", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* b$Rcc.d $RB,$RC,$label9 */
  {
    ARC_INSN_BRCC_RC_D, "brcc_RC.d", "b", 32,
    { 0|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* b$Rcc $RB,$U6,$label9 */
  {
    ARC_INSN_BRCC_U6, "brcc_U6", "b", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* b$Rcc.d $RB,$U6,$label9 */
  {
    ARC_INSN_BRCC_U6_D, "brcc_U6.d", "b", 32,
    { 0|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bl$uncondj$_S $label13a */
  {
    ARC_INSN_BL_S, "bl_s", "bl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bl$Qcondj$_L $label21 */
  {
    ARC_INSN_BLCC, "blcc", "bl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bl$Qcondj$_L.d $label21 */
  {
    ARC_INSN_BLCC_D, "blcc.d", "bl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bl$uncondj$_L $label25a */
  {
    ARC_INSN_BL, "bl", "bl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bl$uncondj$_L.d $label25a */
  {
    ARC_INSN_BL_D, "bl.d", "bl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ld$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LD_ABS, "ld_abs", "ld", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ld$_AW$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LD__AW_ABS, "ld$_AW_abs", "ld", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ld.ab$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LD_AB_ABS, "ld.ab_abs", "ld.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ld.as$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LD_AS_ABS, "ld.as_abs", "ld.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ld$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LD_ABC, "ld_abc", "ld", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ld$_AW$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LD__AW_ABC, "ld$_AW_abc", "ld", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ld.ab$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LD_AB_ABC, "ld.ab_abc", "ld.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ld.as$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LD_AS_ABC, "ld.as_abc", "ld.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ld$_S $R_a,[$R_b,$R_c] */
  {
    ARC_INSN_LD_S_ABC, "ld_s_abc", "ld", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ld$_S $R_c,[$R_b,$sc_u5_] */
  {
    ARC_INSN_LD_S_ABU, "ld_s_abu", "ld", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ld$_S $R_b,[$SP,$u5x4] */
  {
    ARC_INSN_LD_S_ABSP, "ld_s_absp", "ld", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ld$_S $R_b,[$GP,$sc_s9_] */
  {
    ARC_INSN_LD_S_GPREL, "ld_s_gprel", "ld", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ld$_S $R_b,[$PCL,$u8x4] */
  {
    ARC_INSN_LD_S_PCREL, "ld_s_pcrel", "ld", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldb$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB_ABS, "ldb_abs", "ldb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb$_AW$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB__AW_ABS, "ldb$_AW_abs", "ldb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb.ab$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB_AB_ABS, "ldb.ab_abs", "ldb.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb.as$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB_AS_ABS, "ldb.as_abs", "ldb.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB_ABC, "ldb_abc", "ldb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb$_AW$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB__AW_ABC, "ldb$_AW_abc", "ldb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb.ab$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB_AB_ABC, "ldb.ab_abc", "ldb.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb.as$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB_AS_ABC, "ldb.as_abc", "ldb.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb$_S $R_a,[$R_b,$R_c] */
  {
    ARC_INSN_LDB_S_ABC, "ldb_s_abc", "ldb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldb$_S $R_c,[$R_b,$sc_u5b] */
  {
    ARC_INSN_LDB_S_ABU, "ldb_s_abu", "ldb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldb$_S $R_b,[$SP,$u5x4] */
  {
    ARC_INSN_LDB_S_ABSP, "ldb_s_absp", "ldb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldb$_S $R_b,[$GP,$sc_s9b] */
  {
    ARC_INSN_LDB_S_GPREL, "ldb_s_gprel", "ldb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldb.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB_X_ABS, "ldb.x_abs", "ldb.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb$_AW.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB__AW_X_ABS, "ldb$_AW.x_abs", "ldb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb.ab.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB_AB_X_ABS, "ldb.ab.x_abs", "ldb.ab.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb.as.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDB_AS_X_ABS, "ldb.as.x_abs", "ldb.as.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldb.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB_X_ABC, "ldb.x_abc", "ldb.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb$_AW.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB__AW_X_ABC, "ldb$_AW.x_abc", "ldb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb.ab.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB_AB_X_ABC, "ldb.ab.x_abc", "ldb.ab.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldb.as.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDB_AS_X_ABC, "ldb.as.x_abc", "ldb.as.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW_ABS, "ldw_abs", "ldw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw$_AW$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW__AW_ABS, "ldw$_AW_abs", "ldw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw.ab$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW_AB_ABS, "ldw.ab_abs", "ldw.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw.as$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW_AS_ABS, "ldw.as_abs", "ldw.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW_ABC, "ldw_abc", "ldw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw$_AW$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW__AW_ABC, "ldw$_AW_abc", "ldw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw.ab$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW_AB_ABC, "ldw.ab_abc", "ldw.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw.as$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW_AS_ABC, "ldw.as_abc", "ldw.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw$_S $R_a,[$R_b,$R_c] */
  {
    ARC_INSN_LDW_S_ABC, "ldw_s_abc", "ldw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldw$_S $R_c,[$R_b,$sc_u5w] */
  {
    ARC_INSN_LDW_S_ABU, "ldw_s_abu", "ldw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldw$_S $R_b,[$GP,$sc_s9w] */
  {
    ARC_INSN_LDW_S_GPREL, "ldw_s_gprel", "ldw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ldw.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW_X_ABS, "ldw.x_abs", "ldw.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw$_AW.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW__AW_X_ABS, "ldw$_AW.x_abs", "ldw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw.ab.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW_AB_X_ABS, "ldw.ab.x_abs", "ldw.ab.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw.as.x$LDODi $RA,[$RB,$s9] */
  {
    ARC_INSN_LDW_AS_X_ABS, "ldw.as.x_abs", "ldw.as.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ldw.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW_X_ABC, "ldw.x_abc", "ldw.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw$_AW.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW__AW_X_ABC, "ldw$_AW.x_abc", "ldw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw.ab.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW_AB_X_ABC, "ldw.ab.x_abc", "ldw.ab.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw.as.x$LDRDi $RA,[$RB,$RC] */
  {
    ARC_INSN_LDW_AS_X_ABC, "ldw.as.x_abc", "ldw.as.x", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ldw$_S.x $R_c,[$R_b,$sc_u5w] */
  {
    ARC_INSN_LDW_S_X_ABU, "ldw_s.x_abu", "ldw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* st$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_ST_ABS, "st_abs", "st", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* st$_AW$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_ST__AW_ABS, "st$_AW_abs", "st", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* st.ab$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_ST_AB_ABS, "st.ab_abs", "st.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* st.as$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_ST_AS_ABS, "st.as_abs", "st.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* st$_S $R_c,[$R_b,$sc_u5_] */
  {
    ARC_INSN_ST_S_ABU, "st_s_abu", "st", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* st$_S $R_b,[$SP,$u5x4] */
  {
    ARC_INSN_ST_S_ABSP, "st_s_absp", "st", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* stb$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STB_ABS, "stb_abs", "stb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stb$_AW$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STB__AW_ABS, "stb$_AW_abs", "stb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stb.ab$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STB_AB_ABS, "stb.ab_abs", "stb.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stb.as$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STB_AS_ABS, "stb.as_abs", "stb.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stb$_S $R_c,[$R_b,$sc_u5b] */
  {
    ARC_INSN_STB_S_ABU, "stb_s_abu", "stb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* stb$_S $R_b,[$SP,$u5x4] */
  {
    ARC_INSN_STB_S_ABSP, "stb_s_absp", "stb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* stw$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STW_ABS, "stw_abs", "stw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stw$_AW$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STW__AW_ABS, "stw$_AW_abs", "stw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stw.ab$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STW_AB_ABS, "stw.ab_abs", "stw.ab", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stw.as$STODi $RC,[$RB,$s9] */
  {
    ARC_INSN_STW_AS_ABS, "stw.as_abs", "stw.as", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* stw$_S $R_c,[$R_b,$sc_u5w] */
  {
    ARC_INSN_STW_S_ABU, "stw_s_abu", "stw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADD_L_S12__RA_, "add_L_s12 $RA,", "add", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADD_CCU6__RA_, "add_ccu6 $RA,", "add", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADD_L_U6__RA_, "add_L_u6 $RA,", "add", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADD_L_R_R__RA__RC, "add_L_r_r $RA,$RC", "add", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADD_CC__RA__RC, "add_cc $RA,$RC", "add", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add$_S $R_a,$R_b,$R_c */
  {
    ARC_INSN_ADD_S_ABC, "add_s_abc", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add$_S $R_c,$R_b,$u3 */
  {
    ARC_INSN_ADD_S_CBU3, "add_s_cbu3", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add$_S $R_b,$R_b,$Rh */
  {
    ARC_INSN_ADD_S_MCAH, "add_s_mcah", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_H, 0 } } } }
  },
/* add$_S $R_b,$SP,$u5x4 */
  {
    ARC_INSN_ADD_S_ABSP, "add_s_absp", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add$_S $SP,$SP,$u5x4 */
  {
    ARC_INSN_ADD_S_ASSPSP, "add_s_asspsp", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add$_S $R0,$GP,$s9x4 */
  {
    ARC_INSN_ADD_S_GP, "add_s_gp", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add$_S $R_b,$R_b,$u7 */
  {
    ARC_INSN_ADD_S_R_U7, "add_s_r_u7", "add", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* adc$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADC_L_S12__RA_, "adc_L_s12 $RA,", "adc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* adc$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADC_CCU6__RA_, "adc_ccu6 $RA,", "adc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* adc$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADC_L_U6__RA_, "adc_L_u6 $RA,", "adc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* adc$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADC_L_R_R__RA__RC, "adc_L_r_r $RA,$RC", "adc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* adc$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADC_CC__RA__RC, "adc_cc $RA,$RC", "adc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SUB_L_S12__RA_, "sub_L_s12 $RA,", "sub", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SUB_CCU6__RA_, "sub_ccu6 $RA,", "sub", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SUB_L_U6__RA_, "sub_L_u6 $RA,", "sub", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SUB_L_R_R__RA__RC, "sub_L_r_r $RA,$RC", "sub", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SUB_CC__RA__RC, "sub_cc $RA,$RC", "sub", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub$_S $R_c,$R_b,$u3 */
  {
    ARC_INSN_SUB_S_CBU3, "sub_s_cbu3", "sub", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sub$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_SUB_S_GO, "I16_GO_SUB_s_go", "sub", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sub$_S $NE$R_b,$R_b,$R_b */
  {
    ARC_INSN_SUB_S_GO_SUB_NE, "sub_s_go_sub_ne", "sub", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sub$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_SUB_S_SSB, "sub_s_ssb", "sub", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sub$_S $SP,$SP,$u5x4 */
  {
    ARC_INSN_SUB_S_ASSPSP, "sub_s_asspsp", "sub", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sbc$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SBC_L_S12__RA_, "sbc_L_s12 $RA,", "sbc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sbc$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SBC_CCU6__RA_, "sbc_ccu6 $RA,", "sbc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sbc$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SBC_L_U6__RA_, "sbc_L_u6 $RA,", "sbc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sbc$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SBC_L_R_R__RA__RC, "sbc_L_r_r $RA,$RC", "sbc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sbc$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SBC_CC__RA__RC, "sbc_cc $RA,$RC", "sbc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* and$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_AND_L_S12__RA_, "and_L_s12 $RA,", "and", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* and$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_AND_CCU6__RA_, "and_ccu6 $RA,", "and", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* and$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_AND_L_U6__RA_, "and_L_u6 $RA,", "and", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* and$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_AND_L_R_R__RA__RC, "and_L_r_r $RA,$RC", "and", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* and$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_AND_CC__RA__RC, "and_cc $RA,$RC", "and", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* and$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_AND_S_GO, "I16_GO_AND_s_go", "and", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* or$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_OR_L_S12__RA_, "or_L_s12 $RA,", "or", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* or$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_OR_CCU6__RA_, "or_ccu6 $RA,", "or", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* or$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_OR_L_U6__RA_, "or_L_u6 $RA,", "or", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* or$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_OR_L_R_R__RA__RC, "or_L_r_r $RA,$RC", "or", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* or$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_OR_CC__RA__RC, "or_cc $RA,$RC", "or", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* or$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_OR_S_GO, "I16_GO_OR_s_go", "or", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bic$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_BIC_L_S12__RA_, "bic_L_s12 $RA,", "bic", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bic$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_BIC_CCU6__RA_, "bic_ccu6 $RA,", "bic", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bic$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_BIC_L_U6__RA_, "bic_L_u6 $RA,", "bic", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bic$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_BIC_L_R_R__RA__RC, "bic_L_r_r $RA,$RC", "bic", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bic$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_BIC_CC__RA__RC, "bic_cc $RA,$RC", "bic", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bic$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_BIC_S_GO, "I16_GO_BIC_s_go", "bic", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* xor$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_XOR_L_S12__RA_, "xor_L_s12 $RA,", "xor", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* xor$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_XOR_CCU6__RA_, "xor_ccu6 $RA,", "xor", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* xor$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_XOR_L_U6__RA_, "xor_L_u6 $RA,", "xor", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* xor$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_XOR_L_R_R__RA__RC, "xor_L_r_r $RA,$RC", "xor", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* xor$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_XOR_CC__RA__RC, "xor_cc $RA,$RC", "xor", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* xor$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_XOR_S_GO, "I16_GO_XOR_s_go", "xor", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* max$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MAX_L_S12__RA_, "max_L_s12 $RA,", "max", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* max$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MAX_CCU6__RA_, "max_ccu6 $RA,", "max", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* max$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MAX_L_U6__RA_, "max_L_u6 $RA,", "max", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* max$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MAX_L_R_R__RA__RC, "max_L_r_r $RA,$RC", "max", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* max$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MAX_CC__RA__RC, "max_cc $RA,$RC", "max", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* min$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MIN_L_S12__RA_, "min_L_s12 $RA,", "min", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* min$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MIN_CCU6__RA_, "min_ccu6 $RA,", "min", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* min$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MIN_L_U6__RA_, "min_L_u6 $RA,", "min", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* min$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MIN_L_R_R__RA__RC, "min_L_r_r $RA,$RC", "min", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* min$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MIN_CC__RA__RC, "min_cc $RA,$RC", "min", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mov$_L$F $RB,$s12 */
  {
    ARC_INSN_MOV_L_S12_, "mov_L_s12 ", "mov", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* mov$Qcondi$F $RB,$U6 */
  {
    ARC_INSN_MOV_CCU6_, "mov_ccu6 ", "mov", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* mov$_L$F $RB,$U6 */
  {
    ARC_INSN_MOV_L_U6_, "mov_L_u6 ", "mov", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* mov$_L$F $RB,$RC */
  {
    ARC_INSN_MOV_L_R_R__RC, "mov_L_r_r $RC", "mov", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* mov$Qcondi$F $RB,$RC */
  {
    ARC_INSN_MOV_CC__RC, "mov_cc $RC", "mov", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* mov$_S $R_b,$Rh */
  {
    ARC_INSN_MOV_S_MCAH, "mov_s_mcah", "mov", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_H, 0 } } } }
  },
/* mov$_S $Rh,$R_b */
  {
    ARC_INSN_MOV_S_MCAHB, "mov_s_mcahb", "mov", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* mov$_S $R_b,$u7 */
  {
    ARC_INSN_MOV_S_R_U7, "mov_s_r_u7", "mov", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* tst$_L$F1 $RB,$s12 */
  {
    ARC_INSN_TST_L_S12_, "tst_L_s12 ", "tst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* tst$Qcondi$F1 $RB,$U6 */
  {
    ARC_INSN_TST_CCU6_, "tst_ccu6 ", "tst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* tst$_L$F1 $RB,$U6 */
  {
    ARC_INSN_TST_L_U6_, "tst_L_u6 ", "tst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* tst$_L$F1 $RB,$RC */
  {
    ARC_INSN_TST_L_R_R__RC, "tst_L_r_r $RC", "tst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* tst$Qcondi$F1 $RB,$RC */
  {
    ARC_INSN_TST_CC__RC, "tst_cc $RC", "tst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* tst$_S $R_b,$R_c */
  {
    ARC_INSN_TST_S_GO, "tst_s_go", "tst", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* cmp$_L$F1 $RB,$s12 */
  {
    ARC_INSN_CMP_L_S12_, "cmp_L_s12 ", "cmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* cmp$Qcondi$F1 $RB,$U6 */
  {
    ARC_INSN_CMP_CCU6_, "cmp_ccu6 ", "cmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* cmp$_L$F1 $RB,$U6 */
  {
    ARC_INSN_CMP_L_U6_, "cmp_L_u6 ", "cmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* cmp$_L$F1 $RB,$RC */
  {
    ARC_INSN_CMP_L_R_R__RC, "cmp_L_r_r $RC", "cmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* cmp$Qcondi$F1 $RB,$RC */
  {
    ARC_INSN_CMP_CC__RC, "cmp_cc $RC", "cmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* cmp$_S $R_b,$Rh */
  {
    ARC_INSN_CMP_S_MCAH, "cmp_s_mcah", "cmp", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_H, 0 } } } }
  },
/* cmp$_S $R_b,$u7 */
  {
    ARC_INSN_CMP_S_R_U7, "cmp_s_r_u7", "cmp", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* rcmp$_L$F1 $RB,$s12 */
  {
    ARC_INSN_RCMP_L_S12_, "rcmp_L_s12 ", "rcmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rcmp$Qcondi$F1 $RB,$U6 */
  {
    ARC_INSN_RCMP_CCU6_, "rcmp_ccu6 ", "rcmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rcmp$_L$F1 $RB,$U6 */
  {
    ARC_INSN_RCMP_L_U6_, "rcmp_L_u6 ", "rcmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rcmp$_L$F1 $RB,$RC */
  {
    ARC_INSN_RCMP_L_R_R__RC, "rcmp_L_r_r $RC", "rcmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* rcmp$Qcondi$F1 $RB,$RC */
  {
    ARC_INSN_RCMP_CC__RC, "rcmp_cc $RC", "rcmp", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* rsub$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_RSUB_L_S12__RA_, "rsub_L_s12 $RA,", "rsub", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rsub$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_RSUB_CCU6__RA_, "rsub_ccu6 $RA,", "rsub", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rsub$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_RSUB_L_U6__RA_, "rsub_L_u6 $RA,", "rsub", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rsub$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_RSUB_L_R_R__RA__RC, "rsub_L_r_r $RA,$RC", "rsub", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* rsub$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_RSUB_CC__RA__RC, "rsub_cc $RA,$RC", "rsub", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bset$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_BSET_L_S12__RA_, "bset_L_s12 $RA,", "bset", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bset$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_BSET_CCU6__RA_, "bset_ccu6 $RA,", "bset", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bset$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_BSET_L_U6__RA_, "bset_L_u6 $RA,", "bset", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bset$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_BSET_L_R_R__RA__RC, "bset_L_r_r $RA,$RC", "bset", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bset$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_BSET_CC__RA__RC, "bset_cc $RA,$RC", "bset", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bset$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_BSET_S_SSB, "bset_s_ssb", "bset", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bclr$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_BCLR_L_S12__RA_, "bclr_L_s12 $RA,", "bclr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bclr$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_BCLR_CCU6__RA_, "bclr_ccu6 $RA,", "bclr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bclr$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_BCLR_L_U6__RA_, "bclr_L_u6 $RA,", "bclr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bclr$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_BCLR_L_R_R__RA__RC, "bclr_L_r_r $RA,$RC", "bclr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bclr$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_BCLR_CC__RA__RC, "bclr_cc $RA,$RC", "bclr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bclr$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_BCLR_S_SSB, "bclr_s_ssb", "bclr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* btst$_L$F1 $RB,$s12 */
  {
    ARC_INSN_BTST_L_S12_, "btst_L_s12 ", "btst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* btst$Qcondi$F1 $RB,$U6 */
  {
    ARC_INSN_BTST_CCU6_, "btst_ccu6 ", "btst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* btst$_L$F1 $RB,$U6 */
  {
    ARC_INSN_BTST_L_U6_, "btst_L_u6 ", "btst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* btst$_L$F1 $RB,$RC */
  {
    ARC_INSN_BTST_L_R_R__RC, "btst_L_r_r $RC", "btst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* btst$Qcondi$F1 $RB,$RC */
  {
    ARC_INSN_BTST_CC__RC, "btst_cc $RC", "btst", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* btst$_S $R_b,$u5 */
  {
    ARC_INSN_BTST_S_SSB, "btst_s_ssb", "btst", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* bxor$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_BXOR_L_S12__RA_, "bxor_L_s12 $RA,", "bxor", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bxor$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_BXOR_CCU6__RA_, "bxor_ccu6 $RA,", "bxor", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bxor$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_BXOR_L_U6__RA_, "bxor_L_u6 $RA,", "bxor", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bxor$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_BXOR_L_R_R__RA__RC, "bxor_L_r_r $RA,$RC", "bxor", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bxor$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_BXOR_CC__RA__RC, "bxor_cc $RA,$RC", "bxor", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bmsk$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_BMSK_L_S12__RA_, "bmsk_L_s12 $RA,", "bmsk", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bmsk$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_BMSK_CCU6__RA_, "bmsk_ccu6 $RA,", "bmsk", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bmsk$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_BMSK_L_U6__RA_, "bmsk_L_u6 $RA,", "bmsk", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* bmsk$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_BMSK_L_R_R__RA__RC, "bmsk_L_r_r $RA,$RC", "bmsk", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bmsk$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_BMSK_CC__RA__RC, "bmsk_cc $RA,$RC", "bmsk", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* bmsk$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_BMSK_S_SSB, "bmsk_s_ssb", "bmsk", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add1$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADD1_L_S12__RA_, "add1_L_s12 $RA,", "add1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add1$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADD1_CCU6__RA_, "add1_ccu6 $RA,", "add1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add1$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADD1_L_U6__RA_, "add1_L_u6 $RA,", "add1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add1$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADD1_L_R_R__RA__RC, "add1_L_r_r $RA,$RC", "add1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add1$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADD1_CC__RA__RC, "add1_cc $RA,$RC", "add1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add1$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ADD1_S_GO, "I16_GO_ADD1_s_go", "add1", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add2$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADD2_L_S12__RA_, "add2_L_s12 $RA,", "add2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add2$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADD2_CCU6__RA_, "add2_ccu6 $RA,", "add2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add2$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADD2_L_U6__RA_, "add2_L_u6 $RA,", "add2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add2$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADD2_L_R_R__RA__RC, "add2_L_r_r $RA,$RC", "add2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add2$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADD2_CC__RA__RC, "add2_cc $RA,$RC", "add2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add2$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ADD2_S_GO, "I16_GO_ADD2_s_go", "add2", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* add3$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADD3_L_S12__RA_, "add3_L_s12 $RA,", "add3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add3$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADD3_CCU6__RA_, "add3_ccu6 $RA,", "add3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add3$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADD3_L_U6__RA_, "add3_L_u6 $RA,", "add3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* add3$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADD3_L_R_R__RA__RC, "add3_L_r_r $RA,$RC", "add3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add3$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADD3_CC__RA__RC, "add3_cc $RA,$RC", "add3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* add3$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ADD3_S_GO, "I16_GO_ADD3_s_go", "add3", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sub1$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SUB1_L_S12__RA_, "sub1_L_s12 $RA,", "sub1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub1$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SUB1_CCU6__RA_, "sub1_ccu6 $RA,", "sub1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub1$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SUB1_L_U6__RA_, "sub1_L_u6 $RA,", "sub1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub1$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SUB1_L_R_R__RA__RC, "sub1_L_r_r $RA,$RC", "sub1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub1$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SUB1_CC__RA__RC, "sub1_cc $RA,$RC", "sub1", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub2$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SUB2_L_S12__RA_, "sub2_L_s12 $RA,", "sub2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub2$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SUB2_CCU6__RA_, "sub2_ccu6 $RA,", "sub2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub2$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SUB2_L_U6__RA_, "sub2_L_u6 $RA,", "sub2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub2$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SUB2_L_R_R__RA__RC, "sub2_L_r_r $RA,$RC", "sub2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub2$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SUB2_CC__RA__RC, "sub2_cc $RA,$RC", "sub2", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub3$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SUB3_L_S12__RA_, "sub3_L_s12 $RA,", "sub3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub3$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SUB3_CCU6__RA_, "sub3_ccu6 $RA,", "sub3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub3$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SUB3_L_U6__RA_, "sub3_L_u6 $RA,", "sub3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sub3$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SUB3_L_R_R__RA__RC, "sub3_L_r_r $RA,$RC", "sub3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sub3$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SUB3_CC__RA__RC, "sub3_cc $RA,$RC", "sub3", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpy$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MPY_L_S12__RA_, "mpy_L_s12 $RA,", "mpy", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpy$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MPY_CCU6__RA_, "mpy_ccu6 $RA,", "mpy", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpy$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MPY_L_U6__RA_, "mpy_L_u6 $RA,", "mpy", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpy$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MPY_L_R_R__RA__RC, "mpy_L_r_r $RA,$RC", "mpy", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpy$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MPY_CC__RA__RC, "mpy_cc $RA,$RC", "mpy", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpyh$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MPYH_L_S12__RA_, "mpyh_L_s12 $RA,", "mpyh", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyh$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MPYH_CCU6__RA_, "mpyh_ccu6 $RA,", "mpyh", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyh$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MPYH_L_U6__RA_, "mpyh_L_u6 $RA,", "mpyh", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyh$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MPYH_L_R_R__RA__RC, "mpyh_L_r_r $RA,$RC", "mpyh", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpyh$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MPYH_CC__RA__RC, "mpyh_cc $RA,$RC", "mpyh", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpyhu$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MPYHU_L_S12__RA_, "mpyhu_L_s12 $RA,", "mpyhu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyhu$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MPYHU_CCU6__RA_, "mpyhu_ccu6 $RA,", "mpyhu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyhu$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MPYHU_L_U6__RA_, "mpyhu_L_u6 $RA,", "mpyhu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyhu$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MPYHU_L_R_R__RA__RC, "mpyhu_L_r_r $RA,$RC", "mpyhu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpyhu$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MPYHU_CC__RA__RC, "mpyhu_cc $RA,$RC", "mpyhu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpyu$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MPYU_L_S12__RA_, "mpyu_L_s12 $RA,", "mpyu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyu$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MPYU_CCU6__RA_, "mpyu_ccu6 $RA,", "mpyu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyu$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MPYU_L_U6__RA_, "mpyu_L_u6 $RA,", "mpyu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mpyu$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MPYU_L_R_R__RA__RC, "mpyu_L_r_r $RA,$RC", "mpyu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mpyu$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MPYU_CC__RA__RC, "mpyu_cc $RA,$RC", "mpyu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$_L$F0 [$RC_noilink] */
  {
    ARC_INSN_J_L_R_R___RC_NOILINK_, "j_L_r_r [$RC_noilink]", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$Qcondi$F0 [$RC_noilink] */
  {
    ARC_INSN_J_CC___RC_NOILINK_, "j_cc [$RC_noilink]", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$_L$F1F [$RC_ilink] */
  {
    ARC_INSN_J_L_R_R___RC_ILINK_, "j_L_r_r [$RC_ilink]", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$Qcondi$F1F [$RC_ilink] */
  {
    ARC_INSN_J_CC___RC_ILINK_, "j_cc [$RC_ilink]", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$_L$F0 $s12 */
  {
    ARC_INSN_J_L_S12_, "j_L_s12 ", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* j$Qcondi$F0 $U6 */
  {
    ARC_INSN_J_CCU6_, "j_ccu6 ", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* j$_L$F0 $U6 */
  {
    ARC_INSN_J_L_U6_, "j_L_u6 ", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* j$_S [$R_b] */
  {
    ARC_INSN_J_S, "j_s", "j", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* j$_S [$R31] */
  {
    ARC_INSN_J_S__S, "j_s$_S", "j", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* jeq$_S [$R31] */
  {
    ARC_INSN_J_SEQ__S, "j_seq$_S", "jeq", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* jne$_S [$R31] */
  {
    ARC_INSN_J_SNE__S, "j_sne$_S", "jne", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* j$_L$F0.d $s12 */
  {
    ARC_INSN_J_L_S12_D_, "j_L_s12.d ", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* j$Qcondi$F0.d $U6 */
  {
    ARC_INSN_J_CCU6_D_, "j_ccu6.d ", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* j$_L$F0.d $U6 */
  {
    ARC_INSN_J_L_U6_D_, "j_L_u6.d ", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* j$_L$F0.d [$RC] */
  {
    ARC_INSN_J_L_R_R_D___RC_, "j_L_r_r.d [$RC]", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$Qcondi$F0.d [$RC] */
  {
    ARC_INSN_J_CC_D___RC_, "j_cc.d [$RC]", "j", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* j$_S.d [$R_b] */
  {
    ARC_INSN_J_S_D, "j_s.d", "j", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* j$_S.d [$R31] */
  {
    ARC_INSN_J_S__S_D, "j_s$_S.d", "j", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* jl$_L$F0 $s12 */
  {
    ARC_INSN_JL_L_S12_, "jl_L_s12 ", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* jl$Qcondi$F0 $U6 */
  {
    ARC_INSN_JL_CCU6_, "jl_ccu6 ", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* jl$_L$F0 $U6 */
  {
    ARC_INSN_JL_L_U6_, "jl_L_u6 ", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* jl$_S [$R_b] */
  {
    ARC_INSN_JL_S, "jl_s", "jl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* jl$_L$F0 [$RC_noilink] */
  {
    ARC_INSN_JL_L_R_R___RC_NOILINK_, "jl_L_r_r [$RC_noilink]", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* jl$Qcondi$F0 [$RC_noilink] */
  {
    ARC_INSN_JL_CC___RC_NOILINK_, "jl_cc [$RC_noilink]", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* jl$_L$F0.d $s12 */
  {
    ARC_INSN_JL_L_S12_D_, "jl_L_s12.d ", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* jl$Qcondi$F0.d $U6 */
  {
    ARC_INSN_JL_CCU6_D_, "jl_ccu6.d ", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* jl$_L$F0.d $U6 */
  {
    ARC_INSN_JL_L_U6_D_, "jl_L_u6.d ", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* jl$_L$F0.d [$RC] */
  {
    ARC_INSN_JL_L_R_R_D___RC_, "jl_L_r_r.d [$RC]", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* jl$Qcondi$F0.d [$RC] */
  {
    ARC_INSN_JL_CC_D___RC_, "jl_cc.d [$RC]", "jl", 32,
    { 0|A(RELAXED)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* jl$_S.d [$R_b] */
  {
    ARC_INSN_JL_S_D, "jl_s.d", "jl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE)|A(COND_CTI)|A(DELAY_SLOT), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* lp$_L$F0 $s12x2 */
  {
    ARC_INSN_LP_L_S12_, "lp_L_s12 ", "lp", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* lp$Qcondi$F0 $U6x2 */
  {
    ARC_INSN_LPCC_CCU6, "lpcc_ccu6", "lp", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* flag$_L$F0 $s12 */
  {
    ARC_INSN_FLAG_L_S12_, "flag_L_s12 ", "flag", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* flag$Qcondi$F0 $U6 */
  {
    ARC_INSN_FLAG_CCU6_, "flag_ccu6 ", "flag", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* flag$_L$F0 $U6 */
  {
    ARC_INSN_FLAG_L_U6_, "flag_L_u6 ", "flag", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* flag$_L$F0 $RC */
  {
    ARC_INSN_FLAG_L_R_R__RC, "flag_L_r_r $RC", "flag", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* flag$Qcondi$F0 $RC */
  {
    ARC_INSN_FLAG_CC__RC, "flag_cc $RC", "flag", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* lr$_L$F0 $RB,[$RC] */
  {
    ARC_INSN_LR_L_R_R___RC_, "lr_L_r_r [$RC]", "lr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* lr$_L$F0 $RB,[$s12] */
  {
    ARC_INSN_LR_L_S12_, "lr_L_s12 ", "lr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* lr$_L$F0 $RB,[$U6] */
  {
    ARC_INSN_LR_L_U6_, "lr_L_u6 ", "lr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sr$_L$F0 $RB,[$RC] */
  {
    ARC_INSN_SR_L_R_R___RC_, "sr_L_r_r [$RC]", "sr", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* sr$_L$F0 $RB,[$s12] */
  {
    ARC_INSN_SR_L_S12_, "sr_L_s12 ", "sr", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sr$_L$F0 $RB,[$U6] */
  {
    ARC_INSN_SR_L_U6_, "sr_L_u6 ", "sr", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asl$_L$F $RB,$RC */
  {
    ARC_INSN_ASL_L_R_R__RC, "asl_L_r_r $RC", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* asl$_L$F $RB,$U6 */
  {
    ARC_INSN_ASL_L_U6_, "asl_L_u6 ", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asl$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ASL_S_GO, "I16_GO_ASL_s_go", "asl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asr$_L$F $RB,$RC */
  {
    ARC_INSN_ASR_L_R_R__RC, "asr_L_r_r $RC", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* asr$_L$F $RB,$U6 */
  {
    ARC_INSN_ASR_L_U6_, "asr_L_u6 ", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asr$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ASR_S_GO, "I16_GO_ASR_s_go", "asr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* lsr$_L$F $RB,$RC */
  {
    ARC_INSN_LSR_L_R_R__RC, "lsr_L_r_r $RC", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* lsr$_L$F $RB,$U6 */
  {
    ARC_INSN_LSR_L_U6_, "lsr_L_u6 ", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* lsr$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_LSR_S_GO, "I16_GO_LSR_s_go", "lsr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ror$_L$F $RB,$RC */
  {
    ARC_INSN_ROR_L_R_R__RC, "ror_L_r_r $RC", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* ror$_L$F $RB,$U6 */
  {
    ARC_INSN_ROR_L_U6_, "ror_L_u6 ", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rrc$_L$F $RB,$RC */
  {
    ARC_INSN_RRC_L_R_R__RC, "rrc_L_r_r $RC", "rrc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* rrc$_L$F $RB,$U6 */
  {
    ARC_INSN_RRC_L_U6_, "rrc_L_u6 ", "rrc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sexb$_L$F $RB,$RC */
  {
    ARC_INSN_SEXB_L_R_R__RC, "sexb_L_r_r $RC", "sexb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* sexb$_L$F $RB,$U6 */
  {
    ARC_INSN_SEXB_L_U6_, "sexb_L_u6 ", "sexb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sexb$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_SEXB_S_GO, "I16_GO_SEXB_s_go", "sexb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* sexw$_L$F $RB,$RC */
  {
    ARC_INSN_SEXW_L_R_R__RC, "sexw_L_r_r $RC", "sexw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* sexw$_L$F $RB,$U6 */
  {
    ARC_INSN_SEXW_L_U6_, "sexw_L_u6 ", "sexw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* sexw$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_SEXW_S_GO, "I16_GO_SEXW_s_go", "sexw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* extb$_L$F $RB,$RC */
  {
    ARC_INSN_EXTB_L_R_R__RC, "extb_L_r_r $RC", "extb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* extb$_L$F $RB,$U6 */
  {
    ARC_INSN_EXTB_L_U6_, "extb_L_u6 ", "extb", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* extb$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_EXTB_S_GO, "I16_GO_EXTB_s_go", "extb", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* extw$_L$F $RB,$RC */
  {
    ARC_INSN_EXTW_L_R_R__RC, "extw_L_r_r $RC", "extw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* extw$_L$F $RB,$U6 */
  {
    ARC_INSN_EXTW_L_U6_, "extw_L_u6 ", "extw", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* extw$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_EXTW_S_GO, "I16_GO_EXTW_s_go", "extw", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* abs$_L$F $RB,$RC */
  {
    ARC_INSN_ABS_L_R_R__RC, "abs_L_r_r $RC", "abs", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* abs$_L$F $RB,$U6 */
  {
    ARC_INSN_ABS_L_U6_, "abs_L_u6 ", "abs", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* abs$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ABS_S_GO, "I16_GO_ABS_s_go", "abs", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* not$_L$F $RB,$RC */
  {
    ARC_INSN_NOT_L_R_R__RC, "not_L_r_r $RC", "not", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* not$_L$F $RB,$U6 */
  {
    ARC_INSN_NOT_L_U6_, "not_L_u6 ", "not", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* not$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_NOT_S_GO, "I16_GO_NOT_s_go", "not", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* rlc$_L$F $RB,$RC */
  {
    ARC_INSN_RLC_L_R_R__RC, "rlc_L_r_r $RC", "rlc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* rlc$_L$F $RB,$U6 */
  {
    ARC_INSN_RLC_L_U6_, "rlc_L_u6 ", "rlc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ex$_L$EXDi $RB,$RC */
  {
    ARC_INSN_EX_L_R_R__RC, "ex_L_r_r $RC", "ex", 32,
    { 0, { { { (1<<MACH_ARC700), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ex$_L$EXDi $RB,$U6 */
  {
    ARC_INSN_EX_L_U6_, "ex_L_u6 ", "ex", 32,
    { 0, { { { (1<<MACH_ARC700), 0 } }, { { LIMM_B, 0 } } } }
  },
/* neg$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_NEG_S_GO, "I16_GO_NEG_s_go", "neg", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* swi */
  {
    ARC_INSN_SWI, "swi", "swi", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* trap$_S $trapnum */
  {
    ARC_INSN_TRAP_S, "trap_s", "trap", 32,
    { 0|A(SHORT_P)|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* brk */
  {
    ARC_INSN_BRK, "brk", "brk", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* brk_s */
  {
    ARC_INSN_BRK_S, "brk_s", "brk_s", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asl$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ASL_L_S12__RA_, "asl_L_s12 $RA,", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asl$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ASL_CCU6__RA_, "asl_ccu6 $RA,", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asl$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ASL_L_U6__RA_, "asl_L_u6 $RA,", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asl$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ASL_L_R_R__RA__RC, "asl_L_r_r $RA,$RC", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asl$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ASL_CC__RA__RC, "asl_cc $RA,$RC", "asl", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asl$_S $R_c,$R_b,$u3 */
  {
    ARC_INSN_ASL_S_CBU3, "asl_s_cbu3", "asl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asl$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_ASL_S_SSB, "asl_s_ssb", "asl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asl$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ASLM_S_GO, "I16_GO_ASLM_s_go", "asl", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* lsr$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_LSR_L_S12__RA_, "lsr_L_s12 $RA,", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* lsr$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_LSR_CCU6__RA_, "lsr_ccu6 $RA,", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* lsr$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_LSR_L_U6__RA_, "lsr_L_u6 $RA,", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* lsr$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_LSR_L_R_R__RA__RC, "lsr_L_r_r $RA,$RC", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* lsr$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_LSR_CC__RA__RC, "lsr_cc $RA,$RC", "lsr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* lsr$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_LSR_S_SSB, "lsr_s_ssb", "lsr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* lsr$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_LSRM_S_GO, "I16_GO_LSRM_s_go", "lsr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asr$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ASR_L_S12__RA_, "asr_L_s12 $RA,", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asr$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ASR_CCU6__RA_, "asr_ccu6 $RA,", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asr$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ASR_L_U6__RA_, "asr_L_u6 $RA,", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asr$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ASR_L_R_R__RA__RC, "asr_L_r_r $RA,$RC", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asr$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ASR_CC__RA__RC, "asr_cc $RA,$RC", "asr", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asr$_S $R_c,$R_b,$u3 */
  {
    ARC_INSN_ASR_S_CBU3, "asr_s_cbu3", "asr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asr$_S $R_b,$R_b,$u5 */
  {
    ARC_INSN_ASR_S_SSB, "asr_s_ssb", "asr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* asr$_S $R_b,$R_b,$R_c */
  {
    ARC_INSN_I16_GO_ASRM_S_GO, "I16_GO_ASRM_s_go", "asr", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* ror$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ROR_L_S12__RA_, "ror_L_s12 $RA,", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ror$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ROR_CCU6__RA_, "ror_ccu6 $RA,", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ror$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ROR_L_U6__RA_, "ror_L_u6 $RA,", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* ror$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ROR_L_R_R__RA__RC, "ror_L_r_r $RA,$RC", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* ror$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ROR_CC__RA__RC, "ror_cc $RA,$RC", "ror", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mul64$_L$F1 $RB,$s12 */
  {
    ARC_INSN_MUL64_L_S12_, "mul64_L_s12 ", "mul64", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mul64$Qcondi$F1 $RB,$U6 */
  {
    ARC_INSN_MUL64_CCU6_, "mul64_ccu6 ", "mul64", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mul64$_L$F1 $RB,$U6 */
  {
    ARC_INSN_MUL64_L_U6_, "mul64_L_u6 ", "mul64", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mul64$_L$F1 $RB,$RC */
  {
    ARC_INSN_MUL64_L_R_R__RC, "mul64_L_r_r $RC", "mul64", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mul64$Qcondi$F1 $RB,$RC */
  {
    ARC_INSN_MUL64_CC__RC, "mul64_cc $RC", "mul64", 32,
    { 0|A(RELAXED), { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mul64$_S $R_b,$R_c */
  {
    ARC_INSN_MUL64_S_GO, "mul64_s_go", "mul64", 32,
    { 0|A(SHORT_P)|A(RELAXABLE), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* mulu64$_L$F1 $RB,$s12 */
  {
    ARC_INSN_MULU64_L_S12_, "mulu64_L_s12 ", "mulu64", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mulu64$Qcondi$F1 $RB,$U6 */
  {
    ARC_INSN_MULU64_CCU6_, "mulu64_ccu6 ", "mulu64", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mulu64$_L$F1 $RB,$U6 */
  {
    ARC_INSN_MULU64_L_U6_, "mulu64_L_u6 ", "mulu64", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mulu64$_L$F1 $RB,$RC */
  {
    ARC_INSN_MULU64_L_R_R__RC, "mulu64_L_r_r $RC", "mulu64", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mulu64$Qcondi$F1 $RB,$RC */
  {
    ARC_INSN_MULU64_CC__RC, "mulu64_cc $RC", "mulu64", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* adds$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADDS_L_S12__RA_, "adds_L_s12 $RA,", "adds", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* adds$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADDS_CCU6__RA_, "adds_ccu6 $RA,", "adds", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* adds$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADDS_L_U6__RA_, "adds_L_u6 $RA,", "adds", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* adds$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADDS_L_R_R__RA__RC, "adds_L_r_r $RA,$RC", "adds", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* adds$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADDS_CC__RA__RC, "adds_cc $RA,$RC", "adds", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* subs$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SUBS_L_S12__RA_, "subs_L_s12 $RA,", "subs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* subs$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SUBS_CCU6__RA_, "subs_ccu6 $RA,", "subs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* subs$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SUBS_L_U6__RA_, "subs_L_u6 $RA,", "subs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* subs$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SUBS_L_R_R__RA__RC, "subs_L_r_r $RA,$RC", "subs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* subs$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SUBS_CC__RA__RC, "subs_cc $RA,$RC", "subs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* divaw$_L$F0 $RB,$RB,$s12 */
  {
    ARC_INSN_DIVAW_L_S12__RA_, "divaw_L_s12 $RA,", "divaw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* divaw$Qcondi$F0 $RB,$RB,$U6 */
  {
    ARC_INSN_DIVAW_CCU6__RA_, "divaw_ccu6 $RA,", "divaw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* divaw$_L$F0 $RA,$RB,$U6 */
  {
    ARC_INSN_DIVAW_L_U6__RA_, "divaw_L_u6 $RA,", "divaw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* divaw$_L$F0 $RA,$RB,$RC */
  {
    ARC_INSN_DIVAW_L_R_R__RA__RC, "divaw_L_r_r $RA,$RC", "divaw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* divaw$Qcondi$F0 $RB,$RB,$RC */
  {
    ARC_INSN_DIVAW_CC__RA__RC, "divaw_cc $RA,$RC", "divaw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asls$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ASLS_L_S12__RA_, "asls_L_s12 $RA,", "asls", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asls$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ASLS_CCU6__RA_, "asls_ccu6 $RA,", "asls", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asls$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ASLS_L_U6__RA_, "asls_L_u6 $RA,", "asls", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asls$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ASLS_L_R_R__RA__RC, "asls_L_r_r $RA,$RC", "asls", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asls$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ASLS_CC__RA__RC, "asls_cc $RA,$RC", "asls", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asrs$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ASRS_L_S12__RA_, "asrs_L_s12 $RA,", "asrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asrs$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ASRS_CCU6__RA_, "asrs_ccu6 $RA,", "asrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asrs$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ASRS_L_U6__RA_, "asrs_L_u6 $RA,", "asrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* asrs$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ASRS_L_R_R__RA__RC, "asrs_L_r_r $RA,$RC", "asrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* asrs$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ASRS_CC__RA__RC, "asrs_cc $RA,$RC", "asrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* addsdw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_ADDSDW_L_S12__RA_, "addsdw_L_s12 $RA,", "addsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* addsdw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_ADDSDW_CCU6__RA_, "addsdw_ccu6 $RA,", "addsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* addsdw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_ADDSDW_L_U6__RA_, "addsdw_L_u6 $RA,", "addsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* addsdw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_ADDSDW_L_R_R__RA__RC, "addsdw_L_r_r $RA,$RC", "addsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* addsdw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_ADDSDW_CC__RA__RC, "addsdw_cc $RA,$RC", "addsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* subsdw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_SUBSDW_L_S12__RA_, "subsdw_L_s12 $RA,", "subsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* subsdw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_SUBSDW_CCU6__RA_, "subsdw_ccu6 $RA,", "subsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* subsdw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_SUBSDW_L_U6__RA_, "subsdw_L_u6 $RA,", "subsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* subsdw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_SUBSDW_L_R_R__RA__RC, "subsdw_L_r_r $RA,$RC", "subsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* subsdw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_SUBSDW_CC__RA__RC, "subsdw_cc $RA,$RC", "subsdw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* swap$_L$F $RB,$RC */
  {
    ARC_INSN_SWAP_L_R_R__RC, "swap_L_r_r $RC", "swap", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* swap$_L$F $RB,$U6 */
  {
    ARC_INSN_SWAP_L_U6_, "swap_L_u6 ", "swap", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* norm$_L$F $RB,$RC */
  {
    ARC_INSN_NORM_L_R_R__RC, "norm_L_r_r $RC", "norm", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* norm$_L$F $RB,$U6 */
  {
    ARC_INSN_NORM_L_U6_, "norm_L_u6 ", "norm", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* rnd16$_L$F $RB,$RC */
  {
    ARC_INSN_RND16_L_R_R__RC, "rnd16_L_r_r $RC", "rnd16", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* rnd16$_L$F $RB,$U6 */
  {
    ARC_INSN_RND16_L_U6_, "rnd16_L_u6 ", "rnd16", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* abssw$_L$F $RB,$RC */
  {
    ARC_INSN_ABSSW_L_R_R__RC, "abssw_L_r_r $RC", "abssw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* abssw$_L$F $RB,$U6 */
  {
    ARC_INSN_ABSSW_L_U6_, "abssw_L_u6 ", "abssw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* abss$_L$F $RB,$RC */
  {
    ARC_INSN_ABSS_L_R_R__RC, "abss_L_r_r $RC", "abss", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* abss$_L$F $RB,$U6 */
  {
    ARC_INSN_ABSS_L_U6_, "abss_L_u6 ", "abss", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* negsw$_L$F $RB,$RC */
  {
    ARC_INSN_NEGSW_L_R_R__RC, "negsw_L_r_r $RC", "negsw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* negsw$_L$F $RB,$U6 */
  {
    ARC_INSN_NEGSW_L_U6_, "negsw_L_u6 ", "negsw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* negs$_L$F $RB,$RC */
  {
    ARC_INSN_NEGS_L_R_R__RC, "negs_L_r_r $RC", "negs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* negs$_L$F $RB,$U6 */
  {
    ARC_INSN_NEGS_L_U6_, "negs_L_u6 ", "negs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* normw$_L$F $RB,$RC */
  {
    ARC_INSN_NORMW_L_R_R__RC, "normw_L_r_r $RC", "normw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_C, 0 } } } }
  },
/* normw$_L$F $RB,$U6 */
  {
    ARC_INSN_NORMW_L_U6_, "normw_L_u6 ", "normw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* nop_s */
  {
    ARC_INSN_NOP_S, "nop_s", "nop_s", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* unimp_s */
  {
    ARC_INSN_UNIMP_S, "unimp_s", "unimp_s", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* pop$_S $R_b */
  {
    ARC_INSN_POP_S_B, "pop_s_b", "pop", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* pop$_S $R31 */
  {
    ARC_INSN_POP_S_BLINK, "pop_s_blink", "pop", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* push$_S $R_b */
  {
    ARC_INSN_PUSH_S_B, "push_s_b", "push", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* push$_S $R31 */
  {
    ARC_INSN_PUSH_S_BLINK, "push_s_blink", "push", 32,
    { 0|A(SHORT_P), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/* mullw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MULLW_L_S12__RA_, "mullw_L_s12 $RA,", "mullw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mullw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MULLW_CCU6__RA_, "mullw_ccu6 $RA,", "mullw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mullw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MULLW_L_U6__RA_, "mullw_L_u6 $RA,", "mullw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mullw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MULLW_L_R_R__RA__RC, "mullw_L_r_r $RA,$RC", "mullw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mullw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MULLW_CC__RA__RC, "mullw_cc $RA,$RC", "mullw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* maclw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MACLW_L_S12__RA_, "maclw_L_s12 $RA,", "maclw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* maclw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MACLW_CCU6__RA_, "maclw_ccu6 $RA,", "maclw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* maclw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MACLW_L_U6__RA_, "maclw_L_u6 $RA,", "maclw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* maclw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MACLW_L_R_R__RA__RC, "maclw_L_r_r $RA,$RC", "maclw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* maclw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MACLW_CC__RA__RC, "maclw_cc $RA,$RC", "maclw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* machlw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MACHLW_L_S12__RA_, "machlw_L_s12 $RA,", "machlw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* machlw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MACHLW_CCU6__RA_, "machlw_ccu6 $RA,", "machlw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* machlw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MACHLW_L_U6__RA_, "machlw_L_u6 $RA,", "machlw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* machlw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MACHLW_L_R_R__RA__RC, "machlw_L_r_r $RA,$RC", "machlw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* machlw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MACHLW_CC__RA__RC, "machlw_cc $RA,$RC", "machlw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mululw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MULULW_L_S12__RA_, "mululw_L_s12 $RA,", "mululw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mululw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MULULW_CCU6__RA_, "mululw_ccu6 $RA,", "mululw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mululw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MULULW_L_U6__RA_, "mululw_L_u6 $RA,", "mululw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* mululw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MULULW_L_R_R__RA__RC, "mululw_L_r_r $RA,$RC", "mululw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* mululw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MULULW_CC__RA__RC, "mululw_cc $RA,$RC", "mululw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* machulw$_L$F $RB,$RB,$s12 */
  {
    ARC_INSN_MACHULW_L_S12__RA_, "machulw_L_s12 $RA,", "machulw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* machulw$Qcondi$F $RB,$RB,$U6 */
  {
    ARC_INSN_MACHULW_CCU6__RA_, "machulw_ccu6 $RA,", "machulw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* machulw$_L$F $RA,$RB,$U6 */
  {
    ARC_INSN_MACHULW_L_U6__RA_, "machulw_L_u6 $RA,", "machulw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_B, 0 } } } }
  },
/* machulw$_L$F $RA,$RB,$RC */
  {
    ARC_INSN_MACHULW_L_R_R__RA__RC, "machulw_L_r_r $RA,$RC", "machulw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/* machulw$Qcondi$F $RB,$RB,$RC */
  {
    ARC_INSN_MACHULW_CC__RA__RC, "machulw_cc $RA,$RC", "machulw", 32,
    { 0, { { { (1<<MACH_BASE), 0 } }, { { LIMM_BC, 0 } } } }
  },
/*  */
  {
    ARC_INSN_CURRENT_LOOP_END, "current_loop_end", "", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/*  */
  {
    ARC_INSN_CURRENT_LOOP_END_AFTER_BRANCH, "current_loop_end_after_branch", "", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
/*  */
  {
    ARC_INSN_ARC600_CURRENT_LOOP_END_AFTER_BRANCH, "arc600_current_loop_end_after_branch", "", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } }, { { LIMM_NONE, 0 } } } }
  },
};

#undef OP
#undef A

/* Initialize anything needed to be done once, before any cpu_open call.  */

static void
init_tables (void)
{
}

static const CGEN_MACH * lookup_mach_via_bfd_name (const CGEN_MACH *, const char *);
static void build_hw_table      (CGEN_CPU_TABLE *);
static void build_ifield_table  (CGEN_CPU_TABLE *);
static void build_operand_table (CGEN_CPU_TABLE *);
static void build_insn_table    (CGEN_CPU_TABLE *);
static void arc_cgen_rebuild_tables (CGEN_CPU_TABLE *);

/* Subroutine of arc_cgen_cpu_open to look up a mach via its bfd name.  */

static const CGEN_MACH *
lookup_mach_via_bfd_name (const CGEN_MACH *table, const char *name)
{
  while (table->name)
    {
      if (strcmp (name, table->bfd_name) == 0)
	return table;
      ++table;
    }
  abort ();
}

/* Subroutine of arc_cgen_cpu_open to build the hardware table.  */

static void
build_hw_table (CGEN_CPU_TABLE *cd)
{
  int i;
  int machs = cd->machs;
  const CGEN_HW_ENTRY *init = & arc_cgen_hw_table[0];
  /* MAX_HW is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_HW_ENTRY **selected =
    (const CGEN_HW_ENTRY **) xmalloc (MAX_HW * sizeof (CGEN_HW_ENTRY *));

  cd->hw_table.init_entries = init;
  cd->hw_table.entry_size = sizeof (CGEN_HW_ENTRY);
  memset (selected, 0, MAX_HW * sizeof (CGEN_HW_ENTRY *));
  /* ??? For now we just use machs to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_HW_ATTR_VALUE (&init[i], CGEN_HW_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->hw_table.entries = selected;
  cd->hw_table.num_entries = MAX_HW;
}

/* Subroutine of arc_cgen_cpu_open to build the hardware table.  */

static void
build_ifield_table (CGEN_CPU_TABLE *cd)
{
  cd->ifld_table = & arc_cgen_ifld_table[0];
}

/* Subroutine of arc_cgen_cpu_open to build the hardware table.  */

static void
build_operand_table (CGEN_CPU_TABLE *cd)
{
  int i;
  int machs = cd->machs;
  const CGEN_OPERAND *init = & arc_cgen_operand_table[0];
  /* MAX_OPERANDS is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_OPERAND **selected = xmalloc (MAX_OPERANDS * sizeof (* selected));

  cd->operand_table.init_entries = init;
  cd->operand_table.entry_size = sizeof (CGEN_OPERAND);
  memset (selected, 0, MAX_OPERANDS * sizeof (CGEN_OPERAND *));
  /* ??? For now we just use mach to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_OPERAND_ATTR_VALUE (&init[i], CGEN_OPERAND_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->operand_table.entries = selected;
  cd->operand_table.num_entries = MAX_OPERANDS;
}

/* Subroutine of arc_cgen_cpu_open to build the hardware table.
   ??? This could leave out insns not supported by the specified mach/isa,
   but that would cause errors like "foo only supported by bar" to become
   "unknown insn", so for now we include all insns and require the app to
   do the checking later.
   ??? On the other hand, parsing of such insns may require their hardware or
   operand elements to be in the table [which they mightn't be].  */

static void
build_insn_table (CGEN_CPU_TABLE *cd)
{
  int i;
  const CGEN_IBASE *ib = & arc_cgen_insn_table[0];
  CGEN_INSN *insns = xmalloc (MAX_INSNS * sizeof (CGEN_INSN));

  memset (insns, 0, MAX_INSNS * sizeof (CGEN_INSN));
  for (i = 0; i < MAX_INSNS; ++i)
    insns[i].base = &ib[i];
  cd->insn_table.init_entries = insns;
  cd->insn_table.entry_size = sizeof (CGEN_IBASE);
  cd->insn_table.num_init_entries = MAX_INSNS;
}

/* Subroutine of arc_cgen_cpu_open to rebuild the tables.  */

static void
arc_cgen_rebuild_tables (CGEN_CPU_TABLE *cd)
{
  int i;
  CGEN_BITSET *isas = cd->isas;
  unsigned int machs = cd->machs;

  cd->int_insn_p = CGEN_INT_INSN_P;

  /* Data derived from the isa spec.  */
#define UNSET (CGEN_SIZE_UNKNOWN + 1)
  cd->default_insn_bitsize = UNSET;
  cd->base_insn_bitsize = UNSET;
  cd->min_insn_bitsize = 65535; /* Some ridiculously big number.  */
  cd->max_insn_bitsize = 0;
  for (i = 0; i < MAX_ISAS; ++i)
    if (cgen_bitset_contains (isas, i))
      {
	const CGEN_ISA *isa = & arc_cgen_isa_table[i];

	/* Default insn sizes of all selected isas must be
	   equal or we set the result to 0, meaning "unknown".  */
	if (cd->default_insn_bitsize == UNSET)
	  cd->default_insn_bitsize = isa->default_insn_bitsize;
	else if (isa->default_insn_bitsize == cd->default_insn_bitsize)
	  ; /* This is ok.  */
	else
	  cd->default_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Base insn sizes of all selected isas must be equal
	   or we set the result to 0, meaning "unknown".  */
	if (cd->base_insn_bitsize == UNSET)
	  cd->base_insn_bitsize = isa->base_insn_bitsize;
	else if (isa->base_insn_bitsize == cd->base_insn_bitsize)
	  ; /* This is ok.  */
	else
	  cd->base_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Set min,max insn sizes.  */
	if (isa->min_insn_bitsize < cd->min_insn_bitsize)
	  cd->min_insn_bitsize = isa->min_insn_bitsize;
	if (isa->max_insn_bitsize > cd->max_insn_bitsize)
	  cd->max_insn_bitsize = isa->max_insn_bitsize;
      }

  /* Data derived from the mach spec.  */
  for (i = 0; i < MAX_MACHS; ++i)
    if (((1 << i) & machs) != 0)
      {
	const CGEN_MACH *mach = & arc_cgen_mach_table[i];

	if (mach->insn_chunk_bitsize != 0)
	{
	  if (cd->insn_chunk_bitsize != 0 && cd->insn_chunk_bitsize != mach->insn_chunk_bitsize)
	    {
	      fprintf (stderr, "arc_cgen_rebuild_tables: conflicting insn-chunk-bitsize values: `%d' vs. `%d'\n",
		       cd->insn_chunk_bitsize, mach->insn_chunk_bitsize);
	      abort ();
	    }

 	  cd->insn_chunk_bitsize = mach->insn_chunk_bitsize;
	}
      }

  /* Determine which hw elements are used by MACH.  */
  build_hw_table (cd);

  /* Build the ifield table.  */
  build_ifield_table (cd);

  /* Determine which operands are used by MACH/ISA.  */
  build_operand_table (cd);

  /* Build the instruction table.  */
  build_insn_table (cd);
}

/* Initialize a cpu table and return a descriptor.
   It's much like opening a file, and must be the first function called.
   The arguments are a set of (type/value) pairs, terminated with
   CGEN_CPU_OPEN_END.

   Currently supported values:
   CGEN_CPU_OPEN_ISAS:    bitmap of values in enum isa_attr
   CGEN_CPU_OPEN_MACHS:   bitmap of values in enum mach_attr
   CGEN_CPU_OPEN_BFDMACH: specify 1 mach using bfd name
   CGEN_CPU_OPEN_ENDIAN:  specify endian choice
   CGEN_CPU_OPEN_END:     terminates arguments

   ??? Simultaneous multiple isas might not make sense, but it's not (yet)
   precluded.

   ??? We only support ISO C stdargs here, not K&R.
   Laziness, plus experiment to see if anything requires K&R - eventually
   K&R will no longer be supported - e.g. GDB is currently trying this.  */

CGEN_CPU_DESC
arc_cgen_cpu_open (enum cgen_cpu_open_arg arg_type, ...)
{
  CGEN_CPU_TABLE *cd = (CGEN_CPU_TABLE *) xmalloc (sizeof (CGEN_CPU_TABLE));
  static int init_p;
  CGEN_BITSET *isas = 0;  /* 0 = "unspecified" */
  unsigned int machs = 0; /* 0 = "unspecified" */
  enum cgen_endian endian = CGEN_ENDIAN_UNKNOWN;
  va_list ap;

  if (! init_p)
    {
      init_tables ();
      init_p = 1;
    }

  memset (cd, 0, sizeof (*cd));

  va_start (ap, arg_type);
  while (arg_type != CGEN_CPU_OPEN_END)
    {
      switch (arg_type)
	{
	case CGEN_CPU_OPEN_ISAS :
	  isas = va_arg (ap, CGEN_BITSET *);
	  break;
	case CGEN_CPU_OPEN_MACHS :
	  machs = va_arg (ap, unsigned int);
	  break;
	case CGEN_CPU_OPEN_BFDMACH :
	  {
	    const char *name = va_arg (ap, const char *);
	    const CGEN_MACH *mach =
	      lookup_mach_via_bfd_name (arc_cgen_mach_table, name);

	    machs |= 1 << mach->num;
	    break;
	  }
	case CGEN_CPU_OPEN_ENDIAN :
	  endian = va_arg (ap, enum cgen_endian);
	  break;
	default :
	  fprintf (stderr, "arc_cgen_cpu_open: unsupported argument `%d'\n",
		   arg_type);
	  abort (); /* ??? return NULL? */
	}
      arg_type = va_arg (ap, enum cgen_cpu_open_arg);
    }
  va_end (ap);

  /* Mach unspecified means "all".  */
  if (machs == 0)
    machs = (1 << MAX_MACHS) - 1;
  /* Base mach is always selected.  */
  machs |= 1;
  if (endian == CGEN_ENDIAN_UNKNOWN)
    {
      /* ??? If target has only one, could have a default.  */
      fprintf (stderr, "arc_cgen_cpu_open: no endianness specified\n");
      abort ();
    }

  cd->isas = cgen_bitset_copy (isas);
  cd->machs = machs;
  cd->endian = endian;
  /* FIXME: for the sparc case we can determine insn-endianness statically.
     The worry here is where both data and insn endian can be independently
     chosen, in which case this function will need another argument.
     Actually, will want to allow for more arguments in the future anyway.  */
  cd->insn_endian = endian;

  /* Table (re)builder.  */
  cd->rebuild_tables = arc_cgen_rebuild_tables;
  arc_cgen_rebuild_tables (cd);

  /* Default to not allowing signed overflow.  */
  cd->signed_overflow_ok_p = 0;
  
  return (CGEN_CPU_DESC) cd;
}

/* Cover fn to arc_cgen_cpu_open to handle the simple case of 1 isa, 1 mach.
   MACH_NAME is the bfd name of the mach.  */

CGEN_CPU_DESC
arc_cgen_cpu_open_1 (const char *mach_name, enum cgen_endian endian)
{
  return arc_cgen_cpu_open (CGEN_CPU_OPEN_BFDMACH, mach_name,
			       CGEN_CPU_OPEN_ENDIAN, endian,
			       CGEN_CPU_OPEN_END);
}

/* Close a cpu table.
   ??? This can live in a machine independent file, but there's currently
   no place to put this file (there's no libcgen).  libopcodes is the wrong
   place as some simulator ports use this but they don't use libopcodes.  */

void
arc_cgen_cpu_close (CGEN_CPU_DESC cd)
{
  unsigned int i;
  const CGEN_INSN *insns;

  if (cd->macro_insn_table.init_entries)
    {
      insns = cd->macro_insn_table.init_entries;
      for (i = 0; i < cd->macro_insn_table.num_init_entries; ++i, ++insns)
	if (CGEN_INSN_RX ((insns)))
	  regfree (CGEN_INSN_RX (insns));
    }

  if (cd->insn_table.init_entries)
    {
      insns = cd->insn_table.init_entries;
      for (i = 0; i < cd->insn_table.num_init_entries; ++i, ++insns)
	if (CGEN_INSN_RX (insns))
	  regfree (CGEN_INSN_RX (insns));
    }  

  if (cd->macro_insn_table.init_entries)
    free ((CGEN_INSN *) cd->macro_insn_table.init_entries);

  if (cd->insn_table.init_entries)
    free ((CGEN_INSN *) cd->insn_table.init_entries);

  if (cd->hw_table.entries)
    free ((CGEN_HW_ENTRY *) cd->hw_table.entries);

  if (cd->operand_table.entries)
    free ((CGEN_HW_ENTRY *) cd->operand_table.entries);

  free (cd);
}

