/*
 * srt0.s for ERC32. This file is part of ERC32SIM.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.
 *
 */
 
/* The traptable has to be the first code in a boot PROM. */

/* Entry for traps which jump to a programmer-specified trap handler.  */
#define TRAP(H)  mov %psr, %l0; sethi %hi(H), %l4; jmp %l4+%lo(H); nop;

/* Unexcpected trap will halt the processor by forcing it to error state */
#define BAD_TRAP ta 0; nop; nop; nop;

/* Software trap. Treat as BAD_TRAP for the time being... */
#define SOFT_TRAP BAD_TRAP

#define PSR_INIT   0x10c0	/* Disable traps, set s and ps */
#define WIM_INIT   2
#define SP_INIT	   0x02100000

WINDOWSIZE = (16 * 4)
ARGPUSHSIZE = (6 * 4)
ARGPUSH = (WINDOWSIZE + 4)
MINFRAME = (WINDOWSIZE + ARGPUSHSIZE + 4)
#define STACK_ALIGN	8
#define SA(X)	(((X)+(STACK_ALIGN-1)) & ~(STACK_ALIGN-1))

  .seg    "text"
  .global _trap_table, _mecini, start

  /* Hardware traps */
_trap_table:
  TRAP(_mecini);			! 00 reset trap 
  BAD_TRAP;			! 01 instruction_access_exception
  BAD_TRAP;			! 02 illegal_instruction
  BAD_TRAP;			! 03 priveleged_instruction
  BAD_TRAP;			! 04 fp_disabled
  TRAP(_window_overflow);	! 05 window_overflow
  TRAP(_window_underflow);	! 06 window_underflow
  BAD_TRAP;			! 07 memory_address_not_aligned
  BAD_TRAP;			! 08 fp_exception
  BAD_TRAP;			! 09 data_access_exception
  BAD_TRAP;			! 0A tag_overflow

  /* Trap levels from 0B to 0x10 are not defined (used for MEC init) */

_mecini:
  sethi		%hi(0x01f80000), %g1	! 0B
  sethi		%hi(0x001C1000), %g2
  or 		%g1,%lo(0x001C1000),%g1
  st		%g2, [%g1 + 0x10]

  st		%g0, [%g1 + 0x18]	! 0C
  nop
  nop
  nop

  TRAP(_hardreset);			! 0D

  BAD_TRAP;			! 0E undefined
  BAD_TRAP;			! 0F undefined
  BAD_TRAP;			! 10 undefined

  /* Interrupt entries */
  BAD_TRAP;			! 11 interrupt level 1
  BAD_TRAP;			! 12 interrupt level 2
  BAD_TRAP;			! 13 interrupt level 3
  BAD_TRAP;			! 14 interrupt level 4
  BAD_TRAP;			! 15 interrupt level 5
  BAD_TRAP;			! 16 interrupt level 6
  BAD_TRAP;			! 17 interrupt level 7
  BAD_TRAP;			! 18 interrupt level 8
  BAD_TRAP;			! 19 interrupt level 9
  BAD_TRAP;			! 1A interrupt level 1
  BAD_TRAP;			! 1B interrupt level 11
  BAD_TRAP;			! 1C interrupt level 12
  BAD_TRAP;			! 1D interrupt level 13
  BAD_TRAP;			! 1E interrupt level 14
  BAD_TRAP;			! 1F interrupt level 15
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 20 - 23 undefined
  BAD_TRAP;					! 24 cp_disabled
	    BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 25 - 27 undefined
  BAD_TRAP;					! 28 cp_exception
	    BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 29 - 2B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 2C - 2F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 30 - 33 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 34 - 37 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 38 - 3B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 3C - 3F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 40 - 43 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 44 - 47 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 48 - 4B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 4C - 4F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 50 - 53 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 54 - 57 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 58 - 5B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 5C - 5F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 60 - 63 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 64 - 67 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 68 - 6B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 6C - 6F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 70 - 73 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 74 - 77 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 78 - 7B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;	! 7C - 7F undefined

  /* Software traps */
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; 		! 80 - 82
  TRAP(_flush_windows)				! 83
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 84 - 87
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 88 - 8B
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 8C - 8F
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 90 - 93
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 94 - 97
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 98 - 9B
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! 9C - 9F
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! A0 - A3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! A4 - A7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! A8 - AB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! AC - AF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! B0 - B3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! B4 - B7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! B8 - BB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! BC - BF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! C0 - C3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! C4 - C7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! C8 - CB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! CC - CF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! D0 - D3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! D4 - D7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! D8 - DB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! DC - DF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! E0 - E3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! E4 - E7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! E8 - EB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! EC - EF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! F0 - F3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! F4 - F7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! F8 - FB 
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;	! FC - FF


!
! Startup code for standalone system. Wash IU and FPU (if present) registers.
! The registers have to be written to initiate the parity bits.
!

_hardreset:

        sethi %hi(0x01FE0),%o0
        or %o0,%lo(0x01FE0),%o0
	wr	%o0, %psr		! Set valid PSR
	nop

	wr	%g0, %wim 		! Set window invalid mask register
	wr	%g0, %y 		! Init Y-register
	nop
	set	_hardreset, %g1

	wr	%g1, %tbr 		! Set TBR
	sethi %hi(SP_INIT),%sp
	or	%g0, 1, %o0
	ld	[%g0], %f0		! Check if FPU is present

	tst	%o0
	bz	fixiu
	nop
	ba      fixfpu

! FPU disabled trap address

	clr	%i0
	jmpl	%l2, %g0
	rett	%l2 + 4
	nop
	

! Wash register files (fix for 90C601E & 90C602E)

fixfpu:

	ld	[%g0], %f0
	ld	[%g0], %f1
	ld	[%g0], %f2
	ld	[%g0], %f3
	ld	[%g0], %f4
	ld	[%g0], %f5
	ld	[%g0], %f6
	ld	[%g0], %f7
	ld	[%g0], %f8
	ld	[%g0], %f9
	ld	[%g0], %f10
	ld	[%g0], %f11
	ld	[%g0], %f12
	ld	[%g0], %f13
	ld	[%g0], %f14
	ld	[%g0], %f15
	ld	[%g0], %f16
	ld	[%g0], %f17
	ld	[%g0], %f18
	ld	[%g0], %f19
	ld	[%g0], %f20
	ld	[%g0], %f21
	ld	[%g0], %f22
	ld	[%g0], %f23
	ld	[%g0], %f24
	ld	[%g0], %f25
	ld	[%g0], %f26
	ld	[%g0], %f27
	ld	[%g0], %f28
	ld	[%g0], %f29
	ld	[%g0], %f30
	ld	[%g0], %f31

fixiu:
	clr	%g1
	clr	%g2
	clr	%g3
	clr	%g4
	clr	%g5
	clr	%g6
	clr	%g7
	set	8,%g1
wl0:
	clr	%i0
	clr	%i1
	clr	%i2
	clr	%i3
	clr	%i4
	clr	%i5
	clr	%i6
	clr	%i7
	clr	%l0
	clr	%l1
	clr	%l2
	clr	%l3
	clr	%l4
	clr	%l5
	clr	%l6
	clr	%l7
	save
	subcc	%g1, 1, %g1
	bne	wl0
	nop

!
! Start the real-time clock with a tick of 150 clocks
!

rtc:

	set 	0x1f80000, %l0		! MEC register base
	set 	149, %l1
	st	%l1, [%l0 + 0x84]	! RTC scaler = 149
	set	0x0d00, %l1
	st	%l1, [%l0 + 0x98]	! Start RTC

	st	%g0, [%l0 + 0x64]	! Disable watchdog for now
	ld	[%l0], %g1
	or	%g1, 1, %g1
	st	%g1, [%l0]		! Enable power-down mode

! Initialise some registers

_init:
        set     PSR_INIT, %g1		! Initialize psr
        wr      %g1, %psr
        set     WIM_INIT, %g1       	! Initialize WIM
        wr      %g1, %wim               
        set     _trap_table, %g1       	! Initialize TBR
        wr      %g1, %tbr
        nop;nop;nop                     

        set     PSR_INIT, %g1
        wr      %g1, 0x20, %psr		! enable traps
        nop; nop; nop;
        set     0x02100000, %sp         ! Set stack pointer
        mov     %sp, %fp
        nop

start:
        /* clear the bss */
 
        sethi %hi(_edata),%g2
        or    %g2,%lo(_edata),%g2  ! g2 = start of bss
        sethi %hi(_end),%g3
        or    %g3,%lo(_end),%g3         ! g3 = end of bss
        mov   %g0,%g1                   ! so std has two zeros
zerobss:
        std    %g0,[%g2]
        add    %g2,8,%g2
        cmp    %g2,%g3
        bleu,a zerobss
        nop

        /* move data segment to proper location */
 
relocd:
        set (_etext),%g2 		! g2 = start of data in aout file
        set (_environ),%g4		! g4 = start of where data should go
        set (_edata),%g3 		! g3 = end of where data should go
	subcc	%g3, %g4, %g5		! g5 = length of data

	subcc	%g4, %g2, %g0		! need to relocate data ?
	ble	initok
mvdata:
	subcc	%g5, 8, %g5
	ldd	[%g2 + %g5], %g6
	bg	mvdata
        std    	%g6, [%g4 + %g5]

initok:

        call    _main
        sub     %sp, 0x40, %sp  	! room for main to save args
        nop

	ta 	0			! Halt if _main would return ...
	nop



/* Number of register windows */
#define NWINDOWS 	8

  	 		!Window overflow trap handler.
  	.global _window_overflow

_window_overflow:

  	mov  %wim, %l3       		! Calculate next WIM
  	mov  %g1, %l7           
  	srl  %l3, 1, %g1        
  	sll  %l3, NWINDOWS-1 , %l4
  	or   %l4, %g1, %g1

  	save                              ! Get into window to be saved.
  	mov  %g1, %wim
  	nop; nop; nop
        st   %l0, [%sp + 0];
        st   %l1, [%sp + 4];
        st   %l2, [%sp + 8];
        st   %l3, [%sp + 12];
        st   %l4, [%sp + 16]; 
        st   %l5, [%sp + 20];
        st   %l6, [%sp + 24]; 
        st   %l7, [%sp + 28]; 
        st   %i0, [%sp + 32]; 
        st   %i1, [%sp + 36]; 
        st   %i2, [%sp + 40]; 
        st   %i3, [%sp + 44]; 
        st   %i4, [%sp + 48]; 
        st   %i5, [%sp + 52]; 
        st   %i6, [%sp + 56]; 
        st   %i7, [%sp + 60];
  	restore				! Go back to trap window.
  	mov  %l7, %g1
  	jmp  %l1			! Re-execute save.
  	rett %l2

  /* Window underflow trap handler.  */

  	.global  _window_underflow

_window_underflow:

	mov  %wim, %l3			! Calculate next WIM
	sll  %l3, 1, %l4
  	srl  %l3, NWINDOWS-1, %l5
  	or   %l5, %l4, %l5
  	mov  %l5, %wim
  	nop; nop; nop
  	restore				! Two restores to get into the
  	restore				! window to restore
        ld   [%sp + 0], %l0; 		! Restore window from the stack
        ld   [%sp + 4], %l1; 
        ld   [%sp + 8], %l2; 
        ld   [%sp + 12], %l3; 
        ld   [%sp + 16], %l4; 
        ld   [%sp + 20], %l5; 
        ld   [%sp + 24], %l6; 
        ld   [%sp + 28], %l7; 
        ld   [%sp + 32], %i0; 
        ld   [%sp + 36], %i1; 
        ld   [%sp + 40], %i2; 
        ld   [%sp + 44], %i3; 
        ld   [%sp + 48], %i4; 
        ld   [%sp + 52], %i5; 
        ld   [%sp + 56], %i6; 
        ld   [%sp + 60], %i7;
  	save				! Get back to the trap window.
  	save
  	jmp  %l1			! Re-execute restore.
  	rett  %l2

	.global	_flush_windows	
_flush_windows:		

	mov	%psr, %g1
	or	%g1, 0x0f00, %g2
	restore				! enter previous frame (cannot trap)
        wr      %g2, 0x20, %psr		! enable traps, disable interrupts
	nop; nop; nop
	save				! 6 save to flush all windows
	save
	save
	save
	save
	save
	restore				! 5 restore to enter trapped frame
	restore
	restore
	restore
	restore
	wr      %g1, %psr		! restore %psr
	nop; nop; nop
  	jmp  %l2			! Jump to nPC
  	rett  %l2 + 4




        .seg    "data"
        .global .bdata
.bdata:
        .align  8
        .global _environ                ! first symbol in sdata
_environ:
        .word   0
        .global _errno                  ! not defined elsewhere ..?
_errno:
        .word   0
 


