/* Definitions for dealing with stack frames, for GDB, the GNU debugger.

   Copyright 1986, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1996,
   1997, 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#if !defined (FRAME_H)
#define FRAME_H 1

/* The frame object.  */

struct frame_info;

/* The frame object's ID.  This provides a per-frame unique identifier
   that can be used to relocate a `struct frame_info' after a target
   resume or a frame cache destruct (assuming the target hasn't
   unwound the stack past that frame - a problem handled elsewhere).  */

struct frame_id
{
  /* The frame's address.  This should be constant through out the
     lifetime of a frame.  */
  /* NOTE: cagney/2002-11-16: The ia64 has two stacks and hence two
     frame bases.  This will need to be expanded to accomodate that.  */
  CORE_ADDR base;
  /* The frame's current PC.  While the PC within the function may
     change, the function that contains the PC does not.  Should this
     instead be the frame's function?  */
  CORE_ADDR pc;
};

/* For every stopped thread, GDB tracks two frames: current and
   selected.  Current frame is the inner most frame of the selected
   thread.  Selected frame is the frame currently being examined via
   the GDB CLI (selected using `up', `down', ...).  The frames are
   created on-demand (via get_prev_frame()) and then held in a frame
   cache.  Provide mechanims for controlling these frames.  */
/* FIXME: cagney/2002-11-14: At any time, only one thread's selected
   and current frame can be active.  Switching threads causes gdb to
   discard all that cached frame information.  Ulgh!  Instead, current
   and selected frame should be bound to a thread.  */

extern struct frame_info *selected_frame;
extern void select_frame (struct frame_info *);
extern void set_current_frame (struct frame_info *);
extern struct frame_info *get_current_frame (void);

/* Invalidates the frame cache.  */
extern void flush_cached_frames (void);

/* Flushes the frame cache and then selects the inner most (aka
   current) frame - it changes selected frame.  */
/* FIXME: cagney/2002-11-14: Should this re-select the selected frame
   from before the flush?  */
extern void reinit_frame_cache (void);

/* Given a FRAME, return the next (more inner, younger) or previous
   (more outer, older) frame.  */
extern struct frame_info *get_prev_frame (struct frame_info *);
extern struct frame_info *get_next_frame (struct frame_info *);

/* Given a frame's ID, relocate the frame.  Returns NULL if the frame
   is not found.  */
extern struct frame_info *frame_find_by_id (struct frame_id id);

/* Base attributes of a frame: */

/* The frame's `resume' address.  Where the program will resume in
   this frame.  */
extern CORE_ADDR get_frame_pc (struct frame_info *);

/* Return the per-frame unique identifer.  Can be used to relocate a
   frame after a frame cache flush (and other similar operations).  */
extern void get_frame_id (struct frame_info *fi, struct frame_id *id);

/* The frame's level: 0 for innermost, 1 for its caller, ...; or -1
   for an invalid frame).  */
extern int frame_relative_level (struct frame_info *fi);

/* Unwind the stack frame so that the value of REGNUM, in the previous
   (up, older) frame is returned.  If VALUEP is NULL, don't
   fetch/compute the value.  Instead just return the location of the
   value.  */
extern void frame_register_unwind (struct frame_info *frame, int regnum,
				   int *optimizedp, enum lval_type *lvalp,
				   CORE_ADDR *addrp, int *realnump,
				   void *valuep);

/* More convenient interface to frame_register_unwind().  */
/* NOTE: cagney/2002-09-13: Return void as one day these functions may
   be changed to return an indication that the read succeeded.  */

extern void frame_unwind_signed_register (struct frame_info *frame,
					  int regnum, LONGEST *val);

extern void frame_unwind_unsigned_register (struct frame_info *frame,
					    int regnum, ULONGEST *val);

/* Get the value of the register that belongs to this FRAME.  This
   function is a wrapper to the call sequence ``frame_unwind_register
   (get_next_frame (FRAME))''.  As per frame_register_unwind(), if
   VALUEP is NULL, the registers value is not fetched/computed.  */

extern void frame_register (struct frame_info *frame, int regnum,
			    int *optimizedp, enum lval_type *lvalp,
			    CORE_ADDR *addrp, int *realnump,
			    void *valuep);

/* More convenient interface to frame_register().  */
/* NOTE: cagney/2002-09-13: Return void as one day these functions may
   be changed to return an indication that the read succeeded.  */

extern void frame_read_signed_register (struct frame_info *frame,
					int regnum, LONGEST *val);

extern void frame_read_unsigned_register (struct frame_info *frame,
					  int regnum, ULONGEST *val);

/* Map between a frame register number and its name.  A frame register
   space is a superset of the cooked register space --- it also
   includes builtin registers.  */

extern int frame_map_name_to_regnum (const char *name, int strlen);
extern const char *frame_map_regnum_to_name (int regnum);

/* Unwind the PC.  Strictly speaking return the resume address of the
   calling frame.  For GDB, `pc' is the resume address and not a
   specific register.  */

extern CORE_ADDR frame_pc_unwind (struct frame_info *frame);


/* Return the location (and possibly value) of REGNUM for the previous
   (older, up) frame.  All parameters except VALUEP can be assumed to
   be non NULL.  When VALUEP is NULL, just the location of the
   register should be returned.

   UNWIND_CACHE is provided as mechanism for implementing a per-frame
   local cache.  It's initial value being NULL.  Memory for that cache
   should be allocated using frame_obstack_alloc().

   Register window architectures (eg SPARC) should note that REGNUM
   identifies the register for the previous frame.  For instance, a
   request for the value of "o1" for the previous frame would be found
   in the register "i1" in this FRAME.  */

typedef void (frame_register_unwind_ftype) (struct frame_info *frame,
					    void **unwind_cache,
					    int regnum,
					    int *optimized,
					    enum lval_type *lvalp,
					    CORE_ADDR *addrp,
					    int *realnump,
					    void *valuep);

/* Same as for registers above, but return the address at which the
   calling frame would resume.  */

typedef CORE_ADDR (frame_pc_unwind_ftype) (struct frame_info *frame,
					   void **unwind_cache);

/* Describe the saved registers of a frame.  */

#if defined (EXTRA_FRAME_INFO) || defined (FRAME_FIND_SAVED_REGS)
/* XXXX - deprecated */
struct frame_saved_regs
  {
    /* For each register R (except the SP), regs[R] is the address at
       which it was saved on entry to the frame, or zero if it was not
       saved on entry to this frame.  This includes special registers
       such as pc and fp saved in special ways in the stack frame.

       regs[SP_REGNUM] is different.  It holds the actual SP, not the
       address at which it was saved.  */

    CORE_ADDR regs[NUM_REGS];
  };
#endif

/* We keep a cache of stack frames, each of which is a "struct
   frame_info".  The innermost one gets allocated (in
   wait_for_inferior) each time the inferior stops; current_frame
   points to it.  Additional frames get allocated (in
   get_prev_frame) as needed, and are chained through the next
   and prev fields.  Any time that the frame cache becomes invalid
   (most notably when we execute something, but also if we change how
   we interpret the frames (e.g. "set heuristic-fence-post" in
   mips-tdep.c, or anything which reads new symbols)), we should call
   reinit_frame_cache.  */

struct frame_info
  {
    /* Nominal address of the frame described.  See comments at FRAME_FP
       about what this means outside the *FRAME* macros; in the *FRAME*
       macros, it can mean whatever makes most sense for this machine.  */
    CORE_ADDR frame;

    /* Address at which execution is occurring in this frame.
       For the innermost frame, it's the current pc.
       For other frames, it is a pc saved in the next frame.  */
    CORE_ADDR pc;

    /* Level of this frame.  The inner-most (youngest) frame is at
       level 0.  As you move towards the outer-most (oldest) frame,
       the level increases.  This is a cached value.  It could just as
       easily be computed by counting back from the selected frame to
       the inner most frame.  */
    /* NOTE: cagney/2002-04-05: Perhaphs a level of ``-1'' should be
       reserved to indicate a bogus frame - one that has been created
       just to keep GDB happy (GDB always needs a frame).  For the
       moment leave this as speculation.  */
    int level;

    /* Nonzero if this is a frame associated with calling a signal handler.

       Set by machine-dependent code.  On some machines, if
       the machine-dependent code fails to check for this, the backtrace
       will look relatively normal.  For example, on the i386
       #3  0x158728 in sighold ()
       On other machines (e.g. rs6000), the machine-dependent code better
       set this to prevent us from trying to print it like a normal frame.  */
    int signal_handler_caller;

    /* For each register, address of where it was saved on entry to
       the frame, or zero if it was not saved on entry to this frame.
       This includes special registers such as pc and fp saved in
       special ways in the stack frame.  The SP_REGNUM is even more
       special, the address here is the sp for the previous frame, not
       the address where the sp was saved.  */
    /* Allocated by frame_saved_regs_zalloc () which is called /
       initialized by FRAME_INIT_SAVED_REGS(). */
    CORE_ADDR *saved_regs;	/*NUM_REGS + NUM_PSEUDO_REGS*/

#ifdef EXTRA_FRAME_INFO
    /* XXXX - deprecated */
    /* Anything extra for this structure that may have been defined
       in the machine dependent files. */
      EXTRA_FRAME_INFO
#endif

    /* Anything extra for this structure that may have been defined
       in the machine dependent files. */
    /* Allocated by frame_obstack_alloc () which is called /
       initialized by INIT_EXTRA_FRAME_INFO */
    struct frame_extra_info *extra_info;

    /* If dwarf2 unwind frame informations is used, this structure holds all
       related unwind data.  */
    struct context *context;

    /* Unwind cache shared between the unwind functions - they had
       better all agree as to the contents.  */
    void *unwind_cache;

    /* See description above.  The previous frame's registers.  */
    frame_register_unwind_ftype *register_unwind;

    /* See description above.  The previous frame's resume address.
       Save the previous PC in a local cache.  */
    frame_pc_unwind_ftype *pc_unwind;
    int pc_unwind_cache_p;
    CORE_ADDR pc_unwind_cache;

    /* Pointers to the next (down, inner, younger) and previous (up,
       outer, older) frame_info's in the frame cache.  */
    struct frame_info *next; /* down, inner, younger */
    int prev_p;
    struct frame_info *prev; /* up, outer, older */
  };

/* Values for the source flag to be used in print_frame_info_base(). */
enum print_what
  { 
    /* Print only the source line, like in stepi. */
    SRC_LINE = -1, 
    /* Print only the location, i.e. level, address (sometimes)
       function, args, file, line, line num. */
    LOCATION,
    /* Print both of the above. */
    SRC_AND_LOC, 
    /* Print location only, but always include the address. */
    LOC_AND_ADDRESS 
  };

/* Allocate additional space for appendices to a struct frame_info.
   NOTE: Much of GDB's code works on the assumption that the allocated
   saved_regs[] array is the size specified below.  If you try to make
   that array smaller, GDB will happily walk off its end. */

#ifdef SIZEOF_FRAME_SAVED_REGS
#error "SIZEOF_FRAME_SAVED_REGS can not be re-defined"
#endif
#define SIZEOF_FRAME_SAVED_REGS \
        (sizeof (CORE_ADDR) * (NUM_REGS+NUM_PSEUDO_REGS))

extern void *frame_obstack_alloc (unsigned long size);
extern void frame_saved_regs_zalloc (struct frame_info *);

/* Return the frame address from FI.  Except in the machine-dependent
   *FRAME* macros, a frame address has no defined meaning other than
   as a magic cookie which identifies a frame over calls to the
   inferior.  The only known exception is inferior.h
   (PC_IN_CALL_DUMMY) [ON_STACK]; see comments there.  You cannot
   assume that a frame address contains enough information to
   reconstruct the frame; if you want more than just to identify the
   frame (e.g. be able to fetch variables relative to that frame),
   then save the whole struct frame_info (and the next struct
   frame_info, since the latter is used for fetching variables on some
   machines).  */

#define FRAME_FP(fi) ((fi)->frame)

/* Define a default FRAME_CHAIN_VALID, in the form that is suitable for most
   targets.  If FRAME_CHAIN_VALID returns zero it means that the given frame
   is the outermost one and has no caller.

   XXXX - both default and alternate frame_chain_valid functions are
   deprecated.  New code should use dummy frames and one of the
   generic functions. */

extern int file_frame_chain_valid (CORE_ADDR, struct frame_info *);
extern int func_frame_chain_valid (CORE_ADDR, struct frame_info *);
extern int nonnull_frame_chain_valid (CORE_ADDR, struct frame_info *);
extern int generic_file_frame_chain_valid (CORE_ADDR, struct frame_info *);
extern int generic_func_frame_chain_valid (CORE_ADDR, struct frame_info *);
extern void generic_save_dummy_frame_tos (CORE_ADDR sp);

extern struct frame_info *create_new_frame (CORE_ADDR, CORE_ADDR);


#ifdef FRAME_FIND_SAVED_REGS
/* XXX - deprecated */
#define FRAME_INIT_SAVED_REGS(FI) get_frame_saved_regs (FI, NULL)
extern void get_frame_saved_regs (struct frame_info *,
				  struct frame_saved_regs *);
#endif

extern struct block *get_frame_block (struct frame_info *,
                                      CORE_ADDR *addr_in_block);

extern struct block *get_current_block (CORE_ADDR *addr_in_block);

extern struct block *get_selected_block (CORE_ADDR *addr_in_block);

extern struct symbol *get_frame_function (struct frame_info *);

extern CORE_ADDR frame_address_in_block (struct frame_info *);

extern CORE_ADDR get_pc_function_start (CORE_ADDR);

extern struct block *block_for_pc (CORE_ADDR);

extern struct block *block_for_pc_sect (CORE_ADDR, asection *);

extern int frameless_look_for_prologue (struct frame_info *);

extern void print_frame_args (struct symbol *, struct frame_info *,
			      int, struct ui_file *);

extern struct frame_info *find_relative_frame (struct frame_info *, int *);

extern void show_and_print_stack_frame (struct frame_info *fi, int level,
					int source);

extern void print_stack_frame (struct frame_info *, int, int);

extern void print_only_stack_frame (struct frame_info *, int, int);

extern void show_stack_frame (struct frame_info *);

extern void print_frame_info (struct frame_info *, int, int, int);

extern void show_frame_info (struct frame_info *, int, int, int);

extern struct frame_info *block_innermost_frame (struct block *);

extern struct frame_info *find_frame_addr_in_frame_chain (CORE_ADDR);

/* NOTE: cagney/2002-09-13: There is no need for this function.
   Instead either of frame_unwind_signed_register() or
   frame_unwind_unsigned_register() can be used.  */
extern CORE_ADDR deprecated_read_register_dummy (CORE_ADDR pc,
						 CORE_ADDR fp, int);
extern void generic_push_dummy_frame (void);
extern void generic_pop_current_frame (void (*)(struct frame_info *));
extern void generic_pop_dummy_frame (void);

extern int generic_pc_in_call_dummy (CORE_ADDR pc,
				     CORE_ADDR sp, CORE_ADDR fp);

/* NOTE: cagney/2002-06-26: Targets should no longer use this
   function.  Instead, the contents of a dummy frames registers can be
   obtained by applying: frame_register_unwind to the dummy frame; or
   get_saved_register to the next outer frame.  */

extern char *deprecated_generic_find_dummy_frame (CORE_ADDR pc, CORE_ADDR fp);

extern void generic_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun,
				    int nargs, struct value **args,
				    struct type *type, int gcc_p);

/* The function generic_get_saved_register() has been made obsolete.
   GET_SAVED_REGISTER now defaults to the recursive equivalent -
   generic_unwind_get_saved_register() - so there is no need to even
   set GET_SAVED_REGISTER.  Architectures that need to override the
   register unwind mechanism should modify frame->unwind().  */
extern void deprecated_generic_get_saved_register (char *, int *, CORE_ADDR *,
						   struct frame_info *, int,
						   enum lval_type *);

extern void generic_unwind_get_saved_register (char *raw_buffer,
					       int *optimized,
					       CORE_ADDR * addrp,
					       struct frame_info *frame,
					       int regnum,
					       enum lval_type *lval);

extern void generic_save_call_dummy_addr (CORE_ADDR lo, CORE_ADDR hi);

extern void get_saved_register (char *raw_buffer, int *optimized,
				CORE_ADDR * addrp,
				struct frame_info *frame,
				int regnum, enum lval_type *lval);

extern int frame_register_read (struct frame_info *frame, int regnum,
				void *buf);

/* From stack.c.  */
extern void args_info (char *, int);

extern void locals_info (char *, int);

extern void (*selected_frame_level_changed_hook) (int);

extern void return_command (char *, int);

#endif /* !defined (FRAME_H)  */
