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


/*
 * Interface for the Instruction execution routines
 */

/*
 * Macro's that define the parts of an instruction
 */

#define FLOATING_POINT_ENABLED_PROGRAM_INTERRUPT \
     program_interrupt(processor, \
		       cia, \
		       floating_point_enabled_program_interrupt)

#define ILLEGAL_INSN_PROGRAM_INTERRUPT \
     program_interrupt(processor, \
		       cia, \
		       illegal_instruction_program_interrupt)
#define PRIVILEGED_INSN_PROGRAM_INTERRUPT \
     program_interrupt(processor, \
		       cia, \
		       privileged_instruction_program_interrupt)

#define TRAP_PROGRAM_INTERRUPT \
     program_interrupt(processor, \
		       cia, \
		       trap_program_interrupt)

#define FLOATING_POINT_UNAVAILABLE_INTERRUPT \
     floating_point_unavailable_interrupt(processor, \
					  cia)

#define FLOATING_POINT_ASSIST_INTERRUPT \
     floating_point_assist_interrupt(processor, \
				     cia)

#define BREAKPOINT \
     do { \
       ITRACE(trace_breakpoint, \
	      ("breakpoint - cia0x%x\n", \
	       cia)); \
       cpu_halt(processor, cia, was_trap, 0); \
     } while (0)

#define SYSTEM_CALL_INTERRUPT \
     system_call_interrupt(processor, \
			   cia)
