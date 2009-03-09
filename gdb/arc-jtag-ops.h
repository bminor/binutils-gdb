/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Sameer Dhavale <sameer.dhavale@codito.com>
      Soam Vasani <soam.vasani@codito.com>

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/


#define ARC_NR_CORE_REGS        64
#define MAX_ARC700_REG_SIZE     4
#define JTAG_READ_FAILURE 0
#define JTAG_WRITE_FAILURE 0
enum arc_jtag_status
  {
    JTAG_OPENED,
    JTAG_CLOSED, 
    JTAG_RUNNING
  };

struct jtag_ops{
  char *name;
  void (*jtag_open) (void);
  void (*jtag_close) (void);
  int (*jtag_memory_write) (unsigned int, unsigned int *, int);
  int (*jtag_memory_chunk_write) (unsigned int, unsigned int *, int);
  int (*jtag_memory_read) (unsigned int, unsigned int *, int);
  int (*jtag_memory_chunk_read) (unsigned int, unsigned int *, int);
  int (*jtag_read_aux_reg) (unsigned int, unsigned int *);
  int (*jtag_write_aux_reg) (unsigned int, unsigned int);
  int (*jtag_read_core_reg) (unsigned int, unsigned int *);
  int (*jtag_write_core_reg) (unsigned int, unsigned int);
  void (*jtag_wait) (void);
  void (*jtag_reset_board) (void);
  enum arc_jtag_status jtag_status;
  int arc_jtag_state_machine_debug;
};


#define IS_ARC700	(arc_get_architecture() == ARC700)
#define IS_ARC600	(arc_get_architecture() == ARC600)
#define IS_A5		(arc_get_architecture() == A5)
#define IS_A4		(arc_get_architecture() == A4)
