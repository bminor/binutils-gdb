typedef struct _sim_cpu {
  unsigned_word reg[32];
  sim_cpu_base base;
  instruction_address cia;
} sim_cpu;

#define GPR(N) ((CPU)->reg[N])
