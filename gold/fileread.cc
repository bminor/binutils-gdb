// fileread.cc -- read files for gold

// Copyright 2006, 2007 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "gold.h"

#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include "filenames.h"

#include "parameters.h"
#include "options.h"
#include "dirsearch.h"
#include "target.h"
#include "binary.h"
#include "fileread.h"

namespace gold
{

// Class File_read::View.

File_read::View::~View()
{
  gold_assert(!this->is_locked());
  if (!this->mapped_)
    delete[] this->data_;
  else
    {
      if (::munmap(const_cast<unsigned char*>(this->data_), this->size_) != 0)
        gold_warning(_("munmap failed: %s"), strerror(errno));

      File_read::current_mapped_bytes -= this->size_;
    }
}

void
File_read::View::lock()
{
  ++this->lock_count_;
}

void
File_read::View::unlock()
{
  gold_assert(this->lock_count_ > 0);
  --this->lock_count_;
}

bool
File_read::View::is_locked()
{
  return this->lock_count_ > 0;
}

// Class File_read.

// The File_read static variables.
unsigned long long File_read::total_mapped_bytes;
unsigned long long File_read::current_mapped_bytes;
unsigned long long File_read::maximum_mapped_bytes;

// The File_read class is designed to support file descriptor caching,
// but this is not currently implemented.

File_read::~File_read()
{
  gold_assert(this->token_.is_writable());
  if (this->descriptor_ >= 0)
    {
      if (close(this->descriptor_) < 0)
	gold_warning(_("close of %s failed: %s"),
		     this->name_.c_str(), strerror(errno));
      this->descriptor_ = -1;
    }
  this->name_.clear();
  this->clear_views(true);
}

// Open the file.

bool
File_read::open(const Task* task, const std::string& name)
{
  gold_assert(this->token_.is_writable()
	      && this->descriptor_ < 0
	      && this->name_.empty());
  this->name_ = name;

  this->descriptor_ = ::open(this->name_.c_str(), O_RDONLY);

  if (this->descriptor_ >= 0)
    {
      struct stat s;
      if (::fstat(this->descriptor_, &s) < 0)
	gold_error(_("%s: fstat failed: %s"),
		   this->name_.c_str(), strerror(errno));
      this->size_ = s.st_size;
    }

  this->token_.add_writer(task);

  return this->descriptor_ >= 0;
}

// Open the file with the contents in memory.

bool
File_read::open(const Task* task, const std::string& name,
		const unsigned char* contents, off_t size)
{
  gold_assert(this->token_.is_writable()
	      && this->descriptor_ < 0
	      && this->name_.empty());
  this->name_ = name;
  this->contents_ = contents;
  this->size_ = size;
  this->token_.add_writer(task);
  return true;
}

// Release the file.  This is called when we are done with the file in
// a Task.

void
File_read::release()
{
  gold_assert(this->is_locked());

  File_read::total_mapped_bytes += this->mapped_bytes_;
  File_read::current_mapped_bytes += this->mapped_bytes_;
  this->mapped_bytes_ = 0;
  if (File_read::current_mapped_bytes > File_read::maximum_mapped_bytes)
    File_read::maximum_mapped_bytes = File_read::current_mapped_bytes;

  this->clear_views(false);

  this->released_ = true;
}

// Lock the file.

void
File_read::lock(const Task* task)
{
  gold_assert(this->released_);
  this->token_.add_writer(task);
  this->released_ = false;
}

// Unlock the file.

void
File_read::unlock(const Task* task)
{
  this->release();
  this->token_.remove_writer(task);
}

// Return whether the file is locked.

bool
File_read::is_locked() const
{
  if (!this->token_.is_writable())
    return true;
  // The file is not locked, so it should have been released.
  gold_assert(this->released_);
  return false;
}

// See if we have a view which covers the file starting at START for
// SIZE bytes.  Return a pointer to the View if found, NULL if not.

inline File_read::View*
File_read::find_view(off_t start, section_size_type size) const
{
  off_t page = File_read::page_offset(start);

  Views::const_iterator p = this->views_.lower_bound(page);
  if (p == this->views_.end() || p->first > page)
    {
      if (p == this->views_.begin())
	return NULL;
      --p;
    }

  if (p->second->start() + static_cast<off_t>(p->second->size())
      < start + static_cast<off_t>(size))
    return NULL;

  p->second->set_accessed();

  return p->second;
}

// Read SIZE bytes from the file starting at offset START.  Read into
// the buffer at P.

void
File_read::do_read(off_t start, section_size_type size, void* p) const
{
  ssize_t bytes;
  if (this->contents_ != NULL)
    {
      bytes = this->size_ - start;
      if (static_cast<section_size_type>(bytes) >= size)
	{
	  memcpy(p, this->contents_ + start, size);
	  return;
	}
    }
  else
    {
      bytes = ::pread(this->descriptor_, p, size, start);
      if (static_cast<section_size_type>(bytes) == size)
	return;

      if (bytes < 0)
	{
	  gold_fatal(_("%s: pread failed: %s"),
		     this->filename().c_str(), strerror(errno));
	  return;
	}
    }

  gold_fatal(_("%s: file too short: read only %lld of %lld bytes at %lld"),
	     this->filename().c_str(),
	     static_cast<long long>(bytes),
	     static_cast<long long>(size),
	     static_cast<long long>(start));
}

// Read data from the file.

void
File_read::read(off_t start, section_size_type size, void* p) const
{
  const File_read::View* pv = this->find_view(start, size);
  if (pv != NULL)
    {
      memcpy(p, pv->data() + (start - pv->start()), size);
      return;
    }

  this->do_read(start, size, p);
}

// Find an existing view or make a new one.

File_read::View*
File_read::find_or_make_view(off_t start, section_size_type size, bool cache)
{
  gold_assert(!this->token_.is_writable());
  this->released_ = false;

  File_read::View* v = this->find_view(start, size);
  if (v != NULL)
    {
      if (cache)
	v->set_cache();
      return v;
    }

  off_t poff = File_read::page_offset(start);

  File_read::View* const vnull = NULL;
  std::pair<Views::iterator, bool> ins =
    this->views_.insert(std::make_pair(poff, vnull));

  if (!ins.second)
    {
      // There was an existing view at this offset.  It must not be
      // large enough.  We can't delete it here, since something might
      // be using it; put it on a list to be deleted when the file is
      // unlocked.
      v = ins.first->second;
      gold_assert(v->size() - (start - v->start()) < size);
      if (v->should_cache())
	cache = true;
      v->clear_cache();
      this->saved_views_.push_back(v);
    }

  // We need to map data from the file.

  section_size_type psize = File_read::pages(size + (start - poff));

  if (poff + static_cast<off_t>(psize) >= this->size_)
    {
      psize = this->size_ - poff;
      gold_assert(psize >= size);
    }

  if (this->contents_ != NULL)
    {
      unsigned char* p = new unsigned char[psize];
      this->do_read(poff, psize, p);
      v = new File_read::View(poff, psize, p, cache, false);
    }
  else
    {
      void* p = ::mmap(NULL, psize, PROT_READ, MAP_PRIVATE,
                       this->descriptor_, poff);
      if (p == MAP_FAILED)
	gold_fatal(_("%s: mmap offset %lld size %lld failed: %s"),
		   this->filename().c_str(),
		   static_cast<long long>(poff),
		   static_cast<long long>(psize),
		   strerror(errno));

      this->mapped_bytes_ += psize;

      const unsigned char* pbytes = static_cast<const unsigned char*>(p);
      v = new File_read::View(poff, psize, pbytes, cache, true);
    }

  ins.first->second = v;
  return v;
}

// Get a view into the file.

const unsigned char*
File_read::get_view(off_t start, section_size_type size, bool cache)
{
  File_read::View* pv = this->find_or_make_view(start, size, cache);
  return pv->data() + (start - pv->start());
}

File_view*
File_read::get_lasting_view(off_t start, section_size_type size, bool cache)
{
  File_read::View* pv = this->find_or_make_view(start, size, cache);
  pv->lock();
  return new File_view(*this, pv, pv->data() + (start - pv->start()));
}

// Use readv to read COUNT entries from RM starting at START.  BASE
// must be added to all file offsets in RM.

void
File_read::do_readv(off_t base, const Read_multiple& rm, size_t start,
		    size_t count)
{
  unsigned char discard[File_read::page_size];
  iovec iov[File_read::max_readv_entries * 2];
  size_t iov_index = 0;

  off_t first_offset = rm[start].file_offset;
  off_t last_offset = first_offset;
  ssize_t want = 0;
  for (size_t i = 0; i < count; ++i)
    {
      const Read_multiple_entry& i_entry(rm[start + i]);

      if (i_entry.file_offset > last_offset)
	{
	  size_t skip = i_entry.file_offset - last_offset;
	  gold_assert(skip <= sizeof discard);

	  iov[iov_index].iov_base = discard;
	  iov[iov_index].iov_len = skip;
	  ++iov_index;

	  want += skip;
	}

      iov[iov_index].iov_base = i_entry.buffer;
      iov[iov_index].iov_len = i_entry.size;
      ++iov_index;

      want += i_entry.size;

      last_offset = i_entry.file_offset + i_entry.size;
    }

  gold_assert(iov_index < sizeof iov / sizeof iov[0]);

  if (::lseek(this->descriptor_, base + first_offset, SEEK_SET) < 0)
    gold_fatal(_("%s: lseek failed: %s"),
	       this->filename().c_str(), strerror(errno));

  ssize_t got = ::readv(this->descriptor_, iov, iov_index);

  if (got < 0)
    gold_fatal(_("%s: readv failed: %s"),
	       this->filename().c_str(), strerror(errno));
  if (got != want)
    gold_fatal(_("%s: file too short: read only %zd of %zd bytes at %lld"),
	       this->filename().c_str(),
	       got, want, static_cast<long long>(base + first_offset));
}

// Read several pieces of data from the file.

void
File_read::read_multiple(off_t base, const Read_multiple& rm)
{
  size_t count = rm.size();
  size_t i = 0;
  while (i < count)
    {
      // Find up to MAX_READV_ENTRIES consecutive entries which are
      // less than one page apart.
      const Read_multiple_entry& i_entry(rm[i]);
      off_t i_off = i_entry.file_offset;
      off_t end_off = i_off + i_entry.size;
      size_t j;
      for (j = i + 1; j < count; ++j)
	{
	  if (j - i >= File_read::max_readv_entries)
	    break;
	  const Read_multiple_entry& j_entry(rm[j]);
	  off_t j_off = j_entry.file_offset;
	  gold_assert(j_off >= end_off);
	  off_t j_end_off = j_off + j_entry.size;
	  if (j_end_off - end_off >= File_read::page_size)
	    break;
	  end_off = j_end_off;
	}

      if (j == i + 1)
	this->read(base + i_off, i_entry.size, i_entry.buffer);
      else
	{
	  File_read::View* view = this->find_view(base + i_off,
						  end_off - i_off);
	  if (view == NULL)
	    this->do_readv(base, rm, i, j - i);
	  else
	    {
	      const unsigned char* v = (view->data()
					+ (base + i_off - view->start()));
	      for (size_t k = i; k < j; ++k)
		{
		  const Read_multiple_entry& k_entry(rm[k]);
		  gold_assert((convert_to_section_size_type(k_entry.file_offset
                                                           - i_off)
                               + k_entry.size)
			      <= convert_to_section_size_type(end_off
                                                              - i_off));
		  memcpy(k_entry.buffer,
			 v + (k_entry.file_offset - i_off),
			 k_entry.size);
		}
	    }
	}

      i = j;
    }
}

// Mark all views as no longer cached.

void
File_read::clear_view_cache_marks()
{
  // Just ignore this if there are multiple objects associated with
  // the file.  Otherwise we will wind up uncaching and freeing some
  // views for other objects.
  if (this->object_count_ > 1)
    return;

  for (Views::iterator p = this->views_.begin();
       p != this->views_.end();
       ++p)
    p->second->clear_cache();
  for (Saved_views::iterator p = this->saved_views_.begin();
       p != this->saved_views_.end();
       ++p)
    (*p)->clear_cache();
}

// Remove all the file views.  For a file which has multiple
// associated objects (i.e., an archive), we keep accessed views
// around until next time, in the hopes that they will be useful for
// the next object.

void
File_read::clear_views(bool destroying)
{
  Views::iterator p = this->views_.begin();
  while (p != this->views_.end())
    {
      bool should_delete;
      if (p->second->is_locked())
	should_delete = false;
      else if (destroying)
	should_delete = true;
      else if (p->second->should_cache())
	should_delete = false;
      else if (this->object_count_ > 1 && p->second->accessed())
	should_delete = false;
      else
	should_delete = true;

      if (should_delete)
	{
	  delete p->second;

	  // map::erase invalidates only the iterator to the deleted
	  // element.
	  Views::iterator pe = p;
	  ++p;
	  this->views_.erase(pe);
	}
      else
	{
	  gold_assert(!destroying);
	  p->second->clear_accessed();
	  ++p;
	}
    }

  Saved_views::iterator q = this->saved_views_.begin();
  while (q != this->saved_views_.end())
    {
      if (!(*q)->is_locked())
	{
	  delete *q;
	  q = this->saved_views_.erase(q);
	}
      else
	{
	  gold_assert(!destroying);
	  ++q;
	}
    }
}

// Print statistical information to stderr.  This is used for --stats.

void
File_read::print_stats()
{
  fprintf(stderr, _("%s: total bytes mapped for read: %llu\n"),
	  program_name, File_read::total_mapped_bytes);
  fprintf(stderr, _("%s: maximum bytes mapped for read at one time: %llu\n"),
	  program_name, File_read::maximum_mapped_bytes);
}

// Class File_view.

File_view::~File_view()
{
  gold_assert(this->file_.is_locked());
  this->view_->unlock();
}

// Class Input_file.

// Create a file for testing.

Input_file::Input_file(const Task* task, const char* name,
		       const unsigned char* contents, off_t size)
  : file_()
{
  this->input_argument_ =
    new Input_file_argument(name, false, "", false,
			    Position_dependent_options());
  bool ok = file_.open(task, name, contents, size);
  gold_assert(ok);
}

// Return the position dependent options in force for this file.

const Position_dependent_options&
Input_file::options() const
{
  return this->input_argument_->options();
}

// Return the name given by the user.  For -lc this will return "c".

const char*
Input_file::name() const
{
  return this->input_argument_->name();
}

// Return whether we are only reading symbols.

bool
Input_file::just_symbols() const
{
  return this->input_argument_->just_symbols();
}

// Open the file.

// If the filename is not absolute, we assume it is in the current
// directory *except* when:
//    A) input_argument_->is_lib() is true; or
//    B) input_argument_->extra_search_path() is not empty.
// In both cases, we look in extra_search_path + library_path to find
// the file location, rather than the current directory.

bool
Input_file::open(const General_options& options, const Dirsearch& dirpath,
		 const Task* task)
{
  std::string name;

  // Case 1: name is an absolute file, just try to open it
  // Case 2: name is relative but is_lib is false and extra_search_path
  //         is empty
  if (IS_ABSOLUTE_PATH (this->input_argument_->name())
      || (!this->input_argument_->is_lib()
	  && this->input_argument_->extra_search_path() == NULL))
    {
      name = this->input_argument_->name();
      this->found_name_ = name;
    }
  // Case 3: is_lib is true
  else if (this->input_argument_->is_lib())
    {
      // We don't yet support extra_search_path with -l.
      gold_assert(this->input_argument_->extra_search_path() == NULL);
      std::string n1("lib");
      n1 += this->input_argument_->name();
      std::string n2;
      if (options.is_static()
	  || this->input_argument_->options().do_static_search())
	n1 += ".a";
      else
	{
	  n2 = n1 + ".a";
	  n1 += ".so";
	}
      name = dirpath.find(n1, n2, &this->is_in_sysroot_);
      if (name.empty())
	{
	  gold_error(_("cannot find -l%s"),
		     this->input_argument_->name());
	  return false;
	}
      if (n2.empty() || name[name.length() - 1] == 'o')
	this->found_name_ = n1;
      else
	this->found_name_ = n2;
    }
  // Case 4: extra_search_path is not empty
  else
    {
      gold_assert(this->input_argument_->extra_search_path() != NULL);

      // First, check extra_search_path.
      name = this->input_argument_->extra_search_path();
      if (!IS_DIR_SEPARATOR (name[name.length() - 1]))
        name += '/';
      name += this->input_argument_->name();
      struct stat dummy_stat;
      if (::stat(name.c_str(), &dummy_stat) < 0)
        {
          // extra_search_path failed, so check the normal search-path.
          name = dirpath.find(this->input_argument_->name(), "",
			      &this->is_in_sysroot_);
          if (name.empty())
            {
              gold_error(_("cannot find %s"),
			 this->input_argument_->name());
	      return false;
            }
        }
      this->found_name_ = this->input_argument_->name();
    }

  // Now that we've figured out where the file lives, try to open it.

  General_options::Object_format format =
    this->input_argument_->options().input_format();
  bool ok;
  if (format == General_options::OBJECT_FORMAT_ELF)
    ok = this->file_.open(task, name);
  else
    {
      gold_assert(format == General_options::OBJECT_FORMAT_BINARY);
      ok = this->open_binary(task, name);
    }

  if (!ok)
    {
      gold_error(_("cannot open %s: %s"),
		 name.c_str(), strerror(errno));
      return false;
    }

  return true;
}

// Open a file for --format binary.

bool
Input_file::open_binary(const Task* task, const std::string& name)
{
  // In order to open a binary file, we need machine code, size, and
  // endianness.  If we have a target already, use it, otherwise use
  // the defaults.
  elfcpp::EM machine;
  int size;
  bool big_endian;
  if (parameters->is_target_valid())
    {
      Target* target = parameters->target();
      machine = target->machine_code();
      size = target->get_size();
      big_endian = target->is_big_endian();
    }
  else
    {
      machine = elfcpp::GOLD_DEFAULT_MACHINE;
      size = GOLD_DEFAULT_SIZE;
      big_endian = GOLD_DEFAULT_BIG_ENDIAN;
    }

  Binary_to_elf binary_to_elf(machine, size, big_endian, name);
  if (!binary_to_elf.convert(task))
    return false;
  return this->file_.open(task, name, binary_to_elf.converted_data_leak(),
			  binary_to_elf.converted_size());
}

} // End namespace gold.
