/* SPARC-specific values for a.out files */

#define PAGE_SIZE 0x02000		/* 8K.  aka NBPG in <sys/param.h> */
/* Note that some SPARCs have 4K pages, some 8K, some others.  */
#define SEGMENT_SIZE PAGE_SIZE
#define TEXT_START_ADDR PAGE_SIZE	/* Location 0 is not accessible */

#define N_BADMAG(x)					\
 (N_MAGIC(x) != OMAGIC && N_MAGIC(x) != NMAGIC		\
  && N_MAGIC(x) != ZMAGIC)


#define N_TXTOFF(x)	( (N_MAGIC((x)) == ZMAGIC) ? 0 : sizeof(struct exec) )
#define N_DATOFF(x)	( N_TXTOFF(x) + (x).a_text )
#define N_TRELOFF(x)	( N_DATOFF(x) + (x).a_data )
#define N_DRELOFF(x)	( N_TRELOFF(x) + (x).a_trsize )
#define N_SYMOFF(x)	( N_DRELOFF(x) + (x).a_drsize )
#define N_STROFF(x)	( N_SYMOFF(x) + (x).a_syms )

