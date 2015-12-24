/* M32R target configuration file.  -*- C -*- */

#ifndef M32R_TCONFIG_H
#define M32R_TCONFIG_H

/* See sim-hload.c.  We properly handle LMA.  */
#define SIM_HANDLES_LMA 1

/* For MSPR support.  FIXME: revisit.  */
#define WITH_DEVICES 1

/* Define this to enable the intrinsic breakpoint mechanism. */
/* FIXME: may be able to remove SIM_HAVE_BREAKPOINT since it essentially
   duplicates ifdef SIM_BREAKPOINT (right?) */
#if 0
#define SIM_HAVE_BREAKPOINTS
#define SIM_BREAKPOINT { 0x10, 0xf1 }
#define SIM_BREAKPOINT_SIZE 2
#endif

/* This is a global setting.  Different cpu families can't mix-n-match -scache
   and -pbb.  However some cpu families may use -simple while others use
   one of -scache/-pbb.  */
#define WITH_SCACHE_PBB 1

#endif /* M32R_TCONFIG_H */
