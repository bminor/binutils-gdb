/* This file has been modified by Data General Corporation, November 1989. */

#ifndef _tdesc_h
#define _tdesc_h
#ifdef __STDC__ 
#define _ARGS(x)	x
#else
#define _ARGS(x)	()
#endif

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


#define DC_MII_PRECEDING_TDESC_END   (1<<0)
#define DC_MII_FOLLOWING_TDESC_START (1<<1)

#define DC_MIO_ENTRY_POINT            (1<< 0)
#define DC_MIO_PROLOGUE_END           (1<< 1)
#define DC_MIO_EPILOGUE_START         (1<< 2)
#define DC_MIO_IMPLICIT_PROLOGUE_END  (1<<16)
#define DC_MIO_LITERAL_ENTRY_POINT    (1<<17)
#define DC_MIO_LITERAL_EPILOGUE_START (1<<18)


/*
   A word is 32 bits of information.  In memory, a word is word-aligned.

   A common and important use of word_t is to represent values in the
   target process, including (byte) addresses in the target process.
   In this case, C arithmetic can be used to simulate machine address
   arithmetic on the target.  (Unsigned arithmetic is actually modulus
   arithmetic.)
*/
typedef unsigned int dc_word_t;


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


typedef struct dc_debug_info {
   unsigned int            protocol;            /* 1 for this structure */
   dc_word_t               tdesc_ptr;
   unsigned int            text_words_count;
   dc_word_t               text_words_ptr;
   unsigned int            data_words_count;
   dc_word_t               data_words_ptr;
} dc_debug_info_t;


typedef struct dc_mstate {
   dc_word_t reg[32];                      /* general registers */
   dc_word_t xip;
   dc_word_t nip;
   dc_word_t fip;
   dc_word_t fpsr;
   dc_word_t fpcr;
   dc_word_t psr;
} dc_mstate_t;



#if 0

   void error_fcn (env, continuable, message)
      dc_word_t env;                       /* environment (arbitrary datum) */
      boolean_t continuable;            /* whether error function may return */
      char *message;                    /* string (no trailing newline) */

   /* In the future, we probably want the error_fcn to be: */
   void error_fcn (env, continuable, code, ...)
      dc_word_t env;                       /* environment (arbitrary datum) */
      boolean_t continuable;            /* whether error function may return */
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


typedef void *dc_handle_t;

typedef void (*tdesc_error_fcn_type) _ARGS((
      dc_word_t env,                   /* environment (arbitrary datum) */
      dc_boolean_t continuable,        /* whether error function may return */
      const char *message              /* string (no trailing newline) */
));
typedef void (*tdesc_io_fcn_type) _ARGS((
      dc_word_t env,                       /* environment (arbitrary datum) */
      dc_word_t memory,                    /* start address in image */
      int length,                       /* in bytes */
      void *buffer                     /* start address of buffer */
));
typedef void (*tdesc_exec_fcn_type) _ARGS((
      dc_word_t env,                       /* environment (arbitrary datum) */
      dc_mstate_t *mstate              /* machine state (read-write) */
));
typedef void (*tdesc_map_fcn_type) _ARGS((
      dc_word_t map_env,
      dc_word_t loc,
      dc_map_info_in_t map_info_in,
      dc_map_info_out_t *map_info_out
));


extern dc_handle_t dc_initiate _ARGS((
   dc_word_t debug_info_ptr,
   tdesc_error_fcn_type error_fcn,
   dc_word_t error_env,
   tdesc_io_fcn_type read_fcn,
   dc_word_t read_env,
   tdesc_io_fcn_type write_fcn, /* NULL => absent */
   dc_word_t write_env,
   tdesc_exec_fcn_type exec_fcn, /* NULL => absent */
   dc_word_t exec_env,
   tdesc_map_fcn_type map_fcn, /* NULL => absent */
   dc_word_t map_env
));
extern void dc_terminate _ARGS((
   dc_handle_t handle
));

typedef int dc_register_state_t;    /* range 0 to 2 */

#define DC_INVALID  0
#define DC_READABLE 1
#define DC_WRITABLE 2

#define DC_NUM_REG 32

#define DC_AUX_LOC  0
#define DC_AUX_SXIP 1
#define DC_AUX_SNIP 2
#define DC_AUX_SFIP 3
#define DC_AUX_FPSR 4
#define DC_AUX_FPCR 5
#define DC_NUM_AUX  6


typedef void *dc_dcontext_t;

extern dc_dcontext_t dc_make_dcontext _ARGS((
   dc_handle_t handle,
   dc_word_t reg_info[DC_NUM_REG],
   dc_word_t reg_flags[2],
   dc_word_t aux_info[DC_NUM_AUX],
   dc_word_t aux_flags[2],
   dc_exactness_t loc_exact,
   dc_word_t psr_info,
   dc_boolean_t psr_ind,
   dc_word_t psr_flags[2]
));
extern void dc_free_dcontext _ARGS((
   dc_dcontext_t dcontext
));
extern dc_register_state_t dc_location_state _ARGS((
   dc_dcontext_t dcontext
));
extern dc_exactness_t dc_location_exactness _ARGS((
   dc_dcontext_t dcontext
));
extern dc_word_t dc_location _ARGS((
   dc_dcontext_t dcontext
));
extern void dc_set_location _ARGS((
   dc_dcontext_t dcontext,
   dc_word_t value
));
extern dc_register_state_t dc_general_register_state _ARGS((
   dc_dcontext_t dcontext,
   int reg
));
extern dc_word_t dc_general_register _ARGS((
   dc_dcontext_t dcontext,
   int reg
));
extern void dc_set_general_register _ARGS((
   dc_dcontext_t dcontext,
   int reg,
   dc_word_t value
));
extern dc_register_state_t dc_auxiliary_register_state _ARGS((
   dc_dcontext_t dcontext,
   int reg
));
extern dc_word_t dc_auxiliary_register _ARGS((
   dc_dcontext_t dcontext,
   int reg
));
extern void dc_set_auxiliary_register _ARGS((
   dc_dcontext_t dcontext,
   int reg,
   dc_word_t value
));
extern dc_register_state_t dc_psr_register_bit_state _ARGS((
   dc_dcontext_t dcontext,
   int bit
));
extern dc_word_t dc_psr_register _ARGS((
   dc_dcontext_t dcontext
));
extern void dc_set_psr_register _ARGS((
   dc_dcontext_t dcontext,
   dc_word_t mask,
   dc_word_t value
));
extern dc_word_t dc_frame_address _ARGS((
   dc_dcontext_t dcontext
));
extern dc_kind_t dc_context_kind _ARGS((
   dc_dcontext_t dcontext
));
extern dc_register_state_t dc_return_address_state _ARGS((
   dc_dcontext_t dcontext
));
extern dc_exactness_t dc_return_address_exactness _ARGS((
   dc_dcontext_t dcontext
));
extern dc_word_t dc_return_address _ARGS((
   dc_dcontext_t dcontext
));
extern void dc_set_return_address _ARGS((
   dc_dcontext_t dcontext,
   dc_word_t value
));
extern void dc_get_exception_info();
extern void dc_get_protection_info();
extern void dc_get_special_info();
extern dc_dcontext_t dc_previous_dcontext _ARGS((
   dc_dcontext_t dcontext
));
extern dc_boolean_t dc_location_in_text_chunk _ARGS((
   dc_dcontext_t dcontext,
   dc_word_t value
));

#endif
