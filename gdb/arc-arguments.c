/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
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
/*     This module implements operations for setting up the command line      */
/*     arguments to the program which is being debugged.                      */
/*                                                                            */
/*     E.g. if we are passing 4 arguments to the program's 'main' function,   */
/*          we must place them on the stack in the layout:                    */
/*                                                                            */
/*        .                                                                   */
/*        .                                                                   */
/*     stack[top + A3] <== <arg_3>                                            */
/*        .                                                                   */
/*        .                                                                   */
/*     stack[top + A2] <== <arg_2>                                            */
/*        .                                                                   */
/*        .                                                                   */
/*     stack[top + A1] <== <arg_1>                                            */
/*        .                                                                   */
/*        .                                                                   */
/*     stack[top + A0] <== <arg_0>                                            */
/*     stack[top + 24] <== 0x0           # ? NULL terminator                  */
/*     stack[top + 20] <== 0x0           # envp NULL terminator               */
/*     stack[top + 16] <== 0x0           # argv NULL terminator               */
/*     stack[top + 12] <== TOP + A3      # argv[3]                            */
/*     stack[top +  8] <== TOP + A2      # argv[2]                            */
/*     stack[top +  4] <== TOP + A1      # argv[1]                            */
/*     stack[top +  0] <== TOP + A0      # argv[0]                            */
/*                                                                            */
/*     where TOP = &stack[top]                                                */
/*       and A0 .. A3 are the offsets of the stored arguments from the stack  */
/*                    top.                                                    */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <byteswap.h>

/* gdb header files */
#include "defs.h"
#include "target.h"
#include "symtab.h"
#include "regcache.h"
#include "objfiles.h"

/* ARC header files */
#include "arc-arguments.h"
#include "arc-support.h"
#include "arc-tdep.h"


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define MINIMUM_INSTRUCTION_SIZE       2
#define MOV_SP_INSTRUCTION_LE          0x3F80240A
#define MOV_SP_INSTRUCTION_BE          0x803F0A24


/* The address of the top of the target program's stack before the program
   arguments were pushed onto the stack.  */
static CORE_ADDR old_stack_top;

/* The address in the target program code of the instructions which set up the
   stack pointer (SP) at program start-up.  */
static CORE_ADDR stack_pointer_setup_code_operand_address;


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

#define TARGET_IS_BIG_ENDIAN  (gdbarch_byte_order (current_gdbarch) == BFD_ENDIAN_BIG)

#define TARGET_ENDIAN(word)   ((TARGET_IS_BIG_ENDIAN) ? __bswap_32(word) : (word))

#define MK_OPERAND_LE(x)      (ARC_Word) ((((x) & 0xffff0000) >> 16) | \
                                          (((x) & 0x0000ffff) << 16))

#define MK_OPERAND_BE(x)      (ARC_Word) (__bswap_32(x))


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* Find the address in the target program code of the given label.
   Return 0 if the label is not found.  */

static CORE_ADDR
find_label (const char *label)
{
    struct minimal_symbol *msymbol = lookup_minimal_symbol(label, NULL, symfile_objfile);
    CORE_ADDR              address = 0;

    if (msymbol != NULL)
        address = SYMBOL_VALUE_ADDRESS (msymbol);

    DEBUG("%s = %x\n", label, (unsigned int) address);

    return address;
}


/* Write a word of data to target memory.

   Parameters:
       address : the address in memory to write the data
       word    : the data to be written

   Returns TRUE if the data is written. FALSE otherwise.

   If the data is written, the given address is incremented to reference the
   next word in target memory.  */

static Boolean
write_word (CORE_ADDR *address, ARC_Word word)
{
    word = TARGET_ENDIAN(word);

    if (target_write_memory(*address, (gdb_byte*) &word, BYTES_IN_WORD) == 0)
    {
        *address += BYTES_IN_WORD;
        return TRUE;
    }

    return FALSE;
}


/* Try to find the address in the target program code of the instructions which
   set up the program stack pointer (SP).

   Parameter:
       stack_top: the address which is loaded into SP by the setup code

   Return TRUE if the code is found.

   If the setup code is found, the variable stack_pointer_setup_code_operand_address
   is set to the code's address.  */

static Boolean
find_stack_top_setup_code (CORE_ADDR stack_top)
{
    /* Try to find the start address in the target program.  */
    CORE_ADDR code_start = find_label("__start");

    if (code_start != 0)
    {
        CORE_ADDR code = (ARC_Address) code_start;
        ARC_Word  set_sp_insn[2];
        ARC_Byte  buffer[16 * BYTES_IN_WORD];

        DEBUG("setting up arguments: stack_top = %x, code_start = %x\n",
              (unsigned int) stack_top, (unsigned int) code_start);

        if (TARGET_IS_BIG_ENDIAN)
        {
            set_sp_insn[0] = MOV_SP_INSTRUCTION_BE;
            set_sp_insn[1] = MK_OPERAND_BE(stack_top);
        }
        else
        {
            set_sp_insn[0] = MOV_SP_INSTRUCTION_LE;
            set_sp_insn[1] = MK_OPERAND_LE(stack_top);
        }

        /* Scan through the start code of the program, looking for the code that
           sets up the program's stack pointer; we recognize this as a 32-bit
           'mov sp' instruction followed by a 32-bit operand which is the
           address of the stack top (which we obtained from the executable file).  */
        while (TRUE)
        {
            int result = target_read_memory(code, (gdb_byte*) buffer, (int) sizeof(buffer));

            if (result == 0)
            {
                size_t offset = 0;

                while (offset <= sizeof(buffer) - sizeof(set_sp_insn))
                {
                    if (memcmp(buffer + offset, set_sp_insn, sizeof(set_sp_insn)) == 0)
                    {
                        stack_pointer_setup_code_operand_address = code + (CORE_ADDR) offset + BYTES_IN_WORD;

                        DEBUG("found 'mov sp, <stacktop>' instruction operand at address 0x%x\n",
                              (unsigned int) stack_pointer_setup_code_operand_address);
                        return TRUE;
                    }

                    offset += MINIMUM_INSTRUCTION_SIZE;
                }
            }
            else
            {
                warning(_("can not find read target program start code"));
                break;
            }

            code += (CORE_ADDR) (sizeof(buffer) - sizeof(set_sp_insn));

            /* If we haven't found it in the first 100 bytes.  */
            if (code - code_start > 100)
            {
                warning(_("can not find 'mov sp, <stacktop>' instruction in start code"));
                break;
            }
        }
    }

    return FALSE;
}


/* Try to change the setup code in the program so that SP is loaded with a
   given address.

   Parameters:
       old_stack_top: the address which is currently loaded into SP by the code
       new_stack_top: the address which we wish to be loaded into SP by the code

   Return TRUE if the setup code is changed.  */

static Boolean
set_stack_top (CORE_ADDR old_stack_top, CORE_ADDR new_stack_top)
{
    ARC_Word operand = (TARGET_IS_BIG_ENDIAN) ? MK_OPERAND_BE(new_stack_top)
                                              : MK_OPERAND_LE(new_stack_top);

    /* If we do not yet know the address in the program code at which the
       program's stack pointer is set up.  */
    if (stack_pointer_setup_code_operand_address == 0)
    {
        /* Try to find it.  */
        if (!find_stack_top_setup_code(old_stack_top))
            return FALSE;
    }

    DEBUG("set stack top @ 0x%08X to 0x%08X (0x%08X)\n",
          (unsigned int) stack_pointer_setup_code_operand_address,
          (unsigned int) new_stack_top,
          operand);

    return (target_write_memory(stack_pointer_setup_code_operand_address,
                                (gdb_byte*) &operand,
                                BYTES_IN_WORD) == 0);
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* This is called when a program is downloaded to the debug target. */

void
arc_program_loaded (void)
{
    /* The program has just been loaded, so we do not yet know the address in
       the program code at which the program's stack pointer is set up.  */
    stack_pointer_setup_code_operand_address = 0;
}


/* Store the program's arguments on the stack.
   Return TRUE if they are stored successfully.  */

Boolean
arc_setup_arguments (char *args)
{
    Boolean done = FALSE;

    /* Try to find the top of stack in the target program.  */
    old_stack_top = find_label("__stack_top");

    if (old_stack_top != 0)
    {
        char       **argv = buildargv (args);
        char       **argp;
        size_t       string_length = 0;
        unsigned int argc          = 0;
        unsigned int num_pointers;
        unsigned int total_size;
        CORE_ADDR    new_stack_top;

        if (argv == NULL)
            nomem (0);

        /* Calculate the space required to hold the args.  */

        for (argp = argv; *argp != NULL; argp++)
        {
            string_length += strlen (*argp) + 1;
            argc++;
        }

        DEBUG("%d arguments\n", argc);

        num_pointers = argc + 3;

        total_size = (unsigned int) string_length + num_pointers * BYTES_IN_WORD;

        /* Round up to a multiple of 32: strlen expects memory to come in chunks
         * that are at least cache-line (32 bytes) sized.  */
        total_size +=  31;
        total_size &= -32;

        DEBUG("total size: %d\n", total_size);

        new_stack_top = old_stack_top - total_size;

        DEBUG("new stack top: 0x%08x\n", (unsigned int) new_stack_top);

        /* Adjust the setting of the top of the stack in the object code.  */
        if (set_stack_top(old_stack_top, new_stack_top))
        {
            struct regcache *regcache   = get_current_regcache();
            CORE_ADDR        data_space = new_stack_top + num_pointers * BYTES_IN_WORD;
            CORE_ADDR        stack_top  = new_stack_top;
            unsigned int i;

            DEBUG("data space: 0x%08x\n", (unsigned int) data_space);

            done = TRUE;

            /* Write the args onto the top of the stack.  */

            for (i = 0; i < argc; i++)
            {
                char  *parameter = argv[i];
                size_t length    = strlen(parameter) + 1;
                int    result    = target_write_memory(data_space, (gdb_byte*) parameter, (int) length);

                if (result == 0)
                {
                    DEBUG("written argv[%d] to 0x%08x: \"%s\"\n",
                          i, (unsigned int) data_space, parameter);
                }
                else
                    done = FALSE;

                /* Write a pointer to the argument onto the stack.  */
                if (!write_word(&stack_top, (ARC_Word) data_space))
                    done = FALSE;

                data_space += length;
            }

            /* Try to write the NULLs.  */
            if (!write_word(&stack_top, 0) ||
                !write_word(&stack_top, 0) ||
                !write_word(&stack_top, 0))
                done = FALSE;

            /* Set up the R0 and R1 parameter registers.  */

            /* Convert to target byte order if necessary.  */
            if (HOST_AND_TARGET_ENDIANNESS_DIFFER(current_gdbarch))
            {
                 argc          = __bswap_32(argc);
                 new_stack_top = __bswap_32(new_stack_top);
            }

            regcache_raw_supply   (regcache, 0, &argc);
            regcache_raw_supply   (regcache, 1, &new_stack_top);
            target_store_registers(regcache, 0);
            target_store_registers(regcache, 1);
        }

        freeargv(argv);
    }

    return done;
}


/* This function is called just before disconnection from the debug target.  */

void
arc_restore_stack_top_address (void)
{
    /* If we know the address in the program start-up code at which the stack
       pointer is set up, it must be because we changed the stack top address
       in the code - so change it back to the original address as read from the
       excutable file.

       This is done so that if the user disconnects from the target, then
       reconnects to it in a subsequent debugging session but does NOT download
       the program to the target again (as it is still in target memory), the
       mechanism for altering the stack top will still work.

       Note that this has no effect if the target is allowed to resume execution
       (i.e. a 'detach' is being performed) as we are changing code that has
       already been executed.

       0 is passed as the "old" stack top as it is not used in this situation.  */

    if (stack_pointer_setup_code_operand_address != 0)
        (void) set_stack_top(0, old_stack_top);
}

/******************************************************************************/
