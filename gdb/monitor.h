/* Remote debugging interface for ROM68K boot monitor, for GDB.
 *  Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
 *  Contributed by Cygnus Support. Written by Rob Savoye for Cygnus.
 *
 *  Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
 *  Contributed by Cygnus Support. Written by Rob Savoye for Cygnus.
 *
 * This file is part of GDB.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

struct monitor_ops {
  char	*execute;		/* execute or usually GO command */
  char	*resume;		/* continue command */
  char	*step;			/* single step */
  char	*set_break;		/* set a breakpoint */
  char	*clr_break;		/* clear a breakpoint */
  char	*set_mem;		/* set memory to a value */
  char	*dis_mem;		/* display memory */
  char	*mem_prompt;		/* prompt memory commands use */
  char	*set_reg;		/* set a register */
  char	*reg_delim;		/* delimiter between registers */
  char	*get_reg;		/* read a register */
  char	*load;			/* download command */
  char	*prompt;		/* monitor command prompt */
  char	*cmd_delim;		/* end-of-command delimitor */
  char	*cmd_end;		/* optional command terminator */
};

extern struct monitor_ops        *current_monitor;

#define GO_CMD 			(current_monitor->execute)
#define CONT_CMD		(current_monitor->resume)
#define STEP_CMD		(current_monitor->step)
#define SET_BREAK_CMD		(current_monitor->set_break)
#define CLR_BREAK_CMD		(current_monitor->clr_break)
#define MEM_SET_CMD		(current_monitor->set_mem)
#define MEM_DIS_CMD		(current_monitor->dis_mem)
#define LOAD_CMD		(current_monitor->load)
#define GET_REG			(current_monitor->get_reg)
#define REG_DELIM		(current_monitor->reg_delim)
#define SET_REG			(current_monitor->set_reg)
#define CMD_END			(current_monitor->cmd_end)
#define CMD_DELIM		(current_monitor->cmd_delim)
#define PROMPT			(current_monitor->prompt)
#define MEM_PROMPT		(current_monitor->mem_prompt)

#define push_monitor(x)		current_monitor = x;
				 
