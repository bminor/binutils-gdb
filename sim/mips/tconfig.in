/* mips target configuration file.  */

/* See sim-hload.c.  We properly handle LMA.  */
#ifdef TARGET_TX3904
#define SIM_HANDLES_LMA 1
#else
#define SIM_HANDLES_LMA 0
#endif

/* Define this if the simulator supports profiling.
   See the mips simulator for an example.
   This enables the `-p foo' and `-s bar' options.
   The target is required to provide sim_set_profile{,_size}.  */
#define SIM_HAVE_PROFILE

/* Define this if the simulator uses an instruction cache.
   See the h8/300 simulator for an example.
   This enables the `-c size' option to set the size of the cache.
   The target is required to provide sim_set_simcache_size.  */
/* #define SIM_HAVE_SIMCACHE */

/* Define this if the target cpu is bi-endian
   and the simulator supports it.  */
#define SIM_HAVE_BIENDIAN

/* MIPS uses an unusual format for floating point quiet NaNs.  */
#define SIM_QUIET_NAN_NEGATED
