/* Common definitions for 129-bit capabilities

   Copyright (C) 2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GDBSUPPORT_CAPABILITY_H
#define GDBSUPPORT_CAPABILITY_H

#include <stdint.h>
#include <assert.h>
#include <string>

typedef unsigned __int128 uint128_t;

/* Helper functions for bit manipulation.  */

/* Return a value with a bit set in position X.  */

template <class T>
T _bit (uint32_t x)
{
  assert (x < sizeof (T) * 8);
  return ((T)1 << x);
}

/* Return a mask for LEN bits.  */

template <class T>
T _bmask (uint32_t len)
{
  assert (len <= sizeof (T) * 8);
  return _bit<T> (len) - 1;
}

/* Return a mask of LEN bits starting at position START.  */

template <class T>
T _bfmask (uint32_t start, uint32_t len)
{
  assert (len <= sizeof (T) * 8);
  return _bmask<T> (len) << start;
}

/* Sign-extend VALUE from PW bits to NW bits.  */

template <class T>
T _sign_extend (T value, uint32_t pw, uint32_t nw)
{
  assert (nw <= sizeof (T) * 8);
  assert (pw <= sizeof (T) * 8);

  if (value & ((T)1 << (pw - 1)))
    {
      for (int i = pw; i < nw; i++)
	value |= (T) 1 << i;
      return value;
    }
  return value;
}

/* Return bits <END:START> of VALUE (END >= START).  */

template <class T>
T _bits (T value, uint32_t end, uint32_t start)
{
  uint32_t len;

  assert (end >= start);

  if (start >= end)
    len = start - end + 1;
  else
    len = end - start + 1;

  assert (start < sizeof (T) * 8);
  assert (end < sizeof (T) * 8);
  assert (len <= sizeof (T) * 8);

  if (start >= end)
    return (value & _bfmask <T> (end, len)) >> end;

  return (value & _bfmask <T> (start, len)) >> start;
}

/* Return bit BIT from VALUE.  */

template <class T>
uint32_t _get_bit (T value, uint32_t bit)
{
  assert (bit < sizeof (T) * 8);

  return (value >> bit) & 1;
}

/* Set bits <END:START> of VALUE to V (END >= START).  */

template <class T1, class T2>
T1 _bfset (T1 value, T2 v, uint32_t end, uint32_t start)
{
  uint32_t len, shift;

  assert (end >= start);

  if (start >= end)
    {
      len = start - end + 1;
      shift = end;
    }
  else
    {
      len = end - start + 1;
      shift = start;
    }

  assert (start < sizeof (T1) * 8);
  assert (end < sizeof (T1) * 8);
  assert (len <= sizeof (T1) * 8);

  T1 mask = _bmask<T1> (len);

  return ((value & ~(mask << shift)) | ((v & mask) << shift));
}

/* Set bit BIT from VALUE to X.  */

template <class T>
T _set_bit (T value, uint32_t bit, uint32_t x)
{
  assert (bit < sizeof (T) * 8);

  return _bfset (value, x, bit, bit);
}

/* Return the number of leading zeroes from VALUE, adjusting it
   to a field of WIDTH size.  */

template <class T>
uint32_t _clz (T value, uint32_t width)
{
  assert (width <= sizeof (T) * 8);

  uint32_t zero_bits = 0;

  /* Find out the position of the top 1 bit.  */
  for (int i = 0; i < width; i++)
    {
      if ((value & 0x1) == 1)
	zero_bits = 0;
      else
	zero_bits++;

      value >>= 1;
    }

  return zero_bits;
}

/* Useful constants for use with capabilities.  */
enum cap_constants {
  CAP_BASE_EXP_HI_BIT = 66,

  CAP_BASE_HI_BIT = 79,
  CAP_BASE_LO_BIT = 64,

  CAP_BASE_MANTISSA_LO_BIT = 67,
  CAP_BASE_MANTISSA_NUM_BITS = CAP_BASE_HI_BIT-CAP_BASE_MANTISSA_LO_BIT+1,
  CAP_FLAGS_HI_BIT = 63,
  CAP_FLAGS_LO_BIT = 56,
  CAP_IE_BIT = 94,
  CAP_LIMIT_EXP_HI_BIT = 82,

  CAP_LIMIT_HI_BIT = 93,
  CAP_LIMIT_LO_BIT = 80,
  CAP_LIMIT_NUM_BITS = CAP_LIMIT_HI_BIT-CAP_LIMIT_LO_BIT+1,

  CAP_LIMIT_MANTISSA_LO_BIT = 83,
  CAP_LIMIT_MANTISSA_NUM_BITS = CAP_LIMIT_HI_BIT-CAP_LIMIT_MANTISSA_LO_BIT+1,
  CAP_MW = CAP_BASE_HI_BIT-CAP_BASE_LO_BIT+1,
  CAP_NO_SEALING = -1,

  CAP_OTYPE_HI_BIT = 109,
  CAP_OTYPE_LO_BIT = 95,
  CAP_OTYPE_NUM_BITS = CAP_OTYPE_HI_BIT-CAP_OTYPE_LO_BIT+1,

  CAP_MAX_OBJECT_TYPE = (1<<CAP_OTYPE_NUM_BITS)-1,

  CAP_PERMS_HI_BIT = 127,
  CAP_PERMS_LO_BIT = 110,
  CAP_PERMS_NUM_BITS = CAP_PERMS_HI_BIT-CAP_PERMS_LO_BIT+1,

  CAP_PERM_NONE = 0,

  CAP_PERM_GLOBAL = 1,
  CAP_PERM_GLOBAL_BIT = CAP_PERMS_LO_BIT,

  CAP_PERM_EXECUTIVE = (1<<1),
  CAP_PERM_EXECUTIVE_BIT = CAP_PERMS_LO_BIT+1,

  CAP_PERM_USER_HI_BIT = (1<<5),
  CAP_PERM_USER_LO_BIT = (1<<2),
  CAP_PERM_USER_NUM_BITS = CAP_PERM_USER_HI_BIT - CAP_PERM_USER_LO_BIT + 1,

  CAP_PERM_MUTABLE_LOAD = (1<<6),
  CAP_PERM_MUTABLE_LOAD_BIT = CAP_PERMS_LO_BIT+6,

  CAP_PERM_COMPARTMENT_ID = (1<<7),
  CAP_PERM_COMPARTMENT_ID_BIT = CAP_PERMS_LO_BIT+7,

  CAP_PERM_BRANCH_SEALED = (1<<8),
  CAP_PERM_BRANCH_SEALED_BIT = CAP_PERMS_LO_BIT+8,

  CAP_PERM_SYSTEM = (1<<9),
  CAP_PERM_SYSTEM_BIT = CAP_PERMS_LO_BIT+9,

  CAP_PERM_UNSEAL = (1<<10),
  CAP_PERM_UNSEAL_BIT = CAP_PERMS_LO_BIT+10,

  CAP_PERM_SEAL = (1<<11),
  CAP_PERM_SEAL_BIT = CAP_PERMS_LO_BIT+11,

  CAP_PERM_STORE_LOCAL = (1<<12),
  CAP_PERM_STORE_LOCAL_BIT = CAP_PERMS_LO_BIT+12,

  CAP_PERM_STORE_CAP = (1<<13),
  CAP_PERM_STORE_CAP_BIT = CAP_PERMS_LO_BIT+13,

  CAP_PERM_LOAD_CAP = (1<<14),
  CAP_PERM_LOAD_CAP_BIT = CAP_PERMS_LO_BIT+14,

  CAP_PERM_EXECUTE = (1<<15),
  CAP_PERM_EXECUTE_BIT = CAP_PERMS_LO_BIT+15,

  CAP_PERM_STORE = (1<<16),
  CAP_PERM_STORE_BIT = CAP_PERMS_LO_BIT+16,

  CAP_PERM_LOAD = (1<<17),
  CAP_PERM_LOAD_BIT = CAP_PERMS_LO_BIT+17,

  /* Equivalent to CHERI's OTYPE_RESERVED3.  */
  CAP_SEAL_TYPE_LB = 3,
  /* Equivalent to CHERI's OTYPE_RESERVED2.  */
  CAP_SEAL_TYPE_LPB = 2,
  /* Equivalent to CHERI's OTYPE_SENTRY.  */
  CAP_SEAL_TYPE_RB = 1,

  CAP_TAG_BIT = 128,
  CAP_VALUE_FOR_BOUND_HI_BIT = 55,

  CAP_VALUE_HI_BIT = 63,
  CAP_VALUE_LO_BIT = 0,
  CAP_VALUE_NUM_BITS = CAP_VALUE_HI_BIT-CAP_VALUE_LO_BIT+1,
  CAP_VALUE_FOR_BOUND_NUM_BITS = CAP_VALUE_FOR_BOUND_HI_BIT-CAP_VALUE_LO_BIT+1,
  CAP_BOUND_NUM_BITS = CAP_VALUE_NUM_BITS+1,
  CAP_MAX_ENCODEABLE_EXPONENT = 63,
  CAP_MAX_EXPONENT = CAP_VALUE_NUM_BITS-CAP_MW+2,
  CAP_LENGTH_NUM_BITS = CAP_VALUE_NUM_BITS+1,
  CAP_MAX_FIXED_SEAL_TYPE = 3
};

/* Structure defining a capability.  */
struct capability {

/*  From the Architecture document.

    The capability data type as an unforgeable token of authority provides a
    foundation for fine grained memory protection and strong
    compartmentalization.

    A capability is a composite data type with the following fields:

    * Value: Provides values used in capability-based operations

    * Bounds: Limits the range of capability values that can be used

    * Permissions: Controls how the capability can be used

    * Sealed: Restricts the use of a capability to unsealing operations

    * ObjectType: Determines how a sealed capability can be unsealed

    * Global: Restricts the locations where a capability can be stored

    * Executive: Controls banking of System registers

    * Flags: Holds unrestricted user data

    * Tag: Defines the validity of a capability

     -------------------------------
    | Bits    | Capability Field    |
     -------------------------------
    | 128     | Tag		    |
    | 127:110 | Permissions (18)    |
    | 109:95  | Object Type (15)    |
    | 94:64   |	Bounds (31)	    |
    | 63:56   |	Flags and Value(8)  |
    | 55:0    |	Bounds and Value(56)|
     -------------------------------
*/

  /* Tag bit */
  bool m_tag;

  /* The 128-bit capability.  This includes the value, bounds,
     flags, object type and permissions.  */
  uint128_t m_cap;

public:

/* Methods */

  capability (void)
  {
    m_tag = false;
    m_cap = 0;
  }

  capability (uint64_t upper, uint64_t lower)
  {
    uint128_t c = upper;
    c = c << 64;
    c |= lower;

    m_cap = c;
  }

  capability (uint128_t cap, bool tag)
  {
    m_tag = tag;
    m_cap = cap;
  }

  ~capability () {
    m_tag = false;
    m_cap = 0;
  }

  /* Return the tag bit.  */

  bool
  get_tag (void) const
  {
    return m_tag;
  }

  /* Set the tag bit.  */

  void
  set_tag (bool tag)
  {
    m_tag = tag;
  }

  /* Get the permissions field.  */

  uint32_t
  get_permissions (void) const
  {
    return _bits (m_cap, CAP_PERMS_HI_BIT, CAP_PERMS_LO_BIT);
  }

  /* Set the permissions field to PERMS.  */

  void
  set_permissions (uint32_t perms)
  {
    m_cap = _bfset (m_cap, perms, CAP_PERMS_HI_BIT, CAP_PERMS_LO_BIT);
  }

  /* Get the object type field.  */

  uint16_t
  get_otype (void) const
  {
    return _bits (m_cap, CAP_OTYPE_HI_BIT, CAP_OTYPE_LO_BIT);
  }

  /* Set the object type field to OTYPE.  */

  void
  set_otype (uint16_t otype)
  {
    m_cap = _bfset (m_cap, otype, CAP_OTYPE_HI_BIT, CAP_OTYPE_LO_BIT);
  }

  /* Return the flags field.  */

  uint16_t
  get_flags (void) const
  {
    return _bits (m_cap, CAP_FLAGS_HI_BIT, CAP_FLAGS_LO_BIT);
  }

  /* Set the flags field to FLAGS.  */

  void
  set_flags (uint16_t flags)
  {
    m_cap = _bfset (m_cap, flags, CAP_FLAGS_HI_BIT, CAP_FLAGS_LO_BIT);
  }

  /* Return the 64-bit value contained in the 128-bit capability.  */

  uint64_t
  get_value (void) const
  {
    return _bits (m_cap, CAP_VALUE_HI_BIT, CAP_VALUE_LO_BIT);
  }

  /* Set the 64-bit value contained in the 128-bit capability.  */

  void
  set_value (uint64_t v)
  {
    m_cap = _bfset (m_cap, v, CAP_VALUE_HI_BIT, CAP_VALUE_LO_BIT);
  }

  /* Return the 128-bit capability representation, without the tag bit.  */

  uint128_t
  get_capability (void) const
  {
    return m_cap;
  }

  /* Set the 128-bit capability representation, without the tag bit.  */

  void
  set_capability (uint128_t c)
  {
    m_cap = c;
  }

  /* Manipulation functions for capabilities.  */

  /* Returns a capability, derived from the input capability, with base address
     set to the value of the input capability and the length set to a given
     value.  If precise bounds setting is not possible, either the bounds are
     rounded, or tag is cleared, depending on the input exact flag.  */
  void set_bounds (capability &csrc, uint128_t req_len, bool exact);

  /* Returns the bounds as a CAP_BOUND_NUM_BITS bit tuple. As the top bound
     depends on the calculation of the bottom bound it better to always
     calculate them together The base can never have the CAP_BOUND_NUM_BITSth
     bit set.  However in order to do arithmetic combining them base and limit
     must be of the same type.  */
  void get_bounds (uint128_t &b, uint128_t &l);

  /* Returns the effective exponent in the range 0 to CAP_MAX_EXPONENT as used
     by bounds calculations.  */
  uint32_t get_effective_exponent (void) const;

  /* Returns the exponent in the range 0 to 63.  The Te and Be bits are stored
     inverted.  */
  uint32_t get_exponent (void) const;

  /* Returns true if the exponent is not in the legal range, false
     otherwise.  */
  bool is_exponent_out_of_range (void) const;

  /* Returns true if an internal exponent is in use, false otherwise.
     The Ie bit is stored inverted.  */
  bool is_internal_exponent (void) const;

  /* Returns the bottom value.  */
  uint16_t get_bottom (void) const;

  /* Returns the top value.  */
  uint16_t get_top (void);

  /* Return a possibly modified address suitable for generating bounds.  */
  uint64_t bounds_address (uint64_t address);

  /* Returns whether a capability current pointer is within the capability
     boundaries.  */
  bool is_in_bounds (void);

  /* Returns the base virtual address of a capability.  */
  uint64_t get_base (void);

  /* Returns limit address of a capability.  */
  uint64_t get_limit (void);

  /* Returns the length of the capability bounds range.  */
  uint64_t get_length (void);

  /* Returns whether a capability has all permissions in a given bit mask.  */
  bool check_permissions (uint32_t mask) const;

  /* Returns the input capability with permissions cleared according to a
     given bit mask.  */
  void clear_perms (uint32_t mask);

  /* Returns whether a range of addresses is within capability bounds.  */
  inline bool is_range_in_bounds (uint64_t start_address,
				  uint64_t length);

  /* Return a mask that can be used to align down addresses to a value
     that is sufficiently aligned to set precise bounds for the given
     nearest representable length.  */
  bool is_representable (uint64_t address);

  /* Return if the bounds are still representable if a new value is applied to
     an an existing capability. This version is used for CapAdd only and may
     exhibit false negatives vs the full CapIsRepresentable check for values
     which which are outside bounds.

     TODO This used extra functions from the base architecture to do exactly
     what the SAIL does so is not pure ASL.  */
  bool is_representable_fast (uint64_t address);

  /* Return if the bounds of two capbilities are equal.  */
  bool bounds_equal (struct capability &c);

  /* Return whether the capability bounds use value bits in the calculation.  */
  bool bounds_uses_value (uint32_t exponent);

  /* Returns the offset of the capability value relative to the capability
     base address.  */
  uint64_t get_offset (void);

  /* Returns the input capability with the address offset
     set to a given value.  */
  void set_offset (uint64_t offset);

  /* Returns true if the base is strictly greater than the limit, false
     otherwise.  */
  bool is_base_above_limit (void);

  /* Returns true if two capabilities are bitwise identical, false
     otherwise.  */
  bool is_equal (void) const;

  /* Returns true if the capability has the top 64 bits equal to zero.
     Returns false otherwise.  */

  bool is_null_derived (void);

  /* Printing functions.  */

  /* Returns a string representation of capability metadata.  */

  std::string metadata_str (void);

  /* Returns a string representation of the capability.

     If COMPACT is true, use a less verbose form.  Otherwise print
     the more verbose version.  */

  std::string to_str (bool compact);

  /* Print the capability.  */

  void print (void);

  /* Returns true if capability is sealed.
     Returns false otherwise.  */

  inline bool is_sealed (void)
  {
    return get_otype () != 0;
  }

  /* Returns true if this is a system access capability.
     Returns false otherwise.  */

  inline bool is_system_access (void)
  {
    return check_permissions (CAP_PERM_EXECUTE | CAP_PERM_SYSTEM);
  }

  /* Unit tests */

  void test_is_internal_exponent (void);
  void test_get_exponent (void);
  void test_get_effective_exponent (void);
  void test_get_bottom (void);
  void test_get_top (void);
  void test_bounds_address (void);
  void test_get_bounds (void);
  void test_set_bounds (void);
  void test_is_in_bounds (void);
  void test_flags (void);
  void test_object_types (void);
  void test_permissions (void);
};

extern void test_bit_functions (void);

#endif /* GDBSUPPORT_CAPABILITY_H */
