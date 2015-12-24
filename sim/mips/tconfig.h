/* mips target configuration file.  */

/* See sim-hload.c.  We properly handle LMA.  */
#ifdef TARGET_TX3904
#define SIM_HANDLES_LMA 1
#else
#define SIM_HANDLES_LMA 0
#endif

/* MIPS uses an unusual format for floating point quiet NaNs.  */
#define SIM_QUIET_NAN_NEGATED
