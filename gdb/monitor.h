/* Remote debugging interface ROM monitors.
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

struct rom_cmd_data {
  char *cmd;			/* command to send */
  char *delim;			/* the delimiter */
  char *result;			/* the result */
};

struct monitor_ops {
  int	type;			/* 1 is ascii, 0 is GDB remote protocol */
  char  *init;			/* initialize to the monitor */
  char	*execute;		/* execute or usually GO command */
  char	*resume;		/* continue command */
  char	*step;			/* single step */
  char	*set_break;		/* set a breakpoint */
  char	*clr_break;		/* clear a breakpoint */
  int	clr_type;		/* number or address for clearing */
  struct rom_cmd_data setmem;	/* set memory to a value */
  struct rom_cmd_data getmem;	/* display memory */
  struct rom_cmd_data regset;	/* set a register */
  struct rom_cmd_data regget;	/* read a register */
  char	*load;			/* load command */
  char	*prompt;		/* monitor command prompt */
  char	*cmd_delim;		/* end-of-command delimitor */
  char	*cmd_end;		/* optional command terminator */
  struct target_ops *target;	/* target operations */
  char	*loadtypes;		/* the load types that are supported */
  char	*loadprotos;		/* the load protocols that are supported */
  char	*baudrates;		/* supported baud rates */
  int	stopbits;		/* number of stop bits */
  char  **regnames;		/* array of register names in ascii */
};

extern struct monitor_ops        *current_monitor;

#define PROTO_TYPE		(current_monitor->type)
#define LOADTYPES		(current_monitor->loadtypes)
#define LOADPROTOS		(current_monitor->loadprotos)
#define INIT_CMD 		(current_monitor->init)
#define GO_CMD 			(current_monitor->execute)
#define CONT_CMD		(current_monitor->resume)
#define STEP_CMD		(current_monitor->step)
#define SET_BREAK_CMD		(current_monitor->set_break)
#define CLR_BREAK_CMD		(current_monitor->clr_break)
#define CLR_BREAK_ADDR		(current_monitor->clr_type)
#define SET_MEM			(current_monitor->setmem)
#define GET_MEM			(current_monitor->getmem)
#define LOAD_CMD		(current_monitor->load)
#define GET_REG			(current_monitor->regget)
#define SET_REG			(current_monitor->regset)
#define CMD_END			(current_monitor->cmd_end)
#define CMD_DELIM		(current_monitor->cmd_delim)
#define PROMPT			(current_monitor->prompt)
#define TARGET_OPS		(current_monitor->target)
#define TARGET_NAME		(current_monitor->target->to_shortname)
#define BAUDRATES		(current_monitor->baudrates)
#define STOPBITS		(current_monitor->stopbits)
#define REGNAMES(x)		(current_monitor->regnames[x])
#define ROMCMD(x)		(x.cmd)
#define ROMDELIM(x)		(x.delim)
#define ROMRES(x)		(x.result)

#define push_monitor(x)		current_monitor = x;

#define SREC_SIZE 160
#define GDBPROTO		((current_monitor->type) ? 0: 1)

extern void debuglogs();
extern void monitor_open();
extern void monitor_close();
extern void monitor_detach();
extern void monitor_attach();
extern void monitor_resume();
extern int  monitor_wait();
extern void monitor_fetch_register();
extern void monitor_store_register();
extern void monitor_fetch_registers();
extern void monitor_store_registers();
extern void monitor_prepare_to_store();
extern int  monitor_xfer_inferior_memory();
extern void monitor_files_info();
extern int  monitor_insert_breakpoint();
extern int  monitor_remove_breakpoint();
extern void monitor_kill();
extern void monitor_load();
extern void monitor_create_inferior();
extern void monitor_mourn_inferior();

/*
 * FIXME: These are to temporarily maintain compatability with the
 *	old monitor structure till remote-mon.c is fixed to work
 *	like the *-rom.c files.
 */
#define MEM_PROMPT		(current_monitor->loadtypes)
#define MEM_SET_CMD		(current_monitor->setmem)
#define MEM_DIS_CMD		(current_monitor->getmem)
#define REG_DELIM               (current_monitor->regset.delim)
