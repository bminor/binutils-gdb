/* expr.h -> header file for expr.c
   Copyright (C) 1987, 1992 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * Abbreviations (mnemonics).
 *
 *	O	operator
 *	Q	quantity,  operand
 *	X	eXpression
 */

/*
 * By popular demand, we define a struct to represent an expression.
 * This will no doubt mutate as expressions become baroque.
 *
 * Currently, we support expressions like "foo-bar+42".
 * In other words we permit a (possibly undefined) minuend, a
 * (possibly undefined) subtrahend and an (absolute) augend.
 * RMS says this is so we can have 1-pass assembly for any compiler
 * emissions, and a 'case' statement might emit 'undefined1 - undefined2'.
 *
 * To simplify table-driven dispatch, we also have a "segment" for the
 * entire expression. That way we don't require complex reasoning about
 * whether particular components are defined; and we can change component
 * semantics without re-working all the dispatch tables in the assembler.
 * In other words the "type" of an expression is its segment.
 */

typedef struct
{
  struct symbol *X_add_symbol;		/* "foo", above */
  struct symbol *X_subtract_symbol;	/* "bar", above */
  offsetT X_add_number;			/* 42, above -- must be signed */
  /* What segment (expr type)? */
  segT X_seg;
} expressionS;

/* "result" should be type (expressionS *). */
#define expression(result) expr(0,result)

/* If an expression is SEG_BIG, look here for its value. These common
   data may be clobbered whenever expr() is called. */
/* Flonums returned here.  Big enough to hold most precise flonum. */
extern FLONUM_TYPE generic_floating_point_number;
/* Bignums returned here. */
extern LITTLENUM_TYPE generic_bignum[];
/* Number of littlenums in above. */
#define SIZE_OF_LARGE_NUMBER (20)

typedef char operator_rankT;

char get_symbol_end PARAMS ((void));
segT expr PARAMS ((int rank, expressionS * resultP));
unsigned int get_single_number PARAMS ((void));

/* end of expr.h */
