/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: 64bit-cheri128.xml */

#include "gdbsupport/tdesc.h"

static int
create_feature_riscv_64bit_cheri128 (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.riscv.cheri");
  tdesc_create_reg (feature, "cnull", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cra", regnum++, 1, "general", 128, "code_capability");
  tdesc_create_reg (feature, "csp", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cgp", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ctp", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct0", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct1", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct2", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cfp", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs1", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca0", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca1", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca2", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca3", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca4", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca5", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca6", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ca7", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs2", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs3", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs4", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs5", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs6", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs7", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs8", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs9", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs10", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "cs11", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct3", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct4", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct5", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "ct6", regnum++, 1, "general", 128, "data_capability");
  tdesc_create_reg (feature, "pcc", regnum++, 1, "general", 128, "code_capability");
  tdesc_create_reg (feature, "ddc", regnum++, 1, "general", 128, "data_capability");
  return regnum;
}
