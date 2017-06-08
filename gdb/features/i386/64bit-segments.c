/* THIS FILE IS GENERATED.  -*- buffer-read-only: t -*- vi:set ro:
  Original: 64bit-segments.xml.tmp */

#ifdef GDBSERVER
#include "tdesc.h"
#else
#include "defs.h"
#include "osabi.h"
#include "target-descriptions.h"
#endif

static int
create_feature_i386_64bit_segments (struct target_desc *result, long regnum)
{
  struct tdesc_feature *feature;

  feature = tdesc_create_feature (result, "org.gnu.gdb.i386.segments", "64bit-segments.xml");
  tdesc_create_reg (feature, "fs_base", regnum++, 1, NULL, 64, "int");
  tdesc_create_reg (feature, "gs_base", regnum++, 1, NULL, 64, "int");
  return regnum;
}
