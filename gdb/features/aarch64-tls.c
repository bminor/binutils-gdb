/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: aarch64-tls.xml */

#include "gdbsupport/tdesc.h"

static int
create_feature_aarch64_tls (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.aarch64.tls");
  tdesc_create_reg (feature, "tpidr", regnum++, 1, NULL, 64, "data_ptr");
  return regnum;
}
