/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: aarch64-capability.xml */

#include "gdbsupport/tdesc.h"

static int
create_feature_aarch64_capability (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.aarch64.capability");

  tdesc_create_reg (feature, "c0", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c1", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c2", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c3", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c4", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c5", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c6", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c7", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c8", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c9", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c10", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c11", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c12", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c13", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c14", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c15", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c16", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c17", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c18", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c19", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c20", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c21", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c22", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c23", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c24", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c25", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c26", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c27", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c28", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c29", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "c30", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "csp", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "pcc", regnum++, 1, "general", 128, "uintcap");
  tdesc_create_reg (feature, "ddc", regnum++, 1, "system", 128, "uintcap");
  tdesc_create_reg (feature, "ctpidr", regnum++, 1, "system", 128, "uintcap");
  tdesc_create_reg (feature, "rcsp", regnum++, 1, "system", 128, "uintcap");
  tdesc_create_reg (feature, "rddc", regnum++, 1, "system", 128, "uintcap");
  tdesc_create_reg (feature, "rctpidr", regnum++, 1, "system", 128, "uintcap");
  tdesc_create_reg (feature, "cid", regnum++, 1, "system", 128, "uintcap");
  tdesc_create_reg (feature, "tag_map", regnum++, 1, "system", 64, "uint64");
  tdesc_create_reg (feature, "cctlr", regnum++, 1, "system", 64, "uint64");
  return regnum;
}
