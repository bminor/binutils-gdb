/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    */


#ifndef _FUNCTION_UNIT_C_
#define _FUNCTION_UNIT_C_

#ifndef STATIC_INLINE_FUNCTION_UNIT
#define STATIC_INLINE_FUNCTION_UNIT STATIC_INLINE
#endif


#include "basics.h"
#include "cpu.h"
#include "function_unit.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

typedef enum _unit_index {
  UNIT_UNKNOWN,				/* unknown functional unit */
  UNIT_INT,				/* integer functional unit */
  UNIT_SINT,				/* integer or SRU functional unit */
  UNIT_CINT,				/* complex integer functional unit */
  UNIT_FPU,				/* floating point functional unit */
  UNIT_MEM,				/* memory functional unit */
  UNIT_BRANCH,				/* branch functional unit */
  UNIT_SREG,				/* system register functional unit */
  NR_UNITS
} unit_index;

static const char *unit_names[] = {
  "unknown functional unit instruction",		/* UNIT_UNKNOWN */
  "int functional unit instruction",			/* UNIT_INT */
  "int or SRU functional unit instruction",		/* UNIT_SINT */
  "complex int functional unit instruction",		/* UNIT_CINT */
  "floating point functional unit instruction",		/* UNIT_FPU */
  "memory function unit instruction",			/* UNIT_MEM */
  "branch functional unit instruction",			/* UNIT_BRANCH */
  "system register functional unit instruction",	/* UNIT_SREG */
};

typedef struct _timing {
  itable_index index;					/* instruction # */
  unit_index unit;					/* functional unit */
  int cycles;						/* # cycles */
  unsigned flags;					/* random other flags */
} timing;

struct _function_unit {
  unsigned_word old_program_counter;
  unsigned nr_branches;
  unsigned nr_stalls;
  unsigned nr_units[ (int)NR_UNITS ];
  struct {
    unit_index unit;
    int cycles;
    unsigned flags;
  } time[ (int)nr_itable_entries ];
};

/* Flags used in timing info */

#define FUNC_LOAD	0x00000001			/* this is a load */
#define	FUNC_STORE	0x00000002			/* this is a store */


/* 603 timings */
static timing time_603[] = {
  /* Instruction index							Function unit	Cycles	Flags */
  { itable_AND,								UNIT_INT,	1,	0 },
  { itable_AND_Immediate,						UNIT_INT,	1,	0 },
  { itable_AND_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { itable_AND_with_Complement,						UNIT_INT,	1,	0 },
  { itable_Add,								UNIT_SINT,	1,	0 },
  { itable_Add_Carrying,						UNIT_INT,	1,	0 },
  { itable_Add_Extended,						UNIT_INT,	1,	0 },
  { itable_Add_Immediate,						UNIT_SINT,	1,	0 },
  { itable_Add_Immediate_Carrying,					UNIT_INT,	1,	0 },
  { itable_Add_Immediate_Carrying_and_Record,				UNIT_INT,	1,	0 },
  { itable_Add_Immediate_Shifted,					UNIT_SINT,	1,	0 },
  { itable_Add_to_Minus_One_Extended,					UNIT_INT,	1,	0 },
  { itable_Add_to_Zero_Extended,					UNIT_INT,	1,	0 },
  { itable_Branch,							UNIT_BRANCH,	1,	0 },
  { itable_Branch_Conditional,						UNIT_BRANCH,	1,	0 },
  { itable_Branch_Conditional_to_Count_Register,			UNIT_BRANCH,	1,	0 },
  { itable_Branch_Conditional_to_Link_Register,				UNIT_BRANCH,	1,	0 },
  { itable_Compare,							UNIT_SINT,	1,	0 },
  { itable_Compare_Immediate,						UNIT_SINT,	1,	0 },
  { itable_Compare_Logical,						UNIT_SINT,	1,	0 },
  { itable_Compare_Logical_Immediate,					UNIT_SINT,	1,	0 },
  { itable_Condition_Register_AND,					UNIT_SREG,	1,	0 },
  { itable_Condition_Register_AND_with_Complement,			UNIT_SREG,	1,	0 },
  { itable_Condition_Register_Equivalent,				UNIT_SREG,	1,	0 },
  { itable_Condition_Register_NAND,					UNIT_SREG,	1,	0 },
  { itable_Condition_Register_NOR,					UNIT_SREG,	1,	0 },
  { itable_Condition_Register_OR,					UNIT_SREG,	1,	0 },
  { itable_Condition_Register_OR_with_Complement,			UNIT_SREG,	1,	0 },
  { itable_Condition_Register_XOR,					UNIT_SREG,	1,	0 },
  { itable_Count_Leading_Zeros_Word,					UNIT_INT,	1,	0 },
  { itable_Data_Cache_Block_Flush,					UNIT_MEM,	5,	0 },
  { itable_Data_Cache_Block_Invalidate,					UNIT_MEM,	2,	0 },
  { itable_Data_Cache_Block_Store,					UNIT_MEM,	5,	0 },
  { itable_Data_Cache_Block_Touch,					UNIT_MEM,	2,	0 },
  { itable_Data_Cache_Block_Touch_for_Store,				UNIT_MEM,	2,	0 },
  { itable_Data_Cache_Block_set_to_Zero,				UNIT_MEM,	10,	0 },
  { itable_Divide_Word,							UNIT_INT,	37,	0 },
  { itable_Divide_Word_Unsigned,					UNIT_INT,	37,	0 },
  { itable_Enforce_Inorder_Execution_of_IO,				UNIT_SREG,	1,	0 },
  { itable_Equivalent,							UNIT_INT,	1,	0 },
  { itable_Extend_Sign_Byte,						UNIT_INT,	1,	0 },
  { itable_Extend_Sign_Half_Word,					UNIT_INT,	1,	0 },
  { itable_External_Control_In_Word_Indexed,				UNIT_MEM,	2,	0 },
  { itable_External_Control_Out_Word_Indexed,				UNIT_MEM,	2,	0 },
  { itable_Floating_Absolute_Value,					UNIT_FPU,	1,	0 },
  { itable_Floating_Add,						UNIT_FPU,	1,	0 },
  { itable_Floating_Add_Single,						UNIT_FPU,	1,	0 },
  { itable_Floating_Compare_Ordered,					UNIT_FPU,	1,	0 },
  { itable_Floating_Compare_Unordered,					UNIT_FPU,	1,	0 },
  { itable_Floating_Convert_To_Integer_Word,				UNIT_FPU,	1,	0 },
  { itable_Floating_Convert_To_Integer_Word_with_round_towards_Zero,	UNIT_FPU,	1,	0 },
  { itable_Floating_Divide,						UNIT_FPU,	33,	0 },
  { itable_Floating_Divide_Single,					UNIT_FPU,	18,	0 },
  { itable_Floating_Move_Register,					UNIT_FPU,	1,	0 },
  { itable_Floating_Multiply,						UNIT_FPU,	2,	0 },
  { itable_Floating_MultiplyAdd,					UNIT_FPU,	2,	0 },
  { itable_Floating_MultiplyAdd_Single,					UNIT_FPU,	1,	0 },
  { itable_Floating_MultiplySubtract,					UNIT_FPU,	2,	0 },
  { itable_Floating_MultiplySubtract_Single,				UNIT_FPU,	1,	0 },
  { itable_Floating_Multiply_Single,					UNIT_FPU,	1,	0 },
  { itable_Floating_Negate,						UNIT_FPU,	1,	0 },
  { itable_Floating_Negative_Absolute_Value,				UNIT_FPU,	1,	0 },
  { itable_Floating_Negative_MultiplyAdd,				UNIT_FPU,	2,	0 },
  { itable_Floating_Negative_MultiplyAdd_Single,			UNIT_FPU,	1,	0 },
  { itable_Floating_Negative_MultiplySubtract,				UNIT_FPU,	2,	0 },
  { itable_Floating_Negative_MultiplySubtract_Single,			UNIT_FPU,	1,	0 },
  { itable_Floating_Reciprocal_Estimate_Single,				UNIT_FPU,	18,	0 },
  { itable_Floating_Reciprocal_Square_Root_Estimate,			UNIT_FPU,	1,	0 },
  { itable_Floating_Round_to_SinglePrecision,				UNIT_FPU,	1,	0 },
  { itable_Floating_Select,						UNIT_FPU,	1,	0 },
  { itable_Floating_Square_Root,					UNIT_UNKNOWN,	0,	0 },
  { itable_Floating_Square_Root_Single,					UNIT_UNKNOWN,	0,	0 },
  { itable_Floating_Subtract,						UNIT_FPU,	1,	0 },
  { itable_Floating_Subtract_Single,					UNIT_FPU,	1,	0 },
  { itable_Instruction_Cache_Block_Invalidate,				UNIT_MEM,	3,	0 },
  { itable_Instruction_Synchronize,					UNIT_SREG,	1,	0 },
  { itable_Load_Byte_and_Zero,						UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Byte_and_Zero_Indexed,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Byte_and_Zero_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Byte_and_Zero_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double_with_Update,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double_with_Update_Indexed,		UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single_with_Update,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single_with_Update_Indexed,		UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_ByteReverse_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Multiple_Word,						UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_String_Word_Immediate,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_String_Word_Indexed,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_And_Reserve_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_ByteReverse_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero,						UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero_Indexed,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Move_Condition_Register_Field,				UNIT_SREG,	1,	0 },
  { itable_Move_From_Condition_Register,				UNIT_SREG,	1,	0 },
  { itable_Move_From_FPSCR,						UNIT_FPU,	1,	0 },
  { itable_Move_From_Machine_State_Register,				UNIT_SREG,	1,	0 },
  { itable_Move_From_Segment_Register,					UNIT_SREG,	3,	0 },
  { itable_Move_From_Segment_Register_Indirect,				UNIT_SREG,	1,	0 },
  { itable_Move_From_Time_Base,						UNIT_SREG,	1,	0 },
  { itable_Move_To_FPSCR_Bit_0,						UNIT_FPU,	1,	0 },
  { itable_Move_To_FPSCR_Bit_1,						UNIT_FPU,	1,	0 },
  { itable_Move_To_FPSCR_Field_Immediate,				UNIT_FPU,	1,	0 },
  { itable_Move_To_FPSCR_Fields,					UNIT_FPU,	1,	0 },
  { itable_Move_To_Machine_State_Register,				UNIT_SREG,	2,	0 },
  { itable_Move_To_Segment_Register,					UNIT_SREG,	2,	0 },
  { itable_Move_To_Segment_Register_Indirect,				UNIT_SREG,	2,	0 },
  { itable_Move_from_Special_Purpose_Register,				UNIT_SREG,	1,	0 },
  { itable_Move_to_Condition_Register_Fields,				UNIT_SREG,	1,	0 },
  { itable_Move_to_Condition_Register_from_FPSCR,			UNIT_FPU,	1,	0 },
  { itable_Move_to_Condition_Register_from_XER,				UNIT_SREG,	1,	0 },
  { itable_Move_to_Special_Purpose_Register,				UNIT_SREG,	1,	0 },
  { itable_Multiply_High_Word,						UNIT_INT,	5,	0 },
  { itable_Multiply_High_Word_Unsigned,					UNIT_INT,	6,	0 },
  { itable_Multiply_Low_Immediate,					UNIT_INT,	3,	0 },
  { itable_Multiply_Low_Word,						UNIT_INT,	5,	0 },
  { itable_NAND,							UNIT_INT,	1,	0 },
  { itable_NOR,								UNIT_INT,	1,	0 },
  { itable_Negate,							UNIT_INT,	1,	0 },
  { itable_OR,								UNIT_INT,	1,	0 },
  { itable_OR_Immediate,						UNIT_INT,	1,	0 },
  { itable_OR_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { itable_OR_with_Complement,						UNIT_INT,	1,	0 },
  { itable_Return_From_Interrupt,					UNIT_SREG,	3,	0 },
  { itable_Rotate_Left_Word_Immediate_then_AND_with_Mask,		UNIT_INT,	1,	0 },
  { itable_Rotate_Left_Word_Immediate_then_Mask_Insert,			UNIT_INT,	1,	0 },
  { itable_Rotate_Left_Word_then_AND_with_Mask,				UNIT_INT,	1,	0 },
  { itable_Shift_Left_Word,						UNIT_INT,	1,	0 },
  { itable_Shift_Right_Algebraic_Word,					UNIT_INT,	1,	0 },
  { itable_Shift_Right_Algebraic_Word_Immediate,			UNIT_INT,	1,	0 },
  { itable_Shift_Right_Word,						UNIT_INT,	1,	0 },
  { itable_Store_Byte,							UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Byte_Indexed,						UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Byte_with_Update,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Byte_with_Update_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double_with_Update,			UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double_with_Update_Indexed,		UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single_with_Update,			UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single_with_Update_Indexed,		UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_FloatingPoint_as_Integer_Word_Indexed,			UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Half_Word,						UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Half_Word_ByteReversed_Indexed,			UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Half_Word_Indexed,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Half_Word_with_Update,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Half_Word_with_Update_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Multiple_Word,						UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_String_Word_Immedate,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_String_Word_Indexed,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Word,							UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Word_ByteReversed_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Word_Conditional_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Word_Indexed,						UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Word_with_Update,					UNIT_MEM,	1,	FUNC_STORE },
  { itable_Store_Word_with_Update_Indexed,				UNIT_MEM,	1,	FUNC_STORE },
  { itable_Subtract_From,						UNIT_INT,	1,	0 },
  { itable_Subtract_From_Carrying,					UNIT_INT,	1,	0 },
  { itable_Subtract_From_Extended,					UNIT_INT,	1,	0 },
  { itable_Subtract_From_Immediate_Carrying,				UNIT_INT,	1,	0 },
  { itable_Subtract_From_Minus_One_Extended,				UNIT_INT,	1,	0 },
  { itable_Subtract_from_Zero_Extended,					UNIT_INT,	1,	0 },
  { itable_Synchronize,							UNIT_SREG,	3,	0 },
  { itable_System_Call,							UNIT_MEM,	3,	0 },
  { itable_TLB_Invalidate_All,						UNIT_MEM,	3,	0 },
  { itable_TLB_Invalidate_Entry,					UNIT_MEM,	3,	0 },
  { itable_TLB_Sychronize,						UNIT_MEM,	3,	0 },
  { itable_Trap_Word,							UNIT_INT,	2,	0 },
  { itable_Trap_Word_Immediate,						UNIT_INT,	2,	0 },
  { itable_XOR,								UNIT_INT,	1,	0 },
  { itable_XOR_Immediate,						UNIT_INT,	1,	0 },
  { itable_XOR_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { nr_itable_entries,							UNIT_UNKNOWN,	-1,	0 },
};


/* 604 timings */
static timing time_604[] = {
  /* Instruction index							Function unit	Cycles	Flags */
  { itable_AND,								UNIT_INT,	1,	0 },
  { itable_AND_Immediate,						UNIT_INT,	1,	0 },
  { itable_AND_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { itable_AND_with_Complement,						UNIT_INT,	1,	0 },
  { itable_Add,								UNIT_INT,	1,	0 },
  { itable_Add_Carrying,						UNIT_INT,	1,	0 },
  { itable_Add_Extended,						UNIT_INT,	1,	0 },
  { itable_Add_Immediate,						UNIT_INT,	1,	0 },
  { itable_Add_Immediate_Carrying,					UNIT_INT,	1,	0 },
  { itable_Add_Immediate_Carrying_and_Record,				UNIT_INT,	1,	0 },
  { itable_Add_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { itable_Add_to_Minus_One_Extended,					UNIT_INT,	1,	0 },
  { itable_Add_to_Zero_Extended,					UNIT_INT,	1,	0 },
  { itable_Branch,							UNIT_BRANCH,	1,	0 },
  { itable_Branch_Conditional,						UNIT_BRANCH,	1,	0 },
  { itable_Branch_Conditional_to_Count_Register,			UNIT_BRANCH,	1,	0 },
  { itable_Branch_Conditional_to_Link_Register,				UNIT_BRANCH,	1,	0 },
  { itable_Compare,							UNIT_INT,	1,	0 },
  { itable_Compare_Immediate,						UNIT_INT,	1,	0 },
  { itable_Compare_Logical,						UNIT_INT,	1,	0 },
  { itable_Compare_Logical_Immediate,					UNIT_INT,	1,	0 },
  { itable_Condition_Register_AND,					UNIT_INT,	1,	0 },
  { itable_Condition_Register_AND_with_Complement,			UNIT_INT,	1,	0 },
  { itable_Condition_Register_Equivalent,				UNIT_INT,	1,	0 },
  { itable_Condition_Register_NAND,					UNIT_INT,	1,	0 },
  { itable_Condition_Register_NOR,					UNIT_INT,	1,	0 },
  { itable_Condition_Register_OR,					UNIT_INT,	1,	0 },
  { itable_Condition_Register_OR_with_Complement,			UNIT_INT,	1,	0 },
  { itable_Condition_Register_XOR,					UNIT_INT,	1,	0 },
  { itable_Count_Leading_Zeros_Word,					UNIT_INT,	1,	0 },
  { itable_Data_Cache_Block_Flush,					UNIT_MEM,	1,	0 },
  { itable_Data_Cache_Block_Invalidate,					UNIT_MEM,	1,	0 },
  { itable_Data_Cache_Block_Store,					UNIT_MEM,	1,	0 },
  { itable_Data_Cache_Block_Touch,					UNIT_MEM,	1,	0 },
  { itable_Data_Cache_Block_Touch_for_Store,				UNIT_MEM,	1,	0 },
  { itable_Data_Cache_Block_set_to_Zero,				UNIT_MEM,	3,	0 },
  { itable_Divide_Word,							UNIT_CINT,	20,	0 },
  { itable_Divide_Word_Unsigned,					UNIT_CINT,	20,	0 },
  { itable_Enforce_Inorder_Execution_of_IO,				UNIT_INT,	1,	0 },
  { itable_Equivalent,							UNIT_INT,	1,	0 },
  { itable_Extend_Sign_Byte,						UNIT_INT,	1,	0 },
  { itable_Extend_Sign_Half_Word,					UNIT_INT,	1,	0 },
  { itable_External_Control_In_Word_Indexed,				UNIT_MEM,	2,	0 },
  { itable_External_Control_Out_Word_Indexed,				UNIT_MEM,	3,	0 },
  { itable_Floating_Absolute_Value,					UNIT_FPU,	3,	0 },
  { itable_Floating_Add,						UNIT_FPU,	3,	0 },
  { itable_Floating_Add_Single,						UNIT_FPU,	3,	0 },
  { itable_Floating_Compare_Ordered,					UNIT_FPU,	3,	0 },
  { itable_Floating_Compare_Unordered,					UNIT_FPU,	3,	0 },
  { itable_Floating_Convert_To_Integer_Word,				UNIT_FPU,	3,	0 },
  { itable_Floating_Convert_To_Integer_Word_with_round_towards_Zero,	UNIT_FPU,	3,	0 },
  { itable_Floating_Divide,						UNIT_FPU,	32,	0 },
  { itable_Floating_Divide_Single,					UNIT_FPU,	18,	0 },
  { itable_Floating_Move_Register,					UNIT_FPU,	3,	0 },
  { itable_Floating_Multiply,						UNIT_FPU,	3,	0 },
  { itable_Floating_MultiplyAdd,					UNIT_FPU,	3,	0 },
  { itable_Floating_MultiplyAdd_Single,					UNIT_FPU,	3,	0 },
  { itable_Floating_MultiplySubtract,					UNIT_FPU,	3,	0 },
  { itable_Floating_MultiplySubtract_Single,				UNIT_FPU,	3,	0 },
  { itable_Floating_Multiply_Single,					UNIT_FPU,	3,	0 },
  { itable_Floating_Negate,						UNIT_FPU,	3,	0 },
  { itable_Floating_Negative_Absolute_Value,				UNIT_FPU,	3,	0 },
  { itable_Floating_Negative_MultiplyAdd,				UNIT_FPU,	3,	0 },
  { itable_Floating_Negative_MultiplyAdd_Single,			UNIT_FPU,	3,	0 },
  { itable_Floating_Negative_MultiplySubtract,				UNIT_FPU,	3,	0 },
  { itable_Floating_Negative_MultiplySubtract_Single,			UNIT_FPU,	3,	0 },
  { itable_Floating_Reciprocal_Estimate_Single,				UNIT_FPU,	18,	0 },
  { itable_Floating_Reciprocal_Square_Root_Estimate,			UNIT_FPU,	3,	0 },
  { itable_Floating_Round_to_SinglePrecision,				UNIT_FPU,	3,	0 },
  { itable_Floating_Select,						UNIT_FPU,	3,	0 },
  { itable_Floating_Square_Root,					UNIT_UNKNOWN,	0,	0 },
  { itable_Floating_Square_Root_Single,					UNIT_UNKNOWN,	0,	0 },
  { itable_Floating_Subtract,						UNIT_FPU,	3,	0 },
  { itable_Floating_Subtract_Single,					UNIT_FPU,	3,	0 },
  { itable_Instruction_Cache_Block_Invalidate,				UNIT_MEM,	1,	0 },
  { itable_Instruction_Synchronize,					UNIT_INT,	1,	0 },
  { itable_Load_Byte_and_Zero,						UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Byte_and_Zero_Indexed,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Byte_and_Zero_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Byte_and_Zero_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double,					UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double_Indexed,				UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double_with_Update,			UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Double_with_Update_Indexed,		UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single,					UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single_Indexed,				UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single_with_Update,			UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_FloatingPoint_Single_with_Update_Indexed,		UNIT_MEM,	3,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_Algebraic_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_ByteReverse_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Halfword_and_Zero_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Multiple_Word,						UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_String_Word_Immediate,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_String_Word_Indexed,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_And_Reserve_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_ByteReverse_Indexed,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero,						UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero_Indexed,					UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero_with_Update,				UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Load_Word_and_Zero_with_Update_Indexed,			UNIT_MEM,	2,	FUNC_LOAD },
  { itable_Move_Condition_Register_Field,				UNIT_BRANCH,	1,	0 },
  { itable_Move_From_Condition_Register,				UNIT_CINT,	3,	0 },
  { itable_Move_From_FPSCR,						UNIT_FPU,	3,	0 },
  { itable_Move_From_Machine_State_Register,				UNIT_CINT,	3,	0 },
  { itable_Move_From_Segment_Register,					UNIT_CINT,	3,	0 },
  { itable_Move_From_Segment_Register_Indirect,				UNIT_CINT,	3,	0 },
  { itable_Move_From_Time_Base,						UNIT_CINT,	3,	0 },
  { itable_Move_To_FPSCR_Bit_0,						UNIT_FPU,	3,	0 },
  { itable_Move_To_FPSCR_Bit_1,						UNIT_FPU,	3,	0 },
  { itable_Move_To_FPSCR_Field_Immediate,				UNIT_FPU,	3,	0 },
  { itable_Move_To_FPSCR_Fields,					UNIT_FPU,	3,	0 },
  { itable_Move_To_Machine_State_Register,				UNIT_CINT,	1,	0 },
  { itable_Move_To_Segment_Register,					UNIT_CINT,	1,	0 },
  { itable_Move_To_Segment_Register_Indirect,				UNIT_CINT,	1,	0 },
  { itable_Move_from_Special_Purpose_Register,				UNIT_CINT,	1,	0 },
  { itable_Move_to_Condition_Register_Fields,				UNIT_CINT,	1,	0 },
  { itable_Move_to_Condition_Register_from_FPSCR,			UNIT_FPU,	3,	0 },
  { itable_Move_to_Condition_Register_from_XER,				UNIT_CINT,	3,	0 },
  { itable_Move_to_Special_Purpose_Register,				UNIT_CINT,	1,	0 },
  { itable_Multiply_High_Word,						UNIT_CINT,	4,	0 },
  { itable_Multiply_High_Word_Unsigned,					UNIT_CINT,	4,	0 },
  { itable_Multiply_Low_Immediate,					UNIT_CINT,	3,	0 },
  { itable_Multiply_Low_Word,						UNIT_CINT,	4,	0 },
  { itable_NAND,							UNIT_INT,	1,	0 },
  { itable_NOR,								UNIT_INT,	1,	0 },
  { itable_Negate,							UNIT_INT,	1,	0 },
  { itable_OR,								UNIT_INT,	1,	0 },
  { itable_OR_Immediate,						UNIT_INT,	1,	0 },
  { itable_OR_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { itable_OR_with_Complement,						UNIT_INT,	1,	0 },
  { itable_Return_From_Interrupt,					UNIT_INT,	3,	0 },
  { itable_Rotate_Left_Word_Immediate_then_AND_with_Mask,		UNIT_INT,	1,	0 },
  { itable_Rotate_Left_Word_Immediate_then_Mask_Insert,			UNIT_INT,	1,	0 },
  { itable_Rotate_Left_Word_then_AND_with_Mask,				UNIT_INT,	1,	0 },
  { itable_Shift_Left_Word,						UNIT_INT,	1,	0 },
  { itable_Shift_Right_Algebraic_Word,					UNIT_INT,	1,	0 },
  { itable_Shift_Right_Algebraic_Word_Immediate,			UNIT_INT,	1,	0 },
  { itable_Shift_Right_Word,						UNIT_INT,	1,	0 },
  { itable_Store_Byte,							UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Byte_Indexed,						UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Byte_with_Update,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Byte_with_Update_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double_with_Update,			UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Double_with_Update_Indexed,		UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single_with_Update,			UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_Single_with_Update_Indexed,		UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_FloatingPoint_as_Integer_Word_Indexed,			UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Half_Word,						UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Half_Word_ByteReversed_Indexed,			UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Half_Word_Indexed,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Half_Word_with_Update,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Half_Word_with_Update_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Multiple_Word,						UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_String_Word_Immedate,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_String_Word_Indexed,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Word,							UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Word_ByteReversed_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Word_Conditional_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Word_Indexed,						UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Word_with_Update,					UNIT_MEM,	3,	FUNC_STORE },
  { itable_Store_Word_with_Update_Indexed,				UNIT_MEM,	3,	FUNC_STORE },
  { itable_Subtract_From,						UNIT_INT,	1,	0 },
  { itable_Subtract_From_Carrying,					UNIT_INT,	1,	0 },
  { itable_Subtract_From_Extended,					UNIT_INT,	1,	0 },
  { itable_Subtract_From_Immediate_Carrying,				UNIT_INT,	1,	0 },
  { itable_Subtract_From_Minus_One_Extended,				UNIT_INT,	1,	0 },
  { itable_Subtract_from_Zero_Extended,					UNIT_INT,	1,	0 },
  { itable_Synchronize,							UNIT_INT,	1,	0 },
  { itable_System_Call,							UNIT_MEM,	1,	0 },
  { itable_TLB_Invalidate_All,						UNIT_MEM,	1,	0 },
  { itable_TLB_Invalidate_Entry,					UNIT_MEM,	1,	0 },
  { itable_TLB_Sychronize,						UNIT_MEM,	1,	0 },
  { itable_Trap_Word,							UNIT_INT,	1,	0 },
  { itable_Trap_Word_Immediate,						UNIT_INT,	1,	0 },
  { itable_XOR,								UNIT_INT,	1,	0 },
  { itable_XOR_Immediate,						UNIT_INT,	1,	0 },
  { itable_XOR_Immediate_Shifted,					UNIT_INT,	1,	0 },
  { nr_itable_entries,							UNIT_UNKNOWN,	-1,	0 },
};


INLINE_FUNCTION_UNIT function_unit *
function_unit_create(void)
{
  function_unit *new_func;
  timing *timing_ptr;

  if (!WITH_FUNCTION_UNIT)
    return (function_unit *)0;

  new_func = ZALLOC(function_unit);

  /* Get the model specific timing information */
  switch (CURRENT_PPC_MODEL) {
  default:		timing_ptr = (timing *)0;	break;
  case PPC_MODEL_603:	timing_ptr = time_603;		break;
  case PPC_MODEL_603e:	timing_ptr = time_603;		break;
  case PPC_MODEL_604:	timing_ptr = time_604;		break;
  }

  /* If we have it, provide model specific timing info */
  if (timing_ptr) {
    for (; timing_ptr->cycles >= 0; timing_ptr++) {
      int i = (int)timing_ptr->index;
      unit_index unit = timing_ptr->unit;

      /* The 603 as compared to the 603e doesn't support putting add/cmp instructions
	 in the system register unit, so convert these instructions back to normal
	 integer instructions.	*/
      if (CURRENT_PPC_MODEL == PPC_MODEL_603 && unit == UNIT_SINT)
	unit = UNIT_INT;

      new_func->time[i].unit = unit;
      new_func->time[i].cycles = timing_ptr->cycles;
      new_func->time[i].flags = timing_ptr->flags;
    }
  }

  return new_func;
}

INLINE_FUNCTION_UNIT void
function_unit_init(function_unit *func_unit)
{
  func_unit->old_program_counter = 0;
}

INLINE_FUNCTION_UNIT void
function_unit_halt(cpu *processor,
		   function_unit *func_unit)
{
}

INLINE_FUNCTION_UNIT function_unit_print *
function_unit_mon_info(function_unit *func_unit)
{
  function_unit_print *head;
  function_unit_print *tail;
  int i;

  head = tail = ZALLOC(function_unit_print);
  tail->count = func_unit->nr_stalls;
  tail->name = "stall";
  tail->suffix_plural = "s";
  tail->suffix_singular = "";

  tail->next = ZALLOC(function_unit_print);
  tail = tail->next;
  tail->count = func_unit->nr_branches;
  tail->name = "branch";
  tail->suffix_plural = "es";
  tail->suffix_singular = "";

  for (i = 0; i < (int)NR_UNITS; i++) {
    if (func_unit->nr_units[i]) {
      tail->next = ZALLOC(function_unit_print);
      tail = tail->next;
      tail->count = func_unit->nr_units[i];
      tail->name = unit_names[i];
      tail->suffix_plural = "s";
      tail->suffix_singular = "";
    }
  }

  tail->next = (function_unit_print *)0;
  return head;
}

INLINE_FUNCTION_UNIT void
function_unit_mon_free(function_unit *func_unit,
		       function_unit_print *ptr)
{
  function_unit_print *next;

  while (ptr) {
    next = ptr->next;
    free((void *)ptr);
    ptr = next;
  }
}

INLINE_FUNCTION_UNIT void
function_unit_issue(itable_index index,
		    function_unit *func_unit,
		    unsigned_word cia)
{
  if (func_unit->old_program_counter+4 != cia)
    func_unit->nr_branches++;

  func_unit->old_program_counter = cia;
  func_unit->nr_units[ (int)func_unit->time[ (int)index ].unit ]++;
}

INLINE_FUNCTION_UNIT void
function_unit_model(const char *model)
{
  if (!strcmp (model, "601"))
    current_ppc_model = PPC_MODEL_601;
  else if (!strcmp (model, "602"))
    current_ppc_model = PPC_MODEL_602;
  else if (!strcmp (model, "603"))
    current_ppc_model = PPC_MODEL_603;
  else if (!strcmp (model, "603e"))
    current_ppc_model = PPC_MODEL_603e;
  else if (!strcmp (model, "604"))
    current_ppc_model = PPC_MODEL_604;
  else if (!strcmp (model, "403"))
    current_ppc_model = PPC_MODEL_403;
  else if (!strcmp (model, "505"))
    current_ppc_model = PPC_MODEL_505;
  else if (!strcmp (model, "821"))
    current_ppc_model = PPC_MODEL_821;
  else if (!strcmp (model, "860"))
    current_ppc_model = PPC_MODEL_860;
  else
    error ("Unknown PowerPC model %s", model);
}

#endif /* _FUNCTION_UNIT_C_ */
