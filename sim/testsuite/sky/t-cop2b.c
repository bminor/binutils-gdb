/* Copyright (C) 1998 Cygnus Solutions */
/* COP2 function test, with non-expert inline assembly */



/* globals */

int num_ok = 0;
int num_errors = 0;
float data_array[128] __attribute__((aligned(16)));



/* macros */

#define TEST(x) do_test(x, #x, __LINE__)



/* prototypes */


void enable_cop2();
void test00();
void do_test(int ok, const char* test, int line);



/* Utility functions */

void
enable_cop2()
{
  asm volatile (".set noat");
  asm volatile ("mfc0 $1,$12; dli $2,0x40000000; or $1,$2,$2; mtc0 $1,$12"
		: /* no outputs */
		: /* no inputs */
		: "$1", "$2" /* clobbered */);
  asm volatile (".set at");  
}


void
do_test(int ok, const char* test, int line)
{
  static int test_num = 0;

  printf("[%d @ %d] (%s): ", ++test_num, line, test);
  if(ok)
    {
      num_ok ++;
      printf("ok\n");
    }
  else
    {
      num_errors ++;
      printf("ko\n");
    }
}



/* Tests */


/* test00: test LQC2/SQC2 data non-corruption */
void test00()
{
  volatile float* data = & data_array[0];
  volatile float* data2 = & data_array[4];

  /* stuff some initial values */
  data[0] = -10.0;
  data[1] = +10.0;
  data[2] = -20.0;
  data[3] = +20.0;

  /* save values */
  asm volatile ("lqc2 vf01,%0"
		: /* no output */
		: "m" (data[0]) /* input */
		: "memory" /* clobbered */);

  /* test no clobbering */
  TEST(data[0] == -10.0f);
  TEST(data[1] == +10.0f);
  TEST(data[2] == -20.0f);
  TEST(data[3] == +20.0f);

  /* overwrite with VU constants */
  asm volatile ("sqc2 vf00,%0"
		: /* no outputs */
		: "m" (data[0]) /* input */
		: "memory" /* clobbered */);

  /* test proper values */
  TEST(data[0] == 0.0f);
  TEST(data[1] == 0.5f);
  TEST(data[2] == -1.0f);
  TEST(data[3] == +1.0f);

  /* read back original array values */
  asm volatile ("sqc2 vf01,%0"
		: /* no outputs */
		: "m" (data2[0]) /* input */
		: "memory" /* clobbered */);

  /* printf("%f,%f,%f,%f\n", data2[0], data2[1], data2[2], data2[3]); */

  /* test proper values */
  TEST(data2[0] == -10.0f);
  TEST(data2[1] == +10.0f);
  TEST(data2[2] == -20.0f);
  TEST(data2[3] == +20.0f);
} 



/* Mainline */


int main()
{
  enable_cop2();

  /* tests */
  test00();


  /* summarize */
  printf("%d ok, %d bad\n", num_ok, num_errors);
  if(num_errors > 0)
    exit(47);
  else
    exit(0);
}
