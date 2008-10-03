/* Record and reverse target for GDB, the GNU debugger.

   Copyright (C) 2008 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _RECORD_H_
#define _RECORD_H_

#define RECORD_IS_USED				(current_target.beneath == &record_ops)
#define RECORD_IS_REPLAY			(record_list->next || record_execdir == EXEC_REVERSE)
#define RECORD_TARGET_SUPPORT_RECORD_WAIT	(record_ops.beneath->to_support_record_wait)

typedef struct record_reg_s
{
  int num;
  gdb_byte *val;
} record_reg_t;

typedef struct record_mem_s
{
  CORE_ADDR addr;
  int len;
  gdb_byte *val;
} record_mem_t;

enum record_type
{
  record_end = 0,
  record_reg,
  record_mem
};

/* This is the core struct of record function.
   An entity of record_t is a record of the value change of a register
   ("record_reg") or a part of memory ("record_mem"). And Each instruction must
   has a record_t ("record_end") that point out this is the last record_t of
   this instruction.
   Each record_t is linked to "record_list" by "prev" and "next". 
 */
typedef struct record_s
{
  struct record_s *prev;
  struct record_s *next;
  enum record_type type;
  union
  {
    /* reg */
    record_reg_t reg;
    /* mem */
    record_mem_t mem;
    /* end */
    int need_dasm;
  } u;
} record_t;

extern int record_debug;
extern record_t *record_list;
extern record_t *record_arch_list_head;
extern record_t *record_arch_list_tail;
extern struct regcache *record_regcache;

extern struct target_ops record_ops;
extern int record_resume_step;
extern int record_regcache_raw_write_regnum;
extern enum exec_direction_kind record_execdir;

extern int record_arch_list_add_reg (int num);
extern int record_arch_list_add_mem (CORE_ADDR addr, int len);
extern int record_arch_list_add_end (int need_dasm);
extern void record_message (struct gdbarch *gdbarch);
extern void record_not_record_set (void);

extern void (*record_beneath_to_resume) (ptid_t, int, enum target_signal);
extern ptid_t (*record_beneath_to_wait) (ptid_t, struct target_waitstatus *);
extern void (*record_beneath_to_prepare_to_store) (struct regcache *);
extern LONGEST (*record_beneath_to_xfer_partial) (struct target_ops * ops,
						  enum target_object object,
						  const char *annex,
						  gdb_byte * readbuf,
						  const gdb_byte * writebuf,
						  ULONGEST offset,
						  LONGEST len);
extern int (*record_beneath_to_insert_breakpoint) (struct bp_target_info *);
extern int (*record_beneath_to_remove_breakpoint) (struct bp_target_info *);

#endif /* _RECORD_H_ */
