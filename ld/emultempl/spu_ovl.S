/* Overlay manager for SPU.

   Copyright 2006, 2007 Free Software Foundation, Inc.

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/**
 * MFC DMA defn's.
 */
#define MFC_GET_CMD		0x40
#define MFC_MAX_DMA_SIZE	0x4000
#define MFC_TAG_UPDATE_ALL	2
#define MFC_TAG_ID		0


/**
 * Temporary register allocations.
 * These are saved/restored here.
 */
#define tab		$75
#define cgbits		$75
#define add64		$75
#define ealo		$75
#define newmask		$75
#define tagstat		$75
#define bchn		$75
#define rv1		$75

#define off		$76
#define off64		$76
#define maxsize		$76
#define oldmask		$76
#define sz		$76
#define lnkr		$76
#define rv2		$76

#define cur		$77
#define cmp		$77
#define buf		$77
#define genwi		$77
#define tagid		$77
#define cmd		$77
#define rv3		$77

#define cgshuf		$78

#define vma		$6

#define map		$7
#define osize		$7
#define cmp2		$7

#define ea64		$8
#define retval		$8

#ifdef OVLY_IRQ_SAVE
#define irqtmp		$8
#define irq_stat	$9
#endif

# Stack quadword minux N
#define	SQWM1	-16*1
#define	SQWM2	-16*2
#define	SQWM3	-16*3
#define	SQWM4	-16*4
#define	SQWM5	-16*5
#define	SQWM6	-16*6
#define	SQWM7	-16*7
#define	SQWM8	-16*8
#define	SQWM9	-16*9
#define	SQWM10	-16*10
#define	SQWM11	-16*11
#define	SQWM12	-16*12
#define	SQWM13	-16*13
#define	SQWM14	-16*14
#define	SQWM15	-16*15
#define	SQWM16	-16*16

	.extern		_ovly_table
	.extern		_ovly_buf_table

#ifdef OVLY_PRINTFS
#define SPE_C99_VPRINTF 37
__entry_event_format:
	.string		"In entry_event_hook segment=0x%08x entry-address=0x%08x\n"
__debug_event_format:
  	.string		"In debug_event_hook link-register=0x%08x %08x %08x %08x\n"
__dma_event_format:
  	.string		"In dma_event_hook vma=0x%08x ea=%08x%08x sz=%08x\n"
__ovly_buf_table_format:
  	.string		"_ovly_buf_table[%08x]=%08x\n"
#endif

	.text
	.align 		4
	.type		__rv_pattern, @object
	.size		__rv_pattern, 16
__rv_pattern:
	.word		0x00010203, 0x1c1d1e1f, 0x00010203, 0x10111213
	.type		__cg_pattern, @object
	.size		__cg_pattern, 16
__cg_pattern:
	.word		0x04050607, 0x80808080, 0x80808080, 0x80808080

/**
 * __ovly_return - stub for returning from overlay functions.
 *
 * inputs:
 *	$lr	link register
 *
 * outputs:
 *	$78	old partition number, to be reloaded
 *	$79	return address in old partion number
 */
	.global		__ovly_return
	.type		__ovly_return, @function

	.word		0
__ovly_return:
	shlqbyi		$78, $lr, 4
	shlqbyi		$79, $lr, 8
	biz		$78, $79
	.size		__ovly_return, . - __ovly_return

/**
 * __ovly_load - copy an overlay partion to local store.
 *
 * inputs:
 *	$78	partition number to be loaded.
 *	$79	branch target in new partition.
 *	$lr	link register, containing return addr.
 *
 * outputs:
 *	$lr	new link register, returning through __ovly_return.
 *
 * Copy a new overlay partition into local store, or return
 * immediately if the partition is already resident.
 */
	.global		__ovly_load
	.type		__ovly_load, @function

__ovly_load:
/* Save temporary registers to stack. */
	stqd		$6, -16($sp)
	stqd		$7, -32($sp)
	stqd		$8, -48($sp)

#ifdef OVLY_IRQ_SAVE
/* Save irq state, then disable interrupts. */
	stqd		$9, -64($sp)
	ila		irqtmp, __ovly_irq_save
	rdch		irq_stat, $SPU_RdMachStat
	bid		irqtmp
__ovly_irq_save:
#endif

#ifdef OVLY_PRINTFS
//==============================================
// In entry_event_hook segment=0x%08x entry-address=0x%08x
//==============================================
# save registers
	stqd	$10, SQWM5($sp)
	stqd	$11, SQWM6($sp)
	stqd	$12, SQWM7($sp)
# Place input parameters onto the stack to form the
# local storage memory image.
	ila	$10, __entry_event_format
	stqd	$10, SQWM12($sp)
	ai	$10, $sp, SQWM9
	stqd	$10, SQWM11($sp)
	stqd	$sp, SQWM10($sp)
	stqd	$78, SQWM9($sp)
	stqd	$79, SQWM8($sp)
# Construct a message consisting of the 8-bit opcode
# and 24-bit local store pointer to the input
# parameters and place it forllowing the stop and signal
	ila	$10, 0x3ffff		# address mask
	ilhu	$11, SPE_C99_VPRINTF << 8
	ai	$12, $sp, SQWM12	# parameter pointer
	selb	$11, $11, $12, $10	# combine command & address ptr
	brsl	$10, next1a
next1a:
	.type	next1a, @function
	lqr	$12, message1a
	cwd	$10, message1a-next1a($10)
	shufb	$11, $11, $12, $10	# insert msg into inst word
	stqr	$11, message1a		# store cmd/ptr into msg word
	dsync
# Notify the PPE to perform the assisted call request
# by issing a stop and signal with a signal code
# of 0x2100 (C99 class)
	stop	0x2100
message1a:
	.word	0

# save registers
	stqd	$13, SQWM8($sp)
	stqd	$14, SQWM9($sp)
	stqd	$15, SQWM10($sp)
	stqd	$16, SQWM11($sp)

# initialize loop
	il	$13, 1
	ila	$14, _ovly_buf_table
	ila	$15, _ovly_buf_table_end

loop_start1:
# Place input parameters onto the stack to form the
# local storage memory image.
	ila	$10, __ovly_buf_table_format
	stqd	$10, SQWM16($sp)
	ai	$10, $sp, SQWM13
	stqd	$10, SQWM15($sp)
	stqd	$sp, SQWM14($sp)
	stqd	$13, SQWM13($sp)
	lqd	$16, 0($14)
	rotqby	$16, $16, $14
	stqd	$16, SQWM12($sp)
# Construct a message consisting of the 8-bit opcode
# and 24-bit local store pointer to the input
# parameters and place it forllowing the stop and signal
	ila	$10, 0x3ffff		# address mask
	ilhu	$11, SPE_C99_VPRINTF << 8
	ai	$12, $sp, SQWM16	# parameter pointer
	selb	$11, $11, $12, $10	# combine command & address ptr
	brsl	$10, next1b
next1b:
	.type	next1b, @function
	lqr	$12, message1b
	cwd	$10, message1b-next1b($10)
	shufb	$11, $11, $12, $10	# insert msg into inst word
	stqr	$11, message1b		# store cmd/ptr into msg word
	dsync
# Notify the PPE to perform the assisted call request
# by issing a stop and signal with a signal code
# of 0x2100 (C99 class)
	stop	0x2100
message1b:
	.word	0

# move to next entry
	ai	$13, $13, 1
	ai	$14, $14, 4
	clgt	$16, $15, $14
	brnz	$16, loop_start1

# restore registers
	lqd	$16, SQWM11($sp)
	lqd	$15, SQWM10($sp)
	lqd	$14, SQWM9($sp)
	lqd	$13, SQWM8($sp)
	lqd	$12, SQWM7($sp)
	lqd	$11, SQWM6($sp)
	lqd	$10, SQWM5($sp)
//==============================================
#endif

/* Set branch hint to overlay target. */
	hbr		__ovly_load_ret, $79

/* Get caller's overlay index by back chaining through stack frames.
 * Loop until end of stack (back chain all-zeros) or
 * encountered a link register we set here. */
	lqd		bchn, 0($sp)
	ila		retval, __ovly_return

__ovly_backchain_loop:
	lqd		lnkr, 16(bchn)
	lqd		bchn, 0(bchn)
	ceq		cmp, lnkr, retval
	ceqi		cmp2, bchn, 0
	or		cmp, cmp, cmp2
	brz		cmp, __ovly_backchain_loop

/* If we reached the zero back-chain, then lnkr is bogus.  Clear the
 * part of lnkr that we use later (slot 3). */
	rotqbyi		cmp2, cmp2, 4
	andc		lnkr, lnkr, cmp2

/* Set lr = {__ovly_return, prev ovl ndx, caller return adr, callee ovl ndx}. */
	lqd		rv1, (__rv_pattern-__ovly_return+4)(retval)
	shufb		rv2, retval, lnkr, rv1
	shufb		rv3, $lr, $78, rv1
	fsmbi		rv1, 0xff
	selb		rv2, rv2, rv3, rv1
/* If we have a tail call from one overlay function to another overlay,
   then lr is already set up.  Don't change it.  */
	ceq		rv1, $lr, retval
	fsmb		rv1, rv1
	selb		$lr, rv2, $lr, rv1

/* Branch to $79 if non-overlay */
	brz		$78, __ovly_load_restore

/* Load values from _ovly_table[$78].
 *	extern struct {
 *		u32 vma;
 *		u32 size;
 *		u32 file_offset;
 *		u32 buf;
 *	} _ovly_table[];
 */
	shli		off, $78, 4
	ila		tab, _ovly_table - 16
	lqx		vma, tab, off
	rotqbyi		buf, vma, 12

/* Load values from _ovly_buf_table[buf].
 *	extern struct {
 *		u32 mapped;
 *	} _ovly_buf_table[];
 */
	ila		tab, _ovly_buf_table
	ai		off, buf, -1
	shli		off, off, 2
	lqx		map, tab, off
	rotqby		cur, map, off

/* Branch to $79 now if overlay is already mapped.  */
	ceq		cmp, $78, cur
	brnz		cmp, __ovly_load_restore

/* Marker for profiling code.  If we get here, we are about to load
 * a new overlay.
 */
	.global		__ovly_load_event
	.type		__ovly_load_event, @function
__ovly_load_event:

/* Set _ovly_buf_table[buf].mapped = $78. */
	cwx		genwi, tab, off
	shufb		map, $78, map, genwi
	stqx		map, tab, off

/* A new partition needs to be loaded. Prepare for DMA loop.
 * _EAR_ is the 64b base EA, filled in at run time by the
 * loader, and indicating the value for SPU executable image start.
 */
	lqd		cgshuf, (__cg_pattern-__ovly_return+4)(retval)
	rotqbyi		osize, vma, 4
	rotqbyi		sz, vma, 8
	lqa		ea64, _EAR_

__ovly_xfer_loop:
/* 64b add to compute next ea64. */
	rotqmbyi	off64, sz, -4
	cg		cgbits, ea64, off64
	shufb		add64, cgbits, cgbits, cgshuf
	addx		add64, ea64, off64
	ori		ea64, add64, 0

/* Setup DMA parameters, then issue DMA request. */
	rotqbyi		ealo, add64, 4
	ila		maxsize, MFC_MAX_DMA_SIZE
	cgt		cmp, osize, maxsize
	selb		sz, osize, maxsize, cmp
	ila		tagid, MFC_TAG_ID
	wrch		$MFC_LSA, vma
	wrch		$MFC_EAH, ea64
	wrch		$MFC_EAL, ealo
	wrch		$MFC_Size, sz
	wrch		$MFC_TagId, tagid
	ila		cmd, MFC_GET_CMD
	wrch		$MFC_Cmd, cmd

#ifdef OVLY_PRINTFS
//==============================================
// In dma_event_hook vma=0x%08x ea=%08x%08x sz=%08x
//==============================================
# save registers
	stqd	$10, SQWM5($sp)
	stqd	$11, SQWM6($sp)
	stqd	$12, SQWM7($sp)
# Place input parameters onto the stack to form the
# local storage memory image.
	ila	$10, __dma_event_format
	stqd	$10, SQWM14($sp)
	ai	$10, $sp, SQWM11
	stqd	$10, SQWM13($sp)
	stqd	$sp, SQWM12($sp)
	stqd	vma, SQWM11($sp)
	stqd	ea64, SQWM10($sp)
	stqd	ealo, SQWM9($sp)
	stqd	sz, SQWM8($sp)
# Construct a message consisting of the 8-bit opcode
# and 24-bit local store pointer to the input
# parameters and place it forllowing the stop and signal
	ila	$10, 0x3ffff		# address mask
	ilhu	$11, SPE_C99_VPRINTF << 8
	ai	$12, $sp, SQWM14	# parameter pointer
	selb	$11, $11, $12, $10	# combine command & address ptr
	brsl	$10, next3a
next3a:
	.type	next3a, @function
	lqr	$12, message3a
	cwd	$10, message3a-next3a($10)
	shufb	$11, $11, $12, $10	# insert msg into inst word
	stqr	$11, message3a		# store cmd/ptr into msg word
	dsync
# Notify the PPE to perform the assisted call request
# by issing a stop and signal with a signal code
# of 0x2100 (C99 class)
	stop	0x2100
message3a:
	.word	0

# restore registers
	lqd	$12, SQWM7($sp)
	lqd	$11, SQWM6($sp)
	lqd	$10, SQWM5($sp)
//==============================================
#endif

/* Increment vma, decrement size, branch back as needed. */
	a		vma, vma, sz
	sf		osize, sz, osize
	brnz		osize, __ovly_xfer_loop

/* Save app's tagmask, wait for DMA complete, restore mask. */
	rdch		oldmask, $MFC_RdTagMask
#if MFC_TAG_ID < 16
	ilh		newmask, 1 << MFC_TAG_ID
#else
	ilhu		newmask, 1 << (MFC_TAG_ID - 16)
#endif
	wrch		$MFC_WrTagMask, newmask
	ila		tagstat, MFC_TAG_UPDATE_ALL
	wrch		$MFC_WrTagUpdate, tagstat
	rdch		tagstat, $MFC_RdTagStat
	sync
	wrch		$MFC_WrTagMask, oldmask

#ifdef OVLY_PRINTFS
//==============================================
// In debug_event_hook link-register=0x%08x %08x %08x %08x
//==============================================
# save registers
	stqd	$10, SQWM5($sp)
	stqd	$11, SQWM6($sp)
	stqd	$12, SQWM7($sp)
# Place input parameters onto the stack to form the
# local storage memory image.
	ila	$10, __debug_event_format
	stqd	$10, SQWM14($sp)
	ai	$10, $sp, SQWM11
	stqd	$10, SQWM13($sp)
	stqd	$sp, SQWM12($sp)
	stqd	$lr, SQWM11($sp)
	rotqbyi $10, $lr, 4
	stqd	$10, SQWM10($sp)
	rotqbyi $10, $10, 4
	stqd	$10, SQWM9($sp)
	rotqbyi $10, $10, 4
	stqd	$10, SQWM8($sp)
# Construct a message consisting of the 8-bit opcode
# and 24-bit local store pointer to the input
# parameters and place it forllowing the stop and signal
	ila	$10, 0x3ffff		# address mask
	ilhu	$11, SPE_C99_VPRINTF << 8
	ai	$12, $sp, SQWM14	# parameter pointer
	selb	$11, $11, $12, $10	# combine command & address ptr
	brsl	$10, next2a
next2a:
	.type	next2a, @function
	lqr	$12, message2a
	cwd	$10, message2a-next2a($10)
	shufb	$11, $11, $12, $10	# insert msg into inst word
	stqr	$11, message2a		# store cmd/ptr into msg word
	dsync
# Notify the PPE to perform the assisted call request
# by issing a stop and signal with a signal code
# of 0x2100 (C99 class)
	stop	0x2100
message2a:
	.word	0

# save registers
	stqd	$13, SQWM8($sp)
	stqd	$14, SQWM9($sp)
	stqd	$15, SQWM10($sp)
	stqd	$16, SQWM11($sp)

# initialize loop
	il	$13, 1
	ila	$14, _ovly_buf_table
	ila	$15, _ovly_buf_table_end

loop_start2:
# Place input parameters onto the stack to form the
# local storage memory image.
	ila	$10, __ovly_buf_table_format
	stqd	$10, SQWM16($sp)
	ai	$10, $sp, SQWM13
	stqd	$10, SQWM15($sp)
	stqd	$sp, SQWM14($sp)
	stqd	$13, SQWM13($sp)
	lqd	$16, 0($14)
	rotqby	$16, $16, $14
	stqd	$16, SQWM12($sp)
# Construct a message consisting of the 8-bit opcode
# and 24-bit local store pointer to the input
# parameters and place it forllowing the stop and signal
	ila	$10, 0x3ffff		# address mask
	ilhu	$11, SPE_C99_VPRINTF << 8
	ai	$12, $sp, SQWM16	# parameter pointer
	selb	$11, $11, $12, $10	# combine command & address ptr
	brsl	$10, next2b
next2b:
	.type	next2b, @function
	lqr	$12, message2b
	cwd	$10, message2b-next2b($10)
	shufb	$11, $11, $12, $10	# insert msg into inst word
	stqr	$11, message2b		# store cmd/ptr into msg word
	dsync
# Notify the PPE to perform the assisted call request
# by issing a stop and signal with a signal code
# of 0x2100 (C99 class)
	stop	0x2100
message2b:
	.word	0

# move to next entry
	ai	$13, $13, 1
	ai	$14, $14, 4
	clgt	$16, $15, $14
	brnz	$16, loop_start2

# restore registers
	lqd	$16, SQWM11($sp)
	lqd	$15, SQWM10($sp)
	lqd	$14, SQWM9($sp)
	lqd	$13, SQWM8($sp)
	lqd	$12, SQWM7($sp)
	lqd	$11, SQWM6($sp)
	lqd	$10, SQWM5($sp)
//==============================================
#endif

	.global		_ovly_debug_event
	.type		_ovly_debug_event, @function
_ovly_debug_event:
/* GDB inserts debugger trap here.  */
	nop

__ovly_load_restore:
#ifdef OVLY_IRQ_SAVE
/* Conditionally re-enable interrupts. */
	andi		irq_stat, irq_stat, 1
	ila		irqtmp, __ovly_irq_restore
	binze		irq_stat, irqtmp
__ovly_irq_restore:
	lqd		$9, -64($sp)
#endif

/* Restore saved registers. */
	lqd		$8, -48($sp)
	lqd		$7, -32($sp)
	lqd		$6, -16($sp)

__ovly_load_ret:
/* Branch to target address. */
	bi		$79

	.size		__ovly_load, . - __ovly_load
