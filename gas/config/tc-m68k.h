/*
 * This file is tp-generic.h and is intended to be a template for
 * target processor specific header files. 
 */

#define TC_M68K 1

#ifdef TE_SUN3
/* This variable contains the value to write out at the beginning of
   the a.out file.  The 2<<16 means that this is a 68020 file instead
   of an old-style 68000 file */

#define DEFAULT_MAGIC_NUMBER_FOR_OBJECT_FILE (2<<16|OMAGIC);	/* Magic byte for file header */
#endif /* TE_SUN3 */

#define tc_crawl_symbol_chain(a)	; /* not used */
#define tc_headers_hook(a)		; /* not used */

/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of tc-m68k.h */
