#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

static bfd_arch_info_type arch_info_struct = 
  {
    32,	/* 32 bits in a word */
    32,	/* 32 bits in an address */
    8,	/* 8 bits in a byte */
    bfd_arch_a29k,
    0,	/* only 1 machine */
    "a29k",
    "a29k",
    4,
    true, /* the one and only */
    bfd_default_compatible, 
    bfd_default_scan ,
    0,
    bfd_default_reloc_type_lookup,
  };

void DEFUN_VOID(bfd_a29k_arch)
{
  bfd_arch_linkin(&arch_info_struct);
}
