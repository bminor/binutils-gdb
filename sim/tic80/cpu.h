typedef struct _sim_cpu {
  unsigned32 reg[32];
  unsigned64 acc[2];
  instruction_address cia;
  sim_cpu_base base;
} sim_cpu;

#define GPR(N) ((CPU)->reg[N])
#define ACC(N) ((CPU)->acc[N])
