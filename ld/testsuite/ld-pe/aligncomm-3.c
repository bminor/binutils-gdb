
typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));

long s1 = 0;
long s2 = 0;
long s3 = 0;
__m128 r;
__m128 * volatile raddr = &r;

int main (int argc, const char **argv)
{
  return 15 & (int)raddr;
}

void __main (void)
{
  asm (".section .drectve\n"
	"  .ascii \" -aligncomm:_r,4\"\n"
	"  .ascii \" -aligncomm:r,4\"\n"
	"  .text");
}
