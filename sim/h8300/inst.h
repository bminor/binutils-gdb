#define DEBUG

#define MPOWER 16
#define MSIZE (1<<MPOWER)
#define CSIZE 1000
/* Local register names */
typedef enum
{
  R0, R1, R2, R3, R4, R5, R6, R7,
  R_ZERO,
  R_PC,				
  R_CCR,
  R_HARD_0,			
  R_LAST,
} reg_type;


/* Structure used to describe addressing */

typedef struct
{
  int type;
  int reg;
  int literal;
} ea_type;



typedef struct
{
  ea_type src;
  ea_type dst;
  int opcode;
  int next_pc;
  int oldpc;
  int cycles;
#ifdef DEBUG
struct h8_opcode *op;
#endif
}

decoded_inst;



typedef struct
{
  int exception;
  unsigned  int regs[9];
  int pc;
  int ccr;
  

  unsigned char *memory;
  unsigned short *cache_idx;
  int cache_top;
  int maximum;
  int csize;
  int mask;
  
  decoded_inst *cache;
  int cycles;
  int insts;
  int ticks;
  int compiles;
#ifdef ADEBUG
  int stats[O_LAST];
#endif
}

cpu_state_type;
