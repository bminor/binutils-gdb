#include "as.h"

static 

const short seg_N_TYPE[] = {
  N_ABS,
  N_TEXT,
  N_DATA,
  N_BSS,
  N_UNDF, /* unknown */
  N_UNDF, /* absent */
  N_UNDF, /* pass1 */
  N_UNDF, /* error */
  N_UNDF, /* bignum/flonum */
  N_UNDF, /* difference */
  N_REGISTER, /* register */
};

const segT N_TYPE_seg [N_TYPE+2] = {	/* N_TYPE == 0x1E = 32-2 */
	SEG_UNKNOWN,			/* N_UNDF == 0 */
	SEG_GOOF,
	SEG_ABSOLUTE,			/* N_ABS == 2 */
	SEG_GOOF,
	SEG_TEXT,			/* N_TEXT == 4 */
	SEG_GOOF,
	SEG_DATA,			/* N_DATA == 6 */
	SEG_GOOF,
	SEG_BSS,			/* N_BSS == 8 */
	SEG_GOOF,
	SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
	SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
	SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
	SEG_REGISTER,			/* dummy N_REGISTER for regs = 30 */
	SEG_GOOF,
};


void obj_symbol_new_hook(symbolP)
symbolS *symbolP;
{
	return;
} /* obj_symbol_new_hook() */

/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of obj-bfd-sunos.c */
