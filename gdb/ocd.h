/* Definitions for the Macraigor Systems BDM Wiggler
   Copyright 1996, 1997 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef WIGGLER_H
#define WIGGLER_H

/* Wiggler serial protocol definitions */

#define DLE 020			/* Quote char */
#define SYN 026			/* Start of packet */
#define RAW_SYN ((026 << 8) | 026) /* get_quoted_char found a naked SYN */

/* Status flags */

#define WIGGLER_FLAG_RESET 0x01 /* Target is being reset */
#define WIGGLER_FLAG_STOPPED 0x02 /* Target is halted */
#define WIGGLER_FLAG_BDM 0x04	/* Target is in BDM */
#define WIGGLER_FLAG_PWF 0x08	/* Power failed */
#define WIGGLER_FLAG_CABLE_DISC 0x10 /* BDM cable disconnected */

/* Commands */

#define WIGGLER_AYT 0x0		/* Are you there? */
#define WIGGLER_GET_VERSION 0x1	/* Get Version */
#define WIGGLER_SET_BAUD_RATE 0x2 /* Set Baud Rate */
#define WIGGLER_INIT 0x10	/* Initialize Wiggler */
#define WIGGLER_SET_SPEED 0x11	/* Set Speed */
#define WIGGLER_GET_STATUS_MASK 0x12 /* Get Status Mask */
#define WIGGLER_GET_CTRS 0x13	/* Get Error Counters */
#define WIGGLER_SET_FUNC_CODE 0x14 /* Set Function Code */
#define WIGGLER_SET_CTL_FLAGS 0x15 /* Set Control Flags */
#define WIGGLER_SET_BUF_ADDR 0x16 /* Set Register Buffer Address */
#define WIGGLER_RUN 0x20	/* Run Target from PC */
#define WIGGLER_RUN_ADDR 0x21	/* Run Target from Specified Address */
#define WIGGLER_STOP 0x22	/* Stop Target */
#define WIGGLER_RESET_RUN 0x23	/* Reset Target and Run */
#define WIGGLER_RESET 0x24	/* Reset Target and Halt */
#define WIGGLER_STEP 0x25	/* Single step */
#define WIGGLER_READ_REGS 0x30	/* Read Registers */
#define WIGGLER_WRITE_REGS 0x31	/* Write Registers */
#define WIGGLER_READ_MEM 0x32	/* Read Memory */
#define WIGGLER_WRITE_MEM 0x33	/* Write Memory */
#define WIGGLER_FILL_MEM 0x34	/* Fill Memory */
#define WIGGLER_MOVE_MEM 0x35	/* Move Memory */

#define WIGGLER_READ_INT_MEM 0x80 /* Read Internal Memory */
#define WIGGLER_WRITE_INT_MEM 0x81 /* Write Internal Memory */
#define WIGGLER_JUMP 0x82	/* Jump to Subroutine */

#define WIGGLER_ERASE_FLASH 0x90 /* Erase flash memory */
#define WIGGLER_PROGRAM_FLASH 0x91 /* Write flash memory */
#define WIGGLER_EXIT_MON 0x93	/* Exit the flash programming monitor  */
#define WIGGLER_ENTER_MON 0x94	/* Enter the flash programming monitor  */

#define WIGGLER_SET_STATUS 0x0a	/* Set status */
#define   WIGGLER_FLAG_STOP 0x0 /* Stop the target, enter BDM */
#define   WIGGLER_FLAG_START 0x01 /* Start the target at PC */
#define   WIGGLER_FLAG_RETURN_STATUS 0x04 /* Return async status */

/* Target type (for WIGGLER_INIT command) */

enum wiggler_target_type {
  WIGGLER_TARGET_CPU32=0x0,	/* Moto cpu32 family */
  WIGGLER_TARGET_CPU16=0x1,
  WIGGLER_TARGET_MOTO_PPC=0x2,	/* Motorola PPC 5xx/8xx */
  WIGGLER_TARGET_IBM_PPC=0x3};	/* IBM PPC 4xx */

void wiggler_open PARAMS ((char *name, int from_tty, enum wiggler_target_type,
			   struct target_ops *ops));

void wiggler_close PARAMS ((int quitting));

void wiggler_detach PARAMS ((char *args, int from_tty));

void wiggler_resume PARAMS ((int pid, int step, enum target_signal siggnal));

void wiggler_prepare_to_store PARAMS ((void));

void wiggler_stop PARAMS ((void));

void wiggler_files_info PARAMS ((struct target_ops *ignore));


int wiggler_xfer_memory PARAMS ((CORE_ADDR memaddr, char *myaddr,
				 int len, int should_write,
				 struct target_ops *target));

void wiggler_mourn PARAMS ((void));

void wiggler_create_inferior PARAMS ((char *exec_file,
				      char *args,
				      char **env));

int wiggler_thread_alive PARAMS ((int th));

void wiggler_error PARAMS ((char *s, int error_code));

void wiggler_kill PARAMS ((void));

void wiggler_load PARAMS((char *args, int from_tty));

unsigned char * wiggler_read_bdm_registers PARAMS ((int first_bdm_regno,
						    int last_bdm_regno,
						    int *reglen));

CORE_ADDR wiggler_read_bdm_register PARAMS ((int bdm_regno));

void wiggler_write_bdm_registers PARAMS ((int first_bdm_regno,
					  unsigned char *regptr,
					  int reglen));

void wiggler_write_bdm_register PARAMS ((int bdm_regno, CORE_ADDR reg));

int wiggler_wait PARAMS ((void));

#endif /* WIGGLER_H */
