
#ifndef NO_LISTING
#define LISTING_NEWLINE() if (listing) listing_newline(input_line_pointer);
#else
#define LISTING_NEWLINE() 
#endif
void EXFUN(listing_title,(unsigned int depth));
void EXFUN(listing_list,());
void EXFUN(listing_flags,());
void EXFUN(listing_psize,());
void EXFUN(listing_eject,());
void EXFUN(listing_width,(unsigned int x));
void EXFUN(listing_source_line,(unsigned int));
void EXFUN(listing_source_file,(char *));

#define LISTING_LISTING    1
#define LISTING_SYMBOLS    2
#define LISTING_NOFORM     4
#define LISTING_HLL        8
#define LISTING_NODEBUG   16

#define LISTING_DEFAULT    (LISTING_LISTING | LISTING_HLL |  LISTING_SYMBOLS)

/* end of listing.h */
