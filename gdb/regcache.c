/* Cache and manage the values of registers for GDB, the GNU debugger.
   Copyright 1986, 87, 89, 91, 94, 95, 96, 1998, 2000
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

/*
 * DATA STRUCTURE
 *
 * Here is the actual register cache.
 */

/* NOTE: this is a write-back cache.  There is no "dirty" bit for
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

/* FIND_SAVED_REGISTER ()

   Return the address in which frame FRAME's value of register REGNUM
   has been saved in memory.  Or return zero if it has not been saved.
   If REGNUM specifies the SP, the value we return is actually
   the SP value, not an address where it was saved.  */

CORE_ADDR
find_saved_register (struct frame_info *frame, int regnum)
{
  register struct frame_info *frame1 = NULL;
  register CORE_ADDR addr = 0;

  if (frame == NULL)		/* No regs saved if want current frame */
    return 0;

#ifdef HAVE_REGISTER_WINDOWS
  /* We assume that a register in a register window will only be saved
     in one place (since the name changes and/or disappears as you go
     towards inner frames), so we only call get_frame_saved_regs on
     the current frame.  This is directly in contradiction to the
     usage below, which assumes that registers used in a frame must be
     saved in a lower (more interior) frame.  This change is a result
     of working on a register window machine; get_frame_saved_regs
     always returns the registers saved within a frame, within the
     context (register namespace) of that frame. */

  /* However, note that we don't want this to return anything if
     nothing is saved (if there's a frame inside of this one).  Also,
     callers to this routine asking for the stack pointer want the
     stack pointer saved for *this* frame; this is returned from the
     next frame.  */

  if (REGISTER_IN_WINDOW_P (regnum))
    {
      frame1 = get_next_frame (frame);
      if (!frame1)
	return 0;		/* Registers of this frame are active.  */

      /* Get the SP from the next frame in; it will be this
         current frame.  */
      if (regnum != SP_REGNUM)
	frame1 = frame;

      FRAME_INIT_SAVED_REGS (frame1);
      return frame1->saved_regs[regnum];	/* ... which might be zero */
    }
#endif /* HAVE_REGISTER_WINDOWS */

  /* Note that this next routine assumes that registers used in
     frame x will be saved only in the frame that x calls and
     frames interior to it.  This is not true on the sparc, but the
     above macro takes care of it, so we should be all right. */
  while (1)
    {
      QUIT;
      frame1 = get_prev_frame (frame1);
      if (frame1 == 0 || frame1 == frame)
	break;
      FRAME_INIT_SAVED_REGS (frame1);
      if (frame1->saved_regs[regnum])
	addr = frame1->saved_regs[regnum];
    }

  return addr;
}

/* DEFAULT_GET_SAVED_REGISTER ()

   Find register number REGNUM relative to FRAME and put its (raw,
   target format) contents in *RAW_BUFFER.  Set *OPTIMIZED if the
   variable was optimized out (and thus can't be fetched).  Set *LVAL
   to lval_memory, lval_register, or not_lval, depending on whether
   the value was fetched from memory, from a register, or in a strange
   and non-modifiable way (e.g. a frame pointer which was calculated
   rather than fetched).  Set *ADDRP to the address, either in memory
   on as a REGISTER_BYTE offset into the registers array.

   Note that this implementation never sets *LVAL to not_lval.  But
   it can be replaced by defining GET_SAVED_REGISTER and supplying
   your own.

   The argument RAW_BUFFER must point to aligned memory.  */

static void
default_get_saved_register (char *raw_buffer,
			    int *optimized,
			    CORE_ADDR *addrp,
			    struct frame_info *frame,
			    int regnum,
			    enum lval_type *lval)
{
  CORE_ADDR addr;

  if (!target_has_registers)
    error ("No registers.");

  /* Normal systems don't optimize out things with register numbers.  */
  if (optimized != NULL)
    *optimized = 0;
  addr = find_saved_register (frame, regnum);
  if (addr != 0)
    {
      if (lval != NULL)
	*lval = lval_memory;
      if (regnum == SP_REGNUM)
	{
	  if (raw_buffer != NULL)
	    {
	      /* Put it back in target format.  */
	      store_address (raw_buffer, REGISTER_RAW_SIZE (regnum),
			     (LONGEST) addr);
	    }
	  if (addrp != NULL)
	    *addrp = 0;
	  return;
	}
      if (raw_buffer != NULL)
	target_read_memory (addr, raw_buffer, REGISTER_RAW_SIZE (regnum));
    }
  else
    {
      if (lval != NULL)
	*lval = lval_register;
      addr = REGISTER_BYTE (regnum);
      if (raw_buffer != NULL)
	read_register_gen (regnum, raw_buffer);
    }
  if (addrp != NULL)
    *addrp = addr;
}

#if !defined (GET_SAVED_REGISTER)
#define GET_SAVED_REGISTER(raw_buffer, optimized, addrp, frame, regnum, lval) \
  default_get_saved_register(raw_buffer, optimized, addrp, frame, regnum, lval)
#endif

void
get_saved_register (char *raw_buffer,
		    int *optimized,
		    CORE_ADDR *addrp,
		    struct frame_info *frame,
		    int regnum,
		    enum lval_type *lval)
{
  GET_SAVED_REGISTER (raw_buffer, optimized, addrp, frame, regnum, lval);
}

/* READ_RELATIVE_REGISTER_RAW_BYTES_FOR_FRAME

   Copy the bytes of register REGNUM, relative to the input stack frame,
   into our memory at MYADDR, in target byte order.
   The number of bytes copied is REGISTER_RAW_SIZE (REGNUM).

   Returns 1 if could not be read, 0 if could.  */

/* FIXME: This function increases the confusion between FP_REGNUM
   and the virtual/pseudo-frame pointer.  */

static int
read_relative_register_raw_bytes_for_frame (int regnum,
					    char *myaddr,
					    struct frame_info *frame)
{
  int optim;
  if (regnum == FP_REGNUM && frame)
    {
      /* Put it back in target format. */
      store_address (myaddr, REGISTER_RAW_SIZE (FP_REGNUM),
		     (LONGEST) FRAME_FP (frame));

      return 0;
    }

  get_saved_register (myaddr, &optim, (CORE_ADDR *) NULL, frame,
		      regnum, (enum lval_type *) NULL);

  if (register_valid[regnum] < 0)
    return 1;			/* register value not available */

  return optim;
}

/* READ_RELATIVE_REGISTER_RAW_BYTES

   Copy the bytes of register REGNUM, relative to the current stack
   frame, into our memory at MYADDR, in target byte order.  
   The number of bytes copied is REGISTER_RAW_SIZE (REGNUM).

   Returns 1 if could not be read, 0 if could.  */

int
read_relative_register_raw_bytes (int regnum, char *myaddr)
{
  return read_relative_register_raw_bytes_for_frame (regnum, myaddr,
						     selected_frame);
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
    register_valid[i] = 0;

  /* Assume that if all the hardware regs have changed, 
     then so have the pseudo-registers.  */
  for (i = NUM_REGS; i < NUM_REGS + NUM_PSEUDO_REGS; i++)
    register_valid[i] = 0;

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
    register_valid[i] = 1;
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
   regno as an argument, and therefore can't do a partial register
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
  int regno;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  /* See if we are trying to read bytes from out-of-date registers.  If so,
     update just those registers.  */

  for (regno = 0; regno < NUM_REGS + NUM_PSEUDO_REGS; regno++)
    {
      int regstart, regend;

      if (register_valid[regno])
	continue;

      if (REGISTER_NAME (regno) == NULL || *REGISTER_NAME (regno) == '\0')
	continue;

      regstart = REGISTER_BYTE (regno);
      regend = regstart + REGISTER_RAW_SIZE (regno);

      if (regend <= inregbyte || inregend <= regstart)
	/* The range the user wants to read doesn't overlap with regno.  */
	continue;

      /* We've found an uncached register where at least one byte will be read.
         Update it from the target.  */
      if (regno < NUM_REGS)
	target_fetch_registers (regno);
      else if (regno < NUM_REGS + NUM_PSEUDO_REGS)
	FETCH_PSEUDO_REGISTER (regno);

      if (!register_valid[regno])
	error ("read_register_bytes:  Couldn't update register %d.", regno);
    }

  if (myaddr != NULL)
    memcpy (myaddr, &registers[inregbyte], inlen);
}

/* Read register REGNO into memory at MYADDR, which must be large
   enough for REGISTER_RAW_BYTES (REGNO).  Target byte-order.  If the
   register is known to be the size of a CORE_ADDR or smaller,
   read_register can be used instead.  */

void
read_register_gen (int regno, char *myaddr)
{
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  if (!register_valid[regno])
    {
      if (regno < NUM_REGS)
	target_fetch_registers (regno);
      else if (regno < NUM_REGS + NUM_PSEUDO_REGS)
	FETCH_PSEUDO_REGISTER (regno);
    }
  memcpy (myaddr, &registers[REGISTER_BYTE (regno)],
	  REGISTER_RAW_SIZE (regno));
}

/* Write register REGNO at MYADDR to the target.  MYADDR points at
   REGISTER_RAW_BYTES(REGNO), which must be in target byte-order.  */

/* Registers we shouldn't try to store.  */
#if !defined (CANNOT_STORE_REGISTER)
#define CANNOT_STORE_REGISTER(regno) 0
#endif

void
write_register_gen (int regno, char *myaddr)
{
  int size;

  /* On the sparc, writing %g0 is a no-op, so we don't even want to
     change the registers array if something writes to this register.  */
  if (CANNOT_STORE_REGISTER (regno))
    return;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  size = REGISTER_RAW_SIZE (regno);

  /* If we have a valid copy of the register, and new value == old value,
     then don't bother doing the actual store. */

  if (register_valid[regno]
      && memcmp (&registers[REGISTER_BYTE (regno)], myaddr, size) == 0)
    return;

  if (regno < NUM_REGS)
    target_prepare_to_store ();

  memcpy (&registers[REGISTER_BYTE (regno)], myaddr, size);

  register_valid[regno] = 1;

  if (regno < NUM_REGS)
    target_store_registers (regno);
  else if (regno < NUM_REGS + NUM_PSEUDO_REGS)
    STORE_PSEUDO_REGISTER (regno);
}

/* Copy INLEN bytes of consecutive data from memory at MYADDR
   into registers starting with the MYREGSTART'th byte of register data.  */

void
write_register_bytes (int myregstart, char *myaddr, int inlen)
{
  int myregend = myregstart + inlen;
  int regno;

  target_prepare_to_store ();

  /* Scan through the registers updating any that are covered by the
     range myregstart<=>myregend using write_register_gen, which does
     nice things like handling threads, and avoiding updates when the
     new and old contents are the same.  */

  for (regno = 0; regno < NUM_REGS + NUM_PSEUDO_REGS; regno++)
    {
      int regstart, regend;

      regstart = REGISTER_BYTE (regno);
      regend = regstart + REGISTER_RAW_SIZE (regno);

      /* Is this register completely outside the range the user is writing?  */
      if (myregend <= regstart || regend <= myregstart)
	/* do nothing */ ;		

      /* Is this register completely within the range the user is writing?  */
      else if (myregstart <= regstart && regend <= myregend)
	write_register_gen (regno, myaddr + (regstart - myregstart));

      /* The register partially overlaps the range being written.  */
      else
	{
	  char regbuf[MAX_REGISTER_RAW_SIZE];
	  /* What's the overlap between this register's bytes and
             those the caller wants to write?  */
	  int overlapstart = max (regstart, myregstart);
	  int overlapend   = min (regend,   myregend);

	  /* We may be doing a partial update of an invalid register.
	     Update it from the target before scribbling on it.  */
	  read_register_gen (regno, regbuf);

	  memcpy (registers + overlapstart,
		  myaddr + (overlapstart - myregstart),
		  overlapend - overlapstart);

	  if (regno < NUM_REGS)
	    target_store_registers (regno);
	  else if (regno < NUM_REGS + NUM_PSEUDO_REGS)
	    STORE_PSEUDO_REGISTER (regno);
	}
    }
}


/* Return the raw contents of register REGNO, regarding it as an
   UNSIGNED integer. */

ULONGEST
read_register (int regno)
{
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  if (!register_valid[regno])
    {
      if (regno < NUM_REGS)
	target_fetch_registers (regno);
      else if (regno < NUM_REGS + NUM_PSEUDO_REGS)
	FETCH_PSEUDO_REGISTER (regno);
    }

  return (extract_unsigned_integer (&registers[REGISTER_BYTE (regno)],
				    REGISTER_RAW_SIZE (regno)));
}

ULONGEST
read_register_pid (int regno, int pid)
{
  int save_pid;
  CORE_ADDR retval;

  if (pid == inferior_pid)
    return read_register (regno);

  save_pid = inferior_pid;

  inferior_pid = pid;

  retval = read_register (regno);

  inferior_pid = save_pid;

  return retval;
}

/* Return the raw contents of register REGNO, regarding it a SIGNED
   integer. */

LONGEST
read_signed_register (int regno)
{
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  if (!register_valid[regno])
    target_fetch_registers (regno);

  return (extract_signed_integer (&registers[REGISTER_BYTE (regno)],
				  REGISTER_RAW_SIZE (regno)));
}

LONGEST
read_signed_register_pid (int regno, int pid)
{
  int save_pid;
  LONGEST retval;

  if (pid == inferior_pid)
    return read_signed_register (regno);

  save_pid = inferior_pid;

  inferior_pid = pid;

  retval = read_signed_register (regno);

  inferior_pid = save_pid;

  return retval;
}

/* Store VALUE, into the raw contents of register number REGNO.  */

void
write_register (int regno, LONGEST val)
{
  PTR buf;
  int size;

  /* On the sparc, writing %g0 is a no-op, so we don't even want to
     change the registers array if something writes to this register.  */
  if (CANNOT_STORE_REGISTER (regno))
    return;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  size = REGISTER_RAW_SIZE (regno);
  buf = alloca (size);
  store_signed_integer (buf, size, (LONGEST) val);

  /* If we have a valid copy of the register, and new value == old value,
     then don't bother doing the actual store. */

  if (register_valid[regno]
      && memcmp (&registers[REGISTER_BYTE (regno)], buf, size) == 0)
    return;

  if (regno < NUM_REGS)
    target_prepare_to_store ();

  memcpy (&registers[REGISTER_BYTE (regno)], buf, size);

  register_valid[regno] = 1;

  if (regno < NUM_REGS)
    target_store_registers (regno);
  else if (regno < NUM_REGS + NUM_PSEUDO_REGS)
    STORE_PSEUDO_REGISTER (regno);
}

void
write_register_pid (int regno, CORE_ADDR val, int pid)
{
  int save_pid;

  if (pid == inferior_pid)
    {
      write_register (regno, val);
      return;
    }

  save_pid = inferior_pid;

  inferior_pid = pid;

  write_register (regno, val);

  inferior_pid = save_pid;
}

/* SUPPLY_REGISTER()

   Record that register REGNO contains VAL.  This is used when the
   value is obtained from the inferior or core dump, so there is no
   need to store the value there.

   If VAL is a NULL pointer, then it's probably an unsupported register.
   We just set it's value to all zeros.  We might want to record this
   fact, and report it to the users of read_register and friends.  */

void
supply_register (int regno, char *val)
{
#if 1
  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }
#endif

  register_valid[regno] = 1;
  if (val)
    memcpy (&registers[REGISTER_BYTE (regno)], val, 
	    REGISTER_RAW_SIZE (regno));
  else
    memset (&registers[REGISTER_BYTE (regno)], '\000', 
	    REGISTER_RAW_SIZE (regno));

  /* On some architectures, e.g. HPPA, there are a few stray bits in
     some registers, that the rest of the code would like to ignore.  */

#ifdef CLEAN_UP_REGISTER_VALUE
  CLEAN_UP_REGISTER_VALUE (regno, &registers[REGISTER_BYTE (regno)]);
#endif
}

/* read_pc, write_pc, read_sp, write_sp, read_fp, write_fp, etc.
   Special handling for registers PC, SP, and FP.  */

/* This routine is getting awfully cluttered with #if's.  It's probably
   time to turn this into READ_PC and define it in the tm.h file.
   Ditto for write_pc.

   1999-06-08: The following were re-written so that it assumes the
   existance of a TARGET_READ_PC et.al. macro.  A default generic
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
  internal_error ("generic_target_read_pc");
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
  internal_error ("generic_target_write_pc");
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
  internal_error ("generic_target_read_sp");
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
  internal_error ("generic_target_write_sp");
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
  internal_error ("generic_target_read_fp");
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
  internal_error ("generic_target_write_fp");
}

void
write_fp (CORE_ADDR val)
{
  TARGET_WRITE_FP (val);
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
}
