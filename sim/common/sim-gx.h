/* GX generic simulator structs.
   Copyright (C) 1998 Cygnus Solutions.
*/

#ifndef SIM_GX_H
#define SIM_GX_H

#include <stdio.h>
#include "sim-base.h"
#include "sim-core.h"


/*
#ifndef SIM_GX
#error "Compile this file only if configured SIM_GX"
#endif
*/


/* configuration */

#define GX_DIR ".gx"
#define GX_VERSION 1


struct sim_gx_compiled_block;


/* record for a particular GX block */
typedef struct sim_gx_block
{
  /* ---- BLOCK EXTENT ---- */
  address_word origin; /* first code-segment address translated */
  unsigned_4 length;   /* length of translated code-segment */

#define GX_PC_INCLUDES(gx,pc) ((gx)->origin <= (pc) && (pc) < (gx)->origin + (gx)->length)

  short divisor;       /* minimum instruction word size; address_word -> index divisor */
  char* pc_flags;      /* see GX_PC_* below */

#define GX_PCF_INSTRUCTION  0x01 /* learned */
#define GX_PCF_JUMPTARGET   0x02 /* learned */
#define GX_PCF_COND_HALT    0x10 /* translate-time input */
#define GX_PCF_HALT         0x20 /* run-time input */
#define GX_PC_FLAGS_INDEX(gx,pc) ((((pc) - ((gx)->origin)) / (gx)->divisor))
#define GX_PC_FLAGS(gx,pc)       ((gx)->pc_flags[GX_PC_FLAGS_INDEX((gx),(pc))])

  /* GX callbacks */
  struct tgx_callbacks* callbacks;

  /* compilation statistics */
  unsigned_4 compile_time; /* time to compile [s] */

  /* ---- LEARNING MODE STATE ---- */
  unsigned_4 learn_last_change; /* time of last flag change */
  struct sim_gx_compiled_block* learning_block;

  /* ---- OPTIMIZED MODE STATE ---- */
  unsigned opt_compile_count; /* number of optimized compile attempts */
  struct sim_gx_compiled_block* optimized_block;
} sim_gx_block;




typedef struct sim_gx_compiled_block
{
  /* ---- TRANSLATION OBJECTS ---- */
  char* source_name;   /* source file for translated object */
  char* object_name;   /* file name of translated object */
  char* symbol_name;   /* symbol name of function */
  /* (all above pointers are zalloc()'d buffers, to be zfree()'d. */

  FILE* source_file;   /* working file pointer during translation */
  
  /* ---- LOADED TRANSLATIONS ---- */
  void* object_dlhandle; /* dlopen() handle to loaded object (if open) */
  void* function_dlhandle; /* dlsym() pointer to function (if found) */
} sim_gx_compiled_block;



/* GX block vector: for quick search of translated blocks */
typedef struct sim_gx_block_list
{
  /* ---- BLOCK VECTOR ---- */
  sim_gx_block** gx_blocks; /* vector of GX blocks, sorted by origin field */
  unsigned gx_blocks_size;  /* vector length */
  unsigned gx_blocks_used;  /* number of elements used in vector */
  address_word gx_first, gx_last; /* first & last addresses translated by any gx block */
} sim_gx_block_list;


/* actual gx function pointer type */
struct tgx_info;
typedef int (*sim_gx_function)(struct tgx_info* info);


/* return values from gx function */
#define GX_F_HALT 0
#define GX_F_NONPC 1
#define GX_F_RANGE 2
#define GX_F_YIELD 3


/* Limit on loop cycles within a learning mode gx block */
#define GX_LEARN_RUN_LIMIT 10000

/* Operations */
sim_gx_block* sim_gx_block_create(sim_cia cia);

sim_gx_block* sim_gx_block_find(sim_cia cia);
void sim_gx_block_add(sim_gx_block* block);
void sim_gx_block_remove(sim_gx_block* block);

/* State save/restore */
void sim_gx_write_block_list();
void sim_gx_read_block_list();

sim_gx_function sim_gx_compiled_block_f(sim_gx_compiled_block* gx);
void sim_gx_compiled_block_dispose(sim_gx_compiled_block* gx);
void sim_gx_block_translate(sim_gx_block* gx, int optimized);



/* Target-specific translation operations */

int tgx_optimize_test(sim_gx_block* block);

void tgx_block_ctor(sim_gx_block* block, sim_cia cia);
void tgx_block_ctor2(sim_gx_block* block, unsigned_4 origin,
		     unsigned_4 length, unsigned_4 divisor);
void tgx_block_dtor(sim_gx_block* block);

void tgx_emit_pre_function(sim_gx_block* block, int optimized);
void tgx_emit_load_block(sim_gx_block* block, int optimized);
sim_cia tgx_emit_insn(sim_gx_block* block, sim_cia cia, int optimized);
void tgx_emit_save_block(sim_gx_block* block, int optimized);
void tgx_emit_post_function(sim_gx_block* block, int optimized);


#endif /* SIM_GX_H */
