/* Written by Ian Dall
 *            5-Jun-94
 */
#define MYNS(OP) CAT(ns32kaout_,OP)
CONST struct reloc_howto_struct *
MYNS(bfd_reloc_type_lookup)
  PARAMS((bfd *abfd AND
	  bfd_reloc_code_real_type code));

boolean
MYNS(write_object_contents)
  PARAMS((bfd *abfd));

/* Avoid multiple definitions from aoutx if supporting standard a.out format(s)
 * as well as this one
 */
#define NAME(x,y) CAT3(ns32kaout, _32_, y)
