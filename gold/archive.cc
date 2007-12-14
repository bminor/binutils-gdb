// archive.cc -- archive support for gold

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

#include <cerrno>
#include <cstring>
#include <climits>
#include <vector>

#include "elfcpp.h"
#include "options.h"
#include "fileread.h"
#include "readsyms.h"
#include "symtab.h"
#include "object.h"
#include "archive.h"

namespace gold
{

// The header of an entry in the archive.  This is all readable text,
// padded with spaces where necesary.  If the contents of an archive
// are all text file, the entire archive is readable.

struct Archive::Archive_header
{
  // The entry name.
  char ar_name[16];
  // The file modification time.
  char ar_date[12];
  // The user's UID in decimal.
  char ar_uid[6];
  // The user's GID in decimal.
  char ar_gid[6];
  // The file mode in octal.
  char ar_mode[8];
  // The file size in decimal.
  char ar_size[10];
  // The final magic code.
  char ar_fmag[2];
};

// Archive methods.

const char Archive::armag[sarmag] =
{
  '!', '<', 'a', 'r', 'c', 'h', '>', '\n'
};

const char Archive::arfmag[2] = { '`', '\n' };

// Set up the archive: read the symbol map and the extended name
// table.

void
Archive::setup(Task* task)
{
  // We need to ignore empty archives.
  if (this->input_file_->file().filesize() == sarmag)
    {
      this->input_file_->file().unlock(task);
      return;
    }

  // The first member of the archive should be the symbol table.
  std::string armap_name;
  off_t armap_size = this->read_header(sarmag, &armap_name);
  off_t off = sarmag;
  if (armap_name.empty())
    {
      this->read_armap(sarmag + sizeof(Archive_header), armap_size);
      off = sarmag + sizeof(Archive_header) + armap_size;
    }
  else if (!this->input_file_->options().include_whole_archive())
    gold_error(_("%s: no archive symbol table (run ranlib)"),
	       this->name().c_str());

  // See if there is an extended name table.
  if ((off & 1) != 0)
    ++off;
  std::string xname;
  off_t extended_size = this->read_header(off, &xname);
  if (xname == "/")
    {
      const unsigned char* p = this->get_view(off + sizeof(Archive_header),
                                              extended_size, false);
      const char* px = reinterpret_cast<const char*>(p);
      this->extended_names_.assign(px, extended_size);
    }

  // Opening the file locked it.  Unlock it now.
  this->input_file_->file().unlock(task);
}

// Read the archive symbol map.

void
Archive::read_armap(off_t start, off_t size)
{
  // Read in the entire armap.
  const unsigned char* p = this->get_view(start, size, false);

  // Numbers in the armap are always big-endian.
  const elfcpp::Elf_Word* pword = reinterpret_cast<const elfcpp::Elf_Word*>(p);
  unsigned int nsyms = elfcpp::Swap<32, true>::readval(pword);
  ++pword;

  // Note that the addition is in units of sizeof(elfcpp::Elf_Word).
  const char* pnames = reinterpret_cast<const char*>(pword + nsyms);
  off_t names_size = reinterpret_cast<const char*>(p) + size - pnames;
  this->armap_names_.assign(pnames, names_size);

  this->armap_.resize(nsyms);

  off_t name_offset = 0;
  for (unsigned int i = 0; i < nsyms; ++i)
    {
      this->armap_[i].name_offset = name_offset;
      this->armap_[i].file_offset = elfcpp::Swap<32, true>::readval(pword);
      name_offset += strlen(pnames + name_offset) + 1;
      ++pword;
    }

  if (reinterpret_cast<const unsigned char*>(pnames) - p > size)
    gold_error(_("%s: bad archive symbol table names"),
	       this->name().c_str());

  // This array keeps track of which symbols are for archive elements
  // which we have already included in the link.
  this->armap_checked_.resize(nsyms);
}

// Read the header of an archive member at OFF.  Fail if something
// goes wrong.  Return the size of the member.  Set *PNAME to the name
// of the member.

off_t
Archive::read_header(off_t off, std::string* pname)
{
  const unsigned char* p = this->get_view(off, sizeof(Archive_header), false);
  const Archive_header* hdr = reinterpret_cast<const Archive_header*>(p);
  return this->interpret_header(hdr, off,  pname);
}

// Interpret the header of HDR, the header of the archive member at
// file offset OFF.  Fail if something goes wrong.  Return the size of
// the member.  Set *PNAME to the name of the member.

off_t
Archive::interpret_header(const Archive_header* hdr, off_t off,
                          std::string* pname)
{
  if (memcmp(hdr->ar_fmag, arfmag, sizeof arfmag) != 0)
    {
      gold_error(_("%s: malformed archive header at %zu"),
		 this->name().c_str(), static_cast<size_t>(off));
      return this->input_file_->file().filesize() - off;
    }

  const int size_string_size = sizeof hdr->ar_size;
  char size_string[size_string_size + 1];
  memcpy(size_string, hdr->ar_size, size_string_size);
  char* ps = size_string + size_string_size;
  while (ps[-1] == ' ')
    --ps;
  *ps = '\0';

  errno = 0;
  char* end;
  off_t member_size = strtol(size_string, &end, 10);
  if (*end != '\0'
      || member_size < 0
      || (member_size == LONG_MAX && errno == ERANGE))
    {
      gold_error(_("%s: malformed archive header size at %zu"),
		 this->name().c_str(), static_cast<size_t>(off));
      return this->input_file_->file().filesize() - off;
    }

  if (hdr->ar_name[0] != '/')
    {
      const char* name_end = strchr(hdr->ar_name, '/');
      if (name_end == NULL
	  || name_end - hdr->ar_name >= static_cast<int>(sizeof hdr->ar_name))
	{
	  gold_error(_("%s: malformed archive header name at %zu"),
		     this->name().c_str(), static_cast<size_t>(off));
	  return this->input_file_->file().filesize() - off;
	}
      pname->assign(hdr->ar_name, name_end - hdr->ar_name);
    }
  else if (hdr->ar_name[1] == ' ')
    {
      // This is the symbol table.
      pname->clear();
    }
  else if (hdr->ar_name[1] == '/')
    {
      // This is the extended name table.
      pname->assign(1, '/');
    }
  else
    {
      errno = 0;
      long x = strtol(hdr->ar_name + 1, &end, 10);
      if (*end != ' '
	  || x < 0
	  || (x == LONG_MAX && errno == ERANGE)
	  || static_cast<size_t>(x) >= this->extended_names_.size())
	{
	  gold_error(_("%s: bad extended name index at %zu"),
		     this->name().c_str(), static_cast<size_t>(off));
	  return this->input_file_->file().filesize() - off;
	}

      const char* name = this->extended_names_.data() + x;
      const char* name_end = strchr(name, '/');
      if (static_cast<size_t>(name_end - name) > this->extended_names_.size()
	  || name_end[1] != '\n')
	{
	  gold_error(_("%s: bad extended name entry at header %zu"),
		     this->name().c_str(), static_cast<size_t>(off));
	  return this->input_file_->file().filesize() - off;
	}
      pname->assign(name, name_end - name);
    }

  return member_size;
}

// Select members from the archive and add them to the link.  We walk
// through the elements in the archive map, and look each one up in
// the symbol table.  If it exists as a strong undefined symbol, we
// pull in the corresponding element.  We have to do this in a loop,
// since pulling in one element may create new undefined symbols which
// may be satisfied by other objects in the archive.

void
Archive::add_symbols(Symbol_table* symtab, Layout* layout,
		     Input_objects* input_objects)
{
  if (this->input_file_->options().include_whole_archive())
    return this->include_all_members(symtab, layout, input_objects);

  const size_t armap_size = this->armap_.size();

  // This is a quick optimization, since we usually see many symbols
  // in a row with the same offset.  last_seen_offset holds the last
  // offset we saw that was present in the seen_offsets_ set.
  off_t last_seen_offset = -1;

  // Track which symbols in the symbol table we've already found to be
  // defined.

  bool added_new_object;
  do
    {
      added_new_object = false;
      for (size_t i = 0; i < armap_size; ++i)
	{
          if (this->armap_checked_[i])
            continue;
	  if (this->armap_[i].file_offset == last_seen_offset)
            {
              this->armap_checked_[i] = true;
              continue;
            }
	  if (this->seen_offsets_.find(this->armap_[i].file_offset)
              != this->seen_offsets_.end())
	    {
              this->armap_checked_[i] = true;
	      last_seen_offset = this->armap_[i].file_offset;
	      continue;
	    }

	  const char* sym_name = (this->armap_names_.data()
				  + this->armap_[i].name_offset);
	  Symbol* sym = symtab->lookup(sym_name);
	  if (sym == NULL)
	    continue;
	  else if (!sym->is_undefined())
	    {
              this->armap_checked_[i] = true;
	      continue;
	    }
	  else if (sym->binding() == elfcpp::STB_WEAK)
	    continue;

	  // We want to include this object in the link.
	  last_seen_offset = this->armap_[i].file_offset;
	  this->seen_offsets_.insert(last_seen_offset);
          this->armap_checked_[i] = true;
	  this->include_member(symtab, layout, input_objects,
			       last_seen_offset);
	  added_new_object = true;
	}
    }
  while (added_new_object);
}

// Include all the archive members in the link.  This is for --whole-archive.

void
Archive::include_all_members(Symbol_table* symtab, Layout* layout,
                             Input_objects* input_objects)
{
  off_t off = sarmag;
  off_t filesize = this->input_file_->file().filesize();
  while (true)
    {
      if (filesize - off < static_cast<off_t>(sizeof(Archive_header)))
        {
          if (filesize != off)
	    gold_error(_("%s: short archive header at %zu"),
		       this->name().c_str(), static_cast<size_t>(off));
          break;
        }

      unsigned char hdr_buf[sizeof(Archive_header)];
      this->input_file_->file().read(off, sizeof(Archive_header), hdr_buf);

      const Archive_header* hdr =
	reinterpret_cast<const Archive_header*>(hdr_buf);
      std::string name;
      off_t size = this->interpret_header(hdr, off, &name);
      if (name.empty())
        {
          // Symbol table.
        }
      else if (name == "/")
        {
          // Extended name table.
        }
      else
        this->include_member(symtab, layout, input_objects, off);

      off += sizeof(Archive_header) + size;
      if ((off & 1) != 0)
        ++off;
    }
}

// Include an archive member in the link.  OFF is the file offset of
// the member header.

void
Archive::include_member(Symbol_table* symtab, Layout* layout,
			Input_objects* input_objects, off_t off)
{
  std::string n;
  this->read_header(off, &n);

  const off_t memoff = off + static_cast<off_t>(sizeof(Archive_header));

  // Read enough of the file to pick up the entire ELF header.
  unsigned char ehdr_buf[elfcpp::Elf_sizes<64>::ehdr_size];

  off_t filesize = this->input_file_->file().filesize();
  int read_size = elfcpp::Elf_sizes<64>::ehdr_size;
  if (filesize - memoff < read_size)
    read_size = filesize - memoff;

  if (read_size < 4)
    {
      gold_error(_("%s: member at %zu is not an ELF object"),
		 this->name().c_str(), static_cast<size_t>(off));
      return;
    }

  this->input_file_->file().read(memoff, read_size, ehdr_buf);

  static unsigned char elfmagic[4] =
    {
      elfcpp::ELFMAG0, elfcpp::ELFMAG1,
      elfcpp::ELFMAG2, elfcpp::ELFMAG3
    };
  if (memcmp(ehdr_buf, elfmagic, 4) != 0)
    {
      gold_error(_("%s: member at %zu is not an ELF object"),
		 this->name().c_str(), static_cast<size_t>(off));
      return;
    }

  Object* obj = make_elf_object((std::string(this->input_file_->filename())
				 + "(" + n + ")"),
				this->input_file_, memoff, ehdr_buf,
				read_size);

  if (input_objects->add_object(obj))
    {
      Read_symbols_data sd;
      obj->read_symbols(&sd);
      obj->layout(symtab, layout, &sd);
      obj->add_symbols(symtab, &sd);
    }
  else
    {
      // FIXME: We need to close the descriptor here.
      delete obj;
    }

}

// Add_archive_symbols methods.

Add_archive_symbols::~Add_archive_symbols()
{
  if (this->this_blocker_ != NULL)
    delete this->this_blocker_;
  // next_blocker_ is deleted by the task associated with the next
  // input file.
}

// Return whether we can add the archive symbols.  We are blocked by
// this_blocker_.  We block next_blocker_.  We also lock the file.

Task_token*
Add_archive_symbols::is_runnable()
{
  if (this->this_blocker_ != NULL && this->this_blocker_->is_blocked())
    return this->this_blocker_;
  return NULL;
}

void
Add_archive_symbols::locks(Task_locker* tl)
{
  tl->add(this, this->next_blocker_);
  tl->add(this, this->archive_->token());
}

void
Add_archive_symbols::run(Workqueue*)
{
  this->archive_->add_symbols(this->symtab_, this->layout_,
			      this->input_objects_);

  this->archive_->release();

  if (this->input_group_ != NULL)
    this->input_group_->add_archive(this->archive_);
  else
    {
      // We no longer need to know about this archive.
      delete this->archive_;
      this->archive_ = NULL;
    }
}

} // End namespace gold.
