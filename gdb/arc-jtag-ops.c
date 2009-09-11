/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Author:
      Sameer Dhavale  <sameer.dhavale@codito.com>
      Soam Vasani     <soam.vasani@codito.com>
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
/*     This module implements debug access to an ARC processor via its JTAG   */
/*     interface.                                                             */
/*                                                                            */
/*     See                                                                    */
/*           ARCompact Instruction Set Architecture                           */
/*              Programmer's Reference    (5115-018)                          */
/*                                                                            */
/*           for a description of ARC processor architecture (in particular   */
/*           the auxiliary registers and the halting procedure);              */
/*                                                                            */
/*           ARC 700 External Interfaces                                      */
/*              Reference                 (5117-013)                          */
/*                                                                            */
/*           for a description of the JTAG interface (in particular the Test  */
/*           Access Port Controller (TAPC) state machine).                    */
/*                                                                            */
/*     The JTAG interface is accessed by three parallel ports: control, data  */
/*     and status.  Data is read from or written to these ports one byte at a */
/*     at a time, using a GPIO (General Purpose Input/Output) driver.         */
/*                                                                            */
/*     The TDI and TMS signals are written to the data port.                  */
/*     The TCK signal is written to the control port.                         */
/*     The TDO signal is read from the status port.                           */
/*                                                                            */
/* Host/Target Byte Order:                                                    */
/*     The core and auxiliary register contents read from or written to the   */
/*     JTAG interface are ALWAYS in little-endian format, regardless of the   */
/*     endianness of the target processor.                                    */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <byteswap.h>

/* gdb header files */
#include "defs.h"
#include "gdb_assert.h"

/* ARC header files */
#include "arc-gpio.h"
#include "arc-jtag.h"
#include "arc-jtag-ops.h"
#include "arc-support.h"


/* -------------------------------------------------------------------------- */
/*                        conditional compilation flags                       */
/* -------------------------------------------------------------------------- */

/* We would really like to optimise the use of the JTAG Data Register by
   loading a word into it only if that word differs from the last word loaded;
   but there seems to be undocumented behaviour of the JTAG mechanism in that
   the Data Register is altered after a write operation! See ARC Bug #93814.  */
#define JTAG_DATA_REGISTER_IS_CORRUPTED_BY_WRITE

/* Define this if you wish to check the whether the contents of the JTAG Data
   Register are corrupted by write operations. */
//#define CHECK_JTAG_DATA_REGISTER

/* Define this if you wish to perform low-level debugging of the JTAG state
   machine emulated in this module (this should not be necessary).  */
//#define STATE_MACHINE_DEBUG


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

/* Sizes of quantities.  */
#define BITS_IN_COMMAND_CODE     4
#define BITS_IN_REGISTER_CODE    4


typedef unsigned int Bit;    /* Only LSB of word is used.  */
typedef unsigned int JTAG_RegisterContents;


typedef enum
{
    MSB_FIRST,
    MSB_LAST
} Order;


typedef enum
{
    Memory,
    Register
} JTAG_TransactionType;


typedef enum
{
    STALLED,
    FAILURE,
    READY,
    NOT_READY
} JTAG_TransactionStatus;


/* Only these JTAG registers are currently used.  */
typedef enum
{
    JTAG_STATUS_REGISTER              = 0x8,
    JTAG_TRANSACTION_COMMAND_REGISTER = 0x9,
    JTAG_ADDRESS_REGISTER             = 0xA,
    JTAG_DATA_REGISTER                = 0xB,
} JTAG_Register;


#ifdef STATE_MACHINE_DEBUG
typedef enum
{
    UNDEFINED,
    TEST_LOGIC_RESET,
    RUN_TEST_IDLE,
    SELECT_DR_SCAN,
    CAPTURE_DR,
    SHIFT_DR,
    EXIT1_DR,
    PAUSE_DR,
    EXIT2_DR,
    UPDATE_DR,
    SELECT_IR_SCAN,
    CAPTURE_IR,
    SHIFT_IR,
    EXIT1_IR,
    PAUSE_IR,
    EXIT2_IR,
    UPDATE_IR,
    NUMBER_OF_STATES    /* An end-marker, not a state.  */
} JTAG_ControllerState;
#endif


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

/* -------------------------------------- */
/*         For ARC JTAG Cable             */
/*                                        */
/* Pin no.  Signal         Port       Bit */
/*                                        */
/* - 	    TRST 	                  */
/* 8 	    TMS 	   Data       6   */
/* 1 	    TCK 	   Control    0   */
/* 9 	    TDI 	   Data       7   */
/* 13 	    TDO 	   Status     4   */
/* -------------------------------------- */

/* Bit masks for signals written to parallel ports.  */

#define JTAG_TRST 0          /* not there                    */
#define JTAG_TMS  (1 << 6)   /* on Data port                 */
#define JTAG_TCK  (1 << 0)   /* on Control port (driven low) */
#define JTAG_TDI  (1 << 7)   /* on Data port                 */
#define JTAG_TDO  (1 << 4)   /* on Status port               */


/* Commands which can be written to the JTAG Transaction Command Register.  */
#define WRITE_MEMORY_LOCATION   0x0
#define WRITE_CORE_REGISTER     0x1
#define WRITE_AUX_REGISTER      0x2
#define NOP                     0x3
#define READ_MEMORY_LOCATION    0x4
#define READ_CORE_REGISTER      0x5
#define READ_AUX_REGISTER       0x6


/* Gives the endianness of the target processor.  */
static Boolean target_is_big_endian;

/* These accumulate the bit masks to be written to the data and control ports.  */
static Byte data_port_value, control_port_value;

/* Variables for tracking the contents of the JTAG Address and Transaction
   Command registers.  */
static JTAG_RegisterContents address_register_contents;
static JTAG_RegisterContents command_register_contents;
static Boolean               address_register_contents_known;
static Boolean               command_register_contents_known;


#ifdef STATE_MACHINE_DEBUG
/* This table encodes all possible transitions of the JTAG Test Access Port
   (TAP) Controller State Machine: for each state, the transition to one of two
   possible next states is determined by whether a 0 bit or a 1 bit is written
   as the JTAG TMS interface signal.  */
static const JTAG_ControllerState transitions[NUMBER_OF_STATES][2] =
{
/*                              0              1           */
/*  UNDEFINED         */  { UNDEFINED,     UNDEFINED        },
/*  TEST_LOGIC_RESET  */  { RUN_TEST_IDLE, TEST_LOGIC_RESET },
/*  RUN_TEST_IDLE     */  { RUN_TEST_IDLE, SELECT_DR_SCAN   },
/*  SELECT_DR_SCAN    */  { CAPTURE_DR,    SELECT_IR_SCAN   },
/*  CAPTURE_DR        */  { SHIFT_DR,      EXIT1_DR         },
/*  SHIFT_DR          */  { SHIFT_DR,      EXIT1_DR         },
/*  EXIT1_DR          */  { PAUSE_DR,      UPDATE_DR        },
/*  PAUSE_DR          */  { PAUSE_DR,      EXIT2_DR         },
/*  EXIT2_DR          */  { SHIFT_DR,      UPDATE_DR        },
/*  UPDATE_DR         */  { RUN_TEST_IDLE, SELECT_DR_SCAN   },
/*  SELECT_IR_SCAN    */  { CAPTURE_IR,    TEST_LOGIC_RESET },
/*  CAPTURE_IR        */  { SHIFT_IR,      EXIT1_IR         },
/*  SHIFT_IR          */  { SHIFT_IR,      EXIT1_IR         },
/*  EXIT1_IR          */  { PAUSE_IR,      UPDATE_IR        },
/*  PAUSE_IR          */  { PAUSE_IR,      EXIT2_IR         },
/*  EXIT2_IR          */  { SHIFT_IR,      UPDATE_IR        },
/*  UPDATE_IR         */  { RUN_TEST_IDLE, SELECT_DR_SCAN   },
};

/* The current state of the TAP Controller State Machine.  */
static JTAG_ControllerState current_state = UNDEFINED;
#endif


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* This structure holds the operations and data exported by this module.  */
JTAG_Operations arc_jtag_ops;


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

#ifdef DEBUG
#undef DEBUG
#endif

#define DEBUG(...) \
    if (arc_jtag_ops.state_machine_debug) fprintf_unfiltered(gdb_stdlog, __VA_ARGS__)


#ifdef STATE_MACHINE_DEBUG
#define SET_STATE(s)                  set_state(s)
#define NEXT_STATE(b)                 next_state(b)
#define CHANGE_STATE(x, s)            change_state(x, s)
#define STATE_IS(s)                   gdb_assert(current_state == s)
#define STATE_IS_EITHER(s1, s2)       gdb_assert(current_state == s1 || \
                                                 current_state == s2)
#define STATE_IS_ONE_OF(s1, s2, s3)   gdb_assert(current_state == s1 || \
                                                 current_state == s2 || \
                                                 current_state == s3)
#else
#define SET_STATE(s)
#define NEXT_STATE(b)
#define CHANGE_STATE(x, s)            tapc_TMS(x)
#define STATE_IS(s)
#define STATE_IS_EITHER(s1, s2)
#define STATE_IS_ONE_OF(s1, s2, s3)
#endif


#define IS_WORD_ALIGNED(addr)         ((addr) % BYTES_IN_WORD == 0)
#define BYTE(val)                     (Byte) ((val) & 0xFF)

/* This is more efficient than memcpy(to, from, BYTES_IN_WORD).  */
#define COPY_WORD(to, from)           { ((Byte*) to)[0] = ((Byte*) from)[0]; \
                                        ((Byte*) to)[1] = ((Byte*) from)[1]; \
                                        ((Byte*) to)[2] = ((Byte*) from)[2]; \
                                        ((Byte*) to)[3] = ((Byte*) from)[3]; }


/* -------------------------------------------------------------------------- */
/*                               forward declarations                         */
/* -------------------------------------------------------------------------- */

static void tapc_TMS (Bit x);
static void set_interface (JTAG_Status status);
static void interface_is_closed (void);
static JTAG_RegisterContents read_jtag_reg (JTAG_Register regnum,
                                            unsigned int  num_data_bits);


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* 1)                            debug functions                              */
/* -------------------------------------------------------------------------- */

/* Debug function.  */

static const char*
JTAG_register_name (JTAG_Register r)
{
    switch (r)
    {
        case JTAG_STATUS_REGISTER              : return "Status";
        case JTAG_TRANSACTION_COMMAND_REGISTER : return "Transaction Command";
        case JTAG_ADDRESS_REGISTER             : return "Address";
        case JTAG_DATA_REGISTER                : return "Data";
        default: internal_error(__FILE__, __LINE__,  _("invalid JTAG register %d"), r);
    }
}


/* Debug function.  */

static const char*
JTAG_TransactionStatus_Image (JTAG_TransactionStatus value)
{
    switch (value)
    {
        case STALLED  : return "STALLED";
        case FAILURE  : return "FAILURE";
        case READY    : return "READY";
        case NOT_READY: return "NOT READY";
        default: internal_error(__FILE__, __LINE__,  _("invalid JTAG transaction status %d"), value);
    }
}


#ifdef STATE_MACHINE_DEBUG
/* Debug function.  */

static const char*
JTAG_ControllerState_Image (JTAG_ControllerState state)
{
    switch (state)
    {
        case UNDEFINED       : return "<undefined>     ";
        case TEST_LOGIC_RESET: return "Test-Logic-Reset";
        case RUN_TEST_IDLE   : return "Run-Test/Idle   ";
        case SELECT_DR_SCAN  : return "Select-DR-Scan  ";
        case CAPTURE_DR      : return "Capture-DR      ";
        case SHIFT_DR        : return "Shift-DR        ";
        case EXIT1_DR        : return "Exit1-DR        ";
        case PAUSE_DR        : return "Pause-DR        ";
        case EXIT2_DR        : return "Exit2-DR        ";
        case UPDATE_DR       : return "Update-DR       ";
        case SELECT_IR_SCAN  : return "Select-IR-Scan  ";
        case CAPTURE_IR      : return "Capture-IR      ";
        case SHIFT_IR        : return "Shift-IR        ";
        case EXIT1_IR        : return "Exit1-IR        ";
        case PAUSE_IR        : return "Pause-IR        ";
        case EXIT2_IR        : return "Exit2-IR        ";
        case UPDATE_IR       : return "Update-IR       ";
        default              : return "<invalid>       ";
    }
}
#endif


#ifdef CHECK_JTAG_DATA_REGISTER
/* Read back the contents of the JTAG Data Register, and check that the value
   is what is expected, i.e. the last value that was written to that register.  */

static void
check_Data_Register (ARC_Word expected)
{
    /* Read the data from the JTAG Data Register.  */
    ARC_Word actual = (ARC_Word) read_jtag_reg(JTAG_DATA_REGISTER, BITS_IN_WORD);

    /* Is the data still in the register?  */
    if (actual != expected)
        warning(_("JTAG Data Register: expected = %08X, actual = %08X\n"), expected, actual);
    else
        printf_unfiltered(_("word %08X is still in JTAG Data Register\n"), expected);
}
#endif


/* -------------------------------------------------------------------------- */
/* 2)            helper functions for setting TMS / TCK / TDI                 */
/* -------------------------------------------------------------------------- */

/* These functions set the accumulated values to be written out to the ports.
   The final values are written only by doing the pulse, e.g. if TDI and TMS
   are set/unset by subsequent calls, the last GPIO write operation performed
   by those calls before the pulse writes the accumulated bit mask value to
   the port (overwriting the values written by the preceding calls), and it
   is this bit mask that is significant when the JTAG is clocked.  */

static void
tapc_set_TMS (Bit x)
{
    Byte current_value = data_port_value;

    if (x)
        data_port_value |=  JTAG_TMS;
    else
        data_port_value &= ~JTAG_TMS;

    if (data_port_value != current_value)
        gpio_write(DATA_PORT, data_port_value);
}


static void
tapc_set_TDI (Bit x)
{
    Byte current_value = data_port_value;

    if (x)
        data_port_value |=  JTAG_TDI;
    else
        data_port_value &= ~JTAG_TDI;

    if (data_port_value != current_value)
        gpio_write(DATA_PORT, data_port_value);
}


static void
tapc_set_TCK (Bit x)
{
    /* The clock is active low.  */
    if (x)
        control_port_value &= ~JTAG_TCK;
    else
        control_port_value |=  JTAG_TCK;

    gpio_write(CONTROL_PORT, control_port_value);
}


/* -------------------------------------------------------------------------- */
/* 3)                   JTAG state machine handlers                           */
/* -------------------------------------------------------------------------- */

#ifdef STATE_MACHINE_DEBUG
/* Debug function. Perform a state change to the given state.  */

static void
set_state (JTAG_ControllerState new_state)
{
    DEBUG("TAPC state: %s ====> %s\n",
           JTAG_ControllerState_Image(current_state),
           JTAG_ControllerState_Image(new_state));

    current_state = new_state;

    gdb_assert(current_state != UNDEFINED);
}


/* Debug function. Perform a state change from the current state according to
   the transition specified by the given bit.  */

static void
next_state (Bit x)
{
    current_state = (transitions[current_state][x]);
}


/* Debug function. Perform a state change from the current state according to
   the transition specified by the given bit, and check that the new state is
   as expected.  */

static void
change_state (Bit x, JTAG_ControllerState new_state)
{
    tapc_TMS(x);

    gdb_assert(current_state == new_state);
}
#endif /* STATE_MACHINE_DEBUG */


/* Clock the JTAG on the ARC platform.  */

static void
tapc_pulse (void)
{
    /* TCK control bit is active low.  */
    gdb_assert((control_port_value & JTAG_TCK) != (Byte) 0);  /* Clock should be zero on entry.  */

    gpio_write(CONTROL_PORT, control_port_value & ~JTAG_TCK);
    gpio_write(CONTROL_PORT, control_port_value);
}


/* Reset the TAP Controller on the JTAG. */

static void
tapc_reset (void)
{
    ENTERMSG;

    /* The Test Clock signal is active low (i.e. the signal is active when the
       corresponding bit written to the control bit is 0; so initialize the bit
       in the control port value to 1 so that the signal is initially not active.  */
    control_port_value = (Byte) JTAG_TCK;
    data_port_value    = (Byte) 0;

    /* From any state, this many TCK pulses should get the controller into state
       Test-Logic-Reset.  */
    tapc_set_TMS(1);
    tapc_set_TCK(0);  /* We want the rising edge.  */
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();

    SET_STATE(TEST_LOGIC_RESET);

    DEBUG("TAPC has been reset\n");

    /* The reset has re-initialized all the JTAG registers.  */
    address_register_contents_known = FALSE;
    command_register_contents_known = FALSE;

    CHANGE_STATE(0, RUN_TEST_IDLE);

    LEAVEMSG;
}


/* Set the TMS to the value of the bit and clock the JTAG.
   This will cause the TAP Controller State Machine to move to another state.  */

static void
tapc_TMS (Bit x)
{
    tapc_set_TMS(x);
    tapc_pulse();
    NEXT_STATE(x);
}


/* Read a bit from the TDO of the JTAG.  */

static Bit
tapc_readTDO (void)
{
    Byte byte = gpio_read(STATUS_PORT);

    /* Read from the status port.  */
    return ((byte & JTAG_TDO) != (Byte) 0) ? (Bit) 1 : (Bit) 0;
}


/* -------------------------------------------------------------------------- */
/* 4)  interface functions that use the JTAG state machine handler functions  */
/* -------------------------------------------------------------------------- */

/* Shift one bit out on the JTAG TDO and one bit in on the JTAG TDI.  */

static Bit
tapc_shift_bit (Bit out)
{
    Bit in = tapc_readTDO();

    tapc_set_TDI(out);
    tapc_pulse();

//  DEBUG("%u (out) >>> %u (in)\n", out, in);

    return in;
}


/* Shift N bits from to_write into TDI, and out from TDO into to_read.

   If order == MSB_LAST, shift LSB first, starting from to_write[0], to_write[1],
   etc.

   If order == MSB_FIRST, shift to_write[N-1] MSB first, then to_write[N-2] etc.

   Must be called in Shift DR/IR state.
   Leaves in Exit1-DR/IR state.  */

static void
tapc_shift_N_bits (unsigned int n,
                   Byte        *to_write,
                   Byte        *to_read,
                   Order        order)
{
    unsigned int nbytes = (n - 1) / BITS_IN_BYTE + 1;
    unsigned int nbits  = BITS_IN_BYTE;
    unsigned int i, j;

    ENTERARGS("shift %u bits", n);

    STATE_IS_EITHER(SHIFT_DR, SHIFT_IR);

    for (i = 0; i < nbytes; i++)
    {
        Boolean is_last_byte = (i == nbytes - 1);
        Byte    inbyte       = (Byte) 0;
        Byte    outbyte;

        if (order == MSB_FIRST)
            outbyte = to_write[nbytes - 1 - i];
        else
            outbyte = to_write[i];

        if (is_last_byte)
        {
            /* How many significant bits are in this byte?  */
            nbits = ((n - 1) % BITS_IN_BYTE) + 1;
        }

        /* gdb_assert (nbits <= BITS_IN_BYTE); */

        for (j = 0; j < nbits; j++)
        {
            Bit outbit, inbit;

//          DEBUG("byte %u, bit %u\n", i, j);

            /* Get the next bit to be output from the current byte.  */
            if (order == MSB_FIRST)
            {
                /* Get MSB from byte.  */
                outbit = (Bit) ((outbyte >> BITS_IN_BYTE) & 1);
                outbyte <<= 1;
            }
            else
            {
                /* Get LSB from byte.  */
                outbit = (Bit) (outbyte & 1);
                outbyte >>= 1;
            }

            /* The last bit of the last byte.  */
            if (is_last_byte && (j == nbits - 1))
            {
                /* Change to Exit1-DR/IR state before the last bit is shifted:
                   this is necessary because the TAP Controller performs the
                   last sample of TDI when exiting the Shift-DR/IR state.  */
                tapc_set_TMS(1);
                NEXT_STATE(1);
            }

            /* Shift one bit in from the JTAG TDO and one bit out to the JTAG TDI.  */
            inbit = tapc_shift_bit(outbit);

            /* Add the bit read into the input byte.
               N.B. the shift amount will always be positive, as 0 <= j < BITS_IN_BYTE */
            if (order == MSB_FIRST)
                inbyte |= (Byte) (inbit << (BITS_IN_BYTE - 1 - j));
            else
                inbyte |= (Byte) (inbit << j);
        }

        if (order == MSB_FIRST)
            to_read[nbytes - 1 - i] = inbyte;
        else
            to_read[i] = inbyte;
    }

    STATE_IS_EITHER(EXIT1_DR, EXIT1_IR);

    LEAVEMSG;
}


/* Read the JTAG Status Register.
   This indicates the status of the JTAG transaction that has been attempted.  */

static JTAG_TransactionStatus
read_jtag_status_register( void)
{
    JTAG_RegisterContents rd, wr;
    Bit                   bit;

    ENTERMSG;

    STATE_IS_EITHER(RUN_TEST_IDLE, UPDATE_DR);

    CHANGE_STATE(1, SELECT_DR_SCAN);
    CHANGE_STATE(1, SELECT_IR_SCAN);
    CHANGE_STATE(0, CAPTURE_IR);
    CHANGE_STATE(0, SHIFT_IR);

    wr = JTAG_STATUS_REGISTER;

    tapc_shift_N_bits(BITS_IN_REGISTER_CODE, (Byte*) &wr, (Byte*) &rd, MSB_LAST);

    CHANGE_STATE(1, UPDATE_IR);
//  CHANGE_STATE(0, RUN_TEST_IDLE);
    CHANGE_STATE(1, SELECT_DR_SCAN);
    CHANGE_STATE(0, CAPTURE_DR);
    CHANGE_STATE(0, SHIFT_DR);

    /* The JTAG Status Register is read-only, and any bits shifted in are
       ignored - hence the parameter to tapc_shift_bit is irrelevant here.  */

    /* Read Stalled bit; if it is zero then keep reading.  */
    bit = tapc_shift_bit(0);
    if (bit)
        return STALLED;

    /* Read Failed bit; if it is zero then keep reading.  */
    bit = tapc_shift_bit(0);
    if (bit)
        return FAILURE;

    /* Read Ready bit.  */
    bit = tapc_shift_bit(0);
    if (bit)
        return READY;

    /* The last bit (PC_SEL) is optional.  */

    return NOT_READY;
}


/* Write a value to a JTAG register.
      enter in Update-DR/IR state or Run-Test/Idle.
      exit  in Update-DR */

static void
write_jtag_reg (JTAG_Register         regnum,
                JTAG_RegisterContents data,
                unsigned int          num_data_bits)
{
    Byte                  num = (Byte) regnum;
    JTAG_RegisterContents rd  = 0;

    ENTERARGS("regnum %d <== 0x%08X (:%d)", regnum, data, num_data_bits);

    STATE_IS_ONE_OF(UPDATE_DR, UPDATE_IR, RUN_TEST_IDLE);

//  CHANGE_STATE(0, RUN_TEST_IDLE);
    CHANGE_STATE(1, SELECT_DR_SCAN);
    CHANGE_STATE(1, SELECT_IR_SCAN);
    CHANGE_STATE(0, CAPTURE_IR);
    CHANGE_STATE(0, SHIFT_IR);

    tapc_shift_N_bits(BITS_IN_REGISTER_CODE, &num, (Byte*) &rd, MSB_LAST);

    CHANGE_STATE(1, UPDATE_IR);
    CHANGE_STATE(1, SELECT_DR_SCAN);
    CHANGE_STATE(0, CAPTURE_DR);
    CHANGE_STATE(0, SHIFT_DR);

    tapc_shift_N_bits(num_data_bits, (Byte*) &data, (Byte*) &rd, MSB_LAST);

    CHANGE_STATE(1, UPDATE_DR);

    DEBUG("written 0x%08X to JTAG %s register\n", data, JTAG_register_name(regnum));

    LEAVEMSG;
}


/* Read a value from a JTAG register.
      enter in Update-DR/IR state
      exit  in Update-DR */

static JTAG_RegisterContents
read_jtag_reg (JTAG_Register regnum, unsigned int num_data_bits)
{
    Byte                  num = (Byte) regnum;
    JTAG_RegisterContents wr  = 0, rd = 0;

    ENTERARGS("regnum %u, %u bits", regnum, num_data_bits);

    STATE_IS_EITHER(UPDATE_DR, UPDATE_IR);

//  CHANGE_STATE(0, RUN_TEST_IDLE);
    CHANGE_STATE(1, SELECT_DR_SCAN);
    CHANGE_STATE(1, SELECT_IR_SCAN);
    CHANGE_STATE(0, CAPTURE_IR);
    CHANGE_STATE(0, SHIFT_IR);

    tapc_shift_N_bits(BITS_IN_REGISTER_CODE, &num, (Byte*) &rd, MSB_LAST);

    CHANGE_STATE(1, UPDATE_IR);

    /* JTAG registers can be read without going to Run-Test/Idle state.

       Doing CHANGE_STATE(0) would take us to Run-Test/Idle state. This would
       make JTAG perform the transaction in the Transaction Command Register.
       We don't want that!  */
//  CHANGE_STATE(0, RUN_TEST_IDLE);
    CHANGE_STATE(1, SELECT_DR_SCAN);
    CHANGE_STATE(0, CAPTURE_DR);
    CHANGE_STATE(0, SHIFT_DR);

    tapc_shift_N_bits(num_data_bits, (Byte*) &wr, (Byte*) &rd, MSB_LAST);

    CHANGE_STATE(1, UPDATE_DR);

    DEBUG("read 0x%08X from JTAG %s register\n", rd, JTAG_register_name(regnum));
    return rd;
}


/* -------------------------------------------------------------------------- */
/* 5)                        JTAG transaction functions                       */
/* -------------------------------------------------------------------------- */

/* Start a JTAG transaction.

   Parameters:
      command: the JTAG command to be performed
      address: the address (memory address or register number) for the command
*/

static void
start_jtag_transaction (JTAG_RegisterContents command,
                        JTAG_RegisterContents address)
{
    ENTERARGS("command = %d, address = 0x%x", command, address);

    STATE_IS_ONE_OF(UPDATE_DR, UPDATE_IR, RUN_TEST_IDLE);

    /* N.B. do NOT reset the TAP Controller at the start of each transaction, as
            that would re-initialize the JTAG registers to their default values
            (whatever those might be), so invalidating the optimisation of the
            use of the Address and Transaction Command registers performed by
            this module.  */

    if (command == READ_MEMORY_LOCATION || command == WRITE_MEMORY_LOCATION)
    {
        gdb_assert(IS_WORD_ALIGNED(address));
    }

    /* Load the command that is required into the JTAG Transaction Command
       Register, and the address into the JTAG Address Register; by keeping
       track of the values that these registers contain, we can avoid re-loading
       them unnecessarily, which can save time when transferring a stream of data.  */

    if (!command_register_contents_known ||
        (command_register_contents != command))
    {
        write_jtag_reg(JTAG_TRANSACTION_COMMAND_REGISTER, command, BITS_IN_COMMAND_CODE);

        command_register_contents_known = TRUE;
        command_register_contents       = command;
    }

    if (!address_register_contents_known ||
        (address_register_contents != address))
    {
        write_jtag_reg(JTAG_ADDRESS_REGISTER, address, BITS_IN_WORD);

        address_register_contents_known = TRUE;
        address_register_contents       = address;
    }

    LEAVEMSG;
}


/* Perform the given JTAG transaction (a mmeory or register operation).
   If the transaction fails, this function returns the given error.  */

static JTAG_OperationStatus
perform_jtag_transaction (JTAG_TransactionType transaction,
                          JTAG_OperationStatus error)
{
    JTAG_OperationStatus result = error;
    unsigned int         tries  = 0;

    ENTERARGS("transaction: %u", transaction);

    /* This causes the TAP Controller to perform the transaction, according to
       the contents of the JTAG Transaction Command, Address and Data registers.  */
    CHANGE_STATE(0, RUN_TEST_IDLE);

    /* Poll the JTAG Status Register.  */
    do
    {
        JTAG_TransactionStatus status = read_jtag_status_register();

        DEBUG("status: %s\n", JTAG_TransactionStatus_Image(status));

        /* The read has left the TAP Controller FSM in state Shift-DR.  */
        STATE_IS(SHIFT_DR);

        CHANGE_STATE(1, EXIT1_DR);
        CHANGE_STATE(1, UPDATE_DR);

        /* If the transaction is complete.  */
        if (status == READY)
        {
            /* The value in the JTAG Address Register is incremented by four
               (a memory access) or one (a register access) when a read/write
               transaction has completed.  */
            address_register_contents += (transaction == Memory) ? BYTES_IN_WORD : 1;

            result = JTAG_SUCCESS;
            break;
        }

        if (status == FAILURE)
            break;

        /* Pause and re-try.  */
        usleep(1);
    }
    while (++tries <= arc_jtag_ops.retry_count);

    LEAVEMSG;
    return result;
}


/* -------------------------------------------------------------------------- */
/* 6)                  read/write helper functions                            */
/* -------------------------------------------------------------------------- */

/* These functions aid in reading/writing registers/memory.  */

/* Read a processor register (core or auxiliary).  */

static JTAG_OperationStatus
read_processor_register (ARC_RegisterNumber    regnum,
                         JTAG_RegisterContents command,
                         ARC_RegisterContents *contents)
{
    JTAG_OperationStatus status;

    /* Load the number of the register that is to be read into the JTAG Address
       Register.  */
    start_jtag_transaction(command, regnum);

    status = perform_jtag_transaction(Register, JTAG_READ_FAILURE);

    if (status == JTAG_SUCCESS)
    {
        /* Read the register contents from the JTAG Data Register.  */
        *contents = (ARC_RegisterContents) read_jtag_reg(JTAG_DATA_REGISTER, BITS_IN_REGISTER);
    }

    return status;
}


/* Write a processor register (core or auxiliary).  */

static JTAG_OperationStatus
write_processor_register (ARC_RegisterNumber    regnum,
                          JTAG_RegisterContents command,
                          ARC_RegisterContents  contents)
{
    /* Load the number of the register that is to be written into the JTAG
       Address Register.  */
    start_jtag_transaction(command, regnum);

    /* Load the new register contents into the JTAG Data Register.  */
    write_jtag_reg(JTAG_DATA_REGISTER, contents, BITS_IN_REGISTER);

    return perform_jtag_transaction(Register, JTAG_WRITE_FAILURE);
}


/* Write a processor core register.  */

static JTAG_OperationStatus
jtag_write_core_reg (ARC_RegisterNumber regnum, ARC_RegisterContents contents)
{
    ENTERARGS("regnum %d", regnum);

    return write_processor_register(regnum, WRITE_CORE_REGISTER, contents);
}


/* Read a processor auxiliary register.  */

static JTAG_OperationStatus
jtag_read_aux_reg (ARC_RegisterNumber regnum, ARC_RegisterContents *contents)
{
    ENTERARGS("regnum %d", regnum);

    return read_processor_register(regnum, READ_AUX_REGISTER, contents);
}


/* Write a processor auxiliary register.  */

static JTAG_OperationStatus
jtag_write_aux_reg (ARC_RegisterNumber regnum, ARC_RegisterContents contents)
{
    ENTERARGS("regnum %d", regnum);

    return write_processor_register(regnum, WRITE_AUX_REGISTER, contents);
}


/* Read a word of data from memory; the given address must be word-aligned.
   Returns number of bytes read.  */

static unsigned int
jtag_read_word (ARC_Address addr, ARC_Word *data)
{
    JTAG_OperationStatus status;

    ENTERARGS("addr 0x%08X", addr);

    gdb_assert(IS_WORD_ALIGNED(addr));

    /* Load the address of the memory word that is to be read into the JTAG
       Address Register.  */
    start_jtag_transaction(READ_MEMORY_LOCATION, addr);

    status = perform_jtag_transaction(Memory, JTAG_READ_FAILURE);

    if (status == JTAG_SUCCESS)
    {
        /* Read the data from the JTAG Data Register.  */
        ARC_Word word = (ARC_Word) read_jtag_reg(JTAG_DATA_REGISTER, BITS_IN_WORD);

        /* N.B. this assumes that the host is little-endian!  */
        if (target_is_big_endian)
            word = __bswap_32(word);

        DEBUG("read 0x%08X\n", word);

        *data = word;

        return BYTES_IN_WORD;
    }

    /* Failed: no data read.  */
    return 0;
}


/* Write a word of data to memory; the given address must be word-aligned.
   Returns number of bytes written.  */

static unsigned int
jtag_write_word (ARC_Address addr, ARC_Word data)
{
    ENTERARGS("addr 0x%08X, data 0x%08X", addr, data);

    gdb_assert(IS_WORD_ALIGNED(addr));

    /* Load the address of the memory word that is to be written into the JTAG
       Address Register.  */
    start_jtag_transaction(WRITE_MEMORY_LOCATION, addr);

    /* N.B. this assumes that the host is little-endian!  */
    if (target_is_big_endian)
        data = __bswap_32(data);

    /* Load the data to be written into the JTAG Data Register.  */
    write_jtag_reg(JTAG_DATA_REGISTER, data, BITS_IN_WORD);

    if (perform_jtag_transaction(Memory, JTAG_WRITE_FAILURE) == JTAG_SUCCESS)
        return BYTES_IN_WORD;

    /* Failed: no data written.  */
    return 0;
}


/* Read a number of words of data from target memory starting at the given address.
   Returns number of bytes read.  */

static unsigned int
jtag_read_chunk (ARC_Address address, ARC_Byte *data, unsigned int words)
{
    unsigned int total_read = 0;

    ENTERARGS("address 0x%08X, words %u", address, words);

    /* Load the start address of the memory chunk that is to be read
       into the JTAG Address Register.  */
    start_jtag_transaction(READ_MEMORY_LOCATION, address);

    /* Read all the words of data.  */
    while (words--)
    {
        ARC_Word word;

        /* Read the next word of data - this increments the address in
           the JTAG Address Register by the word size, so the register
           does not have to be re-loaded with the next address.  */
        if (perform_jtag_transaction(Memory, JTAG_READ_FAILURE) != JTAG_SUCCESS)
        {
            DEBUG("FAIL: read %u bytes\n", total_read);

            /* Failed - just return amount of data read so far.  */
            return total_read;
        }

        /* Read the word of data from the JTAG Data Register.  */
        word = (ARC_Word) read_jtag_reg(JTAG_DATA_REGISTER, BITS_IN_WORD);

        /* N.B. this assumes that the host is little-endian!  */
        if (target_is_big_endian)
            word = __bswap_32(word);

        /* Copy it into the buffer (byte-by-byte copy means that alignment does not matter).  */
        COPY_WORD(data, &word);

        total_read += BYTES_IN_WORD;
        data       += BYTES_IN_WORD;
   }

    return total_read;
}


/* Write a number of words of data to target memory starting at the given address.
   Returns number of bytes written.  */

static unsigned int
jtag_write_chunk (ARC_Address address, ARC_Byte *data, unsigned int words)
{
    unsigned int total_written = 0;

#ifndef JTAG_DATA_REGISTER_IS_CORRUPTED_BY_WRITE
    ARC_Word last_word;

    /* Initialise last_word with a value that is different from the
       first word to be written.  */
    COPY_WORD(&last_word, data);
    last_word++;
#endif

    ENTERARGS("address 0x%08X, words %u", address, words);

    /* Load the start address of the memory chunk that is to be written
       into the JTAG Address Register.  */
    start_jtag_transaction(WRITE_MEMORY_LOCATION, address);

    /* write all the words of data */
    while (words--)
    {
        ARC_Word word;

        /* Copy the next word of data from the buffer
           (byte-by-byte copy means that alignment does not matter).  */
        COPY_WORD(&word, data);

        /* N.B. this assumes that the host is little-endian!  */
        if (target_is_big_endian)
            word = __bswap_32(word);

#ifndef JTAG_DATA_REGISTER_IS_CORRUPTED_BY_WRITE
        if (word != last_word)
        {
#endif
            write_jtag_reg(JTAG_DATA_REGISTER, word, BITS_IN_WORD);

#ifdef CHECK_JTAG_DATA_REGISTER
            check_Data_Register(word);
#endif

#ifndef JTAG_DATA_REGISTER_IS_CORRUPTED_BY_WRITE
            last_word = word;
        }
#endif

        data += BYTES_IN_WORD;

        /* Write the word - this increments the address in the JTAG
           Address Register by the word size, so the register does not
           have to be re-loaded with the next address.  */
        if (perform_jtag_transaction(Memory, JTAG_WRITE_FAILURE) != JTAG_SUCCESS)
        {
            DEBUG("FAIL: written %u bytes\n", total_written);

            /* Failed - just return amount of data written so far.  */
            return total_written;
        }

        total_written += BYTES_IN_WORD;
    }

    return total_written;
}


/* Write a number of copies of a word-sized pattern of data to memory starting
   at the given address.
   Returns number of bytes written.  */

static unsigned int
jtag_write_pattern (ARC_Address address, ARC_Word pattern, unsigned int words)
{
    unsigned int total_written = 0;

    ENTERARGS("address 0x%08X, pattern 0x%08X, words %u", address, pattern, words);

    /* Load the start address of the memory chunk that is to be written
       into the JTAG Address Register.  */
    start_jtag_transaction(WRITE_MEMORY_LOCATION, address);

    /* N.B. this assumes that the host is little-endian!  */
    if (target_is_big_endian)
        pattern = __bswap_32(pattern);

#ifndef JTAG_DATA_REGISTER_IS_CORRUPTED_BY_WRITE
    /* Load the pattern into the JTAG Data Register.  */
    write_jtag_reg(JTAG_DATA_REGISTER, pattern, BITS_IN_WORD);
#endif

    /* Write all the complete words of data.  */
    while (words--)
    {
#ifdef JTAG_DATA_REGISTER_IS_CORRUPTED_BY_WRITE
        /* Load the pattern into the JTAG Data Register.  */
        write_jtag_reg(JTAG_DATA_REGISTER, pattern, BITS_IN_WORD);
#endif

#ifdef CHECK_JTAG_DATA_REGISTER
        check_Data_Register(pattern);
#endif

        /* Write the word - this increments the address in the JTAG
           Address Register by the word size, so the register does not
           have to be re-loaded with the next address.  */
        if (perform_jtag_transaction(Memory, JTAG_WRITE_FAILURE) != JTAG_SUCCESS)
        {
            DEBUG("FAIL: written %u bytes\n", total_written);

            /* Failed - just return amount of data written so far.  */
            return total_written;
        }

        total_written += BYTES_IN_WORD;
    }

    return total_written;
}


/* -------------------------------------------------------------------------- */
/* 7)                            main operations                              */
/* -------------------------------------------------------------------------- */

/* These are the functions that are called from outside this module via the
   pointers in the arc_jtag_ops global object.
   N.B. none of these functions are called from within this module.  */


/* Read a processor core register.  */

static JTAG_OperationStatus
jtag_read_core_reg (ARC_RegisterNumber regnum, ARC_RegisterContents *contents)
{
    ENTERARGS("regnum %d", regnum);

    return read_processor_register(regnum, READ_CORE_REGISTER, contents);
}


/* Try to open the JTAG interface.
   Returns TRUE for success.  */

static Boolean
jtag_open (ARC_RegisterNumber mem_subsys)
{
    ENTERMSG;

    if (arc_jtag_ops.status == JTAG_CLOSED)
    {
        JTAG_OperationStatus status;

        /* Make sure that the GPIO driver is open.  */
        if (!gpio_open())
            return FALSE;

        set_interface(JTAG_OPENED);

        tapc_reset();

        /* Load the number of the MEMSUBSYS BCR that is to be read into the JTAG
           Address Register.  */
        start_jtag_transaction(READ_AUX_REGISTER, mem_subsys);

        status = perform_jtag_transaction(Register, JTAG_READ_FAILURE);

        if (status == JTAG_SUCCESS)
        {
            /* Read the register contents from the JTAG Data Register.  */
            ARC_RegisterContents contents =
                                 (ARC_RegisterContents) read_jtag_reg(JTAG_DATA_REGISTER, BITS_IN_REGISTER);

            DEBUG("MEMSUBSYS BCR: 0x%08X\n", contents);

            target_is_big_endian = ((contents & 4) != 0);
        }
        else
        {
            warning(_("can not discover endianness of target\n"));
            return FALSE;
        }

        DEBUG("arcjtag opened\n");
    }

    LEAVEMSG;

    return TRUE;
}


/* Close the JTAG interface.  */

static void
jtag_close (void)
{
    ENTERMSG;

    if (arc_jtag_ops.status == JTAG_OPENED)
    {
        tapc_reset();

        /* Close the file descriptor opened for communication with gpio driver.  */
        gpio_close();

        set_interface(JTAG_CLOSED);

#ifdef STATE_MACHINE_DEBUG
        current_state = UNDEFINED;
#endif

        DEBUG("arcjtag closed\n");
    }

    LEAVEMSG;
}


/* Reset the target JTAG controller.  */

static void
jtag_reset (void)
{
    ENTERMSG;
    tapc_reset();
    LEAVEMSG;
}


/* Reset the target board.  */

static void
jtag_reset_board (void)
{
    ENTERMSG;

    /* Make sure that the GPIO driver is open.  */
    if (gpio_open())
    {
        /* Writing 9 did not work. But that's what the manual says. Hmmm.  */
//      gpio_write (CONTROL_PORT, 9);

        /* What is this for?  */
        gpio_write(CONTROL_PORT, (Byte) JTAG_TCK);
        gpio_write(CONTROL_PORT, (Byte) 0xD);
        gpio_write(CONTROL_PORT, (Byte) JTAG_TCK);
        gpio_write(DATA_PORT,    (Byte) 0);
        gpio_write(DATA_PORT,    (Byte) JTAG_TMS);
        gpio_write(DATA_PORT,    (Byte) 0);

        tapc_reset();
    }

    LEAVEMSG;
}


/* Check that the JTAG interface is open.
   If it is closed, 'error' is called.  */

static void jtag_check_open (void)
{
    if (arc_jtag_ops.status == JTAG_CLOSED)
        interface_is_closed();
}


/* -------------------------------------------------------------------------- */
/* 8)                            interface management                         */
/* -------------------------------------------------------------------------- */

/* Report that the JTAG interface is closed.  */

static void
interface_is_closed (void)
{
    error(_("JTAG connection is closed. "
            "Use command 'target " ARC_TARGET_NAME "' first."));
}


/* Set up the function pointers in the arc_jtag_ops structure according to
   whether the JTAG interface is open or closed.  Note that if the interface is
   closed, all the pointers point to the 'interface_is_closed' function - so any
   attempt to invoke one of those operations results in an error; but if the
   interface is open, they point to the appropriate operations (which may be
   called without incurring the overhead of a check on the interface status).  */

static void
set_interface (JTAG_Status status)
{
    arc_jtag_ops.status = status;

    if (status == JTAG_OPENED)
    {
        arc_jtag_ops.memory_read_word     = jtag_read_word;
        arc_jtag_ops.memory_write_word    = jtag_write_word;
        arc_jtag_ops.memory_read_chunk    = jtag_read_chunk;
        arc_jtag_ops.memory_write_chunk   = jtag_write_chunk;
        arc_jtag_ops.memory_write_pattern = jtag_write_pattern;
        arc_jtag_ops.read_aux_reg         = jtag_read_aux_reg;
        arc_jtag_ops.write_aux_reg        = jtag_write_aux_reg;
        arc_jtag_ops.read_core_reg        = jtag_read_core_reg;
        arc_jtag_ops.write_core_reg       = jtag_write_core_reg;
        arc_jtag_ops.reset                = jtag_reset;
    }
    else
    {
        typedef unsigned int         (*Read_Word)       (ARC_Address, ARC_Word*);
        typedef unsigned int         (*Write_Word)      (ARC_Address, ARC_Word);
        typedef unsigned int         (*Transfer_Chunk)  (ARC_Address, ARC_Byte*, unsigned int);
        typedef unsigned int         (*Write_Pattern)   (ARC_Address, ARC_Word,  unsigned int);
        typedef JTAG_OperationStatus (*Read_Register)   (ARC_RegisterNumber, ARC_RegisterContents*);
        typedef JTAG_OperationStatus (*Write_Register)  (ARC_RegisterNumber, ARC_RegisterContents);

        /* The type casts avoid "assignment from incompatible pointer type" warnings
           at compile-time.  */
        arc_jtag_ops.memory_read_word     = (Read_Word)      interface_is_closed;
        arc_jtag_ops.memory_write_word    = (Write_Word)     interface_is_closed;
        arc_jtag_ops.memory_read_chunk    = (Transfer_Chunk) interface_is_closed;
        arc_jtag_ops.memory_write_chunk   = (Transfer_Chunk) interface_is_closed;
        arc_jtag_ops.memory_write_pattern = (Write_Pattern)  interface_is_closed;
        arc_jtag_ops.read_aux_reg         = (Read_Register)  interface_is_closed;
        arc_jtag_ops.write_aux_reg        = (Write_Register) interface_is_closed;
        arc_jtag_ops.read_core_reg        = (Read_Register)  interface_is_closed;
        arc_jtag_ops.write_core_reg       = (Write_Register) interface_is_closed;
        arc_jtag_ops.reset                =                  interface_is_closed;
    }
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Initialize the module. This function is called from the gdb core on start-up.  */

void
_initialize_arc_jtag_ops (void)
{
    ENTERMSG;

    /* Initialize the arc_jtag_ops global variable.  */

    arc_jtag_ops.state_machine_debug = FALSE;
    arc_jtag_ops.retry_count         = 50;

    /* We want to be able to reset the board, and check whether it is connected,
       regardless of the connection state.  */
    arc_jtag_ops.open        = jtag_open;
    arc_jtag_ops.close       = jtag_close;
    arc_jtag_ops.check_open  = jtag_check_open;
    arc_jtag_ops.reset_board = jtag_reset_board;

    /* The JTAG interface is initially closed.  */
    set_interface(JTAG_CLOSED);
}

/******************************************************************************/
