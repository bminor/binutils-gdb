/*
 * Copyright (C) 1993, 1994 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 */


#include "alpha-regdef.h"

#define LEAF_ENTRY(NAME) .text ; .align 4 ; .globl NAME ; .ent NAME, 0 ; NAME: ; .frame sp, 0, ra ; .prologue 0 ;


LEAF_ENTRY(flush_i_cache)
	call_pal 0x86			/* IMB */
	ret	zero, (ra)		/* return */
	.end	flush_i_cache



/* ++
 *
 * VOID
 * outVti(
 *     ULONG Port
 *     ULONG Data
 *    )
 *
 * Routine Description:
 *
 *     This Function Uses The 64-Bit Super-Page To Write Data To A Port
 *     Of The On-Board VTI Combo Chip For JENSEN.
 *
 * Arguments:
 *
 *    Port (A0) - Port Number On VTI Chip To Which To Write Data
 *    data (a1) - data to write to the port, only low byte is significant
 *                 To The VTI
 *
 * Return Value:
 *
 *    None.
 *
 */

	LEAF_ENTRY(outVti)

	/*
	 * generate super-page address of vti, base address
	 * N.B. - Upper Bits Must Be Sign Extension Of Bit 42
	 *   va<42:41> = 10 (binary) for super-page address
	 */

	lda	t0, 0xc01c(zero)	/* t0 = 0000 0000 0000 c01c */
	ldah	t0, -1(t0)		/* t0 = ffff ffff ffff c01c */
	sll	t0, 28, t0		/* t0 = ffff fc01 c000 0000 */


	/*
	 * Shift In The Port Number To Generate The Port Address We
	 * 	wish to access
	 * N.B. - Access Width Is Always Zero = Byte Access For VTI
	 */

	sll	a0, 9, a0		/* a0 << 9 */
	bis	t0, a0, t0		/* T0 = Address Of VTI Port */


	/*
	 * Do The Port Write, Guarantee That Subsequent Writes (And Reads)
	 *   are ordered with respect to this write and return to caller
	 */

	stl	a1, 0(t0)		/* write data to port */
	mb				/* guarantee write ordering */

	ret	zero, (ra)		/* return */

	.end	outVti



/*
 *
 * ULONG
 * inVti(
 *     ULONG Port
 *    )
 *
 * Routine Description:
 *
 *     This Function Uses The 64-Bit Super-Page To Read Data From A Port
 *     Of The On-Board VTI Combo Chip For JENSEN.
 *
 * Arguments:
 *
 *    Port (A0) - Port Number On VTI Chip To Which To Write Data
 *
 * Return Value:
 *
 *    Data (V0) - The Data Read From The VTI Chip, Only The Low Byte Will
 * 			be valid
 *
 */

	LEAF_ENTRY(inVti)

	/*
	 * generate super-page address of vti, base address
	 * N.B. - Upper Bits Must Be Sign Extension Of Bit 42
	 *   va<42:41> = 10 (binary) for super-page address
	 */

	lda	t0, 0xc01c(zero)	/* t0 = 0000 0000 0000 c01c */
	ldah	t0, -1(t0)		/* t0 = ffff ffff ffff c01c */
	sll	t0, 28, t0		/* t0 = ffff fc01 c000 0000 */


	/*
	 * Shift In The Port Number To Generate The Port Address We
	 * 	wish to access
	 * N.B. - Access Width For VTI Is Always 0 = Byte Access
	 */

	sll	a0, 9, a0		/* a0 << 9 */
	bis	t0, a0, t0		/* T0 = Address Of VTI Port */


	/*
	 * Do The Super-Page I/O Access And Return Data To Caller
	 */

	ldl	v0, 0(t0)		/* read data from port */
	and	v0, 0xff, v0

	ret	zero, (ra)		/* return  */

	.end	inVti
