/* GX M32R implementation.
   Copyright (C) 1998 Cygnus Solutions.
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* common simulator framework */
#include "sim-main.h"
#include "sim-base.h"
#include "sim-core.h"
#include "sim-inline.c"
#include "sim-gx.h"
#include "sim-assert.h"
#include "targ-vals.h"


/* internal functions */
void m32r_emit_long_insn(sim_gx_block* block, PCADDR pc, unsigned insn, int optimized);
void m32r_emit_short_insn(sim_gx_block* block, PCADDR pc, unsigned insn, int optimized);

/* callback functions */
unsigned m32r_gx_load(tgx_info* info, unsigned pc, unsigned addr);
void m32r_gx_store(tgx_info* info, unsigned pc, unsigned addr, unsigned data);
signed char m32r_gx_load1(tgx_info* info, unsigned pc, unsigned addr);
void m32r_gx_store1(tgx_info* info, unsigned pc, unsigned addr, signed char data);
signed short m32r_gx_load2(tgx_info* info, unsigned pc, unsigned addr);
void m32r_gx_store2(tgx_info* info, unsigned pc, unsigned addr, signed short data);
void m32r_gx_syscall(tgx_info* info, tgx_syscall_data* data);



/* external functions */

void
tgx_block_ctor(sim_gx_block* block, sim_cia cia)
{
  /* pick translation boundaries */
  unsigned_4 origin, length, divisor;

#define GX_PAGE_SIZE 4096
  origin = (cia / GX_PAGE_SIZE) * GX_PAGE_SIZE;
  length = GX_PAGE_SIZE;

  /* fill in general target-dependent fields */
  divisor = 4; /* m32r instruction-pairs occur on word boundaries */

  tgx_block_ctor2(block, origin, length, divisor);
}


void
tgx_block_ctor2(sim_gx_block* block, unsigned_4 origin,
		unsigned_4 length, unsigned_4 divisor)
{
  block->origin = origin;
  block->length = length;
  block->divisor = divisor;

  /* allocate pc_flags array */
  block->pc_flags = zalloc(block->length / block->divisor);

  /* allocate tgx_callbacks */
  block->callbacks = zalloc(sizeof(tgx_callbacks));
  block->callbacks->load = m32r_gx_load;
  block->callbacks->store = m32r_gx_store;
  block->callbacks->load1 = m32r_gx_load1;
  block->callbacks->store1 = m32r_gx_store1;
  block->callbacks->load2 = m32r_gx_load2;
  block->callbacks->store2 = m32r_gx_store2;
  block->callbacks->syscall = m32r_gx_syscall;
}



void
tgx_emit_pre_function(sim_gx_block* gx, int optimized)
{
  sim_gx_compiled_block* block = optimized ? gx->optimized_block : gx->learning_block;
  ASSERT(block->source_file != NULL);

  fprintf(block->source_file, "\n");
  fprintf(block->source_file, "#include <stdio.h>\n");
  fprintf(block->source_file, "\n");
  fprintf(block->source_file, /* match with definition in sim-main.h! */
	  "typedef struct tgx_cpu_regs\n"
	  "{\n"
	  "  unsigned int h_pc;  /* program counter */\n"
	  "  signed int h_gr[16];  /* general registers */\n"
	  "  unsigned int h_cr[16];  /* control registers */\n"
	  "  long long h_accum;  /* accumulator */\n"
	  "  unsigned h_lock;  /* lock */\n"
	  "} tgx_cpu_regs;\n");

  fprintf(block->source_file, /* match with definition in sim-main.h! */
	  "typedef struct tgx_syscall_data\n"
	  "{\n"
	  "  unsigned pc;\n"
	  "  unsigned func;\n"
	  "  unsigned arg1;\n"
	  "  unsigned arg2;\n"
	  "  unsigned arg3;\n"
	  "  unsigned errcode;\n"
	  "  unsigned result;\n"
	  "  unsigned result2;\n"
	  "} tgx_syscall_data;\n");

  fprintf(block->source_file, /* match with definition in sim-main.h! */
	  "struct tgx_info;\n"
	  "typedef struct tgx_callbacks\n"
	  "{\n"
	  "  unsigned (*load)(struct tgx_info* info, unsigned pc, unsigned addr);\n"
	  "  void (*store)(struct tgx_info* info, unsigned pc, unsigned addr, unsigned data);\n"
	  "  signed char (*load1)(struct tgx_info* info, unsigned pc, unsigned addr);\n"
	  "  void (*store1)(struct tgx_info* info, unsigned pc, unsigned addr, signed char data);\n"
	  "  signed short (*load2)(struct tgx_info* info, unsigned pc, unsigned addr);\n"
	  "  void (*store2)(struct tgx_info* info, unsigned pc, unsigned addr, signed short data);\n"
	  "  void (*syscall)(struct tgx_info* info, tgx_syscall_data* data);\n"
	  "} tgx_callbacks;\n");

  fprintf(block->source_file, /* match with definition in sim-main.h! */
          "typedef struct tgx_mapping_cache\n"
          "{\n"
          "  unsigned base;\n"
          "  unsigned bound;\n"
          "  void* buffer;\n"
          "} tgx_mapping_cache;\n");

  fprintf(block->source_file, /* match with definition in sim-main.h! */
	  "typedef struct tgx_info\n"
	  "{\n"
	  "  struct tgx_cpu_regs* regs;\n"
	  "  char* pc_flags;\n"
	  "  struct tgx_callbacks* callbacks;\n"
	  "} tgx_info;\n");
}


void
tgx_emit_load_block(sim_gx_block* gx, int optimized)
{
  sim_gx_compiled_block* block =
    optimized ? gx->optimized_block : gx->learning_block;

  ASSERT(block->source_file != NULL);
  fprintf(block->source_file, /* match with definition above */
	  "  tgx_cpu_regs* regs = info->regs;\n"
	  "  tgx_callbacks* callbacks = info->callbacks;\n"
	  "  char* pc_flags = info->pc_flags;\n"
	  "\n"
	  "  unsigned int pc = regs->h_pc;\n"
	  "  unsigned int npc = pc;\n"
	  "  signed int temp;\n"
	  "  signed int gr0 = regs->h_gr[0], gr1 = regs->h_gr[1];\n"
	  "  signed int gr2 = regs->h_gr[2], gr3 = regs->h_gr[3];\n"
	  "  signed int gr4 = regs->h_gr[4], gr5 = regs->h_gr[5];\n"
	  "  signed int gr6 = regs->h_gr[6], gr7 = regs->h_gr[7];\n"
	  "  signed int gr8 = regs->h_gr[8], gr9 = regs->h_gr[9];\n"
	  "  signed int gr10 = regs->h_gr[10], gr11 = regs->h_gr[11];\n"
	  "  signed int gr12 = regs->h_gr[12], gr13 = regs->h_gr[13];\n"
	  "  signed int gr14 = regs->h_gr[14];\n"
	  "  unsigned int cr0 = regs->h_cr[0], cr1 = regs->h_cr[1];\n"
	  "  unsigned int cr2 = regs->h_cr[2], cr3 = regs->h_cr[3];\n"
	  "  unsigned int cr4 = regs->h_cr[4], cr5 = regs->h_cr[5];\n"
	  "  unsigned int cr6 = regs->h_cr[6], cr7 = regs->h_cr[7];\n"
	  "  unsigned int cr8 = regs->h_cr[8], cr9 = regs->h_cr[9];\n"
	  "  unsigned int cr10 = regs->h_cr[10], cr11 = regs->h_cr[11];\n"
	  "  unsigned int cr12 = regs->h_cr[12], cr13 = regs->h_cr[13];\n"
	  "  unsigned int cr14 = regs->h_cr[14], cr15 = regs->h_cr[15];\n"
	  "  long long accum = regs->h_accum;\n"
	  "  unsigned cond = cr0 & 0x01;\n"
	  "  unsigned sm = cr0 & 0x80;\n"
 	  "  unsigned int gr15 = sm ? cr3 : cr2;\n"
          "  unsigned lock = regs->h_lock;\n");
}



sim_cia
tgx_emit_insn(sim_gx_block* gx, sim_cia cia, int optimized)
{
  PCADDR pc = cia;
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);
  USI insn = sim_core_read_unaligned_4 (cpu, NULL_CIA, exec_map, cia);
  USI insn1 = (insn >> 16) & 0xffff;
  USI insn2 = insn & 0xffff;
  sim_gx_compiled_block* block =
    optimized ? gx->optimized_block : gx->learning_block;
  
  ASSERT(block->source_file != NULL);

  /* classify instruction word */
  if (insn & 0x80000000) /* single long word */
    m32r_emit_long_insn(gx, pc, insn, optimized);
  else /* two sequential half words */
    {
      /* translate first instruction */
      m32r_emit_short_insn(gx, pc, insn1, optimized);

      if(insn2 & 0x8000) /* parallel? */
	{
	  fprintf(block->source_file, "      /* || */\n");
	  insn2 &= 0x7fff;
	  /* XXX: genuine parallelism handling */
	}
      else
	fprintf(block->source_file, "      /* -> */\n");
      
      /* translate second instruction */
      m32r_emit_short_insn(gx, pc + 2, insn2, optimized);
    }

  return cia + 4; /* next instruction pair */
}


void
tgx_emit_save_block(sim_gx_block* gx, int optimized)
{
  sim_gx_compiled_block* block =
    optimized ? gx->optimized_block : gx->learning_block;

  ASSERT(block->source_file != NULL);
  fprintf(block->source_file, /* match with definition above */
	  "  cr0 = (cr0 & 0x00c140) | (sm ? 0x80 : 0 ) | (cond ? 1 : 0);\n"
	  "  cr1 = cond ? 1 : 0;\n"
	  "  cr2 = sm ? cr2 : gr15;\n"
	  "  cr3 = sm ? gr15 : cr3;\n"
	  "  regs->h_pc = npc;\n"
	  "  regs->h_gr[0] = gr0, regs->h_gr[1] = gr1;\n"
	  "  regs->h_gr[2] = gr2, regs->h_gr[3] = gr3;\n"
	  "  regs->h_gr[4] = gr4, regs->h_gr[5] = gr5;\n"
	  "  regs->h_gr[6] = gr6, regs->h_gr[7] = gr7;\n"
	  "  regs->h_gr[8] = gr8, regs->h_gr[9] = gr9;\n"
	  "  regs->h_gr[10] = gr10, regs->h_gr[11] = gr11;\n"
	  "  regs->h_gr[12] = gr12, regs->h_gr[13] = gr13;\n"
	  "  regs->h_gr[14] = gr14, regs->h_gr[15] = gr15;\n"
	  "  regs->h_cr[0] = cr0, regs->h_cr[1] = cr1;\n"
	  "  regs->h_cr[2] = cr2, regs->h_cr[3] = cr3;\n"
	  "  regs->h_cr[4] = cr4, regs->h_cr[5] = cr5;\n"
	  "  regs->h_cr[6] = cr6, regs->h_cr[7] = cr7;\n"
	  "  regs->h_cr[8] = cr8, regs->h_cr[9] = cr9;\n"
	  "  regs->h_cr[10] = cr10, regs->h_cr[11] = cr11;\n"
	  "  regs->h_cr[12] = cr12, regs->h_cr[13] = cr13;\n"
	  "  regs->h_cr[14] = cr14, regs->h_cr[15] = cr15;\n"
	  "  regs->h_accum = accum;\n"
	  "  regs->h_lock = lock;\n");
}


void
tgx_emit_post_function(sim_gx_block* gx, int optimized)
{
  sim_gx_compiled_block* block =
    optimized ? gx->optimized_block : gx->learning_block;

  ASSERT(block->source_file != NULL);
  fprintf(block->source_file, "\n/* end of file */\n");
}


void
tgx_block_dtor(sim_gx_block* block)
{
}



int
tgx_optimize_test(sim_gx_block* block)
{
  unsigned_4 current_time = time(NULL);
  unsigned_4 constant_time = current_time - block->learn_last_change;
  int opt;
  char* env;

  /* try another optimize run if the system has settled down */
  opt = (block->compile_time != 0
	 && block->learn_last_change != 0
	 && constant_time > block->compile_time);

  /* allow override by environment variable */
#ifdef HAVE_GETENV
  env = getenv("GX_OPTIMIZE");
  if(env)
    opt = atoi(env);
#endif

  /*
  if(opt)
    printf("optimize_test: now: %d, chg: %d, comp: %d, count: %d => opt %d\n",
	   current_time, block->learn_last_change, block->compile_time,
	   block->opt_compile_count, opt);
 
  */

  return opt;
}


unsigned
m32r_gx_load(struct tgx_info* info, unsigned pc, unsigned addr)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);

  USI data = sim_core_read_unaligned_4 (cpu, pc, read_map, addr);
  return data;
}


void
m32r_gx_store(struct tgx_info* info, unsigned pc, unsigned addr, unsigned data)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);

  sim_core_write_unaligned_4 (cpu, pc, write_map, addr, data);
}


signed char
m32r_gx_load1(struct tgx_info* info, unsigned pc, unsigned addr)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);
  signed char data = 0;

  data = sim_core_read_unaligned_1 (cpu, pc, read_map, addr);
  return data;
}


void
m32r_gx_store1(struct tgx_info* info, unsigned pc, unsigned addr, signed char data)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);

  sim_core_write_unaligned_1 (cpu, pc, write_map, addr, data);
}


signed short
m32r_gx_load2(struct tgx_info* info, unsigned pc, unsigned addr)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);

  signed short data = sim_core_read_unaligned_2 (cpu, pc, read_map, addr);
  return data;
}


void
m32r_gx_store2(struct tgx_info* info, unsigned pc, unsigned addr, signed short data)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);

  sim_core_write_unaligned_2 (cpu, pc, write_map, addr, data);
}


static int
syscall_read_mem (host_callback *cb, struct cb_syscall *sc,
                  unsigned long taddr, char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_read_buffer (sd, cpu, read_map, buf, taddr, bytes);
}


static int
syscall_write_mem (host_callback *cb, struct cb_syscall *sc,
                   unsigned long taddr, const char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_write_buffer (sd, cpu, write_map, buf, taddr, bytes);
}


void
m32r_gx_syscall(struct tgx_info* info, tgx_syscall_data* data)
{
  SIM_DESC sd = CURRENT_STATE;
  sim_cpu* cpu = STATE_CPU (sd, 0);
  host_callback *cb = STATE_CALLBACK (sd);
  CB_SYSCALL s;

  CB_SYSCALL_INIT (&s);
  s.func = data->func;
  s.arg1 = data->arg1;
  s.arg2 = data->arg2;
  s.arg3 = data->arg3;
  
  if (s.func == TARGET_SYS_exit)
    {
      sim_engine_halt (sd, cpu, NULL, data->pc, sim_exited, s.arg1);
      exit(s.arg1);
    }
  
  s.p1 = (PTR) sd;
  s.p2 = (PTR) cpu;
  s.read_mem = syscall_read_mem;
  s.write_mem = syscall_write_mem;
  cb_syscall (cb, &s);

  data->errcode = s.errcode;
  data->result = s.result;
  data->result2 = s.result2;

  /* XXX: clear read/write cache in info */
}



void
m32r_emit_long_insn(sim_gx_block* gx, PCADDR pc, unsigned insn, int optimized)
{
  int op1  = (insn & 0xf0000000) >> 28;
  int r1   = (insn & 0x0f000000) >> 24;
  int op2  = (insn & 0x00f00000) >> 20;
  int r2   = (insn & 0x000f0000) >> 16;
  short lit2 = (insn & 0x0000ffff);
  int lit3 = (insn & 0x00ffffff);
  sim_gx_compiled_block* block = optimized ? gx->optimized_block : gx->learning_block;
  FILE* f  = block->source_file;

  ASSERT(f != NULL);

  /* force PC trace by environment variable */
#ifdef HAVE_GETENV
  if(getenv("GX_TRACE"))
    fprintf(f, "      printf(\"0x%06x\\n\");\n", pc);
#endif

  if(op1 == 0x8 && op2 == 0x4 && r1 == 0)
    {
      fprintf(f, "      /* CMPI R%d,#%d */\n", r2, lit2);
      fprintf(f, "      cond = (gr%d < %d);\n", r2, lit2);
    }
  else if(op1 == 0x8 && op2 == 0x5 && r1 == 0)
    {
      fprintf(f, "      /* CMPUI R%d,#%d */\n", r2, lit2);
      fprintf(f, "      cond = ((unsigned) gr%d < %u);\n", r2, (unsigned)((int) lit2));
    }
  else if(op1 == 0x8 && op2 == 0x8)
    {
      fprintf(f, "      /* ADDV3 R%d,R%d,#%d */\n", r1, r2, lit2);
      fprintf(f, "      temp = gr%d + %d;\n", r2, lit2);
      fprintf(f, "      cond = (~(gr%d & %d) & (gr%d ^ temp)) & (1<<31);\n", r2, lit2, r2);
      fprintf(f, "      gr%d = temp;\n", r1);
    }
  else if(op1 == 0x8 && op2 == 0xa)
    {
      fprintf(f, "      /* ADD3 R%d,R%d,#%d */\n", r1, r2, lit2);
      fprintf(f, "      gr%d = gr%d + %d;\n", r1, r2, lit2);
    }
  else if(op1 == 0x8 && op2 == 0xc)
    {
      fprintf(f, "      /* AND3 R%d,R%d,#%u */\n", r1, r2, (unsigned short) lit2);
      fprintf(f, "      gr%d = gr%d & %u;\n", r1, r2, (unsigned short) lit2);
    }
  else if(op1 == 0x8 && op2 == 0xd)
    {
      fprintf(f, "      /* XOR3 R%d,R%d,#%d */\n", r1, r2, lit2);
      fprintf(f, "      gr%d = gr%d ^ %d;\n", r1, r2, lit2);
    }
  else if(op1 == 0x8 && op2 == 0xe)
    {
      fprintf(f, "      /* OR3 R%d,R%d,#%u */\n", r1, r2, (unsigned short) lit2);
      fprintf(f, "      gr%d = gr%d | %u;\n", r1, r2, (unsigned short) lit2);
    }

  else if(op1 == 0x9 && op2 == 0x0 && lit2 == 0)
    {
      fprintf(f, "      /* DIV R%d,R%d */\n", r1, r2);
      fprintf(f, "      if(gr%d != 0)\n", r2);
      fprintf(f, "        gr%d = gr%d / gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x9 && op2 == 0x1 && lit2 == 0)
    {
      fprintf(f, "      /* DIVU R%d,R%d */\n", r1, r2);
      fprintf(f, "      if(gr%d != 0)\n", r2);
      fprintf(f, "        gr%d = (unsigned) gr%d / (unsigned) gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x9 && op2 == 0x2 && lit2 == 0)
    {
      fprintf(f, "      /* REM R%d,R%d */\n", r1, r2);
      fprintf(f, "      if(gr%d != 0)\n", r2);
      fprintf(f, "        gr%d = gr%d %% gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x9 && op2 == 0x3 && lit2 == 0)
    {
      fprintf(f, "      /* REMU R%d,R%d */\n", r1, r2);
      fprintf(f, "      if(gr%d != 0)\n", r2);
      fprintf(f, "        gr%d = (unsigned) gr%d %% (unsigned) gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x9 && op2 == 0x8)
    {
      fprintf(f, "      /* SRL3 R%d,R%d,#%d */\n", r1, r2, lit2);
      fprintf(f, "      gr%d = (unsigned) gr%d >> %d;\n", r1, r2, (lit2 & 0x1f));
    }
  else if(op1 == 0x9 && op2 == 0xa)
    {
      fprintf(f, "      /* SRA3 R%d,R%d,#%d */\n", r1, r2, lit2);
      fprintf(f, "      gr%d = gr%d >> %d;\n", r1, r2, (lit2 & 0x1f));
    }
  else if(op1 == 0x9 && op2 == 0xc)
    {
      fprintf(f, "      /* SLL3 R%d,R%d,#%d */\n", r1, r2, lit2);
      fprintf(f, "      gr%d = gr%d << %d;\n", r1, r2, (lit2 & 0x1f));
    }
  else if(op1 == 0x9 && op2 == 0xf && r2 == 0)
    {
      fprintf(f, "      /* LDI R%d,#%d */\n", r1, lit2);
      fprintf(f, "      gr%d = %d;\n", r1, lit2);
    }

  else if(op1 == 0xa && op2 == 0x0)
    {
      fprintf(f, "      /* STB R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      (*(callbacks->store1))(info, 0x%08x, gr%d + %d, gr%d & 0xff);\n", (unsigned)pc, r2, lit2, r1);
    }
  else if(op1 == 0xa && op2 == 0x2)
    {
      fprintf(f, "      /* STH R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      (*(callbacks->store2))(info, 0x%08x, gr%d + %d, gr%d & 0xffff);\n", (unsigned)pc, r2, lit2, r1);
    }
  else if(op1 == 0xa && op2 == 0x4)
    {
      fprintf(f, "      /* ST R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      (*(callbacks->store))(info, 0x%08x, gr%d + %d, gr%d);\n", (unsigned)pc, r2, lit2, r1);
    }
  else if(op1 == 0xa && op2 == 0x8)
    {
      fprintf(f, "      /* LDB R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      gr%d = (*(callbacks->load1))(info, 0x%08x, gr%d + %d);\n", r1, (unsigned)pc, r2, lit2);
    }
  else if(op1 == 0xa && op2 == 0x9)
    {
      fprintf(f, "      /* LDUB R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      gr%d = (unsigned char)(*(callbacks->load1))(info, 0x%08x, gr%d + %d);\n", r1, (unsigned)pc, r2, lit2);
    }
  else if(op1 == 0xa && op2 == 0xa)
    {
      fprintf(f, "      /* LDH R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      gr%d = (*(callbacks->load2))(info, 0x%08x, gr%d + %d);\n", r1, (unsigned)pc, r2, lit2);
    }
  else if(op1 == 0xa && op2 == 0xb)
    {
      fprintf(f, "      /* LDUH R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      gr%d = (unsigned short)(*(callbacks->load2))(info, 0x%08x, gr%d + %d);\n", r1, (unsigned)pc, r2, lit2);
    }
  else if(op1 == 0xa && op2 == 0xc)
    {
      fprintf(f, "      /* LD R%d,@(%d,R%d) */\n", r1, lit2, r2);
      fprintf(f, "      gr%d = (*(callbacks->load))(info, 0x%08x, gr%d + %d);\n", r1, (unsigned)pc, r2, lit2);
    }

  else if(op1 == 0xb && op2 == 0x0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BEQ R%d,R%d,%d */\n", r1, r2, lit2);
      fprintf(f, "      if (gr%d == gr%d)\n", r1, r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0x1)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BNE R%d,R%d,%d */\n", r1, r2, lit2);
      fprintf(f, "      if (gr%d != gr%d)\n", r1, r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0x8 && r1 == 0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BEQZ R%d,%d */\n", r2, lit2);
      fprintf(f, "      if (gr%d == 0)\n", r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0x9 && r1 == 0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BNEZ R%d,%d */\n", r2, lit2);
      fprintf(f, "      if (gr%d != 0)\n", r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0xa && r1 == 0x0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BLTZ R%d,%d */\n", r2, lit2);
      fprintf(f, "      if (gr%d < 0)\n", r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0xb && r1 == 0x0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BGEZ R%d,%d */\n", r2, lit2);
      fprintf(f, "      if (gr%d >= 0)\n", r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0xc && r1 == 0x0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BLEZ R%d,%d */\n", r2, lit2);
      fprintf(f, "      if (gr%d <= 0)\n", r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xb && op2 == 0xd && r1 == 0x0)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) lit2) << 2);
      fprintf(f, "      /* BGTZ R%d,%d */\n", r2, lit2);
      fprintf(f, "      if (gr%d > 0)\n", r2);
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }

  else if(op1 == 0xd && op2 == 0xc && r2 == 0)
    {
      fprintf(f, "      /* SETH R%d,#%d */\n", r1, lit2);
      fprintf(f, "      gr%d = 0x%08x;\n", r1, (lit2 << 16));
    }

  else if(op1 == 0xe)
    {
      fprintf(f, "      /* LD24 R%d,#%d */\n", r1, lit3);
      fprintf(f, "      gr%d = 0x%08x;\n", r1, lit3);
    }

  else if(op1 == 0xf && r1 == 0xc)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((lit3 << 8) >> 8) << 2);
      fprintf(f, "      /* BC %d */\n", lit3);
      fprintf(f, "      if (cond)\n");
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xf && r1 == 0xd)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((lit3 << 8) >> 8) << 2);
      fprintf(f, "      /* BNC %d */\n", lit3);
      fprintf(f, "      if (! cond)\n");
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0xf && r1 == 0xe)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((lit3 << 8) >> 8) << 2);
      unsigned retpc = (pc & 0xfffffffc) + 4;
      fprintf(f, "      /* BL %d */\n", lit3);
      fprintf(f, "      gr14 = 0x%08x;\n", retpc);
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "      goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "      npc = 0x%08x;\n", newpc);
	  fprintf(f, "      goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
    }
  else if(op1 == 0xf && r1 == 0xf)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((lit3 << 8) >> 8) << 2);
      fprintf(f, "      /* BRA %d */\n", lit3);
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "      goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "      npc = 0x%08x;\n", newpc);
	  fprintf(f, "      goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
    }

  else
   {
      /* illegal instruction */
      /* XXX */
      fprintf(f, "      printf(\"ILLEGAL INSN 0x%08x @ PC 0x%08x\\n\");\n", insn, (unsigned)pc);
      fprintf(f, "      abort();\n");
    }
}


void
m32r_emit_short_insn(sim_gx_block* gx, PCADDR pc, unsigned insn, int optimized)
{
  int op1  = (insn & 0xf000) >> 12;
  int r1   = (insn & 0x0f00) >> 8;
  int op2  = (insn & 0x00f0) >> 4;
  int op2part  = (insn & 0x00e0) >> 5;
  int r2   = (insn & 0x000f);
  signed char c = (insn & 0x00ff);
  signed char c5 = (insn & 0x001f);
  sim_gx_compiled_block* block = optimized ? gx->optimized_block : gx->learning_block;
  FILE* f  = block->source_file;

  ASSERT(f != NULL);

  /* force PC trace by environment variable */
#ifdef HAVE_GETENV
  if(getenv("GX_TRACE"))
    fprintf(f, "      printf(\"0x%06x\\n\");\n", pc);
#endif

  if(0)
    ; /* place holder */
  else if(op1 == 0x0 && op2 == 0x0)
    {
      fprintf(f, "      /* SUBV R%d,R%d */\n", r1, r2);
      fprintf(f, "      temp = gr%d - gr%d;\n", r1, r2);
      fprintf(f, "      cond = (unsigned) gr%d < (unsigned) gr%d;\n", r1, r2);
      fprintf(f, "      gr%d = temp;\n", r1);
    }
  else if(op1 == 0x0 && op2 == 0x1)
    {
      fprintf(f, "      /* SUBX R%d,R%d */\n", r1, r2);
      fprintf(f, "      temp = gr%d - gr%d - (cond ? 1:0);\n", r1, r2);
      fprintf(f, "      cond = (unsigned) gr%d < (unsigned) gr%d;\n", r1, r2);
      fprintf(f, "      gr%d = temp;\n", r1);
    }
  else if(op1 == 0x0 && op2 == 0x2)
    {
      fprintf(f, "      /* SUB R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d - gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0x3)
    {
      fprintf(f, "      /* NEG R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = 0 - gr%d;\n", r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0x4)
    {
      fprintf(f, "      /* CMP R%d,R%d */\n", r1, r2);
      fprintf(f, "      cond = (gr%d < gr%d);\n", r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0x5)
    {
      fprintf(f, "      /* CMPU R%d,R%d */\n", r1, r2);
      fprintf(f, "      cond = ((unsigned) gr%d < (unsigned) gr%d);\n", r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0x8)
    {
      fprintf(f, "      /* ADDV R%d,R%d */\n", r1, r2);
      fprintf(f, "      temp = gr%d + gr%d;\n", r1, r2);
      fprintf(f, "      cond = ((gr%d & gr%d) | (gr%d & ~temp) | (gr%d & ~temp)) & (1<<31);\n", r1, r2, r1, r2);
      fprintf(f, "      cond = (gr%d ^ gr%d ^ temp ^ cond) & (1<<31);\n", r1, r2);
      fprintf(f, "      gr%d = temp;\n", r1);
    }
  else if(op1 == 0x0 && op2 == 0x9)
    {
      fprintf(f, "      /* ADDX R%d,R%d */\n", r1, r2);
      fprintf(f, "      temp = gr%d + gr%d + (cond ? 1:0);\n", r1, r2);
      fprintf(f, "      cond = ((gr%d & gr%d) | (gr%d & ~temp) | (gr%d & ~temp)) & (1<<31);\n", r1, r2, r1, r2);
      fprintf(f, "      gr%d = temp;\n", r1);
    }
  else if(op1 == 0x0 && op2 == 0xa)
    {
      fprintf(f, "      /* ADD R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d + gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0xb)
    {
      fprintf(f, "      /* NOT R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = ~ gr%d;\n", r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0xc)
    {
      fprintf(f, "      /* AND R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d & gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0xd)
    {
      fprintf(f, "      /* XOR R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d ^ gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x0 && op2 == 0xe)
    {
      fprintf(f, "      /* OR R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d | gr%d;\n", r1, r1, r2);
    }

  else if(op1 == 0x1 && op2 == 0x0)
    {
      fprintf(f, "      /* SRL R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = (unsigned) gr%d >> (gr%d & 0x1f);\n", r1, r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0x2)
    {
      fprintf(f, "      /* SRA R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d >> (gr%d & 0x1f);\n", r1, r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0x4)
    {
      fprintf(f, "      /* SLL R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d << (gr%d & 0x1f);\n", r1, r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0x6)
    {
      fprintf(f, "      /* MUL R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d * gr%d;\n", r1, r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0x8)
    {
      fprintf(f, "      /* MV R%d,R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d;\n", r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0x9)
    {
      fprintf(f, "      /* MVFC R%d,CR%d */\n", r1, r2);
      if(r2 == 0) /* psw */
	fprintf(f, "      gr%d = (cr0 & 0x00c140) | (sm ? 0x80 : 0 ) | (cond ? 1 : 0);\n", r1);
      else if(r2 == 1) /* cbr */
	fprintf(f, "      gr%d = (cond ? 1 : 0);\n", r1);
      else if(r2 == 2) /* spi */
	{
	  fprintf(f, "      if (sm) gr%d = cr2;\n", r1);
	  fprintf(f, "      else gr%d = gr15;\n", r1);
	}
      else if(r2 == 3) /* spu */
	{
	  fprintf(f, "      if (!sm) gr%d = cr3;\n", r1);
	  fprintf(f, "      else gr%d = gr15;\n", r1);
	}
      else
	fprintf(f, "      gr%d = cr%d;\n", r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0xa)
    {
      fprintf(f, "      /* MVTC R%d,CR%d */\n", r1, r2);
      if(r2 == 0) /* psw */
	{
	  fprintf(f, "      cr0 = gr%d & 0x0000c1c1;\n", r1);
	  fprintf(f, "      cond = cr0 & 1;\n");
	  fprintf(f, "      cr2 = sm ? cr2 : gr15;\n");
	  fprintf(f, "      cr3 = sm ? gr15 : cr3;\n");
	  fprintf(f, "      sm = cr0 & 80;\n");
 	  fprintf(f, "      gr15 = sm ? cr3 : cr2;\n");
	}
      else if(r2 == 1) /* cbr */
	  ; /* no effect */
      else if(r2 == 2) /* spi */
	{
	  fprintf(f, "      if (sm) cr2 = gr%d;\n", r1);
	  fprintf(f, "      else gr15 = gr%d;\n", r1);
	}
      else if(r2 == 3) /* spu */
	{
	  fprintf(f, "      if (!sm) cr3 = gr%d;\n", r1);
	  fprintf(f, "      else gr15 = gr%d;\n", r1);
	}
      else if(r2 == 6) /* bpc */
	fprintf(f, "      cr6 = gr%d & 0xfffffffe;\n", r1);
      else
	fprintf(f, "      cr%d = gr%d;\n", r1, r2);
    }
  else if(op1 == 0x1 && op2 == 0xc && r1 == 0xe)
    {
      fprintf(f, "      /* JL R%d */\n", r2);
      fprintf(f, "      gr14 = (0x%08x & 0xfffffffc) + 4;\n", (unsigned)pc);
      fprintf(f, "      npc = gr%d & 0xfffffffc;\n", r2);
      fprintf(f, "      goto unknownjump;\n");
    }
  else if(op1 == 0x1 && op2 == 0xc && r1 == 0xf)
    {
      fprintf(f, "      /* JMP R%d */\n", r2);
      fprintf(f, "      npc = gr%d & 0xfffffffc;\n", r2);
      fprintf(f, "      goto unknownjump;\n");
    }
  else if(op1 == 0x1 && op2 == 0xd && r1 == 0x0 && r2 == 0x6)
    {
      fprintf(f, "      /* RTE */\n");
      fprintf(f, "      cr0 = (cr0 & 0xff00) | (cr0 >> 8);\n");
      fprintf(f, "      cond = cr0 & 0x01;\n");
      fprintf(f, "      sm = cr0 & 0x80;\n");
      fprintf(f, "      gr15 = sm ? cr3 : cr2;\n");
      fprintf(f, "      npc = cr6 & 0xfffffffc;\n");
      fprintf(f, "      goto unknownjump;\n");
    }
  else if(op1 == 0x1 && op2 == 0xf)
    {
      fprintf(f, "      /* TRAP #%d */\n", r2);
      if (r2 == TRAP_SYSCALL) /* general syscall ABI */
	{
	  fprintf(f, "        {\n");
	  fprintf(f, "          tgx_syscall_data d = { 0x%08x, gr0, gr1, gr2, gr3 };\n", (unsigned) pc);
	  fprintf(f, "          (*(callbacks->syscall))(info, &d);\n");
	  fprintf(f, "          gr2 = d.errcode;\n");
	  fprintf(f, "          gr0 = d.result;\n");
	  fprintf(f, "          gr1 = d.result2;\n");
	  fprintf(f, "        }\n");
	}
      else if (r2 == 1) /* gdb breakpoint */
	{
	  fprintf(f, "      rc = %d;\n", GX_F_HALT);
	  fprintf(f, "      goto save;\n");
	}
      else
	{
	  fprintf(f, "      cr6 = 0x%08x;\n", (unsigned)(pc + 4));
	  fprintf(f, "      cr0 = (cr0 & 0x000040) | (sm ? 0x80 : 0 ) | (cond ? 1 : 0);\n");
	  fprintf(f, "      cr0 = (cr0 & 0x0000ff) << 8 | (cr0 & 0x0000ff);\n");
	  fprintf(f, "      npc = 0x%08x;\n", 0x40 + (r2 * 4)); /* EIT_TRAP_BASE_ADDR */
	  fprintf(f, "      goto unknownjump;\n");
	}
    }

  else if(op1 == 0x2 && op2 == 0x0)
    {
      fprintf(f, "      /* STB R%d,@R%d */\n", r1, r2);
      fprintf(f, "      (*(callbacks->store1))(info, 0x%08x, gr%d, gr%d & 0xff);\n", (unsigned)pc, r2, r1);
    }
  else if(op1 == 0x2 && op2 == 0x2)
    {
      fprintf(f, "      /* STH R%d,@R%d */\n", r1, r2);
      fprintf(f, "      (*(callbacks->store2))(info, 0x%08x, gr%d, gr%d & 0x0000ffff);\n", (unsigned)pc, r2, r1);
    }
  else if(op1 == 0x2 && op2 == 0x4)
    {
      fprintf(f, "      /* ST R%d,@R%d */\n", r1, r2);
      fprintf(f, "      (*(callbacks->store))(info, 0x%08x, gr%d, gr%d);\n", (unsigned)pc, r2, r1);
    }
  else if(op1 == 0x2 && op2 == 0x5)
    {
      fprintf(f, "      /* UNLOCK R%d,@R%d */\n", r1, r2);
      fprintf(f, "      if(lock)\n");
      fprintf(f, "        {\n");
      fprintf(f, "          lock = 0;\n");
      fprintf(f, "          (*(callbacks->store))(info, 0x%08x, gr%d, gr%d);\n", (unsigned)pc, r2, r1);
      fprintf(f, "        }\n");
    }
  else if(op1 == 0x2 && op2 == 0x6)
    {
      fprintf(f, "      /* ST R%d,@+R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d + 4;\n", r2, r2);
      fprintf(f, "      (*(callbacks->store))(info, 0x%08x, gr%d, gr%d);\n", (unsigned)pc, r2, r1);
    }
  else if(op1 == 0x2 && op2 == 0x7)
    {
      fprintf(f, "      /* ST R%d,@-R%d */\n", r1, r2);
      fprintf(f, "      gr%d = gr%d - 4;\n", r2, r2);
      fprintf(f, "      (*(callbacks->store))(info, 0x%08x, gr%d, gr%d);\n", (unsigned)pc, r2, r1);
    }
  else if(op1 == 0x2 && op2 == 0x8)
    {
      fprintf(f, "      /* LDB R%d,@R%d */\n", r1, r2);
      fprintf(f, "      gr%d = (*(callbacks->load1))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
    }
  else if(op1 == 0x2 && op2 == 0x9)
    {
      fprintf(f, "      /* LDUB R%d,@R%d */\n", r1, r2);
      fprintf(f, "      gr%d = (unsigned char)(*(callbacks->load1))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
    }
  else if(op1 == 0x2 && op2 == 0xa)
    {
      fprintf(f, "      /* LDH R%d,@R%d */\n", r1, r2);
      fprintf(f, "      gr%d = (*(callbacks->load2))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
    }
  else if(op1 == 0x2 && op2 == 0xb)
    {
      fprintf(f, "      /* LDUH R%d,@R%d */\n", r1, r2);
      fprintf(f, "      gr%d = (unsigned short)(*(callbacks->load2))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
    }
  else if(op1 == 0x2 && op2 == 0xc)
    {
      fprintf(f, "      /* LD R%d,@R%d */\n", r1, r2);
      fprintf(f, "      gr%d = (*(callbacks->load))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
    }
  else if(op1 == 0x2 && op2 == 0xd)
    {
      fprintf(f, "      /* LOCK R%d,@R%d */\n", r1, r2);
      fprintf(f, "      lock = 1;\n");
      fprintf(f, "      gr%d = (*(callbacks->load))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
    }
  else if(op1 == 0x2 && op2 == 0xe)
    {
      fprintf(f, "      /* LD R%d,@R%d+ */\n", r1, r2);
      fprintf(f, "      gr%d = (*(callbacks->load))(info, 0x%08x, gr%d);\n", r1, (unsigned)pc, r2);
      fprintf(f, "      gr%d = gr%d + 4;\n", r2, r2);
    }

  else if(op1 == 0x4)
    {
      fprintf(f, "      /* ADDI R%d,#%d */\n", r1, c);
      fprintf(f, "      gr%d = gr%d + %d;\n", r1, r1, c);
    }

  else if(op1 == 0x5 && op2part == 0x0)
    {
      fprintf(f, "      /* SRLI R%d,#%d */\n", r1, c5);
      fprintf(f, "      gr%d = ((unsigned) gr%d) >> %d;\n", r1, r1, c5);
    }
  else if(op1 == 0x5 && op2part == 0x1)
    {
      fprintf(f, "      /* SRAI R%d,#%d */\n", r1, c5);
      fprintf(f, "      gr%d = gr%d >> %d;\n", r1, r1, c5);
    }
  else if(op1 == 0x5 && op2part == 0x2)
    {
      fprintf(f, "      /* SRLI R%d,#%d */\n", r1, c5);
      fprintf(f, "      gr%d = gr%d << %d;\n", r1, r1, c5);
    }

  else if(op1 == 0x6)
    {
      fprintf(f, "      /* LDI R%d,#%d */\n", r1, c);
      fprintf(f, "      gr%d = 0x%08x;\n", r1, c);
    }

  else if(op1 == 0x7 && r1 == 0xc)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) c) << 2);
      fprintf(f, "      /* BC %d */\n", c);
      fprintf(f, "      if (cond)\n");
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0x7 && r1 == 0xd)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) c) << 2);
      fprintf(f, "      /* BNC %d */\n", c);
      fprintf(f, "      if (! cond)\n");
      fprintf(f, "        {\n");
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "          goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "          npc = 0x%08x;\n", newpc);
	  fprintf(f, "          goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
      fprintf(f, "        }\n");
    }
  else if(op1 == 0x7 && r1 == 0xe)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) c) << 2);
      unsigned retpc = (pc & 0xfffffffc) + 4;
      fprintf(f, "      /* BL %d */\n", c);
      fprintf(f, "      gr14 = 0x%08x;\n", retpc);
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "      goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "      npc = 0x%08x;\n", newpc);
	  fprintf(f, "      goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
    }
  else if(op1 == 0x7 && r1 == 0xf)
    {
      unsigned newpc = (pc & 0xfffffffc) + (((int) c) << 2);
      fprintf(f, "      /* BRA %d */\n", c);
      if (optimized &&
	  (GX_PC_INCLUDES(gx,newpc)) &&
	  (GX_PC_FLAGS(gx, newpc) & GX_PCF_JUMPTARGET))
	{
	  fprintf(f, "      goto gx_label_%ld;\n",
		  ((newpc - gx->origin) / gx->divisor));
	}
      else
	{
	  fprintf(f, "      npc = 0x%08x;\n", newpc);
	  fprintf(f, "      goto %s;\n", (GX_PC_INCLUDES(gx,newpc)) ? "shortjump" : "longjump");
	}
    }

  else if(op1 == 0x7 && op2 == 0x0 && r1 == 0x0 && r1 == 0x0)
    {
      fprintf(f, "      /* NOP */\n");
    }

  else
    {
      /* illegal instruction */
      /* XXX */
      fprintf(f, "      printf(\"ILLEGAL INSN 0x%04x @ PC 0x%08x\\n\");\n", insn, (unsigned) pc);
      fprintf(f, "      abort();\n");
    }
}

