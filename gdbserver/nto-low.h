/* Internal interfaces for the QNX Neutrino specific target code for gdbserver.
   Copyright (C) 2009-2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GDBSERVER_NTO_LOW_H
#define GDBSERVER_NTO_LOW_H

struct target_desc;

enum regset_type
{
  NTO_REG_GENERAL,
  NTO_REG_FLOAT,
  NTO_REG_SYSTEM,
  NTO_REG_ALT,
  NTO_REG_END
};

struct nto_target_ops
{
  /* Architecture specific setup.  */
  void (*arch_setup) (void);
  int num_regs;
  int (*register_offset) (int gdbregno);
  const unsigned char *breakpoint;
  int breakpoint_len;
};

extern struct nto_target_ops the_low_target;

/* Target ops definitions for a QNX Neutrino target.  */

class nto_process_target : public process_stratum_target
{
public:

  int create_inferior (const char *program,
		       const std::vector<char *> &program_args) override;

  int attach (unsigned long pid) override;

  int kill (process_info *proc) override;

  int detach (process_info *proc) override;

  void mourn (process_info *proc) override;

  void join (int pid) override;

  bool thread_alive (ptid_t pid) override;

  void resume (thread_resume *resume_info, size_t n) override;

  ptid_t wait (ptid_t ptid, target_waitstatus *status,
	       int options) override;

  void fetch_registers (regcache *regcache, int regno) override;

  void store_registers (regcache *regcache, int regno) override;

  int read_memory (CORE_ADDR memaddr, unsigned char *myaddr,
		   int len) override;

  int write_memory (CORE_ADDR memaddr, const unsigned char *myaddr,
		    int len) override;

  void request_interrupt () override;

  bool supports_read_auxv () override;

  int read_auxv (CORE_ADDR offset, unsigned char *myaddr,
		 unsigned int len) override;

  bool supports_z_point_type (char z_type) override;

  int insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
		    int size, raw_breakpoint *bp) override;

  int remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
		    int size, raw_breakpoint *bp) override;

  bool supports_hardware_single_step () override;

  bool stopped_by_watchpoint () override;

  CORE_ADDR stopped_data_address () override;

  const gdb_byte *sw_breakpoint_from_kind (int kind, int *size) override;
};

/* The inferior's target description.  This is a global because the
   LynxOS ports support neither bi-arch nor multi-process.  */
extern const struct target_desc *nto_tdesc;

#endif /* GDBSERVER_NTO_LOW_H */
