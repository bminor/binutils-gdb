#ifndef AS_DWARF2DBG_H
#define AS_DWARF2DBG_H

#include "as.h"

#define DWARF2_FLAG_BEGIN_STMT	(1 << 0)	/* beginning of statement */
#define DWARF2_FLAG_BEGIN_BLOCK	(1 << 1)	/* beginning of basic block */

struct dwarf2_line_info
  {
    char *filename;
    unsigned int filenum;
    unsigned int line;
    unsigned int column;
    unsigned int flags;
  };

/* Implements the .file FILENO "FILENAME" directive.  FILENO can be 0
   to indicate that no file number has been assigned.  All real file
   number must be >0.  */
extern void dwarf2_directive_file (int dummy);

/* Implements the .loc FILENO LINENO [COLUMN] directive.  FILENO is
   the file number, LINENO the line number and the (optional) COLUMN
   the column of the source code that the following instruction
   corresponds to.  FILENO can be 0 to indicate that the filename
   specified by the textually most recent .file directive should be
   used.  */
extern void dwarf2_directive_loc (int dummy);

/* Returns the current source information.  If .file directives have
   been encountered, the info for the corresponding source file is
   returned.  Otherwise, the info for the assembly source file is
   returned.  */
extern void dwarf2_where (struct dwarf2_line_info *l);

/* This function generates .debug_line info based on the address and
   source information passed in the arguments.  ADDR should be the
   frag-relative offset of the instruction the information is for and
   L is the source information that should be associated with that
   address. */
extern void dwarf2_gen_line_info (bfd_vma addr, struct dwarf2_line_info *l);

/* Must be called after all other input is processed to finish up the
   .debug_line section.  */
extern void dwarf2_finish (void);

#endif /* AS_DWARF2DBG_H */
