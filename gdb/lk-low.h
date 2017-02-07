/* Basic Linux kernel support, architecture independent.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#ifndef __LK_LOW_H__
#define __LK_LOW_H__

#include "target.h"

extern struct target_ops *linux_kernel_ops;

/* Copy constants defined in Linux kernel.  */
#define LK_TASK_COMM_LEN 16
#define LK_BITS_PER_BYTE 8

/* Definitions used in linux kernel target.  */
#define LK_CPU_INVAL -1U

/* Private data structs for this target.  */
/* Forward declarations.  */
struct lk_private_hooks;
struct lk_ptid_map;

/* Short hand access to private data.  */
#define LK_PRIVATE ((struct lk_private *) linux_kernel_ops->to_data)
#define LK_HOOK (LK_PRIVATE->hooks)

struct lk_private
{
  /* Hashtab for needed addresses, structs and fields.  */
  htab_t data;

  /* Linked list to map between cpu number and original ptid from target
     beneath.  */
  struct lk_ptid_map *old_ptid;

  /* Hooks for architecture dependent functions.  */
  struct lk_private_hooks *hooks;
};

/* We use the following convention for PTIDs:

   ptid->pid = inferiors PID
   ptid->lwp = PID from task_stuct
   ptid->tid = address of task_struct

   The task_structs address as TID has two reasons.  First, we need it quite
   often and there is no other reasonable way to pass it down.  Second, it
   helps us to distinguish swapper tasks as they all have PID = 0.

   Furthermore we cannot rely on the target beneath to use the same PID as the
   task_struct. Thus we need a mapping between our PTID and the PTID of the
   target beneath. Otherwise it is impossible to pass jobs, e.g. fetching
   registers of running tasks, to the target beneath.  */

/* Private data struct to map between our and the target beneath PTID.  */

struct lk_ptid_map
{
  struct lk_ptid_map *next;
  unsigned int cpu;
  ptid_t old_ptid;
};

/* Private data struct to be stored in hashtab.  */

struct lk_private_data
{
  const char *alias;

  union
  {
    CORE_ADDR addr;
    struct type *type;
    struct field *field;
  } data;
};

/* Wrapper for htab_hash_string to work with our private data.  */

static inline hashval_t
lk_hash_private_data (const struct lk_private_data *entry)
{
  return htab_hash_string (entry->alias);
}

/* Function for htab_eq to work with our private data.  */

static inline int
lk_private_data_eq (const struct lk_private_data *entry,
		    const struct lk_private_data *element)
{
  return streq (entry->alias, element->alias);
}

/* Wrapper for htab_find_slot to work with our private data.  Do not use
   directly, use the macros below instead.  */

static inline void **
lk_find_slot (const char *alias)
{
  const struct lk_private_data dummy = { alias };
  return htab_find_slot (LK_PRIVATE->data, &dummy, INSERT);
}

/* Wrapper for htab_find to work with our private data.  Do not use
   directly, use the macros below instead.  */

static inline struct lk_private_data *
lk_find (const char *alias)
{
  const struct lk_private_data dummy = { alias };
  return (struct lk_private_data *) htab_find (LK_PRIVATE->data, &dummy);
}

/* Functions to initialize private data.  Do not use directly, use the
   macros below instead.  */

extern struct lk_private_data *lk_init_addr (const char *name,
					     const char *alias, int silent);
extern struct lk_private_data *lk_init_struct (const char *name,
					       const char *alias, int silent);
extern struct lk_private_data *lk_init_field (const char *s_name,
					      const char *f_name,
					      const char *alias, int silent);

/* The names we use to store our private data in the hashtab.  */

#define LK_STRUCT_NAME(s_name) ("struct " #s_name)
#define LK_FIELD_NAME(s_name, f_name) (#s_name " " #f_name)

/* Macros to initiate addresses and fields, where (S_/F_)NAME is the variables
   name as used in Linux.  LK_DECLARE_FIELD also initializes the corresponding
   struct entry.  Throws an error, if no symbol with the given name is found.
 */

#define LK_DECLARE_ADDR(name) \
  lk_init_addr (#name, #name, 0)
#define LK_DECLARE_FIELD(s_name, f_name) \
  lk_init_field (LK_STRUCT_NAME (s_name), #f_name,\
		 LK_FIELD_NAME (s_name, f_name), 0)

/* Same as LK_DECLARE_*, but returns NULL instead of throwing an error if no
   symbol was found.  The caller is responsible to check for possible errors.
 */

#define LK_DECLARE_ADDR_SILENT(name) \
  lk_init_addr (#name, #name, 1)
#define LK_DECLARE_FIELD_SILENT(s_name, f_name) \
  lk_init_field (LK_STRUCT_NAME (s_name), #f_name,\
		 LK_FIELD_NAME (s_name, f_name), 1)

/* Same as LK_DECLARE_*_SILENT, but allows you to give an ALIAS name.  If used
   for a struct, the struct has to be declared explicitly _before_ any of its
   fields.  They are ment to be used, when a variable in the kernel was simply
   renamed (at least from our point of view).  The caller is responsible to
   check for possible errors.  */

#define LK_DECLARE_ADDR_ALIAS(name, alias) \
  lk_init_addr (#name, #alias, 1)
#define LK_DECLARE_STRUCT_ALIAS(s_name, alias) \
  lk_init_struct (LK_STRUCT_NAME(s_name), LK_STRUCT_NAME (alias), 1)
#define LK_DECLARE_FIELD_ALIAS(s_alias, f_name, f_alias) \
  lk_init_field (LK_STRUCT_NAME (s_alias), #f_name, \
		 LK_FIELD_NAME (s_alias, f_alias), 1)

/* Macros to retrieve private data from hashtab. Returns NULL (-1) if no entry
   with the given ALIAS exists. The caller only needs to check for possible
   errors if not done so at initialization.  */

#define LK_ADDR(alias) \
  (lk_find (#alias) ? (lk_find (#alias))->data.addr : -1)
#define LK_STRUCT(alias) \
  (lk_find (LK_STRUCT_NAME (alias)) \
   ? (lk_find (LK_STRUCT_NAME (alias)))->data.type \
   : NULL)
#define LK_FIELD(s_alias, f_alias) \
  (lk_find (LK_FIELD_NAME (s_alias, f_alias)) \
   ? (lk_find (LK_FIELD_NAME (s_alias, f_alias)))->data.field \
   : NULL)


/* Definitions for architecture dependent hooks.  */
/* Hook to read registers from the target and supply their content
   to the regcache.  */
typedef void (*lk_hook_get_registers) (CORE_ADDR task,
				       struct target_ops *target,
				       struct regcache *regcache,
				       int regnum);

/* Hook to return the per_cpu_offset of cpu CPU.  Only architectures that
   do not use the __per_cpu_offset array to determin the offset have to
   supply this hook.  */
typedef CORE_ADDR (*lk_hook_get_percpu_offset) (unsigned int cpu);

/* Hook to map a running task to a logical CPU.  Required if the target
   beneath uses a different PID as struct rq.  */
typedef unsigned int (*lk_hook_map_running_task_to_cpu) (struct thread_info *ti);

struct lk_private_hooks
{
  /* required */
  lk_hook_get_registers get_registers;

  /* optional, required if __per_cpu_offset array is not used to determine
     offset.  */
  lk_hook_get_percpu_offset get_percpu_offset;

  /* optional, required if the target beneath uses a different PID as struct
     rq.  */
  lk_hook_map_running_task_to_cpu map_running_task_to_cpu;
};

/* Helper functions to read and return a value at a given ADDRess.  */
extern int lk_read_int (CORE_ADDR addr);
extern unsigned int lk_read_uint (CORE_ADDR addr);
extern LONGEST lk_read_long (CORE_ADDR addr);
extern ULONGEST lk_read_ulong (CORE_ADDR addr);
extern CORE_ADDR lk_read_addr (CORE_ADDR addr);

/* Reads a bitmap at a given ADDRess of size SIZE (in bits). Allocates and
   returns an array of ulongs.  The caller is responsible to free the array
   after it is no longer needed.  */
extern ULONGEST *lk_read_bitmap (CORE_ADDR addr, size_t size);

/* Walks the bitmap BITMAP of size SIZE from bit (index) BIT.
   Returns the index of the next set bit or SIZE, when the end of the bitmap
   was reached.  To iterate over all set bits use macro
   LK_BITMAP_FOR_EACH_SET_BIT defined below.  */
extern size_t lk_bitmap_find_next_bit (ULONGEST *bitmap, size_t bit,
				       size_t size);
#define LK_BITMAP_FOR_EACH_SET_BIT(bitmap, size, bit)			\
  for ((bit) = lk_bitmap_find_next_bit ((bitmap), (size), 0);		\
       (bit) < (size);							\
       (bit) = lk_bitmap_find_next_bit ((bitmap), (size), (bit) + 1))

/* Returns the size of BITMAP in bits.  */
#define LK_BITMAP_SIZE(bitmap) \
  (FIELD_SIZE (LK_FIELD (bitmap, bits)) * LK_BITS_PER_BYTE)

/* Returns the Hamming weight, i.e. number of set bits, of bitmap BITMAP with
   size SIZE (in bits).  */
extern size_t lk_bitmap_hweight (ULONGEST *bitmap, size_t size);


/* Short hand access to current gdbarchs builtin types and their
   size (in byte).  For TYPE replace spaces " " by underscore "_", e.g.
   "unsigned int" => "unsigned_int".  */
#define lk_builtin_type(type)					\
  (builtin_type (current_inferior ()->gdbarch)->builtin_##type)
#define lk_builtin_type_size(type)		\
  (lk_builtin_type (type)->length)

/* If field FIELD is an array returns its length (in #elements).  */
#define LK_ARRAY_LEN(field)			\
  (FIELD_SIZE (field) / FIELD_TARGET_SIZE (field))

/* Short hand access to the offset of field F_NAME in struct S_NAME.  */
#define LK_OFFSET(s_name, f_name)		\
  (FIELD_OFFSET (LK_FIELD (s_name, f_name)))

/* Returns the container of field FNAME of struct SNAME located at address
   ADDR.  */
#define LK_CONTAINER_OF(addr, sname, fname)		\
  ((addr) - LK_OFFSET (sname, fname))

/* Divides numinator N by demoniator D and rounds up the result.  */
#define LK_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))


/* Additional access macros to fields in the style of gdbtypes.h */
/* Returns the size of field FIELD (in bytes). If FIELD is an array returns
   the size of the whole array.  */
#define FIELD_SIZE(field)			\
  TYPE_LENGTH (check_typedef (FIELD_TYPE (*field)))

/* Returns the size of the target type of field FIELD (in bytes).  If FIELD is
   an array returns the size of its elements.  */
#define FIELD_TARGET_SIZE(field)		\
  TYPE_LENGTH (check_typedef (TYPE_TARGET_TYPE (FIELD_TYPE (*field))))

/* Returns the offset of field FIELD (in bytes).  */
#define FIELD_OFFSET(field)			\
  (FIELD_BITPOS (*field) / TARGET_CHAR_BIT)

/* Provides the per_cpu_offset of cpu CPU.  If the architecture
   provides a get_percpu_offset hook, the call is passed to it.  Otherwise
   returns the __per_cpu_offset[CPU] element.  */
extern CORE_ADDR lk_get_percpu_offset (unsigned int cpu);

/* Tests if a given task TASK is running. Returns either the cpu-id
   if running or LK_CPU_INVAL if not.  */
extern unsigned int lk_task_running (CORE_ADDR task);
#endif /* __LK_LOW_H__ */
