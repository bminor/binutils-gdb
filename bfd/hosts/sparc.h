#define STDC_HEADERS
#undef FILE_OFFSET_IS_CHAR_INDEX
#if defined(__STDC__) && __GNUC__ >= 2
#define abort __hide_abort
#define exit __hide_exit
#endif
#include "hosts/std-host.h"
#include <alloca.h>
#include <memory.h>
#undef exit
#undef abort
