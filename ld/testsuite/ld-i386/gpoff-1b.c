#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <syscall.h>

extern int foo;
extern int __gp;
extern int foo_gpoff;
extern int get_foo (void);
extern int get_foo_gpoff (void);

/* Structure passed to 'set_thread_area' syscall.  */
struct user_desc
{
  unsigned int entry_number;
  unsigned long int base_addr;
  unsigned int limit;
  unsigned int seg_32bit:1;
  unsigned int contents:2;
  unsigned int read_exec_only:1;
  unsigned int limit_in_pages:1;
  unsigned int seg_not_present:1;
  unsigned int useable:1;
  unsigned int empty:25;
};

/* Initializing bit fields is slow.  We speed it up by using a union.  */
union user_desc_init
{
  struct user_desc desc;
  unsigned int vals[4];
};

int
setup_gp (void *p)
{
  union user_desc_init segdescr;
  int result;

  /* Let the kernel pick a value for the 'entry_number' field.  */
  segdescr.vals[0] = -1;
  /* The 'base_addr' field.  */
  segdescr.vals[1] = (unsigned long int) p;
  /* The 'limit' field.  We use 4GB which is 0xfffff pages.  */
  segdescr.vals[2] = 0xfffff;
  /* Collapsed value of the bitfield:
     .seg_32bit = 1
     .contents = 0
     .read_exec_only = 0
     .limit_in_pages = 1
     .seg_not_present = 0
     .useable = 1 */
  segdescr.vals[3] = 0x51;
  result = syscall (SYS_set_thread_area, &segdescr.desc);
  if (result == 0)
    /* We know the index in the GDT, now load the segment register.
       The use of the GDT is described by the value 3 in the lower
       three bits of the segment descriptor value.
       Note that we have to do this even if the numeric value of
       the descriptor does not change.  Loading the segment register
       causes the segment information from the GDT to be loaded
       which is necessary since we have changed it.   */
    asm ("movw %w0, %%fs" :: "q" (segdescr.desc.entry_number * 8 + 3));

  return result;
}

int
main ()
{
  if (setup_gp (&__gp) == 0
      && foo == 0x12345678
      && *(int *) ((char *) &__gp + foo_gpoff) == 0x12345678
      && *(int *) ((char *) &__gp + get_foo_gpoff ()) == 0x12345678
      && get_foo () == 0x12345678)
    printf ("PASS\n");
  return 0;
}
