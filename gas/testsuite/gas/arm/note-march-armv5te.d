# name: armv5te note with -march=armv5te
# source: note-march-armv5te.s RUN_OBJCOPY
# as: -march=armv5te
# objcopy_objects: -R .ARM.attributes
# ld: -e 0x10000
# objcopy_linked_file:
# readelf: -p .note.gnu.arm.ident
# This test is only valid on EABI based ports.
# target: *-*-*eabi*

String dump of section '.note.gnu.arm.ident':
  \[\s*[0-9a-f]+\]  arch: 
  \[\s*[0-9a-f]+\]  armv5te
