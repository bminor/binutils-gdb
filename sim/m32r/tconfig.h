/* M32R target configuration file.  -*- C -*- */

#ifndef M32R_TCONFIG_H
#define M32R_TCONFIG_H

/* For MSPR support.  FIXME: revisit.  */
#define WITH_DEVICES 1

/* This is a global setting.  Different cpu families can't mix-n-match -scache
   and -pbb.  However some cpu families may use -simple while others use
   one of -scache/-pbb.  */
#define WITH_SCACHE_PBB 1

#endif /* M32R_TCONFIG_H */
