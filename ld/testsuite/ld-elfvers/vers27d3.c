extern void ref ();
extern void foo ();

void _start() __asm__("_start");

void
_start ()
{
  foo ();
  ref ();
}
