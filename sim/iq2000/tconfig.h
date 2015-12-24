/* IQ2000 target configuration file.  -*- C -*- */

/* See sim-hload.c.  We properly handle LMA. -- TODO: check this */
#define SIM_HANDLES_LMA 1

/* For MSPR support.  FIXME: revisit.  */
#define WITH_DEVICES 0

/* ??? Temporary hack until model support unified.  */
#define SIM_HAVE_MODEL

/* This is a global setting.  Different cpu families can't mix-n-match -scache
   and -pbb.  However some cpu families may use -simple while others use
   one of -scache/-pbb. ???? */
#define WITH_SCACHE_PBB 1
