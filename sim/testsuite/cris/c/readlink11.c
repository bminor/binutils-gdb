/* As readlink5.c (sic), but specifying silent ENOSYS.
#notarget: cris*-*-elf
#sim: --cris-unknown-syscall=enosys-quiet --argv0 ./readlink11.c.x
#output: ENOSYS\n
#output: xyzzy\n
*/

#include "readlink2.c"
