/* memattr.h */
#ifndef MEMATTR_H
#define MEMATTR_H

enum mem_access_mode
{
  MEM_RW,			/* read/write */
  MEM_RO,			/* read only */
  MEM_WO			/* write only */
};

enum mem_access_width
{
  MEM_WIDTH_UNSPECIFIED,
  MEM_WIDTH_8,			/*  8 bit accesses */
  MEM_WIDTH_16,			/* 16  "      "    */
  MEM_WIDTH_32,			/* 32  "      "    */
  MEM_WIDTH_64			/* 64  "      "    */
};

/* The set of all attributes that can be set for a memory region.
  
   This structure was created so that memory attributes can be passed
   to target_ functions without exposing the details of memory region
   list, which would be necessary if these fields were simply added to
   the mem_region structure.

   FIXME: It would be useful if there was a mechanism for targets to
   add their own attributes.  For example, the number of wait states. */
 
struct mem_attrib 
{
  /* read/write, read-only, or write-only */
  enum mem_access_mode mode;

  enum mem_access_width width;

  /* enables hardware breakpoints */
  int hwbreak;
  
  /* enables host-side caching of memory region data */
  int cache;
  
  /* enables memory verification.  after a write, memory is re-read
     to verify that the write was successful. */
  int verify; 
};

struct mem_region 
{
  /* FIXME: memory regions are stored in an unsorted singly-linked
     list.  This probably won't scale to handle hundreds of memory
     regions --- that many could be needed to describe the allowed
     access modes for memory mapped i/o device registers. */
  struct mem_region *next;
  
  CORE_ADDR lo;
  CORE_ADDR hi;

  /* Item number of this memory region. */
  int number;

  /* Status of this memory region (enabled or disabled) */
  int status;

  /* Attributes for this region */
  struct mem_attrib attrib;
};

extern struct mem_region *lookup_mem_region(CORE_ADDR);

#endif	/* MEMATTR_H */
