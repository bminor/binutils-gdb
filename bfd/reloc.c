/* BFD support for handling relocation entries.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*doc*
@section Relocations

BFD maintains relocations in much the same was as it maintains
symbols; they are left alone until required, then read in en-mass and
traslated into an internal form. There is a common routine
@code{bfd_perform_relocation} which acts upon the canonical form to to
the actual fixup.

Note that relocations are maintained on a per section basis, whilst
symbols are maintained on a per BFD basis.

All a back end has to do to fit the BFD interface is to create as many
@code{struct reloc_cache_entry} as there are relocations in a
particuar section, and fill in the right bits:

@menu
* typedef arelent::
* reloc handling functions::
@end menu

*/
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
/*doc
@node typedef arelent, Relocations, reloc handling functions, Relocations
@section typedef arelent


*/

/*proto* bfd_perform_relocation
The relocation routine returns as a status an enumerated type:

*+++

$typedef enum bfd_reloc_status {
No errors detected

$  bfd_reloc_ok,

The relocation was performed, but there was an overflow.

$  bfd_reloc_overflow,

The address to relocate was not within the section supplied

$  bfd_reloc_outofrange,

Used by special functions

$  bfd_reloc_continue,

Unused 

$  bfd_reloc_notsupported,

Unsupported relocation size requested. 

$  bfd_reloc_other,

The symbol to relocate against was undefined.

$  bfd_reloc_undefined,

The relocaction was performed, but may not be ok - presently generated
only when linking i960 coff files with i960 b.out symbols.

$  bfd_reloc_dangerous
$   }
$ bfd_reloc_status_enum_type;

*---

*/

/*proto*

*+++

$typedef struct reloc_cache_entry 
${

A pointer into the canonical table of pointers 

$  struct symbol_cache_entry **sym_ptr_ptr;

offset in section                 

$  rawdata_offset address;

addend for relocation value        

$  bfd_vma addend;    

if sym is null this is the section 

$  struct sec *section;

Pointer to how to perform the required relocation

$  CONST struct reloc_howto_struct *howto;
$} arelent;

*---

*/

/*doc*
@table @code
@item sym_ptr_ptr
The symbol table pointer points to a pointer to the symbol ascociated with the
relocation request. This would naturaly be the pointer into the table
returned by the back end's get_symtab action. @xref{Symbols}. The
symbol is referenced through a pointer to a pointer so that tools like
the linker can fixup all the symbols of the same name by modifying
only one pointer. The relocation routine looks in the symbol and uses
the base of the section the symbol is attached to and the value of
the symbol as the initial relocation offset. If the symbol pointer is
zero, then the section provided is looked up.
@item address
The address field gives the offset in bytes from the base of the
section data which owns the relocation record to the first byte of
relocatable information. The actual data relocated will be relative to
this point - for example, a relocation type which modifies the bottom
two bytes of a four byte word would not touch the first byte pointed
to in a big endian world.
@item addend
The addend is a value provided by the back end to be added (!) to the
relocation offset. It's interpretation is dependent upon the howto.
For example, on the 68k the code:

*+
        char foo[];
        main()
                {
                return foo[0x12345678];
                }
*-
Could be compiled into:

*+
        linkw fp,#-4
        moveb @@#12345678,d0
        extbl d0
        unlk fp
        rts
*-

This could create a reloc pointing to foo, but leave the offset in the data
(something like)

*+
RELOCATION RECORDS FOR [.text]:
OFFSET   TYPE      VALUE 
00000006 32        _foo

00000000 4e56 fffc          ; linkw fp,#-4
00000004 1039 1234 5678     ; moveb @@#12345678,d0
0000000a 49c0               ; extbl d0
0000000c 4e5e               ; unlk fp
0000000e 4e75               ; rts
*-
Using coff and an 88k, some instructions don't have enough space in them to
represent the full address range, and pointers have to be loaded in
two parts. So you'd get something like:

*+
        or.u     r13,r0,hi16(_foo+0x12345678)
        ld.b     r2,r13,lo16(_foo+0x12345678)
        jmp      r1
*-
This whould create two relocs, both pointing to _foo, and with 0x12340000
in their addend field. The data would consist of:

*+

RELOCATION RECORDS FOR [.text]:
OFFSET   TYPE      VALUE 
00000002 HVRT16    _foo+0x12340000
00000006 LVRT16    _foo+0x12340000

00000000 5da05678           ; or.u r13,r0,0x5678
00000004 1c4d5678           ; ld.b r2,r13,0x5678
00000008 f400c001           ; jmp r1
*-
The relocation routine digs out the value from the data, adds it to
the addend to get the original offset and then adds the value of _foo.
Note that all 32 bits have to be kept around somewhere, to cope with
carry from bit 15 to bit 16.

On further example is the sparc and the a.out format. The sparc has a
similar problem to the 88k, in that some instructions don't have
room for an entire offset, but on the sparc the parts are created odd
sized lumps. The designers of the a.out format chose not to use the
data within the section for storing part of the offset; all the offset
is kept within the reloc. Any thing in the data should be ignored.

*+
        save %sp,-112,%sp
        sethi %hi(_foo+0x12345678),%g2
        ldsb [%g2+%lo(_foo+0x12345678)],%i0
        ret
        restore
*-
Both relocs contains a pointer to foo, and the offsets would contain junk.

*+
RELOCATION RECORDS FOR [.text]:
OFFSET   TYPE      VALUE 
00000004 HI22      _foo+0x12345678
00000008 LO10      _foo+0x12345678

00000000 9de3bf90     ; save %sp,-112,%sp
00000004 05000000     ; sethi %hi(_foo+0),%g2
00000008 f048a000     ; ldsb [%g2+%lo(_foo+0)],%i0
0000000c 81c7e008     ; ret
00000010 81e80000     ; restore
*-
@item section  
The section field is only used when the symbol pointer field is null.
It supplies the section into which the data should be relocated. The
field's main use comes from assemblers which do most of the symbol fixups
themselves; an assembler may take an internal reference to a label,
but since it knows where the label is, it can turn the relocation
request from a symbol lookup into a section relative relocation - the
relocation emitted has no symbol, just a section to relocate against.

I'm not sure what it means when both a symbol pointer an a section
pointer are present. Some formats use this sort of mechanism to
describe PIC relocations, but BFD can't to that sort of thing yet.
@item howto
The howto field can be imagined as a relocation instruction. It is a
pointer to a struct which contains information on what to do with all
the other information in the reloc record and data section. A back end
would normally have a relocation instruction set and turn relocations
into pointers to the correct structure on input - but it would be
possible to create each howto field on demand.
@end table
*/


/*proto* reloc_howto_type
The @code{reloc_howto_type} is a structure which contains all the
information that BFD needs to know to tie up a back end's data.

*+++

$typedef CONST struct reloc_howto_struct 
${ 
The type field has mainly a documetary use - the back end can to what
it wants with it, though the normally the back end's external idea of
what a reloc number would be would be stored in this field. For
example, the a PC relative word relocation in a coff environment would
have the type 023 - because that's what the outside world calls a
R_PCRWORD reloc.

$  unsigned int type;

The value the final relocation is shifted right by. This drops
unwanted data from the relocation. 

$  unsigned int rightshift;

The size of the item to be relocated - 0, is one byte, 1 is 2 bytes, 3
is four bytes.

$  unsigned int size;

Now obsolete

$  unsigned int bitsize;

Notes that the relocation is relative to the location in the data
section of the addend. The relocation function will subtract from the
relocation value the address of the location being relocated.

$  boolean pc_relative;

Now obsolete

$  unsigned int bitpos;

Now obsolete

$  boolean absolute;

Causes the relocation routine to return an error if overflow is
detected when relocating.

$  boolean complain_on_overflow;

If this field is non null, then the supplied function is called rather
than the normal function. This allows really strange relocation
methods to be accomodated (eg, i960 callj instructions).

$  bfd_reloc_status_enum_type (*special_function)();

The textual name of the relocation type.

$  char *name;

When performing a partial link, some formats must modify the
relocations rather than the data - this flag signals this.

$  boolean partial_inplace;

The src_mask is used to select what parts of the read in data are to
be used in the relocation sum. Eg, if this was an 8 bit bit of data
which we read and relocated, this would be 0x000000ff. When we have
relocs which have an addend, such as sun4 extended relocs, the value
in the offset part of a relocating field is garbage so we never use
it. In this case the mask would be 0x00000000.

$  bfd_word src_mask;
The dst_mask is what parts of the instruction are replaced into the
instruction. In most cases src_mask == dst_mask, except in the above
special case, where dst_mask would be 0x000000ff, and src_mask would
be 0x00000000.

$  bfd_word dst_mask;           

When some formats create PC relative instructions, they leave the
value of the pc of the place being relocated in the offset slot of the
instruction, so that a PC relative relocation can be made just by
adding in an ordinary offset (eg sun3 a.out). Some formats leave the
displacement part of an instruction empty (eg m88k bcs), this flag
signals the fact.

$  boolean pcrel_offset;
$} reloc_howto_type;
*---

*/

/*proto* HOWTO
The HOWTO define is horrible and will go away.
*+
#define HOWTO(C, R,S,B, P, BI, ABS, O, SF, NAME, INPLACE, MASKSRC, MASKDST, PC) \
  {(unsigned)C,R,S,B, P, BI, ABS,O,SF,NAME,INPLACE,MASKSRC,MASKDST,PC}
*-

*/

/*proto* reloc_chain
*+
typedef unsigned char bfd_byte;

typedef struct relent_chain {
  arelent relent;
  struct   relent_chain *next;
} arelent_chain;

*-

*/



/*proto*
If an output_bfd is supplied to this function the generated image
will be relocatable, the relocations are copied to the output file
after they have been changed to reflect the new state of the world.
There are two ways of reflecting the results of partial linkage in an
output file; by modifying the output data in place, and by modifying
the relocation record. Some native formats (eg basic a.out and basic
coff) have no way of specifying an addend in the relocation type, so
the addend has to go in the output data.  This is no big deal since in
these formats the output data slot will always be big enough for the
addend. Complex reloc types with addends were invented to solve just
this problem.
*; PROTO(bfd_reloc_status_enum_type,
                bfd_perform_relocation,
                        (bfd * abfd,
                        arelent *reloc_entry,
                        PTR data,
                        asection *input_section,
                        bfd *output_bfd));
*/


bfd_reloc_status_enum_type
DEFUN(bfd_perform_relocation,(abfd,
                              reloc_entry,
                              data,
                              input_section,
                              output_bfd),
      bfd *abfd AND
      arelent *reloc_entry AND
      PTR data AND
      asection *input_section AND
      bfd *output_bfd)
{
  bfd_vma relocation;
  bfd_reloc_status_enum_type flag = bfd_reloc_ok;
  bfd_vma addr = reloc_entry->address ;
  bfd_vma output_base = 0;
  reloc_howto_type *howto = reloc_entry->howto;
  asection *reloc_target_output_section;
  asection *reloc_target_input_section;
  asymbol *symbol;

  if (reloc_entry->sym_ptr_ptr) {
    symbol = *( reloc_entry->sym_ptr_ptr);
    if ((symbol->flags & BSF_UNDEFINED) && output_bfd == (bfd *)NULL) {
      flag = bfd_reloc_undefined;
    }
  }
  else {
    symbol = (asymbol*)NULL;
  }

  if (howto->special_function){
    bfd_reloc_status_enum_type cont;
    cont = howto->special_function(abfd,
                                   reloc_entry,
                                   symbol,
                                   data,
                                   input_section);
    if (cont != bfd_reloc_continue) return cont;
  }

  /* 
    Work out which section the relocation is targetted at and the
    initial relocation command value.
    */


  if (symbol != (asymbol *)NULL){
    if (symbol->flags & BSF_FORT_COMM) {
      relocation = 0;
    }
    else {
      relocation = symbol->value;
    }
    if (symbol->section != (asection *)NULL)
        {
          reloc_target_input_section = symbol->section;
        }
    else {
      reloc_target_input_section = (asection *)NULL;
    }
  }
  else if (reloc_entry->section != (asection *)NULL)
      {
        relocation = 0;
        reloc_target_input_section = reloc_entry->section;
      }
  else {
    relocation = 0;
    reloc_target_input_section = (asection *)NULL;
  }


  if (reloc_target_input_section != (asection *)NULL) {

    reloc_target_output_section =
      reloc_target_input_section->output_section;

    if (output_bfd && howto->partial_inplace==false) {
      output_base = 0;
    }
    else {
      output_base = reloc_target_output_section->vma;

    }

    relocation += output_base +   reloc_target_input_section->output_offset;
  }

  relocation += reloc_entry->addend ;


  if(reloc_entry->address > (bfd_vma)(input_section->size)) 
      {
        return bfd_reloc_outofrange;
      }
          

  if (howto->pc_relative == true)
      {
        /*
          Anything which started out as pc relative should end up that
	  way too. 
	  
	  There are two ways we can see a pcrel instruction. Sometimes
	  the pcrel displacement has been partially calculated, it
	  includes the distance from the start of the section to the
	  instruction in it (eg sun3), and sometimes the field is
	  totally blank - eg m88kbcs.
	  */

        
        relocation -= 
          output_base +   input_section->output_offset;

        if (howto->pcrel_offset == true) {
          relocation -= reloc_entry->address;
        }

      }

  if (output_bfd!= (bfd *)NULL) {
    if ( howto->partial_inplace == false)  {
      /*
        This is a partial relocation, and we want to apply the relocation
	to the reloc entry rather than the raw data. Modify the reloc
	inplace to reflect what we now know.
	*/
      reloc_entry->addend = relocation  ;
      reloc_entry->section = reloc_target_input_section;
      if (reloc_target_input_section != (asection *)NULL) {
        /* If we know the output section we can forget the symbol */
        reloc_entry->sym_ptr_ptr = (asymbol**)NULL;
      }
      reloc_entry->address += 
        input_section->output_offset;
      return flag;
    }
    else 
        {
          /* This is a partial relocation, but inplace, so modify the
	     reloc record a bit. 
	     
	     If we've relocated with a symbol with a section, change
	     into a ref to  the section belonging to the symbol
	     */

	  if (symbol != (asymbol *)NULL && reloc_target_input_section != (asection *)NULL) 
	      {
		reloc_entry->section = reloc_target_input_section;
		reloc_entry->sym_ptr_ptr  = (asymbol **)NULL;
	      }

        }
  }

  reloc_entry->addend = 0;


  /* 
    Either we are relocating all the way, or we don't want to apply
    the relocation to the reloc entry (probably because there isn't
    any room in the output format to describe addends to relocs)
    */
  relocation >>= howto->rightshift;

  /* Shift everything up to where it's going to be used */
   
  relocation <<= howto->bitpos;

  /* Wait for the day when all have the mask in them */

  /* What we do:
     i instruction to be left alone
     o offset within instruction
     r relocation offset to apply
     S src mask
     D dst mask
     N ~dst mask
     A part 1
     B part 2
     R result
     
     Do this:
     i i i i i o o o o o        from bfd_get<size>
     and           S S S S S    to get the size offset we want
     +   r r r r r r r r r r  to get the final value to place
     and           D D D D D  to chop to right size
     -----------------------
     A A A A A 
     And this:
     ...   i i i i i o o o o o  from bfd_get<size>
     and   N N N N N            get instruction
     -----------------------
     ...   B B B B B
     
     And then:       
     B B B B B       
     or              A A A A A     
     -----------------------
     R R R R R R R R R R        put into bfd_put<size>
     */

#define DOIT(x) \
  x = ( (x & ~howto->dst_mask) | (((x & howto->src_mask) +  relocation) & howto->dst_mask))

    switch (howto->size)
        {
        case 0:
            {
              char x = bfd_get_8(abfd, (char *)data + addr);
              DOIT(x);
              bfd_put_8(abfd,x, (unsigned char *) data + addr);
            }
          break;

        case 1:
            { 
              short x = bfd_get_16(abfd, (bfd_byte *)data + addr);
              DOIT(x);
              bfd_put_16(abfd, x,   (unsigned char *)data + addr);
            }
          break;
        case 2:
            {
              long  x = bfd_get_32(abfd, (bfd_byte *) data + addr);
              DOIT(x);
              bfd_put_32(abfd,x,    (bfd_byte *)data + addr);
            }      
          break;
        case 3:

          /* Do nothing */
          break;
        default:
          return bfd_reloc_other;
        }

  return flag;
}


