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
/*     This module implements operations for recording an instruction trace   */
/*     of successive PC values in a compressed binary format in a file.       */
/*                                                                            */
/*     The file is regarded as a linear sequence of bits, of the form:        */
/*                                                                            */
/*        <64-bit first instruction count>                                    */
/*        <64-bit last  instruction count>                                    */
/*        { <3-bit-code> [ <16-bit-data> | <31-bit-data> ] }                  */
/*        [ <zero-pad-bits> ]                                                 */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

/* gdb header files */
#include "defs.h"

/* ARC header files */
#include "arc-inst-tracing.h"
#include "arc-tdep.h"


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define MAX_ORDINAL    0xFFFFFFFFFFFFFFFFULL

/* The file that is currently open for read or write.  */
static FILE *file;

/* The byte of data that has been read from, or is about to be written to, the
   currently open file; and the number of bits in that byte that have been used
   (i.e. been consumed on input, or produced on output).  */
static unsigned char byte;
static unsigned int  bits_used;

/* Set to TRUE if EOF occurs whilst trying to read from the file.  */
static Boolean EOF_detected;


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* Write the N least-significant bits of the given value to the file.  */

static void
output_bits (unsigned int N, unsigned int value)
{
    unsigned int i;

    for (i = 0; i < N; i++)
    {
        if (bits_used == BITS_IN_BYTE)
        {
            (void) putc((int) byte, file);
            byte      = 0;
            bits_used = 0;
//          printf("byte = %02X\n", byte);
        }

        byte <<= 1;
        byte |= (value >> (N - i - 1)) & 0x1;
        bits_used++;
    }
}


/* Read N bits from the file and return them as the least-significant bits of a value.  */

static unsigned int
input_bits (unsigned int N)
{
    unsigned int value = 0;
    unsigned int i;

    for (i = 0; i < N; i++)
    {
        unsigned int bit;

        /* If all the bits in the current input byte have been used.  */
        if (bits_used == BITS_IN_BYTE)
        {
            /* Read the next byte from the file.  */
            int input = getc(file);

            if (input == EOF)
            {
                EOF_detected = TRUE;
                return 0;
            }

            byte      = (unsigned char) input;
            bits_used = 0;
//          printf("byte = %02X\n", byte);
        }

        bit = (unsigned int) ((byte & 0x80) >> 7);
        value <<= 1;
        value |= bit;
        byte <<= 1;
        bits_used++;
    }

    return value;
}


/* Write a 64-bit value to the file.  */

static void
output_ordinal (Ordinal value)
{
    output_bits(32, (unsigned int) (value >> 32));
    output_bits(32, (unsigned int) (value &  0xFFFFFFFF));
}


/* Read a 64-bit value from the file.  */

static Ordinal
input_ordinal (void)
{
    unsigned int high = input_bits(32);
    unsigned int low  = input_bits(32);

    return ((Ordinal) high) << 32 | (Ordinal) low;
}


/* Open the named file with the given access mode.  */

static Boolean
open_file (const char *filename, const char *mode)
{
    file = fopen(filename, mode);

    if (file == NULL)
    {
        fprintf(stderr, "Can not open file %s: %s\n", filename, strerror(errno));
        return FALSE;
    }

    return TRUE;
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Start encoding trace data into the named file.  */

Boolean
arc_start_encoding (const char *filename,
                    Ordinal     first_instr_count)
{
    ENTERARGS("filename = %s, first = %llu", filename, first_instr_count);

    byte      = 0;
    bits_used = 0;

    if (open_file(filename, "wb"))
    {
        /* The second value will be fixed up later.  */
        output_ordinal(first_instr_count);
        output_ordinal(MAX_ORDINAL);
        return TRUE;
    }

    return FALSE;
}


/* Stop encoding trace data into the file.  */

void
arc_stop_encoding (Ordinal last_instr_count)
{
    ENTERARGS("last = %llu", last_instr_count);

    if (file)
    {
        /* Make sure the last partial byte output is flushed to the file,
           padded with 0 bits as necessary.  */
        if (bits_used > 0)
            output_bits(BITS_IN_BYTE, 0);

         /* N.B. this is necessary!  */
         (void) fflush(file);

        /* Now fix up the second 8-byte value in tjhe file.  */

        if (lseek(fileno(file), (off_t) sizeof(Ordinal), SEEK_SET) == -1)
            warning(_("can not seek in file: %s"), strerror(errno));
        else
        {
            bits_used = 0;
            byte      = 0;
            output_ordinal(last_instr_count);

            /* Make sure the last byte is flushed.  */
            output_bits(1, 0);
         }

        (void) fclose(file);
        file = NULL;
    }
}


/* Start decoding trace data from the named file.
   Retrieve the first and last instruction ordinal positions.  */

Boolean
arc_start_decoding (const char *filename,
                    Ordinal    *first_instr_count,
                    Ordinal    *last_instr_count)
{
    ENTERARGS("filename = %s", filename);

    /* The first attempt to input a bit will result in a read from the file.  */
    bits_used    = BITS_IN_BYTE;
    EOF_detected = FALSE;

    if (open_file(filename, "r"))
    {
        *first_instr_count = input_ordinal();
        *last_instr_count  = input_ordinal();

        DEBUG("first = %llu, last = %llu\n", *first_instr_count, *last_instr_count);

        return !EOF_detected;
    }

    return FALSE;
}


/* Stop decoding trace data from the file.  */

void
arc_stop_decoding (void)
{
    if (file)
    {
        if (bits_used < BITS_IN_BYTE)
            fprintf(stderr, "all data not processed!\n");

        (void) fclose(file);
        file = NULL;
    }
}


/* Write a PC value into the file with the specified encoding.  */

void
arc_encode_PC (ARC_ProgramCounterEncoding encoding, unsigned int value)
{
//  printf("%d:%x\n", encoding, value);

    output_bits(3, (unsigned int) encoding);

    switch (encoding)
    {
        case NO_CHANGE:
        case PLUS_16_BITS:
        case PLUS_32_BITS:
        case PLUS_48_BITS:
        case PLUS_64_BITS:
            break;
        case DELTA_16_BIT_POSITIVE:
        case DELTA_16_BIT_NEGATIVE:
            output_bits(16, value);
            break;
        case ABSOLUTE_31_BITS:
            output_bits(31, value);
            break;
    }
}


/* Read a PC value and its encoding from the file.
   Return TRUE if a value could be read, FALSE otherwise.  */

Boolean
arc_decode_PC (ARC_ProgramCounterEncoding *encoding, unsigned int *value)
{
    unsigned int code = input_bits(3);

    if (EOF_detected)
        return FALSE;

    *encoding = (ARC_ProgramCounterEncoding) code;

    switch (*encoding)
    {
        case NO_CHANGE:
        case PLUS_16_BITS:
        case PLUS_32_BITS:
        case PLUS_48_BITS:
        case PLUS_64_BITS:
            *value = 0;
            break;
        case DELTA_16_BIT_POSITIVE:
        case DELTA_16_BIT_NEGATIVE:
            *value = input_bits(16);
            break;
        case ABSOLUTE_31_BITS:
            *value = input_bits(31);
            break;
    }

    if (EOF_detected)
        return FALSE;

//  printf("%d:%x\n", *encoding, *value);

    return TRUE;
}

/******************************************************************************/
