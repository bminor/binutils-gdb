/* libbfd.h -- Declarations used by bfd library *implementation*.
   (This include file is not for users of the library.)
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
   Written by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Use builtin alloca for gcc.  */
#ifdef __GNUC__
#ifndef alloca
#define alloca __builtin_alloca
#endif
#endif

/* Align an address upward to a boundary, expressed as a number of bytes.
   E.g. align to an 8-byte boundary with argument of 8.  */
#define BFD_ALIGN(this, boundary) \
  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))

/* If you want to read and write large blocks, you might want to do it
   in quanta of this amount */
#define DEFAULT_BUFFERSIZE 8192

/* Set a tdata field.  Can't use the other macros for this, since they
   do casts, and casting to the left of assignment isn't portable.  */
#define set_tdata(bfd, v) ((bfd)->tdata.any = (PTR) (v))

/* tdata for an archive.  For an input archive, cache
   needs to be free()'d.  For an output archive, symdefs do.  */

struct artdata {
  file_ptr first_file_filepos;
  /* Speed up searching the armap */
  struct ar_cache *cache;
  bfd *archive_head;            /* Only interesting in output routines */
  carsym *symdefs;		/* the symdef entries */
  symindex symdef_count;             /* how many there are */
  char *extended_names;		/* clever intel extension */
  /* when more compilers are standard C, this can be a time_t */
  long  armap_timestamp;	/* Timestamp value written into armap.
				   This is used for BSD archives to check
				   that the timestamp is recent enough
				   for the BSD linker to not complain,
				   just before we finish writing an
				   archive.  */
  file_ptr armap_datepos;	/* Position within archive to seek to
				   rewrite the date field.  */
  PTR tdata;			/* Backend specific information.  */
};

#define bfd_ardata(bfd) ((bfd)->tdata.aout_ar_data)

/* Goes in bfd's arelt_data slot */
struct areltdata {
  char * arch_header;			     /* it's actually a string */
  unsigned int parsed_size;     /* octets of filesize not including ar_hdr */
  char *filename;			     /* null-terminated */
};

#define arelt_size(bfd) (((struct areltdata *)((bfd)->arelt_data))->parsed_size)

/* There is major inconsistency in how running out of memory is handled.
   Some routines return a NULL, and set bfd_error to no_memory.
   However, obstack routines can't do this ... */

char *bfd_zmalloc PARAMS ((bfd_size_type size));
/* From libiberty.  */
extern PTR xmalloc PARAMS ((size_t));
/* SIZE is bfd_size_type.  */
#define bfd_xmalloc(size) xmalloc ((size_t) size)
/* SIZE is size_t.  */
#define bfd_xmalloc_by_size_t(size) xmalloc (size)

/* These routines allocate and free things on the BFD's obstack.  Note
   that realloc can never occur in place.  */

PTR	bfd_alloc PARAMS ((bfd *abfd, size_t size));
PTR	bfd_zalloc PARAMS ((bfd *abfd, size_t size));
PTR	bfd_realloc PARAMS ((bfd *abfd, PTR orig, size_t new));
void	bfd_alloc_grow PARAMS ((bfd *abfd, PTR thing, size_t size));
PTR	bfd_alloc_finish PARAMS ((bfd *abfd));
PTR	bfd_alloc_by_size_t PARAMS ((bfd *abfd, size_t wanted));

#define	bfd_release(x,y) (void) obstack_free(&(x->memory),y)


bfd_size_type	bfd_read  PARAMS ((PTR ptr, bfd_size_type size,
				   bfd_size_type nitems, bfd *abfd));
bfd_size_type	bfd_write PARAMS ((CONST PTR ptr, bfd_size_type size,
				   bfd_size_type nitems, bfd *abfd));
int		bfd_seek  PARAMS ((bfd* CONST abfd, CONST file_ptr fp,
				   CONST int direction));
long		bfd_tell  PARAMS ((bfd *abfd));

int		bfd_flush PARAMS ((bfd *abfd));
int		bfd_stat  PARAMS ((bfd *abfd, struct stat *));

bfd *	_bfd_create_empty_archive_element_shell PARAMS ((bfd *obfd));
bfd *	_bfd_look_for_bfd_in_cache PARAMS ((bfd *arch_bfd, file_ptr index));
boolean _bfd_add_bfd_to_archive_cache PARAMS ((bfd *, file_ptr, bfd *));
boolean	_bfd_generic_mkarchive PARAMS ((bfd *abfd));
struct areltdata *	_bfd_snarf_ar_hdr PARAMS ((bfd *abfd));
bfd_target *		bfd_generic_archive_p PARAMS ((bfd *abfd));
boolean	bfd_slurp_armap PARAMS ((bfd *abfd));
boolean bfd_slurp_bsd_armap_f2 PARAMS ((bfd *abfd));
#define bfd_slurp_bsd_armap bfd_slurp_armap
#define bfd_slurp_coff_armap bfd_slurp_armap
boolean	_bfd_slurp_extended_name_table PARAMS ((bfd *abfd));
boolean	_bfd_write_archive_contents PARAMS ((bfd *abfd));
bfd *_bfd_get_elt_at_filepos PARAMS ((bfd *archive, file_ptr filepos));
bfd * _bfd_new_bfd PARAMS ((void));

#define DEFAULT_STRING_SPACE_SIZE 0x2000
boolean	bfd_add_to_string_table PARAMS ((char **table, char *new_string,
					 unsigned int *table_length,
					 char **free_ptr));

boolean	bfd_false PARAMS ((bfd *ignore));
boolean	bfd_true PARAMS ((bfd *ignore));
PTR	bfd_nullvoidptr PARAMS ((bfd *ignore));
int	bfd_0 PARAMS ((bfd *ignore));
unsigned int	bfd_0u PARAMS ((bfd *ignore));
void	bfd_void PARAMS ((bfd *ignore));

bfd *	_bfd_new_bfd_contained_in PARAMS ((bfd *));
boolean	 _bfd_dummy_new_section_hook PARAMS ((bfd *ignore, asection *newsect));
char *	 _bfd_dummy_core_file_failing_command PARAMS ((bfd *abfd));
int	 _bfd_dummy_core_file_failing_signal PARAMS ((bfd *abfd));
boolean	 _bfd_dummy_core_file_matches_executable_p PARAMS ((bfd *core_bfd,
							    bfd *exec_bfd));
bfd_target *	_bfd_dummy_target PARAMS ((bfd *abfd));

void	bfd_dont_truncate_arname PARAMS ((bfd *abfd, CONST char *filename,
					char *hdr));
void	bfd_bsd_truncate_arname PARAMS ((bfd *abfd, CONST char *filename,
					char *hdr));
void	bfd_gnu_truncate_arname PARAMS ((bfd *abfd, CONST char *filename,
					char *hdr));

boolean	bsd_write_armap PARAMS ((bfd *arch, unsigned int elength,
				  struct orl *map, unsigned int orl_count, int stridx));

boolean	coff_write_armap PARAMS ((bfd *arch, unsigned int elength,
				   struct orl *map, unsigned int orl_count, int stridx));

bfd *	bfd_generic_openr_next_archived_file PARAMS ((bfd *archive,
						     bfd *last_file));

int	bfd_generic_stat_arch_elt PARAMS ((bfd *, struct stat *));

boolean	bfd_generic_get_section_contents PARAMS ((bfd *abfd, sec_ptr section,
 						  PTR location, file_ptr offset,
						  bfd_size_type count));

boolean	bfd_generic_set_section_contents PARAMS ((bfd *abfd, sec_ptr section,
						  PTR location, file_ptr offset,
						  bfd_size_type count));

/* A routine to create entries for a bfd_link_hash_table.  */
extern struct bfd_hash_entry *_bfd_link_hash_newfunc
  PARAMS ((struct bfd_hash_entry *entry,
	   struct bfd_hash_table *table,
	   const char *string));

/* Initialize a bfd_link_hash_table.  */
extern boolean _bfd_link_hash_table_init
  PARAMS ((struct bfd_link_hash_table *, bfd *,
	   struct bfd_hash_entry *(*) (struct bfd_hash_entry *,
				       struct bfd_hash_table *,
				       const char *)));

/* Generic link hash table creation routine.  */
extern struct bfd_link_hash_table *_bfd_generic_link_hash_table_create
  PARAMS ((bfd *));

/* Generic add symbol routine.  */
extern boolean _bfd_generic_link_add_symbols
  PARAMS ((bfd *, struct bfd_link_info *));

/* Generic archive add symbol routine.  */
extern boolean _bfd_generic_link_add_archive_symbols
  PARAMS ((bfd *, struct bfd_link_info *,
	   boolean (*checkfn) (bfd *, struct bfd_link_info *, boolean *)));

/* Forward declaration to avoid prototype errors.  */
typedef struct bfd_link_hash_entry _bfd_link_hash_entry;

/* Generic routine to add a single symbol.  */
extern boolean _bfd_generic_link_add_one_symbol
  PARAMS ((struct bfd_link_info *, bfd *, const char *name, flagword,
	   asection *, bfd_vma, const char *, boolean copy,
	   boolean constructor, unsigned int bitsize,
	   struct bfd_link_hash_entry **));

/* Generic link routine.  */
extern boolean _bfd_generic_final_link
  PARAMS ((bfd *, struct bfd_link_info *));

/* Default link order processing routine.  */
extern boolean _bfd_default_link_order
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   struct bfd_link_order *));

/* Final link relocation routine.  */
extern bfd_reloc_status_type _bfd_final_link_relocate
  PARAMS ((const reloc_howto_type *, bfd *, asection *, bfd_byte *,
	   bfd_vma address, bfd_vma value, bfd_vma addend));

/* Relocate a particular location by a howto and a value.  */
extern bfd_reloc_status_type _bfd_relocate_contents
  PARAMS ((const reloc_howto_type *, bfd *, bfd_vma, bfd_byte *));

/* Macros to tell if bfds are read or write enabled.

   Note that bfds open for read may be scribbled into if the fd passed
   to bfd_fdopenr is actually open both for read and write
   simultaneously.  However an output bfd will never be open for
   read.  Therefore sometimes you want to check bfd_read_p or
   !bfd_read_p, and only sometimes bfd_write_p.
*/

#define	bfd_read_p(abfd) ((abfd)->direction == read_direction || (abfd)->direction == both_direction)
#define	bfd_write_p(abfd) ((abfd)->direction == write_direction || (abfd)->direction == both_direction)

void	bfd_assert PARAMS ((char*,int));

#define BFD_ASSERT(x) \
{ if (!(x)) bfd_assert(__FILE__,__LINE__); }

#define BFD_FAIL() \
{ bfd_assert(__FILE__,__LINE__); }

FILE *	bfd_cache_lookup_worker PARAMS ((bfd *));

extern bfd *bfd_last_cache;
    
/* Now Steve, what's the story here? */
#ifdef lint
#define itos(x) "l"
#define stoi(x) 1
#else
#define itos(x) ((char*)(x))
#define stoi(x) ((int)(x))
#endif

/* Generic routine for close_and_cleanup is really just bfd_true.  */
#define	bfd_generic_close_and_cleanup	bfd_true

/* List of supported target vectors, and the default vector (if
   bfd_default_vector[0] is NULL, there is no default).  */
extern bfd_target *bfd_target_vector[];
extern bfd_target *bfd_default_vector[];

/* And more follows */

