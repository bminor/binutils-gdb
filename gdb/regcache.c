/* Cache and manage the values of registers for GDB, the GNU debugger.
   Copyright 1986, 87, 89, 91, 94, 95, 96, 1998, 2000, 2001
   Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "target.h"
#include "gdbarch.h"
#include "gdbcmd.h"

/*
 * DATA STRUCTURE
 *
 * Here is the actual register cache.
 */

/* NOTE: this is a write-through cache.  There is no "dirty" bit for
   recording if the register values have been changed (eg. by the
   user).  Therefore all registers must be written back to the
   target when appropriate.  */

/* REGISTERS contains the cached register values (in target byte order). */

char *registers;

/* REGISTER_VALID is 0 if the register needs to be fetched,
                     1 if it has been fetched, and
		    -1 if the register value was not available.  
   "Not available" means don't try to fetch it again.  */

signed char *register_valid;

/* The thread/process associated with the current set of registers.
   For now, -1 is special, and means `no current process'.  */

static int registers_pid = -1;

/*
 * FUNCTIONS:
 */

/* REGISTER_CACHED()

   Returns 0 if the value is not in the cache (needs fetch).
          >0 if the value is in the cache.
	  <0 if the value is permanently unavailable (don't ask again).  */

int
register_cached (int regnum)
{
  return register_valid[regnum];
}

/* Record that REGNUM's value is cached if STATE is >0, uncached but
   fetchable if STATE is 0, and uncached and unfetchable if STATE is <0.  */

void
set_register_cached (int regnum, int state)
{
  register_valid[regnum] = state;
}

/* REGISTER_CHANGED

   invalidate a single register REGNUM in the cache */
void
register_changed (int regnum)
{
  set_register_cached (regnum, 0);
}

/* If REGNUM >= 0, return a pointer to register REGNUM's cache buffer area,
   else return a pointer to the start of the cache buffer.  */

char *
register_buffer (int regnum)
{
  if (regnum < 0)
    return registers;
  else
    return &registers[REGISTER_BYTE (regnum)];
}

/* Return whether register REGNUM is a real register.  */

static int
real_register (int regnum)
{
  return regnum >= 0 && regnum < NUM_REGS;
}

/* Return whether register REGNUM is a pseudo register.  */

static int
pseudo_register (int regnum)
{
  return regnum >= NUM_REGS && regnum < NUM_REGS + NUM_PSEUDO_REGS;
}

/* Fetch register REGNUM into the cache.  */

static void
fetch_register (int regnum)
{
  if (real_register (regnum))
    target_fetch_registers (regnum);
  else if (pseudo_register (regnum))
    FETCH_PSEUDO_REGISTER (regnum);
}

/* Write register REGNUM cached value to the target.  */

static void
store_register (int regnum)
{
  if (real_register (regnum))
    target_store_registers (regnum);
  else if (pseudo_register (regnum))
    STORE_PSEUDO_REGISTER (regnum);
}

/* Low level examining and depositing of registers.

   The caller is responsible for making sure that the inferior is
   stopped before calling the fetching routines, or it will get
   garbage.  (a change from GDB version 3, in which the caller got the
   value from the last stop).  */

/* REGISTERS_CHANGED ()

   Indicate that registers may have changed, so invalidate the cache.  */

void
registers_changed (void)
{
  int i;

  registers_pid = -1;

  /* Force cleanup of any alloca areas if using C alloca instead of
     a builtin alloca.  This particular call is used to clean up
     areas allocated by low level target code which may build up
     during lengthy interactions between gdb and the target before
     gdb gives control to the user (ie watchpoints).  */
  alloca (0);

  for (i = 0; i < ARCH_NUM_REGS; i++)
    set_register_cached (i, 0);

  /* Assume that if all the hardware regs have changed, 
     then so have the pseudo-registers.  */
  for (i = NUM_REGS; i < NUM_REGS + NUM_PSEUDO_REGS; i++)
    set_register_cached (i, 0);

  if (registers_changed_hook)
    registers_changed_hook ();
}

/* REGISTERS_FETCHED ()

   Indicate that all registers have been fetched, so mark them all valid.  */


void
registers_fetched (void)
{
  int i;

  for (i = 0; i < ARCH_NUM_REGS; i++)
    set_register_cached (i, 1);
  /* Do not assume that the pseudo-regs have also been fetched.
     Fetching all real regs might not account for all pseudo-regs.  */
}

/* read_register_bytes and write_register_bytes are generally a *BAD*
   idea.  They are inefficient because they need to check for partial
   updates, which can only be done by scanning through all of the
   registers and seeing if the bytes that are being read/written fall
   inside of an invalid register.  [The main reason this is necessary
   is that register sizes can vary, so a simple index won't suffice.]
   It is far better to call read_register_gen and write_register_gen
   if you want to get at the raw register contents, as it only takes a
   regnum as an argument, and therefore can't do a partial register
   update.

   Prior to the recent fixes to check for partial updates, both read
   and write_register_bytes always checked to see if any registers
   were stale, and then called target_fetch_registers (-1) to update
   the whole set.  This caused really slowed things down for remote
   targets.  */

/* Copy INLEN bytes of consecutive data from registers
   starting with the INREGBYTE'th byte of register data
   into memory at MYADDR.  */

void
read_register_bytes (int inregbyte, char *myaddr, int inlen)
{
  int inregend = inregbyte + inlen;
  int regnum;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  /* See if we are trying to read bytes from out-of-date registers.  If so,
     update just those registers.  */

  for (regnum = 0; regnum < NUM_REGS + NUM_PSEUDO_REGS; regnum++)
    {
      int regstart, regend;

      if (register_cached (regnum))
	continue;

      if (REGISTER_NAME (regnum) == NULL || *REGISTER_NAME (regnum) == '\0')
	continue;

      regstart = REGISTER_BYTE (regnum);
      regend = regstart + REGISTER_RAW_SIZE (regnum);

      if (regend <= inregbyte || inregend <= regstart)
	/* The range the user wants to read doesn't overlap with regnum.  */
	continue;

      /* We've found an uncached register where at least one byte will be read.
         Update it from the target.  */
      fetch_register (regnum);

      if (!register_cached (regnum))
	{
	  /* Sometimes pseudoregs are never marked valid, so that they 
	     will be fetched every time (it can be complicated to know
	     if a pseudoreg is valid, while "fetching" them can be cheap). 
	     */
	  if (regnum < NUM_REGS)
 	    error ("read_register_bytes:  Couldn't update register %d.", regnum);
	}
    }

  if (myaddr != NULL)
    memcpy (myaddr, register_buffer (-1) + inregbyte, inlen);
}

/* Read register REGNUM into memory at MYADDR, which must be large
   enough for REGISTER_RAW_BYTES (REGNUM).  Target byte-order.  If the
   register is known to be the size of a CORE_ADDR or smaller,
   read_register can be used instead.  */

void
read_register_gen (int regnum, char *myaddr)
{
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  if (!register_cached (regnum))
    fetch_register (regnum);

  memcpy (myaddr, register_buffer (regnum),
	  REGISTER_RAW_SIZE (regnum));
}

/* Write register REGNUM at MYADDR to the target.  MYADDR points at
   REGISTER_RAW_BYTES(REGNUM), which must be in target byte-order.  */

/* Registers we shouldn't try to store.  */
#if !defined (CANNOT_STORE_REGISTER)
#define CANNOT_STORE_REGISTER(regnum) 0
#endif

void
write_register_gen (int regnum, char *myaddr)
{
  int size;

  /* On the sparc, writing %g0 is a no-op, so we don't even want to
     change the registers array if something writes to this register.  */
  if (CANNOT_STORE_REGISTER (regnum))
    return;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  size = REGISTER_RAW_SIZE (regnum);

  /* If we have a valid copy of the register, and new value == old value,
     then don't bother doing the actual store. */

  if (register_cached (regnum)
      && memcmp (register_buffer (regnum), myaddr, size) == 0)
    return;

  if (real_register (regnum))
    target_prepare_to_store ();

  memcpy (register_buffer (regnum), myaddr, size);

  set_register_cached (regnum, 1);
  store_register (regnum);
}

/* Copy INLEN bytes of consecutive data from memory at MYADDR
   into registers starting with the MYREGSTART'th byte of register data.  */

void
write_register_bytes (int myregstart, char *myaddr, int inlen)
{
  int myregend = myregstart + inlen;
  int regnum;

  target_prepare_to_store ();

  /* Scan through the registers updating any that are covered by the
     range myregstart<=>myregend using write_register_gen, which does
     nice things like handling threads, and avoiding updates when the
     new and old contents are the same.  */

  for (regnum = 0; regnum < NUM_REGS + NUM_PSEUDO_REGS; regnum++)
    {
      int regstart, regend;

      regstart = REGISTER_BYTE (regnum);
      regend = regstart + REGISTER_RAW_SIZE (regnum);

      /* Is this register completely outside the range the user is writing?  */
      if (myregend <= regstart || regend <= myregstart)
	/* do nothing */ ;		

      /* Is this register completely within the range the user is writing?  */
      else if (myregstart <= regstart && regend <= myregend)
	write_register_gen (regnum, myaddr + (regstart - myregstart));

      /* The register partially overlaps the range being written.  */
      else
	{
	  char *regbuf = (char*) alloca (MAX_REGISTER_RAW_SIZE);
	  /* What's the overlap between this register's bytes and
             those the caller wants to write?  */
	  int overlapstart = max (regstart, myregstart);
	  int overlapend   = min (regend,   myregend);

	  /* We may be doing a partial update of an invalid register.
	     Update it from the target before scribbling on it.  */
	  read_register_gen (regnum, regbuf);

	  memcpy (registers + overlapstart,
		  myaddr + (overlapstart - myregstart),
		  overlapend - overlapstart);

	  store_register (regnum);
	}
    }
}


/* Return the contents of register REGNUM as an unsigned integer.  */

ULONGEST
read_register (int regnum)
{
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  if (!register_cached (regnum))
    fetch_register (regnum);

  return (extract_unsigned_integer (register_buffer (regnum),
				    REGISTER_RAW_SIZE (regnum)));
}

ULONGEST
read_register_pid (int regnum, int pid)
{
  int save_pid;
  CORE_ADDR retval;

  if (pid == inferior_pid)
    return read_register (regnum);

  save_pid = inferior_pid;

  inferior_pid = pid;

  retval = read_register (regnum);

  inferior_pid = save_pid;

  return retval;
}

/* Return the contents of register REGNUM as a signed integer.  */

LONGEST
read_signed_register (int regnum)
{
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  if (!register_cached (regnum))
    fetch_register (regnum);

  return (extract_signed_integer (register_buffer (regnum),
				  REGISTER_RAW_SIZE (regnum)));
}

LONGEST
read_signed_register_pid (int regnum, int pid)
{
  int save_pid;
  LONGEST retval;

  if (pid == inferior_pid)
    return read_signed_register (regnum);

  save_pid = inferior_pid;

  inferior_pid = pid;

  retval = read_signed_register (regnum);

  inferior_pid = save_pid;

  return retval;
}

/* Store VALUE into the raw contents of register number REGNUM.  */

void
write_register (int regnum, LONGEST val)
{
  PTR buf;
  int size;

  /* On the sparc, writing %g0 is a no-op, so we don't even want to
     change the registers array if something writes to this register.  */
  if (CANNOT_STORE_REGISTER (regnum))
    return;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  size = REGISTER_RAW_SIZE (regnum);
  buf = alloca (size);
  store_signed_integer (buf, size, (LONGEST) val);

  /* If we have a valid copy of the register, and new value == old value,
     then don't bother doing the actual store. */

  if (register_cached (regnum)
      && memcmp (register_buffer (regnum), buf, size) == 0)
    return;

  if (real_register (regnum))
    target_prepare_to_store ();

  memcpy (register_buffer (regnum), buf, size);

  set_register_cached (regnum, 1);
  store_register (regnum);
}

void
write_register_pid (int regnum, CORE_ADDR val, int pid)
{
  int save_pid;

  if (pid == inferior_pid)
    {
      write_register (regnum, val);
      return;
    }

  save_pid = inferior_pid;

  inferior_pid = pid;

  write_register (regnum, val);

  inferior_pid = save_pid;
}

/* SUPPLY_REGISTER()

   Record that register REGNUM contains VAL.  This is used when the
   value is obtained from the inferior or core dump, so there is no
   need to store the value there.

   If VAL is a NULL pointer, then it's probably an unsupported register.
   We just set its value to all zeros.  We might want to record this
   fact, and report it to the users of read_register and friends.  */

void
supply_register (int regnum, char *val)
{
#if 1
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }
#endif

  set_register_cached (regnum, 1);
  if (val)
    memcpy (register_buffer (regnum), val, 
	    REGISTER_RAW_SIZE (regnum));
  else
    memset (register_buffer (regnum), '\000', 
	    REGISTER_RAW_SIZE (regnum));

  /* On some architectures, e.g. HPPA, there are a few stray bits in
     some registers, that the rest of the code would like to ignore.  */

#ifdef CLEAN_UP_REGISTER_VALUE
  CLEAN_UP_REGISTER_VALUE (regnum, register_buffer (regnum));
#endif
}

/* read_pc, write_pc, read_sp, write_sp, read_fp, write_fp, etc.
   Special handling for registers PC, SP, and FP.  */

/* This routine is getting awfully cluttered with #if's.  It's probably
   time to turn this into READ_PC and define it in the tm.h file.
   Ditto for write_pc.

   1999-06-08: The following were re-written so that it assumes the
   existence of a TARGET_READ_PC et.al. macro.  A default generic
   version of that macro is made available where needed.

   Since the ``TARGET_READ_PC'' et.al. macro is going to be controlled
   by the multi-arch framework, it will eventually be possible to
   eliminate the intermediate read_pc_pid().  The client would call
   TARGET_READ_PC directly. (cagney). */

CORE_ADDR
generic_target_read_pc (int pid)
{
#ifdef PC_REGNUM
  if (PC_REGNUM >= 0)
    {
      CORE_ADDR pc_val = ADDR_BITS_REMOVE ((CORE_ADDR) read_register_pid (PC_REGNUM, pid));
      return pc_val;
    }
#endif
  internal_error (__FILE__, __LINE__,
		  "generic_target_read_pc");
  return 0;
}

CORE_ADDR
read_pc_pid (int pid)
{
  int saved_inferior_pid;
  CORE_ADDR pc_val;

  /* In case pid != inferior_pid. */
  saved_inferior_pid = inferior_pid;
  inferior_pid = pid;

  pc_val = TARGET_READ_PC (pid);

  inferior_pid = saved_inferior_pid;
  return pc_val;
}

CORE_ADDR
read_pc (void)
{
  return read_pc_pid (inferior_pid);
}

void
generic_target_write_pc (CORE_ADDR pc, int pid)
{
#ifdef PC_REGNUM
  if (PC_REGNUM >= 0)
    write_register_pid (PC_REGNUM, pc, pid);
  if (NPC_REGNUM >= 0)
    write_register_pid (NPC_REGNUM, pc + 4, pid);
  if (NNPC_REGNUM >= 0)
    write_register_pid (NNPC_REGNUM, pc + 8, pid);
#else
  internal_error (__FILE__, __LINE__,
		  "generic_target_write_pc");
#endif
}

void
write_pc_pid (CORE_ADDR pc, int pid)
{
  int saved_inferior_pid;

  /* In case pid != inferior_pid. */
  saved_inferior_pid = inferior_pid;
  inferior_pid = pid;

  TARGET_WRITE_PC (pc, pid);

  inferior_pid = saved_inferior_pid;
}

void
write_pc (CORE_ADDR pc)
{
  write_pc_pid (pc, inferior_pid);
}

/* Cope with strage ways of getting to the stack and frame pointers */

CORE_ADDR
generic_target_read_sp (void)
{
#ifdef SP_REGNUM
  if (SP_REGNUM >= 0)
    return read_register (SP_REGNUM);
#endif
  internal_error (__FILE__, __LINE__,
		  "generic_target_read_sp");
}

CORE_ADDR
read_sp (void)
{
  return TARGET_READ_SP ();
}

void
generic_target_write_sp (CORE_ADDR val)
{
#ifdef SP_REGNUM
  if (SP_REGNUM >= 0)
    {
      write_register (SP_REGNUM, val);
      return;
    }
#endif
  internal_error (__FILE__, __LINE__,
		  "generic_target_write_sp");
}

void
write_sp (CORE_ADDR val)
{
  TARGET_WRITE_SP (val);
}

CORE_ADDR
generic_target_read_fp (void)
{
#ifdef FP_REGNUM
  if (FP_REGNUM >= 0)
    return read_register (FP_REGNUM);
#endif
  internal_error (__FILE__, __LINE__,
		  "generic_target_read_fp");
}

CORE_ADDR
read_fp (void)
{
  return TARGET_READ_FP ();
}

void
generic_target_write_fp (CORE_ADDR val)
{
#ifdef FP_REGNUM
  if (FP_REGNUM >= 0)
    {
      write_register (FP_REGNUM, val);
      return;
    }
#endif
  internal_error (__FILE__, __LINE__,
		  "generic_target_write_fp");
}

void
write_fp (CORE_ADDR val)
{
  TARGET_WRITE_FP (val);
}

/* ARGSUSED */
static void
reg_flush_command (char *command, int from_tty)
{
  /* Force-flush the register cache.  */
  registers_changed ();
  if (from_tty)
    printf_filtered ("Register cache flushed.\n");
}


static void
build_regcache (void)
{
  /* We allocate some extra slop since we do a lot of memcpy's around
     `registers', and failing-soft is better than failing hard.  */
  int sizeof_registers = REGISTER_BYTES + /* SLOP */ 256;
  int sizeof_register_valid = 
    (NUM_REGS + NUM_PSEUDO_REGS) * sizeof (*register_valid);
  registers = xmalloc (sizeof_registers);
  memset (registers, 0, sizeof_registers);
  register_valid = xmalloc (sizeof_register_valid);
  memset (register_valid, 0, sizeof_register_valid);
}

void
_initialize_regcache (void)
{
  build_regcache ();

  register_gdbarch_swap (&registers, sizeof (registers), NULL);
  register_gdbarch_swap (&register_valid, sizeof (register_valid), NULL);
  register_gdbarch_swap (NULL, 0, build_regcache);

  add_com ("flushregs", class_maintenance, reg_flush_command,
	   "Force gdb to flush its register cache (maintainer command)");
}
