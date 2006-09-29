// fileread.cc -- read files for gold

#include "gold.h"

#include <cassert>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#include "options.h"
#include "dirsearch.h"
#include "fileread.h"

namespace gold
{

// Class File_read::View.

File_read::View::~View()
{
  assert(!this->is_locked());
  delete[] this->data_;
}

void
File_read::View::lock()
{
  ++this->lock_count_;
}

void
File_read::View::unlock()
{
  assert(this->lock_count_ > 0);
  --this->lock_count_;
}

bool
File_read::View::is_locked()
{
  return this->lock_count_ > 0;
}

// Class File_read.

// The File_read class is designed to support file descriptor caching,
// but this is not currently implemented.

File_read::~File_read()
{
  assert(this->lock_count_ == 0);
  if (this->descriptor_ >= 0)
    {
      if (close(this->descriptor_) < 0)
	fprintf(stderr, _("%s: warning: close(%s) failed: %s"),
		program_name, this->name_.c_str(), strerror(errno));
      this->descriptor_ = -1;
    }
  this->name_.clear();
  this->clear_views(true);
}

bool
File_read::open(const std::string& name)
{
  assert(this->lock_count_ == 0
	 && this->descriptor_ < 0
	 && this->name_.empty());
  this->name_ = name;
  this->descriptor_ = ::open(this->name_.c_str(), O_RDONLY);
  ++this->lock_count_;
  return this->descriptor_ >= 0;
}

int
File_read::get_descriptor()
{
  assert(this->lock_count_ > 0);
  return this->descriptor_;
}

void
File_read::lock()
{
  ++this->lock_count_;
}

void
File_read::unlock()
{
  assert(this->lock_count_ > 0);
  --this->lock_count_;
}

bool
File_read::is_locked()
{
  return this->lock_count_ > 0;
}

// See if we have a view which covers the file starting at START for
// SIZE bytes.  Return a pointer to the View if found, NULL if not.

File_read::View*
File_read::find_view(off_t start, off_t size)
{
  for (std::list<File_read::View*>::iterator p = this->view_list_.begin();
       p != this->view_list_.end();
       ++p)
    {
      if ((*p)->start() <= start
	  && (*p)->start() + (*p)->size() >= start + size)
	return *p;
    }
  return NULL;
}

// Read data from the file.  Return the number of bytes read.  If
// PBYTES is not NULL, store the number of bytes in *PBYTES, otherwise
// require that we read exactly the number of bytes requested.

off_t
File_read::do_read(off_t start, off_t size, void* p, off_t* pbytes)
{
  assert(this->lock_count_ > 0);
  int o = this->descriptor_;

  if (lseek(o, start, SEEK_SET) < 0)
    {
      fprintf(stderr, _("%s: %s: lseek to %lld failed: %s"),
	      program_name, this->filename().c_str(),
	      static_cast<long long>(start),
	      strerror(errno));
      gold_exit(false);
    }

  off_t bytes = ::read(o, p, size);
  if (bytes < 0)
    {
      fprintf(stderr, _("%s: %s: read failed: %s\n"),
	      program_name, this->filename().c_str(), strerror(errno));
      gold_exit(false);
    }

  if (pbytes != NULL)
    *pbytes = bytes;
  else if (bytes != size)
    {
      fprintf(stderr,
	      _("%s: %s: file too short: read only %lld of %lld "
		"bytes at %lld\n"),
	      program_name, this->filename().c_str(),
	      static_cast<long long>(bytes),
	      static_cast<long long>(size),
	      static_cast<long long>(start));
      gold_exit(false);
    }

  return bytes;
}

void
File_read::read(off_t start, off_t size, void* p, off_t* pbytes)
{
  assert(this->lock_count_ > 0);

  File_read::View* pv = this->find_view(start, size);
  if (pv != NULL)
    {
      memcpy(p, pv->data() + (start - pv->start()), size);
      if (pbytes != NULL)
	*pbytes = size;
      return;
    }

  this->do_read(start, size, p, pbytes);
}

// Find an existing view or make a new one.

File_read::View*
File_read::find_or_make_view(off_t start, off_t size, off_t* pbytes)
{
  assert(this->lock_count_ > 0);

  File_read::View* pv = this->find_view(start, size);
  if (pv != NULL)
    return pv;

  unsigned char* p = new unsigned char[size];
  off_t bytes = this->do_read(start, size, p, pbytes);
  pv = new File_read::View(start, bytes, p);
  this->view_list_.push_back(pv);
  return pv;
}

// This implementation of get_view just reads into a memory buffer,
// which we store on view_list_.  At some point we should support
// mmap.

const unsigned char*
File_read::get_view(off_t start, off_t size, off_t* pbytes)
{
  assert(this->lock_count_ > 0);
  File_read::View* pv = this->find_or_make_view(start, size, pbytes);
  return pv->data() + (start - pv->start());
}

File_view*
File_read::get_lasting_view(off_t start, off_t size, off_t* pbytes)
{
  assert(this->lock_count_ > 0);
  File_read::View* pv = this->find_or_make_view(start, size, pbytes);
  pv->lock();
  return new File_view(*this, pv, pv->data() + (start - pv->start()));
}

// Remove all the file views.

void
File_read::clear_views(bool destroying)
{
  std::list<File_read::View*>::iterator p = this->view_list_.begin();
  while (p != this->view_list_.end())
    {
      if ((*p)->is_locked())
	{
	  assert(!destroying);
	  ++p;
	}
      else
	{
	  delete *p;
	  p = this->view_list_.erase(p);
	}
    }
}

// Class File_view.

File_view::~File_view()
{
  assert(this->file_.is_locked());
  this->view_->unlock();
}

// Class Input_file.

void
Input_file::open(const General_options& options, const Dirsearch& dirpath)
{
  std::string name;
  if (!this->input_argument_.is_lib())
    name = this->input_argument_.name();
  else
    {
      std::string n1("lib");
      n1 += this->input_argument_.name();
      std::string n2;
      if (!options.is_static())
	n2 = n1 + ".so";
      n1 += ".a";
      name = dirpath.find(n1, n2);
      if (name.empty())
	{
	  fprintf(stderr, _("%s: cannot find %s\n"), program_name,
		  this->input_argument_.name());
	  gold_exit(false);
	}
    }

  if (!this->file_.open(name))
    {
      fprintf(stderr, _("%s: cannot open %s: %s\n"), program_name,
	      name.c_str(), strerror(errno));
      gold_exit(false);
    }
}

} // End namespace gold.
