/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

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
/*     This header file defines some useful types and constants, and macros   */
/*     for use in debugging.                                                  */
/*                                                                            */
/******************************************************************************/

#ifndef ARC_SUPPORT_H
#define ARC_SUPPORT_H


#define ARC_DEBUG 1

#ifdef ARC_DEBUG
#define DEBUG(...)              if (arc_debug_target) fprintf_unfiltered(gdb_stdlog, __VA_ARGS__)
#define ENTERMSG                DEBUG("--- entered %s:%s()\n",        __FILE__, __FUNCTION__)
#define ENTERARGS(fmt, args...) DEBUG("--- entered %s:%s(" fmt ")\n", __FILE__, __FUNCTION__, args)
#define LEAVEMSG                DEBUG("--- exited  %s:%s()\n",        __FILE__, __FUNCTION__)
#else
#define DEBUG(...)
#define ENTERMSG
#define ENTERARGS(fmt, args...)
#define LEAVEMSG
#endif


/* N.B. it must be possible to build some ARC modules without the rest of gdb so
        that they can be exercised by stand-alone test drivers.  */
#ifdef STANDALONE_TEST
#define error(...)                    { printf(__VA_ARGS__); printf("\n"); }
#define warning(...)                  { printf(__VA_ARGS__); printf("\n"); }
#define printf_filtered(...)            printf(__VA_ARGS__)
#define printf_unfiltered(...)          printf(__VA_ARGS__)
#define fprintf_unfiltered(STR, ...)    printf(__VA_ARGS__)
#define internal_error(...)             abort()
#endif


/* Useful Boolean type and constants.  */
typedef int Boolean;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


/* Useful types for machine-related quantities.

   N.B. the type ARC_RegisterNumber does not denote gdb register numbers;
        instead, it denotes ARC processor hardware numbers, which are not
        the same.  */

typedef unsigned int           ARC_RegisterNumber;
typedef unsigned int           ARC_RegisterContents;
typedef unsigned int           ARC_Address;
typedef unsigned long long int ARC_Doubleword;
typedef unsigned int           ARC_Word;
typedef unsigned short int     ARC_Halfword;
typedef unsigned char          ARC_Byte;


/* Types for machine-access functions.  */
typedef Boolean (*ReadRegisterFunction)(ARC_RegisterNumber    hw_regno,
                                        ARC_RegisterContents *contents,
                                        Boolean               warn_on_failure);

typedef Boolean (*WriteRegisterFunction)(ARC_RegisterNumber   hw_regno,
                                         ARC_RegisterContents contents,
                                         Boolean              warn_on_failure);


typedef unsigned int (*MemoryTransferFunction)(ARC_Address  address,
                                               ARC_Byte    *data,    /* May be not word-aligned.  */
                                               unsigned int amount);

typedef unsigned int (*MemoryFillFunction)    (ARC_Address  address,
                                               ARC_Word     pattern,
                                               unsigned int amount);

typedef struct
{
    ReadRegisterFunction   read_core_register;
    WriteRegisterFunction  write_core_register;
    ReadRegisterFunction   read_auxiliary_register;
    WriteRegisterFunction  write_auxiliary_register;
    MemoryTransferFunction read_memory;
    MemoryTransferFunction write_memory;
    MemoryFillFunction     fill_memory;
} TargetOperations;


/* Sizes of machine quantities.  */
#define BYTES_IN_WORD              4
#define BYTES_IN_REGISTER          4
#define BITS_IN_BYTE               8
#define BITS_IN_WORD              32
#define BITS_IN_ADDRESS           32
#define BITS_IN_REGISTER          32


/* Useful macros.  */
#define ELEMENTS_IN_ARRAY(arr)           (unsigned int) (sizeof(arr) / sizeof(arr[0]))

#define IS_WORD_ALIGNED(addr)  ((addr) % BYTES_IN_WORD == 0)


#endif /* ARC_SUPPORT_H */
/******************************************************************************/
