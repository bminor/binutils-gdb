#include <stdio.h>
#include "ansidecl.h"
#include "bfd.h"

typedef int (*fprintf_ftype) PARAMS((FILE*, const char*, ...));

typedef struct disassemble_info {
  fprintf_ftype fprintf_func;
  FILE *stream;
  int flags;
} disassemble_info;

typedef int (*disassembler_ftype)
     PARAMS((bfd_vma, bfd_byte *, disassemble_info *));

#define INIT_DISASSEMBLE_INFO(INFO, STREAM) \
  INFO.fprintf_func = (fprintf_ftype)fprintf, \
  INFO.stream = (STREAM)

#define GDB_INIT_DISASSEMBLE_INFO(INFO, STREAM) \
  INFO.fprintf_func = (fprintf_ftype)fprintf_filtered, \
  INFO.stream = (STREAM)

/* Standard dis-assemblers. */

extern int print_insn_big_mips PARAMS ((bfd_vma, bfd_byte*,disassemble_info*));
extern int print_insn_little_mips
  PARAMS ((bfd_vma,bfd_byte*,disassemble_info*));
extern int print_insn_i386 PARAMS ((bfd_vma,bfd_byte*,disassemble_info*));
extern int print_insn_m68k PARAMS ((bfd_vma,bfd_byte*,disassemble_info*));
extern int print_insn_z8001 PARAMS ((bfd_vma,bfd_byte*,disassemble_info*));
extern int print_insn_z8002 PARAMS ((bfd_vma,bfd_byte*,disassemble_info*));
