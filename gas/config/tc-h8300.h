
#define TC_H8300

/* This macro translates between an internal fix and an coff reloc type */
#define TC_COFF_FIX2RTYPE(fixP) \
   (fixP->fx_pcrel ?  (fixP->fx_size == 1 ? R_PCRBYTE : R_PCRWORD ) : \
   (fixP->fx_size == 1 ? R_RELBYTE : R_RELWORD ))


