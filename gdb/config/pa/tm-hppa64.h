/* Parameters for execution on any Hewlett-Packard PA-RISC machine.
   Copyright 1986, 1987, 1989, 1990, 1991, 1992, 1993, 1995
   Free Software Foundation, Inc. 

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* PA 64-bit specific definitions.  Override those which are in
   tm-hppa.h */

#include "pa/tm-hppah.h"

#define HPUX_1100 1

/* jimb: this must go.  I'm just using it to disable code I haven't
   gotten working yet.  */
#define GDB_TARGET_IS_HPPA_20W

/* The low two bits of the IA are the privilege level of the instruction.  */
#define ADDR_BITS_REMOVE(addr) ((CORE_ADDR)addr & (CORE_ADDR)~3)

/* Say how long (ordinary) registers are.  This is used in
   push_word and a few other places, but REGISTER_RAW_SIZE is
   the real way to know how big a register is.  */

#undef REGISTER_SIZE
#define REGISTER_SIZE 8

/* Number of bytes of storage in the actual machine representation
   for register N.  On the PA-RISC 2.0, all regs are 8 bytes, including
   the FP registers (they're accessed as two 4 byte halves).  */

#undef REGISTER_RAW_SIZE
#define REGISTER_RAW_SIZE(N) 8

/* Largest value REGISTER_RAW_SIZE can have.  */

#undef MAX_REGISTER_RAW_SIZE
#define MAX_REGISTER_RAW_SIZE 8

/* Total amount of space needed to store our copies of the machine's
   register state, the array `registers'.  */

#undef REGISTER_BYTES
#define REGISTER_BYTES (NUM_REGS * 8)

/* Index within `registers' of the first byte of the space for
   register N.  */

#undef REGISTER_BYTE
#define REGISTER_BYTE(N) ((N) * 8)

#undef REGISTER_VIRTUAL_TYPE
#define REGISTER_VIRTUAL_TYPE(N) \
 ((N) < FP4_REGNUM ? builtin_type_unsigned_long_long : builtin_type_double)


/* Number of machine registers */
#undef NUM_REGS
#define NUM_REGS 96

/* Initializer for an array of names of registers.
   There should be NUM_REGS strings in this initializer.
   They are in rows of eight entries  */
#undef REGISTER_NAMES
#define REGISTER_NAMES	\
 {"flags",  "r1",      "rp",      "r3",    "r4",     "r5",      "r6",     "r7",    \
  "r8",     "r9",      "r10",     "r11",   "r12",    "r13",     "r14",    "r15",   \
  "r16",    "r17",     "r18",     "r19",   "r20",    "r21",     "r22",    "r23",   \
  "r24",    "r25",     "r26",     "dp",    "ret0",   "ret1",    "sp",     "r31",   \
  "sar",    "pcoqh",   "pcsqh",   "pcoqt", "pcsqt",  "eiem",    "iir",    "isr",   \
  "ior",    "ipsw",    "goto",    "sr4",   "sr0",    "sr1",     "sr2",    "sr3",   \
  "sr5",    "sr6",     "sr7",     "cr0",   "cr8",    "cr9",     "ccr",    "cr12",  \
  "cr13",   "cr24",    "cr25",    "cr26",  "mpsfu_high","mpsfu_low","mpsfu_ovflo","pad",\
  "fpsr",    "fpe1",   "fpe2",    "fpe3",  "fr4",    "fr5",     "fr6",    "fr7", \
  "fr8",     "fr9",    "fr10",    "fr11",  "fr12",   "fr13",    "fr14",   "fr15", \
  "fr16",    "fr17",   "fr18",    "fr19",  "fr20",   "fr21",    "fr22",   "fr23", \
  "fr24",    "fr25",   "fr26",    "fr27",   "fr28",  "fr29",    "fr30",   "fr31"}

#undef FP0_REGNUM
#undef FP4_REGNUM
#define FP0_REGNUM 64		/* floating point reg. 0 (fspr)*/
#define FP4_REGNUM 68

/* Redefine some target bit sizes from the default.  */

/* Number of bits in a long or unsigned long for the target machine. */

#define TARGET_LONG_BIT 64

/* Number of bits in a long long or unsigned long long for the 
   target machine.  */

#define TARGET_LONG_LONG_BIT 64

/* Number of bits in a pointer for the target machine */

#define TARGET_PTR_BIT 64

/* Argument Pointer Register */
#define AP_REGNUM 29

#define DP_REGNUM 27

#define FP5_REGNUM 70

#define SR5_REGNUM 48

#undef FRAME_ARGS_ADDRESS
#define FRAME_ARGS_ADDRESS(fi) ((fi)->ap)

/* We access locals from SP. This may not work for frames which call
   alloca; for those, we may need to consult unwind tables.
   jimb: FIXME.  */
#undef FRAME_LOCALS_ADDRESS
#define FRAME_LOCALS_ADDRESS(fi) ((fi)->frame)

#define INIT_FRAME_AP init_frame_ap
  
#define EXTRA_FRAME_INFO  \
  CORE_ADDR ap;

/* For a number of horrible reasons we may have to adjust the location
   of variables on the stack.  Ugh.  jimb: why? */
#define HPREAD_ADJUST_STACK_ADDRESS(ADDR) hpread_adjust_stack_address(ADDR)

extern int hpread_adjust_stack_address PARAMS ((CORE_ADDR));


/* jimb: omitted dynamic linking stuff here */

/* This sequence of words is the instructions

; Call stack frame has already been built by gdb. Since we could be calling
; a varargs function, and we do not have the benefit of a stub to put things in
; the right place, we load the first 8 word of arguments into both the general
; and fp registers.
call_dummy
        copy %r4,%r29
        copy %r5,%r22
        copy %r6,%r27
        fldd -64(0,%r29),%fr4
        fldd -56(0,%r29),%fr5
        fldd -48(0,%r29),%fr6
        fldd -40(0,%r29),%fr7
        fldd -32(0,%r29),%fr8
        fldd -24(0,%r29),%fr9
        fldd -16(0,%r29),%fr10
        fldd -8(0,%r29),%fr11
        copy %r22,%r1
        ldd -64(%r29), %r26
        ldd -56(%r29), %r25
        ldd -48(%r29), %r24
        ldd -40(%r29), %r23
        ldd -32(%r29), %r22
        ldd -24(%r29), %r21
        ldd -16(%r29), %r20
        bve,l (%r1),%r2
        ldd -8(%r29), %r19
        break 4, 8
	mtsp %r21, %sr0
	ble 0(%sr0, %r22)
        nop
*/

#undef CALL_DUMMY
#define CALL_DUMMY {0x349d0000, 0x34b60000, 0x34db0000, \
                    0x53a43f83, 0x53a53f93, 0x53a63fa3, 0x53a73fb3,\
                    0x53a83fc3, 0x53a93fd3, 0x2fa1100a, 0x2fb1100b,\
                    0x36c10000, 0x53ba3f81, 0x53b93f91, 0x53b83fa1,\
                    0x53b73fb1, 0x53b63fc1, 0x53b53fd1, 0x0fa110d4,\
                    0xe820f000, 0x0fb110d3, 0x00010004, 0x00151820,\
                    0xe6c00000, 0x08000240}

#undef CALL_DUMMY_LENGTH
#define CALL_DUMMY_LENGTH (INSTRUCTION_SIZE * 25)

#undef FUNC_LDIL_OFFSET
#undef FUNC_LDO_OFFSET
#undef SR4EXPORT_LDIL_OFFSET
#undef SR4EXPORT_LDO_OFFSET
#undef CALL_DUMMY_LOCATION
/* jimb: need to find out what AT_WDB_CALL_DUMMY is about */
#if 0
#define CALL_DUMMY_LOCATION AFTER_TEXT_END
extern CORE_ADDR wdb_call_dummy_addr;
#undef PC_IN_CALL_DUMMY
#define PC_IN_CALL_DUMMY(pc, sp, frame_address) \
   ((pc) >= wdb_call_dummy_addr && \
    (pc) <= wdb_call_dummy_addr + CALL_DUMMY_LENGTH)
#endif

#undef REG_STRUCT_HAS_ADDR

#undef EXTRACT_RETURN_VALUE
/* RM: floats are returned in FR4R, doubles in FR4
 *     integral values are in r28, padded on the left 
 *     aggregates less that 65 bits are in r28, right padded 
 *     aggregates upto 128 bits are in r28 and r29, right padded
 */ 
#define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \
  { \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT && !SOFT_FLOAT) \
      memcpy ((VALBUF), \
	      ((char *)(REGBUF)) + REGISTER_BYTE (FP4_REGNUM) + \
              (REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
	      TYPE_LENGTH (TYPE)); \
    else if  (is_integral_type(TYPE) || SOFT_FLOAT)   \
       memcpy ((VALBUF), \
               (char *)(REGBUF) + REGISTER_BYTE (28) + \
               (REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 8)   \
       memcpy ((VALBUF), \
               (char *)(REGBUF) + REGISTER_BYTE (28), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 16)   \
      { \
        memcpy ((VALBUF), \
                (char *)(REGBUF) + REGISTER_BYTE (28), \
                8); \
        memcpy (((char *) VALBUF + 8), \
                (char *)(REGBUF) + REGISTER_BYTE (29), \
                TYPE_LENGTH (TYPE) - 8); \
       } \
  }

/* RM: struct upto 128 bits are returned in registers */
#undef USE_STRUCT_CONVENTION
#define USE_STRUCT_CONVENTION(gcc_p, value_type)\
  (TYPE_LENGTH (value_type) > 16)                

/* RM: for return command */
#undef STORE_RETURN_VALUE
#define STORE_RETURN_VALUE(TYPE,VALBUF) \
  { \
    if (TYPE_CODE (TYPE) == TYPE_CODE_FLT && !SOFT_FLOAT) \
      write_register_bytes \
	      (REGISTER_BYTE (FP4_REGNUM) + \
              (REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
              (VALBUF), \
	      TYPE_LENGTH (TYPE)); \
    else if (is_integral_type(TYPE) || SOFT_FLOAT)   \
       write_register_bytes \
              (REGISTER_BYTE (28) + \
                 (REGISTER_SIZE - TYPE_LENGTH (TYPE)), \
               (VALBUF), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 8)   \
       write_register_bytes \
             ( REGISTER_BYTE (28), \
               (VALBUF), \
               TYPE_LENGTH (TYPE)); \
    else if (TYPE_LENGTH (TYPE) <= 16)   \
      { \
        write_register_bytes \
               (REGISTER_BYTE (28), \
                (VALBUF), \
                8); \
        write_register_bytes \
               (REGISTER_BYTE (29), \
                ((char *) VALBUF + 8), \
                TYPE_LENGTH (TYPE) - 8); \
       } \
  }

/* RM: these are the PA64 equivalents of the macros in tm-hppah.h --
 * see comments there.  For PA64, the save_state structure is at an
 * offset of 24 32-bit words from the sigcontext structure. The 64 bit
 * general registers are at an offset of 640 bytes from the beginning of the
 * save_state structure, and the floating pointer register are at an offset
 * of 256 bytes from the beginning of the save_state structure.
 */
#undef FRAME_SAVED_PC_IN_SIGTRAMP
#define FRAME_SAVED_PC_IN_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer ((FRAME)->frame + (24 * 4) + 640 + (33 * 8), 8); \
}

#undef FRAME_BASE_BEFORE_SIGTRAMP
#define FRAME_BASE_BEFORE_SIGTRAMP(FRAME, TMP) \
{ \
  *(TMP) = read_memory_integer ((FRAME)->frame + (24 * 4) + 640 + (30 * 8), 8); \
}

#undef FRAME_FIND_SAVED_REGS_IN_SIGTRAMP
#define FRAME_FIND_SAVED_REGS_IN_SIGTRAMP(FRAME, FSR) \
{ \
  int i; \
  CORE_ADDR TMP1, TMP2; \
  TMP1 = (FRAME)->frame + (24 * 4) + 640; \
  TMP2 = (FRAME)->frame + (24 * 4) + 256; \
  for (i = 0; i < NUM_REGS; i++) \
    { \
      if (i == SP_REGNUM) \
        (FSR)->regs[SP_REGNUM] = read_memory_integer (TMP1 + SP_REGNUM * 8, 8); \
      else if (i >= FP0_REGNUM) \
        (FSR)->regs[i] = TMP2 + (i - FP0_REGNUM) * 8; \
      else \
        (FSR)->regs[i] = TMP1 + i * 8; \
    } \
}

/* jimb: omitted purify call support */
