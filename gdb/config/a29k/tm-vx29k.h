/* OBSOLETE /* Target machine description for VxWorks on the 29k, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1994, 1995, 1998, 1999, 2000 Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by Cygnus Support. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #include "regcache.h" */
/* OBSOLETE #include "value.h" */
/* OBSOLETE #include "a29k/tm-a29k.h" */
/* OBSOLETE #include "tm-vxworks.h" */
/* OBSOLETE  */
/* OBSOLETE /* Number of registers in a ptrace_getregs call. */ */
/* OBSOLETE  */
/* OBSOLETE #define VX_NUM_REGS (NUM_REGS) */
/* OBSOLETE  */
/* OBSOLETE /* Number of registers in a ptrace_getfpregs call. */ */
/* OBSOLETE  */
/* OBSOLETE /* #define VX_SIZE_FPREGS */ */
/* OBSOLETE  */
/* OBSOLETE /* This is almost certainly the wrong place for this: */ */
/* OBSOLETE #define LR2_REGNUM 34 */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Vxworks has its own CALL_DUMMY since it manages breakpoints in the kernel */ */
/* OBSOLETE  */
/* OBSOLETE #undef CALL_DUMMY */
/* OBSOLETE  */
/* OBSOLETE /* Replace the breakpoint instruction in the CALL_DUMMY with a nop. */
/* OBSOLETE    For Vxworks, the breakpoint is set and deleted by calls to */
/* OBSOLETE    CALL_DUMMY_BREAK_SET and CALL_DUMMY_BREAK_DELETE.  */ */
/* OBSOLETE  */
/* OBSOLETE #error "This file is broken.  GDB does not define HOST_BYTE_ORDER." */
/* OBSOLETE #if TARGET_BYTE_ORDER == HOST_BYTE_ORDER */
/* OBSOLETE #define CALL_DUMMY {0x0400870f,\ */
/* OBSOLETE 		0x36008200|(MSP_HW_REGNUM), \ */
/* OBSOLETE 		0x15000040|(MSP_HW_REGNUM<<8)|(MSP_HW_REGNUM<<16), \ */
/* OBSOLETE 		0x03ff80ff, 0x02ff80ff, 0xc8008080, 0x70400101, 0x70400101} */
/* OBSOLETE #else /* Byte order differs.  */ */
/* OBSOLETE #define CALL_DUMMY {0x0f870004,\ */
/* OBSOLETE 		0x00820036|(MSP_HW_REGNUM << 24), \ */
/* OBSOLETE 		0x40000015|(MSP_HW_REGNUM<<8)|(MSP_HW_REGNUM<<16), \ */
/* OBSOLETE 		0xff80ff03, 0xff80ff02, 0x808000c8, 0x01014070, 0x01014070} */
/* OBSOLETE #endif /* Byte order differs.  */ */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* For the basic CALL_DUMMY definitions, see "tm-29k.h."  We use the */
/* OBSOLETE    same CALL_DUMMY code, but define FIX_CALL_DUMMY (and related macros) */
/* OBSOLETE    locally to handle remote debugging of VxWorks targets.  The difference */
/* OBSOLETE    is in the setting and clearing of the breakpoint at the end of the */
/* OBSOLETE    CALL_DUMMY code fragment; under VxWorks, we can't simply insert a */
/* OBSOLETE    breakpoint instruction into the code, since that would interfere with */
/* OBSOLETE    the breakpoint management mechanism on the target. */
/* OBSOLETE    Note that CALL_DUMMY is a piece of code that is used to call any C function */
/* OBSOLETE    thru VxGDB */ */
/* OBSOLETE  */
/* OBSOLETE /* The offset of the instruction within the CALL_DUMMY code where we */
/* OBSOLETE    want the inferior to stop after the function call has completed. */
/* OBSOLETE    call_function_by_hand () sets a breakpoint here (via CALL_DUMMY_BREAK_SET), */
/* OBSOLETE    which POP_FRAME later deletes (via CALL_DUMMY_BREAK_DELETE).  */ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_STOP_OFFSET (7 * 4) */
/* OBSOLETE  */
/* OBSOLETE /* The offset of the first instruction of the CALL_DUMMY code fragment */
/* OBSOLETE    relative to the frame pointer for a dummy frame.  This is equal to */
/* OBSOLETE    the size of the CALL_DUMMY plus the arg_slop area size (see the diagram */
/* OBSOLETE    in "tm-29k.h").  */ */
/* OBSOLETE /* PAD : the arg_slop area size doesn't appear to me to be useful since, the */
/* OBSOLETE    call dummy code no longer modify the msp. See below. This must be checked. */ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_OFFSET_IN_FRAME (CALL_DUMMY_LENGTH + 16 * 4) */
/* OBSOLETE  */
/* OBSOLETE /* Insert the specified number of args and function address */
/* OBSOLETE    into a CALL_DUMMY sequence stored at DUMMYNAME, replace the third */
/* OBSOLETE    instruction (add msp, msp, 16*4) with a nop, and leave the final nop. */
/* OBSOLETE    We can't keep using a CALL_DUMMY that modify the msp since, for VxWorks, */
/* OBSOLETE    CALL_DUMMY is stored in the Memory Stack. Adding 16 words to the msp */
/* OBSOLETE    would then make possible for the inferior to overwrite the CALL_DUMMY code, */
/* OBSOLETE    thus creating a lot of trouble when exiting the inferior to come back in */
/* OBSOLETE    a CALL_DUMMY code that no longer exists... Furthermore, ESF are also stored */
/* OBSOLETE    from the msp in the memory stack. If msp is set higher than the dummy code, */
/* OBSOLETE    an ESF may clobber this code. */ */
/* OBSOLETE  */
/* OBSOLETE #if TARGET_BYTE_ORDER == BFD_ENDIAN_BIG */
/* OBSOLETE #define NOP_INSTR  0x70400101 */
/* OBSOLETE #else /* Target is little endian */ */
/* OBSOLETE #define NOP_INSTR  0x01014070 */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #undef FIX_CALL_DUMMY */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)  \ */
/* OBSOLETE   {                                                                   \ */
/* OBSOLETE     *(int *)((char *)dummyname + 8) = NOP_INSTR;                      \ */
/* OBSOLETE     STUFF_I16((char *)dummyname + CONST_INSN, fun);                   \ */
/* OBSOLETE     STUFF_I16((char *)dummyname + CONST_INSN + 4, fun >> 16);         \ */
/* OBSOLETE   } */
/* OBSOLETE  */
/* OBSOLETE /* For VxWorks, CALL_DUMMY must be stored in the stack of the task that is */
/* OBSOLETE    being debugged and executed "in the context of" this task */ */
/* OBSOLETE  */
/* OBSOLETE #undef CALL_DUMMY_LOCATION */
/* OBSOLETE #define CALL_DUMMY_LOCATION     ON_STACK */
/* OBSOLETE  */
/* OBSOLETE /* Set or delete a breakpoint at the location within a CALL_DUMMY code */
/* OBSOLETE    fragment where we want the target program to stop after the function */
/* OBSOLETE    call is complete.  CALL_DUMMY_ADDR is the address of the first */
/* OBSOLETE    instruction in the CALL_DUMMY.  DUMMY_FRAME_ADDR is the value of the */
/* OBSOLETE    frame pointer in the dummy frame. */
/* OBSOLETE  */
/* OBSOLETE    NOTE: in the both of the following definitions, we take advantage of */
/* OBSOLETE    knowledge of the implementation of the target breakpoint operation, */
/* OBSOLETE    in that we pass a null pointer as the second argument.  It seems */
/* OBSOLETE    reasonable to assume that any target requiring the use of  */
/* OBSOLETE    CALL_DUMMY_BREAK_{SET,DELETE} will not store the breakpoint */
/* OBSOLETE    shadow contents in GDB; in any case, this assumption is vaild */
/* OBSOLETE    for all VxWorks-related targets.  */ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_BREAK_SET(call_dummy_addr) \ */
/* OBSOLETE   target_insert_breakpoint ((call_dummy_addr) + CALL_DUMMY_STOP_OFFSET, \ */
/* OBSOLETE 			    (char *) 0) */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_BREAK_DELETE(dummy_frame_addr) \ */
/* OBSOLETE   target_remove_breakpoint ((dummy_frame_addr) - (CALL_DUMMY_OFFSET_IN_FRAME \ */
/* OBSOLETE 				                  - CALL_DUMMY_STOP_OFFSET), \ */
/* OBSOLETE 			    (char *) 0) */
/* OBSOLETE  */
/* OBSOLETE /* Return nonzero if the pc is executing within a CALL_DUMMY frame.  */ */
/* OBSOLETE  */
/* OBSOLETE #define PC_IN_CALL_DUMMY(pc, sp, frame_address) \ */
/* OBSOLETE   ((pc) >= (sp) \ */
/* OBSOLETE     && (pc) <= (sp) + CALL_DUMMY_OFFSET_IN_FRAME + CALL_DUMMY_LENGTH) */
/* OBSOLETE  */
/* OBSOLETE /* Defining this prevents us from trying to pass a structure-valued argument */
/* OBSOLETE    to a function called via the CALL_DUMMY mechanism.  This is not handled */
/* OBSOLETE    properly in call_function_by_hand (), and the fix might require re-writing */
/* OBSOLETE    the CALL_DUMMY handling for all targets (at least, a clean solution */
/* OBSOLETE    would probably require this).  Arguably, this should go in "tm-29k.h" */
/* OBSOLETE    rather than here.  */ */
/* OBSOLETE  */
/* OBSOLETE #define STRUCT_VAL_ARGS_UNSUPPORTED */
/* OBSOLETE  */
/* OBSOLETE #define BKPT_OFFSET	(7 * 4) */
/* OBSOLETE #define BKPT_INSTR	0x72500101 */
/* OBSOLETE  */
/* OBSOLETE #undef FIX_CALL_DUMMY */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p)   \ */
/* OBSOLETE   {\ */
/* OBSOLETE     STUFF_I16((char *)dummyname + CONST_INSN, fun);\ */
/* OBSOLETE     STUFF_I16((char *)dummyname + CONST_INSN + 4, fun >> 16);\ */
/* OBSOLETE     *(int *)((char *)dummyname + BKPT_OFFSET) = BKPT_INSTR;\ */
/* OBSOLETE   } */
/* OBSOLETE  */
/* OBSOLETE  */
/* OBSOLETE /* Offsets into jmp_buf.  They are derived from VxWorks' REG_SET struct */
/* OBSOLETE    (see VxWorks' setjmp.h). Note that Sun2, Sun3 and SunOS4 and VxWorks have */
/* OBSOLETE    different REG_SET structs, hence different layouts for the jmp_buf struct. */
/* OBSOLETE    Only JB_PC is needed for getting the saved PC value.  */ */
/* OBSOLETE  */
/* OBSOLETE #define JB_ELEMENT_SIZE 4	/* size of each element in jmp_buf */ */
/* OBSOLETE #define JB_PC		3	/* offset of pc (pc1) in jmp_buf */ */
/* OBSOLETE  */
/* OBSOLETE /* Figure out where the longjmp will land.  We expect that we have just entered */
/* OBSOLETE    longjmp and haven't yet setup the stack frame, so the args are still in the */
/* OBSOLETE    output regs.  lr2 (LR2_REGNUM) points at the jmp_buf structure from which we */
/* OBSOLETE    extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR. */
/* OBSOLETE    This routine returns true on success */ */
/* OBSOLETE  */
/* OBSOLETE #define GET_LONGJMP_TARGET(ADDR) get_longjmp_target(ADDR) */
/* OBSOLETE extern int get_longjmp_target (CORE_ADDR *); */
/* OBSOLETE  */
/* OBSOLETE /* VxWorks adjusts the PC after a breakpoint has been hit.  */ */
/* OBSOLETE  */
/* OBSOLETE #undef DECR_PC_AFTER_BREAK */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 0 */
/* OBSOLETE  */
/* OBSOLETE /* Do whatever promotions are appropriate on a value being returned */
/* OBSOLETE    from a function.  VAL is the user-supplied value, and FUNC_TYPE */
/* OBSOLETE    is the return type of the function if known, else 0. */
/* OBSOLETE  */
/* OBSOLETE    For the Am29k, as far as I understand, if the function return type is known, */
/* OBSOLETE    cast the value to that type; otherwise, ensure that integer return values */
/* OBSOLETE    fill all of gr96. */
/* OBSOLETE  */
/* OBSOLETE    This definition really belongs in "tm-29k.h", since it applies */
/* OBSOLETE    to most Am29K-based systems; but once moved into that file, it might */
/* OBSOLETE    need to be redefined for all Am29K-based targets that also redefine */
/* OBSOLETE    STORE_RETURN_VALUE.  For now, to be safe, we define it here.  */ */
/* OBSOLETE  */
/* OBSOLETE #define PROMOTE_RETURN_VALUE(val, func_type) \ */
/* OBSOLETE   do {                                                                  \ */
/* OBSOLETE       if (func_type)                                                    \ */
/* OBSOLETE         val = value_cast (func_type, val);                              \ */
/* OBSOLETE       if ((TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_INT                \ */
/* OBSOLETE            || TYPE_CODE (VALUE_TYPE (val)) == TYPE_CODE_ENUM)           \ */
/* OBSOLETE           && TYPE_LENGTH (VALUE_TYPE (val)) < REGISTER_RAW_SIZE (0))    \ */
/* OBSOLETE         val = value_cast (builtin_type_int, val);                       \ */
/* OBSOLETE   } while (0) */
/* OBSOLETE  */
/* OBSOLETE extern int vx29k_frame_chain_valid (CORE_ADDR, struct frame_info *); */
/* OBSOLETE #define FRAME_CHAIN_VALID(chain, thisframe) vx29k_frame_chain_valid (chain, thisframe) */
/* OBSOLETE  */
/* OBSOLETE extern CORE_ADDR frame_saved_call_site (); */
/* OBSOLETE  */
/* OBSOLETE #undef PREPARE_TO_INIT_FRAME_INFO */
/* OBSOLETE #define PREPARE_TO_INIT_FRAME_INFO(fci)	do {				      \ */
/* OBSOLETE   long current_msp = read_register (MSP_REGNUM);			      \ */
/* OBSOLETE   if (PC_IN_CALL_DUMMY (fci->pc, current_msp, 0))			      \ */
/* OBSOLETE     {									      \ */
/* OBSOLETE       fci->rsize = DUMMY_FRAME_RSIZE;					      \ */
/* OBSOLETE       fci->msize = 0;							      \ */
/* OBSOLETE       fci->saved_msp =	 						      \ */
/* OBSOLETE 	read_register_stack_integer (fci->frame + DUMMY_FRAME_RSIZE - 4, 4);  \ */
/* OBSOLETE       fci->flags |= (TRANSPARENT|MFP_USED);				      \ */
/* OBSOLETE       return;								      \ */
/* OBSOLETE     }									      \ */
/* OBSOLETE   } while (0) */
