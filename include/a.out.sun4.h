/* SPARC-specific values for a.out files */

#define PAGE_SIZE	0x2000		/* 8K.  aka NBPG in <sys/param.h> */
/* Note that some SPARCs have 4K pages, some 8K, some others.  */
#define SEGMENT_SIZE	PAGE_SIZE
#define TEXT_START_ADDR	PAGE_SIZE	/* Location 0 is not accessible */

/* Non-default definitions of the accessor macros... */

/* Offset in a.out file of the text section.  For ZMAGIC, the text section
   actually includes the a.out header.  */

#define N_TXTOFF(x)	( (N_MAGIC((x)) == ZMAGIC) ? 0 : EXEC_BYTES_SIZE)

/* Virtual Address of text segment from the a.out file.  For OMAGIC,
   (almost always "unlinked .o's" these days), should be zero.
   Sun added a kludge so that shared libraries linked ZMAGIC get
   an address of zero if a_entry (!!!) is lower than the otherwise
   expected text address.  These kludges have gotta go!
   For linked files, should reflect reality if we know it.  */

#define N_TXTADDR(x) \
    (N_MAGIC(x)==OMAGIC? 0 \
     : (N_MAGIC(x) == ZMAGIC && (x).a_entry < TEXT_START_ADDR)? 0 \
     : TEXT_START_ADDR)
