/* GX target-independent functions for block translation.
  Copyright (C) 1998 Cygnus Solutions.  */


#include "sim-main.h"
#include "sim-assert.h"
#include "sim-gx.h"

#include "config.h"
/* #include "cconfig.h" */

/* shared object functions */
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#else
#error "need dlfcn.h"
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
#error "need errno.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "bfd.h"




/* Load the object file with given gx block.  Return pointer to GX
   function or NULL on failure. */

sim_gx_function
sim_gx_compiled_block_f(sim_gx_compiled_block* gx)
{
  sim_gx_function f = gx->function_dlhandle;
  SIM_DESC sd = current_state;
  int rc;

  if(f == NULL)
    {
      /* compile object */
      if(gx->object_name == NULL && gx->source_name != NULL)
	{
	  char compile_command[2000];

	  gx->object_name = strdup(gx->source_name);
	  /* turn *.c into *.o */
	  gx->object_name[strlen(gx->object_name)]='o';

	  /* compute command string to compile object */
	  sprintf(compile_command,
		  "make -f %s OBJ=%s SRC=%s gx",
#define GX_MAKEFILE "--no-makefile-yet--"
		  GX_MAKEFILE,
		  gx->object_name,
		  gx->source_name);

	  rc = system(compile_command);
	  if(rc != 0)
	    {
	      sim_io_error(sd, "Compile error rc %d for GX source %s: %s", 
			rc,
			gx->source_name,
			strerror(errno));
	    }
	}
      
      /* load object */
      if(gx->object_dlhandle == NULL && gx->object_name != NULL)
	{
	  gx->object_dlhandle = dlopen(gx->object_name, RTLD_NOW);
	  if(gx->object_dlhandle == NULL)
	    {
	      sim_io_error(sd, "Load error for GX object %s: %s", 
			gx->object_name,
			dlerror());
	    }
	}
      
      /* locate function */
      if(gx->function_dlhandle == NULL && gx->object_dlhandle != NULL && gx->symbol_name != NULL)
	{
	  f = gx->function_dlhandle = dlsym(gx->object_dlhandle, gx->symbol_name);
	  if(f == NULL)
	    {
	      sim_io_error(sd, "Resolve error for GX object %s symbol %s: %s", 
			gx->object_name,
			gx->symbol_name,
			dlerror());
	    }
	}
    } /* f == NULL */

  return f;
}



/* Forget about given GX block.  Remove its source/object; unload it
   from memory. */
void
sim_gx_compiled_block_dispose(sim_gx_compiled_block* gx)
{
  SIM_DESC sd = current_state;
  int rc;

  /* forget dl information */
  gx->function_dlhandle = NULL;

  /* unload shared library */
  if(gx->object_dlhandle != NULL)
    {
      rc = dlclose(gx->object_dlhandle);
      if(rc != 0)
	{
	  sim_io_error(sd, "dlclose() error for GX object %s: %s", 
		    gx->object_name,
		    dlerror());
	}
      gx->object_dlhandle = NULL;
    }

  /* final gasps */
  zfree(gx->source_name);
  zfree(gx->object_name);
  zfree(gx->symbol_name);
  zfree(gx);
}



/* Translate a piece of the code segment around given PC, in given mode. */
sim_gx_block*
sim_gx_block_create(sim_cia cia)
{
  sim_gx_block* block;

  /* allocate emtpy block */
  block = zalloc(sizeof(sim_gx_block));

  /* initialize block bounds, callback struct etc. */ 
  tgx_block_ctor(block, cia);

  /* create learning mode translation */ 
  sim_gx_block_translate(block, 0 /* learning mode */);

  /* add block to block list */
  sim_gx_block_add(block);

  return block;
}



/* Write the current block list to the state file */
void
sim_gx_write_block_list()
{
  int i;
  SIM_DESC sd = current_state;
  sim_gx_block_list* blocks = STATE_BLOCKS(sd);
  FILE* f;
  char state_file_name[PATH_MAX];
  char *exec_name;

  /* get base of executable name */
  exec_name = bfd_get_filename(STATE_PROG_BFD(sd));
  if(strrchr(exec_name, '/') != NULL)
    exec_name = strrchr(exec_name, '/') + 1;

  /* generate base name */
  sprintf(state_file_name, "%s/%s.gx",
	  GX_DIR,
	  exec_name);

  f = fopen(state_file_name, "w");
  if(f == NULL)
    {
      sim_io_error(sd, "Error: cannot write to state file %s, errno %d", 
		   state_file_name, errno);
    }

  fprintf(f, "# This file was automatically generated.  Do not edit.\n");

  /* write block descriptors into state file */
  for(i=0; i<blocks->gx_blocks_used; i++)
    {
      sim_gx_block* gx = blocks->gx_blocks[i];
      sim_gx_compiled_block* block;
      int j;
      int age;

      age = time(NULL) - gx->learn_last_change; /* store interval */
      fprintf(f, "BLOCK 0x%lx 0x%lx %u %u\n", gx->origin, gx->length, gx->divisor, age);
      fprintf(f, "FLAGS ");
      for(j=0; j<GX_PC_FLAGS_INDEX(gx, gx->origin + gx->length); j++)
	{
	  fprintf(f, "%2x ", gx->pc_flags[j]);
	}
      fprintf(f, "\n");

      /* write learning mode names */
      block = gx->learning_block;
      fprintf(f, "LEARNING %s %s %s %lu %u\n",
	      block->source_name, block->object_name, block->symbol_name,
	      gx->compile_time, gx->opt_compile_count);

      /* write optimized mode names */
      block = gx->optimized_block;
      if(block)
	fprintf(f, "OPTIMIZED %s %s %s\n",
		block->source_name, block->object_name, block->symbol_name);

      /* NB: other fields will be filled in with freshly guessed values */
    }

  (void) fclose(f);
}



void
print_gx_blocks(sim_gx_block_list* blocks, char* where)
{
  printf("print_gx_blocks: %s\n", where);

  if(blocks == NULL)
    printf("(null)\n");
  else
    {
      int i;
      printf("size: %d, used: %d\n",
	     blocks->gx_blocks_size, blocks->gx_blocks_used);

      /* linear search */
      for(i=0; i<blocks->gx_blocks_used; i++)
	{
	  sim_gx_block* gx = blocks->gx_blocks[i];
	  printf("block %d: %p\n", i, (void*) gx);
	  if(gx == NULL)
	    printf("** NULL!\n");
	  else
	    printf("  begin 0x%08x length 0x%08x [opt %d%s]\n",
		   (unsigned)gx->origin, (unsigned)gx->length,
		   gx->opt_compile_count,
		   (gx->optimized_block ? " loaded" : " discarded"));
	}
      
    }
}



/* Read the current block list from the cache */
void
sim_gx_read_block_list()
{
  SIM_DESC sd = current_state;
  FILE* f;
  char state_file_name[PATH_MAX];
  char *exec_name;

  /* check for block */
  if(STATE_PROG_BFD(sd) == NULL)
    return;

  /* get base of executable name */
  exec_name = bfd_get_filename(STATE_PROG_BFD(sd));
  if(strrchr(exec_name, '/') != NULL)
    exec_name = strrchr(exec_name, '/') + 1;

  /* generate base name */
  sprintf(state_file_name, "%s/%s.gx",
	  GX_DIR,
	  exec_name);

  f = fopen(state_file_name, "r");
  if(f == NULL)
    {
      /* XXX: print warning */
      return;
    }

  fscanf(f, "#%*[^\n]\n"); /* swallow # comment line */

  while(1)
    {
      unsigned_4 origin, length;
      unsigned divisor;
      sim_gx_block* gx;
      int rc;
      sim_gx_compiled_block* block;
      unsigned age;
      int j;

      rc = fscanf(f, "BLOCK 0x%0lx 0x%lx %u %u\n", & origin, & length, & divisor, & age);
      if(rc != 4) /* not all fields matched - assume EOF */
	break;

      gx = zalloc(sizeof(sim_gx_block));

      /* initialize block bounds, callback struct etc. */ 
      tgx_block_ctor2(gx, origin, length, divisor);

      /* read flags */
      fscanf(f, "FLAGS");
      for(j=0; j<GX_PC_FLAGS_INDEX(gx, gx->origin + gx->length); j++)
	{
	  unsigned value;
	  fscanf(f, "%2x ", & value);
	  gx->pc_flags[j] = (unsigned_1) value;
	}
      fscanf(f, "\n");

      /* read learning mode info */
      block = zalloc(sizeof(sim_gx_compiled_block));
      gx->learning_block = block;
      block->source_name = zalloc(PATH_MAX);
      block->object_name = zalloc(PATH_MAX);
      block->symbol_name = zalloc(PATH_MAX);
      fscanf(f, "LEARNING %s %s %s %lu %u\n",
	     block->source_name, block->object_name, block->symbol_name,
	     & gx->compile_time, & gx->opt_compile_count);

      /* read optimized mode info */
      block = zalloc(sizeof(sim_gx_compiled_block));
      gx->optimized_block = block;
      block->source_name = zalloc(PATH_MAX);
      block->object_name = zalloc(PATH_MAX);
      block->symbol_name = zalloc(PATH_MAX);
      rc = fscanf(f, "OPTIMIZED %s %s %s\n",
		  block->source_name, block->object_name, block->symbol_name);
      if(rc != 3)
	{
	  /* oops, not an optimized block */
	  zfree(block->source_name);
	  zfree(block->object_name);
	  zfree(block->symbol_name);
	  zfree(block);
	  gx->optimized_block = NULL;
	}

      /* fill in remaining fields */
      gx->learn_last_change = time(NULL) - age; /* make absolute */

      /* store it away */
      sim_gx_block_add(gx);
    }

  print_gx_blocks(STATE_BLOCKS(sd), "after restoring state");
}






/* Add a gx block to list */
void
sim_gx_block_add(sim_gx_block* block)
{
  SIM_DESC sd = current_state;
  sim_gx_block_list* blocks = STATE_BLOCKS(sd);
  int i;

  /* print_gx_blocks(blocks, "pre add"); */

  if(blocks == NULL)
    blocks = STATE_BLOCKS(sd) = zalloc(sizeof(sim_gx_block_list));

  /* need to enlarge block vector? */
  if(blocks->gx_blocks_used == blocks->gx_blocks_size)
    {
      sim_gx_block** new_blocks;
      int j;

      blocks->gx_blocks_size += 20;
      new_blocks = zalloc(blocks->gx_blocks_size * sizeof(sim_gx_block*));
      for(j=0; j<blocks->gx_blocks_used; j++)
	new_blocks[j] = blocks->gx_blocks[j];
      if(blocks->gx_blocks) zfree(blocks->gx_blocks);
      blocks->gx_blocks = new_blocks;
    }

  /* insert new block */
  for(i=0; i<blocks->gx_blocks_used; i++)
    {
      ASSERT(blocks->gx_blocks[i] != NULL);

      /* insertion point reached? */
      if(blocks->gx_blocks[i]->origin > block->origin)
	{
	  int j;
	  for(j=blocks->gx_blocks_used; j>=i; j--)
	    blocks->gx_blocks[j] = blocks->gx_blocks[j-1];
	  blocks->gx_blocks[i] = block;
	  blocks->gx_blocks_used ++;
	  break;
	}
    }

  /* end of block vector */
  if(i == blocks->gx_blocks_used)
    {
      blocks->gx_blocks[blocks->gx_blocks_used ++] = block;
    }

  /* print_gx_blocks(blocks, "post add"); */
}



/* Remove a gx block from list */
void
sim_gx_block_remove(sim_gx_block* block)
{
  SIM_DESC sd = current_state;
  sim_gx_block_list* blocks = STATE_BLOCKS(sd);
  int i;

  /* print_gx_blocks(blocks, "pre remove"); */

  /* linear search */
  for(i=0; i<blocks->gx_blocks_used; i++)
    {
      if(blocks->gx_blocks[i] == block)
	{
	  /* found it */
	  while(i < blocks->gx_blocks_used - 1)
	    {
	      blocks->gx_blocks[i] = blocks->gx_blocks[i+1];
	      i++;
	    }
	  blocks->gx_blocks_used --;
	  break;
	}
    }

  /* print_gx_blocks(blocks, "post remove"); */
}


/* Find a gx block from list */
sim_gx_block*
sim_gx_block_find(sim_cia cia)
{
  SIM_DESC sd = current_state;
  sim_gx_block_list* blocks = STATE_BLOCKS(sd);
  int i;

  if(blocks == NULL) return NULL;

  /* print_gx_blocks(blocks, "pre find"); */

  /* linear search */
  for(i=0; i<blocks->gx_blocks_used; i++)
    {
      sim_gx_block* gx = blocks->gx_blocks[i];
      ASSERT(gx != NULL);

      if(GX_PC_INCLUDES(gx,cia))
	{
	  return gx;
	}
    }

  return NULL;
}



/* generate */
void
sim_gx_block_translate(sim_gx_block* gx, int optimized)
{
  char pwd_name[PATH_MAX];
  char dir_name[PATH_MAX];
  char base_name[PATH_MAX];
  char compile_command[PATH_MAX*4];
  char* exec_name;
  SIM_DESC sd = current_state;
  int rc;
  sim_cia gx_cia;
  sim_gx_compiled_block* block = zalloc(sizeof(sim_gx_compiled_block));
  unsigned time_begin, time_end;

  time_begin = time(NULL);

  if(optimized) gx->optimized_block = block;
  else gx->learning_block = block;

  /* get base of executable name */
  exec_name = bfd_get_filename(STATE_PROG_BFD(sd));
  if(strrchr(exec_name, '/') != NULL)
    exec_name = strrchr(exec_name, '/') + 1;

  /* generate base name */
  sprintf(dir_name, "%s/%s",
	  GX_DIR,
	  exec_name);

  /* generate base name */
  getcwd(pwd_name, sizeof(pwd_name));
  
  /* create work directory */
  rc = mkdir(GX_DIR, 0777);
  if(rc != 0 &&
     errno != EEXIST)
    {
      sim_io_error(sd, "Error: cannot create directory %s, errno %d", 
		   GX_DIR, errno);
    }

  rc = mkdir(dir_name, 0777);
  if(rc != 0 &&
     errno != EEXIST)
    {
      sim_io_error(sd, "Error: cannot create directory %s, errno %d",
		   dir_name, errno);
    }

  /* compute base name */
  if(optimized)
    sprintf(base_name, "%08lx_opt%d", gx->origin, gx->opt_compile_count);
  else
    sprintf(base_name, "%08lx", gx->origin);

  /* generate source/object file names */ 
  block->source_name = zalloc(PATH_MAX);
  block->object_name = zalloc(PATH_MAX);
  sprintf(block->source_name, "%s/%s.c", dir_name, base_name);

  /* generate symbol name for gx function */
  block->symbol_name = zalloc(PATH_MAX);
  sprintf(block->symbol_name, "gx_%s", base_name);

  /* open source file */
  block->source_file = fopen(block->source_name, "w");
  if(block->source_file == NULL)
    {
      sim_io_error(sd, "Error: cannot open file %s, errno %d",
		   block->source_name, errno);
    }

  /* front matter */
  fprintf(block->source_file, "/* sim-gx version %d */\n", GX_VERSION);
  fprintf(block->source_file, "/* gx block date stamp %lu */\n\n", time(NULL));

  /* emit head end of source */
  tgx_emit_pre_function(gx, optimized); 

  /* emit function header */
  fprintf(block->source_file, "\n\n");
  fprintf(block->source_file, "extern int\n");
  fprintf(block->source_file, "%s", block->symbol_name);
  fprintf(block->source_file, "(struct tgx_cpu_regs* regs, char* pc_flags, struct tgx_callbacks* callbacks)\n");
  fprintf(block->source_file, "{\n");
  fprintf(block->source_file, "  int rc = 0;\n");
  if(! optimized)
    fprintf(block->source_file, "  unsigned int insn_count = 0;\n");

  /* pre-block gunk: register load */
  tgx_emit_load_block(gx, optimized);

  /* emit intra-block jump label */
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "shortjump:\n");
  fprintf(block->source_file, "  pc = npc;\n");

  /* translate jumptarget table */
  if(! optimized)
    {
      fprintf(block->source_file, "  pc_flags[(pc - 0x%08x) / %u] |= %d;\n",
	      (unsigned)gx->origin, gx->divisor, GX_PCF_JUMPTARGET);
    }

  /* enforce learning mode run limit */
  if(! optimized)
    {
      fprintf(block->source_file, "  insn_count++;\n");
      fprintf(block->source_file, "  if (insn_count > %d)\n", GX_LEARN_RUN_LIMIT);
      fprintf(block->source_file, "    {\n");
      fprintf(block->source_file, "      rc = %d;\n", GX_F_YIELD);
      fprintf(block->source_file, "      npc = pc;\n");
      fprintf(block->source_file, "      goto save;\n");
      fprintf(block->source_file, "    }\n");
    }

  /* emit PC switch, use compressed case numbers */
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "  switch((pc - 0x%08x) / %u)\n",
	  (unsigned)gx->origin, gx->divisor); 
  fprintf(block->source_file, "    {\n"); 

  /* handle bad-PC event */
  fprintf(block->source_file, "    /* handle unknown jump target */\n");
  fprintf(block->source_file, "    default:\n");
  fprintf(block->source_file, "      rc = %d;\n", GX_F_NONPC);
  fprintf(block->source_file, "      npc = pc;\n");
  fprintf(block->source_file, "      goto save;\n");
  
  /* start translating at the origin */
  gx_cia = gx->origin;

  /* translate instructions in block */ 
  while(GX_PC_INCLUDES(gx,gx_cia))
    { 
      sim_cia next_gx_cia;

      /* translate PC case statement */
      fprintf(block->source_file, "\n");
      fprintf(block->source_file, "    /* PC: 0x%08x, flags %02x */\n",
	      gx_cia, (int) GX_PC_FLAGS(gx, gx_cia));


      /* skip over this instruction if it is not executed */
      if(optimized && !(GX_PC_FLAGS(gx, gx_cia) & GX_PCF_INSTRUCTION))
	{
	  fprintf(block->source_file, "    /* (not reached) */\n");

	  /* prevent fall-through from previous translated insn */
	  if(gx_cia > gx->origin &&
	     GX_PC_FLAGS(gx, (gx_cia - gx->divisor)) & GX_PCF_INSTRUCTION)
	    {
	      fprintf(block->source_file, "    /* prevent fall-through */\n");
	      fprintf(block->source_file, "    npc = 0x%08x;\n", gx_cia);  
	      fprintf(block->source_file, "    rc = %d;\n", GX_F_NONPC);  
	      fprintf(block->source_file, "    goto save;\n");
	    }

	  next_gx_cia = gx_cia + gx->divisor;
	  goto skip_instruction;
	}

      /* translate PC case statement */
      if((! optimized) ||
	 (GX_PC_FLAGS(gx, gx_cia) & GX_PCF_JUMPTARGET))
	{
	  fprintf(block->source_file, "    case %ld:\n",
		  ((gx_cia - gx->origin) / gx->divisor));
	}

      /* translate breakpoint check & exit */
      if(GX_PC_FLAGS(gx, gx_cia) & GX_PCF_COND_HALT)
	{
	  fprintf(block->source_file, "      if(pc_flags[%ld] & %d)\n",
		  GX_PC_FLAGS_INDEX(gx, gx_cia),
		  GX_PCF_HALT);
	  fprintf(block->source_file, "        {\n");
	  fprintf(block->source_file, "          rc = %d;\n", GX_F_HALT);
	  fprintf(block->source_file, "          npc = pc;\n");
	  fprintf(block->source_file, "          goto save;\n");
	  fprintf(block->source_file, "        }\n");
	}

      /* [don't] emit PC-setting */
      /* fprintf(block->source_file, "      pc = 0x%08x;\n", gx_cia); */

      /* mark traversed instructions */
      if(! optimized)
	{
	  fprintf(block->source_file, "      pc_flags[%ld] |= %d;\n",
		  GX_PC_FLAGS_INDEX(gx, gx_cia),
		  GX_PCF_INSTRUCTION);
	}
      
      
      /* translate instruction semantics */
      next_gx_cia = tgx_emit_insn(gx, gx_cia, optimized);

    skip_instruction:

      /* go to next instruction */
      gx_cia = next_gx_cia;
    } 
  fprintf(block->source_file, "    }\n"); 

  /* dropped through last instruction in switch block */
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "  /* dropped through PC switch */\n");
  fprintf(block->source_file, "  npc = 0x%08x;\n", gx_cia);
  fprintf(block->source_file, "  rc = %d;\n", GX_F_RANGE);
  fprintf(block->source_file, "  goto save;\n");

  /* unknown length jump */
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "unknownjump:\n");
  fprintf(block->source_file, "  if(npc >= 0x%08lx && npc < 0x%08lx)\n",
	  gx->origin, gx->origin + gx->length);
  fprintf(block->source_file, "    goto shortjump;\n");

  /* long jump */
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "longjump:\n");
  fprintf(block->source_file, "  rc = %d;\n", GX_F_RANGE);

  /* post-block gunk: SAVE etc. */
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "save:\n");

  tgx_emit_save_block(gx, optimized); 

  /* emit tail end of function */ 
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "  return rc;\n");
  fprintf(block->source_file, "}\n"); 

  /* emit tail end of source */
  tgx_emit_post_function(gx, optimized); 

  /* close source file */ 
  fclose(block->source_file);
  block->source_file = NULL;

  /* compile source & produce shared object */

  sprintf(compile_command, 
	  "gxtool --silent --mode=compile gcc -c -g %s %s",
	  (optimized ? "-O3" : "-O"), block->source_name);

  rc = system(compile_command);
  if(rc != 0)
    {
      sim_io_error(sd, "Error during compiling: `%s' rc %d",
		   compile_command, rc);
    }

  /* link source */

  sprintf(compile_command,
	  "gxtool --silent --mode=link gcc -export-dynamic -rpath %s -g -o lib%s.la %s.lo",
	  dir_name, base_name, base_name);

  rc = system(compile_command);
  if(rc != 0)
    {
      sim_io_error(sd, "Error during linking: `%s' rc %d",
		   compile_command, rc);
    }


  /* install */

  sprintf(compile_command,
	  "gxtool --silent --mode=install cp lib%s.la %s/%s >/dev/null 2>/dev/null",
	  base_name, pwd_name, dir_name);

  rc = system(compile_command);
  if(rc != 0)
    {
      sim_io_error(sd, "Error during install: `%s' rc %d",
		   compile_command, rc);
    }


  /* finish */

  sprintf(compile_command,
	  "gxtool --silent --mode=finish %s >/dev/null 2>/dev/null",
	  dir_name);

  rc = system(compile_command);
  if(rc != 0)
    {
      sim_io_error(sd, "Error during finish: `%s' rc %d",
		   compile_command, rc);
    }

  /* clean up */

  sprintf(compile_command, "rm -f lib%s.la %s.lo", base_name, base_name);
  rc = system(compile_command);
  if(rc != 0)
    {
      sim_io_error(sd, "Error during cleanup: `%s' rc %d",
		   compile_command, rc);
    }

  /* XXX: FILL IN  block->object_name from .la file */
  sprintf(block->object_name, "%s/%s/lib%s.so.0",
	  pwd_name, dir_name, base_name);

  /* measure compile time */
  time_end = time(NULL);

  if(time_end == time_begin) time_end ++; /* clamp minimum duration to 1 */
  gx->compile_time += time_end - time_begin;
  /* fprintf(stderr, "*** compile time: %d\n", gx->compile_time); */
} 

