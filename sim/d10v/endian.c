#ifndef ENDIAN_INLINE
#define NO_ENDIAN_INLINE
#include "d10v_sim.h"
#define ENDIAN_INLINE
#endif

ENDIAN_INLINE uint32
get_longword (x)
      uint8 *x;
{
  return ((uint32)x[0]<<24) + ((uint32)x[1]<<16) + ((uint32)x[2]<<8) + ((uint32)x[3]);
}

ENDIAN_INLINE int64
get_longlong (x)
      uint8 *x;
{
  uint32 top = ((uint32)x[0]<<24) + ((uint32)x[1]<<16) + ((uint32)x[2]<<8) + ((uint32)x[3]);
  uint32 bottom = ((uint32)x[4]<<24) + ((uint32)x[5]<<16) + ((uint32)x[6]<<8) + ((uint32)x[7]);
  return (((int64)top)<<32) | (int64)bottom;
}

ENDIAN_INLINE uint16
get_word (x)
      uint8 *x;
{
  return ((uint16)x[0]<<8) + x[1];
}

ENDIAN_INLINE void
write_word (addr, data)
     uint8 *addr;
     uint16 data;
{
  addr[0] = (data >> 8) & 0xff;
  addr[1] = data & 0xff;
}

ENDIAN_INLINE void
write_longword (addr, data)
     uint8 *addr;
     uint32 data;
{
  addr[0] = (data >> 24) & 0xff;
  addr[1] = (data >> 16) & 0xff;
  addr[2] = (data >> 8) & 0xff;
  addr[3] = data & 0xff;
}

ENDIAN_INLINE void
write_longlong (addr, data)
     uint8 *addr;
     int64 data;
{
  addr[0] = data >> 56;
  addr[1] = (data >> 48) & 0xff;
  addr[2] = (data >> 40) & 0xff;
  addr[3] = (data >> 32) & 0xff;
  addr[4] = (data >> 24) & 0xff;
  addr[5] = (data >> 16) & 0xff;
  addr[6] = (data >> 8) & 0xff;
  addr[7] = data & 0xff;
}
