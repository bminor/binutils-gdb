#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

static bfd_arch_info_type arch_info_struct = 
  {
    32,	/* 32 bits in a word */
    32,	/* 32 bits in an address */
    8,	/* 8 bits in a byte */
    bfd_arch_hppa,
    0,	/* only 1 machine */
    "hppa",
    "hppa",
    3,
    true, /* the one and only */
    bfd_default_compatible, 
    bfd_default_scan ,
    0,
    0,
  };

void DEFUN_VOID(bfd_hppa_arch)
{
  bfd_arch_linkin(&arch_info_struct);
}
