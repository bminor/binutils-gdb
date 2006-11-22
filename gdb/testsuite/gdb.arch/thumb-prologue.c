/* Unwinder test program.

   Copyright 2006 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

void tpcs_frame (void);

int
main (void)
{
  tpcs_frame ();
  return 0;
}

/* Normally Thumb functions use r7 as the frame pointer.  However,
   with the GCC option -mtpcs-frame, they may use fp instead.  */

asm(".text\n"
    "	.align 2\n"
    "	.thumb_func\n"
    "	.code 16\n"
    "tpcs_frame_1:\n"
    "	sub	sp, #16\n"
    "	push	{r7}\n"
    "	add	r7, sp, #20\n"
    "	str	r7, [sp, #8]\n"
    "	mov	r7, pc\n"
    "	str	r7, [sp, #16]\n"
    "	mov	r7, fp\n"
    "	str	r7, [sp, #4]\n"
    "	mov	r7, lr\n"
    "	str	r7, [sp, #12]\n"
    "	add	r7, sp, #16\n"
    "	mov	fp, r7\n"
    "	mov	r7, sl\n"
    "	push	{r7}\n"

    /* Trap.  */
    "	.short	0xdffe\n"

    "	pop	{r2}\n"
    "	mov	sl, r2\n"
    "	pop	{r7}\n"
    "	pop	{r1, r2}\n"
    "	mov	fp, r1\n"
    "	mov	sp, r2\n"
    "	bx	lr\n"

    "	.align 2\n"
    "	.thumb_func\n"
    "	.code 16\n"
    "tpcs_frame:\n"
    "	sub	sp, #16\n"
    "	push	{r7}\n"
    "	add	r7, sp, #20\n"
    "	str	r7, [sp, #8]\n"
    "	mov	r7, pc\n"
    "	str	r7, [sp, #16]\n"
    "	mov	r7, fp\n"
    "	str	r7, [sp, #4]\n"
    "	mov	r7, lr\n"
    "	str	r7, [sp, #12]\n"
    "	add	r7, sp, #16\n"
    "	mov	fp, r7\n"
    "	mov	r7, sl\n"
    "	push	{r7}\n"

    /* Clobber saved regs.  */
    "	mov	r7, #0\n"
    "	mov	lr, r7\n"
    "	bl	tpcs_frame_1\n"

    "	pop	{r2}\n"
    "	mov	sl, r2\n"
    "	pop	{r7}\n"
    "	pop	{r1, r2}\n"
    "	mov	fp, r1\n"
    "	mov	sp, r2\n"
    "	bx	lr\n"
);
