// parameters.cc -- general parameters for a link using gold

#include "gold.h"

#include "options.h"
#include "parameters.h"

namespace gold
{

// Initialize the parameters from the options.

Parameters::Parameters(const General_options* options)
  : optimization_level_(options->optimization_level())
{
  if (options->is_shared())
    this->output_file_type_ = OUTPUT_SHARED;
  else if (options->is_relocatable())
    this->output_file_type_ = OUTPUT_OBJECT;
  else
    this->output_file_type_ = OUTPUT_EXECUTABLE;
}

// The global variable.

const Parameters* parameters;

// Initialize the global variable.

void
initialize_parameters(const General_options* options)
{
  parameters = new Parameters(options);
}

} // End namespace gold.
