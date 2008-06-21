/*
 * remote-breakpoint -- interface
 */

#ifndef REMOTE_BREAKPOINT_H
#define REMOTE_BREAKPOINT_H

typedef struct BREAKPOINT {
  unsigned long addr;
  unsigned long len;
  struct BREAKPOINT *next;
} breakpoint;

enum breakpoint_type {
  SOFTWARE_BP,
  HARDWARE_BP,
  WRITE_BP,
  READ_BP,
  ACCESS_BP
};

enum successcode {
  FAIL = 0,
  PASS = 1
};

enum direction_code {
  DIR_FORWARD = 0,
  DIR_BACKWARD
};

extern enum successcode remote_remove_breakpoint (enum breakpoint_type,
						  unsigned long,
						  unsigned long);

extern enum successcode remote_set_breakpoint    (enum breakpoint_type,
						  unsigned long,
						  unsigned long);

extern enum successcode remote_breakpoint_here_p (enum breakpoint_type,
						  unsigned long);

#endif

