/* This file has been modified by Data General Corporation, November 1989. */

/*
This file provides an abstract interface to "tdesc" information.
      It is designed to be used in a uniform manner by several kinds
      of debuggers:
         (1) code in live debugged process (e.g., a traceback routine)
         (2) a separate-process debugger debugging a live process
         (3) a separate-process debugger debugging a memory dump

      Dcontext model notes
         * captures machine context
            * partial: excludes memory
         * frames
            * kinds
         * make one for starters, chain in reverse order to previous ones
         * representation: pointer to opaque
            * alloc/free protocol

      Overall model
         * access functions
         * handle
         * error handling
*/



typedef int dc_boolean_t;   /* range 0 .. 1 */
#define DC_FALSE 0
#define DC_TRUE 1


typedef int dc_tristate_t;  /* range 0 .. 2 */
#define DC_NO 0
#define DC_YES 1
#define DC_MAYBE 2


/*
   A word is 32 bits of information.  In memory, a word is word-aligned.

   A common and important use of dc_word_t is to represent values in the
   target process, including (byte) addresses in the target process.
   In this case, C arithmetic can be used to simulate machine address
   arithmetic on the target.  (Unsigned arithmetic is actually modulus
   arithmetic.)
*/
typedef unsigned int dc_word_t;


/* These bit operations number bits from 0 at the least significant end. */
#define bit_test(word,bit) ((word) & (1 << (bit)))    /* returns 0 or other */
#define bit_value(word,bit) (((word) >> (bit)) & 1)   /* returns 0 or 1 */
#define bit_set(word,bit) ((word) |= (1 << (bit)))
#define bit_clear(word,bit) ((word) &= ~(1 << (bit)))
#define bit_assign(word, bit, bool) \
   if (bool) bit_set(word, bit); else bit_clear(word, bit)


/*----------------*/


/* The exactness of locations may not be certainly known. */
typedef dc_tristate_t dc_exactness_t;


/*
   The model includes five kinds of contexts.  Because each context
   has an associated region and frame, these describe region kinds
   and frame kinds as well.
   [more description needed]
   Currently, only call contexts exist.
*/

typedef int dc_kind_t;   /* range 0 .. 4 */
#define DC_CALL_KIND          0
#define DC_SAVE_KIND          1
#define DC_EXCEPTION_KIND     2
#define DC_PROTECTION_KIND    3
#define DC_SPECIAL_KIND       4
#define DC_NUM_KINDS          5

#define DC_MIO_ENTRY_POINT            (1<< 0)
#define DC_MIO_PROLOGUE_END           (1<< 1)
#define DC_MIO_EPILOGUE_START         (1<< 2)
#define DC_MIO_IMPLICIT_PROLOGUE_END  (1<<16)
#define DC_MIO_LITERAL_ENTRY_POINT    (1<<17)
#define DC_MIO_LITERAL_EPILOGUE_START (1<<18)

#define DC_MII_PRECEDING_TDESC_END   (1<<0)
#define DC_MII_FOLLOWING_TDESC_START (1<<1)

typedef struct dc_debug_info {
   unsigned int         protocol;            /* 1 for this structure */
   dc_word_t               tdesc_ptr;
   unsigned int         text_words_count;
   dc_word_t               text_words_ptr;
   unsigned int         data_words_count;
   dc_word_t               data_words_ptr;
} dc_debug_info_t;


typedef struct tdesc_hdr {
   unsigned int         map_protocol;        /* 1 for this structure */
   unsigned int         end;                 /* address beyond end */
} tdesc_hdr_t;


typedef struct tdesc_chunk_hdr {
   int                  zeroes : 8;
   int                  info_length : 22;
   int                  info_alignment : 2;
   unsigned int         info_protocol;
   dc_word_t               start_address;
   dc_word_t               end_address;
} tdesc_chunk_hdr_t;


typedef struct tdesc_chunk_info1 {
   int                  variant : 8;         /* 1 for this structure */
   int                  register_save_mask : 17;
   int                  pad1 : 1;
   int                  return_address_info_discriminant : 1;
   int                  frame_address_register : 5;
   unsigned int         frame_address_offset;
   unsigned int         return_address_info;
   unsigned int         register_save_offset;
} tdesc_chunk_info1_t;


typedef struct tdesc_chunk1 {
   tdesc_chunk_hdr_t    hdr;
   tdesc_chunk_info1_t  info;
} tdesc_chunk1_t;


typedef struct dc_mstate {
   dc_word_t reg[32];                      /* general registers */
   dc_word_t xip;
   dc_word_t nip;
   dc_word_t fip;
   dc_word_t fpsr;
   dc_word_t fpcr;
   dc_word_t psr;
} dc_mstate_t;


typedef struct dc_map_info_in {
   dc_word_t flags;
   dc_word_t preceding_tdesc_end;
   dc_word_t following_tdesc_start;
} dc_map_info_in_t;


typedef struct dc_map_info_out {
   dc_word_t flags;
   dc_word_t entry_point;
   dc_word_t prologue_end;
   dc_word_t epilogue_start;
} dc_map_info_out_t;


#if 0

   void error_fcn (env, continuable, message)
      dc_word_t env;                       /* environment (arbitrary datum) */
      dc_boolean_t continuable;            /* whether error function may return */
      char *message;                    /* string (no trailing newline) */

   /* In the future, we probably want the error_fcn to be: */
   void error_fcn (env, continuable, code, ...)
      dc_word_t env;                       /* environment (arbitrary datum) */
      dc_boolean_t continuable;            /* whether error function may return */
      int code;                         /* error code */
      ...                               /* parameters to message associated
                                           with the code */

   void read_fcn (env, memory, length, buffer)
      dc_word_t env;                       /* environment (arbitrary datum) */
      dc_word_t memory;                    /* start address in image */
      int length;                       /* in bytes */
      char *buffer;                     /* start address of buffer */
      /* There are no alignment assumptions for the read function. */

   void write_fcn (env, memory, length, buffer)
      dc_word_t env;                       /* environment (arbitrary datum) */
      dc_word_t memory;                    /* start address in image */
      int length;                       /* in bytes */
      char *buffer;                     /* start address of buffer */
      /* There are no alignment assumptions for the write function. */
      /* The write function is optional.  It must be provided if changes
         to writable registers are to be made. */

   void exec_fcn (env, mstate)
      dc_word_t env;                       /* environment (arbitrary datum) */
      dc_mstate_t *mstate;              /* machine state (read-write) */
      /* The execute function is optional.  It would be used (in the future)
         by the implementation of a procedurally specified tdesc mechanism. */

#endif

/*----------------*/


#ifndef NULL
#define NULL ((void *) 0)
#endif

extern char *malloc();
extern char *calloc();
extern void qsort();


/*
   At initialization, create a tdesc table from the tdesc info.
   A tdesc table is simply a sorted array of tdesc elements.
   A tdesc element is the last 6 words of the tdesc chunk.
   We require that all tdesc chunks have info protocol 1.
*/

typedef struct tdesc_elem {
   dc_word_t start_address;
   dc_word_t end_address;
   tdesc_chunk_info1_t info;
} tdesc_elem_t;

typedef tdesc_elem_t *tdesc_table_t;

void dc_correct_cr_data();

int dc_compare_tdesc_elems (elem1, elem2)
   char *elem1, *elem2;
{
   dc_word_t s1, s2, e1, e2;
   s1 = ((tdesc_elem_t *) elem1)->start_address;
   s2 = ((tdesc_elem_t *) elem2)->start_address;
   if (s1 < s2) return -1;
   if (s1 > s2) return +1;
   e1 = ((tdesc_elem_t *) elem1)->end_address;
   e2 = ((tdesc_elem_t *) elem2)->end_address;
   if (e1 < e2) return -1;
   if (e1 > e2) return +1;
   return 0;
}


typedef struct handle_info {
   dc_word_t debug_info_ptr;
   void (*error_fcn)();
   dc_word_t error_env;
   void (*read_fcn)();
   dc_word_t read_env;
   void (*write_fcn)();                 /* NULL => absent */
   dc_word_t write_env;
   void (*exec_fcn)();                  /* NULL => absent */
   dc_word_t exec_env;
   void (*map_fcn)();                  /* NULL => absent */
   dc_word_t map_env;
   tdesc_table_t tdesc_table;
   int tdesc_table_size;
} handle_info_t;

typedef handle_info_t *dc_handle_t;


/*
   Errors detected in this module are funnelled through dc_error or dc_warn,
   as appropriate.  Both routines call dc_exception, which invokes the error
   handler supplied by the user.

   Currently, dc_exception substitutes parameters into the message given
   it and passes the resulting string to the user error handler.
   In the future, dc_exception should simply pass an error code and
   the parameters on to the user error handler.
*/

#include <varargs.h>
extern int vsprintf();

/* Exit status for exception-processing machinery failure */
#define DC_EXCEPTION_FAILURE    250

void dc_exception(continuable, args)
   dc_boolean_t continuable;
   va_list args;
{
   dc_handle_t handle;
   char *format;
   char buffer[1024];

   handle = va_arg(args, dc_handle_t);
   format = va_arg(args, char *);
   (void) vsprintf(buffer, format, args);
   (*(handle->error_fcn)) (handle->error_env, continuable, buffer);
   if (!continuable)
      exit(DC_EXCEPTION_FAILURE);  /* User error handler should never return in this case. */
}


void dc_error(va_alist)  /* (handle, format, args... ) */
   va_dcl
{
   va_list args;

   va_start(args);
   dc_exception(DC_FALSE, args);
   va_end(args);
}


void dc_warn(va_alist)  /* (handle, format, args... ) */
   va_dcl
{
   va_list args;

   va_start(args);
   dc_exception(DC_TRUE, args);
   va_end(args);
}



#define MALLOC_FAILURE_MESSAGE "Heap space exhausted (malloc failed)."
#define CALLOC_FAILURE_MESSAGE "Heap space exhausted (Calloc failed)."


/* Commonize memory allocation call so failure diagnosis is easier */

char* dc_malloc( handle, size )
    dc_handle_t handle;
    int         size;
{
    char* space = malloc( size );
    if (space == (char *)NULL)
        dc_error( handle, MALLOC_FAILURE_MESSAGE );

    return space;
}


/* Commonize memory allocation call so failure diagnosis is easier */

char* dc_calloc( handle,nelem, size )
    dc_handle_t handle;
    int         nelem;
    int		size;
{
    char* space = calloc( nelem, size );
    if (space == (char *)NULL)
        dc_error( handle, CALLOC_FAILURE_MESSAGE );

    return space;
}


dc_word_t dc_read_word (handle, address)
   dc_handle_t handle;
   dc_word_t address;
{
   dc_word_t word;
   (*(handle->read_fcn)) (handle->read_env, address,
                          sizeof(dc_word_t), (char *)(&(word)));
   return word;
}


void dc_write_word (handle, address, value)
   dc_handle_t handle;
   dc_word_t address;
   dc_word_t value;
{
   dc_word_t word;
   word = value;
   if (handle->write_fcn) {
      (*(handle->write_fcn)) (handle->write_env, address,
                              sizeof(dc_word_t), (char *)(&(word)));
   } else {
      dc_error (handle, "Writing is disabled.");
   }
}


void dc_write_masked_word (handle, address, mask, value)
   dc_handle_t handle;
   dc_word_t address;
   dc_word_t mask;
   dc_word_t value;
{
   dc_write_word (handle, address,
      (value & mask) | (dc_read_word(handle, address) & ~mask));
}


dc_handle_t dc_initiate (debug_info_ptr,
			 error_fcn, error_env,
                         read_fcn, read_env,
                         write_fcn, write_env,
                         exec_fcn, exec_env,
                         map_fcn, map_env)
   dc_word_t debug_info_ptr;
   void (*error_fcn)();
   dc_word_t error_env;
   void (*read_fcn)();
   dc_word_t read_env;
   void (*write_fcn)();                 /* NULL => absent */
   dc_word_t write_env;
   void (*exec_fcn)();                  /* NULL => absent */
   dc_word_t exec_env;
   void (*map_fcn)();                  /* NULL => absent */
   dc_word_t map_env;
   /* write_fcn may be given as NULL if no writing is required. */
   /* exec_fcn may be given as NULL if no execution is required.
      Currently, no execution is required.  It would be if the
      implementation needed to invoke procedures in the debugged process. */
{
   dc_handle_t handle;
   unsigned int debug_info_protocol;
   dc_debug_info_t debug_info;
   unsigned int tdesc_map_protocol;
   tdesc_hdr_t tdesc_hdr;
   dc_word_t tdesc_info_start;
   dc_word_t tdesc_info_end;
   dc_word_t tdesc_info_length;

   /* Set up handle enough for dc_error. */
   handle = (dc_handle_t) malloc(sizeof(handle_info_t));
   /* Cant use dc_malloc() as handle is being created ... */
   /* if (handle == NULL) (*error_fcn)( error_env, MALLOC_FAILURE_MESSAGE ) */
   handle->error_fcn = error_fcn;
   handle->error_env = error_env;
   handle->read_fcn = read_fcn;
   handle->read_env = read_env;
   handle->write_fcn = write_fcn;
   handle->write_env = write_env;
   handle->exec_fcn = exec_fcn;
   handle->exec_env = exec_env;
/****************************************************************/
/* BUG 9/19/89 Found by hls.  Map functions not initialized.    */
/****************************************************************/
   handle->map_fcn = map_fcn;
   handle->map_env = map_env;
   handle->debug_info_ptr = debug_info_ptr;
   handle->tdesc_table = (tdesc_table_t)NULL;

   /* Find tdesc info. */
   if (debug_info_ptr) {
      (*read_fcn) (read_env, debug_info_ptr, sizeof(unsigned int),
		   (char *)(&debug_info_protocol));
      if (debug_info_protocol != 1)
	 dc_error (handle, "Unrecognized debug info protocol: %d",
		   debug_info_protocol);
      (*read_fcn) (read_env, debug_info_ptr, sizeof(dc_debug_info_t),
		   (char *)(&debug_info));
      (*read_fcn) (read_env, debug_info.tdesc_ptr, sizeof(unsigned int),
		   (char *)(&tdesc_map_protocol));
      if (tdesc_map_protocol != 1)
	 dc_error (handle, "Unrecognized tdesc map protocol: %d",
		   tdesc_map_protocol);
      (*read_fcn) (read_env, debug_info.tdesc_ptr, sizeof(tdesc_hdr_t),
		   (char *)(&tdesc_hdr));
      tdesc_info_start = debug_info.tdesc_ptr + sizeof(tdesc_hdr_t);
      tdesc_info_end = tdesc_hdr.end;
      tdesc_info_length = tdesc_info_end - tdesc_info_start;

      /* Create tdesc table from tdesc info. */
      {
	 /* Over-allocate in order to avoid second pass over tdesc info. */
	 tdesc_table_t tt = (tdesc_table_t) dc_malloc(handle, tdesc_info_length);
	 dc_word_t p = tdesc_info_start;
	 dc_word_t q = tdesc_info_end - sizeof(tdesc_chunk1_t);
	 int n = 0;
	 tdesc_chunk1_t chunk;
	 dc_word_t start_address, end_address;
	 int i;

	 for (; p <= q; ) {
	    (*read_fcn) (read_env, p, sizeof(tdesc_chunk1_t), (char *)(&chunk));
	    if (chunk.hdr.zeroes != 0) {
	       /* Skip padding. */
	       p += sizeof(dc_word_t);
	       continue;
	    }
	    if (chunk.hdr.info_protocol != 1) {
	       dc_warn (handle, "Unrecognized tdesc info protocol: %d",
			 chunk.hdr.info_protocol);
	       goto next_chunk;
	    }
	    if (chunk.hdr.info_length != 16) {
	       dc_warn (handle, "Incorrect tdesc info length: %d",
			 chunk.hdr.info_length);
	       goto next_chunk;
	    }
	    if (chunk.hdr.info_alignment > 2) {
	       dc_warn (handle, "Incorrect tdesc info alignment: %d",
			 chunk.hdr.info_alignment);
	       goto next_chunk;
	    }
	    start_address = chunk.hdr.start_address;
	    end_address = chunk.hdr.end_address;
	    if ((start_address&3)!=0) {
	       dc_warn (handle,
		  "Tdesc start address is not word-aligned: %#.8X",
		  start_address);
	       goto next_chunk;
	    }
	    if ((end_address&3)!=0) {
	       dc_warn (handle,
		  "Tdesc end address is not word-aligned: %#.8X",
		  end_address);
	       goto next_chunk;
	    }
	    if (start_address > end_address) {
		     /* Note that the range may be null. */
	       dc_warn (handle,
		  "Tdesc start address (%#.8X) follows end address (%#.8X).",
		  start_address, end_address);
	       goto next_chunk;
	    }
	    if (chunk.info.variant != 1) {
	       dc_warn (handle, "Invalid tdesc chunk variant: %d",
		  chunk.info.variant);
	       goto next_chunk;
	    }
	    if (chunk.info.pad1 != 0) {
	       dc_warn (handle, "Tdesc chunk padding is not zero.");
	       goto next_chunk;
	    }
	    if (chunk.info.return_address_info_discriminant != 0) {
	       if ((chunk.info.return_address_info & 3) != 0) {
		  dc_warn (handle,
		     "Tdesc return address offset is not word-aligned: %#.8X",
		     chunk.info.return_address_info);
		  goto next_chunk;
	       }
	    } else {
	       if ((chunk.info.return_address_info & ~31) != 0) {
		  dc_warn (handle,
		     "Invalid tdesc return address register: %d",
		     chunk.info.return_address_info);
		  goto next_chunk;
	       }
	    }
	    if ((chunk.info.register_save_offset & 3) != 0) {
	       dc_warn (handle,
		  "Tdesc register save offset is not word-aligned: %#.8X",
		  chunk.info.register_save_offset);
	       goto next_chunk;
	    }

	    tt[n].start_address = start_address;
	    tt[n].end_address = end_address;
	    tt[n].info = chunk.info;
	    n++;

   next_chunk:
	    p += sizeof(tdesc_chunk1_t);
	 }
	 /* Leftover (less than a tdesc_chunk1_t in size) is padding or
	    in error.  Ignore it in either case. */

	 if (n != 0) {

		 /* Sort table by start address. */
		 qsort ((char *)tt, n, sizeof(tdesc_elem_t), dc_compare_tdesc_elems);

		 /* Check for overlap among tdesc chunks. */
		 for (i=0; i<(n-1); i++) {
		    if (tt[i].end_address > tt[i+1].start_address)
		       dc_error (handle, "Text chunks overlap.");
		 }
	 }

	 /* Finish setting up handle. */
	 handle->tdesc_table = tt;
	 handle->tdesc_table_size = n;
      }
   } else {
      handle->tdesc_table_size = 0;
   }

   return (dc_handle_t) handle;
}


void dc_terminate (handle)
   dc_handle_t handle;
{
   if (((dc_handle_t)handle)->tdesc_table) {
      free((char *)(((dc_handle_t)handle)->tdesc_table));
   }
   free((char *)handle);
}



/*

   Dcontext Model

   For each interesting register (word-sized piece of machine state),
   a word of value information is kept.  This word may
   be either the value of the register, or the address in
   subject memory where the value can be found (and changed).  In
   addition, the register may be invalid (in which case the value
   information is undefined).  These three cases are encoded for
   a given register in the same-numbered bit of two words of flags:

      flags[0] bit  flags[1] bit  meaning
      ------------  ------------  -------
            0             0       register is invalid; info is undefined
            0             1       register is readable; info is value
            1             0       register is writable; info is address
            1             1       (reserved)

   The general registers (r0-r31) are handled by reg_info and
   reg_flags.  The bit number for a register is that register's number.
   The other registers are grouped together for convenience and are
   handled by aux_info and aux_flags.  The bit numbers for these
   registers are:

      bit number     register
      ----------     --------
           0         location
           1         SXIP
           2         SNIP
           3         SFIP
           4         FPSR
           5         FPCR

   The SXIP, SNIP, and SFIP are the exception-time values of the
   XIP, NIP, and FIP registers.  They are valid only in the topmost frame.
   (That is, in any context obtained from dc_previous_context, they
   are invalid.)

   "location" is a pseudo-register of this model and represents the
   location of the context.  It is always valid.  It also has an
   exactness associated with it.  The location and its exactness of a
   context obtained from dc_previous_context are taken from the
   return address and its exactness of the context given as an argument
   to dc_previous_context.

   The following model is recommended for dealing with the partial
   redundancy between location and the SXIP, SNIP, and SFIP values
   in the topmost frame.  The location should be set to either the
   SNIP or SXIP value, and its exactness should be set to DC_NO.  A
   change to the register whose value the location is set to should
   be accompanied by an identical change to the location.

   The PSR is handled separately, because it is a diverse collection
   of flags.  The PSR, as a whole, is always valid.  A separate
   psr_ind flag tells whether the psr_info data is a value or
   an address.  Each bit of the PSR has its own pair of flag bits to
   mark validity and writability.

*/


/* The following value means "other", because state is stored in 2 bits. */
#define DC_RESERVED 3


#define RSTATE(flags, bit) \
   ((bit_value((flags)[0], bit) << 1) + bit_value((flags)[1], bit))

#define REG_STATE(dcontext, reg) RSTATE(dcontext->reg_flags, reg)
#define AUX_STATE(dcontext, reg) RSTATE(dcontext->aux_flags, reg)
#define PSR_STATE(dcontext, reg) RSTATE(dcontext->psr_flags, reg)


#define SET_INVALID(flags, bit) \
   { bit_clear ((flags)[0], bit); bit_clear ((flags)[1], bit); }

#define SET_READABLE(flags, bit) \
   { bit_clear ((flags)[0], bit); bit_set   ((flags)[1], bit); }

#define SET_WRITABLE(flags, bit) \
   { bit_set   ((flags)[0], bit); bit_clear ((flags)[1], bit); }

#define ASSIGN_RSTATE(to_flags, to_bit, from_flags, from_bit) \
   { bit_assign ((to_flags)[0], to_bit, bit_value((from_flags)[0], from_bit));\
     bit_assign ((to_flags)[1], to_bit, bit_value((from_flags)[1], from_bit));}


#define CHECK_REG_READ(dcontext, reg) \
   if (REG_STATE(dcontext, reg) == DC_INVALID) \
      dc_error (dcontext->handle, \
         "General register %d is not readable.", reg)

#define CHECK_REG_WRITE(dcontext, reg) \
   if (REG_STATE(dcontext, reg) != DC_WRITABLE) \
      dc_error (dcontext->handle, \
         "General register %d is not writable.", reg)

#define CHECK_AUX_READ(dcontext, reg) \
   if (AUX_STATE(dcontext, reg) == DC_INVALID) \
      dc_error (dcontext->handle, \
         "Auxiliary register %d is not readable.", reg)

#define CHECK_AUX_WRITE(dcontext, reg) \
   if (AUX_STATE(dcontext, reg) != DC_WRITABLE) \
      dc_error (dcontext->handle, \
         "Auxiliary register %d is not writable.", reg)



#define DC_REG_RA   1
#define DC_REG_FP  30
#define DC_REG_SP  31
#define DC_NUM_REG 32

#define DC_AUX_LOC  0
   /* DC_AUX_LOC must be first, with value 0 */
#define DC_AUX_SXIP 1
#define DC_AUX_SNIP 2
#define DC_AUX_SFIP 3
#define DC_AUX_FPSR 4
#define DC_AUX_FPCR 5
#define DC_NUM_AUX  6



#define CHECK_REG(dcontext, reg) \
   if ((reg < 0) || (reg >= DC_NUM_REG)) \
      dc_error (dcontext->handle, \
         "Bad general register number: %d", reg)

#define CHECK_AUX(dcontext, reg) \
   if ((reg < 1) || (reg >= DC_NUM_AUX)) \
      dc_error (dcontext->handle, \
         "Bad auxiliary register number: %d", reg)
   /* CHECK_AUX is not used for location pseudo-register. */

#define CHECK_BIT(dcontext, bit) \
   if ((bit < 0) || (bit >= 32)) \
      dc_error (dcontext->handle, \
         "Bad bit number: %d", bit)



typedef struct cr_value {
   int reg;
   unsigned int off;
 } dc_cr_value_t;

#define DC_UNDEF 32

/*
   A "dc_cr_value" represents an execution-time value symbolically, in
   terms of the initial value of a register (the value on entry to
   the procedure being analyzed) and a known offset.  A value with
   a 'reg' field value of 0 through 31 represents the value obtained
   by summing (using 32-bit modulus arithmetic) the initial value of
   register 'reg' and the value 'off'.  Note that the value (0,k)
   represents the constant value k, that (31,0) represents the CFA, and
   that (1,0) represents the return address.  A value with a 'reg' field
   of DC_UNDEF represents an indeterminable value; in this case the
   'off' field is undefined.  Other values of 'reg' are erroneous.
*/

typedef struct cr_data {
   dc_cr_value_t reg_val[DC_NUM_REG];
   dc_word_t saved;
   dc_word_t how;
   unsigned int where[DC_NUM_REG];
} dc_cr_data_t;

/*
   'cr_data' collects all the information needed to represent the
   symbolic machine state during code reading.

   The 'reg_val' array gives the current dc_cr_value for each register.
   
   The 'saved', 'how', and 'where' fields combine to describe what
   registers have been saved, and where.  The 'saved' and 'how' fields
   are implicitly bit arrays over 0..31, where the numbering is from
   0 on the right.  (Hence, 1<<r gives the mask for register r.)
   If saved[r] is 0, the register is not saved, and how[r] and where[r]
   are undefined.  If saved[r] is 1, then how[r] tells whether register r
   was saved in another register (how[r]==0) or in the frame (how[r]==1).
   In the former case, where[r] gives the register number; in the latter
   case, where[r] gives the frame position.
*/


typedef int dc_register_state_t;    /* range 0 to 2 */

#define DC_INVALID  0
#define DC_READABLE 1
#define DC_WRITABLE 2




typedef struct dcontext_info {
   dc_handle_t handle;                     /* environment of context */
   dc_word_t reg_info[DC_NUM_REG];
   dc_word_t reg_flags[2];
   dc_word_t aux_info[DC_NUM_AUX];
   dc_word_t aux_flags[2];
   dc_exactness_t loc_exact;
   dc_word_t psr_info;                     /* value or address */
   dc_word_t psr_ind;                      /* DC_TRUE iff address */
   dc_word_t psr_flags[2];                 /* per-PSR-bit flags */
   unsigned int code_reading;		       /* no tdesc therefore must read code*/
	 union {
	   tdesc_elem_t *tdesc_elem_ptr;       /* locates tdesc chunk */
	   dc_cr_data_t *cr_data_ptr;          /* or code reading data */
	 } info_ptr;
} dcontext_info_t;

typedef dcontext_info_t *dc_dcontext_t;

dc_word_t dc_get_value (handle, info, flags, pos)
   dc_handle_t handle;
   dc_word_t info[];
   dc_word_t flags[2];
   int pos;
   /* Assumes either DC_READABLE or DC_WRITABLE. */
{
   if (bit_test(flags[0], pos)) {
      /* DC_WRITABLE case */
      return dc_read_word(handle, info[pos]);
   } else {
      /* DC_READABLE case */
      return info[pos];
   }
}

void dc_set_value (handle, info, flags, pos, value)
   dc_handle_t handle;
   dc_word_t info[];
   dc_word_t flags[2];
   int pos;
   dc_word_t value;
   /* Assumes DC_WRITABLE. */
{
   dc_write_word(handle, info[pos], value);
}


#define GET_REG_VALUE(dcontext, reg) \
   dc_get_value(dcontext->handle, dcontext->reg_info, dcontext->reg_flags, reg)

#define SET_REG_VALUE(dcontext, reg, value) \
   dc_set_value(dcontext->handle, dcontext->reg_info, dcontext->reg_flags, reg, \
      value)

#define GET_AUX_VALUE(dcontext, reg) \
   dc_get_value(dcontext->handle, dcontext->aux_info, dcontext->aux_flags, reg)

#define SET_AUX_VALUE(dcontext, reg, value) \
   dc_set_value(dcontext->handle, dcontext->aux_info, dcontext->aux_flags, reg, \
      value)



void dc_check_dcontext (dc)
   dc_dcontext_t dc;
   /* Check consistency of information supplied to make a dcontext. */
{
   int i;

   if ((REG_STATE(dc, 0) != DC_READABLE) || (dc->reg_info[0] != 0))
      dc_error (dc->handle, "Register 0 is misspecified");
   for (i = 1; i < DC_NUM_REG; i++)
      if (REG_STATE(dc, i) == DC_RESERVED)
         dc_error (dc->handle,
            "State for general register %d is incorrect", i);
   for (i = 0; i < DC_NUM_AUX; i++)
      if (AUX_STATE(dc, i) == DC_RESERVED)
         dc_error (dc->handle,
            "State for auxiliary register %d is incorrect", i);
   if (AUX_STATE(dc, DC_AUX_LOC) == DC_INVALID)
      dc_error (dc->handle, "Location is specified as invalid");
   if (GET_AUX_VALUE(dc, DC_AUX_LOC) == 0)
      dc_error (dc->handle, "Location is zero.");
   if (dc->loc_exact >= 3)
      dc_error (dc->handle, "Location exactness is incorrectly specified: %d",
         dc->loc_exact);
   if (dc->psr_ind >= 2)
      dc_error (dc->handle,
         "PSR indirection flag is incorrectly specified: %d",
         dc->psr_ind);
   for (i = 0; i < 32; i++)
      if (PSR_STATE(dc, i) == DC_RESERVED)
         dc_error (dc->handle, "State for PSR bit %d is incorrect", i);
}



tdesc_elem_t * dc_tdesc_lookup (loc, tt, tt_size, map_info_in_ptr)
   dc_word_t loc;
   tdesc_table_t tt;
   int tt_size;
   dc_map_info_in_t *map_info_in_ptr;
   /* Return address of tdesc_elem_t for given location, or NULL if
      there is no tdesc chunk for the location.
   */
{
   int l = 0;
   int h = tt_size;
   int m;

   if (tt_size == 0) {
      map_info_in_ptr->flags = 0;
      return (tdesc_elem_t *)NULL;
   }
   for (;;) {
      m = (l + h) / 2;
      if (m == l) break;
      if (loc >= tt[m].start_address)
         l = m;
      else
         h = m;
   }
   if (loc >= tt[m].end_address) {
      map_info_in_ptr->preceding_tdesc_end = tt[m].end_address;
      if (m+1 < tt_size) {
	 map_info_in_ptr->following_tdesc_start = tt[m+1].start_address;
	 map_info_in_ptr->flags = DC_MII_PRECEDING_TDESC_END |
	                          DC_MII_FOLLOWING_TDESC_START;
      } else {
	 map_info_in_ptr->flags = DC_MII_PRECEDING_TDESC_END;
      }
      return (tdesc_elem_t *)NULL;
   } else if (loc < tt[m].start_address) {
      map_info_in_ptr->following_tdesc_start = tt[m].start_address;
      map_info_in_ptr->flags = DC_MII_FOLLOWING_TDESC_START;
      return (tdesc_elem_t *)NULL;
   } else {
      return (&tt[m]);
   }
}



dc_dcontext_t dc_make_dcontext (handle,
                                reg_info, reg_flags,
                                aux_info, aux_flags, loc_exact,
                                psr_info, psr_ind, psr_flags)
   dc_handle_t handle;
   dc_word_t reg_info[DC_NUM_REG];
   dc_word_t reg_flags[2];
   dc_word_t aux_info[DC_NUM_AUX];
   dc_word_t aux_flags[2];
   dc_exactness_t loc_exact;
   dc_word_t psr_info;
   dc_boolean_t psr_ind;
   dc_word_t psr_flags[2];
{
   dc_dcontext_t dc = (dc_dcontext_t) dc_malloc (handle, sizeof(dcontext_info_t));
   int i;
   dc_map_info_in_t map_info_in;

   /* Fill in supplied content. */
   dc->handle = ((dc_handle_t)handle);
   for (i = 0; i < DC_NUM_REG; i++) dc->reg_info[i]  = reg_info[i];
   for (i = 0; i < 2; i++)       dc->reg_flags[i] = reg_flags[i];
   for (i = 0; i < DC_NUM_AUX; i++) dc->aux_info[i]  = aux_info[i];
   for (i = 0; i < 2; i++)       dc->aux_flags[i] = aux_flags[i];
   dc->loc_exact = loc_exact;
   dc->psr_info = psr_info;
   dc->psr_ind = psr_ind;
   for (i = 0; i < 2; i++)       dc->psr_flags[i] = psr_flags[i];

   dc_check_dcontext(dc);

   /* Find tdesc information for the text chunk. */
   {
/***************************************************************/
/* BUG 8/16/89 Found by hls.  Not zeroing EV bits of location. */
/*                            SHOULD USE dc_location()!        */
/*      dc_word_t loc = GET_AUX_VALUE(dc, DC_AUX_LOC);         */
/***************************************************************/
      dc_word_t loc = GET_AUX_VALUE(dc, DC_AUX_LOC) & ~3;
      tdesc_elem_t *tep =
         dc_tdesc_lookup(loc, ((dc_handle_t)handle)->tdesc_table,
             ((dc_handle_t)handle)->tdesc_table_size,&map_info_in);
      if (tep) {
		 dc->code_reading = 0;
         dc->info_ptr.tdesc_elem_ptr = tep;
      } else {
	  dc->code_reading = 1;
	  if (!dc->handle->map_fcn) {
	      dc_error (dc->handle, "No tdesc information for %#.8X and no map function supplied.",loc);
	  }
/****************************************************************/
/* BUG 9/18/89 Found by hls.  Not using dc_malloc()             */
/* dc->info_ptr.cr_data_ptr= (dc_cr_data_t *)malloc(sizeof(dc_cr_data_t )); */
/****************************************************************/
	  dc->info_ptr.cr_data_ptr= (dc_cr_data_t *)dc_calloc(dc->handle,1,sizeof(dc_cr_data_t ));
	  dc_read_code(loc,dc,map_info_in,dc->info_ptr.cr_data_ptr);
      }
   }

   return (dc_dcontext_t) dc;
}



void dc_free_dcontext (dcontext)
   dc_dcontext_t dcontext;
{
/****************************************************************/
/* BUG 9/19/89 Found by hls.  Freeing non-pointer value.        */
/*	  free((char *)dcontext->code_reading);                 */
/****************************************************************/
   if (dcontext->code_reading)
	  free((char *)dcontext->info_ptr.cr_data_ptr);
   free((char *)dcontext);
}



dc_register_state_t dc_location_state (dcontext)
   dc_dcontext_t dcontext;
{
   return AUX_STATE(((dc_dcontext_t)dcontext), DC_AUX_LOC);
}


dc_exactness_t dc_location_exactness (dcontext)
   dc_dcontext_t dcontext;
{
   return ((dc_dcontext_t)dcontext)->loc_exact;
}


dc_word_t dc_location (dcontext)
   dc_dcontext_t dcontext;
   /* Return high 30 bits only. */
{
   /* Don't need: CHECK_AUX_READ (((dc_dcontext_t)dcontext), DC_AUX_LOC); */
   return GET_AUX_VALUE (((dc_dcontext_t)dcontext), DC_AUX_LOC) & ~3;
}


dc_boolean_t dc_location_in_text_chunk( dcontext, value )
   dc_dcontext_t dcontext;
   dc_word_t value;
{
   /* Check that new location is still within same text chunk. */
   tdesc_elem_t *tep = ((dc_dcontext_t)dcontext)->info_ptr.tdesc_elem_ptr;
/********************************************************************/
/* Bug in predicate -- LS adjusted according to OCS documentation.. */
/*  if ((value < tep->start_address) || (value >= tep->end_address))*/
/********************************************************************/
   if ((value >= tep->start_address) && (value < tep->end_address))
        return DC_TRUE;
    else
        return DC_FALSE;

}


void dc_set_location (dcontext, value)
   dc_dcontext_t dcontext;
   dc_word_t value;
   /* Set high 30 bits only. */
{
   if (dc_location_in_text_chunk( dcontext, value ) != DC_TRUE)
      dc_warn (((dc_dcontext_t)dcontext)->handle,
         "New location is not in same text chunk.");

   CHECK_AUX_WRITE (((dc_dcontext_t)dcontext), DC_AUX_LOC);
   dc_write_masked_word (((dc_dcontext_t)dcontext)->handle,
      ((dc_dcontext_t)dcontext)->aux_info[DC_AUX_LOC], ~3, value);
}



dc_register_state_t dc_general_register_state (dcontext, reg)
   dc_dcontext_t dcontext;
   int reg;
{
   CHECK_REG (((dc_dcontext_t)dcontext), reg);
   return REG_STATE(((dc_dcontext_t)dcontext), reg);
}


dc_word_t dc_general_register (dcontext, reg)
   dc_dcontext_t dcontext;
   int reg;
{
   CHECK_REG (((dc_dcontext_t)dcontext), reg);
   CHECK_REG_READ (((dc_dcontext_t)dcontext), reg);
   return GET_REG_VALUE(((dc_dcontext_t)dcontext), reg);
}


void dc_set_general_register (dcontext, reg, value)
   dc_dcontext_t dcontext;
   int reg;
   dc_word_t value;
{
   CHECK_REG (((dc_dcontext_t)dcontext), reg);
   CHECK_REG_WRITE (((dc_dcontext_t)dcontext), reg);
   SET_REG_VALUE (((dc_dcontext_t)dcontext), reg, value);
}



dc_register_state_t dc_auxiliary_register_state (dcontext, reg)
   dc_dcontext_t dcontext;
   int reg;
{
   CHECK_AUX (((dc_dcontext_t)dcontext), reg);
   return AUX_STATE(((dc_dcontext_t)dcontext), reg);
}


dc_word_t dc_auxiliary_register (dcontext, reg)
   dc_dcontext_t dcontext;
   int reg;
{
   CHECK_AUX (((dc_dcontext_t)dcontext), reg);
   CHECK_AUX_READ (((dc_dcontext_t)dcontext), reg);
   return GET_AUX_VALUE(((dc_dcontext_t)dcontext), reg);
}


void dc_set_auxiliary_register (dcontext, reg, value)
   dc_dcontext_t dcontext;
   int reg;
   dc_word_t value;
{
   CHECK_AUX (((dc_dcontext_t)dcontext), reg);
   CHECK_AUX_WRITE (((dc_dcontext_t)dcontext), reg);
   SET_AUX_VALUE (((dc_dcontext_t)dcontext), reg, value);
}



dc_register_state_t dc_psr_register_bit_state (dcontext, bit)
   dc_dcontext_t dcontext;
   int bit;
{
   CHECK_BIT (((dc_dcontext_t)dcontext), bit);
   return PSR_STATE(((dc_dcontext_t)dcontext), bit);
}


dc_word_t dc_psr_register (dcontext)
   dc_dcontext_t dcontext;
{
   if (((dc_dcontext_t)dcontext)->psr_ind) {
      return dc_read_word(((dc_dcontext_t)dcontext)->handle,
                       ((dc_dcontext_t)dcontext)->psr_info);
   } else {
      return ((dc_dcontext_t)dcontext)->psr_info;
   }
}


void dc_set_psr_register (dcontext, mask, value)
   dc_dcontext_t dcontext;
   dc_word_t mask;
   dc_word_t value;
   /* Set bits of PSR corresponding to 1 bits in mask. */
{
   if (((dc_dcontext_t)dcontext)->psr_ind) {
      if (((((dc_dcontext_t)dcontext)->psr_flags[0] & mask) != mask) ||
          ((((dc_dcontext_t)dcontext)->psr_flags[1] & mask) != 0))
         dc_error (((dc_dcontext_t)dcontext)->handle,
            "Some PSR bits specified are not writable.");
      dc_write_masked_word (((dc_dcontext_t)dcontext)->handle,
                         ((dc_dcontext_t)dcontext)->psr_info, mask, value);
   } else {
      dc_error (((dc_dcontext_t)dcontext)->handle, "PSR is not writable.");
   }
}



dc_word_t dc_frame_address (dcontext)
   dc_dcontext_t dcontext;
{
  if (!dcontext->code_reading) {
	 tdesc_elem_t *tep = ((dc_dcontext_t)dcontext)->info_ptr.tdesc_elem_ptr;
	 return dc_general_register(dcontext,
		tep->info.frame_address_register) + tep->info.frame_address_offset;
  } else {
	 if (dcontext->info_ptr.cr_data_ptr->reg_val[DC_REG_FP].reg == DC_REG_SP) {
		return (dc_general_register(dcontext,DC_REG_FP)
			- dcontext->info_ptr.cr_data_ptr->reg_val[DC_REG_FP].off);
	 }
	 if (dcontext->info_ptr.cr_data_ptr->reg_val[DC_REG_SP].reg == DC_REG_SP) {
		return (dc_general_register(dcontext,DC_REG_SP)
			- dcontext->info_ptr.cr_data_ptr->reg_val[DC_REG_SP].off);
	 }
     dc_error (((dc_dcontext_t)dcontext)->handle, "Cannot locate frame pointer.");
  }
}



dc_kind_t dc_context_kind (dcontext)
   dc_dcontext_t dcontext;
{
   return DC_CALL_KIND;
}




/* operations valid for call contexts only */


dc_register_state_t dc_return_address_state (dcontext)
   dc_dcontext_t dcontext;
{
   tdesc_elem_t *tep = ((dc_dcontext_t)dcontext)->info_ptr.tdesc_elem_ptr;
   int reg;

   if (!dcontext->code_reading) {
	  if (tep->info.return_address_info_discriminant) {
	 	 return DC_WRITABLE;
	  } else {
		 return REG_STATE(((dc_dcontext_t)dcontext), tep->info.return_address_info);
	  }
   } else {
	  reg= DC_REG_RA;
	  if (bit_test(dcontext->info_ptr.cr_data_ptr->saved,DC_REG_RA)) {
		if (bit_test(dcontext->info_ptr.cr_data_ptr->how,DC_REG_RA)) {
		   return DC_WRITABLE;
		} else {
		   reg= dcontext->info_ptr.cr_data_ptr->where[DC_REG_RA];
		}
	  }
		return REG_STATE(((dc_dcontext_t)dcontext),reg);

	  
   }
}


dc_exactness_t dc_return_address_exactness (dcontext)
   dc_dcontext_t dcontext;
{
   return DC_MAYBE;
}


dc_word_t dc_return_address (dcontext)
   dc_dcontext_t dcontext;
   /* Return high 30 bits only. */
{
   tdesc_elem_t *tep = ((dc_dcontext_t)dcontext)->info_ptr.tdesc_elem_ptr;
   dc_word_t rai = tep->info.return_address_info;
   dc_word_t val;
   int reg;

   if (!dcontext->code_reading) {
	 if (tep->info.return_address_info_discriminant) {
		val = dc_read_word (((dc_dcontext_t)dcontext)->handle,
						 dc_frame_address(dcontext) + rai);
	 } else {
		val = dc_general_register (dcontext, rai);
	 }
   } else {
	  reg=DC_REG_RA;
	  if (bit_test(dcontext->info_ptr.cr_data_ptr->saved,reg)) {
		  if (bit_test(dcontext->info_ptr.cr_data_ptr->how,reg)) {
			 val = dc_read_word (((dc_dcontext_t)dcontext)->handle,
						   dc_frame_address(dcontext) + 
						    (dcontext->info_ptr.cr_data_ptr->where[reg]));
		  } else {
			 reg= dcontext->info_ptr.cr_data_ptr->where[DC_REG_RA];
			 val = dc_general_register (dcontext, reg);
		  }
	  } else {
		 val = dc_general_register (dcontext, reg);
	  }
   }
	 return val & ~3;
}


void dc_set_return_address (dcontext, value)
   dc_dcontext_t dcontext;
   dc_word_t value;
   /* Set high 30 bits only. */
{
  if (!dcontext->code_reading) {
	 tdesc_elem_t *tep = ((dc_dcontext_t)dcontext)->info_ptr.tdesc_elem_ptr;
	 dc_word_t rai = tep->info.return_address_info;

	 if (tep->info.return_address_info_discriminant) {
		dc_write_masked_word (((dc_dcontext_t)dcontext)->handle,
		   dc_frame_address(dcontext) + rai, ~3, value);
	 } else {
      dc_set_general_register (dcontext, rai,
         (value & ~3) | (dc_general_register(dcontext, rai) & 3));
   }
 } else {
	if (bit_test(dcontext->info_ptr.cr_data_ptr->saved,DC_REG_RA)) {
		if (bit_test(dcontext->info_ptr.cr_data_ptr->how,DC_REG_RA)) {
		   dc_write_masked_word (((dc_dcontext_t)dcontext)->handle,
		      dc_frame_address(dcontext)
			   + dcontext->info_ptr.cr_data_ptr->where[DC_REG_RA], ~3, value);
		  } else {
			 dc_set_general_register( dcontext,
								  dcontext->info_ptr.cr_data_ptr->where[DC_REG_RA]);
		  }
	  } else {
		  dc_set_general_register( dcontext,
							  dcontext->info_ptr.cr_data_ptr->where[DC_REG_RA]);
	  }
   }
}



/* operations valid for save contexts only */

/* (none) */



/* operations valid for exception contexts only */


void dc_get_exception_info (dcontext, handler, datum)
   dc_dcontext_t dcontext;
   dc_word_t *handler;
   dc_word_t *datum;
{
   dc_error (((dc_dcontext_t)dcontext)->handle,
      "dc_get_exception_info is not yet implemented.");
}



/* operations valid for protection contexts only */


void dc_get_protection_info (dcontext, handler, datum)
   dc_dcontext_t dcontext;
   dc_word_t *handler;
   dc_word_t *datum;
{
   dc_error (((dc_dcontext_t)dcontext)->handle,
      "dc_get_protection_info is not yet implemented.");
}



/* operations valid for special contexts only */


void dc_get_special_info (dcontext, kind, datum)
   dc_dcontext_t dcontext;
   dc_word_t *kind;
   dc_word_t *datum;
{
   dc_error (((dc_dcontext_t)dcontext)->handle,
      "dc_get_special_info is not yet implemented.");
}



/* operations valid for all contexts (again) */


dc_dcontext_t dc_previous_dcontext (dcontext)
   dc_dcontext_t dcontext;
   /* Return NULL if there is no previous context. */
{
   dc_dcontext_t old = (dc_dcontext_t) dcontext;
   dcontext_info_t new;    /* to serve as temporary storage only */
   tdesc_elem_t *tep;
	dc_cr_data_t *cdp;
   dc_word_t cfa;
   int rsm;
   dc_word_t offset;
   dc_word_t rai;
   int r;

   if (dc_return_address_state((dc_dcontext_t)old) == DC_INVALID)
      dc_error (old->handle, "Return address is invalid.");

   if (dc_return_address((dc_dcontext_t)old) == 0)
      return (dc_dcontext_t)NULL;  /* end of the chain */

   /* Copy over old contents. */
   new = *old;

   cfa = dc_frame_address(old);
	/* Restore stack pointer. */
	new.reg_info[DC_REG_SP] = cfa;
	SET_READABLE (new.reg_flags, DC_REG_SP);

   /* Invalidate temporary registers. */
   for (r = 1; r <= 13; r++) SET_INVALID (new.reg_flags, r);

	if (!old->code_reading) {
		tep = old->info_ptr.tdesc_elem_ptr;
		/* Restore preserved registers. */
		rsm = tep->info.register_save_mask;
		offset = cfa + tep->info.register_save_offset;
		for (r = 14; r <= 30; r++) {
			if (bit_test(rsm, 30-r)) {
				new.reg_info[r] = offset;
				SET_WRITABLE (new.reg_flags, r);
				offset += sizeof(dc_word_t);
			}
		}

		/* Set location from old return address. */
		rai = tep->info.return_address_info;
		if (tep->info.return_address_info_discriminant) {
			new.aux_info[DC_AUX_LOC] = cfa + rai;
			SET_WRITABLE (new.aux_flags, DC_AUX_LOC);
		} else {
			new.aux_info[DC_AUX_LOC] = old->reg_info[rai];
			ASSIGN_RSTATE (new.aux_flags, DC_AUX_LOC, old->reg_flags, rai);
		}
	} else {
		cdp = old->info_ptr.cr_data_ptr;
		
		/* Restore preserved registers. */
		for (r = 14; r <= 30; r++) { 
			if (bit_test(cdp->saved,r)) {
				if (bit_test(cdp->how,r)){ /* saved in the frame */
					new.reg_info[r] = cfa+cdp->where[r];
					SET_WRITABLE (new.reg_flags, r);
				} else { /* saved in the in a register */
					new.reg_info[r] = dc_general_register(old,cdp->where[r]);
					ASSIGN_RSTATE (new.aux_flags, r, old->reg_flags, cdp->where[r]);
				}
			}  /* not saved, therefore, already valid , no else*/
		}

		/* Set location from old return address. */
		if (bit_test(cdp->saved,DC_REG_RA)) {
			if (bit_test(cdp->how,DC_REG_RA)){ /* saved in the frame */
				new.aux_info[DC_AUX_LOC] = 
					new.reg_info[DC_REG_RA] = cfa+cdp->where[DC_REG_RA];
				SET_WRITABLE (new.reg_flags, DC_REG_RA);
				SET_WRITABLE (new.aux_flags, DC_AUX_LOC);
			} else { /* saved in the in a register */
				new.reg_info[DC_REG_RA] =
					new.aux_info[DC_AUX_LOC] =
							dc_general_register(old,cdp->where[DC_REG_RA]);
				ASSIGN_RSTATE (new.aux_flags, DC_AUX_LOC, 
									old->reg_flags, cdp->where[DC_REG_RA]);
			}
		} else { /* not saved, therefore, already valid , set DC_AUX_LOC only*/
				new.aux_info[DC_AUX_LOC] =
						dc_general_register(old,DC_REG_RA);
				ASSIGN_RSTATE (new.aux_flags, DC_AUX_LOC, 
									old->reg_flags, DC_REG_RA);
		}
	}

   /* Invalidate instruction pointers. */
   SET_INVALID (new.aux_flags, DC_AUX_SXIP);
   SET_INVALID (new.aux_flags, DC_AUX_SNIP);
   SET_INVALID (new.aux_flags, DC_AUX_SFIP);

   /* No change to FCR registers. */

   /* No change to PSR register. */

   return dc_make_dcontext ((dc_handle_t)new.handle,
                            new.reg_info, new.reg_flags,
                            new.aux_info, new.aux_flags, new.loc_exact,
                            new.psr_info, new.psr_ind, new.psr_flags);
}



/* extensions for nonlocal goto */

#if 0

typedef
   struct label {
      ???
   } label_t;


label_t dc_make_label (dcontext, location)
   dc_dcontext_t dcontext;
   dc_word_t location;
{
}

#endif

/* procedure for reading code */

dc_read_code(loc,dc,map_info_in,cdp)
dc_word_t loc;
dc_dcontext_t dc;
dc_cr_data_t *cdp;
dc_map_info_in_t map_info_in;
{
dc_map_info_out_t map_info_out;
dc_word_t pc;
dc_boolean_t found_branch=DC_FALSE;
dc_word_t instr;

	(*dc->handle->map_fcn)(dc->handle->map_env,loc,map_info_in,&map_info_out);
	if (map_info_out.flags & DC_MIO_ENTRY_POINT 
		&& (!(map_info_in.flags & DC_MII_PRECEDING_TDESC_END)
		    || map_info_out.entry_point >= map_info_in.preceding_tdesc_end
			|| map_info_out.flags & DC_MIO_LITERAL_ENTRY_POINT)) {
	   dc_init_cr_data(cdp,(tdesc_elem_t *)NULL);
	   pc= map_info_out.entry_point;
	} else if (map_info_in.flags & DC_MII_PRECEDING_TDESC_END) { 
		/**/
		/* tdesc_lookup gets the tep for the preceeding tdesc information
		/* so we call it with one less than the preceding tdesc end since
		/* tdesc information is exclusive of the ending address
		/**/
	   dc_init_cr_data(cdp, 
					   dc_tdesc_lookup(map_info_in.preceding_tdesc_end-1, 
									((dc_handle_t)dc->handle)->tdesc_table,
             						((dc_handle_t)dc->handle)->tdesc_table_size,
									&map_info_in));
	   pc= map_info_in.preceding_tdesc_end;
	} else {
      dc_error (dc->handle, "Insufficient information for code reading.");
	}
	for (;;pc+=4) {
		if (pc==loc) {
		   return (DC_TRUE);
		}
		instr= dc_read_word(dc->handle,pc);
		found_branch= dc_decode_finds_branch(dc,instr);
		if ((map_info_out.flags & DC_MIO_PROLOGUE_END) 
			  && (pc==map_info_out.prologue_end)) {
			break;
		}
		if (found_branch) {
			if (DC_MIO_IMPLICIT_PROLOGUE_END & map_info_out.flags) {
				break;
			} else {
      			dc_error (dc->handle, "Found branch before end of prologue.");
			}
		}
	}
	if (!(map_info_out.flags & DC_MIO_LITERAL_EPILOGUE_START)
		 && (map_info_out.epilogue_start >= loc 
				|| !(map_info_out.flags & DC_MIO_EPILOGUE_START))) {
		return (DC_TRUE);
	}
	dc_correct_cr_data(cdp,dc->handle);
	for (pc=map_info_out.epilogue_start;pc<loc;pc+=4) {
		instr= dc_read_word(dc->handle,pc);
		if (dc_decode_finds_branch(dc,instr)) {
			return (DC_FALSE);
		}
	}
	return (DC_TRUE);

}



dc_init_cr_data(cdp,tep)
dc_cr_data_t *cdp;
tdesc_elem_t *tep;
{
int reg;
dc_word_t rai;
dc_word_t raid;
dc_word_t rsm;
dc_word_t frpos;

	if (tep){

	   /* Start off with all registers undefined and none saved. */
	   for (reg = 0; reg < DC_NUM_REG; reg++) {
		  cdp->reg_val[reg].reg = DC_UNDEF;
	   }
	   cdp->saved = 0;

	   /* Overwrite with what tdesc element says. */

	   cdp->reg_val[tep->info.frame_address_register].reg = DC_REG_SP;
	   cdp->reg_val[tep->info.frame_address_register].off =
				  - tep->info.frame_address_offset;

	   rai = tep->info.return_address_info;
	   raid = tep->info.return_address_info_discriminant;
	   if (raid || rai != DC_REG_RA) {
		  bit_set(cdp->saved,DC_REG_RA);
		  bit_assign(cdp->how,DC_REG_RA,raid);
		  cdp->where[DC_REG_RA] = rai;
	   }

	   rsm = tep->info.register_save_mask;
	   frpos = tep->info.register_save_offset;
	   for (reg = 14; reg <= 30; reg++) {
			if (bit_test(rsm, 30-reg)) {
				bit_set(cdp->saved,reg);
				bit_set(cdp->how,reg);
				cdp->where[reg] = frpos;
				frpos += sizeof(dc_word_t);
			} else {
					cdp->reg_val[reg].reg = reg;
					cdp->reg_val[reg].off = 0;
			}
	   }

	   cdp->reg_val[0].reg = 0;   /* guarantee what hardware does */
	   cdp->reg_val[0].off = 0;

	} else {
	   /* Each register has its own initial value. */
	   for (reg = 0; reg < DC_NUM_REG; reg++) {
		  cdp->reg_val[reg].reg = reg;
		  cdp->reg_val[reg].off = 0;
	   }
   /* No register is yet saved. */
   cdp->saved = 0;
   cdp->how = 0;
	}
}
void dc_correct_cr_data(cdp,handle)
dc_cr_data_t *cdp;
dc_handle_t handle;
{
long sr,r;
dc_word_t save_regs = 0;  /* registers used to save others */
   for (r = 1; r < DC_REG_SP; r++) {
      if (bit_test(cdp->saved,r) && !bit_test(cdp->how,r)) {
         sr = cdp->where[r];
         if (bit_test(save_regs,sr)) {
            dc_error(handle, "Same register used to save two others.");
	 }
         bit_set(save_regs,sr);
      }
   }
   for (r = 1; r < DC_REG_FP; r++) {
      if ((r < 14 || bit_test(cdp->saved,r)) && !bit_test(save_regs,r)) {
         cdp->reg_val[r].reg = DC_UNDEF;
      }
   }
   if (bit_test(cdp->saved,DC_REG_FP) &&
       cdp->reg_val[DC_REG_FP].reg == DC_REG_SP) {  /* is r30 the far? */
      cdp->reg_val[DC_REG_SP].reg = DC_UNDEF;  /* trash sp */
   } else if (cdp->reg_val[DC_REG_SP].reg == DC_REG_SP) { /* is r31 the far? */
      if (bit_test(cdp->saved,DC_REG_FP) && !bit_test(save_regs,DC_REG_FP)) {
         cdp->reg_val[DC_REG_FP].reg = DC_UNDEF;  /* trash r30 */
      }
   }
}
