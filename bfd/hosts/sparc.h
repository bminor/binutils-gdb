#define STDC_HEADERS
#if defined(__STDC__) && __GNUC__ >= 2
#define abort __hide_abort
#define exit __hide_exit
#endif
#include "hosts/std-host.h"
#include <alloca.h>
#undef exit
#undef abort


