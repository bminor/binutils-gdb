/* DIE indexing 

   Copyright (C) 2022-2024 Free Software Foundation, Inc.

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

#ifndef GDB_DWARF2_COOKED_INDEX_H
#define GDB_DWARF2_COOKED_INDEX_H

#include "dwarf2.h"
#include "dwarf2/cooked-index-entry.h"
#include "dwarf2/types.h"
#include "symtab.h"
#include "hashtab.h"
#include "quick-symbol.h"
#include "gdbsupport/gdb_obstack.h"
#include "addrmap.h"
#include "gdbsupport/iterator-range.h"
#include "dwarf2/mapped-index.h"
#include "dwarf2/read.h"
#include "dwarf2/parent-map.h"
#include "gdbsupport/range-chain.h"
#include "gdbsupport/string-set.h"
#include "complaints.h"

#if CXX_STD_THREAD
#include <mutex>
#include <condition_variable>
#endif /* CXX_STD_THREAD */

struct dwarf2_per_cu;
struct dwarf2_per_bfd;
struct index_cache_store_context;
struct cooked_index_entry;

class cooked_index;

/* An index of interesting DIEs.  This is "cooked", in contrast to a
   mapped .debug_names or .gdb_index, which are "raw".  An entry in
   the index is of type cooked_index_entry.

   Operations on the index are described below.  They are chosen to
   make it relatively simple to implement the symtab "quick"
   methods.  */
class cooked_index_shard
{
public:
  cooked_index_shard () = default;
  DISABLE_COPY_AND_ASSIGN (cooked_index_shard);

  /* Create a new cooked_index_entry and register it with this object.
     Entries are owned by this object.  The new item is returned.  */
  cooked_index_entry *add (sect_offset die_offset, enum dwarf_tag tag,
			   cooked_index_flag flags, enum language lang,
			   const char *name,
			   cooked_index_entry_ref parent_entry,
			   dwarf2_per_cu *per_cu);

  /* Install a new fixed addrmap from the given mutable addrmap.  */
  void install_addrmap (addrmap_mutable *map)
  {
    gdb_assert (m_addrmap == nullptr);
    m_addrmap = new (&m_storage) addrmap_fixed (&m_storage, map);
  }

  friend class cooked_index;

  /* A simple range over part of m_entries.  */
  typedef iterator_range<std::vector<cooked_index_entry *>::const_iterator>
       range;

  /* Return a range of all the entries.  */
  range all_entries () const
  {
    return { m_entries.cbegin (), m_entries.cend () };
  }

  /* Look up an entry by name.  Returns a range of all matching
     results.  If COMPLETING is true, then a larger range, suitable
     for completion, will be returned.  */
  range find (const std::string &name, bool completing) const;

private:

  /* Return the entry that is believed to represent the program's
     "main".  This will return NULL if no such entry is available.  */
  const cooked_index_entry *get_main () const
  {
    return m_main;
  }

  /* Look up ADDR in the address map, and return either the
     corresponding CU, or nullptr if the address could not be
     found.  */
  dwarf2_per_cu *lookup (unrelocated_addr addr)
  {
    if (m_addrmap == nullptr)
      return nullptr;

    return (static_cast<dwarf2_per_cu *> (m_addrmap->find ((CORE_ADDR) addr)));
  }

  /* Create a new cooked_index_entry and register it with this object.
     Entries are owned by this object.  The new item is returned.  */
  cooked_index_entry *create (sect_offset die_offset,
			      enum dwarf_tag tag,
			      cooked_index_flag flags,
			      enum language lang,
			      const char *name,
			      cooked_index_entry_ref parent_entry,
			      dwarf2_per_cu *per_cu);

  /* When GNAT emits mangled ("encoded") names in the DWARF, and does
     not emit the module structure, we still need this structuring to
     do lookups.  This function recreates that information for an
     existing entry, modifying ENTRY as appropriate.  Any new entries
     are added to NEW_ENTRIES.  */
  void handle_gnat_encoded_entry
       (cooked_index_entry *entry, htab_t gnat_entries,
	std::vector<cooked_index_entry *> &new_entries);

  /* Finalize the index.  This should be called a single time, when
     the index has been fully populated.  It enters all the entries
     into the internal table and fixes up all missing parent links.
     This may be invoked in a worker thread.  */
  void finalize (const parent_map_map *parent_maps);

  /* Storage for the entries.  */
  auto_obstack m_storage;
  /* List of all entries.  */
  std::vector<cooked_index_entry *> m_entries;
  /* If we found an entry with 'is_main' set, store it here.  */
  cooked_index_entry *m_main = nullptr;
  /* The addrmap.  This maps address ranges to dwarf2_per_cu objects.  */
  addrmap_fixed *m_addrmap = nullptr;
  /* Storage for canonical names.  */
  gdb::string_set m_names;
};

using cooked_index_shard_up = std::unique_ptr<cooked_index_shard>;

/* The possible states of the index.  See the explanatory comment
   before cooked_index for more details.  */
enum class cooked_state
{
  /* The default state.  This is not a valid argument to 'wait'.  */
  INITIAL,
  /* The initial scan has completed.  The name of "main" is now
     available (if known).  The addrmaps are usable now.
     Finalization has started but is not complete.  */
  MAIN_AVAILABLE,
  /* Finalization has completed.  This means the index is fully
     available for queries.  */
  FINALIZED,
  /* Writing to the index cache has finished.  */
  CACHE_DONE,
};

/* An object of this type controls the scanning of the DWARF.  It
   schedules the worker tasks and tracks the current state.  Once
   scanning is done, this object is discarded.
   
   This is an abstract base class that defines the basic behavior of
   scanners.  Separate concrete implementations exist for scanning
   .debug_names and .debug_info.  */

class cooked_index_worker
{
public:

  explicit cooked_index_worker (dwarf2_per_objfile *per_objfile)
    : m_per_objfile (per_objfile),
      m_cache_store (global_index_cache, per_objfile->per_bfd)
  { }
  virtual ~cooked_index_worker ()
  { }
  DISABLE_COPY_AND_ASSIGN (cooked_index_worker);

  /* Start reading.  */
  void start ();

  /* Wait for a particular state to be achieved.  If ALLOW_QUIT is
     true, then the loop will check the QUIT flag.  Normally this
     method may only be called from the main thread; however, it can
     be called from a worker thread provided that the desired state
     has already been attained.  (This oddity is used by the index
     cache writer.)  */
  bool wait (cooked_state desired_state, bool allow_quit);

protected:

  /* Let cooked_index call the 'set' and 'write_to_cache' methods.  */
  friend class cooked_index;

  /* Set the current state.  */
  void set (cooked_state desired_state);

  /* Write to the index cache.  */
  void write_to_cache (const cooked_index *idx,
		       deferred_warnings *warn) const;

  /* Helper function that does the work of reading.  This must be able
     to be run in a worker thread without problems.  */
  virtual void do_reading () = 0;

  /* A callback that can print stats, if needed.  This is called when
     transitioning to the 'MAIN_AVAILABLE' state.  */
  virtual void print_stats ()
  { }

  /* Each thread returns a tuple holding a cooked index, any collected
     complaints, a vector of errors that should be printed, and a
     parent map.

     The errors are retained because GDB's I/O system is not
     thread-safe.  run_on_main_thread could be used, but that would
     mean the messages are printed after the prompt, which looks
     weird.  */
  using result_type = std::tuple<cooked_index_shard_up,
				 complaint_collection,
				 std::vector<gdb_exception>,
				 parent_map>;

  /* The per-objfile object.  */
  dwarf2_per_objfile *m_per_objfile;
  /* Result of each worker task.  */
  std::vector<result_type> m_results;
  /* Any warnings emitted.  This is not in 'result_type' because (for
     the time being at least), it's only needed in do_reading, not in
     every worker.  Note that deferred_warnings uses gdb_stderr in its
     constructor, and this should only be done from the main thread.
     This is enforced in the cooked_index_worker constructor.  */
  deferred_warnings m_warnings;

  /* A map of all parent maps.  Used during finalization to fix up
     parent relationships.  */
  parent_map_map m_all_parents_map;

#if CXX_STD_THREAD
  /* Current state of this object.  */
  cooked_state m_state = cooked_state::INITIAL;
  /* Mutex and condition variable used to synchronize.  */
  std::mutex m_mutex;
  std::condition_variable m_cond;
#endif /* CXX_STD_THREAD */
  /* This flag indicates whether any complaints or exceptions that
     arose during scanning have been reported by 'wait'.  This may
     only be modified on the main thread.  */
  bool m_reported = false;
  /* If set, an exception occurred during reading; in this case the
     scanning is stopped and this exception will later be reported by
     the 'wait' method.  */
  std::optional<gdb_exception> m_failed;
  /* An object used to write to the index cache.  */
  index_cache_store_context m_cache_store;
};

using cooked_index_worker_up = std::unique_ptr<cooked_index_worker>;

/* The main index of DIEs.

   The index is created by multiple threads.  The overall process is
   somewhat complicated, so here's a diagram to help sort it out.

   The basic idea behind this design is (1) to do as much work as
   possible in worker threads, and (2) to start the work as early as
   possible.  This combination should help hide the effort from the
   user to the maximum possible degree.

   . Main Thread                |       Worker Threads
   ============================================================
   . dwarf2_initialize_objfile
   . 	      |
   .          v
   .     cooked index ------------> cooked_index_worker::start
   .          |                           / | \
   .          v                          /  |  \
   .       install                      /   |	\
   .  cooked_index_functions        scan CUs in workers
   .          |               create cooked_index_shard objects
   .          |                           \ | /
   .          v                            \|/
   .    return to caller                    v
   .                                 initial scan is done
   .                                state = MAIN_AVAILABLE
   .                              "main" name now available
   .                                        |
   .                                        |
   .   if main thread calls...              v
   .   compute_main_name         cooked_index::set_contents
   .          |                           / | \
   .          v                          /  |  \
   .   wait (MAIN_AVAILABLE)          finalization
   .          |                          \  |  /
   .          v                           \ | /        
   .        done                      state = FINALIZED
   .                                        |
   .                                        v
   .                              maybe write to index cache
   .                                  state = CACHE_DONE
   .
   .
   .   if main thread calls...
   .   any other "quick" API
   .          |
   .          v
   .   wait (FINALIZED)
   .          |
   .          v
   .    use the index
*/

class cooked_index : public dwarf_scanner_base
{
public:
  cooked_index (cooked_index_worker_up &&worker);
  ~cooked_index () override;

  DISABLE_COPY_AND_ASSIGN (cooked_index);

  /* Start reading the DWARF.  */
  void start_reading () override;

  /* Called by cooked_index_worker to set the contents of this index
     and transition to the MAIN_AVAILABLE state.  WARN is used to
     collect any warnings that may arise when writing to the cache.
     PARENT_MAPS is used when resolving pending parent links.
     PARENT_MAPS may be NULL if there are no IS_PARENT_DEFERRED
     entries in VEC.  */
  void set_contents (std::vector<cooked_index_shard_up> &&vec,
		     deferred_warnings *warn,
		     const parent_map_map *parent_maps);

  /* A range over a vector of subranges.  */
  using range = range_chain<cooked_index_shard::range>;

  /* Look up an entry by name.  Returns a range of all matching
     results.  If COMPLETING is true, then a larger range, suitable
     for completion, will be returned.  */
  range find (const std::string &name, bool completing);

  /* Return a range of all the entries.  */
  range all_entries ()
  {
    wait (cooked_state::FINALIZED, true);
    std::vector<cooked_index_shard::range> result_range;
    result_range.reserve (m_shards.size ());
    for (auto &shard : m_shards)
      result_range.push_back (shard->all_entries ());
    return range (std::move (result_range));
  }

  /* Look up ADDR in the address map, and return either the
     corresponding CU, or nullptr if the address could not be
     found.  */
  dwarf2_per_cu *lookup (unrelocated_addr addr) override;

  /* Return a new vector of all the addrmaps used by all the indexes
     held by this object.

     Elements of the vector may be nullptr.  */
  std::vector<const addrmap *> get_addrmaps ();

  /* Return the entry that is believed to represent the program's
     "main".  This will return NULL if no such entry is available.  */
  const cooked_index_entry *get_main () const;

  const char *get_main_name (struct obstack *obstack, enum language *lang)
    const;

  cooked_index *index_for_writing () override
  {
    wait (cooked_state::FINALIZED, true);
    return this;
  }

  quick_symbol_functions_up make_quick_functions () const override;

  /* Dump a human-readable form of the contents of the index.  */
  void dump (gdbarch *arch);

  /* Wait until this object reaches the desired state.  Note that
     DESIRED_STATE may not be INITIAL -- it does not make sense to
     wait for this.  If ALLOW_QUIT is true, timed waits will be done
     and the quit flag will be checked in a loop.  This may normally
     only be called from the main thread; however, it is ok to call
     from a worker as long as the desired state has already been
     attained.  (This property is needed by the index cache
     writer.)  */
  void wait (cooked_state desired_state, bool allow_quit = false);

  void wait_completely () override
  { wait (cooked_state::CACHE_DONE); }

private:

  /* The vector of cooked_index objects.  This is stored because the
     entries are stored on the obstacks in those objects.  */
  std::vector<cooked_index_shard_up> m_shards;

  /* This tracks the current state.  When this is nullptr, it means
     that the state is CACHE_DONE -- it's important to note that only
     the main thread may change the value of this pointer.  */
  cooked_index_worker_up m_state;
};

/* An implementation of quick_symbol_functions for the cooked DWARF
   index.  */

struct cooked_index_functions : public dwarf2_base_index_functions
{
  cooked_index *wait (struct objfile *objfile, bool allow_quit)
  {
    dwarf2_per_objfile *per_objfile = get_dwarf2_per_objfile (objfile);
    cooked_index *table
      = (gdb::checked_static_cast<cooked_index *>
	 (per_objfile->per_bfd->index_table.get ()));
    table->wait (cooked_state::MAIN_AVAILABLE, allow_quit);
    return table;
  }

  struct compunit_symtab *find_compunit_symtab_by_address
    (struct objfile *objfile, CORE_ADDR address) override;

  bool has_unexpanded_symtabs (struct objfile *objfile) override
  {
    wait (objfile, true);
    return dwarf2_base_index_functions::has_unexpanded_symtabs (objfile);
  }

  struct symtab *find_last_source_symtab (struct objfile *objfile) override
  {
    wait (objfile, true);
    return dwarf2_base_index_functions::find_last_source_symtab (objfile);
  }

  void forget_cached_source_info (struct objfile *objfile) override
  {
    wait (objfile, true);
    dwarf2_base_index_functions::forget_cached_source_info (objfile);
  }

  void print_stats (struct objfile *objfile, bool print_bcache) override
  {
    wait (objfile, true);
    dwarf2_base_index_functions::print_stats (objfile, print_bcache);
  }

  void dump (struct objfile *objfile) override
  {
    cooked_index *index = wait (objfile, true);
    gdb_printf ("Cooked index in use:\n");
    gdb_printf ("\n");
    index->dump (objfile->arch ());
  }

  void expand_all_symtabs (struct objfile *objfile) override
  {
    wait (objfile, true);
    dwarf2_base_index_functions::expand_all_symtabs (objfile);
  }

  bool expand_symtabs_matching
    (struct objfile *objfile,
     expand_symtabs_file_matcher file_matcher,
     const lookup_name_info *lookup_name,
     expand_symtabs_symbol_matcher symbol_matcher,
     expand_symtabs_expansion_listener expansion_notify,
     block_search_flags search_flags,
     domain_search_flags domain,
     expand_symtabs_lang_matcher lang_matcher) override;

  struct compunit_symtab *find_pc_sect_compunit_symtab
    (struct objfile *objfile, bound_minimal_symbol msymbol,
     CORE_ADDR pc, struct obj_section *section, int warn_if_readin) override
  {
    wait (objfile, true);
    return (dwarf2_base_index_functions::find_pc_sect_compunit_symtab
	    (objfile, msymbol, pc, section, warn_if_readin));
  }

  void map_symbol_filenames (objfile *objfile, symbol_filename_listener fun,
			     bool need_fullname) override
  {
    wait (objfile, true);
    return (dwarf2_base_index_functions::map_symbol_filenames
	    (objfile, fun, need_fullname));
  }

  void compute_main_name (struct objfile *objfile) override
  {
    wait (objfile, false);
  }
};

#endif /* GDB_DWARF2_COOKED_INDEX_H */
