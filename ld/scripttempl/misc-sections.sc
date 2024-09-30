# Copyright (C) 2014-2024 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

# This script fragment gathers together some of the more miscellaneous
# sections that might appear in a linker's input.  It can be invoked
# with a single parameter which is the memory region into which loadable
# sections should be placed.  If the parameter is missing, no memory
# section placement is used.

if test "x$2" = "x" ;
then
  REGION=""
else
  REGION="> $2"
fi

cat <<EOF
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) } 
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }

  .comment 0 (INFO) : { *(.comment); LINKER_VERSION; }

  .gnu.build.attributes : { *(.gnu.build.attributes${RELOCATING+ .gnu.build.attributes.*}) }

  /* Various note sections.  Placed here so that they do not get
     treated as orphan sections.  */
  .note.build-id :      { *(.note.build-id) } ${RELOCATING+${REGION}}
  .note.GNU-stack :     { *(.note.GNU-stack) } ${RELOCATING+${REGION}}
  .note.gnu-property :  { *(.note.gnu-property) } ${RELOCATING+${REGION}}
  .note.ABI-tag :       { *(.note.ABI-tag) } ${RELOCATING+${REGION}}
  .note.package :       { *(.note.package) } ${RELOCATING+${REGION}}
  .note.dlopen :        { *(.note.dlopen) } ${RELOCATING+${REGION}}
  .note.netbsd.ident :  { *(.note.netbsd.ident) } ${RELOCATING+${REGION}}
  .note.openbsd.ident : { *(.note.openbsd.ident) } ${RELOCATING+${REGION}}

EOF
