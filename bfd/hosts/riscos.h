/* RISC/os 4.52C, and presumably other versions.  */

#include "hosts/std-host.h"

#include <bsd43/machine/machparam.h>
#include <bsd43/machine/vmparam.h>

#define HOST_TEXT_START_ADDR BSD43_USRTEXT
#define HOST_DATA_START_ADDR BSD43_USRDATA
#define HOST_STACK_END_ADDR BSD43_USRSTACK

/* This is just needed for a.out formats where the executable does not
   identify the architecture.  I'm not sure whether RISC/os is such a case
   or not.  */
#define HOST_MACHINE_ARCH bfd_arch_mips
