#define TARGET_IS_BIG_ENDIAN_P
#define N_HEADER_IN_TEXT(x) 0
#define BYTES_IN_WORD 4
#define ARCH 32
#define ENTRY_CAN_BE_ZERO
#define N_SHARED_LIB(x) 0 /* Avoids warning */
#define TEXT_START_ADDR 0
#define PAGE_SIZE 4096
#define SEGMENT_SIZE PAGE_SIZE
#define DEFAULT_ARCH bfd_arch_m68k

#define MY(OP) CAT(hp300bsd_,OP)
#define TARGETNAME "a.out-hp300bsd"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libaout.h"

#include "aout-target.h"
