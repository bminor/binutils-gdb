/* Check that rare readlink calls don't cause the simulator to abort.
#notarget: cris*-*-elf
#sim: --argv0 $pwd/readlink6.c.x
*/
#include "readlink2.c"
