#ifndef SIM_MAIN_H
#define SIM_MAIN_H

/* The v850 has 32bit words, numbered 31 (MSB) to 0 (LSB) */

#define WITH_TARGET_WORD_MSB 31

#include "sim-basics.h"
#include "sim-base.h"

/**
 * TODO: Move these includes to the igen files that need them.
 * This requires extending the igen syntax to support header includes.
 */
#include "sim-signal.h"
#include "v850-sim.h"

#endif
