
#define TC_H8300

/* This macro translates between an internal fix and an coff reloc type */
#define TC_COFF_FIX2RTYPE(fixP) abort();
    
#define BFD_ARCH bfd_arch_h8300
#define COFF_MAGIC 0x8300
#define TC_COUNT_RELOC(x) (1)


#define TC_RELOC_MANGLE(a,b,c) tc_reloc_mangle(a,b,c)

#define DO_NOT_STRIP 1
#define DO_STRIP 0
#define LISTING_HEADER "Hitachi H8/300 GAS "

/* end of tc-h8300.h */
