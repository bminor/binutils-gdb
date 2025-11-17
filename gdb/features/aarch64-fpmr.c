/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: aarch64-fpmr.xml */

#include "gdbsupport/tdesc.h"

static int
create_feature_aarch64_fpmr (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.aarch64.fpmr");
  tdesc_type_with_fields *type_with_fields;
  type_with_fields = tdesc_create_enum (feature, "fp8_fmt", 3);
  tdesc_add_enum_value (type_with_fields, 0, "E5M2");
  tdesc_add_enum_value (type_with_fields, 1, "E4M3");

  type_with_fields = tdesc_create_enum (feature, "osc", 1);
  tdesc_add_enum_value (type_with_fields, 0, "Inf/NaN");
  tdesc_add_enum_value (type_with_fields, 1, "MaxNormal");

  type_with_fields = tdesc_create_enum (feature, "osm", 1);
  tdesc_add_enum_value (type_with_fields, 0, "Inf");
  tdesc_add_enum_value (type_with_fields, 1, "MaxNormal");

  type_with_fields = tdesc_create_flags (feature, "fpmr_flags", 8);
  tdesc_type *field_type;
  field_type = tdesc_named_type (feature, "fp8_fmt");
  tdesc_add_typed_bitfield (type_with_fields, "F8S1", 0, 2, field_type);
  field_type = tdesc_named_type (feature, "fp8_fmt");
  tdesc_add_typed_bitfield (type_with_fields, "F8S2", 3, 5, field_type);
  field_type = tdesc_named_type (feature, "fp8_fmt");
  tdesc_add_typed_bitfield (type_with_fields, "F8D", 6, 8, field_type);
  field_type = tdesc_named_type (feature, "osm");
  tdesc_add_typed_bitfield (type_with_fields, "OSM", 14, 14, field_type);
  field_type = tdesc_named_type (feature, "osc");
  tdesc_add_typed_bitfield (type_with_fields, "OSC", 15, 15, field_type);
  tdesc_add_bitfield (type_with_fields, "LSCALE", 16, 22);
  field_type = tdesc_named_type (feature, "int8");
  tdesc_add_typed_bitfield (type_with_fields, "NSCALE", 24, 31, field_type);
  tdesc_add_bitfield (type_with_fields, "LSCALE2", 32, 37);

  tdesc_create_reg (feature, "fpmr", regnum++, 1, NULL, 64, "fpmr_flags");
  return regnum;
}
