/* Macros for general registry objects.

   Copyright (C) 2011, 2012
   Free Software Foundation, Inc.

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

#ifndef REGISTRY_H
#define REGISTRY_H

/* The macros here implement a template type and functions for
   associating some user data with a container object.

   A registry is associated with a struct tag name.  To attach a
   registry to a structure, use DEFINE_REGISTRY.  This takes the
   structure tag and an access method as arguments.  In the usual
   case, where the registry fields appear directly in the struct, you
   can use the 'REGISTRY_FIELDS' macro to declare the fields in the
   struct definition, and you can pass 'REGISTRY_ACCESS_FIELD' as the
   access argument to DEFINE_REGISTRY.  In other cases, use
   REGISTRY_FIELDS to define the fields in the appropriate spot, and
   then define your own accessor to find the registry field structure
   given an instance of your type.

   The API user requests a key from a registry during gdb
   initialization.  Later this key can be used to associate some
   module-specific data with a specific container object.

   The exported API is best used via the wrapper macros:
   
   - register_TAG_data(TAG)
   Get a new key for the container type TAG.
   
   - register_TAG_data_with_cleanup(TAG, SAVE, FREE)
   Get a new key for the container type TAG.
   SAVE and FREE are defined as void (*) (struct TAG *, void *)
   When the container is destroyed, first all registered SAVE
   functions are called.
   Then all FREE functions are called.
   Either or both may be NULL.
   
   - clear_TAG_data(TAG, OBJECT)
   Clear all the data associated with OBJECT.  Should be called by the
   container implementation when a container object is destroyed.
   
   - set_TAG_data(TAG, OBJECT, KEY, DATA)
   Set the data on an object.
   
   - TAG_data(TAG, OBJECT, KEY)
   Fetch the data for an object; returns NULL if it has not been set.
*/

/* This structure is used in a container to hold the data that the
   registry uses.  */

struct registry_fields
{
  void **data;
  unsigned num_data;
};

/* This macro is used in a container struct definition to define the
   fields used by the registry code.  */

#define REGISTRY_FIELDS				\
  struct registry_fields registry_data

/* A convenience macro for the typical case where the registry data is
   kept as fields of the object.  This can be passed as the ACCESS
   method to DEFINE_REGISTRY.  */

#define REGISTRY_ACCESS_FIELD(CONTAINER) \
  (CONTAINER)

/* Define a new registry implementation.  */

#define DEFINE_REGISTRY(TAG, ACCESS)					\
struct TAG ## _data							\
{									\
  unsigned index;							\
  void (*save) (struct TAG *, void *);					\
  void (*free) (struct TAG *, void *);					\
};									\
									\
struct TAG ## _data_registration					\
{									\
  struct TAG ## _data *data;						\
  struct TAG ## _data_registration *next;				\
};									\
									\
struct TAG ## _data_registry						\
{									\
  struct TAG ## _data_registration *registrations;			\
  unsigned num_registrations;						\
};									\
									\
struct TAG ## _data_registry TAG ## _data_registry = { NULL, 0 };	\
									\
const struct TAG ## _data *						\
register_ ## TAG ## _data_with_cleanup (void (*save) (struct TAG *, void *), \
				    void (*free) (struct TAG *, void *)) \
{									\
  struct TAG ## _data_registration **curr;				\
									\
  /* Append new registration.  */					\
  for (curr = &TAG ## _data_registry.registrations;			\
       *curr != NULL; curr = &(*curr)->next);				\
									\
  *curr = XMALLOC (struct TAG ## _data_registration);			\
  (*curr)->next = NULL;							\
  (*curr)->data = XMALLOC (struct TAG ## _data);			\
  (*curr)->data->index = TAG ## _data_registry.num_registrations++;	\
  (*curr)->data->save = save;						\
  (*curr)->data->free = free;						\
									\
  return (*curr)->data;							\
}									\
									\
const struct TAG ## _data *						\
register_ ## TAG ## _data (void)					\
{									\
  return register_ ## TAG ## _data_with_cleanup (NULL, NULL);		\
}									\
									\
static void								\
TAG ## _alloc_data (struct TAG *container)				\
{									\
  struct registry_fields *rdata = &ACCESS (container)->registry_data;	\
  gdb_assert (rdata->data == NULL);					\
  rdata->num_data = TAG ## _data_registry.num_registrations;		\
  rdata->data = XCALLOC (rdata->num_data, void *);			\
}									\
									\
void									\
clear_ ## TAG ## _data (struct TAG *container)				\
{									\
  struct registry_fields *rdata = &ACCESS (container)->registry_data;	\
  struct TAG ## _data_registration *registration;			\
  int i;								\
									\
  gdb_assert (rdata->data != NULL);					\
									\
  /* Process all the save handlers.  */					\
									\
  for (registration = TAG ## _data_registry.registrations, i = 0;	\
       i < rdata->num_data;						\
       registration = registration->next, i++)				\
    if (rdata->data[i] != NULL && registration->data->save != NULL)	\
      registration->data->save (container, rdata->data[i]);		\
									\
  /* Now process all the free handlers.  */				\
									\
  for (registration = TAG ## _data_registry.registrations, i = 0;	\
       i < rdata->num_data;						\
       registration = registration->next, i++)				\
    if (rdata->data[i] != NULL && registration->data->free != NULL)	\
      registration->data->free (container, rdata->data[i]);		\
									\
  memset (rdata->data, 0, rdata->num_data * sizeof (void *));		\
}									\
									\
static void								\
TAG ## _free_data (struct TAG *container)				\
{									\
  struct registry_fields *rdata = &ACCESS (container)->registry_data;	\
  gdb_assert (rdata->data != NULL);					\
  clear_ ## TAG ## _data (container);					\
  xfree (rdata->data);							\
  rdata->data = NULL;							\
}									\
									\
void									\
set_ ## TAG ## _data (struct TAG *container, const struct TAG ## _data *data, \
		  void *value)						\
{									\
  struct registry_fields *rdata = &ACCESS (container)->registry_data;	\
  gdb_assert (data->index < rdata->num_data);				\
  rdata->data[data->index] = value;					\
}									\
									\
void *									\
TAG ## _data (struct TAG *container, const struct TAG ## _data *data)	\
{									\
  struct registry_fields *rdata = &ACCESS (container)->registry_data;	\
  gdb_assert (data->index < rdata->num_data);				\
  return rdata->data[data->index];					\
}


/* External declarations for the registry functions.  */

#define DECLARE_REGISTRY(TAG)						\
extern const struct TAG ## _data *register_ ## TAG ## _data (void);	\
extern const struct TAG ## _data *register_ ## TAG ## _data_with_cleanup \
 (void (*save) (struct TAG *, void *), void (*free) (struct TAG *, void *)); \
extern void clear_ ## TAG ## _data (struct TAG *);		\
extern void set_ ## TAG ## _data (struct TAG *,			\
			       const struct TAG ## _data *data, void *value); \
extern void *TAG ## _data (struct TAG *,			\
			   const struct TAG ## _data *data);

#endif /* REGISTRY_H */
