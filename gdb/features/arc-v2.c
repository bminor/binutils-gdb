/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: arc-v2.xml */

#include "defs.h"
#include "osabi.h"
#include "target-descriptions.h"

struct target_desc *tdesc_arc_v2;
static void
initialize_tdesc_arc_v2 (void)
{
  struct target_desc *result = allocate_target_description ();
  struct tdesc_feature *feature;
  struct tdesc_type *field_type;
  struct tdesc_type *type;

  set_tdesc_architecture (result, bfd_scan_arch ("ARCv2"));

  feature = tdesc_create_feature (result, "org.gnu.gdb.arc.core.v2");
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
  tdesc_create_reg (feature, "r14", 14, 1, NULL, 32, "int");
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
  tdesc_create_reg (feature, "gp", 26, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "fp", 27, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "sp", 28, 1, NULL, 32, "data_ptr");
  tdesc_create_reg (feature, "ilink", 29, 1, NULL, 32, "code_ptr");
  tdesc_create_reg (feature, "r30", 30, 1, "", 32, "int");
  tdesc_create_reg (feature, "blink", 31, 1, NULL, 32, "code_ptr");
  tdesc_create_reg (feature, "lp_count", 32, 1, NULL, 32, "uint32");
  tdesc_create_reg (feature, "pcl", 33, 1, "", 32, "code_ptr");

  feature = tdesc_create_feature (result, "org.gnu.gdb.arc.aux-minimal");
  type = tdesc_create_flags (feature, "status32_type", 4);
  tdesc_add_bitfield (type, "H", 0, 0);
  tdesc_add_bitfield (type, "E", 1, 4);
  tdesc_add_bitfield (type, "AE", 5, 5);
  tdesc_add_bitfield (type, "DE", 6, 6);
  tdesc_add_bitfield (type, "U", 7, 7);
  tdesc_add_bitfield (type, "V", 8, 8);
  tdesc_add_bitfield (type, "C", 9, 9);
  tdesc_add_bitfield (type, "N", 10, 10);
  tdesc_add_bitfield (type, "Z", 11, 11);
  tdesc_add_bitfield (type, "L", 12, 12);
  tdesc_add_bitfield (type, "DZ", 13, 13);
  tdesc_add_bitfield (type, "SC", 14, 14);
  tdesc_add_bitfield (type, "ES", 15, 15);
  tdesc_add_bitfield (type, "RB", 16, 18);
  tdesc_add_bitfield (type, "AD", 19, 19);
  tdesc_add_bitfield (type, "US", 20, 20);
  tdesc_add_bitfield (type, "IE", 31, 31);

  tdesc_create_reg (feature, "pc", 34, 1, NULL, 32, "code_ptr");
  tdesc_create_reg (feature, "status32", 35, 1, NULL, 32, "status32_type");

  tdesc_arc_v2 = result;
}
