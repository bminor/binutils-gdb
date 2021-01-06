/* Make sure we don't just assume ELF all over.  (We have to jump
   through hoops to get runnable a.out out of the ELF setup, and
   having problems with a.out and discontinous section arrangements
   doesn't help.  Adjust as needed to get a.out which says "pass".  If
   necessary, move to the asm subdir.  By design, it doesn't work with
   CRIS v32.)

NB: We'd rely on kfail, but that doesn't skip compilation, and that's where
the crash in ld happens to break the testcase.
#target: disabled-cris-*-elf
#kfail: ld/13900 cris-*-elf
#cc: ldflags=-Wl,-mcrisaout\ -sim\ -Ttext=0
*/
#include "hello.c"
