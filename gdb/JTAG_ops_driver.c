/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey <richard.stuckey@arc.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
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
/*     This module contains a test driver for the JTAG operations module of   */
/*     the ARC port of gdb.                                                   */
/*                                                                            */
/* Usage:                                                                     */
/*           <driver>  [ -c ] [ -d ] [ -r <count> ] [ -m ]                    */
/*                                                                            */
/*           where -c specifies target connection & disconnection only        */
/*                 -d switches on JTAG operation debuggging                   */
/*                 -r specifies the JTAG operation retry count                */
/*                 -m specifies testing memory operations only                */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "arc-jtag-ops.h"
#include "arc-memory.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef enum
{
    RO,    // read-only
    RW,    // read/write
    WO,    // write-only
    UU
} RegisterMode;


typedef struct
{
   const char*          name;
   ARC_RegisterNumber   regno;
   ARC_RegisterContents mask;
   RegisterMode         mode;
} RegisterInfo;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

// what should this be?
#define DATA_AREA      0x00001000
#define BUFFER_LENGTH         128

#define ARC_NR_CORE_REGS       32


#undef  RBCR
#define RAUX(name, hwregno, desc, gdbregno, mask, mode, version) { #name, hwregno, mask, mode },
static const RegisterInfo aux[] =
{
   #include "arc-regnums-defs.h"
};

#undef  RAUX
#define RBCR(name, hwregno, desc, gdbregno, mask, mode, version) { #name, hwregno, mask, mode },
static const RegisterInfo bcr[] =
{
    #include "arc-regnums-defs.h"
};


static Boolean          test        = TRUE;
static Boolean          memory_only = FALSE;
static TargetOperations operations;


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* global debug flag */
Boolean arc_debug_target;


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

static void failed(const char* fmt, ...)
{
    va_list ap;

    fprintf(stderr, "*** FAILED: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");

//  exit(EXIT_FAILURE);
}


static void read_core_registers(void)
{
    ARC_RegisterNumber r;

    for (r = 0; r < ARC_NR_CORE_REGS; r++)
    {
        ARC_RegisterContents contents;
        JTAG_OperationStatus status = arc_jtag_ops.read_core_reg(r, &contents);

        if (status == JTAG_SUCCESS)
            printf("core register %02d: 0x%08X\n", r, contents);
        else
            failed("could not read core register %02d: %d", r, (int) status);
    }
}


static void write_core_registers(void)
{
    ARC_RegisterNumber r;

    for (r = 0; r < ARC_NR_CORE_REGS; r++)
    {
        ARC_RegisterContents contents = 0xDEADBEEF + r;
        JTAG_OperationStatus status   = arc_jtag_ops.write_core_reg(r, contents);

        if (status == JTAG_SUCCESS)
        {
            ARC_RegisterContents new_contents;

            status = arc_jtag_ops.read_core_reg(r, &new_contents);

            if (status == JTAG_SUCCESS)
            {
                if (new_contents != contents)
                    failed("discrepancy in core register %02d contents: 0x%08X != 0x%08X", r, contents, new_contents);
            }
            else
               failed("could not read back core register %02d: %d", r, (int) status);
        }
        else
            failed("could not write core register %02d: %d", r, (int) status);
    }
}


static void read_auxiliary_registers(void)
{
    unsigned int i;

    for (i = 0; i < ELEMENTS_IN_ARRAY(aux); i++)
    {
        const RegisterInfo* info = &aux[i];

        if (info->mode != WO)
        {
            ARC_RegisterContents contents;
            JTAG_OperationStatus status = arc_jtag_ops.read_aux_reg(info->regno, &contents);

            if (status == JTAG_SUCCESS)
                printf("aux register 0x%03x (%-15s): 0x%08X\n", info->regno, info->name, contents);
            else
                failed("could not read aux register 0x%03x (%s): %d", info->regno, info->name, (int) status);
        }
    }
}


static void write_auxiliary_registers(void)
{
    unsigned int i;

    for (i = 0; i < ELEMENTS_IN_ARRAY(aux); i++)
    {
  const RegisterInfo* info = &aux[i];

        if (info->mode != RO)
        {
            ARC_RegisterContents contents = 0xFFFFFFFF;
            JTAG_OperationStatus status   = arc_jtag_ops.write_aux_reg(info->regno, contents);

            if (status == JTAG_SUCCESS)
            {
                if (info->mode != WO)
                {
                    ARC_RegisterContents new_contents;

                    status = arc_jtag_ops.read_aux_reg(info->regno, &new_contents);

                    if (status == JTAG_SUCCESS)
                    {
                        ARC_RegisterContents masked_contents     = contents     & info->mask;
                        ARC_RegisterContents masked_new_contents = new_contents & info->mask;

                        if (masked_new_contents != masked_contents)
                            failed("discrepancy in aux register %03x (%s) contents: 0x%08X != 0x%08X",
                                   info->regno, info->name, masked_contents, masked_new_contents);
                    }
                    else
                       failed("could not read back aux register 0x%03x (%s): %d", info->regno, info->name, (int) status);
                }
            }
            else
                failed("could not write aux register 0x%03x (%s): %d", info->regno, info->name, (int) status);
        }
    }
}


static void read_build_configuration_registers(void)
{
    unsigned int i;

    for (i = 0; i < ELEMENTS_IN_ARRAY(bcr); i++)
    {
        const RegisterInfo* info = &bcr[i];

        /* if the register is not unused */
        if (info->mode != UU)
        {
            ARC_RegisterContents contents;
            JTAG_OperationStatus status = arc_jtag_ops.read_aux_reg(info->regno, &contents);

            if (status == JTAG_SUCCESS)
                printf("BCR 0x%02x (%-16s): 0x%08X\n", info->regno, info->name, contents);
            else
                failed("could not read BCR 0x%02x (%s): %d", info->regno, info->name, (int) status);
        }
    }
}


/* these functions should NOT be used within this module: they are intended
 * purely for use by the arc-memory module for reading/writing multiple words
 * of data at word-aligned addresses
 */

static unsigned int read_jtag_words(ARC_Address  address,
                                    ARC_Byte*    data,
                                    unsigned int words)
{
    DEBUG("reading %u words from 0x%08X in xISS\n", words, address);

    assert(IS_WORD_ALIGNED(address));

    return arc_jtag_ops.memory_read_chunk(address, data, words);
}


static unsigned int write_jtag_words(ARC_Address  address,
                                     ARC_Byte*    data,
                                     unsigned int words)
{
    assert(IS_WORD_ALIGNED(address));

    DEBUG("writing %u words to 0x%08X in xISS\n", words, address);

    return arc_jtag_ops.memory_write_chunk(address, data, words);
}


static unsigned int write_jtag_pattern(ARC_Address  address,
                                       ARC_Word     pattern,
                                       unsigned int words)
{
    assert(IS_WORD_ALIGNED(address));

    DEBUG("writing pattern 0x%08X repeated %u times to 0x%08X in xISS\n", pattern, words, address);

    return arc_jtag_ops.memory_write_pattern(address, pattern, words);
}


/* these functions should be used within this module for all memory accesses */
static unsigned int read_chunk(ARC_Address addr, ARC_Byte* data, unsigned int bytes)
{
    unsigned int total_read;

    ENTERARGS("addr 0x%08X, bytes %u", addr, bytes);

    total_read = arc_read_memory(&operations, addr, data, bytes);

    DEBUG("read %u bytes\n", total_read);

    return total_read;
}


static unsigned int write_chunk(ARC_Address addr, ARC_Byte* data, unsigned int bytes)
{
    unsigned int total_written;

    ENTERARGS("addr 0x%08X, bytes %u", addr, bytes);

    total_written = arc_write_memory(&operations, addr, data, bytes);

    DEBUG("written %u bytes\n", total_written);

    return total_written;
}


/* write a repeated pattern of data to memory;
 * the start of each pattern is always word-aligned, so if the given address is
 * not word-aligned, the first partial word written will contain trailing bytes
 * of the pattern
 */
static unsigned int write_pattern(ARC_Address addr, ARC_Word pattern, unsigned int bytes)
{
    unsigned int total_written;

    ENTERARGS("addr 0x%08X, pattern 0x%08X, bytes %u", addr, pattern, bytes);

    total_written = arc_write_pattern(&operations, addr, pattern, bytes);

    DEBUG("written %u bytes\n", total_written);

    return total_written;
}


/* test word read/write operations - write several words to different
 * addresses to ensure that JTAG Data Register does not still hold first
 * word; use both contiguous and non-contiguous words to check that
 * JTAG Address Register is loaded correctly
 */
static void read_write_memory_words(void)
{
    const ARC_Word write1 = 0xCAFEBABE;
    const ARC_Word write2 = 0xDEADBEEF;
    const ARC_Word write3 = 0xBABEFACE;
    const ARC_Word write4 = 0x12345678;
    ARC_Word       read1  = 0;
    ARC_Word       read2  = 0;
    ARC_Word       read3  = 0;
    ARC_Word       read4  = 0;
    unsigned int   bytes;

    bytes = arc_jtag_ops.memory_write_word(DATA_AREA, write1);
    if (bytes != 4)
        failed("memory word 1 write: %d", bytes);

    bytes = arc_jtag_ops.memory_write_word(DATA_AREA + 4, write2);
    if (bytes != 4)
        failed("memory word 2 write: %d", bytes);

    bytes = arc_jtag_ops.memory_write_word(DATA_AREA + 8, write3);
    if (bytes != 4)
        failed("memory word 3 write: %d", bytes);

    bytes = arc_jtag_ops.memory_write_word(DATA_AREA + 20, write4);
    if (bytes != 4)
        failed("memory word 4 write: %d", bytes);

    bytes = arc_jtag_ops.memory_read_word(DATA_AREA, &read1);
    if (bytes != 4)
        failed("memory word 1 read: %d", bytes);

    bytes = arc_jtag_ops.memory_read_word(DATA_AREA + 4, &read2);
    if (bytes != 4)
        failed("memory word 2 read: %d", bytes);

    bytes = arc_jtag_ops.memory_read_word(DATA_AREA + 8, &read3);
    if (bytes != 4)
        failed("memory word 3 read: %d", bytes);

    bytes = arc_jtag_ops.memory_read_word(DATA_AREA + 20, &read4);
    if (bytes != 4)
        failed("memory word 4 read: %d", bytes);

    if (read1 != write1)
        failed("word 1: %08X != %08X", read1, write1);

    if (read2 != write2)
        failed("word 2: %08X != %08X", read2, write2);

    if (read3 != write3)
        failed("word 3: %08X != %08X", read3, write3);

    if (read3 != write3)
        failed("word 3: %08X != %08X", read3, write3);

    if (read4 != write4)
        failed("word 4: %08X != %08X", read4, write4);
}


/* test chunk read/write operations */
static void read_write_memory_chunks(void)
{
    ARC_Byte     in [BUFFER_LENGTH];
    ARC_Byte     out[BUFFER_LENGTH];
    unsigned int bytes;
    unsigned int i;
    unsigned int offset;

    /* initialize output buffer to 7 bit values */
    for (i = 0; i < BUFFER_LENGTH; i++)
        out[i] = (ARC_Byte) i;

    /* ----------------------------------------------------------- */
    /*                       initialize data area                  */
    /* ----------------------------------------------------------- */

    /* write series of complete words */

    bytes = write_chunk(DATA_AREA, out, BUFFER_LENGTH);
    if (bytes != BUFFER_LENGTH)
        failed("memory chunk write: %d", bytes);

    /* and read them back */

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (bytes != BUFFER_LENGTH)
        failed("memory chunk read: %d", bytes);

    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk read/write:");


    /* ----------------------------------------------------------- */
    /*                       read operations                       */
    /* ----------------------------------------------------------- */

    /* 0) read single word */

    bytes = read_chunk(DATA_AREA, in, 4);
    if (bytes != 4)
        failed("memory chunk read word: %d", bytes);

    if (memcmp(in, out, 4) != 0)
        failed("memory chunk read word");

    /* 1) read leading bytes */

    for (i = 1; i <= 3; i++)
    {
        bytes = read_chunk(DATA_AREA + 4 - i, in, i);
        if (bytes != i)
            failed("memory chunk read leading bytes: %d", bytes);

        if (memcmp(in, out + 4 - i, i) != 0)
            failed("memory chunk read leading bytes");
    }

    /* 2) read trailing bytes */

    for (i = 1; i <= 3; i++)
    {
        bytes = read_chunk(DATA_AREA, in, i);
        if (bytes != i)
            failed("memory chunk read trailing bytes: %d", bytes);

        if (memcmp(in, out, i) != 0)
            failed("memory chunk read trailing bytes");
    }

    /* 3) read leading bytes and series of complete words */

    bytes = read_chunk(DATA_AREA + 1, in, 11);
    if (bytes != 11)
        failed("memory chunk read leading bytes and words: %d", bytes);

    if (memcmp(in, out + 1, 11) != 0)
        failed("memory chunk read leading bytes and words");

    /* 4) read series of complete words and trailing bytes */

    bytes = read_chunk(DATA_AREA, in, 11);
    if (bytes != 11)
        failed("memory chunk read words and trailing bytes: %d", bytes);

    if (memcmp(in, out, 11) != 0)
        failed("memory chunk read words and trailing bytes");

    /* 5) read leading bytes and trailing bytes */

    bytes = read_chunk(DATA_AREA + 1, in, 5);
    if (bytes != 5)
        failed("memory chunk read leading and trailing bytes: %d", bytes);

    if (memcmp(in, out + 1, 5) != 0)
        failed("memory chunk read leading and trailing bytes");

    /* 6) read leading bytes, series of complete words and trailing bytes */

    bytes = read_chunk(DATA_AREA + 2, in, 23);
    if (bytes != 23)
        failed("memory chunk read leading bytes, words and trailing bytes: %d", bytes);

    if (memcmp(in, out + 2, 23) != 0)
        failed("memory chunk read leading bytes, words and trailing bytes");

    /* 7) pathological cases: bytes in middle of word */

    bytes = read_chunk(DATA_AREA + 1, in, 1);
    if (bytes != 1)
        failed("memory chunk read bytes in middle (1) : %d", bytes);

    if (memcmp(in, out + 1, 1) != 0)
        failed("memory chunk read middle bytes (1)");

    bytes = read_chunk(DATA_AREA + 2, in, 2);
    if (bytes != 2)
        failed("memory chunk read bytes in middle (2) : %d", bytes);

    if (memcmp(in, out + 2, 2) != 0)
        failed("memory chunk read middle bytes (2)");

    bytes = read_chunk(DATA_AREA + 3, in, 1);
    if (bytes != 1)
        failed("memory chunk read bytes in middle (3) : %d", bytes);

    if (memcmp(in, out + 3, 1) != 0)
        failed("memory chunk read middle bytes (3)");


    /* ----------------------------------------------------------- */
    /*                       write operations                      */
    /* ----------------------------------------------------------- */

    /* on each test, read back the whole area: we must check that no other data
     * in the area has been changed, hence the use of 8-bit values on the write
     * to distinguish them from the 7-bit values used to initialise the area;
     * also, a different section of the area is used for each test
     */

    /* 0) write single word */

    out[0] = 45;
    out[1] = 89;
    out[2] = 66;
    out[3] = 53;

    bytes = write_chunk(DATA_AREA, out, 4);
    if (bytes != 4)
        failed("memory chunk write word: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, 4);
    if (memcmp(in, out, 4) != 0)
        failed("memory chunk write word");

    /* 1) write leading bytes */

    for (i = 1; i <= 3; i++)
    {
       offset = 8 - i;

        /* change data in output buffer to 8-bit values */
        *(out + offset) = 128 + i;

        bytes = write_chunk(DATA_AREA + offset, out + offset, i);
        if (bytes != i)
            failed("memory chunk write leading bytes: %d", bytes);

        bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
        if (memcmp(in, out, BUFFER_LENGTH) != 0)
            failed("memory chunk write leading bytes");
    }

    /* 2) write trailing bytes */

    offset = 8;

    for (i = 1; i <= 3; i++)
    {
        /* change data in output buffer to 8-bit values */
        *(out + offset + i - 1) = 128 + i;

        bytes = write_chunk(DATA_AREA + offset, out + offset, i);
        if (bytes != i)
            failed("memory chunk write trailing bytes: %d", bytes);

        bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
        if (memcmp(in, out, BUFFER_LENGTH) != 0)
            failed("memory chunk write trailing bytes");
    }

    /* 3) write leading bytes and series of complete words */

    offset = 13;

    for (i = 0; i < 11; i++)
        *(out + offset + i) = 128 + i;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 11);
    if (bytes != 11)
        failed("memory chunk write leading bytes and words: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write leading bytes and words");

    /* 4) write series of complete words and trailing bytes */

    offset = 28;

    for (i = 0; i < 11; i++)
        *(out + offset + i) = 128 + i;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 11);
    if (bytes != 11)
        failed("memory chunk write words and trailing bytes: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write words and trailing bytes");

    /* 5) write leading bytes and trailing bytes */

    offset = 40;

    for (i = 0; i < 5; i++)
        *(out + offset + i) = 128 + i;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 5);
    if (bytes != 5)
        failed("memory chunk write leading and trailing bytes: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write leading and trailing bytes");

    /* 6) write leading bytes, series of complete words and trailing bytes */

    offset = 48;

    for (i = 0; i < 23; i++)
        *(out + offset + i) = 128 + i;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 23);
    if (bytes != 23)
        failed("memory chunk write leading bytes, words and trailing bytes: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write leading bytes, words and trailing bytes");

    /* 7) pathological cases: bytes in middle of word */

    offset = 85;
    *(out + offset) = 128;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 1);
    if (bytes != 1)
        failed("memory chunk write bytes in middle (1) : %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write middle bytes (1)");

    offset = 95;
    *(out + offset)     = 129;
    *(out + offset + 1) = 130;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 2);
    if (bytes != 2)
        failed("memory chunk write bytes in middle (2) : %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write middle bytes (2)");

    offset = 106;
    *(out + offset) = 131;

    bytes = write_chunk(DATA_AREA + offset, out + offset, 1);
    if (bytes != 1)
        failed("memory chunk write bytes in middle (3) : %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write middle bytes (3)");


    /* ----------------------------------------------------------- */
    /*                       write zeroes operation                */
    /* ----------------------------------------------------------- */

    offset = 110;

    for (i = 0; i < 15; i++)
        *(out + offset + i) = 0;

    bytes = write_pattern(DATA_AREA + offset, 0, 15);
    if (bytes != 15)
        failed("memory zero: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory zero");


    /* ----------------------------------------------------------- */
    /*                       write pattern operation               */
    /* ----------------------------------------------------------- */

    offset = 38;

    /* the pattern will be word-aligned */

#ifdef TARGET_IS_BIG_ENDIAN
    *(out + offset +  0) = 0xBE;   // leading bytes
    *(out + offset +  1) = 0xEF;

    *(out + offset +  2) = 0xDE;   // word 0
    *(out + offset +  3) = 0xAD;
    *(out + offset +  4) = 0xBE;
    *(out + offset +  5) = 0xEF;

    *(out + offset +  6) = 0xDE;   // word 1
    *(out + offset +  7) = 0xAD;
    *(out + offset +  8) = 0xBE;
    *(out + offset +  9) = 0xEF;

    *(out + offset + 10) = 0xDE;   // word 2
    *(out + offset + 11) = 0xAD;
    *(out + offset + 12) = 0xBE;
    *(out + offset + 13) = 0xEF;

    *(out + offset + 14) = 0xDE;   // word 3
    *(out + offset + 15) = 0xAD;
    *(out + offset + 16) = 0xBE;
    *(out + offset + 17) = 0xEF;

    *(out + offset + 18) = 0xDE;   // trailing bytes
    *(out + offset + 19) = 0xAD;
#else
    *(out + offset +  0) = 0xAD;   // leading bytes
    *(out + offset +  1) = 0xDE;

    *(out + offset +  5) = 0xDE;   // word 0
    *(out + offset +  4) = 0xAD;
    *(out + offset +  3) = 0xBE;
    *(out + offset +  2) = 0xEF;

    *(out + offset +  9) = 0xDE;   // word 1
    *(out + offset +  8) = 0xAD;
    *(out + offset +  7) = 0xBE;
    *(out + offset +  6) = 0xEF;

    *(out + offset + 13) = 0xDE;   // word 2
    *(out + offset + 12) = 0xAD;
    *(out + offset + 11) = 0xBE;
    *(out + offset + 10) = 0xEF;

    *(out + offset + 17) = 0xDE;   // word 3
    *(out + offset + 16) = 0xAD;
    *(out + offset + 15) = 0xBE;
    *(out + offset + 14) = 0xEF;

    *(out + offset + 18) = 0xEF;   // trailing bytes
    *(out + offset + 19) = 0xBE;
#endif

    bytes = write_pattern(DATA_AREA + offset, 0xDEADBEEF, 20);
    if (bytes != 20)
        failed("memory pattern: %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory pattern");

    /* ----------------------------------------------------------- */
    /*                       write repeated values operation       */
    /* ----------------------------------------------------------- */

    /* initialize output buffer to the same value */
    for (i = 0; i < BUFFER_LENGTH; i++)
        out[i] = 0xA;

    bytes = write_chunk(DATA_AREA, out, BUFFER_LENGTH);
    if (bytes != BUFFER_LENGTH)
        failed("memory chunk write repeated values : %d", bytes);

    bytes = read_chunk(DATA_AREA, in, BUFFER_LENGTH);
    if (memcmp(in, out, BUFFER_LENGTH) != 0)
        failed("memory chunk write repeated values");
}


static void run_tests(void)
{
    if (!memory_only)
    {
        read_core_registers();
        read_auxiliary_registers();
        read_build_configuration_registers();

        write_core_registers();
        write_auxiliary_registers();
    }

    read_write_memory_words();
    read_write_memory_chunks();
}


static void process_options(int argc, char** argv)
{
    int c;

    while ((c = getopt (argc, argv, "mdcr:")) != -1)
    {
        switch (c)
        {
            case 'd':
                arc_jtag_ops.state_machine_debug = TRUE;
                break;
            case 'r':
                arc_jtag_ops.retry_count = atoi(optarg);
                break;
            case 'c':
                test = FALSE;
                break;
            case 'm':
                memory_only = TRUE;
                break;
            default:
                fprintf(stderr, "Usage: %s [ -d ]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

extern void _initialize_arc_jtag_ops(void);


int main(int argc, char** argv)
{
    Boolean opened;

    printf("Starting test of ARC JTAG interface...\n");

    _initialize_arc_jtag_ops();

    process_options(argc, argv);

    opened = arc_jtag_ops.open();

    if (opened)
    {
        printf("ARC processor is connected\n");

        if (test)
        {
            operations.read_core_register       = NULL;
            operations.write_core_register      = NULL;
            operations.read_auxiliary_register  = NULL;
            operations.write_auxiliary_register = NULL;
            operations.read_memory              = read_jtag_words;
            operations.write_memory             = write_jtag_words;
            operations.fill_memory              = write_jtag_pattern;

            run_tests();
            printf("resetting board...\n");
            arc_jtag_ops.reset_board();
            printf("board reset\n");
            run_tests();
        }

        arc_jtag_ops.close();
    }

    printf("Finished test of ARC JTAG interface\n");

    return 0;
}

/******************************************************************************/
