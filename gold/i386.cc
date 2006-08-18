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
    : Sized_target<32, false>(false, false)
  { }
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
