/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: 32bit-linux.xml */

#include "gdbsupport/tdesc.h"

static int
create_feature_i386_32bit_linux (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.i386.linux");
  tdesc_type *element_type;
  element_type = tdesc_named_type (feature, "uint32");
  tdesc_create_vector (feature, "i386_tls_gdt_reg", element_type, 4);

  regnum = 41;
  tdesc_create_reg (feature, "orig_eax", regnum++, 1, NULL, 32, "int");
  tdesc_create_reg (feature, "i386_tls_gdt_0", regnum++, 1, "system", 128, "i386_tls_gdt_reg");
  tdesc_create_reg (feature, "i386_tls_gdt_1", regnum++, 1, "system", 128, "i386_tls_gdt_reg");
  tdesc_create_reg (feature, "i386_tls_gdt_2", regnum++, 1, "system", 128, "i386_tls_gdt_reg");
  return regnum;
}
