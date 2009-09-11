/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Author:
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
/*     This module implements operations for reading data from and writing    */
/*     data to target memory.                                                 */
/*                                                                            */
/*     The operations may be required to transfer arbitrary amounts of data   */
/*     to/from arbitrary addresses in memory; however, the supplied transfer  */
/*     operations must be assumed to be able only to transfer multiple words  */
/*     of data to/from word-aligned addresses.                                */
/*                                                                            */
/*     This gives the general case:                                           */
/*                                                                            */
/*                           word boundaries                                  */
/*                                  |                                         */
/*           -------------------------------------------------                */
/*          |         |         |         |         |         |               */
/*      -----------------------------------------------------------           */
/*          | g g L L | W W W W | W W W W | W W W W | T T e e |               */
/*      -----------------------------------------------------------           */
/*                ^                                                           */
/*                |-------------------------------------|                     */
/*                |                len                                        */
/*                addr                                                        */
/*                                                                            */
/*       where addr is the base address of the data to be transferred         */
/*             len  is the number of bytes of data to be transferred          */
/*             W denotes a byte that can be transfered in a whole word        */
/*             L denotes a leading byte                                       */
/*             T denotes a trailing byte                                      */
/*             g denotes a gap byte                                           */
/*             e denotes a end gap byte                                       */
/*                                                                            */
/*     There may be 0 .. BYTES_IN_WORD - 1 leading bytes, 0 or more whole     */
/*     words, and 0 .. BYTES_IN_WORD - 1 trailing bytes. If the given address */
/*     is word-aligned, there is no gap and hence no leading bytes.           */
/*                                                                            */
/*     There is also a pathological case:                                     */
/*                                                                            */
/*                           word boundaries                                  */
/*                                  |                                         */
/*                               ---------                                    */
/*                              |         |                                   */
/*      -----------------------------------------------------------           */
/*                              | g B B e |                                   */
/*      -----------------------------------------------------------           */
/*                                  ^                                         */
/*                                  |-|                                       */
/*                                  | len                                     */
/*                                  addr                                      */
/*                                                                            */
/*     where 1 .. BYTES_IN_WORD - 2 bytes of data in the middle of a word     */
/*     must be transfered.                                                    */
/*                                                                            */
/*     In a write operation, it is necessary to preserve the contents of the  */
/*     gap and end gap bytes, if any - this is done by first reading the word */
/*     which contains those bytes, constructing a new word which contains     */
/*     those bytes and the new bytes, and writing the new value back to that  */
/*     location.                                                              */
/*                                                                            */
/******************************************************************************/

/* system header files */

/* gdb header files */
#include "defs.h"

/* ARC header files */
#include "arc-memory.h"
#include "arc-tdep.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef struct
{
    unsigned int leading_bytes;
    unsigned int trailing_bytes;
    unsigned int words;
} Layout;


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* Split a memory chunk up into its layout - see diagram in file header.  */

static Layout
split (ARC_Address addr, unsigned int bytes)
{
    unsigned int gap = addr % BYTES_IN_WORD;
    Layout       layout;

    layout.leading_bytes  = (gap == 0) ? 0 : (BYTES_IN_WORD - gap);
    layout.trailing_bytes = (addr + bytes) % BYTES_IN_WORD;
    layout.words          = (bytes - layout.leading_bytes
                                   - layout.trailing_bytes) / BYTES_IN_WORD;

    DEBUG("%u leading bytes, %u words, %u trailing bytes\n",
          layout.leading_bytes, layout.words, layout.trailing_bytes);

    return layout;
}


/* Read part of a word of data from memory; the given address must be word-aligned.  */

static unsigned int
read_partial_word (TargetOperations *ops,
                   ARC_Address       addr,
                   ARC_Byte         *data,
                   unsigned int      bytes,
                   unsigned int      offset)  /* Offset of required bytes within word.  */
{
    ARC_Byte word[BYTES_IN_WORD];

    /* Read the word: only some bytes of this are required.  */
    if (ops->read_memory(addr, word, 1) > 0)
    {
        unsigned int i;

        for (i = 0; i < bytes; i++)
            data[i] = word[offset + i];

        /* Have read the specified number of bytes.  */
        return bytes;
    }

    /* Failed: no data read.  */
    return 0;
}


/* Write part of a word of data to memory; the given address must be word-aligned.  */

static unsigned int
write_partial_word (TargetOperations *ops,
                    ARC_Address       addr,
                    ARC_Byte         *data,
                    unsigned int      bytes,
                    unsigned int      offset)   /* Offset of required bytes within word.  */
{
    ARC_Byte word[BYTES_IN_WORD];

    /* First read the word of memory that will be overwritten.  */
    if (ops->read_memory(addr, word, 1) > 0)
    {
        unsigned int i;

        /* Next replace the bytes in that word that are to be written.  */
        for (i = 0; i < bytes; i++)
            word[offset + i] = data[i];

        /* Now write it!  */
        if (ops->write_memory(addr, word, 1) > 0)
            /* Have written the specified number of bytes.  */
            return bytes;
    }

    /* Failed: no data written.  */
    return 0;
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Read a chunk of data from target memory.
   Returns number of bytes read.  */

unsigned int
arc_read_memory (TargetOperations *ops,
                 ARC_Address       address,
                 ARC_Byte         *data,
                 unsigned int      bytes)
{
    unsigned int gap        = address % BYTES_IN_WORD;
    unsigned int total_read = 0;

    ENTERARGS("address 0x%08X, bytes %u", address, bytes);

    /* Special fast case for reading a single word.  */
    if (gap == 0 && bytes == BYTES_IN_WORD)
    {
        DEBUG("read single word\n");

        /* N.B. assumes that 'data' is word-aligned, or that host does not care!  */
        total_read = ops->read_memory(address, data, 1);
    }
    /* Pathological case: bytes in middle of word.  */
    else if (gap > 0 && gap + bytes < BYTES_IN_WORD)
    {
        DEBUG("read pathological\n");

        total_read = read_partial_word(ops,
                                       address - gap,   /* Word-aligned address.  */
                                       data,
                                       bytes,
                                       gap);
    }
    else /* The general case.  */
    {
        Layout chunk = split(address, bytes);

        if (chunk.leading_bytes > 0)
        {
            /* Read the first few bytes.  */
            total_read = read_partial_word(ops,
                                           address - gap,   /* Word-aligned addres.  */
                                           data,
                                           chunk.leading_bytes,
                                           gap);
            data += chunk.leading_bytes;
            address += chunk.leading_bytes;
        }

        if (chunk.words > 0)
        {
            unsigned int bytes_read = ops->read_memory(address, data, chunk.words);

            total_read += bytes_read;
            address       += bytes_read;
            data       += bytes_read;
        }

        if (chunk.trailing_bytes > 0)
        {
            /* Read the last few bytes of data.  */
            total_read += read_partial_word(ops,
                                            address,     // Word-aligned address.  */
                                            data,
                                            chunk.trailing_bytes,
                                            0);
        }
    }

    DEBUG("read %u bytes\n", total_read);

    return total_read;
}


/* Write a chunk of data to target memory.
   Returns number of bytes written.  */

unsigned int
arc_write_memory (TargetOperations *ops,
                  ARC_Address       address,
                  ARC_Byte         *data,
                  unsigned int      bytes)
{
    unsigned int gap           = address % BYTES_IN_WORD;
    unsigned int total_written = 0;

    ENTERARGS("address 0x%08X, bytes %u", address, bytes);

    /* Useful debugging code: just change 0 to 1.  */
    if (0)
    {
        unsigned int i;
        for (i = 0; i < bytes; i++)
        {
            DEBUG("%02X", data[i]);
            if ((i + 1) % 16 == 0)
                DEBUG("\n");
        }
        DEBUG("\n");
    }

    /* Special fast case for writing a single word.  */
    if (gap == 0 && bytes == BYTES_IN_WORD)
    {
        DEBUG("write single word (%02X %02X %02X %02X)\n", data[0], data[1], data[2], data[3]);

        total_written = ops->write_memory(address, data, 1);
    }
    /* Pathological case: bytes in middle of word.  */
    else if (gap > 0 && gap + bytes < BYTES_IN_WORD)
    {
        DEBUG("write pathological\n");

        total_written = write_partial_word(ops,
                                           address - gap,   /* Word-aligned address.  */
                                           data,
                                           bytes,
                                           gap);
    }
    else /* general case */
    {
        Layout chunk = split(address, bytes);

        if (chunk.leading_bytes > 0)
        {
            /* Write the first few bytes.  */
            total_written = write_partial_word(ops,
                                               address - gap,   /* Word-aligned address.  */
                                               data,
                                               chunk.leading_bytes,
                                               gap);
            data += chunk.leading_bytes;
            address += chunk.leading_bytes;
        }

        if (chunk.words > 0)
        {
            unsigned int bytes_written = ops->write_memory(address, data, chunk.words);

            total_written += bytes_written;
            address          += bytes_written;
            data          += bytes_written;
        }

        if (chunk.trailing_bytes > 0)
        {
            /* Write the last few bytes of data.  */
            total_written += write_partial_word(ops,
                                                address,    /* Word-aligned address.  */
                                                data,
                                                chunk.trailing_bytes,
                                                0);
        }
    }

    DEBUG("written %u bytes\n", total_written);

    return total_written;
}


/* Write a repeated pattern of data to memory;
   the start of each pattern is always word-aligned, so if the given address is
   not word-aligned, the first partial word written will contain trailing bytes
   of the pattern.  */

unsigned int
arc_write_pattern (TargetOperations *ops,
                   ARC_Address       address,
                   ARC_Word          pattern,
                   unsigned int      bytes)
{
    unsigned int gap           = address % BYTES_IN_WORD;
    unsigned int total_written = 0;

    ENTERARGS("address 0x%08X, pattern 0x%08X, bytes %u", address, pattern, bytes);

    /* Special fast case for writing a single word.  */
    if (gap == 0 && bytes == BYTES_IN_WORD)
    {
        DEBUG("write single word (%08X)\n", pattern);

        total_written = ops->write_memory(address, (ARC_Byte*) &pattern, 1);
    }
    /* Pathological case: bytes in middle of word.  */
    else if (gap > 0 && gap + bytes < BYTES_IN_WORD)
    {
        DEBUG("write pathological\n");

        total_written = write_partial_word(ops,
                                           address - gap,   /* Word-aligned address.  */
                                           ((ARC_Byte*) &pattern) + gap ,
                                           bytes,
                                           gap);
    }
    else /* General case.  */
    {
        Layout chunk = split(address, bytes);

        if (chunk.leading_bytes > 0)
        {
            /* Write the first few bytes.  */
            total_written = write_partial_word(ops,
                                               address - gap,   /* Word-aligned address.   */
                                               ((ARC_Byte*) &pattern) + gap ,
                                               chunk.leading_bytes,
                                               gap);

            address += chunk.leading_bytes;
        }

        /* If we have been given a fill_memory operation.  */
        if (ops->fill_memory)
        {
            /* Write the complete words of data in one go.  */
            unsigned int bytes_written = ops->fill_memory(address, pattern, chunk.words);

            total_written += bytes_written;
            address       += bytes_written;
        }
        else
        {
            /* Write all the complete words of data, one word at a time.  */
            while (chunk.words--)
            {
                unsigned int bytes_written = ops->write_memory(address, (ARC_Byte*) &pattern, 1);

                total_written += bytes_written;
                address       += bytes_written;
            }
        }

        if (chunk.trailing_bytes > 0)
        {
            /* Write the last few bytes of data.  */
            total_written += write_partial_word(ops,
                                                address,    /* Word-aligned address.  */
                                                ((ARC_Byte*) &pattern),
                                                chunk.trailing_bytes,
                                                0);
        }
    }

    DEBUG("written %u bytes\n", total_written);

    return total_written;
}

/******************************************************************************/
