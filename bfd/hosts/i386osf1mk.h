#include "hosts/std-host.h"

#include <machine/machparam.h>

#define	HAVE_STRERROR

#define	HOST_PAGE_SIZE		NBPG
#define	HOST_SEGMENT_SIZE	NBPG
#define	HOST_MACHINE_ARCH	bfd_arch_i386
#define	HOST_TEXT_START_ADDR	0x10000		/* By inspection */
#define	HOST_STACK_END_ADDR	KERNBASE
