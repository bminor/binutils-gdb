// OBSOLETE /****************************************************************************
// OBSOLETE 
// OBSOLETE 		THIS SOFTWARE IS NOT COPYRIGHTED
// OBSOLETE 
// OBSOLETE    HP offers the following for use in the public domain.  HP makes no
// OBSOLETE    warranty with regard to the software or it's performance and the
// OBSOLETE    user accepts the software "AS IS" with all faults.
// OBSOLETE 
// OBSOLETE    HP DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD
// OBSOLETE    TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OBSOLETE    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// OBSOLETE 
// OBSOLETE ****************************************************************************/
// OBSOLETE 
// OBSOLETE /****************************************************************************
// OBSOLETE  *  Header: remcom.c,v 1.34 91/03/09 12:29:49 glenne Exp $
// OBSOLETE  *
// OBSOLETE  *  Module name: remcom.c $
// OBSOLETE  *  Revision: 1.34 $
// OBSOLETE  *  Date: 91/03/09 12:29:49 $
// OBSOLETE  *  Contributor:     Lake Stevens Instrument Division$
// OBSOLETE  *
// OBSOLETE  *  Description:     low level support for gdb debugger. $
// OBSOLETE  *
// OBSOLETE  *  Considerations:  only works on target hardware $
// OBSOLETE  *
// OBSOLETE  *  Written by:      Glenn Engel $
// OBSOLETE  *  ModuleState:     Experimental $
// OBSOLETE  *
// OBSOLETE  *  NOTES:           See Below $
// OBSOLETE  *
// OBSOLETE  *  Modified for SPARC by Stu Grossman, Cygnus Support.
// OBSOLETE  *  Based on sparc-stub.c, it's modified for SPARClite Debug Unit hardware
// OBSOLETE  *  breakpoint support to create sparclite-stub.c, by Kung Hsu, Cygnus Support.
// OBSOLETE  *
// OBSOLETE  *  This code has been extensively tested on the Fujitsu SPARClite demo board.
// OBSOLETE  *
// OBSOLETE  *  To enable debugger support, two things need to happen.  One, a
// OBSOLETE  *  call to set_debug_traps() is necessary in order to allow any breakpoints
// OBSOLETE  *  or error conditions to be properly intercepted and reported to gdb.
// OBSOLETE  *  Two, a breakpoint needs to be generated to begin communication.  This
// OBSOLETE  *  is most easily accomplished by a call to breakpoint().  Breakpoint()
// OBSOLETE  *  simulates a breakpoint by executing a trap #1.
// OBSOLETE  *
// OBSOLETE  *************
// OBSOLETE  *
// OBSOLETE  *    The following gdb commands are supported:
// OBSOLETE  *
// OBSOLETE  * command          function                               Return value
// OBSOLETE  *
// OBSOLETE  *    g             return the value of the CPU registers  hex data or ENN
// OBSOLETE  *    G             set the value of the CPU registers     OK or ENN
// OBSOLETE  *    P             set the value of a single CPU register OK or ENN
// OBSOLETE  *
// OBSOLETE  *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
// OBSOLETE  *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
// OBSOLETE  *
// OBSOLETE  *    c             Resume at current address              SNN   ( signal NN)
// OBSOLETE  *    cAA..AA       Continue at address AA..AA             SNN
// OBSOLETE  *
// OBSOLETE  *    s             Step one instruction                   SNN
// OBSOLETE  *    sAA..AA       Step one instruction from AA..AA       SNN
// OBSOLETE  *
// OBSOLETE  *    k             kill
// OBSOLETE  *
// OBSOLETE  *    ?             What was the last sigval ?             SNN   (signal NN)
// OBSOLETE  *
// OBSOLETE  * All commands and responses are sent with a packet which includes a
// OBSOLETE  * checksum.  A packet consists of
// OBSOLETE  *
// OBSOLETE  * $<packet info>#<checksum>.
// OBSOLETE  *
// OBSOLETE  * where
// OBSOLETE  * <packet info> :: <characters representing the command or response>
// OBSOLETE  * <checksum>    :: <two hex digits computed as modulo 256 sum of <packetinfo>>
// OBSOLETE  *
// OBSOLETE  * When a packet is received, it is first acknowledged with either '+' or '-'.
// OBSOLETE  * '+' indicates a successful transfer.  '-' indicates a failed transfer.
// OBSOLETE  *
// OBSOLETE  * Example:
// OBSOLETE  *
// OBSOLETE  * Host:                  Reply:
// OBSOLETE  * $m0,10#2a               +$00010203040506070809101112131415#42
// OBSOLETE  *
// OBSOLETE  ****************************************************************************/
// OBSOLETE 
// OBSOLETE #include <string.h>
// OBSOLETE #include <signal.h>
// OBSOLETE 
// OBSOLETE /************************************************************************
// OBSOLETE  *
// OBSOLETE  * external low-level support routines
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE extern void putDebugChar();	/* write a single character      */
// OBSOLETE extern int getDebugChar();	/* read and return a single char */
// OBSOLETE 
// OBSOLETE /************************************************************************/
// OBSOLETE /* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
// OBSOLETE /* at least NUMREGBYTES*2 are needed for register packets */
// OBSOLETE #define BUFMAX 2048
// OBSOLETE 
// OBSOLETE static int initialized = 0;	/* !0 means we've been initialized */
// OBSOLETE static int remote_debug = 0;	/* turn on verbose debugging */
// OBSOLETE 
// OBSOLETE extern void breakinst();
// OBSOLETE void _cprint();
// OBSOLETE static void hw_breakpoint();
// OBSOLETE static void set_mem_fault_trap();
// OBSOLETE static void get_in_break_mode();
// OBSOLETE static unsigned char *mem2hex();
// OBSOLETE 
// OBSOLETE static const char hexchars[]="0123456789abcdef";
// OBSOLETE 
// OBSOLETE #define NUMREGS 121
// OBSOLETE 
// OBSOLETE static unsigned long saved_stack_pointer;
// OBSOLETE 
// OBSOLETE /* Number of bytes of registers.  */
// OBSOLETE #define NUMREGBYTES (NUMREGS * 4)
// OBSOLETE enum regnames { G0, G1, G2, G3, G4, G5, G6, G7,
// OBSOLETE 		O0, O1, O2, O3, O4, O5, SP, O7,
// OBSOLETE 		L0, L1, L2, L3, L4, L5, L6, L7,
// OBSOLETE 		I0, I1, I2, I3, I4, I5, FP, I7,
// OBSOLETE 
// OBSOLETE 		F0, F1, F2, F3, F4, F5, F6, F7,
// OBSOLETE 		F8, F9, F10, F11, F12, F13, F14, F15,
// OBSOLETE 		F16, F17, F18, F19, F20, F21, F22, F23,
// OBSOLETE 		F24, F25, F26, F27, F28, F29, F30, F31,
// OBSOLETE 
// OBSOLETE 		Y, PSR, WIM, TBR, PC, NPC, FPSR, CPSR,
// OBSOLETE 		CCSR, CCPR, CCCRCR, CCOR, CCOBR, CCIBR, CCIR, UNUSED1,
// OBSOLETE 
// OBSOLETE 		ASR1, ASR15, ASR17, ASR18, ASR19, ASR20, ASR21, ASR22, 
// OBSOLETE 		/* the following not actually implemented */
// OBSOLETE 		AWR0,  AWR1,  AWR2,  AWR3,  AWR4,  AWR5,  AWR6,  AWR7,  
// OBSOLETE 		AWR8,  AWR9,  AWR10, AWR11, AWR12, AWR13, AWR14, AWR15,  
// OBSOLETE 		AWR16, AWR17, AWR18, AWR19, AWR20, AWR21, AWR22, AWR23,  
// OBSOLETE 		AWR24, AWR25, AWR26, AWR27, AWR28, AWR29, AWR30, AWR31,  
// OBSOLETE 		APSR
// OBSOLETE };
// OBSOLETE 
// OBSOLETE /***************************  ASSEMBLY CODE MACROS *************************/
// OBSOLETE /* 									   */
// OBSOLETE 
// OBSOLETE extern void trap_low();
// OBSOLETE 
// OBSOLETE asm("
// OBSOLETE 	.reserve trapstack, 1000 * 4, \"bss\", 8
// OBSOLETE 
// OBSOLETE 	.data
// OBSOLETE 	.align	4
// OBSOLETE 
// OBSOLETE in_trap_handler:
// OBSOLETE 	.word	0
// OBSOLETE 
// OBSOLETE 	.text
// OBSOLETE 	.align 4
// OBSOLETE 
// OBSOLETE ! This function is called when any SPARC trap (except window overflow or
// OBSOLETE ! underflow) occurs.  It makes sure that the invalid register window is still
// OBSOLETE ! available before jumping into C code.  It will also restore the world if you
// OBSOLETE ! return from handle_exception.
// OBSOLETE !
// OBSOLETE ! On entry, trap_low expects l1 and l2 to contain pc and npc respectivly.
// OBSOLETE 
// OBSOLETE 	.globl _trap_low
// OBSOLETE _trap_low:
// OBSOLETE 	mov	%psr, %l0
// OBSOLETE 	mov	%wim, %l3
// OBSOLETE 
// OBSOLETE 	srl	%l3, %l0, %l4		! wim >> cwp
// OBSOLETE 	and	%l4, 0xff, %l4		! Mask off windows 28, 29
// OBSOLETE 	cmp	%l4, 1
// OBSOLETE 	bne	window_fine		! Branch if not in the invalid window
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE ! Handle window overflow
// OBSOLETE 
// OBSOLETE 	mov	%g1, %l4		! Save g1, we use it to hold the wim
// OBSOLETE 	srl	%l3, 1, %g1		! Rotate wim right
// OBSOLETE 	and	%g1, 0xff, %g1		! Mask off windows 28, 29
// OBSOLETE 	tst	%g1
// OBSOLETE 	bg	good_wim		! Branch if new wim is non-zero
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE ! At this point, we need to bring a 1 into the high order bit of the wim.
// OBSOLETE ! Since we don't want to make any assumptions about the number of register
// OBSOLETE ! windows, we figure it out dynamically so as to setup the wim correctly.
// OBSOLETE 
// OBSOLETE 	! The normal way doesn't work on the sparclet as register windows
// OBSOLETE 	! 28 and 29 are special purpose windows.
// OBSOLETE 	!not	%g1			! Fill g1 with ones
// OBSOLETE 	!mov	%g1, %wim		! Fill the wim with ones
// OBSOLETE 	!nop
// OBSOLETE 	!nop
// OBSOLETE 	!nop
// OBSOLETE 	!mov	%wim, %g1		! Read back the wim
// OBSOLETE 	!inc	%g1			! Now g1 has 1 just to left of wim
// OBSOLETE 	!srl	%g1, 1, %g1		! Now put 1 at top of wim
// OBSOLETE 
// OBSOLETE 	mov	0x80, %g1		! Hack for sparclet
// OBSOLETE 
// OBSOLETE 	! This doesn't work on the sparclet.
// OBSOLETE 	!mov	%g0, %wim		! Clear wim so that subsequent save
// OBSOLETE 					!  won't trap
// OBSOLETE 	andn	%l3, 0xff, %l5		! Clear wim but not windows 28, 29
// OBSOLETE 	mov	%l5, %wim
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE good_wim:
// OBSOLETE 	save	%g0, %g0, %g0		! Slip into next window
// OBSOLETE 	mov	%g1, %wim		! Install the new wim
// OBSOLETE 
// OBSOLETE 	std	%l0, [%sp + 0 * 4]	! save L & I registers
// OBSOLETE 	std	%l2, [%sp + 2 * 4]
// OBSOLETE 	std	%l4, [%sp + 4 * 4]
// OBSOLETE 	std	%l6, [%sp + 6 * 4]
// OBSOLETE 
// OBSOLETE 	std	%i0, [%sp + 8 * 4]
// OBSOLETE 	std	%i2, [%sp + 10 * 4]
// OBSOLETE 	std	%i4, [%sp + 12 * 4]
// OBSOLETE 	std	%i6, [%sp + 14 * 4]
// OBSOLETE 
// OBSOLETE 	restore				! Go back to trap window.
// OBSOLETE 	mov	%l4, %g1		! Restore %g1
// OBSOLETE 
// OBSOLETE window_fine:
// OBSOLETE 	sethi	%hi(in_trap_handler), %l4
// OBSOLETE 	ld	[%lo(in_trap_handler) + %l4], %l5
// OBSOLETE 	tst	%l5
// OBSOLETE 	bg	recursive_trap
// OBSOLETE 	inc	%l5
// OBSOLETE 
// OBSOLETE 	set	trapstack+1000*4, %sp	! Switch to trap stack
// OBSOLETE 
// OBSOLETE recursive_trap:
// OBSOLETE 	st	%l5, [%lo(in_trap_handler) + %l4]
// OBSOLETE 	sub	%sp,(16+1+6+1+88)*4,%sp ! Make room for input & locals
// OBSOLETE  					! + hidden arg + arg spill
// OBSOLETE 					! + doubleword alignment
// OBSOLETE 					! + registers[121]
// OBSOLETE 
// OBSOLETE 	std	%g0, [%sp + (24 + 0) * 4] ! registers[Gx]
// OBSOLETE 	std	%g2, [%sp + (24 + 2) * 4]
// OBSOLETE 	std	%g4, [%sp + (24 + 4) * 4]
// OBSOLETE 	std	%g6, [%sp + (24 + 6) * 4]
// OBSOLETE 
// OBSOLETE 	std	%i0, [%sp + (24 + 8) * 4] ! registers[Ox]
// OBSOLETE 	std	%i2, [%sp + (24 + 10) * 4]
// OBSOLETE 	std	%i4, [%sp + (24 + 12) * 4]
// OBSOLETE 	std	%i6, [%sp + (24 + 14) * 4]
// OBSOLETE 
// OBSOLETE 	! FP regs (sparclet doesn't have fpu)
// OBSOLETE 
// OBSOLETE 	mov	%y, %l4
// OBSOLETE 	mov	%tbr, %l5
// OBSOLETE 	st	%l4, [%sp + (24 + 64) * 4] ! Y
// OBSOLETE 	st	%l0, [%sp + (24 + 65) * 4] ! PSR
// OBSOLETE 	st	%l3, [%sp + (24 + 66) * 4] ! WIM
// OBSOLETE 	st	%l5, [%sp + (24 + 67) * 4] ! TBR
// OBSOLETE 	st	%l1, [%sp + (24 + 68) * 4] ! PC
// OBSOLETE 	st	%l2, [%sp + (24 + 69) * 4] ! NPC
// OBSOLETE 					! CPSR and FPSR not impl
// OBSOLETE 	or	%l0, 0xf20, %l4
// OBSOLETE 	mov	%l4, %psr		! Turn on traps, disable interrupts
// OBSOLETE 	nop
// OBSOLETE         nop
// OBSOLETE         nop
// OBSOLETE 
// OBSOLETE ! Save coprocessor state.
// OBSOLETE ! See SK/demo/hdlc_demo/ldc_swap_context.S.
// OBSOLETE 
// OBSOLETE 	mov	%psr, %l0
// OBSOLETE 	sethi	%hi(0x2000), %l5		! EC bit in PSR
// OBSOLETE 	or	%l5, %l0, %l5
// OBSOLETE 	mov	%l5, %psr			! enable coprocessor
// OBSOLETE 	nop			! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 	crdcxt	%ccsr, %l1			! capture CCSR
// OBSOLETE 	mov	0x6, %l2
// OBSOLETE 	cwrcxt	%l2, %ccsr	! set CCP state machine for CCFR
// OBSOLETE 	crdcxt	%ccfr, %l2			! capture CCOR
// OBSOLETE 	cwrcxt	%l2, %ccfr			! tickle  CCFR
// OBSOLETE 	crdcxt	%ccfr, %l3			! capture CCOBR
// OBSOLETE 	cwrcxt	%l3, %ccfr			! tickle  CCFR
// OBSOLETE 	crdcxt	%ccfr, %l4			! capture CCIBR
// OBSOLETE 	cwrcxt	%l4, %ccfr			! tickle  CCFR
// OBSOLETE 	crdcxt	%ccfr, %l5			! capture CCIR
// OBSOLETE 	cwrcxt	%l5, %ccfr			! tickle  CCFR
// OBSOLETE 	crdcxt	%ccpr, %l6			! capture CCPR
// OBSOLETE 	crdcxt	%cccrcr, %l7			! capture CCCRCR
// OBSOLETE 	st	%l1, [%sp + (24 + 72) * 4]	! save CCSR
// OBSOLETE 	st	%l2, [%sp + (24 + 75) * 4]	! save CCOR
// OBSOLETE 	st	%l3, [%sp + (24 + 76) * 4]	! save CCOBR
// OBSOLETE 	st	%l4, [%sp + (24 + 77) * 4]	! save CCIBR
// OBSOLETE 	st	%l5, [%sp + (24 + 78) * 4]	! save CCIR
// OBSOLETE 	st	%l6, [%sp + (24 + 73) * 4]	! save CCPR
// OBSOLETE 	st	%l7, [%sp + (24 + 74) * 4]	! save CCCRCR
// OBSOLETE 	mov	%l0, %psr 			! restore original PSR
// OBSOLETE 	nop			! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE ! End of saving coprocessor state.
// OBSOLETE ! Save asr regs
// OBSOLETE 
// OBSOLETE ! Part of this is silly -- we should not display ASR15 or ASR19 at all.
// OBSOLETE 
// OBSOLETE 	sethi	%hi(0x01000000), %l6
// OBSOLETE 	st	%l6, [%sp + (24 + 81) * 4]	! ASR15 == NOP
// OBSOLETE 	sethi	%hi(0xdeadc0de), %l6
// OBSOLETE 	or	%l6, %lo(0xdeadc0de), %l6
// OBSOLETE 	st	%l6, [%sp + (24 + 84) * 4]	! ASR19 == DEADC0DE
// OBSOLETE 
// OBSOLETE 	rd	%asr1, %l4
// OBSOLETE 	st	%l4, [%sp + (24 + 80) * 4]
// OBSOLETE !	rd	%asr15, %l4			! must not read ASR15
// OBSOLETE !	st	%l4, [%sp + (24 + 81) * 4]	! (illegal instr trap)
// OBSOLETE 	rd	%asr17, %l4
// OBSOLETE 	st	%l4, [%sp + (24 + 82) * 4]
// OBSOLETE 	rd	%asr18, %l4
// OBSOLETE 	st	%l4, [%sp + (24 + 83) * 4]
// OBSOLETE !	rd	%asr19, %l4			! must not read asr19
// OBSOLETE !	st	%l4, [%sp + (24 + 84) * 4]	! (halts the CPU)
// OBSOLETE 	rd	%asr20, %l4
// OBSOLETE 	st	%l4, [%sp + (24 + 85) * 4]
// OBSOLETE 	rd	%asr21, %l4
// OBSOLETE 	st	%l4, [%sp + (24 + 86) * 4]
// OBSOLETE 	rd	%asr22, %l4
// OBSOLETE 	st	%l4, [%sp + (24 + 87) * 4]
// OBSOLETE 
// OBSOLETE ! End of saving asr regs
// OBSOLETE 
// OBSOLETE 	call	_handle_exception
// OBSOLETE 	add	%sp, 24 * 4, %o0	! Pass address of registers
// OBSOLETE 
// OBSOLETE ! Reload all of the registers that aren't on the stack
// OBSOLETE 
// OBSOLETE 	ld	[%sp + (24 + 1) * 4], %g1 ! registers[Gx]
// OBSOLETE 	ldd	[%sp + (24 + 2) * 4], %g2
// OBSOLETE 	ldd	[%sp + (24 + 4) * 4], %g4
// OBSOLETE 	ldd	[%sp + (24 + 6) * 4], %g6
// OBSOLETE 
// OBSOLETE 	ldd	[%sp + (24 + 8) * 4], %i0 ! registers[Ox]
// OBSOLETE 	ldd	[%sp + (24 + 10) * 4], %i2
// OBSOLETE 	ldd	[%sp + (24 + 12) * 4], %i4
// OBSOLETE 	ldd	[%sp + (24 + 14) * 4], %i6
// OBSOLETE 
// OBSOLETE 	! FP regs (sparclet doesn't have fpu)
// OBSOLETE 
// OBSOLETE ! Update the coprocessor registers.
// OBSOLETE ! See SK/demo/hdlc_demo/ldc_swap_context.S.
// OBSOLETE 
// OBSOLETE 	mov	%psr, %l0
// OBSOLETE 	sethi	%hi(0x2000), %l5		! EC bit in PSR
// OBSOLETE 	or	%l5, %l0, %l5
// OBSOLETE 	mov	%l5, %psr			! enable coprocessor
// OBSOLETE 	nop			! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE 	mov 0x6, %l2
// OBSOLETE 	cwrcxt	%l2, %ccsr	! set CCP state machine for CCFR
// OBSOLETE 
// OBSOLETE 	ld	[%sp + (24 + 72) * 4], %l1	! saved CCSR
// OBSOLETE 	ld	[%sp + (24 + 75) * 4], %l2	! saved CCOR
// OBSOLETE 	ld	[%sp + (24 + 76) * 4], %l3	! saved CCOBR
// OBSOLETE 	ld	[%sp + (24 + 77) * 4], %l4	! saved CCIBR
// OBSOLETE 	ld	[%sp + (24 + 78) * 4], %l5	! saved CCIR
// OBSOLETE 	ld	[%sp + (24 + 73) * 4], %l6	! saved CCPR
// OBSOLETE 	ld	[%sp + (24 + 74) * 4], %l7	! saved CCCRCR
// OBSOLETE 
// OBSOLETE 	cwrcxt	%l2, %ccfr			! restore CCOR
// OBSOLETE 	cwrcxt	%l3, %ccfr			! restore CCOBR
// OBSOLETE 	cwrcxt	%l4, %ccfr			! restore CCIBR
// OBSOLETE 	cwrcxt	%l5, %ccfr			! restore CCIR
// OBSOLETE 	cwrcxt	%l6, %ccpr			! restore CCPR
// OBSOLETE 	cwrcxt	%l7, %cccrcr			! restore CCCRCR
// OBSOLETE 	cwrcxt	%l1, %ccsr			! restore CCSR
// OBSOLETE 
// OBSOLETE 	mov %l0, %psr				! restore PSR
// OBSOLETE 	nop		! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE ! End of coprocessor handling stuff.
// OBSOLETE ! Update asr regs
// OBSOLETE 
// OBSOLETE 	ld	[%sp + (24 + 80) * 4], %l4
// OBSOLETE 	wr	%l4, %asr1
// OBSOLETE !	ld	[%sp + (24 + 81) * 4], %l4	! can't write asr15
// OBSOLETE !	wr	%l4, %asr15
// OBSOLETE 	ld	[%sp + (24 + 82) * 4], %l4
// OBSOLETE 	wr	%l4, %asr17
// OBSOLETE 	ld	[%sp + (24 + 83) * 4], %l4
// OBSOLETE 	wr	%l4, %asr18
// OBSOLETE !	ld	[%sp + (24 + 84) * 4], %l4	! can't write asr19
// OBSOLETE !	wr	%l4, %asr19
// OBSOLETE !	ld	[%sp + (24 + 85) * 4], %l4	! can't write asr20
// OBSOLETE !	wr	%l4, %asr20
// OBSOLETE !	ld	[%sp + (24 + 86) * 4], %l4	! can't write asr21
// OBSOLETE !	wr	%l4, %asr21
// OBSOLETE 	ld	[%sp + (24 + 87) * 4], %l4
// OBSOLETE 	wr	%l4, %asr22
// OBSOLETE 
// OBSOLETE ! End of restoring asr regs
// OBSOLETE 
// OBSOLETE 
// OBSOLETE 	ldd	[%sp + (24 + 64) * 4], %l0 ! Y & PSR
// OBSOLETE 	ldd	[%sp + (24 + 68) * 4], %l2 ! PC & NPC
// OBSOLETE 
// OBSOLETE 	restore				! Ensure that previous window is valid
// OBSOLETE 	save	%g0, %g0, %g0		!  by causing a window_underflow trap
// OBSOLETE 
// OBSOLETE 	mov	%l0, %y
// OBSOLETE 	mov	%l1, %psr		! Make sure that traps are disabled
// OBSOLETE 					! for rett
// OBSOLETE 	nop	! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE 	sethi	%hi(in_trap_handler), %l4
// OBSOLETE 	ld	[%lo(in_trap_handler) + %l4], %l5
// OBSOLETE 	dec	%l5
// OBSOLETE 	st	%l5, [%lo(in_trap_handler) + %l4]
// OBSOLETE 
// OBSOLETE 	jmpl	%l2, %g0		! Restore old PC
// OBSOLETE 	rett	%l3			! Restore old nPC
// OBSOLETE ");
// OBSOLETE 
// OBSOLETE /* Convert ch from a hex digit to an int */
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE hex (unsigned char ch)
// OBSOLETE {
// OBSOLETE   if (ch >= 'a' && ch <= 'f')
// OBSOLETE     return ch-'a'+10;
// OBSOLETE   if (ch >= '0' && ch <= '9')
// OBSOLETE     return ch-'0';
// OBSOLETE   if (ch >= 'A' && ch <= 'F')
// OBSOLETE     return ch-'A'+10;
// OBSOLETE   return -1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static char remcomInBuffer[BUFMAX];
// OBSOLETE static char remcomOutBuffer[BUFMAX];
// OBSOLETE 
// OBSOLETE /* scan for the sequence $<data>#<checksum>     */
// OBSOLETE 
// OBSOLETE unsigned char *
// OBSOLETE getpacket (void)
// OBSOLETE {
// OBSOLETE   unsigned char *buffer = &remcomInBuffer[0];
// OBSOLETE   unsigned char checksum;
// OBSOLETE   unsigned char xmitcsum;
// OBSOLETE   int count;
// OBSOLETE   char ch;
// OBSOLETE 
// OBSOLETE   while (1)
// OBSOLETE     {
// OBSOLETE       /* wait around for the start character, ignore all other characters */
// OBSOLETE       while ((ch = getDebugChar ()) != '$')
// OBSOLETE 	;
// OBSOLETE 
// OBSOLETE retry:
// OBSOLETE       checksum = 0;
// OBSOLETE       xmitcsum = -1;
// OBSOLETE       count = 0;
// OBSOLETE 
// OBSOLETE       /* now, read until a # or end of buffer is found */
// OBSOLETE       while (count < BUFMAX)
// OBSOLETE 	{
// OBSOLETE 	  ch = getDebugChar ();
// OBSOLETE 	  if (ch == '$')
// OBSOLETE 	    goto retry;
// OBSOLETE 	  if (ch == '#')
// OBSOLETE 	    break;
// OBSOLETE 	  checksum = checksum + ch;
// OBSOLETE 	  buffer[count] = ch;
// OBSOLETE 	  count = count + 1;
// OBSOLETE 	}
// OBSOLETE       buffer[count] = 0;
// OBSOLETE 
// OBSOLETE       if (ch == '#')
// OBSOLETE 	{
// OBSOLETE 	  ch = getDebugChar ();
// OBSOLETE 	  xmitcsum = hex (ch) << 4;
// OBSOLETE 	  ch = getDebugChar ();
// OBSOLETE 	  xmitcsum += hex (ch);
// OBSOLETE 
// OBSOLETE 	  if (checksum != xmitcsum)
// OBSOLETE 	    {
// OBSOLETE 	      putDebugChar ('-');	/* failed checksum */
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    {
// OBSOLETE 	      putDebugChar ('+');	/* successful transfer */
// OBSOLETE 
// OBSOLETE 	      /* if a sequence char is present, reply the sequence ID */
// OBSOLETE 	      if (buffer[2] == ':')
// OBSOLETE 		{
// OBSOLETE 		  putDebugChar (buffer[0]);
// OBSOLETE 		  putDebugChar (buffer[1]);
// OBSOLETE 
// OBSOLETE 		  return &buffer[3];
// OBSOLETE 		}
// OBSOLETE 
// OBSOLETE 	      return &buffer[0];
// OBSOLETE 	    }
// OBSOLETE 	}
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* send the packet in buffer.  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE putpacket (unsigned char *buffer)
// OBSOLETE {
// OBSOLETE   unsigned char checksum;
// OBSOLETE   int count;
// OBSOLETE   unsigned char ch;
// OBSOLETE 
// OBSOLETE   /*  $<packet info>#<checksum>. */
// OBSOLETE   do
// OBSOLETE     {
// OBSOLETE       putDebugChar('$');
// OBSOLETE       checksum = 0;
// OBSOLETE       count = 0;
// OBSOLETE 
// OBSOLETE       while (ch = buffer[count])
// OBSOLETE 	{
// OBSOLETE 	  putDebugChar(ch);
// OBSOLETE 	  checksum += ch;
// OBSOLETE 	  count += 1;
// OBSOLETE 	}
// OBSOLETE 
// OBSOLETE       putDebugChar('#');
// OBSOLETE       putDebugChar(hexchars[checksum >> 4]);
// OBSOLETE       putDebugChar(hexchars[checksum & 0xf]);
// OBSOLETE 
// OBSOLETE     }
// OBSOLETE   while (getDebugChar() != '+');
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Indicate to caller of mem2hex or hex2mem that there has been an
// OBSOLETE    error.  */
// OBSOLETE static volatile int mem_err = 0;
// OBSOLETE 
// OBSOLETE /* Convert the memory pointed to by mem into hex, placing result in buf.
// OBSOLETE  * Return a pointer to the last char put in buf (null), in case of mem fault,
// OBSOLETE  * return 0.
// OBSOLETE  * If MAY_FAULT is non-zero, then we will handle memory faults by returning
// OBSOLETE  * a 0, else treat a fault like any other fault in the stub.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static unsigned char *
// OBSOLETE mem2hex (unsigned char *mem, unsigned char *buf, int count, int may_fault)
// OBSOLETE {
// OBSOLETE   unsigned char ch;
// OBSOLETE 
// OBSOLETE   set_mem_fault_trap(may_fault);
// OBSOLETE 
// OBSOLETE   while (count-- > 0)
// OBSOLETE     {
// OBSOLETE       ch = *mem++;
// OBSOLETE       if (mem_err)
// OBSOLETE 	return 0;
// OBSOLETE       *buf++ = hexchars[ch >> 4];
// OBSOLETE       *buf++ = hexchars[ch & 0xf];
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   *buf = 0;
// OBSOLETE 
// OBSOLETE   set_mem_fault_trap(0);
// OBSOLETE 
// OBSOLETE   return buf;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* convert the hex array pointed to by buf into binary to be placed in mem
// OBSOLETE  * return a pointer to the character AFTER the last byte written */
// OBSOLETE 
// OBSOLETE static char *
// OBSOLETE hex2mem (unsigned char *buf, unsigned char *mem, int count, int may_fault)
// OBSOLETE {
// OBSOLETE   int i;
// OBSOLETE   unsigned char ch;
// OBSOLETE 
// OBSOLETE   set_mem_fault_trap(may_fault);
// OBSOLETE 
// OBSOLETE   for (i=0; i<count; i++)
// OBSOLETE     {
// OBSOLETE       ch = hex(*buf++) << 4;
// OBSOLETE       ch |= hex(*buf++);
// OBSOLETE       *mem++ = ch;
// OBSOLETE       if (mem_err)
// OBSOLETE 	return 0;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   set_mem_fault_trap(0);
// OBSOLETE 
// OBSOLETE   return mem;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* This table contains the mapping between SPARC hardware trap types, and
// OBSOLETE    signals, which are primarily what GDB understands.  It also indicates
// OBSOLETE    which hardware traps we need to commandeer when initializing the stub. */
// OBSOLETE 
// OBSOLETE static struct hard_trap_info
// OBSOLETE {
// OBSOLETE   unsigned char tt;		/* Trap type code for SPARClite */
// OBSOLETE   unsigned char signo;		/* Signal that we map this trap into */
// OBSOLETE } hard_trap_info[] = {
// OBSOLETE   {1, SIGSEGV},			/* instruction access exception */
// OBSOLETE   {0x3b, SIGSEGV},		/* instruction access error */
// OBSOLETE   {2, SIGILL},			/* illegal    instruction */
// OBSOLETE   {3, SIGILL},			/* privileged instruction */
// OBSOLETE   {4, SIGEMT},			/* fp disabled */
// OBSOLETE   {0x24, SIGEMT},		/* cp disabled */
// OBSOLETE   {7, SIGBUS},			/* mem address not aligned */
// OBSOLETE   {0x29, SIGSEGV},		/* data access exception */
// OBSOLETE   {10, SIGEMT},			/* tag overflow */
// OBSOLETE   {128+1, SIGTRAP},		/* ta 1 - normal breakpoint instruction */
// OBSOLETE   {0, 0}			/* Must be last */
// OBSOLETE };
// OBSOLETE 
// OBSOLETE /* Set up exception handlers for tracing and breakpoints */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE set_debug_traps (void)
// OBSOLETE {
// OBSOLETE   struct hard_trap_info *ht;
// OBSOLETE 
// OBSOLETE   for (ht = hard_trap_info; ht->tt && ht->signo; ht++)
// OBSOLETE     exceptionHandler(ht->tt, trap_low);
// OBSOLETE 
// OBSOLETE   initialized = 1;
// OBSOLETE }
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE ! Trap handler for memory errors.  This just sets mem_err to be non-zero.  It
// OBSOLETE ! assumes that %l1 is non-zero.  This should be safe, as it is doubtful that
// OBSOLETE ! 0 would ever contain code that could mem fault.  This routine will skip
// OBSOLETE ! past the faulting instruction after setting mem_err.
// OBSOLETE 
// OBSOLETE 	.text
// OBSOLETE 	.align 4
// OBSOLETE 
// OBSOLETE _fltr_set_mem_err:
// OBSOLETE 	sethi %hi(_mem_err), %l0
// OBSOLETE 	st %l1, [%l0 + %lo(_mem_err)]
// OBSOLETE 	jmpl %l2, %g0
// OBSOLETE 	rett %l2+4
// OBSOLETE ");
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE set_mem_fault_trap (int enable)
// OBSOLETE {
// OBSOLETE   extern void fltr_set_mem_err();
// OBSOLETE   mem_err = 0;
// OBSOLETE 
// OBSOLETE   if (enable)
// OBSOLETE     exceptionHandler(0x29, fltr_set_mem_err);
// OBSOLETE   else
// OBSOLETE     exceptionHandler(0x29, trap_low);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE asm ("
// OBSOLETE 	.text
// OBSOLETE 	.align 4
// OBSOLETE 
// OBSOLETE _dummy_hw_breakpoint:
// OBSOLETE 	jmpl %l2, %g0
// OBSOLETE 	rett %l2+4
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE ");
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE set_hw_breakpoint_trap (int enable)
// OBSOLETE {
// OBSOLETE   extern void dummy_hw_breakpoint();
// OBSOLETE 
// OBSOLETE   if (enable)
// OBSOLETE     exceptionHandler(255, dummy_hw_breakpoint);
// OBSOLETE   else
// OBSOLETE     exceptionHandler(255, trap_low);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE get_in_break_mode (void)
// OBSOLETE {
// OBSOLETE #if 0
// OBSOLETE   int x;
// OBSOLETE   mesg("get_in_break_mode, sp = ");
// OBSOLETE   phex(&x);
// OBSOLETE #endif
// OBSOLETE   set_hw_breakpoint_trap(1);
// OBSOLETE 
// OBSOLETE   asm("
// OBSOLETE         sethi   %hi(0xff10), %l4
// OBSOLETE         or      %l4, %lo(0xff10), %l4
// OBSOLETE 	sta 	%g0, [%l4]0x1	
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE       ");
// OBSOLETE 
// OBSOLETE   set_hw_breakpoint_trap(0);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* Convert the SPARC hardware trap type code to a unix signal number. */
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE computeSignal (int tt)
// OBSOLETE {
// OBSOLETE   struct hard_trap_info *ht;
// OBSOLETE 
// OBSOLETE   for (ht = hard_trap_info; ht->tt && ht->signo; ht++)
// OBSOLETE     if (ht->tt == tt)
// OBSOLETE       return ht->signo;
// OBSOLETE 
// OBSOLETE   return SIGHUP;		/* default for things we don't know about */
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * While we find nice hex chars, build an int.
// OBSOLETE  * Return number of chars processed.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static int
// OBSOLETE hexToInt(char **ptr, int *intValue)
// OBSOLETE {
// OBSOLETE   int numChars = 0;
// OBSOLETE   int hexValue;
// OBSOLETE 
// OBSOLETE   *intValue = 0;
// OBSOLETE 
// OBSOLETE   while (**ptr)
// OBSOLETE     {
// OBSOLETE       hexValue = hex(**ptr);
// OBSOLETE       if (hexValue < 0)
// OBSOLETE 	break;
// OBSOLETE 
// OBSOLETE       *intValue = (*intValue << 4) | hexValue;
// OBSOLETE       numChars ++;
// OBSOLETE 
// OBSOLETE       (*ptr)++;
// OBSOLETE     }
// OBSOLETE 
// OBSOLETE   return (numChars);
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * This function does all command procesing for interfacing to gdb.  It
// OBSOLETE  * returns 1 if you should skip the instruction at the trap address, 0
// OBSOLETE  * otherwise.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE handle_exception (unsigned long *registers)
// OBSOLETE {
// OBSOLETE   int tt;			/* Trap type */
// OBSOLETE   int sigval;
// OBSOLETE   int addr;
// OBSOLETE   int length;
// OBSOLETE   char *ptr;
// OBSOLETE   unsigned long *sp;
// OBSOLETE   unsigned long dsr;
// OBSOLETE 
// OBSOLETE /* First, we must force all of the windows to be spilled out */
// OBSOLETE 
// OBSOLETE   asm("
// OBSOLETE 	! Ugh.  sparclet has broken save
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	!save %sp, -64, %sp
// OBSOLETE 	save
// OBSOLETE 	add %fp,-64,%sp
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE 	restore
// OBSOLETE ");
// OBSOLETE 
// OBSOLETE   if (registers[PC] == (unsigned long)breakinst)
// OBSOLETE     {
// OBSOLETE       registers[PC] = registers[NPC];
// OBSOLETE       registers[NPC] += 4;
// OBSOLETE     }
// OBSOLETE   sp = (unsigned long *)registers[SP];
// OBSOLETE 
// OBSOLETE   tt = (registers[TBR] >> 4) & 0xff;
// OBSOLETE 
// OBSOLETE   /* reply to host that an exception has occurred */
// OBSOLETE   sigval = computeSignal(tt);
// OBSOLETE   ptr = remcomOutBuffer;
// OBSOLETE 
// OBSOLETE   *ptr++ = 'T';
// OBSOLETE   *ptr++ = hexchars[sigval >> 4];
// OBSOLETE   *ptr++ = hexchars[sigval & 0xf];
// OBSOLETE 
// OBSOLETE   *ptr++ = hexchars[PC >> 4];
// OBSOLETE   *ptr++ = hexchars[PC & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((char *)&registers[PC], ptr, 4, 0);
// OBSOLETE   *ptr++ = ';';
// OBSOLETE 
// OBSOLETE   *ptr++ = hexchars[FP >> 4];
// OBSOLETE   *ptr++ = hexchars[FP & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex(sp + 8 + 6, ptr, 4, 0); /* FP */
// OBSOLETE   *ptr++ = ';';
// OBSOLETE 
// OBSOLETE   *ptr++ = hexchars[SP >> 4];
// OBSOLETE   *ptr++ = hexchars[SP & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((char *)&sp, ptr, 4, 0);
// OBSOLETE   *ptr++ = ';';
// OBSOLETE 
// OBSOLETE   *ptr++ = hexchars[NPC >> 4];
// OBSOLETE   *ptr++ = hexchars[NPC & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((char *)&registers[NPC], ptr, 4, 0);
// OBSOLETE   *ptr++ = ';';
// OBSOLETE 
// OBSOLETE   *ptr++ = hexchars[O7 >> 4];
// OBSOLETE   *ptr++ = hexchars[O7 & 0xf];
// OBSOLETE   *ptr++ = ':';
// OBSOLETE   ptr = mem2hex((char *)&registers[O7], ptr, 4, 0);
// OBSOLETE   *ptr++ = ';';
// OBSOLETE 
// OBSOLETE   *ptr++ = 0;
// OBSOLETE 
// OBSOLETE   putpacket(remcomOutBuffer);
// OBSOLETE 
// OBSOLETE   while (1)
// OBSOLETE     {
// OBSOLETE       remcomOutBuffer[0] = 0;
// OBSOLETE 
// OBSOLETE       ptr = getpacket();
// OBSOLETE       switch (*ptr++)
// OBSOLETE 	{
// OBSOLETE 	case '?':
// OBSOLETE 	  remcomOutBuffer[0] = 'S';
// OBSOLETE 	  remcomOutBuffer[1] = hexchars[sigval >> 4];
// OBSOLETE 	  remcomOutBuffer[2] = hexchars[sigval & 0xf];
// OBSOLETE 	  remcomOutBuffer[3] = 0;
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	case 'd':
// OBSOLETE 	  remote_debug = !(remote_debug);	/* toggle debug flag */
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	case 'g':		/* return the value of the CPU registers */
// OBSOLETE 	  {
// OBSOLETE 	    ptr = remcomOutBuffer;
// OBSOLETE 	    ptr = mem2hex((char *)registers, ptr, 16 * 4, 0); /* G & O regs */
// OBSOLETE 	    ptr = mem2hex(sp + 0, ptr, 16 * 4, 0); /* L & I regs */
// OBSOLETE 	    memset(ptr, '0', 32 * 8); /* Floating point */
// OBSOLETE 	    ptr = mem2hex((char *)&registers[Y],
// OBSOLETE 		    ptr + 32 * 4 * 2,
// OBSOLETE 		    8 * 4,
// OBSOLETE 		    0); /* Y, PSR, WIM, TBR, PC, NPC, FPSR, CPSR */
// OBSOLETE 	    ptr = mem2hex((char *)&registers[CCSR],
// OBSOLETE 		    ptr,
// OBSOLETE 		    8 * 4,
// OBSOLETE 		    0); /* CCSR, CCPR, CCCRCR, CCOR, CCOBR, CCIBR, CCIR */
// OBSOLETE 	    ptr = mem2hex((char *)&registers[ASR1],
// OBSOLETE 		    ptr,
// OBSOLETE 		    8 * 4,
// OBSOLETE 		    0); /* ASR1,ASR15,ASR17,ASR18,ASR19,ASR20,ASR21,ASR22 */
// OBSOLETE #if 0 /* not implemented */
// OBSOLETE 	    ptr = mem2hex((char *) &registers[AWR0], 
// OBSOLETE 		    ptr, 
// OBSOLETE 		    32 * 4, 
// OBSOLETE 		    0); /* Alternate Window Registers */
// OBSOLETE #endif
// OBSOLETE 	  }
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	case 'G':	/* set value of all the CPU registers - return OK */
// OBSOLETE 	case 'P':	/* set value of one CPU register      - return OK */
// OBSOLETE 	  {
// OBSOLETE 	    unsigned long *newsp, psr;
// OBSOLETE 
// OBSOLETE 	    psr = registers[PSR];
// OBSOLETE 
// OBSOLETE 	    if (ptr[-1] == 'P')	/* do a single register */
// OBSOLETE 	      {
// OBSOLETE 		int regno;
// OBSOLETE  
// OBSOLETE                 if (hexToInt (&ptr, &regno)
// OBSOLETE                     && *ptr++ == '=')
// OBSOLETE                   if (regno >= L0 && regno <= I7)
// OBSOLETE                     hex2mem (ptr, sp + regno - L0, 4, 0);
// OBSOLETE                   else
// OBSOLETE                     hex2mem (ptr, (char *)&registers[regno], 4, 0);
// OBSOLETE                 else
// OBSOLETE                   {
// OBSOLETE                     strcpy (remcomOutBuffer, "E01");
// OBSOLETE                     break;
// OBSOLETE                   }
// OBSOLETE 	      }
// OBSOLETE 	    else
// OBSOLETE 	      {
// OBSOLETE 		hex2mem(ptr, (char *)registers, 16 * 4, 0); /* G & O regs */
// OBSOLETE 		hex2mem(ptr + 16 * 4 * 2, sp + 0, 16 * 4, 0); /* L & I regs */
// OBSOLETE 		hex2mem(ptr + 64 * 4 * 2, (char *)&registers[Y],
// OBSOLETE 			8 * 4, 0); /* Y,PSR,WIM,TBR,PC,NPC,FPSR,CPSR */
// OBSOLETE 		hex2mem(ptr + 72 * 4 * 2, (char *)&registers[CCSR],
// OBSOLETE 			8 * 4, 0); /* CCSR,CCPR,CCCRCR,CCOR,CCOBR,CCIBR,CCIR */
// OBSOLETE 		hex2mem(ptr + 80 * 4 * 2, (char *)&registers[ASR1],
// OBSOLETE 			8 * 4, 0); /* ASR1 ... ASR22 */
// OBSOLETE #if 0 /* not implemented */
// OBSOLETE 		hex2mem(ptr + 88 * 4 * 2, (char *)&registers[AWR0],
// OBSOLETE 			8 * 4, 0); /* Alternate Window Registers */
// OBSOLETE #endif
// OBSOLETE 	      }
// OBSOLETE 	    /* See if the stack pointer has moved.  If so, then copy the saved
// OBSOLETE 	       locals and ins to the new location.  This keeps the window
// OBSOLETE 	       overflow and underflow routines happy.  */
// OBSOLETE 
// OBSOLETE 	    newsp = (unsigned long *)registers[SP];
// OBSOLETE 	    if (sp != newsp)
// OBSOLETE 	      sp = memcpy(newsp, sp, 16 * 4);
// OBSOLETE 
// OBSOLETE 	    /* Don't allow CWP to be modified. */
// OBSOLETE 
// OBSOLETE 	    if (psr != registers[PSR])
// OBSOLETE 	      registers[PSR] = (psr & 0x1f) | (registers[PSR] & ~0x1f);
// OBSOLETE 
// OBSOLETE 	    strcpy(remcomOutBuffer,"OK");
// OBSOLETE 	  }
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	case 'm':	  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
// OBSOLETE 	  /* Try to read %x,%x.  */
// OBSOLETE 
// OBSOLETE 	  if (hexToInt(&ptr, &addr)
// OBSOLETE 	      && *ptr++ == ','
// OBSOLETE 	      && hexToInt(&ptr, &length))
// OBSOLETE 	    {
// OBSOLETE 	      if (mem2hex((char *)addr, remcomOutBuffer, length, 1))
// OBSOLETE 		break;
// OBSOLETE 
// OBSOLETE 	      strcpy (remcomOutBuffer, "E03");
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    strcpy(remcomOutBuffer,"E01");
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
// OBSOLETE 	  /* Try to read '%x,%x:'.  */
// OBSOLETE 
// OBSOLETE 	  if (hexToInt(&ptr, &addr)
// OBSOLETE 	      && *ptr++ == ','
// OBSOLETE 	      && hexToInt(&ptr, &length)
// OBSOLETE 	      && *ptr++ == ':')
// OBSOLETE 	    {
// OBSOLETE 	      if (hex2mem(ptr, (char *)addr, length, 1))
// OBSOLETE 		strcpy(remcomOutBuffer, "OK");
// OBSOLETE 	      else
// OBSOLETE 		strcpy(remcomOutBuffer, "E03");
// OBSOLETE 	    }
// OBSOLETE 	  else
// OBSOLETE 	    strcpy(remcomOutBuffer, "E02");
// OBSOLETE 	  break;
// OBSOLETE 
// OBSOLETE 	case 'c':    /* cAA..AA    Continue at address AA..AA(optional) */
// OBSOLETE 	  /* try to read optional parameter, pc unchanged if no parm */
// OBSOLETE 
// OBSOLETE 	  if (hexToInt(&ptr, &addr))
// OBSOLETE 	    {
// OBSOLETE 	      registers[PC] = addr;
// OBSOLETE 	      registers[NPC] = addr + 4;
// OBSOLETE 	    }
// OBSOLETE 
// OBSOLETE /* Need to flush the instruction cache here, as we may have deposited a
// OBSOLETE    breakpoint, and the icache probably has no way of knowing that a data ref to
// OBSOLETE    some location may have changed something that is in the instruction cache.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE 	  flush_i_cache();
// OBSOLETE 	  return;
// OBSOLETE 
// OBSOLETE 	  /* kill the program */
// OBSOLETE 	case 'k' :		/* do nothing */
// OBSOLETE 	  break;
// OBSOLETE #if 0
// OBSOLETE 	case 't':		/* Test feature */
// OBSOLETE 	  asm (" std %f30,[%sp]");
// OBSOLETE 	  break;
// OBSOLETE #endif
// OBSOLETE 	case 'r':		/* Reset */
// OBSOLETE 	  asm ("call 0
// OBSOLETE 		nop ");
// OBSOLETE 	  break;
// OBSOLETE 	}			/* switch */
// OBSOLETE 
// OBSOLETE       /* reply to the request */
// OBSOLETE       putpacket(remcomOutBuffer);
// OBSOLETE     }
// OBSOLETE }
// OBSOLETE 
// OBSOLETE /* This function will generate a breakpoint exception.  It is used at the
// OBSOLETE    beginning of a program to sync up with a debugger and can be used
// OBSOLETE    otherwise as a quick means to stop program execution and "break" into
// OBSOLETE    the debugger. */
// OBSOLETE 
// OBSOLETE void
// OBSOLETE breakpoint (void)
// OBSOLETE {
// OBSOLETE   if (!initialized)
// OBSOLETE     return;
// OBSOLETE 
// OBSOLETE   asm("	.globl _breakinst
// OBSOLETE 
// OBSOLETE 	_breakinst: ta 1
// OBSOLETE       ");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE static void
// OBSOLETE hw_breakpoint (void)
// OBSOLETE {
// OBSOLETE   asm("
// OBSOLETE       ta 127
// OBSOLETE       ");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #if 0 /* experimental and never finished, left here for reference */
// OBSOLETE static void
// OBSOLETE splet_temp(void)
// OBSOLETE {
// OBSOLETE   asm("	sub	%sp,(16+1+6+1+121)*4,%sp ! Make room for input & locals
// OBSOLETE  					! + hidden arg + arg spill
// OBSOLETE 					! + doubleword alignment
// OBSOLETE 					! + registers[121]
// OBSOLETE 
// OBSOLETE ! Leave a trail of breadcrumbs! (save register save area for debugging)
// OBSOLETE 	mov	%sp, %l0
// OBSOLETE 	add	%l0, 24*4, %l0
// OBSOLETE 	sethi	%hi(_debug_registers), %l1
// OBSOLETE 	st	%l0, [%lo(_debug_registers) + %l1]
// OBSOLETE 
// OBSOLETE ! Save the Alternate Register Set: (not implemented yet)
// OBSOLETE !    To save the Alternate Register set, we must:
// OBSOLETE !    1) Save the current SP in some global location.
// OBSOLETE !    2) Swap the register sets.
// OBSOLETE !    3) Save the Alternate SP in the Y register
// OBSOLETE !    4) Fetch the SP that we saved in step 1.
// OBSOLETE !    5) Use that to save the rest of the regs (not forgetting ASP in Y)
// OBSOLETE !    6) Restore the Alternate SP from Y
// OBSOLETE !    7) Swap the registers back.
// OBSOLETE 
// OBSOLETE ! 1) Copy the current stack pointer to global _SAVED_STACK_POINTER:
// OBSOLETE 	sethi	%hi(_saved_stack_pointer), %l0
// OBSOLETE 	st	%sp, [%lo(_saved_stack_pointer) + %l0]
// OBSOLETE 
// OBSOLETE ! 2) Swap the register sets:
// OBSOLETE 	mov	%psr, %l1
// OBSOLETE 	sethi	%hi(0x10000), %l2
// OBSOLETE 	xor	%l1, %l2, %l1
// OBSOLETE 	mov	%l1, %psr
// OBSOLETE 	nop			! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE 
// OBSOLETE ! 3) Save Alternate L0 in Y
// OBSOLETE 	wr	%l0, 0, %y
// OBSOLETE 
// OBSOLETE ! 4) Load former SP into alternate SP, using L0
// OBSOLETE 	sethi	%hi(_saved_stack_pointer), %l0
// OBSOLETE 	or	%lo(_saved_stack_pointer), %l0, %l0
// OBSOLETE 	swap	[%l0], %sp
// OBSOLETE 
// OBSOLETE ! 4.5) Restore alternate L0
// OBSOLETE 	rd	%y, %l0
// OBSOLETE 
// OBSOLETE ! 5) Save the Alternate Window Registers
// OBSOLETE 	st	%r0, [%sp + (24 + 88) * 4]	! AWR0
// OBSOLETE 	st	%r1, [%sp + (24 + 89) * 4]	! AWR1
// OBSOLETE 	st	%r2, [%sp + (24 + 90) * 4]	! AWR2
// OBSOLETE 	st	%r3, [%sp + (24 + 91) * 4]	! AWR3
// OBSOLETE 	st	%r4, [%sp + (24 + 92) * 4]	! AWR4
// OBSOLETE 	st	%r5, [%sp + (24 + 93) * 4]	! AWR5
// OBSOLETE 	st	%r6, [%sp + (24 + 94) * 4]	! AWR6
// OBSOLETE 	st	%r7, [%sp + (24 + 95) * 4]	! AWR7
// OBSOLETE 	st	%r8, [%sp + (24 + 96) * 4]	! AWR8
// OBSOLETE 	st	%r9, [%sp + (24 + 97) * 4]	! AWR9
// OBSOLETE 	st	%r10, [%sp + (24 + 98) * 4]	! AWR10
// OBSOLETE 	st	%r11, [%sp + (24 + 99) * 4]	! AWR11
// OBSOLETE 	st	%r12, [%sp + (24 + 100) * 4]	! AWR12
// OBSOLETE 	st	%r13, [%sp + (24 + 101) * 4]	! AWR13
// OBSOLETE !	st	%r14, [%sp + (24 + 102) * 4]	! AWR14	(SP)
// OBSOLETE 	st	%r15, [%sp + (24 + 103) * 4]	! AWR15
// OBSOLETE 	st	%r16, [%sp + (24 + 104) * 4]	! AWR16
// OBSOLETE 	st	%r17, [%sp + (24 + 105) * 4]	! AWR17
// OBSOLETE 	st	%r18, [%sp + (24 + 106) * 4]	! AWR18
// OBSOLETE 	st	%r19, [%sp + (24 + 107) * 4]	! AWR19
// OBSOLETE 	st	%r20, [%sp + (24 + 108) * 4]	! AWR20
// OBSOLETE 	st	%r21, [%sp + (24 + 109) * 4]	! AWR21
// OBSOLETE 	st	%r22, [%sp + (24 + 110) * 4]	! AWR22
// OBSOLETE 	st	%r23, [%sp + (24 + 111) * 4]	! AWR23
// OBSOLETE 	st	%r24, [%sp + (24 + 112) * 4]	! AWR24
// OBSOLETE 	st	%r25, [%sp + (24 + 113) * 4]	! AWR25
// OBSOLETE 	st	%r26, [%sp + (24 + 114) * 4]	! AWR26
// OBSOLETE 	st	%r27, [%sp + (24 + 115) * 4]	! AWR27
// OBSOLETE 	st	%r28, [%sp + (24 + 116) * 4]	! AWR28
// OBSOLETE 	st	%r29, [%sp + (24 + 117) * 4]	! AWR29
// OBSOLETE 	st	%r30, [%sp + (24 + 118) * 4]	! AWR30
// OBSOLETE 	st	%r31, [%sp + (24 + 119) * 4]	! AWR21
// OBSOLETE 
// OBSOLETE ! Get the Alternate PSR (I hope...)
// OBSOLETE 
// OBSOLETE 	rd	%psr, %l2
// OBSOLETE 	st	%l2, [%sp + (24 + 120) * 4]	! APSR
// OBSOLETE 
// OBSOLETE ! Don't forget the alternate stack pointer
// OBSOLETE 
// OBSOLETE 	rd	%y, %l3
// OBSOLETE 	st	%l3, [%sp + (24 + 102) * 4]	! AWR14 (SP)
// OBSOLETE 
// OBSOLETE ! 6) Restore the Alternate SP (saved in Y)
// OBSOLETE 
// OBSOLETE 	rd	%y, %o6
// OBSOLETE 
// OBSOLETE 
// OBSOLETE ! 7) Swap the registers back:
// OBSOLETE 
// OBSOLETE 	mov	%psr, %l1
// OBSOLETE 	sethi	%hi(0x10000), %l2
// OBSOLETE 	xor	%l1, %l2, %l1
// OBSOLETE 	mov	%l1, %psr
// OBSOLETE 	nop			! 3 nops after write to %psr (needed?)
// OBSOLETE 	nop
// OBSOLETE 	nop
// OBSOLETE ");
// OBSOLETE }
// OBSOLETE 
// OBSOLETE #endif
