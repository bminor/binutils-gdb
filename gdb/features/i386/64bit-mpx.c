/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: 64bit-mpx.xml */

#include "arch/tdesc.h"

static int
create_feature_i386_64bit_mpx (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.i386.mpx", "64bit-mpx.xml");
  struct tdesc_type *field_type;
  struct tdesc_type *type;
  type = tdesc_create_struct (feature, "br128");
  field_type = tdesc_named_type (feature, "uint64");
  tdesc_add_field (type, "lbound", field_type);
  field_type = tdesc_named_type (feature, "uint64");
  tdesc_add_field (type, "ubound_raw", field_type);

  type = tdesc_create_struct (feature, "_bndstatus");
  tdesc_set_struct_size (type, 8);
  tdesc_add_bitfield (type, "bde", 2, 63);
  tdesc_add_bitfield (type, "error", 0, 1);

  type = tdesc_create_union (feature, "status");
  field_type = tdesc_named_type (feature, "data_ptr");
  tdesc_add_field (type, "raw", field_type);
  field_type = tdesc_named_type (feature, "_bndstatus");
  tdesc_add_field (type, "status", field_type);

  type = tdesc_create_struct (feature, "_bndcfgu");
  tdesc_set_struct_size (type, 8);
  tdesc_add_bitfield (type, "base", 12, 63);
  tdesc_add_bitfield (type, "reserved", 2, 11);
  tdesc_add_bitfield (type, "preserved", 1, 1);
  tdesc_add_bitfield (type, "enabled", 0, 0);

  type = tdesc_create_union (feature, "cfgu");
  field_type = tdesc_named_type (feature, "data_ptr");
  tdesc_add_field (type, "raw", field_type);
  field_type = tdesc_named_type (feature, "_bndcfgu");
  tdesc_add_field (type, "config", field_type);

  tdesc_create_reg (feature, "bnd0raw", regnum++, 1, NULL, 128, "br128");
  tdesc_create_reg (feature, "bnd1raw", regnum++, 1, NULL, 128, "br128");
  tdesc_create_reg (feature, "bnd2raw", regnum++, 1, NULL, 128, "br128");
  tdesc_create_reg (feature, "bnd3raw", regnum++, 1, NULL, 128, "br128");
  tdesc_create_reg (feature, "bndcfgu", regnum++, 1, NULL, 64, "cfgu");
  tdesc_create_reg (feature, "bndstatus", regnum++, 1, NULL, 64, "status");
  return regnum;
}
