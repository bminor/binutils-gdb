#ifndef N
#error "N must be #defined"
#endif

#include "sim-xcat.h"

/* NOTE: see end of file for #undef of these macros */
#define unsignedN    XCONCAT2(unsigned,N)
#define OP_BEGIN     XCONCAT3(ALU,N,_BEGIN)
#define OP_ADD       XCONCAT3(ALU,N,_ADD)
#define OP_SUB       XCONCAT3(ALU,N,_SUB)
#define HAD_OVERFLOW (XCONCAT3(ALU,N,_HAD_OVERFLOW) != 0)
#define HAD_CARRY    (XCONCAT3(ALU,N,_HAD_CARRY) != 0)
#define RESULT          XCONCAT3(ALU,N,_RESULT)
#define OVERFLOW_RESULT XCONCAT3(ALU,N,_OVERFLOW_RESULT)
#define CARRY_RESULT    XCONCAT3(ALU,N,_CARRY_RESULT)
#define do_op_N      XCONCAT2(do_op_,N)

void
do_op_N (const alu_test *tst)
{
  const alu_op *op;
  /* without type cast */
  {
    OP_BEGIN (tst->begin);
    print_hex (tst->begin, N);
    for (op = tst->ops; op->op != NULL; op++)
      {
	printf (" %s ", op->op);
	print_hex (op->arg, N);
	if (strcmp (op->op, "add") == 0
	    || strcmp (op->op, "ADD") == 0)
	  OP_ADD (op->arg);
	else if (strcmp (op->op, "sub") == 0
		 || strcmp (op->op, "SUB") == 0)
	  OP_SUB (op->arg);
	else
	  {
	    printf (" -- operator unknown\n");
	    abort ();
	  }
      }
    printf (" = ");
    print_hex (tst->result, N);
    printf (" C%d V%d", tst->carry, tst->overflow);
    if (tst->carry != HAD_CARRY)
      {
	printf (" -- carry wrong %d", HAD_CARRY);
	errors ++;
      }
    if (tst->overflow != HAD_OVERFLOW)
      {
	printf (" -- overflow wrong %d", HAD_OVERFLOW);
	errors ++;
      }
    if ((unsignedN) CARRY_RESULT != (unsignedN) tst->result)
      {
	printf (" -- carry result wrong ");
	print_hex (CARRY_RESULT, N);
	errors ++;
      }
    if ((unsignedN) OVERFLOW_RESULT != (unsignedN) tst->result)
      {
	printf (" -- overflow result wrong ");
	print_hex (OVERFLOW_RESULT, N);
	errors ++;
      }
    if ((unsignedN) RESULT != (unsignedN) tst->result)
      {
	printf (" -- result wrong ");
	print_hex (RESULT, N);
	errors ++;
      }
    printf ("\n");
  }
}

#undef OP_BEGIN
#undef OP_ADD
#undef OP_SUB
#undef HAD_OVERFLOW
#undef HAD_CARRY
#undef OVERFLOW_RESULT
#undef CARRY_RESULT
#undef RESULT
#undef do_op_N
#undef unsignedN
