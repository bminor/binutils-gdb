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


#define com1Rbr	0x3f8		/* Receiver Buffer - Read	*/
#define com1Thr	0x3f8		/* Transmitter Holding - Write	*/
#define com1Ier	0x3f9		/* Interrupt Enable		*/
#define com1Iir	0x3fa		/* Interrupt Identification	*/
#define com1Lcr	0x3fb		/* Line Control			*/
#define com1Mcr	0x3fc		/* Modem Control		*/
#define com1Lsr	0x3fd		/* Line Status			*/
#define com1Msr	0x3fe		/* Modem Status			*/
#define com1Scr	0x3ff		/* Scratch			*/
#define com1Dll	0x3f8		/* Divisor Latch - lsb		*/
#define com1Dlm	0x3f9		/* Divisor Latch - msb		*/

#define com2Rbr	0x2f8		/* Receiver Buffer - Read	*/
#define com2Thr	0x2f8		/* Transmitter Holding - Write	*/
#define com2Ier	0x2f9		/* Interrupt Enable		*/
#define com2Iir	0x2fa		/* Interrupt Identification	*/
#define com2Lcr	0x2fb		/* Line Control			*/
#define com2Mcr	0x2fc		/* Modem Control		*/
#define com2Lsr	0x2fd		/* Line Status			*/
#define com2Msr	0x2fe		/* Modem Status			*/
#define com2Scr	0x2ff		/* Scratch			*/
#define com2Dll	0x2f8		/* Divisor Latch - lsb		*/
#define com2Dlm	0x2f9		/* Divisor Latch - msb		*/

#define	lptDr	0x3bc
#define	lptSr	0x3bd
#define	lptCr	0x3be

#define	lptSTB	0x01
#define	lptAFD	0x02
#define	lptnInit 0x04
#define	lptSlct	0x08
#define	lptIrq	0x10
#define	lptDir	0x20

#define	lptDly	100000

