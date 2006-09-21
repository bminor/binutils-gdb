// fileread.h -- read files for gold   -*- C++ -*-

// Classes used to read data from binary input files.

#ifndef GOLD_FILEREAD_H
#define GOLD_FILEREAD_H

#include <string>
#include <list>

#include "options.h"

namespace gold
{

class Dirsearch;

class File_view;

// File_read manages a file descriptor for a file we are reading.  We
// close file descriptors if we run out of them, so this class reopens
// the file as needed.

class File_read
{
 public:
  File_read()
    : name_(), descriptor_(-1), lock_count_(0)
  { }
  ~File_read();

  // Open a file.
  bool
  open(const std::string& name);

  // Return the file name.
  const std::string&
  filename() const
  { return this->name_; }

  // Return the file descriptor.
  int
  get_descriptor();

  // Lock the file for access within a particular Task::run execution.
  // This means that the descriptor can not be closed.  This routine
  // may only be called from the main thread.
  void
  lock();

  // Unlock the descriptor, permitting it to be closed if necessary.
  void
  unlock();
  
  // Test whether the object is locked.
  bool
  is_locked();

  // Return a view into the file.  The pointer will remain valid until
  // the File_read is unlocked.  If PBYTES is NULL, it is an error if
  // we can not read enough data.  Otherwise *PBYTES is set to the
  // number of bytes read.
  const unsigned char*
  get_view(off_t start, off_t size, off_t *pbytes = NULL);

  // Read data from the file into the buffer P.  PBYTES is as in
  // get_view.
  void
  read(off_t start, off_t size, void* p, off_t *pbytes = NULL);

  // Return a lasting view into the file.  This is allocated with new,
  // and the caller is responsible for deleting it when done.  The
  // data associated with this view will remain valid until the view
  // is deleted.  PBYTES is handled as with get_view.
  File_view*
  get_lasting_view(off_t start, off_t size, off_t *pbytes = NULL);

 private:
  // This class may not be copied.
  File_read(const File_read&);
  File_read& operator=(const File_read&);

  // A view into the file when not using mmap.
  class View
  {
   public:
    View(off_t start, off_t size, unsigned char* data)
      : start_(start), size_(size), data_(data), lock_count_(0)
    { }

    ~View();

    off_t
    start() const
    { return this->start_; }

    off_t
    size() const
    { return this->size_; }

    unsigned char*
    data() const
    { return this->data_; }

    void
    lock();

    void
    unlock();

    bool
    is_locked();

   private:
    View(const View&);
    View& operator=(const View&);

    off_t start_;
    off_t size_;
    unsigned char* data_;
    int lock_count_;
  };

  friend class File_view;

  View*
  find_view(off_t start, off_t size);

  off_t
  do_read(off_t start, off_t size, void* p, off_t* pbytes);

  View*
  find_or_make_view(off_t start, off_t size, off_t* pbytes);

  void
  clear_views(bool);

  std::string name_;
  int descriptor_;
  int lock_count_;
  std::list<View*> view_list_;
};

// A view of file data that persists even when the file is unlocked.
// Callers should destroy these when no longer required.  These are
// obtained form File_read::get_lasting_view.  They may only be
// destroyed when the underlying File_read is locked.

class File_view
{
 public:
  // This may only be called when the underlying File_read is locked.
  ~File_view();

  // Return a pointer to the data associated with this view.
  const unsigned char*
  data() const
  { return this->data_; }

 private:
  File_view(const File_view&);
  File_view& operator=(const File_view&);

  friend class File_read;

  // Callers have to get these via File_read::get_lasting_view.
  File_view(File_read& file, File_read::View* view, const unsigned char* data)
    : file_(file), view_(view), data_(data)
  { }

  File_read& file_;
  File_read::View* view_;
  const unsigned char* data_;
};

// All the information we hold for a single input file.  This can be
// an object file, a shared library, or an archive.

class Input_file
{
 public:
  Input_file(const Input_argument& input_argument)
    : input_argument_(input_argument)
  { }

  void
  open(const General_options&, const Dirsearch&);

  // Return the name given by the user.
  const char*
  name() const
  { return this->input_argument_.name(); }

  // Return the file name.
  const std::string&
  filename() const
  { return this->file_.filename(); }

  File_read&
  file()
  { return this->file_; }

 private:
  const Input_argument& input_argument_;
  File_read file_;
};

} // end namespace gold

#endif // !defined(GOLD_FILEREAD_H)
