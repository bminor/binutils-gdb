/* GX generic simulator run.
   Copyright (C) 1998 Cygnus Solutions.
*/

#include "sim-main.h"
#include "sim-assert.h"
#include "sim-gx.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif


/* GX implementation of sim_engine_run that works within the
   sim_engine setjmp/longjmp framework. */


void
sim_engine_run (SIM_DESC sd,
		int next_cpu_nr,
		int nr_cpus, /* ignore */
		int siggnal) /* ignore */
{
  sim_cpu* cpu;
  int cont = 1;
  int rc;

  SIM_ASSERT (STATE_MAGIC (sd) == SIM_MAGIC_NUMBER);
  cpu = STATE_CPU (sd, next_cpu_nr);

  while(cont)
    {
      sim_gx_block* block;
      sim_gx_compiled_block* compiled_block;
      sim_gx_function f;
      sim_cia cia = CIA_GET(cpu);
      int optimized = 0;
      int pre_checksum = 0;
      int post_checksum = 0;

      /* find optimized gx block that includes this PC */
      block = sim_gx_block_find(cia);
      if(block == NULL)
	{
	  /* start new learning block */
	  block = sim_gx_block_create(cia);
          sim_gx_write_block_list();
	}
      ASSERT(block != NULL);

      /* pick preferred compiled block */
      if(block->optimized_block != NULL)
	{
	  compiled_block = block->optimized_block;
	  /* no stats */
	}
      else
	{
	  /* test for optimization policy */
	  if(tgx_optimize_test(block))
	    {
	      block->opt_compile_count ++;
	      sim_gx_block_translate(block, 1 /* optimized */);
              sim_gx_write_block_list();
	      compiled_block = block->optimized_block;
	      optimized = 1;
	    }
	  else
	    {
	      compiled_block = block->learning_block;
	      optimized = 0;
	    }
	}      
      ASSERT(compiled_block != NULL);
      
      /* load & resolve gx function */
      f = sim_gx_compiled_block_f(compiled_block);

      /* XXX: debug 
      printf("calling into gx function %p, pc=%08lx, opt %d\n",
	     (void*) f, (unsigned long) cpu->regs.h_pc, optimized);
      */

      /* compute pc_flags checksum */
      if(! optimized)
	{
	  int i;
	  pre_checksum = 0;
	  for(i=0; i < block->length / block->divisor; i++)
	    pre_checksum += block->pc_flags[i];
	}

      /* call into gx function */
      {
	struct tgx_info info = {& cpu->regs,
				block->pc_flags, 
				block->callbacks };
	rc = (*f)(& info);
      }

      /* compute pc_flags checksum */
      if(! optimized)
	{
	  int i;
	  post_checksum = 0;
	  for(i=0; i < block->length / block->divisor; i++)
	    post_checksum += block->pc_flags[i];

	  if(post_checksum != pre_checksum) /* system changing */
	    {
	      block->learn_last_change = time(NULL);
	    }
	}

      /* XXX: debug
      printf("returned from gx function %p, rc=%d, pc=%08lx\n",
	     (void*) f, rc, (unsigned long) cpu->regs.h_pc);
      */

      switch(rc)
	{
	case GX_F_YIELD: /* gx block voluntarily gave up control */
	case GX_F_RANGE: /* PC travelled outside this block */
	  ; /* continue block dispatch loop */
	  break;
	  
	case GX_F_NONPC: /* non-instruction PC in this block */
	  if(compiled_block == block->optimized_block)
	    {
	      /*	      sim_io_printf(sd, "NOTE: cancelling premature optimization, GX block %p, PC %08lx\n", 
			      block, (long) cpu->regs.h_pc); */
	      sim_gx_compiled_block_dispose(compiled_block);
	      block->learn_last_change = time(NULL);
	      block->optimized_block = NULL;
	    }
	  else
	    {
	      /* learning-mode gx block should not fail this way */
	      sim_io_error(sd, "Internal error - GX block cia %08lx NONPC\n", (long) cia);
	    }
	  break;
	  
	case GX_F_HALT: /* gx function returning control */
	  cont = 0; /* merely exit loop */
	  break;

	  /* should not happen */
	default:
	  sim_io_error(sd, "Translation error (bad rc 0x%d in gx block)", rc);
	  /* NOTREACHED */
	}
      
      if(sim_events_tick(sd))
	sim_events_process(sd);
    }
}
