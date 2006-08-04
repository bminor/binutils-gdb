// target.h -- target support for gold

// The abstract class Target is the interface for target specific
// support.  It defines abstract methods which each target must
// implement.  Typically there will be one target per processor, but
// in some cases it may be necessary to have subclasses.

// For speed and consistency we want to use inline functions to handle
// relocation processing.  So besides implementations of the abstract
// methods, each target is expected to define a template
// specialization of the relocation functions.

#ifndef GOLD_TARGET_H
#define GOLD_TARGET_H

namespace gold
{

class Target
{
 public:
};

extern Target* select_target(int machine, int size, bool big_endian,
			     int osabi, int abiversion);

} // End namespace gold.

#endif // !defined(GOLD_TARGET_H)
