/* Routines for debug use only.  Don't link into product.
 */

#include "as.h"
#include "subsegs.h"

dmp_frags()
{
	frchainS *chp;
	char *p;

	for ( chp=frchain_root; chp; chp = chp->frch_next ){
		switch ( chp->frch_seg ){
			case SEG_DATA:
				p ="Data";
				break;
			case SEG_TEXT:
				p ="Text";
				break;
			default:
				p ="???";
				break;
		}
		printf("\nSEGMENT %s %d\n", p, chp->frch_subseg);
		dmp_frag( chp->frch_root,"\t");
	}
}

dmp_frag( fp, indent )
    struct frag *fp;
    char *indent;
{
	for ( ; fp; fp = fp->fr_next ){
		printf("%sFRAGMENT @ 0x%x\n", indent, fp);
		switch( fp->fr_type ){
			case rs_align:
				printf("%srs_align(%d)\n",indent, fp->fr_offset);
				break;
			case rs_fill:
				printf("%srs_fill(%d)\n",indent, fp->fr_offset);
				printf("%s", indent);
				var_chars( fp, fp->fr_var + fp->fr_fix );
				printf("%s\t repeated %d times,",
							indent, fp->fr_offset);
				printf(" fixed length if # chars == 0)\n");
				break;
			case rs_org:
				printf("%srs_org(%d+sym @0x%x)\n",indent,
					fp->fr_offset, fp->fr_symbol);
				printf("%sfill with ",indent);
				var_chars( fp, 1 );
				printf("\n");
				break;
			case rs_machine_dependent:
				printf("%smachine_dep\n",indent);
				break;
			default:
				printf("%sunknown type\n",indent);
				break;
		}
		printf("%saddr=%d(0x%x)\n",indent,fp->fr_address,fp->fr_address);
		printf("%sfr_fix=%d\n",indent,fp->fr_fix);
		printf("%sfr_var=%d\n",indent,fp->fr_var);
		printf("%sfr_offset=%d\n",indent,fp->fr_offset);
		printf("%schars @ 0x%x\n",indent,fp->fr_literal);
		printf("\n");
	}
}

var_chars( fp, n )
    struct frag *fp;
    int n;
{
	unsigned char *p;

	for ( p=(unsigned char*)fp->fr_literal; n; n-- , p++ ){
		printf("%02x ", *p );
	}
}
