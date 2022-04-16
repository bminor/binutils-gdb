/* addrmap.c --- implementation of address map data structure.

   Copyright (C) 2007-2022 Free Software Foundation, Inc.

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

#include "defs.h"
#include "splay-tree.h"
#include "gdbsupport/gdb_obstack.h"
#include "addrmap.h"
#include "gdbsupport/selftest.h"

/* Make sure splay trees can actually hold the values we want to
   store in them.  */
gdb_static_assert (sizeof (splay_tree_key) >= sizeof (CORE_ADDR *));
gdb_static_assert (sizeof (splay_tree_value) >= sizeof (void *));


/* The base class for addrmaps.  */

struct addrmap : public allocate_on_obstack
{
  virtual ~addrmap () = default;

  virtual void set_empty (CORE_ADDR start, CORE_ADDR end_inclusive,
			  void *obj) = 0;
  virtual void *find (CORE_ADDR addr) const = 0;
  virtual struct addrmap *create_fixed (struct obstack *obstack) = 0;
  virtual void relocate (CORE_ADDR offset) = 0;
  virtual int foreach (addrmap_foreach_fn fn) = 0;
};


void
addrmap_set_empty (struct addrmap *map,
		   CORE_ADDR start, CORE_ADDR end_inclusive,
		   void *obj)
{
  map->set_empty (start, end_inclusive, obj);
}


void *
addrmap_find (const addrmap *map, CORE_ADDR addr)
{
  return map->find (addr);
}


struct addrmap *
addrmap_create_fixed (struct addrmap *original, struct obstack *obstack)
{
  return original->create_fixed (obstack);
}


/* Relocate all the addresses in MAP by OFFSET.  (This can be applied
   to either mutable or immutable maps.)  */
void
addrmap_relocate (struct addrmap *map, CORE_ADDR offset)
{
  map->relocate (offset);
}


int
addrmap_foreach (struct addrmap *map, addrmap_foreach_fn fn)
{
  return map->foreach (fn);
}

/* Fixed address maps.  */

/* A transition: a point in an address map where the value changes.
   The map maps ADDR to VALUE, but if ADDR > 0, it maps ADDR-1 to
   something else.  */
struct addrmap_transition
{
  CORE_ADDR addr;
  void *value;
};


struct addrmap_fixed : public addrmap
{
  void set_empty (CORE_ADDR start, CORE_ADDR end_inclusive,
		  void *obj) override;
  void *find (CORE_ADDR addr) const override;
  struct addrmap *create_fixed (struct obstack *obstack) override;
  void relocate (CORE_ADDR offset) override;
  int foreach (addrmap_foreach_fn fn) override;

  /* The number of transitions in TRANSITIONS.  */
  size_t num_transitions;

  /* An array of transitions, sorted by address.  For every point in
     the map where either ADDR == 0 or ADDR is mapped to one value and
     ADDR - 1 is mapped to something different, we have an entry here
     containing ADDR and VALUE.  (Note that this means we always have
     an entry for address 0).  */
  struct addrmap_transition *transitions;
};


void
addrmap_fixed::set_empty (CORE_ADDR start, CORE_ADDR end_inclusive,
			  void *obj)
{
  internal_error (__FILE__, __LINE__,
		  "addrmap_fixed_set_empty: "
		  "fixed addrmaps can't be changed\n");
}


void *
addrmap_fixed::find (CORE_ADDR addr) const
{
  const struct addrmap_transition *bottom = &transitions[0];
  const struct addrmap_transition *top = &transitions[num_transitions - 1];

  while (bottom < top)
    {
      /* This needs to round towards top, or else when top = bottom +
	 1 (i.e., two entries are under consideration), then mid ==
	 bottom, and then we may not narrow the range when (mid->addr
	 < addr).  */
      const addrmap_transition *mid = top - (top - bottom) / 2;

      if (mid->addr == addr)
	{
	  bottom = mid;
	  break;
	}
      else if (mid->addr < addr)
	/* We don't eliminate mid itself here, since each transition
	   covers all subsequent addresses until the next.  This is why
	   we must round up in computing the midpoint.  */
	bottom = mid;
      else
	top = mid - 1;
    }

  return bottom->value;
}


struct addrmap *
addrmap_fixed::create_fixed (struct obstack *obstack)
{
  internal_error (__FILE__, __LINE__,
		  _("addrmap_create_fixed is not implemented yet "
		    "for fixed addrmaps"));
}


void
addrmap_fixed::relocate (CORE_ADDR offset)
{
  size_t i;

  for (i = 0; i < num_transitions; i++)
    transitions[i].addr += offset;
}


int
addrmap_fixed::foreach (addrmap_foreach_fn fn)
{
  size_t i;

  for (i = 0; i < num_transitions; i++)
    {
      int res = fn (transitions[i].addr, transitions[i].value);

      if (res != 0)
	return res;
    }

  return 0;
}



/* Mutable address maps.  */

struct addrmap_mutable : public addrmap
{
  void set_empty (CORE_ADDR start, CORE_ADDR end_inclusive,
		  void *obj) override;
  void *find (CORE_ADDR addr) const override;
  struct addrmap *create_fixed (struct obstack *obstack) override;
  void relocate (CORE_ADDR offset) override;
  int foreach (addrmap_foreach_fn fn) override;

  /* The obstack to use for allocations for this map.  */
  struct obstack *obstack;

  /* A splay tree, with a node for each transition; there is a
     transition at address T if T-1 and T map to different objects.

     Any addresses below the first node map to NULL.  (Unlike
     fixed maps, we have no entry at (CORE_ADDR) 0; it doesn't 
     simplify enough.)

     The last region is assumed to end at CORE_ADDR_MAX.

     Since we can't know whether CORE_ADDR is larger or smaller than
     splay_tree_key (unsigned long) --- I think both are possible,
     given all combinations of 32- and 64-bit hosts and targets ---
     our keys are pointers to CORE_ADDR values.  Since the splay tree
     library doesn't pass any closure pointer to the key free
     function, we can't keep a freelist for keys.  Since mutable
     addrmaps are only used temporarily right now, we just leak keys
     from deleted nodes; they'll be freed when the obstack is freed.  */
  splay_tree tree;

  /* A freelist for splay tree nodes, allocated on obstack, and
     chained together by their 'right' pointers.  */
  splay_tree_node free_nodes;
};


/* Allocate a copy of CORE_ADDR in MAP's obstack.  */
static splay_tree_key
allocate_key (struct addrmap_mutable *map, CORE_ADDR addr)
{
  CORE_ADDR *key = XOBNEW (map->obstack, CORE_ADDR);

  *key = addr;
  return (splay_tree_key) key;
}


/* Type-correct wrappers for splay tree access.  */
static splay_tree_node
addrmap_splay_tree_lookup (const struct addrmap_mutable *map, CORE_ADDR addr)
{
  return splay_tree_lookup (map->tree, (splay_tree_key) &addr);
}


static splay_tree_node
addrmap_splay_tree_predecessor (const struct addrmap_mutable *map,
				CORE_ADDR addr)
{
  return splay_tree_predecessor (map->tree, (splay_tree_key) &addr);
}


static splay_tree_node
addrmap_splay_tree_successor (struct addrmap_mutable *map, CORE_ADDR addr)
{
  return splay_tree_successor (map->tree, (splay_tree_key) &addr);
}


static void
addrmap_splay_tree_remove (struct addrmap_mutable *map, CORE_ADDR addr)
{
  splay_tree_remove (map->tree, (splay_tree_key) &addr);
}


static CORE_ADDR
addrmap_node_key (splay_tree_node node)
{
  return * (CORE_ADDR *) node->key;
}


static void *
addrmap_node_value (splay_tree_node node)
{
  return (void *) node->value;
}


static void
addrmap_node_set_value (splay_tree_node node, void *value)
{
  node->value = (splay_tree_value) value;
}


static void
addrmap_splay_tree_insert (struct addrmap_mutable *map,
			   CORE_ADDR key, void *value)
{
  splay_tree_insert (map->tree,
		     allocate_key (map, key),
		     (splay_tree_value) value);
}


/* Without changing the mapping of any address, ensure that there is a
   tree node at ADDR, even if it would represent a "transition" from
   one value to the same value.  */
static void
force_transition (struct addrmap_mutable *self, CORE_ADDR addr)
{
  splay_tree_node n
    = addrmap_splay_tree_lookup (self, addr);

  if (! n)
    {
      n = addrmap_splay_tree_predecessor (self, addr);
      addrmap_splay_tree_insert (self, addr,
				 n ? addrmap_node_value (n) : NULL);
    }
}


void
addrmap_mutable::set_empty (CORE_ADDR start, CORE_ADDR end_inclusive,
			    void *obj)
{
  splay_tree_node n, next;
  void *prior_value;

  /* If we're being asked to set all empty portions of the given
     address range to empty, then probably the caller is confused.
     (If that turns out to be useful in some cases, then we can change
     this to simply return, since overriding NULL with NULL is a
     no-op.)  */
  gdb_assert (obj);

  /* We take a two-pass approach, for simplicity.
     - Establish transitions where we think we might need them.
     - First pass: change all NULL regions to OBJ.
     - Second pass: remove any unnecessary transitions.  */

  /* Establish transitions at the start and end.  */
  force_transition (this, start);
  if (end_inclusive < CORE_ADDR_MAX)
    force_transition (this, end_inclusive + 1);

  /* Walk the area, changing all NULL regions to OBJ.  */
  for (n = addrmap_splay_tree_lookup (this, start), gdb_assert (n);
       n && addrmap_node_key (n) <= end_inclusive;
       n = addrmap_splay_tree_successor (this, addrmap_node_key (n)))
    {
      if (! addrmap_node_value (n))
	addrmap_node_set_value (n, obj);
    }

  /* Walk the area again, removing transitions from any value to
     itself.  Be sure to visit both the transitions we forced
     above.  */
  n = addrmap_splay_tree_predecessor (this, start);
  prior_value = n ? addrmap_node_value (n) : NULL;
  for (n = addrmap_splay_tree_lookup (this, start), gdb_assert (n);
       n && (end_inclusive == CORE_ADDR_MAX
	     || addrmap_node_key (n) <= end_inclusive + 1);
       n = next)
    {
      next = addrmap_splay_tree_successor (this, addrmap_node_key (n));
      if (addrmap_node_value (n) == prior_value)
	addrmap_splay_tree_remove (this, addrmap_node_key (n));
      else
	prior_value = addrmap_node_value (n);
    }
}


void *
addrmap_mutable::find (CORE_ADDR addr) const
{
  splay_tree_node n = addrmap_splay_tree_lookup (this, addr);
  if (n != nullptr)
    {
      gdb_assert (addrmap_node_key (n) == addr);
      return addrmap_node_value (n);
    }

  n = addrmap_splay_tree_predecessor (this, addr);
  if (n != nullptr)
    {
      gdb_assert (addrmap_node_key (n) < addr);
      return addrmap_node_value (n);
    }

  return nullptr;
}


/* A function to pass to splay_tree_foreach to count the number of nodes
   in the tree.  */
static int
splay_foreach_count (splay_tree_node n, void *closure)
{
  size_t *count = (size_t *) closure;

  (*count)++;
  return 0;
}


/* A function to pass to splay_tree_foreach to copy entries into a
   fixed address map.  */
static int
splay_foreach_copy (splay_tree_node n, void *closure)
{
  struct addrmap_fixed *fixed = (struct addrmap_fixed *) closure;
  struct addrmap_transition *t = &fixed->transitions[fixed->num_transitions];

  t->addr = addrmap_node_key (n);
  t->value = addrmap_node_value (n);
  fixed->num_transitions++;

  return 0;
}


struct addrmap *
addrmap_mutable::create_fixed (struct obstack *obstack)
{
  struct addrmap_fixed *fixed;
  size_t num_transitions;

  /* Count the number of transitions in the tree.  */
  num_transitions = 0;
  splay_tree_foreach (tree, splay_foreach_count, &num_transitions);

  /* Include an extra entry for the transition at zero (which fixed
     maps have, but mutable maps do not.)  */
  num_transitions++;

  fixed = new (obstack) struct addrmap_fixed;
  fixed->num_transitions = 1;
  fixed->transitions = XOBNEWVEC (obstack, struct addrmap_transition,
				  num_transitions);
  fixed->transitions[0].addr = 0;
  fixed->transitions[0].value = NULL;

  /* Copy all entries from the splay tree to the array, in order 
     of increasing address.  */
  splay_tree_foreach (tree, splay_foreach_copy, fixed);

  /* We should have filled the array.  */
  gdb_assert (fixed->num_transitions == num_transitions);

  return fixed;
}


void
addrmap_mutable::relocate (CORE_ADDR offset)
{
  /* Not needed yet.  */
  internal_error (__FILE__, __LINE__,
		  _("addrmap_relocate is not implemented yet "
		    "for mutable addrmaps"));
}


/* This is a splay_tree_foreach_fn.  */

static int
addrmap_mutable_foreach_worker (splay_tree_node node, void *data)
{
  addrmap_foreach_fn *fn = (addrmap_foreach_fn *) data;

  return (*fn) (addrmap_node_key (node), addrmap_node_value (node));
}


int
addrmap_mutable::foreach (addrmap_foreach_fn fn)
{
  return splay_tree_foreach (tree, addrmap_mutable_foreach_worker, &fn);
}


static void *
splay_obstack_alloc (int size, void *closure)
{
  struct addrmap_mutable *map = (struct addrmap_mutable *) closure;
  splay_tree_node n;

  /* We should only be asked to allocate nodes and larger things.
     (If, at some point in the future, this is no longer true, we can
     just round up the size to sizeof (*n).)  */
  gdb_assert (size >= sizeof (*n));

  if (map->free_nodes)
    {
      n = map->free_nodes;
      map->free_nodes = n->right;
      return n;
    }
  else
    return obstack_alloc (map->obstack, size);
}


static void
splay_obstack_free (void *obj, void *closure)
{
  struct addrmap_mutable *map = (struct addrmap_mutable *) closure;
  splay_tree_node n = (splay_tree_node) obj;

  /* We've asserted in the allocation function that we only allocate
     nodes or larger things, so it should be safe to put whatever
     we get passed back on the free list.  */
  n->right = map->free_nodes;
  map->free_nodes = n;
}


/* Compare keys as CORE_ADDR * values.  */
static int
splay_compare_CORE_ADDR_ptr (splay_tree_key ak, splay_tree_key bk)
{
  CORE_ADDR a = * (CORE_ADDR *) ak;
  CORE_ADDR b = * (CORE_ADDR *) bk;

  /* We can't just return a-b here, because of over/underflow.  */
  if (a < b)
    return -1;
  else if (a == b)
    return 0;
  else
    return 1;
}


struct addrmap *
addrmap_create_mutable (struct obstack *obstack)
{
  struct addrmap_mutable *map = new (obstack) struct addrmap_mutable;

  map->obstack = obstack;

  /* splay_tree_new_with_allocator uses the provided allocation
     function to allocate the main splay_tree structure itself, so our
     free list has to be initialized before we create the tree.  */
  map->free_nodes = NULL;

  map->tree = splay_tree_new_with_allocator (splay_compare_CORE_ADDR_ptr,
					     NULL, /* no delete key */
					     NULL, /* no delete value */
					     splay_obstack_alloc,
					     splay_obstack_free,
					     map);

  return map;
}

/* See addrmap.h.  */

void
addrmap_dump (struct addrmap *map, struct ui_file *outfile, void *payload)
{
  /* True if the previously printed addrmap entry was for PAYLOAD.
     If so, we want to print the next one as well (since the next
     addrmap entry defines the end of the range).  */
  bool previous_matched = false;

  auto callback = [&] (CORE_ADDR start_addr, void *obj)
  {
    QUIT;

    bool matches = payload == nullptr || payload == obj;
    const char *addr_str = nullptr;
    if (matches)
      addr_str = host_address_to_string (obj);
    else if (previous_matched)
      addr_str = "<ends here>";

    if (matches || previous_matched)
      gdb_printf (outfile, "  %s%s %s\n",
		  payload != nullptr ? "  " : "",
		  core_addr_to_string (start_addr),
		  addr_str);

    previous_matched = matches;

    return 0;
  };

  addrmap_foreach (map, callback);
}

#if GDB_SELF_TEST
namespace selftests {

/* Convert P to CORE_ADDR.  */

static CORE_ADDR
core_addr (void *p)
{
  return (CORE_ADDR)(uintptr_t)p;
}

/* Check that &ARRAY[LOW]..&ARRAY[HIGH] has VAL in MAP.  */

#define CHECK_ADDRMAP_FIND(MAP, ARRAY, LOW, HIGH, VAL)			\
  do									\
    {									\
      for (unsigned i = LOW; i <= HIGH; ++i)				\
	SELF_CHECK (addrmap_find (MAP, core_addr (&ARRAY[i])) == VAL);	\
    }									\
  while (0)

/* Entry point for addrmap unit tests.  */

static void
test_addrmap ()
{
  /* We'll verify using the addresses of the elements of this array.  */
  char array[20];

  /* We'll verify using these values stored into the map.  */
  void *val1 = &array[1];
  void *val2 = &array[2];

  /* Create mutable addrmap.  */
  struct obstack temp_obstack;
  obstack_init (&temp_obstack);
  struct addrmap *map = addrmap_create_mutable (&temp_obstack);
  SELF_CHECK (map != nullptr);

  /* Check initial state.  */
  CHECK_ADDRMAP_FIND (map, array, 0, 19, nullptr);

  /* Insert address range into mutable addrmap.  */
  addrmap_set_empty (map, core_addr (&array[10]), core_addr (&array[12]),
		     val1);
  CHECK_ADDRMAP_FIND (map, array, 0, 9, nullptr);
  CHECK_ADDRMAP_FIND (map, array, 10, 12, val1);
  CHECK_ADDRMAP_FIND (map, array, 13, 19, nullptr);

  /* Create corresponding fixed addrmap.  */
  struct addrmap *map2 = addrmap_create_fixed (map, &temp_obstack);
  SELF_CHECK (map2 != nullptr);
  CHECK_ADDRMAP_FIND (map2, array, 0, 9, nullptr);
  CHECK_ADDRMAP_FIND (map2, array, 10, 12, val1);
  CHECK_ADDRMAP_FIND (map2, array, 13, 19, nullptr);

  /* Iterate over both addrmaps.  */
  auto callback = [&] (CORE_ADDR start_addr, void *obj)
    {
      if (start_addr == core_addr (nullptr))
	SELF_CHECK (obj == nullptr);
      else if (start_addr == core_addr (&array[10]))
	SELF_CHECK (obj == val1);
      else if (start_addr == core_addr (&array[13]))
	SELF_CHECK (obj == nullptr);
      else
	SELF_CHECK (false);
      return 0;
    };
  SELF_CHECK (addrmap_foreach (map, callback) == 0);
  SELF_CHECK (addrmap_foreach (map2, callback) == 0);

  /* Relocate fixed addrmap.  */
  addrmap_relocate (map2, 1);
  CHECK_ADDRMAP_FIND (map2, array, 0, 10, nullptr);
  CHECK_ADDRMAP_FIND (map2, array, 11, 13, val1);
  CHECK_ADDRMAP_FIND (map2, array, 14, 19, nullptr);

  /* Insert partially overlapping address range into mutable addrmap.  */
  addrmap_set_empty (map, core_addr (&array[11]), core_addr (&array[13]),
		     val2);
  CHECK_ADDRMAP_FIND (map, array, 0, 9, nullptr);
  CHECK_ADDRMAP_FIND (map, array, 10, 12, val1);
  CHECK_ADDRMAP_FIND (map, array, 13, 13, val2);
  CHECK_ADDRMAP_FIND (map, array, 14, 19, nullptr);

  /* Cleanup.  */
  obstack_free (&temp_obstack, NULL);
}

} // namespace selftests
#endif /* GDB_SELF_TEST */

void _initialize_addrmap ();
void
_initialize_addrmap ()
{
#if GDB_SELF_TEST
  selftests::register_test ("addrmap", selftests::test_addrmap);
#endif /* GDB_SELF_TEST */
}
