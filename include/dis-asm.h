#include <stdio.h>
#include "ansidecl.h"
#include "bfd.h"

typedef int (*fprintf_ftype) PARAMS((FILE*, const char*, ...));

typedef struct disassemble_info {
  fprintf_ftype fprintf_func;
  FILE *stream;

  /* For use by the disassembler.  */
  int flags;
  PTR private_data;

  /* Function used to get bytes to disassemble.  MEMADDR is the
     address of the stuff to be disassembled, MYADDR is the address to
     put the bytes in, and LENGTH is the number of bytes to read.
     INFO is a pointer to this struct.
     Returns an errno value or 0 for success.  */
  int (*read_memory_func)
    PARAMS ((bfd_vma memaddr, bfd_byte *myaddr, int length,
	     struct disassemble_info *info));

  /* Function which should be called if we get an error that we can't
     recover from.  STATUS is the errno value from read_memory_func and
     MEMADDR is the address that we were trying to read.  INFO is a
     pointer to this struct.  */
  void (*memory_error_func)
    PARAMS ((int status, bfd_vma memaddr, struct disassemble_info *info));

  /* These are for buffer_read_memory.  */
  bfd_byte *buffer;
  bfd_vma buffer_vma;
  int buffer_length;
} disassemble_info;

/* Here is a function which callers may wish to use for read_memory_func.
   It gets bytes from a buffer.  */
extern int buffer_read_memory
  PARAMS ((bfd_vma, bfd_byte *, int, struct disassemble_info *));

/* This function goes with buffer_read_memory.
   It prints a message using info->fprintf_func and info->stream.  */
extern void perror_memory PARAMS ((int, bfd_vma, struct disassemble_info *));

#define INIT_DISASSEMBLE_INFO(INFO, STREAM) \
  (INFO).fprintf_func = (fprintf_ftype)fprintf, \
  (INFO).stream = (STREAM), \
  (INFO).buffer = NULL, \
  (INFO).buffer_vma = 0, \
  (INFO).buffer_length = 0, \
  (INFO).read_memory_func = buffer_read_memory, \
  (INFO).memory_error_func = perror_memory

/* GDB--Like target_read_memory, but slightly different parameters.  */
extern int
dis_asm_read_memory PARAMS ((bfd_vma memaddr, bfd_byte *myaddr, int len,
			     disassemble_info *info));

/* GDB--Like memory_error with slightly different parameters.  */
extern void
dis_asm_memory_error
  PARAMS ((int status, bfd_vma memaddr, disassemble_info *info));

#define GDB_INIT_DISASSEMBLE_INFO(INFO, STREAM) \
  (INFO).fprintf_func = (fprintf_ftype)fprintf_filtered, \
  (INFO).stream = (STREAM), \
  (INFO).read_memory_func = dis_asm_read_memory, \
  (INFO).memory_error_func = dis_asm_memory_error


/* Standard disassemblers.  Disassemble one instruction at the given
   target address.  Return number of bytes processed.  */
typedef int (*disassembler_ftype)
     PARAMS((bfd_vma, disassemble_info *));

extern int print_insn_big_mips PARAMS ((bfd_vma, disassemble_info*));
extern int print_insn_little_mips PARAMS ((bfd_vma,disassemble_info*));
extern int print_insn_i386 PARAMS ((bfd_vma,disassemble_info*));
extern int print_insn_m68k PARAMS ((bfd_vma,disassemble_info*));
extern int print_insn_z8001 PARAMS ((bfd_vma,disassemble_info*));
extern int print_insn_z8002 PARAMS ((bfd_vma,disassemble_info*));
extern int print_insn_h8500 PARAMS ((bfd_vma,disassemble_info*));
extern int print_insn_sparc PARAMS ((bfd_vma,disassemble_info*));
