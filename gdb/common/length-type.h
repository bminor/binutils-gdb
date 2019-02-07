/* Length types for GDB.

   Copyright (C) 2019 Free Software Foundation, Inc.

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

#ifndef COMMON_LENGH_TYPE_H
#define COMMON_LENGH_TYPE_H

#include <limits>

template<typename T>
class gdb_len
{
public:
  T m_len;

  gdb_len () = default;

  gdb_len (T l)
    : m_len (l)
  {
  }

  gdb_len (int l)
    : m_len (l)
  {
  }

  /* const T get () const */
  /* { */
  /*   return m_len; */
  /* } */

  /* void set (const T l) */
  /* { */
  /*   m_len = l; */
  /* } */

  const char *c_str () const
  {
    return std::numeric_limits<T>::is_signed
      ? plongest (m_len) : pulongest (m_len);
  }

  explicit operator int () const
  {
    if (m_len > std::numeric_limits<int>::max ())
      error (_("Cannot cast variable: too long"));

    return static_cast<int> (m_len);
  }

  explicit operator unsigned int () const
  {
    if (std::numeric_limits<T>::is_signed && m_len < 0)
      error (_("Cannot cast negative variable."));

    if (m_len > std::numeric_limits<unsigned int>::max ())
      error (_("Cannot cast variable: too long."));

    return static_cast<unsigned int> (m_len);
  }

  /* explicit operator size_t () const */
  /* { */
  /*   if (std::numeric_limits<T>::is_signed && m_len < 0) */
  /*     error (_("Cannot cast negative variable to 'size_t'.")); */

  /*   if (m_len > std::numeric_limits<size_t>::max ()) */
  /*     error (_("Cannot cast variable: too long")); */

  /*   return static_cast<size_t> (m_len); */
  /* } */

  explicit operator ULONGEST () const
  {
    if (!std::numeric_limits<T>::is_signed)
      return m_len;

    if (m_len < 0)
      error (_("Cannot cast negative variable to 'ULONGEST'."));

    return static_cast<ULONGEST> (m_len);
  }

  explicit operator LONGEST () const
  {
    if (std::numeric_limits<T>::is_signed)
      return m_len;

    if (m_len > std::numeric_limits<T>::max ())
      error (_("Cannot cast variable: too long"));

    return static_cast<LONGEST> (m_len);
  }

#define DEFINE_LENGTH_REL_OP_TYPE(TYPE, OP)		\
  constexpr bool					\
  operator OP (const TYPE rhs)				\
  {							\
    return (static_cast<TYPE> (*this) OP rhs);		\
  }							\

#define DEFINE_ALL_LENGTH_REL_OP_TYPE(TYPE)			\
  DEFINE_LENGTH_REL_OP_TYPE (TYPE, >)				\
  DEFINE_LENGTH_REL_OP_TYPE (TYPE, >=)				\
  DEFINE_LENGTH_REL_OP_TYPE (TYPE, <)				\
  DEFINE_LENGTH_REL_OP_TYPE (TYPE, <=)				\
  DEFINE_LENGTH_REL_OP_TYPE (TYPE, ==)				\
  DEFINE_LENGTH_REL_OP_TYPE (TYPE, !=)

DEFINE_ALL_LENGTH_REL_OP_TYPE (int)
DEFINE_ALL_LENGTH_REL_OP_TYPE (ULONGEST)
DEFINE_ALL_LENGTH_REL_OP_TYPE (LONGEST)

#define DEFINE_LENGTH_REL_OP(OP)			\
  constexpr bool					\
  operator OP (const gdb_len<T> &rhs)			\
  {							\
    return (m_len OP rhs.m_len);			\
  }

DEFINE_LENGTH_REL_OP (>)
DEFINE_LENGTH_REL_OP (>=)
DEFINE_LENGTH_REL_OP (<)
DEFINE_LENGTH_REL_OP (<=)
DEFINE_LENGTH_REL_OP (==)
DEFINE_LENGTH_REL_OP (!=)


#define DEFINE_LENGTH_ARITHM_OP_TYPE(TYPE, OP)				\
  gdb_len<T> &								\
  operator OP ## = (const TYPE rhs)					\
  {									\
    if (std::numeric_limits<TYPE>::is_signed				\
        && !std::numeric_limits<T>::is_signed				\
        && rhs < 0)							\
      error (_("Cannot cast negative variable to unsigned type."));	\
    else if (!std::numeric_limits<TYPE>::is_signed			\
	     && std::numeric_limits<T>::is_signed			\
	     && rhs > std::numeric_limits<T>::max ())			\
      error (_("Cannot cast variable: too long."));			\
									\
    m_len OP ## = static_cast<T> (rhs);					\
    return *this;							\
  }									\
									\
  friend constexpr gdb_len<T>						\
  operator OP (const gdb_len<T> &lhs, const TYPE rhs)			\
  {									\
    return gdb_len<T> (static_cast<TYPE> (lhs) OP rhs);			\
  }									\
									\
  friend constexpr gdb_len<T>						\
  operator OP (const TYPE rhs, const gdb_len<T> &lhs)			\
  {									\
    return gdb_len<T> (rhs OP static_cast<TYPE> (lhs));			\
  }

#define DEFINE_ALL_LENGTH_ARITHM_OP_TYPE(TYPE)	  \
  DEFINE_LENGTH_ARITHM_OP_TYPE (TYPE, +)	  \
  DEFINE_LENGTH_ARITHM_OP_TYPE (TYPE, -)	  \
  DEFINE_LENGTH_ARITHM_OP_TYPE (TYPE, *)	  \
  DEFINE_LENGTH_ARITHM_OP_TYPE (TYPE, /)	  \
  DEFINE_LENGTH_ARITHM_OP_TYPE (TYPE, %)

DEFINE_ALL_LENGTH_ARITHM_OP_TYPE (int)
DEFINE_ALL_LENGTH_ARITHM_OP_TYPE (unsigned int)
DEFINE_ALL_LENGTH_ARITHM_OP_TYPE (ULONGEST)
DEFINE_ALL_LENGTH_ARITHM_OP_TYPE (LONGEST)

#define DEFINE_LENGTH_ARITHM_OP(OP)				\
  gdb_len<T> &							\
  operator OP ## = (const gdb_len<T> &rhs)			\
  {								\
    m_len OP ## = rhs.m_len;					\
    return *this;						\
  }								\
								\
  friend constexpr gdb_len<T>					\
  operator OP (const gdb_len<T> &lhs, const gdb_len<T> &rhs)	\
  {								\
    return gdb_len<T> (lhs.m_len OP rhs.m_len);			\
  }

DEFINE_LENGTH_ARITHM_OP (+)
DEFINE_LENGTH_ARITHM_OP (-)
DEFINE_LENGTH_ARITHM_OP (*)
DEFINE_LENGTH_ARITHM_OP (/)
DEFINE_LENGTH_ARITHM_OP (%)
DEFINE_LENGTH_ARITHM_OP (&)

private:
};

typedef gdb_len<ULONGEST> length_t;
typedef gdb_len<LONGEST> slength_t;


/* Operators for length_t and slength_t.  */

#define DEFINE_ARITHM_OPS(OP)					\
  length_t &							\
  operator OP ## = (length_t &lhs, const slength_t &rhs)	\
  {								\
    lhs.m_len OP ## = static_cast<ULONGEST> (rhs);		\
    return lhs;							\
  }								\
								\
  slength_t &							\
  operator OP ## = (slength_t &lhs, const length_t &rhs)	\
  {								\
    lhs.m_len OP ## = static_cast<LONGEST> (rhs);		\
    return lhs;							\
  }								\
								\
  slength_t							\
  operator OP (const length_t &lhs, const slength_t &rhs)	\
  {								\
    return static_cast<LONGEST> (lhs) OP rhs;			\
  }								\
								\
  slength_t							\
  operator OP (const slength_t &lhs, const length_t &rhs)	\
  {								\
    return lhs OP static_cast<LONGEST> (rhs);			\
  }

DEFINE_ARITHM_OPS (+)
DEFINE_ARITHM_OPS (-)
DEFINE_ARITHM_OPS (*)
DEFINE_ARITHM_OPS (/)
DEFINE_ARITHM_OPS (%)






#if 0

#define DEFINE_LENGTH_ARITHM_OP(TYPE, OP)			\
  constexpr TYPE						\
  operator OP (TYPE lhs, ULONGEST rhs)				\
  {								\
    return (TYPE) (lhs.m_len OP rhs);				\
  }								\
								\
  constexpr TYPE						\
  operator OP (ULONGEST lhs, TYPE rhs)				\
  {								\
    return (TYPE) (lhs OP static_cast<ULONGEST> (rhs));		\
  }								\
								\
  length_t &							\
  operator OP ## = (length_t &lhs, ULONGEST rhs)		\
  {								\
    lhs = (length_t) (static_cast<ULONGEST> (lhs) OP rhs);	\
    return lhs;							\
  }								\
								\
  constexpr length_t						\
  operator OP (length_t lhs, length_t rhs)			\
  {								\
    return (length_t) (static_cast<ULONGEST> (lhs)		\
		       OP static_cast<ULONGEST> (rhs));		\
  }								\
								\
  length_t &							\
  operator OP ## = (length_t &lhs, length_t rhs)		\
  {								\
    lhs = (length_t) (static_cast<ULONGEST> (lhs)		\
		      OP static_cast<ULONGEST> (rhs));		\
    return lhs;							\
  }								\

DEFINE_LENGTH_ARITHM_OP (+)
DEFINE_LENGTH_ARITHM_OP (-)
DEFINE_LENGTH_ARITHM_OP (*)
DEFINE_LENGTH_ARITHM_OP (/)
#endif


#if 0
enum class length_t : ULONGEST {};

enum class slenght_t : LONGEST {};

#define DEFINE_LENGTH_REL_OP(OP)					\
  constexpr bool							\
  operator OP (length_t lhs, int rhs)					\
  {									\
    return (lhs OP static_cast<ULONGEST> (rhs));			\
  }

DEFINE_LENGTH_REL_OP (>)
DEFINE_LENGTH_REL_OP (>=)
DEFINE_LENGTH_REL_OP (<)
DEFINE_LENGTH_REL_OP (<=)
DEFINE_LENGTH_REL_OP (==)
DEFINE_LENGTH_REL_OP (!=)

#define DEFINE_LENGTH_ARITHM_OP(OP)				\
  constexpr length_t						\
  operator OP (length_t lhs, ULONGEST rhs)			\
  {								\
    return (length_t) (static_cast<ULONGEST> (lhs) OP rhs);	\
  }								\
								\
  constexpr length_t						\
  operator OP (ULONGEST lhs, length_t rhs)			\
  {								\
    return (length_t) (lhs OP static_cast<ULONGEST> (rhs));	\
  }								\
								\
  length_t &							\
  operator OP ## = (length_t &lhs, ULONGEST rhs)		\
  {								\
    lhs = (length_t) (static_cast<ULONGEST> (lhs) OP rhs);	\
    return lhs;							\
  }								\
								\
  constexpr length_t						\
  operator OP (length_t lhs, length_t rhs)			\
  {								\
    return (length_t) (static_cast<ULONGEST> (lhs)		\
		       OP static_cast<ULONGEST> (rhs));		\
  }								\
								\
  length_t &							\
  operator OP ## = (length_t &lhs, length_t rhs)		\
  {								\
    lhs = (length_t) (static_cast<ULONGEST> (lhs)		\
		      OP static_cast<ULONGEST> (rhs));		\
    return lhs;							\
  }								\

DEFINE_LENGTH_ARITHM_OP (+)
DEFINE_LENGTH_ARITHM_OP (-)
DEFINE_LENGTH_ARITHM_OP (*)
DEFINE_LENGTH_ARITHM_OP (/)

inline const char *
to_str (length_t &l)
{
  return pulongest (to_underlying (l));
}

inline int
to_int (length_t &l)
{
  return static_cast<int> (l);
}

inline ssize_t
to_ssize_t (length_t &l)
{
  return static_cast<ssize_t> (l);
}

inline LONGEST
to_LONGEST (length_t &l)
{
  return static_cast<LONGEST> (l);
}
#endif //0



#endif /* ! COMMON_LENGH_TYPE_H */
