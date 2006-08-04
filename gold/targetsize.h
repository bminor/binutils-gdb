// targetsize.h -- representations which change size based on the target

#ifndef GOLD_TARGETSIZE_H
#define GOLD_TARGETSIZE_H

// Tell GNU/Linux inttypes.h that we want the formatting macros.
#define __STDC_FORMAT_MACROS

#include <stdint.h>
#include <inttypes.h>

namespace gold
{

// A template we use to get the right type sizes via specialization.
// We never actually use the default version.

template<int size>
struct Size_types
{
  typedef signed char Signed_type;
  typedef unsigned char Unsigned_type;
  static const char* signed_printf_dec_format();
  static const char* unsigned_printf_dec_format();
  static const char* unsigned_printf_hex_format();
};

template<>
struct Size_types<32>
{
  typedef int32_t Signed_type;
  typedef uint32_t Unsigned_type;
  static const char* signed_printf_dec_format()
  { return PRId32; }
  static const char* unsigned_printf_dec_format()
  { return PRIu32; }
  static const char* unsigned_printf_hex_format()
  { return PRIx32; }
};

template<>
struct Size_types<64>
{
  typedef int64_t Signed_type;
  typedef uint64_t Unsigned_type;
  static const char* signed_printf_dec_format()
  { return PRId64; }
  static const char* unsigned_printf_dec_format()
  { return PRIu64; }
  static const char* unsigned_printf_hex_format()
  { return PRIx64; }
};

} // End namespace gold.

#endif // !defined(GOLD_TARGETSIZE_H)
