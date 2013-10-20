/* DWARF 2 Arange-Set.
   Copyright 2007 Free Software Foundation, Inc.
   Contributed by Doug Kwan, Google Inc.
 
   This file is part of BFD.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libiberty.h"
#include "libbfd.h"
#include "arange-set.h"
#include "splay-tree.h"

/* Implementation of an arange-set.  The set is implemented using the
   splay tree support in libiberty.  The advantage of using this is
   that it has been well tested and is relatively simple to use.  The
   disadvantage is that it is too general and it does not fit our design
   exactly.  So we waste a bit of memory for unneeded generality and work
   around for mis-match between the splay tree API and the arange-set
   internals.  A specialized implentation of a balanced tree type for
   arange-set exclusively may speed up things a little and reduce memory
   consumption.  Until there is a pressing need, we stick to the splay
   tree in libiberty.  */

struct arange_set_s
{
  /* Splay tree containing aranges.  */
  splay_tree ranges;

  /* Lowest address in set.  If set is empty, it is ~0.  */
  bfd_vma lower_bound;

  /* Highest address in set.  If set is empty, it is 0.  */
  bfd_vma upper_bound;

  /* TRUE if aranges in this set have values.  */
  bfd_boolean value_p;

  /* Function to compare arange values.  */
  arange_value_equal_fn value_equal_fn;

  /* Function to copy an arange value.  */
  arange_value_copy_fn value_copy_fn;

  /* Function to combine arange values.  */
  arange_value_combine_fn value_combine_fn;

  /* Function to delete an arange value.  */
  arange_value_delete_fn value_delete_fn;

  /* Function to allocate a piece of memory.  */
  arange_set_allocate_fn allocate_fn;

  /* Function to deallocate a piece of memory.  */
  arange_set_deallocate_fn deallocate_fn;

  /* Call back data shared by all callbacks.  */
  void *data;
};

/* Structure for aranges with a value attached.  Since a splay tree
   node can only hold one value,  we need to use the container struct
   to store data associated with an arange and have the splay tree value
   to be a pointer to this struct. */

typedef struct
{
  /* High-pc of an arange.  This is different from the DWARF2 semantics that
     the high-pc is really the last location in an arange.  */
  bfd_vma high;

  /* We need to store a pointer to the set because splay_tree_value_delete
     only takes a pointer to the value deleted.  If we use a deallocator
     that need extra information like a pointer to the memory pool, we need to
     look up via the set pointer.  This adds one extra pointer per arange. */
  arange_set set;

  /* Value associated with this arange.  */
  arange_value_type value;

} arange_value_container_t;



static void
arange_set_delete_value (arange_set set, arange_value_type value)
{
  if (set->value_delete_fn)
    (set->value_delete_fn) (value, set->data);
}

/* Compare two VMAs as keys of splay tree nodes.  */

static int
splay_tree_compare_bfd_vmas (splay_tree_key k1, splay_tree_key k2)
{
  if ((bfd_vma) k1 < (bfd_vma) k2)
    return -1;
  else if ((bfd_vma) k1 > (bfd_vma) k2)
    return 1;

  return 0;
}

/* Default memory allocator and deallocator.  */

void *
arange_set_allocate (arange_set set, int size)
{
  if (set->allocate_fn)
    return (set->allocate_fn) (size, set->data); 

  return xmalloc (size);
}

void
arange_set_deallocate (arange_set set, void *object)
{
  if (set->deallocate_fn)
    (set->deallocate_fn) (object, set->data); 
  else
    free (object);
}

static void
arange_set_delete_value_container (splay_tree_value value)
{
  arange_value_container_t *container;

  container = (arange_value_container_t*) value;
  arange_set_delete_value (container->set, container->value);
  arange_set_deallocate (container->set, container);
}

/* Create an arange set.  Return the new set of NULL if there is any
   error.  

   allocate_fn is the memory allocator function of this arange set. If
   it is NULL, the default allocator will be used.

   deallocate_fn is the memory deallocator function of this arange set. If
   it is NULL, the default allocator will be used.

   value_p specifies whether an arange set supports values.  If it is
   TURE.  Each arange can be associated with a value of type arange_value_type.
   If it is FALSE, the following parameters value_equal_fn, value_copy_fn,
   value_combine_fn and value_delete_fn will be ignored.

   value_equal_fn is the value equality function.  An arange uses it to
   check if two values are the same.  If it is NULL, the default bit-wise
   equality function will be used.

   value_copy_fn is the value copy function.  An arange uses it to copy
   values of type arange_value_type.  If it is NULL, the default bit-wise
   copy function will be used.

   value_combine_fn is the value combine function. An arange uses it to
   combine values of two identical arange.  If it is NULL, the default
   constant zero function will be used.

   value_delete_fn is the value deletion function. If it is not NULL,
   it will be called when an arange deletes a value.

   data is pointer to an object, which will be passed to all allocate_fn,
   deallocate_fn, value_equal_fn, value_copy_fn, value_combine_fn and
   value_delete_fn.  */

arange_set
arange_set_new (arange_set_allocate_fn allocate_fn,
		arange_set_deallocate_fn deallocate_fn,
		bfd_boolean value_p,
		arange_value_equal_fn value_equal_fn,
		arange_value_copy_fn value_copy_fn,
		arange_value_combine_fn value_combine_fn,
		arange_value_delete_fn value_delete_fn,
		void *data)
{
  arange_set set;
  splay_tree sp;
  splay_tree_delete_value_fn fn;

  /* Allocate space for arange structure.  */
  set = (arange_set)
    (*allocate_fn) (sizeof (struct arange_set_s), data);
  if (!set)
    return set;
  
  fn = value_p ? arange_set_delete_value_container : NULL;
  sp = splay_tree_new_with_allocator (splay_tree_compare_bfd_vmas, NULL,
				      fn, allocate_fn, deallocate_fn,
				      data);
  if (!sp)
    {
      (deallocate_fn) (set, data);
      return NULL;
    }

  set->ranges = sp;
  set->lower_bound = ~0;
  set->upper_bound = 0;
  set->value_p = value_p;
  set->allocate_fn = allocate_fn;
  set->deallocate_fn = deallocate_fn;
  set->value_equal_fn = value_equal_fn;
  set->value_copy_fn = value_copy_fn;
  set->value_combine_fn = value_combine_fn;
  set->value_delete_fn = value_delete_fn;
  set->data = data;
  return set;
}

/*  Delete an arange set.  */

void
arange_set_delete (arange_set set)
{
  splay_tree_delete (set->ranges);
  (*set->deallocate_fn) (set, set->data);
}

/* Return TRUE if and only if arange set is empty.  */

bfd_boolean
arange_set_empty_p (arange_set set)
{
  return set->lower_bound > set->upper_bound;
}

/* Accessors for low and high of an arange.
 
   There is no arange_set_node_set_low since the low address is the
   key of the splay tree node.  */

/* Get the high VMA address of a node.  */

static bfd_vma
arange_set_node_high (arange_set set, splay_tree_node node)
{
  arange_value_container_t *container;

  if (set->value_p)
    {
      container = (arange_value_container_t*) node->value;
      return container->high;
    }

  return (bfd_vma) node->value;
}

/* Set the high VMA address of a node.  */

static void
arange_set_node_set_high (arange_set set, splay_tree_node node, bfd_vma address)
{
  arange_value_container_t *container;

  if (set->value_p)
    {
      container = (arange_value_container_t*) node->value;
      container->high = address;
    }
  else
    node->value = (splay_tree_value) address;
}

/* Get the low VMA address of a node.  */

static bfd_vma
arange_set_node_low (splay_tree_node node)
{
  return (bfd_vma) node->key;
}

/* If arange set supports values, return value of an arange; otheriwse
   always return 0 so that it appears that all aranges have the same value.  */

static arange_value_type
arange_set_node_value (arange_set set, splay_tree_node node)
{
  arange_value_container_t *container;

  if (set->value_p)
    {
      container = (arange_value_container_t*) node->value;
      return container->value;
    }

  return 0;
}

/* If arange set supports values, return value of an arange; otheriwse
   always return 0 so that it appears that all aranges have the same value.  */

static void
arange_set_node_set_value (arange_set set,
			   splay_tree_node node,
			   arange_value_type value)
{
  arange_value_container_t *container;

  if (set->value_p)
    {
      container = (arange_value_container_t*) node->value;
      container->value = value;
    }
}

/* Return TRUE if and only if arange set supports values.  */

bfd_boolean
arange_set_has_values_p (arange_set set)
{
  return set->value_p;
}

/* Copy a value using the value copying function of an arange set.  If
   the set does not support values or if there is not value copying
   function specified, it simply returns the input value.  */

arange_value_type
arange_set_copy_value (arange_set set, arange_value_type value)
{
  /* If no copy function is specified or set does not support values,
     default is bit-wise copy.  */
  if (set->value_p && set->value_copy_fn)
    return (set->value_copy_fn) (value, set->data);

  return value;
}

static arange_value_type
arange_set_combine_value (arange_set set,
			  arange_value_type value1,
			  arange_value_type value2)
{
  /* If no combine function is specified or set does not support values,
     default is returning 0.  */
  if (set->value_p && set->value_combine_fn)
    return (set->value_combine_fn) (value1, value2, set->data);

  return (arange_value_type) 0;
}

/* Compares two values for equality.  If the arange set does not support values
   or if no value equality function is specified, this function simply does
   a bit-wise comparison.  */

bfd_boolean
arange_set_value_equal_p (arange_set set,
			  arange_value_type value1,
			  arange_value_type value2)
{
  /* If no equality function is specified or set does not support values,
     default is bit-wise comparison.  */
  if (set->value_p && set->value_equal_fn)
    return (set->value_equal_fn) (value1, value2, set->data);

  return value1 == value2;
}

/* Check to see if a given address is in an arange set.  Return TRUE if the
   address is inside one of the aranges. If low_ptr, high_ptr and value_ptr are
   used to return lower address, upper address and value associated with a
   found arounge.  If anyone of them is NULL, the corresponding information
   is not returned.  For arange set without values, no information is returned
   through the pointer value_ptr.  */

bfd_boolean
arange_set_lookup_address (arange_set set, bfd_vma address,
			   bfd_vma *low_ptr, bfd_vma *high_ptr,
			   arange_value_type *value_ptr)
{
  splay_tree_node pred, node;

  if (address < set->lower_bound || address > set->upper_bound)
    return FALSE;

  /* Find immediate predecessor.  */
  pred = splay_tree_predecessor (set->ranges, (splay_tree_key) address);
  if (pred
      && arange_set_node_high (set, pred) >= address)
    node = pred;
  else
    /* If the predecessor range does not cover this address, the address
       is in the arange set only if itself starts an arange.  */
    node = splay_tree_lookup (set->ranges, (splay_tree_key) address);

  if (node)
    {
      /* Also return arange boundaries if caller supplies pointers.  */
      if (low_ptr)
	*low_ptr = arange_set_node_low (node);
      if (high_ptr)
	*high_ptr = arange_set_node_high (set, node);
      if (set->value_p && value_ptr)
	*value_ptr = arange_set_node_value (set, node);
      return TRUE;
    }

  return FALSE;
}

/* Insert an arange [low, high] into a set's splay tree.  If the set supports
   value, also insert with the given value.  Return the inserted node if there
   is no error or NULL otherwise.  */

static splay_tree_node
arange_set_splay_tree_insert (arange_set set,
			      bfd_vma low,
			      bfd_vma high,
			      arange_value_type value)
{
  splay_tree_value sp_value;
  arange_value_container_t *container;
   
  if (set->value_p)
    {
      int size = sizeof (arange_value_container_t);
      void *data = set->ranges->allocate_data;

      container =
	(arange_value_container_t*) (*set->ranges->allocate) (size, data);
      if (!container)
	return NULL;
      container->high = high;

      /* Due to the design of splay tree API, there is no way of passing
	 callback data to the splay tree value delete function.  Hence we need
	 to store a pointer to set in every containier!  */
      container->set = set;

      container->value = value;
      sp_value = (splay_tree_value) container;
    }
  else
    sp_value = (splay_tree_value) high;	

  /* Currently splay_tree_insert does not return any status to tell if there
     is an error.  */
  return splay_tree_insert (set->ranges, (splay_tree_key) low, sp_value);
}

/* Split [low, high] to [low, address) & [address, high].  */

static bfd_boolean
arange_set_split_node (arange_set set, splay_tree_node node, bfd_vma address)
{
  splay_tree_node node2;
  arange_value_type value;
  bfd_vma low, high;

  low = arange_set_node_low (node);
  high = arange_set_node_high (set, node);

  BFD_ASSERT (low < address && address <= high);
  
  value = arange_set_copy_value (set, arange_set_node_value (set, node));
  node2 = arange_set_splay_tree_insert (set, address, high, value);
  if (!node2)
    return FALSE;

  arange_set_node_set_high (set, node, address - 1);
  return TRUE;
}

static splay_tree_node
arange_set_maybe_merge_with_predecessor (arange_set set, splay_tree_node node)
{
  splay_tree_node pred;
  bfd_vma low, high;

  low = arange_set_node_low (node);
  high = arange_set_node_high (set, node);

  pred = splay_tree_predecessor (set->ranges, low);
  if (! pred)
    return node;

  if (arange_set_node_high (set, pred) + 1 == low
      && arange_set_value_equal_p (set,
				   arange_set_node_value (set, pred),
				   arange_set_node_value (set, node)))
    {
      splay_tree_remove (set->ranges, arange_set_node_low (node));
      arange_set_node_set_high (set, pred, high);
      return arange_set_maybe_merge_with_predecessor (set, pred);	
    }

  return node;
}

/* Insert an arange [low,high] into a set. Return TRUE if and only if there
   is no error.  Note that the address high is also included where as in
   DWARF2 an address range between low & high means [low,high).

   This only handles sets with values. For the simpler case of sets without
   value, it is handled in arange_set_insert().  This function is
   tail-recurive.  It is guaranteed to terminate because it only recurses
   with a smaller range than it is given.  */

static bfd_boolean
arange_set_insert_value (arange_set set,
			 bfd_vma low,
			 bfd_vma high,
			 arange_value_type value)
{
  splay_tree_node succ, pred, node;
  bfd_vma succ_high, succ_low;
  arange_value_type combined, old_value;

  if (low > high)
    {
      arange_set_delete_value (set, value);
      return FALSE;
    }

  pred = splay_tree_predecessor (set->ranges, low);
  if (pred && arange_set_node_high (set, pred) >= low)
    arange_set_split_node (set, pred, low);

  node = splay_tree_lookup (set->ranges, low);
  if (node)
    {
      /* Split node if its arange is larger than inserted arange. */
      if (arange_set_node_high (set, node) > high)
	arange_set_split_node (set, node, high + 1);

      old_value = arange_set_node_value (set, node);
      combined = arange_set_combine_value (set, old_value, value); 
      arange_set_node_set_value (set, node, combined);
      node = arange_set_maybe_merge_with_predecessor (set, node);
      arange_set_delete_value (set, old_value);

      /* Insert remaining arange by tail-recursion.  */
      if (high > arange_set_node_high (set, node))
	return arange_set_insert_value (set,
					arange_set_node_high (set, node) + 1,
					high, value);
      else
	{
	  /* Node must cover exactly the range. */
	  BFD_ASSERT (high == arange_set_node_high (set, node));
	  arange_set_delete_value (set, value);
	  succ = splay_tree_successor (set->ranges, arange_set_node_low (node));
	  if (succ)
	    succ = arange_set_maybe_merge_with_predecessor (set, succ);	
	  return TRUE;
	}
    }
  
  succ = splay_tree_successor (set->ranges, low);
  if (succ)
    {
      succ_low = arange_set_node_low (succ);	
      succ_high = arange_set_node_high (set, succ);

      if (succ_low <= high)
	{
	  node = arange_set_splay_tree_insert (set, low, succ_low - 1, value); 
	  if (!node)
	    return FALSE;

	  /* Update set lower bound only after insertion is successful.  */
	  if (low < set->lower_bound)
	    set->lower_bound = low;

	  node = arange_set_maybe_merge_with_predecessor (set, node);

	  /* Recurse to handle rest of insertion.  Note that we have to copy
	     value here since it has already been used in the node above.  */
	  return arange_set_insert_value (set, succ_low, high,
					  arange_set_copy_value (set, value));
	}
     }
  
  node = arange_set_splay_tree_insert (set, low, high, value);
  if (!node)
    return FALSE;

  /* Update set boundaries only after insertion is successful.  */
  if (low < set->lower_bound)
    set->lower_bound = low;
  if (high > set->upper_bound)
    set->upper_bound = high;

  node = arange_set_maybe_merge_with_predecessor (set, node);

  succ = splay_tree_successor (set->ranges, arange_set_node_low (node));
  if (succ)
    succ = arange_set_maybe_merge_with_predecessor (set, succ);	

  return TRUE;
}

bfd_boolean
arange_set_insert (arange_set set,
		   bfd_vma low,
		   bfd_vma high,
		   arange_value_type value)
{
  splay_tree tree = set->ranges;
  splay_tree_node pred, succ, node = NULL;
  bfd_vma pred_high, node_low;

  if (set->value_p)
    return arange_set_insert_value (set, low, high, value);

  if (low > high)
    return FALSE;

  pred = splay_tree_predecessor (tree, low);
  if (pred)
    {
      pred_high = arange_set_node_high (set, pred);

      /* Nothing to be done if predecessor contains new aranges.  */ 
      if (pred_high >= high)
	return TRUE;

      /* If we can expand predecessor, do so.  Test for the case in which
	 predecessor does not contain new arange but touches it.  */
      if (pred_high >= low || pred_high + 1 == low)
	{
	  node = pred;
	  arange_set_node_set_high (set, node, high);
	}
    }

  /* Try to see if [low,something] is already in splay tree.  */ 
  if (node == NULL)
    {
      node = splay_tree_lookup (tree, low); 	
      if (node)
	{
	  /* Nothing to be done if node contains new aranges.  */ 
	  if (arange_set_node_high (set, node) >= high)
	    return TRUE;

	  arange_set_node_set_high (set, node, high);
	}
    }

  if (node == NULL)
    {
      node = arange_set_splay_tree_insert (set, low, high, 0);
      if (!node)
	return FALSE;
    }

  BFD_ASSERT (node
	      && arange_set_node_low (node) <= low
	      && arange_set_node_high (set, node) >= high);

  /* Update set upper and lower bounds.  */
  if (low < set->lower_bound)
    set->lower_bound = low;
  if (high > set->upper_bound)
    set->upper_bound = high;

  /* Merge successor if it overlaps or touches node.  */
  node_low = arange_set_node_low (node);
  while ((succ = splay_tree_successor (tree, node_low)) != NULL
	 && ((arange_set_node_high (set, node) >= arange_set_node_low (succ))
	     || (arange_set_node_high (set, node) + 1
		 == arange_set_node_low (succ))))
    {
      if (arange_set_node_high (set, succ) > high)
        arange_set_node_set_high (set, node, arange_set_node_high (set, succ));
      splay_tree_remove (tree, arange_set_node_low (succ));
    }
  return TRUE;
}

struct arange_set_foreach_adapter_data
{
  void *data;
  arange_set set;
  arange_set_foreach_fn foreach_fn;
};

/* Adaptor to make arange_set_foreach works with splay_tree_foreach.  */

static int
arange_set_foreach_adapter (splay_tree_node node, void *data)
{
  struct arange_set_foreach_adapter_data *adapter_data;
  arange_set set;

  adapter_data = data;
  set = adapter_data->set;
  return (adapter_data->foreach_fn) (arange_set_node_low (node),
				     arange_set_node_high (set, node),
				     arange_set_node_value (set, node),
				     adapter_data->data);
}

/* Traverse aranges in a set.  For each arange in ascending order of
   low addresses, call foreach_fn with arange boundaries and data.
   If any invocation of foreach_fn returns a non-zero value, stop traversal
   and return that value. Otherwise, return 0.  */

int
arange_set_foreach (arange_set set,
		    arange_set_foreach_fn foreach_fn,
		    void *data)
{
  struct arange_set_foreach_adapter_data adapter_data;

  adapter_data.data = data;
  adapter_data.foreach_fn = foreach_fn;
  adapter_data.set = set;
  return splay_tree_foreach (set->ranges, arange_set_foreach_adapter,
			     (void *) &adapter_data);
}
