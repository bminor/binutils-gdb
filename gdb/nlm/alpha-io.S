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


/*#include "kxalpha.h"*/
#include "alpha-regdef.h"

#define LEAF_ENTRY(NAME) .text ; .align 4 ; .globl NAME ; .ent NAME, 0 ; NAME: ; .frame sp, 0, ra ; .prologue 0 ;


#define PICIACKADR	0xfffffc0100000000
#define EISABAD		0xfffffc0200000000
#define EISABIO		0xfffffc0300000000
#define EISA_SHIFT	7
#define EISA_BYTE_ADJ	0x80
#define EISA_WORD_ADJ	0x100
#define EISA_LONG_ADJ	0x200
#define HALF_USEC	75
#define ONE_USEC	150


LEAF_ENTRY(flush_i_cache)
	call_pal 0x86			// IMB
	ret	zero, (ra)		// return
	.end	outportb

//++
//
// VOID
// { outportX
//     ULONG port
//     ULONG data
//    )
// X variants are:
//
// 	b - byte    	8 bits
//	w - word       16 bits
//      t - tri-byte   24 bits
//      l - long       32 bits
//
// 
// Routine Description:
//
//     This function uses the 64-bit super-page to write data to a port
//     of the EISA bus for JENSEN. Only AT (ISA) cycles are supported.
//
// Arguments:
//
//    port (a0) - port address on the EISA  to which to write data
//    data (a1) - data to write to the port.
//              
//
// Return Value:
//
//    None.
//
//--


//+
// 	outportb
//--
	LEAF_ENTRY(outportb)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and 	a0,3,t0			// get byte index from address
	insbl	a1,t0,t5		// put byte in proper position
	and	a0,0x1ffffff,t1		// 25 bit mask
	ldiq	t0,EISABIO		// get EISA IO base address
	sll	t1, 7, a0		// shift 7 for EISA 
	bis	t0,a0,t0		// t0 = address of EISA 
	stl	t5, 0(t0)		// write data to port
	mb				// guarantee write ordering
	ret	zero, (ra)		// return
	.end	outportb


//+
// 	outportw
//--
	LEAF_ENTRY(outportw)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and 	a0,3,t0			// get byte index from address
	inswl	a1,t0,t5		// put byte in proper position
	and	a0,0x1ffffff,t1		// 25 bit mask
	ldiq	t0,EISABIO		// get EISA IO base address
	or	t0,0x20,t0		// t0 = ffff fc03 0000 0020 - word
	sll	t1, 7, a0		// shift 7 for EISA 
	bis	t0,a0,t0		// t0 = address of EISA 
	stl	t5, 0(t0)		// write data to port
	mb				// guarantee write ordering
	ret	zero, (ra)		// return
	.end	outportw


//+
// 	outportl
//--
	LEAF_ENTRY(outportl)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask
	ldiq	t0,EISABIO		// get EISA IO base address
	or	t0,0x60,t0		// t0 = ffff fc03 0000 0060 - long
	sll	t1, 7, a0		// shift 7 for EISA 
	bis	t0,a0,t0		// t0 = address of EISA 
	stl	a1, 0(t0)		// write data to port
	mb				// guarantee write ordering
	ret	zero, (ra)		// return
	.end	outportl



//+
// 	vgastl
//--
	LEAF_ENTRY(vgastl)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	sra	a0, 3, t0		// right shift addr by 3
	and	t0, 3, t1		// and addr with 3	
	s8addq  t1, zero, t2		// multiply by 8
	sll 	a1, t2, t3		// left shift data
	sll	a0, 4, t4		// left shift addr by 4
	lda	t0,0xfc00		// t0 = 0000 0000 0000 0c00
	ldah	t0,-1(t0)		// t0 = ffff ffff ffff 0c00
	sll	t0,32,t0		// t0 = ffff fc00 0000 0000
	or 	t0,t4,t4                // make io address
	stl	t3, 0(t4)		// store data
	mb				// guarantee write ordering
	ret	zero, (ra)		// return
	.end	vgastl




//++
//
// ULONG
// inportX(
//     ULONG port
//    )
//
// X variants are:
//
// 	b - byte    	8 bits
//	w - word       16 bits
//      t - tri-byte   24 bits
//      l - long       32 bits
//
// Routine Description:
//
//     This function uses the 64-bit super-page to read data from an EISA
//     port for JENSEN.
//
// Arguments:
//
//    port (a0) - EISA port number.
//
// Return Value:
//
//    data (v0) - the data read and  only the low byte is valid
//
//--

//+
// 	inportb
//--
	LEAF_ENTRY(inportb)

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask
	ldiq	t0,EISABIO		// get EISA IO base address
	sll	t1, 7, t2		// shift 7 for EISA 
	bis	t0,t2,t0		// t0 = address of EISA 
	ldl	v0, 0(t0)		// get EISA IO byte
	and 	a0,0x3,t1		// setup word shift count
	extbl	v0,t1,v0		// put into low byte
  	ret	zero, (ra)		// return 
	.end	inportb


//+
// 	inportw
//--
	LEAF_ENTRY(inportw)
	//
	// generate super-page address of vti, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask
	ldiq	t0,EISABIO		// get EISA IO base addr
	or	t0,0x20,t0		// t0 = ffff fc03 0000 0020 - word
	sll	t1, 7, t2		// shift 7 for EISA 
	bis	t0,t2,t0		// t0 = address of EISA 
	ldl	v0, 0(t0)		// load EISA word
	and 	a0,0x3,t1		// setup shift count
	extwl 	v0,t1,v0		// put into low word
	ret	zero, (ra)		// return 
	.end	inportw



//+
// 	inportl
//--
	LEAF_ENTRY(inportl)
	//
	// generate super-page address of vti, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask
	ldiq	t0,EISABIO		// get EISA IO base address
	or	t0,0x60,t0		// t0 = ffff fc03 0000 0060
	sll	t1, 7, t2		// shift 7 for EISA 
	bis	t0,t2,t0		// t0 = address of EISA 
	ldl	v0, 0(t0)		// load EISA word
	ret	zero, (ra)		// return 
	.end	inportl


//+
// 	vgaldl
//--
	LEAF_ENTRY(vgaldl)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	sll	a0, 4, t5		// left shift address by 4
	lda	t0,0xfc00		// t0 = 0000 0000 0000 fc00
	ldah	t0,-1(t0)		// t0 = ffff ffff ffff fc00
	sll	t0,32,t0		// t0 = ffff fc00 0000 0000
	or 	t0,t5,t0                // make io address
	ldl	t4, 0(t0)		// load data
	sra	a0, 3, t1		// right shift addr by 3
	and	t1, 3, t2		// and addr with 3	
	s8addq	t2, zero, t3		// multiply by 8
	srl 	t4, t3, v0		// right shift data
	ret	zero, (ra)		// return
	.end	vgaldl




//+
// 	inIack
//--
	
	LEAF_ENTRY(inIack)
	
	//	
	//	get Iack from pic, need two to get the vector
	//
	ldiq	t0,PICIACKADR
	ldl	v0,0(t0)		// load data
	and	v0,0xff,v0		// make it a byte
	ret	zero, (ra)		// return
	.end	inIack





	LEAF_ENTRY(outmemb)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and 	a0,3,t0			// get byte index from address
	insbl	a1,t0,t5		// put byte in proper position

	and	a0,0x1ffffff,t1		// 25 bit mask	
	ldiq	t0,EISABAD		// get EISA address

	sll	t1, 7, a0		// shift 7 for EISA 
	bis	t0,a0,t0		// t0 = address of EISA 
	stl	t5, 0(t0)		// write data to port
	ret	zero, (ra)		// return

	.end	outmemb



	LEAF_ENTRY(outmemw)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and 	a0,3,t0			// get byte index from address
	inswl	a1,t0,t5		// put byte in proper position

	and	a0,0x1ffffff,t1		// 25 bit mask
	
	ldiq	t0,EISABAD		// get EISA address
	or	t0,0x20,t0		// t0 = ffff fc02 0000 0020 - word

	sll	t1, 7, a0		// shift 7 for EISA 
	bis	t0,a0,t0		// t0 = address of EISA 
	stl	t5, 0(t0)		// write data to port
	ret	zero, (ra)		// return

	.end	outmemw




	LEAF_ENTRY(outmeml)
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask

	ldiq	t0,EISABAD		// get EISA address
	or	t0,0x60,t0		// t0 = ffff fc02 0000 0060 - long

	sll	t1, 7, a0		// shift 7 for EISA 
	bis	t0,a0,t0		// t0 = address of EISA 
	stl	a1, 0(t0)		// write data to port
	ret	zero, (ra)		// return

	.end	outmeml



	LEAF_ENTRY(outbuffb)

	/*  Arguments:
		a0	address of destination buffer (byte aligned).
        	a1	address of source buffer in memory (byte aligned)
       		a2      Number of bytes to move (Count).	
	*/	
		
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	beq	a2, donewb		// leave if nothing to do

	ldiq	t0,EISABAD		// get EISA address

	and 	a0,3,t3			// get byte index from dst address

	and	a0,0x1ffffff,t1		// 25 bit mask
	sll	t1,EISA_SHIFT,t4	// shift 7 for EISA
   	bis	t0,t4,t0		// t0 = address of EISA 

loopwb:
	ldq_u	t1, 0(a1)		// get src data
	subl	a2, 1, a2		// decrement count
	extbl	t1, a1,t1		// extract byte
	addl	a1, 1, a1		// point ot next src address
	insbl	t1,t3,t1		// insert byte in proper place
	stl	t1, 0(t0)		// write data to EISA memory
	addq    t0,EISA_BYTE_ADJ, t0 	// increment EISA memory pointer
	addl	t3,1,t3			// increment index
	and 	t3,3,t3			// mask off overflow
	bne	a2, loopwb
donewb:
	ret	zero, (ra)		

	.end	outbuffb



	LEAF_ENTRY(outbuffw)

	/*  Arguments:
		a0	address of destination buffer (word aligned), eisa.
        	a1	address of source buffer in memory (word aligned)
       		a2      Number of words to move (Count).	
	*/	
		
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	beq	a2, doneww		// leave if nothing to do

	ldiq	t0,EISABAD		// get EISA address
	or	t0,0x20,t0		// t0 = ffff fc02 0000 0020 - word

	and 	a0,3,t3			// get word index from dst address

	and	a0,0x1ffffff,t1		// 25 bit mask
	sll	t1,EISA_SHIFT,t4	// shift 7 for EISA
   	bis	t0,t4,t0		// t0 = address of EISA 

loopww:
	ldq_u	t1, 0(a1)		// get src data
	subl	a2, 1, a2		// decrement count
	extwl	t1, a1,t1		// extract word
	addl	a1, 2, a1		// point ot next src address
	inswl	t1,t3,t1		// insert in proper place
	stl	t1, 0(t0)		// write data to EISA memory
	addq    t0,EISA_WORD_ADJ, t0 	// increment EISA memory pointer
	addl	t3,2,t3			// increment index
	and 	t3,3,t3			// mask off overflow
	bne	a2, loopww
doneww:
	ret	zero, (ra)		

	.end	outbuffw





	LEAF_ENTRY(outbuffl)

	/*  Arguments:
		a0	address of destination buffer (long aligned), eisa.
        	a1	address of source buffer in memory (long aligned)
       		a2      Number of longs to move (Count).	
	*/	
		
	//
	// generate super-page address of EISA base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	beq	a2, donewl		// leave if nothing to do

	ldiq	t0,EISABAD		// get EISA address
	or	t0,0x60,t0		// t0 = ffff fc02 0000 0060 - long

	and	a0,0x1ffffff,t1		// 25 bit mask
	sll	t1,EISA_SHIFT,t4	// shift 7 for EISA
   	bis	t0,t4,t0		// t0 = address of EISA 

loopwl:
	ldl	t1, 0(a1)		// get src data
	subl	a2, 1, a2		// decrement count
	stl	t1, 0(t0)		// write data to EISA memory
	addl	a1, 4, a1		// point ot next src address
	addq    t0,EISA_LONG_ADJ, t0 	// increment EISA memory pointer
	bne	a2, loopwl
donewl:
	ret	zero, (ra)		

	.end	outbuffw



	LEAF_ENTRY(inmemb)

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask

	ldiq	t0,EISABAD		// get EISA address

	sll	t1, 7, t2		// shift 7 for EISA 
	bis	t0,t2,t0		// t0 = address of EISA 
	ldl	v0, 0(t0)		// get EISA byte

	and 	a0,0x3,t1		// setup word shift count
	extbl	v0,t1,v0		// put into low byte

	ret	zero, (ra)		// return 

	.end	inmemb




	LEAF_ENTRY(inmemw)

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask

	ldiq	t0,EISABAD		// get EISA base addr
	or	t0,0x20,t0		// t0 = ffff fc02 0000 0020 - word

	sll	t1, 7, t2		// shift 7 for EISA 
	bis	t0,t2,t0		// t0 = address of EISA 

	ldl	v0, 0(t0)		// get EISA short

	and	a0,0x3,t1		// setup byte shift count
	extwl 	v0,t1,v0		// put into low word

	ret	zero, (ra)		// return 

	.end	inmemw




	LEAF_ENTRY(inmeml)

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	and	a0,0x1ffffff,t1		// 25 bit mask

	ldiq	t0,EISABAD		// get EISA base address
	or	t0,0x60,t0              // t0 = ffff fc02 0000 0060 - long

	sll	t1, 7, t2		// shift 7 for EISA 
	bis	t0,t2,t0		// t0 = address of EISA 

	ldl	v0, 0(t0)		// get EISA 4 bytes
	ret	zero, (ra)		// return 

	.end	inmeml




	LEAF_ENTRY(inbuffb)

	/*
	Arguments:
		
	a0	source buffer in eisa bus memory.
        a1	destination buffer in memory.
        a2      Number of bytes to move (Count).
	*/	

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	beq	a2, donerb		// leave if nothing to do

	ldiq	t0,EISABAD		// get EISA base address

	and 	a0,3,t3			// get byte index from src address

	and	a0,0x1ffffff,t1		// 25 bit mask
	sll	t1, EISA_SHIFT, t4	// shift 7 for EISA 
	bis	t0,t4,t0		// t0 = address of EISA 

looprb:
	ldl	t1, 0(t0)		// get EISA 4 bytes
	subl	a2, 1, a2		// decrement byte count
	extbl	t1, t3, t1		// extract byte
	addq	t0, EISA_BYTE_ADJ, t0 	// increment EISA address
	stb	t1, 0(a1)		// assembler preserves the memory
					// behind the newly stored byte
	addl	a1, 1, a1		// increment memory pointer
	addl	t3, 1, t3		// point to next byte in long
	and	t3, 3, t3		// get new index		 
	bne	a2, looprb
donerb:
	ret	zero, (ra)		// return 

	.end	inbuffb




	LEAF_ENTRY(inbuffw)
	/*
	Arguments:
		
	a0	source buffer in eisa bus memory.
        a1	destination buffer in memory.
        a2      Number of words to move (Count).
	*/	


	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	beq	a2, donerw		// leave if nothing to do

	ldiq	t0,EISABAD		// get EISA base address
	or	t0,0x20,t0              // t0 = ffff fc02 0000 0020 - word


	and 	a0,3,t3			// get byte index from src address

	and	a0,0x1ffffff,t1		// 25 bit mask
	sll	t1, EISA_SHIFT, t4	// shift 7 for EISA 
	bis	t0,t4,t0		// t0 = address of EISA 

looprw:
	ldl	t1, 0(t0)		// get EISA 4 bytes
	subl	a2, 1, a2		// decrement word count
	extwl	t1, t3, t1		// extract word
	addq	t0, EISA_WORD_ADJ, t0 	// increment EISA address
	stw	t1, 0(a1)		// store in dst memory
	addl	a1, 2, a1		// increment memory pointer
	addl	t3, 2, t3		// point to next word in long
	and	t3, 3, t3		// get new index		 
	bne	a2, looprw
donerw:
	ret	zero, (ra)		// return 

	.end	inbuffw



	LEAF_ENTRY(inbuffl)
	/*
	Arguments:
		
	a0	source buffer in eisa bus memory.
        a1	destination buffer in memory.
        a2      Number of longs to move (Count).
	*/	

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	beq	a2, donerl		// leave if nothing to do

	ldiq	t0,EISABAD		// get EISA base address
	or	t0,0x60,t0              // t0 = ffff fc02 0000 0060 - long

	and	a0,0x1ffffff,t1		// 25 bit mask
	sll	t1, EISA_SHIFT, t4	// shift 7 for EISA 
	bis	t0,t4,t0		// t0 = address of EISA 

looprl:
	ldl	v0, 0(t0)		// get EISA 4 bytes
	subl	a2, 1, a2		// decrement long count
	stl	v0, 0(a1)		// store in dst memory
	addl	a1, 4, a1		// increment memory pointer
	addq	t0, EISA_LONG_ADJ, t0	// increment EISA address
	bne	a2, looprl
donerl:
	ret	zero, (ra)		// return 

	.end	inbuffl



	LEAF_ENTRY(inctl)

	//
	// generate super-page address of EISA, base address
	// upper bits must be sign extension of bit 42
	// va<42:41> = 10 (binary) for super-page address
	//
	lda	t0,0xfc01		// t0 = 0000 0000 0000 fc01
	ldah	t0,-1(t0)		// t0 = ffff ffff ffff fc01
	sll	t0,32,t0		// t0 = ffff fc01 0000 0000
	bis	t0,0xe0000000,t0	// t0 = ffff fc01 e000 0000
	
	ldl	v0, 0(t0)		// get EISA byte
	and	v0,0xff,v0

	ret	zero, (ra)		// return 

	.end	inctl




//++
//
// VOID
// outVti(
//     ULONG port
//     ULONG data
//    )
//
// Routine Description:
//
//     This function uses the 64-bit super-page to write data to a port
//     of the on-board VTI combo chip for JENSEN.
//
// Arguments:
//
//    port (a0) - port number on VTI chip to which to write data
//    data (a1) - data to write to the port, only low byte is significant
//                 to the VTI
//
// Return Value:
//
//    None.
//
//--

	LEAF_ENTRY(outVti)

	//
	// generate super-page address of vti, base address
	// N.B. - upper bits must be sign extension of bit 42
	//   va<42:41> = 10 (binary) for super-page address
	//

	lda	t0, 0xc01c(zero)	// t0 = 0000 0000 0000 c01c
	ldah	t0, -1(t0)		// t0 = ffff ffff ffff c01c
	sll	t0, 28, t0		// t0 = ffff fc01 c000 0000


	//
	// Shift in the port number to generate the port address we
	//	wish to access
	// N.B. - access width is always zero = byte access for VTI
	//

	sll	a0, 9, a0		// a0 << 9
	bis	t0, a0, t0		// t0 = address of VTI port


	//
	// Do the port write, guarantee that subsequent writes (and reads)
	//   are ordered with respect to this write and return to caller
	//

	stl	a1, 0(t0)		// write data to port
	mb				// guarantee write ordering

	ret	zero, (ra)		// return

	.end	outVti



//++
//
// ULONG
// inVti(
//     ULONG port
//    )
//
// Routine Description:
//
//     This function uses the 64-bit super-page to read data from a port
//     of the on-board VTI combo chip for JENSEN.
//
// Arguments:
//
//    port (a0) - port number on VTI chip to which to write data
//
// Return Value:
//
//    data (v0) - the data read from the VTI chip, only the low byte will
//			be valid
//
//--

	LEAF_ENTRY(inVti)

	//
	// generate super-page address of vti, base address
	// N.B. - upper bits must be sign extension of bit 42
	//   va<42:41> = 10 (binary) for super-page address
	//

	lda	t0, 0xc01c(zero)	// t0 = 0000 0000 0000 c01c
	ldah	t0, -1(t0)		// t0 = ffff ffff ffff c01c
	sll	t0, 28, t0		// t0 = ffff fc01 c000 0000


	//
	// Shift in the port number to generate the port address we
	//	wish to access
	// N.B. - access width for VTI is always 0 = byte access
	//

	sll	a0, 9, a0		// a0 << 9
	bis	t0, a0, t0		// t0 = address of VTI port


	//
	// Do the super-page i/o access and return data to caller
	//

	ldl	v0, 0(t0)		// read data from port
	and	v0, 0xff, v0

	ret	zero, (ra)		// return 

	.end	inVti



LEAF_ENTRY(delay_us)
	bis 	zero, ONE_USEC, t1
        rpcc    t0              	/* RCC T0, read cycle counter 	*/

loop0:  rpcc    t2              	/* RCC T2, read cycle counter	*/
        subl    t2, t0, t2      	/* check for wrapping 		*/
        bge     t2, over0      		/* check, and see if negative 	*/

	ornot	t0, zero, t4		/* calculate the offset		*/
	zap	t4, 0xf0, t4
	cmplt	t4, t1, t2
	beq	t2, done0
	subl	t1, t4, t1		/* adjust the counter		*/
	and	zero, t0, t0		/* set t0 to zero		*/

over0:  cmplt   t2, t1, t2      	/* compare these for usec timer */
        bne     t2, loop0       	/* stay in... 			*/
done0:  ret     zero, (ra)
        .end    delay_us



LEAF_ENTRY(delay_500ns)
	bis 	zero, HALF_USEC, t1
        rpcc    t0              	/* RCC T0, read cycle counter 	*/

loop1: 	rpcc    t2              	/* RCC T2, read cycle counter	*/
        subl    t2, t0, t2      	/* check for wrapping 		*/
        bge     t2, over1      		/* check, and see if negative 	*/

	ornot	t0, zero, t4		/* calculate the offset		*/
	zap	t4, 0xf0, t4
	cmplt	t4, t1, t2
	beq	t2, done1
	subl	t1, t4, t1		/* adjust the counter		*/
	and	zero, t0, t0		/* set t0 to zero		*/

over1:	cmplt   t2, t1, t2      	/* compare these for usec timer */
        bne     t2, loop1       	/* stay in... 			*/
done1:  ret     zero, (ra)
        .end    delay_500ns



