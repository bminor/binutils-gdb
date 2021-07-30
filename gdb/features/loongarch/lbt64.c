/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: lbt64.xml */

#include "gdbsupport/tdesc.h"

static int
create_feature_loongarch_lbt64 (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.loongarch.lbt");
  tdesc_create_reg (feature, "scr0", regnum++, 1, "lbt", 64, "uint64");
  tdesc_create_reg (feature, "scr1", regnum++, 1, "lbt", 64, "uint64");
  tdesc_create_reg (feature, "scr2", regnum++, 1, "lbt", 64, "uint64");
  tdesc_create_reg (feature, "scr3", regnum++, 1, "lbt", 64, "uint64");
  tdesc_create_reg (feature, "EFLAG", regnum++, 1, "lbt", 32, "uint32");
  tdesc_create_reg (feature, "x86_top", regnum++, 1, "lbt", 8, "uint8");
  return regnum;
}
