#define WITH_TARGET_WORD_MSB 0
#define WITH_TARGET_WORD_BITSIZE 64
#define WITH_HOST_WORD_BITSIZE (sizeof (int) * 8)

#define ASSERT(EXPRESSION) \
{ \
  if (!(EXPRESSION)) { \
    fprintf (stderr, "%s:%d: assertion failed - %s\n", \
             __FILE__, __LINE__, #EXPRESSION); \
    abort (); \
  } \
}

#define SIM_BITS_INLINE (INCLUDE_MODULE | INCLUDED_BY_MODULE)

#include "sim-basics.h"
#include "sim-types.h"
#include "sim-bits.h"

#include "sim-alu.h"

#include <stdio.h>


typedef struct {
  char *op;
  unsigned64 arg;
} alu_op;

typedef struct {
  unsigned64 begin;
  alu_op ops[3];
  unsigned64 result;
  int carry;
  int overflow;
} alu_test;

#define MAX_INT16 (32767)
#define MIN_INT16 (32768)

const alu_test alu16_tests[] = {
  /* */
  { MAX_INT16, { { "ADD", 1 }, }, MIN_INT16, 0, 1, },
  { MIN_INT16, { { "ADD", -1 }, }, MAX_INT16, 1, 1, },
  { MAX_INT16, { { "ADD", MIN_INT16 }, }, -1, 0, 0, },
  { MIN_INT16, { { "ADD", MAX_INT16 }, }, -1, 0, 0, },
  { MAX_INT16, { { "ADD", MAX_INT16 }, }, MAX_INT16 * 2, 0, 1, },
  { MIN_INT16, { { "ADD", MIN_INT16 }, }, 0, 1, 1, },
  /* */
  { 0, { { "SUB", MIN_INT16 }, }, MIN_INT16, 0, 1, },
  { MAX_INT16, { { "SUB", MAX_INT16 }, }, 0, 0, 0, },
};


static void
print_hex (unsigned64 val, int nr_bits)
{
  switch (nr_bits)
    {
    case 16:
      printf ("0x%04lx", (long) (unsigned16) (val));
      break;
    case 32:
      printf ("0x%08lx", (long) (unsigned32) (val));
      break;
    case 64:
      printf ("0x%08lx%08lx",
	      (long) (unsigned32) (val >> 32),
	      (long) (unsigned32) (val));
    default:
      abort ();
    }
}


int errors = 0;


#define N 16
#include "alu-n-tst.h"
#undef N

#if 0
#define N 32
#include "alu-n-tst.h"
#undef N

#define N 64
#include "alu-n-tst.h"
#undef N
#endif

int
main ()
{
  int i;
  for (i = 0; i < sizeof (alu16_tests) / sizeof (*alu16_tests); i++)
    do_op_16 (alu16_tests + i);
  return (errors != 0);
}
