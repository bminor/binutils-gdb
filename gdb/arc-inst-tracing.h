/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey <richard.stuckey@arc.com>

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
/*     This header file defines operations for recording an instruction trace */
/*     of successive PC values in a compressed binary format in a file.       */
/*                                                                            */
/*     We use the fact that instructions are always 16-bit aligned to encode  */
/*     17-bit deltas as 16-bit quantities.  We apply a 2-byte bias to the     */
/*     delta (as the minimum delta could be -2, for a shortest distance       */
/*     backward branch), so giving us                                         */
/*                                                                            */
/*         delta    range:  2 .. 2^17                                         */
/*         encoding range:  0 .. 2^16 - 1                                     */
/*                                                                            */
/*     We could encode positive and negative deltas differently (since the    */
/*     minimum positive delta is 10, given that 2, 4, 6 and 8 byte positive   */
/*     deltas are handled diferently), but this hardly seems worthwhile.      */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_INSTRUCTION_TRACING_H
#define ARC_INSTRUCTION_TRACING_H

/* ARC header files */
#include "arc-support.h"


typedef enum
{
    NO_CHANGE,
    PLUS_16_BITS,
    PLUS_32_BITS,
    PLUS_48_BITS,
    PLUS_64_BITS,
    DELTA_16_BIT_POSITIVE,
    DELTA_16_BIT_NEGATIVE,
    ABSOLUTE_31_BITS
} ARC_ProgramCounterEncoding;


typedef unsigned long long Ordinal;


#define MAX_DELTA   (1 << 17)

#define ENCODE_DELTA(delta)    (unsigned int) (((delta) - 2) / 2)
#define DECODE_DELTA(value)                   (((value) * 2) + 2)


Boolean arc_start_encoding (const char *filename,
                            Ordinal     first_instr_count);

void    arc_stop_encoding  (Ordinal last_instr_count);

Boolean arc_start_decoding (const char *filename,
                            Ordinal    *first_instr_count,
                            Ordinal    *last_instr_count);

void    arc_stop_decoding  (void);

void    arc_encode_PC (ARC_ProgramCounterEncoding  encoding, unsigned int  value);
Boolean arc_decode_PC (ARC_ProgramCounterEncoding *encoding, unsigned int *value);

#endif /* ARC_INSTRUCTION_TRACING_H */
/******************************************************************************/
