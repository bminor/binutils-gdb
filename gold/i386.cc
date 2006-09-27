// i386.cc -- i386 target support for gold.

#include "gold.h"
#include "elfcpp.h"
#include "target.h"
#include "target-select.h"

namespace
{

using namespace gold;

// The i386 target class.

class Target_i386 : public Sized_target<32, false>
{
 public:
  Target_i386()
    : Sized_target<32, false>(&i386_info)
  { }

 private:
  static const Target::Target_info i386_info;
};

const Target::Target_info Target_i386::i386_info =
{
  32,		// size
  false,	// is_big_endian
  false,	// has_make_symbol
  false,	// has_resolve,
  0x08048000,	// text_segment_address,
  0x1000,	// abi_pagesize
  0x1000	// common_pagesize
};

// The selector for i386 object files.

class Target_selector_i386 : public Target_selector
{
public:
  Target_selector_i386()
    : Target_selector(elfcpp::EM_386, 32, false)
  { }

  Target*
  recognize(int machine, int osabi, int abiversion) const;
};

// Recognize an i386 object file when we already know that the machine
// number is EM_386.

Target*
Target_selector_i386::recognize(int, int, int) const
{
  return new Target_i386();
}

Target_selector_i386 target_selector_i386;

} // End anonymous namespace.
