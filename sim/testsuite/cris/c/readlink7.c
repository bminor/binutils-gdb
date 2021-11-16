/* Check that rare readlink calls don't cause the simulator to abort.
#notarget: cris*-*-elf
#sim: --env-unset PWD
 */
#include "readlink2.c"
