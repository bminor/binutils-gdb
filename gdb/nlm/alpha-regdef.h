/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1992    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/*
 * Modification History: /sys/machine/alpha/regdef.h
 *
 * 01-Oct-90 -- rjl
 *	Defined the k0 and k1 registers.  They are not really used as such
 *	but they have to be defined in order for them to be saved during
 *	exception handling
 *
 * 06-Sep-90 -- kjl and afd
 *	Created this file for Alpha support.
 */

#define v0	$0

#define t0	$1
#define t1	$2
#define t2	$3
#define t3	$4
#define t4	$5
#define t5	$6
#define t6	$7
#define t7	$8

#define s0	$9
#define s1	$10
#define s2	$11
#define s3	$12
#define s4	$13
#define s5	$14
#define s6	$15
#define fp	$15	/* fp & s6 are the same	*/

#define a0	$16
#define a1	$17
#define a2	$18
#define a3	$19
#define a4	$20
#define a5	$21

#define t8	$22
#define t9	$23
#define t10	$24
#define t11	$25

#define ra	$26

#define pv	$27	/* pv and t5 are the same */
#define t12	$27

#define AT	$at

#define gp	$29

#define	sp	$30
#define zero	$31
