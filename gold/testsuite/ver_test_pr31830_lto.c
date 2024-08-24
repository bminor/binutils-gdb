extern __inline __attribute__((__gnu_inline__)) void foo(void) {}
extern void __collector_foo_2_2(void);
#if defined(__GNUC__) && __GNUC__ >= 10
__attribute__((__symver__("foo@GLIBC_2.2.5")))
#else
__asm__ (".symver __collector_foo_2_2, foo@GLIBC_2.2.5");
#endif
void __collector_foo_2_2(void) {}
void foo(void) {}
