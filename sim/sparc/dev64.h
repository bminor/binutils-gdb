/* sparc64 device support
   Copyright (C) 1999 Cygnus Solutions.  */

#ifndef DEV64_H
#define DEV64_H

/* From libgloss/sparc/erc32-io.c.  */

#define ERC32_DEVICE_ADDR 0x1f80000
#define ERC32_DEVICE_LEN  (0x2000000 - 0x1f80000)

#define RXADATA 0x01F800E0
#define RXBDATA 0x01F800E4
#define RXSTAT 0x01F800E8

extern device sparc_devices;

/* FIXME: Temporary, until device support ready.  */
struct _device { int foo; };

#endif /* DEV64_H */
