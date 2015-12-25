/* Blackfin target configuration file.  -*- C -*- */

/* We use this so that we are passed the requesting CPU for HW acesses.
   Common sim core by default sets hw_system_cpu to NULL for WITH_HW.  */
#define WITH_DEVICES 1

/* ??? Temporary hack until model support unified.  */
#define SIM_HAVE_MODEL
