/* VAX-specific definitions for a.out file header fields.  */

#define PAGE_SIZE 512			/* aka NBPG in <sys/param.h> */
#define SEGMENT_SIZE PAGE_SIZE		/* rounding between text/data ? */
#define TEXT_START_ADDR 0		/* Text start address: see
					   <machine/vmparam.h> USRTEXT.  */
#define	STACK_END_ADDR	0x80000000-(14+14)*PAGE_SIZE   /* see
					   <machine/vmparam.h> USRSTACK.  */

#define N_BADMAG(x)					\
 (N_MAGIC(x) != OMAGIC && N_MAGIC(x) != NMAGIC		\
  && N_MAGIC(x) != ZMAGIC)


#define N_TXTOFF(x)	( (N_MAGIC((x)) == ZMAGIC) ? 1024 : EXEC_BYTES_SIZE )
#define N_DATOFF(x)	( N_TXTOFF(x) + (x).a_text )
#define N_TRELOFF(x)	( N_DATOFF(x) + (x).a_data )
#define N_DRELOFF(x)	( N_TRELOFF(x) + (x).a_trsize )
#define N_SYMOFF(x)	( N_DRELOFF(x) + (x).a_drsize )
#define N_STROFF(x)	( N_SYMOFF(x) + (x).a_syms )
