/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
   Original: cskyv2-linux.xml */

#include "gdbsupport/tdesc.h"

static void
create_feature_cskyv2_linux (struct target_desc *result)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.csky.abiv2");
  tdesc_create_reg (feature, "r0", 0, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r1", 1, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r2", 2, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r3", 3, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r4", 4, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r5", 5, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r6", 6, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r7", 7, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r8", 8, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r9", 9, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r10", 10, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r11", 11, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r12", 12, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r13", 13, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r14", 14, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "r15", 15, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r16", 16, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r17", 17, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r18", 18, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r19", 19, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r20", 20, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r21", 21, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r22", 22, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r23", 23, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r24", 24, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r25", 25, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r26", 26, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r27", 27, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r28", 28, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r29", 29, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r30", 30, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "r31", 31, 1, NULL, 32, "int");

  tdesc_create_reg (feature, "lo", 36, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "hi", 37, 1, NULL, 32, "int");

  tdesc_create_reg (feature, "fr0", 40, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr1", 41, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr2", 42, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr3", 43, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr4", 44, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr5", 45, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr6", 46, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr7", 47, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr8", 48, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr9", 49, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr10", 50, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr11", 51, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr12", 52, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr13", 53, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr14", 54, 1, NULL, 64, "ieee_double");
  tdesc_create_reg (feature, "fr15", 55, 1, NULL, 64, "ieee_double");

  tdesc_type *element_type;
  element_type = tdesc_named_type (feature, "ieee_single");
  tdesc_create_vector (feature, "v4f", element_type, 4);

  element_type = tdesc_named_type (feature, "ieee_double");
  tdesc_create_vector (feature, "v2d", element_type, 2);

  element_type = tdesc_named_type (feature, "int8");
  tdesc_create_vector (feature, "v16i8", element_type, 16);

  element_type = tdesc_named_type (feature, "int16");
  tdesc_create_vector (feature, "v8i16", element_type, 8);

  element_type = tdesc_named_type (feature, "int32");
  tdesc_create_vector (feature, "v4i32", element_type, 4);

  element_type = tdesc_named_type (feature, "int64");
  tdesc_create_vector (feature, "v2i64", element_type, 2);

  tdesc_type_with_fields *type_with_fields;
  type_with_fields = tdesc_create_union (feature, "vec128");
  tdesc_type *field_type;
  field_type = tdesc_named_type (feature, "v4f");
  tdesc_add_field (type_with_fields, "v4_float", field_type);
  field_type = tdesc_named_type (feature, "v2d");
  tdesc_add_field (type_with_fields, "v2_double", field_type);
  field_type = tdesc_named_type (feature, "v16i8");
  tdesc_add_field (type_with_fields, "v16_int8", field_type);
  field_type = tdesc_named_type (feature, "v8i16");
  tdesc_add_field (type_with_fields, "v8_int16", field_type);
  field_type = tdesc_named_type (feature, "v4i32");
  tdesc_add_field (type_with_fields, "v4_int32", field_type);
  field_type = tdesc_named_type (feature, "v2i64");
  tdesc_add_field (type_with_fields, "v2_int64", field_type);
  field_type = tdesc_named_type (feature, "uint128");
  tdesc_add_field (type_with_fields, "uint128", field_type);


  tdesc_create_reg (feature, "vr0", 56, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr1", 57, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr2", 58, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr3", 59, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr4", 60, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr5", 61, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr6", 62, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr7", 63, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr8", 64, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr9", 65, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr10", 66, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr11", 67, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr12", 68, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr13", 69, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr14", 70, 1, NULL, 128, "vec128");
  tdesc_create_reg (feature, "vr15", 71, 1, NULL, 128, "vec128");

  tdesc_create_reg (feature, "pc", 72, 1, NULL, 32, "code_ptr");
  tdesc_create_reg (feature, "psr", 89, 1, NULL, 32, "int");

  tdesc_create_reg (feature, "fid", 121, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "fcr", 122, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "fesr", 123, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "usp", 127, 1, NULL, 32, "int");
}
